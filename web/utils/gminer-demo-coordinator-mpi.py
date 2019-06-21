import os
import sys
import json
import argparse
import os
import subprocess
import io
import time
import numpy as np

from mpi4py import MPI

comm = MPI.COMM_WORLD

my_rank = comm.Get_rank()
comm_sz = comm.Get_size()

if(comm_sz == 1):
    print("run this with mpi.")
    exit(0)

# the master
master_rank = comm_sz - 1

# check environment variables
if not ('GMINER_MERGE_LOG_PATH' in os.environ and 'GMINER_LOG_PATH' in os.environ):
    print('environment variable GMINER_MERGE_LOG_PATH and GMINER_LOG_PATH is needed')
    exit(1)

parser = argparse.ArgumentParser()

parser.add_argument('-t', '-timestamp', '--timestamp', help = 'the timestamp when GMiner application was launched', required = True)
parser.add_argument('-i', '-interval', '--interval', default='1.0')

args = vars(parser.parse_args())

def run_bg_cmd(command):
    proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    proc.wait() #this works
    lns_content = []
    for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
        if(line[-1] == '\n'):
            lns_content.append(line[:-1])
        else:
            lns_content.append(line)
    return lns_content

def TanhLikeMapping(x, f):
    return 1.0 -  1.0 / (np.log(x / f + 1) * np.log(x / f + 1) + 1)

def TaskCountToBarLengthMapping(x):
    #x: any numerical value
    if(x <= 0.0):
        return 0.0
    if(not x >= 0.0):
        return 0.0
    return 0.2 * TanhLikeMapping(x, 1) + 0.175 * TanhLikeMapping(x, 10) + 0.175 * TanhLikeMapping(x, 100) + 0.175 * TanhLikeMapping(x, 1000) + 0.175 * TanhLikeMapping(x, 10000) + 0.1 * TanhLikeMapping(x, 100000)

def PrintWithFlush(*args):
    print(*args)
    sys.stdout.flush()

def GraphMatchingPostProcess(dic):
    if ('subg' in dic):
        dic.pop('subg')

def TriangleCountingPostProcess(dic):
    if ('subg' in dic):
        dic.pop('subg')

