__all__ = ['gminer_datasets', 'gminer_apps', 'gminer_sysconfig']
cd_param = {
        'name':'k-threshold',
        'type':None,
        'help':'Threshold that only store the result communities with size >= K_THRESHOLD. Should be an integer within [2, 10000].',
        'default': 6,
        }

min_weight = {
        'name': 'min-weight',
        'type': None,
        'help': 'Threshold that only add the new candidates into subgraph with weight >= MIN_WEIGHT. Should be a number within [0.0, 1.0].',
        'default': 0.57,
        }

min_core_size = {
        'name': 'min-core-size',
        'type': None,
        'help': 'Threshold that only compute the seedtask with its subgraph size >= MIN_CORE_SIZE. Should be an integer within [1, 100].',
        'default': 3
        }

min_result_size = {
        'name': 'min-result-size',
        'type': None,
        'help':'Threshold that only store the result cluster with size >= MIN_RESULT_SIZE. Should be an integer within [1, 1000].',
        'default': 20
        }

diff_ratio = {
        'name': 'diff-ratio',
        'type': None,
        'help': 'Threshold for judging two weight is similarity or not. Should be a number within [0.0, 1.0].',
        'default': 0.05
        }

iter_round_max = {
        'name': 'iter-round-max',
        'type': None,
        'help': 'Threshold that only compute number of iterations < ITER_ROUND_MAX with each task. Should be an integer within [1, 3000].',
        'default': 10
        }

cand_max_time = {
        'name': 'cand-max-time',
        'type': None,
        'help': 'Threshold that only compute the top CAND_MAX_TIME*subgraph_size candidates in each round during computation. Should be an integer within [1, 3000].',
        'default': 3
        }

# visualization sampling

tc_sampling_min = {
        'name': 'tc-sampling-min',
        'type': None,
        'help': 'Minimum triangle count. Should be an integer within [4, 200].',
        'default': 4
        }

tc_sampling_max = {
        'name': 'tc-sampling-max',
        'type': None,
        'help': 'Maximum triangle count. Should be an integer within [4, 200].',
        'default': 100
        }

gm_sampling_min = {
        'name': 'gm-sampling-min',
        'type': None,
        'help': 'Minimum matched pattern count. Should be an integer within [4, 200].',
        'default': 4
        }

gm_sampling_max = {
        'name': 'gm-sampling-max',
        'type': None,
        'help': 'Maximum matched pattern count. Should be an integer within [4, 200].',
        'default': 100
        }

cd_sampling_min = {
        'name': 'cd-sampling-min',
        'type': None,
        'help': 'Minimum community size. Should be an integer within [4, 100].',
        'default': 4
        }

cd_sampling_max = {
        'name': 'cd-sampling-max',
        'type': None,
        'help': 'Maximum community size. Should be an integer within [4, 100].',
        'default': 50
        }

gc_sampling_min = {
        'name': 'gc-sampling-min',
        'type': None,
        'help': 'Minimum cluster size. Should be an integer within [4, 100].',
        'default': 8
        }

gc_sampling_max = {
        'name': 'gc-sampling-max',
        'type': None,
        'help': 'Maximum cluster size. Should be an integer within [4, 100].',
        'default': 50
        }

cache_size = {
        'name': 'cache-size',
        'help': 'The size of cachetable in each worker. Should be an integer within [10000, INT_MAX].',
        'default': 1000000000
        }
num_comp_th = {
        'name': 'num-comp-thread',
        'help': 'Number of threads in threadpool for task computation. Should be an integer within [1, 22].',
        'default': 22
        }
pipe_pop_num = {
        'name': 'pipe-pop-num',
        'help': 'Number of tasks popped out each batch in the pipeline. Should be an integer within [100, 5000].',
        'default': 500
        }
pop_num = {
        'name': 'pop-num',
        'help': 'Number of tasks for pque pops tasks to remote worker during one stealing procedure. Should be an integer within [50, 5000].',
        'default': 100
        }
