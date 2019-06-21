import flask
from flask import request
import json
import subprocess
import time, os
import signal

from gminer_infos import *
import gminer_infos
import utils.ini_generator

app = flask.Flask(__name__)

# app.logger.setLevel(40)
import logging
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

app_table = {}
coordinator_table = {}
paused_key_set = set()
finished_key_set = set()

merger_log_path = os.environ['GMINER_MERGE_LOG_PATH']
worker_log_path = os.environ['GMINER_LOG_PATH']

dev_debug = False

def discardByKey(key):
    # del app_table[key]
    coordinator_table[key].kill()
    paused_key_set.discard(key)

def get_timestamp():
    t = time.time()
    t = int(t * 1000 + 0.5)
    return t

def correctSubgList(graph_json):
    help_set = set();
    for src, dst in graph_json["conn_list"]:
        help_set.add(src)
        help_set.add(dst)
    graph_json["subg_list"] = list(help_set)
    graph_json["subg_size"] = len(help_set)
    return graph_json

@app.route('/')
def main():
    persons = gminer_infos.gminer_persons
    persons = [persons[i * 2:i * 2 + 2] for i in range(7//2 + 1)]
    supervisors = gminer_infos.gminer_supervisors
    supervisors = [supervisors[i * 2:i * 2 + 2] for i in range(7//2 + 1)]
    return flask.render_template('index.html', apps=gminer_apps,
            supervisors = supervisors,
            datasets=gminer_datasets,
            sysconfigs0=gminer_sysconfig[:3],
            sysconfigs1=gminer_sysconfig[3:], 
            slideimages = gminer_infos.gminer_compare, 
            teammembers = persons, 
            codes = gminer_infos.gminer_codes)

@app.route('/load_json/<folder>/<path>')
def return_cpu_info(folder, path):
    path = os.path.join(folder, path)
    try:
        with open(path) as f:
            res = json.load(f)
    except Exception:
        res = {}

    resp = flask.Response(json.dumps(res), mimetype='application/json')
    return resp

@app.route('/runrequest', methods=['POST'])
def runApplication():
    timestamp = get_timestamp()
    myenv = os.environ.copy()
    # try:
    data = json.loads(request.data)
    print(data)
    # 1. run python coordinator
    merger_log_file = open('{}/merger_{}.log'.format(merger_log_path, str(timestamp)), 'w')
    num_worker = os.environ['NUM_WORKER']
    merger_cmd = 'mpiexec -n {} -f machines.cfg python utils/gminer-demo-coordinator-mpi.py -t {}'.format(num_worker, timestamp)
    print('merger_cmd = {}'.format(merger_cmd))
    proc = subprocess.Popen(merger_cmd, shell=True, stdout=merger_log_file)
    coordinator_table[timestamp] = proc
    # 2. run gminer
    cmd, ini_str = utils.ini_generator.gminer_ini_gen(data)
    tmpf_dir = os.path.join(myenv['GMINER_HOME'], 'tmp')
    if not os.path.exists(tmpf_dir):
        os.mkdir(tmpf_dir)
    myenv['GMINER_INI_NAME'] = os.path.join(myenv['GMINER_HOME'], 'tmp', str(timestamp)+'.ini')

    myenv['GMINER_START_TIMESTAMP'] = timestamp
    with open(myenv['GMINER_INI_NAME'], 'w') as f:
        f.write(ini_str)

    print('run command: ', cmd)
    final_cmd = 'GMINER_INI_NAME={} GMINER_START_TIMESTAMP={} {}'.\
            format(myenv['GMINER_INI_NAME'], myenv['GMINER_START_TIMESTAMP'], cmd)

    log_file = open('{}/{}.log'.format(merger_log_path, str(timestamp)), 'w')
    proc = subprocess.Popen(final_cmd, shell=True, stdout=log_file)
    app_table[timestamp] = proc

    data.update({'key': timestamp, 'status': "ok"})

    resp = flask.Response(json.dumps(data), mimetype='application/json')
    return resp

@app.route('/stoprequest', methods=['POST'])
def kill_by_timestamp():
    data = json.loads(request.data)
    key = data['key']
    app_table[key].kill()
    coordinator_table[key].kill()
    # os.kill(app_table[key].pid, 15)
    # os.kill(coordinator_table[key].pid, 15)
    # del app_table[key]
    
    data = {'key': key, 'status': "stop"}
    resp = flask.Response(json.dumps(data), mimetype='application/json')
    return resp

@app.route('/pauserequest', methods=['POST'])
def pause_by_timestamp():
    data = json.loads(request.data)
    key = data['key']
    print('[pauserequest] key: ',key)
    paused_key_set.add(key)
    # app_table[key].send_signal(signal.SIGSTOP)
    
    data = {'key': key, 'status': "pause"}
    resp = flask.Response(json.dumps(data), mimetype='application/json')

    pause_signal_filename = 'runtime-infos/{}/pause_signal.txt'.format(key)
    os.system('touch {}'.format(pause_signal_filename))

    return resp

@app.route('/resumerequest', methods=['POST'])
def resume_by_timestamp():
    data = json.loads(request.data)
    key = data['key']

    pause_signal_filename = 'runtime-infos/{}/pause_signal.txt'.format(key)
    os.system('rm -f {}'.format(pause_signal_filename))

    # write seed and edges and nodes to a file
    # record this seed and control it when interact
    if data['seed_id'] == -1:
        print("resume_by_timestamp, data['seed_id'] == -1")
        paused_key_set.discard(key)
        # app_table[key].send_signal(signal.SIGCONT)
        data = {'key': key, 'status': "ok"}
    elif key in finished_key_set:
        finished_key_set.discard(key)
        # app_table[key].send_signal(signal.SIGCONT)
        data = {"key": key, 'status': 'finished'}
    else:
        with open('runtime-infos/{}/resume_file.txt'.format(key),'w') as f:
            print("resume requested: {}".format(data))
            f.write(str(data['seed_id']))
            if data.get('removed_nodes'):
                f.write('\n')
                for node in data['removed_nodes']:
                    f.write(str(node)+' ')
            f.write('-1')
            if data.get('removed_edges'):
                f.write('\n')
                for src, dst in data['removed_edges']:
                    f.write(str(src) + ' ' + str(dst) + '\n')
            f.write('-1')
        # app_table[key].send_signal(signal.SIGCONT)
        data = {'key': key, 'status': "ok"}

    resp = flask.Response(json.dumps(data), mimetype='application/json')
    return resp

last_sub_graph = ""
@app.route('/interaction', methods=['POST'])
def send_infos():
    data = json.loads(request.data)
    if (dev_debug):
        print('recv interaction ',data)
    key = data["key"]
    res = {}
    fname = os.path.join('runtime-infos', '{}.log'.format(key))
    stdpt = int(data['stdpt'])
    # 1. read stdout file
    with open(fname) as f:
        f.seek(stdpt)
        res['text'] = f.read()
        stdpt = f.tell()
        res['stdpt'] = stdpt
    res['text'] = res['text'].replace('\n', '<br>')

    # 2. read queue data
    que = 'runtime-infos/{}/master_5q.json'.format(key)
    try:
        with open(que) as f:
            que = json.load(f)
            res.update(que)
    except Exception:
        q_dic = {}
        q_dic['task_num_in_memory'] = 0
        q_dic['task_num_in_disk'] = 0
        q_dic['cmq_size'] = 0
        q_dic['cpq_size'] = 0
        q_dic['taskbuf_size'] = 0

        q_dic['task_num_in_memory_float'] = 0.0
        q_dic['task_num_in_disk_float'] = 0.0
        q_dic['cmq_size_float'] = 0.0
        q_dic['cpq_size_float'] = 0.0
        q_dic['taskbuf_size_float'] = 0.0

        q_dic['task_store_to_cmq'] = 0
        q_dic['cmq_to_cpq'] = 0
        q_dic['cpq_to_task_store'] = 0
        q_dic['cpq_finished'] = 0

        q_dic['task_transfer_1'] = 0
        q_dic['task_transfer_2'] = 0
        q_dic['task_transfer_3'] = 0
        q_dic['task_transfer_4'] = 0

        res.update(q_dic)

    # 3. read system info

    # 4. read graph info
    resume_finished = False
    if key in paused_key_set:
        fname = 'runtime-infos/{}/resume_result.json'.format(key)
        resume_finished = True
    else:
        fname = 'runtime-infos/{}/slaves.json'.format(key)

    if os.path.exists(fname):
        try:
            with open(fname) as f:
                try:
                    graph = json.load(f)
                    res['taskRes'] = graph
                    if key in paused_key_set and 'status' not in graph:
                        res['taskRes']['status'] = "resume"
                        # res["taskRes"] = correctSubgList(graph)
                except json.decoder.JSONDecodeError:
                    res['taskRes'] = ""
                if (resume_finished):
                    app_table[key].kill()
                    # os.kill(app_table[key].pid, 15)
        except Exception:
            res['taskRes'] = ""
    else:
        res['taskRes']=""

    if (os.path.isfile('runtime-infos/{}/hdfs_loaded.log'.format(key))):
        res['hdfsLoaded'] = True
    else:
        res['hdfsLoaded'] = False

    global last_sub_graph
    if res['taskRes'] == last_sub_graph:
        res['taskRes'] = ""
    else:
        last_sub_graph = res['taskRes']
    # 5. if end
    res['end'] = 0 if app_table[int(key)].poll() is None else 1
    if res['end'] == 1:
        discardByKey(key)
    respon = flask.Response(json.dumps(res), mimetype='application/json')
    res['text'] = 'deleted'
    if (dev_debug):
        print('interaction info:', res)
    return respon