class Master:
    # 
    def __init__(self, gminer_timestamp):
        self.gminer_timestamp = gminer_timestamp

        self.merged_path = '{}/{}/'.format(os.environ['GMINER_MERGE_LOG_PATH'], self.gminer_timestamp)
        self.log_path = '{}/{}/'.format(os.environ['GMINER_LOG_PATH'], self.gminer_timestamp)

        PrintWithFlush('master, merged_path = {}, log_path = {}'.format(self.merged_path, self.log_path))

        self.signal_file_name = '{}/signal-file-gminer.{}'.format(self.log_path, args['timestamp'])

        os.system('mkdir -p {}'.format(self.merged_path))

        # the input path
        self.queue_file_path = '{}/master_5q.log'.format(self.log_path)
        self.resume_result_path = '{}/resume_result.json'.format(self.merged_path)
        self.sys_sync_signal_path = '{}/start-sys-sync.log'.format(self.log_path)

        # the output path for different files
        self.slaves_final_path = '{}/slaves.json'.format(self.merged_path)
        self.slaves_writting_path = '{}/slaves_writting.json'.format(self.merged_path)
        self.slaves_append_path = '{}/slaves_append.json'.format(self.merged_path)
        self.queue_final_path = '{}/master_5q.json'.format(self.merged_path)
        self.queue_writting_path = '{}/master_writting_5q.json'.format(self.merged_path)
        self.queue_append_path = '{}/master_5q_append.json'.format(self.merged_path)
        self.hdfs_loaded_signal_path = '{}/hdfs_loaded.log'.format(self.merged_path)

        # queues related
        self.queue_dic_list = []
        self.aggstr_dic_list = []

        # app output related
        self.results_from_workers = []
        self.results_displayed = []

        self.GenerateDefaultDic()

        self.regular_loop_count = 0
        self.sys_sync_signal_detected = 0

    def RegularProcess(self):
        if (self.sys_sync_signal_detected == 0):
            if (os.path.isfile(self.sys_sync_signal_path)):
                self.sys_sync_signal_detected = 1
                os.system('touch {}'.format(self.hdfs_loaded_signal_path))

        self.regular_loop_count += 1

    def WaitForSignalFile(self):
        #
        PrintWithFlush('waiting for signal file {}'.format(self.signal_file_name))
        while True:
            run_bg_cmd("ls -al .")
            if (os.path.isfile(self.signal_file_name)):
                break
            time.sleep(0.05)

        f = open(self.signal_file_name, 'r')
        ln = f.readline()
        f.close()
        signal_dic = json.loads(ln)

        self.signal_dic = signal_dic

        return signal_dic

    def CheckAppFinished(self):
        if (not os.path.isfile(self.signal_file_name)):
            return True
        if (os.path.isfile(self.resume_result_path)):
            return True

        return False

    def WriteInitJsonFile(self):
        # write queue files
        with open(self.queue_writting_path, 'w') as f:
            f.write(json.dumps(self.queue_dic_default) + '\n')
            f.close()

    def ReadQueueInfo(self):
        if (not os.path.isfile(self.queue_file_path)):
            return self.queue_dic_default

        with open(self.queue_file_path, 'r') as f:
            lines = f.readlines()
            line_id_to_parse = len(self.queue_dic_list)

            while True:
                if (line_id_to_parse == len(lines)):
                    break

                line = lines[line_id_to_parse]

                try:
                    ln_dic = json.loads(line)
                    avail = True
                except Exception:
                    avail = False

                if (not avail):
                    break

                final_queue_dic = {}

                final_queue_dic['task_num_in_memory'] = 0
                final_queue_dic['task_num_in_disk'] = 0
                final_queue_dic['cmq_size'] = 0
                final_queue_dic['cpq_size'] = 0
                final_queue_dic['taskbuf_size'] = 0
                final_queue_dic['task_store_to_cmq'] = 0
                final_queue_dic['cmq_to_cpq'] = 0
                final_queue_dic['cpq_to_task_store'] = 0
                final_queue_dic['cpq_finished'] = 0
            
                for i in range(ln_dic['nodes']):
                    stri = str(i)
                    final_queue_dic['task_num_in_memory'] += ln_dic[stri][0]
                    final_queue_dic['task_num_in_disk'] += ln_dic[stri][1]
                    final_queue_dic['cmq_size'] += ln_dic[stri][2]
                    final_queue_dic['cpq_size'] += ln_dic[stri][3]
                    final_queue_dic['taskbuf_size'] += ln_dic[stri][4]
                    final_queue_dic['task_store_to_cmq'] += ln_dic[stri][5]
                    final_queue_dic['cmq_to_cpq'] += ln_dic[stri][6]
                    final_queue_dic['cpq_to_task_store'] += ln_dic[stri][7]
                    final_queue_dic['cpq_finished'] += ln_dic[stri][8]

                final_queue_dic['task_transfer_1'] = final_queue_dic['task_store_to_cmq']
                final_queue_dic['task_transfer_2'] = final_queue_dic['cmq_to_cpq']
                final_queue_dic['task_transfer_3'] = final_queue_dic['cpq_to_task_store']
                final_queue_dic['task_transfer_4'] = final_queue_dic['cpq_finished']

                final_queue_dic['task_num_in_memory_float'] = TaskCountToBarLengthMapping(final_queue_dic['task_num_in_memory'])
                final_queue_dic['task_num_in_disk_float'] = TaskCountToBarLengthMapping(final_queue_dic['task_num_in_disk'])
                final_queue_dic['cmq_size_float'] = TaskCountToBarLengthMapping(final_queue_dic['cmq_size'])
                final_queue_dic['cpq_size_float'] = TaskCountToBarLengthMapping(final_queue_dic['cpq_size'])
                final_queue_dic['taskbuf_size_float'] = TaskCountToBarLengthMapping(final_queue_dic['taskbuf_size'])

                self.queue_dic_list.append(final_queue_dic)
                if ('agg_str' in ln_dic):
                    self.aggstr_dic_list.append(ln_dic['agg_str'])

                line_id_to_parse += 1

    def GetQueueDic(self):
        if (len(self.queue_dic_list) == 0):
            return self.queue_dic_default
        return self.queue_dic_list[-1]

    def AppendTaskResultDicList(self, dic_list):
        for dic in dic_list:
            if (not len(dic) == 0):
                # PrintWithFlush('AppendTaskResultDicList dic["seed_id"] = {}'.format(dic["seed_id"]))
                self.results_from_workers.append(dic)

    def GetTaskResultDic(self):
        # for normal tasks
        if (len(self.results_from_workers) == 0):
            if (len(self.results_displayed) == 0):
                task_result_dic = {}
            else:
                task_result_dic = self.results_displayed.pop()
                self.results_displayed.insert(0, task_result_dic)
        else:
            task_result_dic = self.results_from_workers.pop()
            self.results_displayed.append(task_result_dic)

        # for mc tasks
        if (len(self.aggstr_dic_list) and self.signal_dic['app_name'] == 'MC'):
            if (len(self.aggstr_dic_list) > 0):
                task_result_dic = self.aggstr_dic_list[-1].copy()
                task_result_dic['size'] = task_result_dic.pop('mc')
                task_result_dic['mc'] = []
                for i in range(task_result_dic['count']):
                    stri = str(i)
                    task_result_dic['mc'].append(task_result_dic.pop(stri))

        return task_result_dic

    def WriteFrontendFiles(self):
        # write normal task file
        task_result_dic = self.GetTaskResultDic()
        # PrintWithFlush('to write file {}'.format(self.slaves_writting_path))
        if (len(task_result_dic) != 0):
            # PrintWithFlush('WriteFrontendFiles task_result_dic["seed_id"] = {}'.format(task_result_dic["seed_id"]))
            with open(self.slaves_writting_path, 'w') as f:
                PrintWithFlush('written file {}'.format(self.slaves_writting_path))
                f.write(json.dumps(task_result_dic) + '\n')
                f.close()
                append_f = open(self.slaves_append_path, 'a')
                append_f.write(json.dumps(task_result_dic) + '\n')
                append_f.close()

            os.system("mv {} {}".format(self.slaves_writting_path, self.slaves_final_path))

        # write queue file
        queue_dic = self.GetQueueDic()
        # PrintWithFlush('to write file {}'.format(self.queue_writting_path))
        with open(self.queue_writting_path, 'w') as f:
            PrintWithFlush('written file {}'.format(self.queue_writting_path))
            f.write('{}\n'.format(json.dumps(queue_dic)))
            f.close()
            append_f = open(self.queue_append_path, 'a')
            append_f.write('{}\n'.format(json.dumps(queue_dic)))
            append_f.close()

        os.system("mv {} {}".format(self.queue_writting_path, self.queue_final_path))


    def WriteFinalizeFile(self):
        queue_dic_list = []
        self.WriteFrontendFiles()

    def GenerateDefaultDic(self):
        # avoid frontend display error due to latency
        self.queue_dic_default  = {}
        self.queue_dic_default['task_num_in_memory'] = 0
        self.queue_dic_default['task_num_in_disk'] = 0
        self.queue_dic_default['cmq_size'] = 0
        self.queue_dic_default['cpq_size'] = 0
        self.queue_dic_default['taskbuf_size'] = 0
        self.queue_dic_default['task_num_in_memory_float'] = 0.0
        self.queue_dic_default['task_num_in_disk_float'] = 0.0
        self.queue_dic_default['cmq_size_float'] = 0.0
        self.queue_dic_default['cpq_size_float'] = 0.0
        self.queue_dic_default['taskbuf_size_float'] = 0.0
        self.queue_dic_default['task_store_to_cmq'] = 0
        self.queue_dic_default['cmq_to_cpq'] = 0
        self.queue_dic_default['cpq_to_task_store'] = 0
        self.queue_dic_default['cpq_finished'] = 0
        self.queue_dic_default['task_transfer_1'] = 0
        self.queue_dic_default['task_transfer_2'] = 0
        self.queue_dic_default['task_transfer_3'] = 0
        self.queue_dic_default['task_transfer_4'] = 0

        # self.app_output_default_dic = {}


