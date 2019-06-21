
# web server
export FLASK_PORT=5080

# cluster
export NUM_WORKER=11

# gminer_output path
export GMINER_LOG_PATH=/tmp/${USER}-gminer-demo-log/
export GMINER_MERGE_LOG_PATH=/dev/shm/${USER}-gminer-demo-output-merged/

# cluster monitor
export NETWORK_DEVICE=em1
export MAX_NETWORK_THROUGHPUT_MB=256
export MAX_DISK_THROUGHPUT_MB=519.424

# HDFS data path
## for TC and MC
export YOUTUBE_PLAIN_GRAPH=/GMiner_input/youtube_10/
export SKITTER_PLAIN_GRAPH=/GMiner_input/skitter_10/
export ORKUT_PLAIN_GRAPH=/GMiner_input/orkut_10/
export FRIENDSTER_PLAIN_GRAPH=/GMiner_input/friendster_10/
## for GM
export YOUTUBE_LABELED_GRAPH=/GMiner_input/youtube_label_10/
export SKITTER_LABELED_GRAPH=/GMiner_input/skitter_label_10/
export ORKUT_LABELED_GRAPH=/GMiner_input/orkut_label_10/
export FRIENDSTER_LABELED_GRAPH=/GMiner_input/friendster_label_10/
## for CD
export YOUTUBE_ATTR_GRAPH=/GMiner_input/youtube_attr_10/
export SKITTER_ATTR_GRAPH=/GMiner_input/skitter_attr_10/
export ORKUT_ATTR_GRAPH=/GMiner_input/orkut_attr_10/
export TENCENT_ATTR_GRAPH=/GMiner_input/tencent_attr_10/
## for GC
export YOUTUBE_FCO_GRAPH=/GMiner_input/youtube_focus_10/
export SKITTER_FCO_GRAPH=/GMiner_input/skitter_focus_10/
export ORKUT_FCO_GRAPH=/GMiner_input/orkut_focus_10/
export DBLP_FCO_GRAPH=/GMiner_input/dblp_focus_10/