subg_size_t = {
        'name': 'subg-size-t',
        'help': 'Threshold that task can be moved to other workers only if its current subgraph size <= SUBG_SIZE_T. Should be an integer within [10, 100].',
        'default': 30
        }

gminer_datasets = ['youtube', 'skitter', 'orkut', 'friendster', 'dblp', 'tencent']

gminer_apps = [
            {'tc': {'name':'Triangle Counting', 'param': None, 'rules': [tc_sampling_min, tc_sampling_max]}} , 
            {'mc': {'name':'Max Clique', 'param': None, 'rules': None}} , 
            {'gm': {'name':'Graph Matching', 'param': None, 'rules': [gm_sampling_min, gm_sampling_max]}} , 
            {'cd': {'name': 'Community Detection', 'param': [cd_param], 'rules': [cd_sampling_min, cd_sampling_max]}}, 
            {'fco': {'name': 'Graph Clustering', 'param': [min_weight, min_core_size, min_result_size, diff_ratio, iter_round_max, cand_max_time], 'rules': [gc_sampling_min, gc_sampling_max]}}
            ]

gminer_supervisors = [
    {'name': 'James Cheng', 'img':'jcheng.jpg', 'info':'', 'other':'Supervisor'},
    {'name': 'Hongzhi Cheng', 'img': 'chz.jpg', 'info': ' is a PhD student in the Department of Computer Science and Engineering, The Chinese University of Hong Kong. His research interests cover the broad area of distributed systems and databases, with special emphasis on large-scale graph processing systems, distributed data analytics systems.'}
    ]
gminer_persons = [
    {'name': 'Xiaoxi Wang', 'img': 'noimage.jpg', 'info': ' is working as a research assistant in CUHK. She is interested in distributed machine learning'},
    {'name': 'Chenghuan Huang', 'img': 'hch.jpg', 'info': ' is a Research Assistant in the Department of Computer Science and Engineering, Chinese University of Hong Kong. He is familiar with parallel programming.'},
    {'name': 'Juncheng Fang', 'img': 'jc.jpg', 'info': ' is an undergraduate in the Department of Computer Science and Engineering, The Chinese University of Hong Kong. He is now interested in distributed system.'},
    {'name': 'Yifan Hou', 'img': 'hyf.jpg', 'info': ' is working toward the MPhil degree in the Department of Computer Science and Engineering, Chinese University of Hong Kong. He is interested in large-scale graph processing and graph embedding'},
    {'name': 'Changji Li', 'img': 'lcj.jpg', 'info': ' is currently a Master student in Department of Computer Science and Engineering, Chinese University of Hong Kong. He will pursue MPhil degree in CSE, CUHK as well whose research interests are about distributed computing system and large-scale graph processing.'},
    {'name': 'Jian Zhang', 'img': 'zj.jpg', 'info': ' is a PhD student in the Department of Computer Science and Engineering, The Chinese University of Hong Kong. He is now interested in distributed systems and high-performanced computation.'},
    ]

