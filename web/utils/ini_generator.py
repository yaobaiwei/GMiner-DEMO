import os
import sys
import json
import argparse
import subprocess
import io
import time

gminer_root = os.environ['GMINER_HOME']
input_path = {}
input_path['skitter'] = {}
input_path['youtube'] = {}
input_path['orkut'] = {}
input_path['friendster'] = {}
input_path['tencent'] = {}
input_path['dblp'] = {}

input_path['youtube']['tc'] = os.environ['YOUTUBE_PLAIN_GRAPH']
input_path['skitter']['tc'] = os.environ['SKITTER_PLAIN_GRAPH']
input_path['orkut']['tc'] = os.environ['ORKUT_PLAIN_GRAPH']
input_path['friendster']['tc'] = os.environ['FRIENDSTER_PLAIN_GRAPH']

input_path['youtube']['mc'] = os.environ['YOUTUBE_PLAIN_GRAPH']
input_path['skitter']['mc'] = os.environ['SKITTER_PLAIN_GRAPH']
input_path['orkut']['mc'] = os.environ['ORKUT_PLAIN_GRAPH']
input_path['friendster']['mc'] = os.environ['FRIENDSTER_PLAIN_GRAPH']

input_path['youtube']['gm'] = os.environ['YOUTUBE_LABELED_GRAPH']
input_path['skitter']['gm'] = os.environ['SKITTER_LABELED_GRAPH']
input_path['orkut']['gm'] = os.environ['ORKUT_LABELED_GRAPH']
input_path['friendster']['gm'] = os.environ['FRIENDSTER_LABELED_GRAPH']

input_path['youtube']['cd'] = os.environ['YOUTUBE_ATTR_GRAPH']
input_path['skitter']['cd'] = os.environ['SKITTER_ATTR_GRAPH']
input_path['orkut']['cd'] = os.environ['ORKUT_ATTR_GRAPH']
input_path['tencent']['cd'] = os.environ['TENCENT_ATTR_GRAPH']

input_path['youtube']['fco'] = os.environ['YOUTUBE_FCO_GRAPH']
input_path['skitter']['fco'] = os.environ['SKITTER_FCO_GRAPH']
input_path['orkut']['fco'] = os.environ['ORKUT_FCO_GRAPH']
input_path['dblp']['fco'] = os.environ['DBLP_FCO_GRAPH']

threshold_sampling_dic = {'tc':{}, 'gm':{}, 'cd':{}, 'fco':{}}
threshold_sampling_dic['tc']['tc-sampling-min'] = 'TC_SAMPLING_MIN'
threshold_sampling_dic['tc']['tc-sampling-max'] = 'TC_SAMPLING_MAX'
threshold_sampling_dic['gm']['gm-sampling-min'] = 'GM_SAMPLING_MIN'
threshold_sampling_dic['gm']['gm-sampling-max'] = 'GM_SAMPLING_MAX'
threshold_sampling_dic['cd']['cd-sampling-min'] = 'CD_SAMPLING_MIN'
threshold_sampling_dic['cd']['cd-sampling-max'] = 'CD_SAMPLING_MAX'
threshold_sampling_dic['fco']['gc-sampling-min'] = 'GC_SAMPLING_MIN'
threshold_sampling_dic['fco']['gc-sampling-max'] = 'GC_SAMPLING_MAX'

machine_file = {False : 'machines.cfg', 'ib' : 'ib_machines.cfg'}
# machine_file = {False : os.path.join(gminer_root, 'machines.cfg'), 'ib' : 'ib_machines.cfg'}

def gminer_ini_gen(param_dic):

    ini_str = """#Copyright 2018 Husky Data Lab, CUHK
#Authors: Hongzhi Chen, Miao Liu


#ini file for example
#new line to end this ini

[PATH]
;for application I/O and local temporary storage
HDFS_HOST_ADDRESS = master
HDFS_PORT = 9000
HDFS_INPUT_PATH = {}
HDFS_OUTPUT_PATH = /gminer_default_output/  ;just for partition, will not be used in this demo
LOCAL_TEMP_PATH = /tmp/{}-gminer-dump
FORCE_WRITE = TRUE    ;force to write HDFS_OUTPUT_PATH

[COMPUTING]
;for task computing configurations
CACHE_SIZE = {}  ;the size of cachetable in each worker
NUM_COMP_THREAD = {}  ;number of threads in threadpool for task computation
PIPE_POP_NUM = {}  ;number of tasks popped out each batch in the pipeline

[STEALING]
;for task stealing configurations
POP_NUM = {}  ;number of tasks for pque pops tasks to remote worker during one stealing procedure
SUBG_SIZE_T = {}  ;threshold that task can be moved to other workers only if its current subgraph size <= SUBG_SIZE_T
LOCAL_RATE = 0.5  ;threshold that task can be moved to other workers only if its current local rate <= LOCAL_RATE

[SYNC]
;for context and aggregator sync
AGG_SLEEP_TIME = 0        ;unit:second; do context and aggregator sync periodically during computation; if AGG_SLEEP_TIME == 0, then no sync happens during computation
SYS_SLEEP_TIME = 1        ;unit:second; ; do system sync

""".format(input_path[param_dic['dataset']][param_dic['apps']], os.environ['USER'], str(param_dic['cache-size']), 
           str(param_dic['num-comp-thread']), str(param_dic['pipe-pop-num']), 
           str(param_dic['pop-num']), str(param_dic['subg-size-t']))

    if(param_dic['apps'] == 'tc'):
        app_cmd = 'tc'

    if(param_dic['apps'] == 'mc'):
        app_cmd = 'mc '

    if(param_dic['apps'] == 'gm'):
        app_cmd = 'gm '

    if(param_dic['apps'] == 'cd'):
        app_cmd = 'cd {}'.format(str(param_dic['k-threshold']))

    if(param_dic['apps'] == 'fco'):
        app_cmd = 'fco {} {} {} {} {} {} '.format(str(param_dic['min-weight']), str(param_dic['min-core-size']), str(param_dic['min-result-size']),
                                                 str(param_dic['diff-ratio']), str(param_dic['iter-round-max']), str(param_dic['cand-max-time']))



    threshold_str = ''
    #### threshold for sampling
    if (param_dic['apps'] in threshold_sampling_dic):
        for env_key in threshold_sampling_dic[param_dic['apps']]:
            threshold_str = '{} {}={}'.format(threshold_str, threshold_sampling_dic[param_dic['apps']][env_key], param_dic[env_key])

    #### gen cmd
    num_worker = os.environ['NUM_WORKER']
    cmd = "{} mpiexec -n {} -f {} $GMINER_HOME/release/{}".format(threshold_str, num_worker, machine_file[param_dic['ib']], app_cmd)

    return [cmd, ini_str]
