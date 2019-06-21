import os
import sys
import json
import argparse
import subprocess
import io
import time
import psutil

from mpi4py import MPI

comm = MPI.COMM_WORLD

my_rank = comm.Get_rank()
comm_sz = comm.Get_size()

if(comm_sz == 1):
    print("run this with mpi.")
    exit(0)

master_rank = comm_sz - 1

is_master = (my_rank == master_rank)

parser = argparse.ArgumentParser()

parser.add_argument('-i', '-interval', '--interval', default='1.0')
parser.add_argument('-m', '-max_second', '--max_second', default='120.0')
parser.add_argument('-d', '-localdir', '--localdir', required=True)
parser.add_argument('-nd', '-network_device', '--network_device', required=True)
parser.add_argument('-nt', '-network_max_throughput', '--network_max_throughput', required=True)
parser.add_argument('-dt', '-disk_max_throughput', '--disk_max_throughput', required=True)

args = vars(parser.parse_args())

space_strs = []
for i in range(1000):
    space_strs.append("")
    for j in range(i):
        space_strs[i] = space_strs[i] + " "

def get_timestamp():
    t = time.time()
    t = int(t * 1000 + 0.5);
    return t

def run_bg_cmd(command):
    proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    proc.wait()
    lns_content = []
    for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
        if(line[-1] == '\n'):
            lns_content.append(line[:-1])
        else:
            lns_content.append(line)
    return lns_content


def submit_bg_cmd(command):
    proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    return proc

def test_wait_bg_cmd(proc):
    if(proc.poll() == None):
        return {'ok' : False}

    proc.wait()

    lns_content = []
    for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
        if(line[-1] == '\n'):
            lns_content.append(line[:-1])
        else:
            lns_content.append(line)

    return {'ok' : True, 'out' : lns_content}


def get_hostname():
    return run_bg_cmd('hostname')[0]


def get_mem_swap_sz():
    ret_lns = run_bg_cmd('free -m')
    for ln in ret_lns:
        #if Swap
        if(ln[:3] == 'Swa'):
            swap = int(ln.split()[1])

        #if Mem
        if(ln[:3] == 'Mem'):
            mem = int(ln.split()[1])

    return mem, swap


def get_cur_used_mem():
    # output type 1
#                  total        used        free      shared  buff/cache   available
#Mem:        1031824       18753      659891        7433      353179     1003603
#Swap:         20479          21       20458
    
    # output type 2
#                 total       used       free     shared    buffers     cached
#Mem:         64183      13988      50195          0        476      10978
#-/+ buffers/cache:       2533      61649
#Swap:        20479        121      20358
    ret_lns = run_bg_cmd('free -m')

    pos_avail = ret_lns[0].find('avail')
    mem_ln_sp = ret_lns[1].split()

    if(pos_avail == -1):
        #condition2
        mem_used = int(mem_ln_sp[2]) - int(mem_ln_sp[5]) - int(mem_ln_sp[6])
    else:
        mem_used = int(mem_ln_sp[2])

    return mem_used


get_cpu_average_usage_async_p_list = []
last_cpu_usage = 0.0


def get_cpu_average_usage_async(sleep_interval):
    global get_cpu_average_usage_async_p_list
    global last_cpu_usage

    to_submit = True
    if(len(get_cpu_average_usage_async_p_list) * sleep_interval > 1.0):
        to_submit = False
    # if(len(get_cpu_average_usage_async_p_list) >= 4):
    #     to_submit = False

    if(to_submit):
        get_cpu_average_usage_async_p_list.insert(0, submit_bg_cmd('mpstat 1 1'))

    #check which column %idle is in, and where the 'average' is start
    test_dic = test_wait_bg_cmd(get_cpu_average_usage_async_p_list[-1])
    if(test_dic['ok']):
        get_cpu_average_usage_async_p_list.pop()
        av_ln_idx = -1
        av_hit = False
        usage_list = []
        for ln in test_dic['out']:
            av_ln_idx += 1
            
            if(ln.find('%idle') >= 0):
                av_hit = True
                # break
                #do not break; parse where '%idle' is
                ln_sp = ln.split()
                idle_idx = ln_sp.index('%idle')

            elif(av_hit):
                #get the idle
                ln_sp = ln.split()
                usage = 100.0 - float(ln_sp[idle_idx])
                usage_list.append(usage)
                break

        # print(test_dic['out'][3])
        last_cpu_usage = usage_list[0]
        return usage_list[0]
    else:
        return last_cpu_usage


network_info_dic = {}

def get_network_usage(network_device):
    global network_info_dic

    current_time = time.time()

    #network_info_dic[]
    recv = psutil.net_io_counters(pernic=True).get(network_device).bytes_recv
    sent = psutil.net_io_counters(pernic=True).get(network_device).bytes_sent

    ret = 0.0

    max_network = float(args['network_max_throughput']) * 1024 * 1024 * 1.0

    if('time' in network_info_dic):
        ret = ((recv + sent) - (network_info_dic['sent'] + network_info_dic['recv'])) / max_network
        
    network_info_dic['time'] = current_time
    network_info_dic['sent'] = sent
    network_info_dic['recv'] = recv

    if(ret < 0.0001):
        return 0.0

    return ret