gminer_compare = [
    {'slide': 1, 'description': 'We tested only TC and MCF on the four non-attributed graphs, as most of the systems could not handle the other three applications. Table 3 presents the results. Arabesque, Giraph and GraphX either ran out of memory (OOM) or over 24 hours for most of the cases. For the relatively small graphs (i.e., Skitter and Orkut), their performance (even for TC) is significantly worse than G-Miner. The main reasons for their poor performance are because (1) these systems do not have a mechanism to process a large number of subgraphs (e.g., pipelining them to disk) and hence result in OOM, and (2) they all suffer from high synchronization barrier costs. Thus, they cannot scale to handle heavier workloads and larger datasets.'},
    {'slide': 2, 'description': 'Table 4 reports the elapsed running time, the average CPU utilization, the peak aggregate memory usage, and the aggregate amount of network communication of the cluster. We also list the total number of matched patterns under the dataset names. For this heavier workload, G-Miner is significantly faster than G-thinker for all the graphs. This can be partially explained by GMiner’s much higher CPU utilization. Note that the CPU utilization for Skitter is considerably low because it is the smallest graph and so the workload is not heavy enough to make full use of the resource. G-Miner also reduces the communication load and memory usage by its design of process-level cache shared by all the computing threads.'},
    {'slide': 3, 'description': 'As shown in Figure 5, although G-thinker also adopts a subgraphcentric computational model, its system design does not allow it to effectively overlap network communication with CPU computation, and CPU cores are waiting for data from network in short intermittent periods throughout the whole process.'},
    {'slide': 4, 'description': 'In contrast, Figure 6 shows that G-Miner allows all the CPU cores to be highly utilized all the time, as its streamlined task-pipeline continues to feed the computing threads with active tasks to be processed. The design of the LSH-based task priority queue and RCV Cache in G-Miner also effectively reduces the vertex pulling except at the early stage when the cache is not yet filled.'},
    {'slide': 5, 'description': 'Table 5 reports the elapsed running time and memory usage. Due to the much more complicated candidate filtering and processing of CD and GC, G-Miner used considerably more memory and time to complete the jobs. However, even for these heavy, complicated workloads on graphs with high-dimensional attribute lists, G-Miner still recorded good performance numbers, especially considering that Arabesque, Giraph and GraphX actually obtained worse numbers even for the lightest workload, i.e., TC, as shown in Table 3.'},
    {'slide': 6, 'description': 'McSherry et al. proposed COST to measure the cost of scalability using a distributed system. COST is defined as the minimum number of cores a parallel/distributed solution used to outperform an optimized single-threaded implementation. Figure 7 reports the result, where the horizontal dotted lines plot the running time of a single-threaded implementation. The COST of G-Miner is 3, 3, 2, 2, respectively, for TC on Skitter, TC on Orkut, GM on Skitter, GM on Orkut. These numbers are much lower than the systems measured in Frank McSherry\'s article: Scalability! But at what COST?'},
    {'slide': 7, 'description': 'Figure 8 and Figure 9 show that G-Miner achieves good speedups in most of the cases, especially for the heavier workload GM on Friendster. G-Miner’s good scalability is due to two reasons: (1) our task-pipeline design enables the computing threads to totally focus on the local task processing without being interrupted by the network communication requests, which matches the computationintensive nature of graph mining workloads; (2) graph partitioning and task stealing enable good load balancing among the nodes.'},
    {'slide': 8, 'description': 'Figure 8 and Figure 9 show that G-Miner achieves good speedups in most of the cases, especially for the heavier workload GM on Friendster. G-Miner’s good scalability is due to two reasons: (1) our task-pipeline design enables the computing threads to totally focus on the local task processing without being interrupted by the network communication requests, which matches the computationintensive nature of graph mining workloads; (2) graph partitioning and task stealing enable good load balancing among the nodes.'},
    {'slide': 9, 'description': 'We also show the scalability of the four systems we compared with in Figure 10 as a reference. The figure shows that without a good system design and load balancing mechanism, there is no guarantee on the system scalability. Note that we were only able to measure the scalability for these systems on the smaller datasets using TC for reasons such as OOM and time limit.'},
    ]

gminer_sysconfig = [
    cache_size, num_comp_th, pipe_pop_num, pop_num, subg_size_t 
]

def get_gminer_codes():
    keys = ['tc','mc','gm','cd','fco']
    filen = ['trianglecount.cpp','maxclique.cpp','graphmatch.cpp','community.cpp','focusCO.cpp']
    res = []
    for k, name in zip(keys, filen):
        with open('./apps/' + name) as f:
            a = f.read()
            res.append((k, a))
    return res

gminer_codes = get_gminer_codes()
