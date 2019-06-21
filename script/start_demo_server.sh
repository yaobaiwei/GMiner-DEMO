#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source $DIR/demo-env.sh

# port of the web server
export FLASK_APP=main.py
export FLASK_ENV=deployment

export GMINER_HOME=$DIR/../
cd $GMINER_HOME

mkdir -p $GMINER_MERGE_LOG_PATH

rm -rf $GMINER_HOME/web/runtime-infos
ln -s $GMINER_MERGE_LOG_PATH $GMINER_HOME/web/runtime-infos
rm -rf $GMINER_HOME/web/runtime-infos/*

if [[ -d tmp ]]; then
  rm -rf tmp/*
fi

rm -rf /tmp/${USER}-gminer-dump
mkdir -p /tmp/${USER}-gminer-dump

# clean remaining processes

python $GMINER_HOME/script/python/kill-remaining-gminer-processes.py

# cluster monitor
mpirun -n $NUM_WORKER -ppn 1 -f web/machines.cfg python $GMINER_HOME/script/python/cluster-monitor.py \
                                            -d $GMINER_MERGE_LOG_PATH \
                                            -nd $NETWORK_DEVICE \
                                            -nt $MAX_NETWORK_THROUGHPUT_MB \
                                            -dt $MAX_DISK_THROUGHPUT_MB > /dev/null &

# web server
cd web/

flask run --port $FLASK_PORT