root_disk_info_dic = {}

def get_root_disk_usage():
    global root_disk_info_dic

    current_time = time.time()

    read = psutil.disk_io_counters().read_bytes
    write = psutil.disk_io_counters().write_bytes

    ret = 0.0

    max_io = float(args['network_max_throughput']) * 1024 * 1024 * 1.0

    if('time' in root_disk_info_dic):
        ret = ((read + write) - (root_disk_info_dic['write'] + root_disk_info_dic['read'])) / max_io
        
    root_disk_info_dic['time'] = current_time
    root_disk_info_dic['write'] = write
    root_disk_info_dic['read'] = read

    if(ret < 0.0001):
        return 0.0

    return ret


if __name__ == "__main__":
    #read the hostfile
    sleep_interval = float(args['interval'])
    if(sleep_interval <= 0):
        sleep_interval = 0.5

    #get sys config
    mem, swap = get_mem_swap_sz()
    hostname = get_hostname()

    my_info_dic = {}

    #online data that will be refreshed
    online_list_of_dic_to_write = []
    offline_list_of_dic_to_write = []

    max_monitor_second = float(args['max_second'])

    last_time = time.time()
    launch_time = last_time

    append_log_name = "{}/{}".format(args['localdir'], 'monitor-append.log')

    loop_cnt = 0
    penalty_time = 0.0

    while True:
        loop_cnt += 1
        #run the command
        used_mem = get_cur_used_mem()
        free_mem = mem - used_mem

        cpu_usage = get_cpu_average_usage_async(sleep_interval) #this should be a integer list
        network_usage = get_network_usage(args['network_device'])
        disk_usage = get_root_disk_usage()

        my_info_dic = {}
        if(my_rank == master_rank):
            _ = 1
        else:
            my_info_dic['cpu'] = cpu_usage / 100.0
            my_info_dic['mem'] = (1.0 * used_mem) / mem
            my_info_dic['network'] = network_usage
            my_info_dic['disk'] = disk_usage

        host_info = comm.gather(my_info_dic, root = master_rank)

        my_info_dic['cpu'] = 0.0
        my_info_dic['mem'] = 0.0
        my_info_dic['network'] = 0.0
        my_info_dic['disk'] = 0.0

        if my_rank == master_rank:
            #print the info
            #flush the stdout
            dic_count = 0.0
            for dics in host_info:
                if(not len(dics) == 0):
                    dic_count += 1.0
                    for key in my_info_dic:
                        my_info_dic[key] += dics[key]

            for key in my_info_dic:
                my_info_dic[key] /= dic_count
                if(my_info_dic[key] < 0.0001):
                    my_info_dic[key] = 0.0

            cur_time = time.time()
            for ele in online_list_of_dic_to_write:
                # ele['time'] -= (cur_time - last_time)
                ele['time'] -= sleep_interval
            last_time = cur_time

            for key in my_info_dic:
                dic_to_append = {}
                dic_to_append['value'] = my_info_dic[key]
                dic_to_append['time'] = max_monitor_second
                dic_to_append['type'] = key
                online_list_of_dic_to_write.append(dic_to_append.copy())

                dic_to_append['time'] = cur_time - launch_time
                offline_list_of_dic_to_write.append(dic_to_append.copy())

            my_info_dic['time'] = cur_time
            print(json.dumps(my_info_dic))
            sys.stdout.flush()
            with open(append_log_name, 'a') as out_f:
                out_f.write(json.dumps(my_info_dic) + '\n')

            while(len(online_list_of_dic_to_write) > 0):
                if(online_list_of_dic_to_write[0]['time'] < 0.0):
                    online_list_of_dic_to_write.pop(0)
                else:
                    break

            monitor_writting_path = args['localdir'] + '/monitor-data_writting.json'
            monitor_finish_path = args['localdir'] + '/monitor-data.json'
            with open(monitor_writting_path, 'w') as f:
                f.write('[\n')
                for i in range(len(online_list_of_dic_to_write)):
                    f.write(json.dumps(online_list_of_dic_to_write[i]))
                    if(i != len(online_list_of_dic_to_write) - 1):
                        f.write(',\n')
                    else:
                        f.write('\n')
                f.write(']\n')
                f.close()
                os.system("mv {} {}".format(monitor_writting_path, monitor_finish_path))



        # break
        comm.Barrier() #this is critical, because MPI_Gather on small data do not sync all nodes
        next_wake_time = launch_time + penalty_time + loop_cnt * sleep_interval
        time_after_barrier = time.time()
        time_to_sleep = next_wake_time - time_after_barrier

        local_time_penalty = 0.0

        if (time_to_sleep < 0):
            # make time_to_sleep = 0
            local_time_penalty += 0.0 - time_to_sleep

        local_time_penalty = comm.allreduce(local_time_penalty, op=MPI.MAX)
        penalty_time += local_time_penalty

        next_wake_time = launch_time + penalty_time + loop_cnt * sleep_interval
        time_to_sleep = next_wake_time - time_after_barrier

        time.sleep(time_to_sleep)