class Slave:
    #
    def __init__(self, gminer_timestamp):
        self.gminer_timestamp = gminer_timestamp

        self.log_path = '{}/{}/'.format(os.environ['GMINER_LOG_PATH'], self.gminer_timestamp)

        PrintWithFlush('slave {}, log_path = {}'.format(my_rank, self.log_path))

        self.output_slice = 0

        self.log_dic_list = []

        # post process data from GMiner
        self.post_process_func = {}
        self.post_process_func['GM'] = GraphMatchingPostProcess
        self.post_process_func['TC'] = TriangleCountingPostProcess


    def ReadLog(self):
        # return a single task to be sent to the master
        # before returning the dic, read logs
        self.ReadSlice(self.output_slice)

        # check if finish flag exists
        finish_flag_filename = '{}/demo_node_{}_part_{}_finish.log'.format(self.log_path, my_rank, self.output_slice)
        if (os.path.isfile(finish_flag_filename)):
            self.output_slice += 1
            if (my_rank == 0):
                PrintWithFlush('self.output_slice += 1, {}'.format(self.output_slice))
            for tid in range(signal_dic['nthreads']):
                self.log_line_count[tid] = 0

    def GetTaskOutputDic(self):
        if (len(self.log_dic_list) == 0):
            return {}
        else:
            return self.log_dic_list.pop()

    def ReadSlice(self, cur_slice):
        for tid in range(self.signal_dic['nthreads']):
            filename = '{}/demo_node_{}_thread_{}_part_{}.log'.format(self.log_path, my_rank, tid, cur_slice)
            if (not os.path.isfile(filename)):
                break

            with open(filename, 'r') as f:
                # some waste here
                lines = f.readlines()
                line_id_to_parse = self.log_line_count[tid]
                while True:
                    if (line_id_to_parse == len(lines)):
                        break

                    # read lines, till the end
                    line = lines[line_id_to_parse]

                    try:
                        ln_dic = json.loads(line)
                        avail = True
                    except Exception:
                        avail = False

                    if (not avail):
                        break

                    # check if need to do Post-process
                    if (self.signal_dic['app_name'] in self.post_process_func):
                        self.post_process_func[self.signal_dic['app_name']](ln_dic)

                    # append the parsed dic
                    self.log_dic_list.append(ln_dic)

                    line_id_to_parse += 1
                    self.log_line_count[tid] = line_id_to_parse

    def SetSignalDic(self, signal_dic):
        self.signal_dic = signal_dic
        self.log_line_count = []  # to avoid duplicate read

        for tid in range(signal_dic['nthreads']):
            self.log_line_count.append(0)


