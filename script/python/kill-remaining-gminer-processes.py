import os
import sys
import json
import argparse
import subprocess
import io
import time
import psutil

def run_bg_cmd(command):
    proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    proc.wait()
    lns_content = []
    for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
        if(line[-1] == '\n'):
            lns_content.append(line[:-1])
        else:
            lns_content.append(line)
    return lns_content

if __name__ == "__main__":
    grep_monitor_command = 'ps -ef | grep {} | grep mpiexec | grep "utils/gminer-demo-coordinator-mpi.py"'.format(os.environ['USER'])
    grep_coordinator_command = 'ps -ef | grep {} | grep mpirun | grep "script/python/cluster-monitor.py"'.format(os.environ['USER'])

    for cmd in [grep_monitor_command, grep_coordinator_command]:
        grep_result = run_bg_cmd(cmd)
        for line in grep_result:
            line_split = line.split()
            if len(line_split) > 1:
                run_bg_cmd('kill -15 {} >> /dev/null'.format(line_split[1]))