if __name__ == "__main__":
    signal_dic = {}
    gminer_timestamp = args['timestamp']
    sleep_interval = float(args['interval'])

    if (my_rank == master_rank):
        # master
        me = Master(gminer_timestamp)
        me.WriteFrontendFiles()
        PrintWithFlush('me.WriteFrontendFiles()')
        signal_dic = me.WaitForSignalFile()
        PrintWithFlush('signal: ', signal_dic)
        comm.Barrier()
        signal_dic = comm.bcast(signal_dic, root = master_rank)

        # wait until the signal file appears
        while True:
            # comm.gather()
            app_dic_list = comm.gather({}, root = master_rank)
            me.RegularProcess()
            me.AppendTaskResultDicList(app_dic_list)
            me.ReadQueueInfo()
            me.WriteFrontendFiles()

            comm.Barrier()

            app_finished = me.CheckAppFinished()
            app_finished = comm.bcast(app_finished, root = master_rank)

            if (app_finished):
                break
            time.sleep(sleep_interval)

        me.WriteFinalizeFile()

    else:
        # slaves
        me = Slave(gminer_timestamp)
        comm.Barrier()
        signal_dic = comm.bcast(signal_dic, root = master_rank)
        me.SetSignalDic(signal_dic)

        while (True):
            # if no output, dic_to_send = {}
            me.ReadLog()
            dic_to_send = me.GetTaskOutputDic()
            comm.gather(dic_to_send, root = master_rank)
            
            comm.Barrier()

            app_finished = True
            app_finished = comm.bcast(app_finished, root = master_rank)

            if (app_finished):
                break
            time.sleep(sleep_interval)
