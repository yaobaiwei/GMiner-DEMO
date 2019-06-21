//Copyright 2018 Husky Data Lab, CUHK
//Authors: Hongzhi Chen, Miao Liu


#include "global.hpp"

int _my_rank;
int _num_workers;
string _given_timestamp_str;

double get_running_wtime()
{
	static int cnt = 0;
	static double ini_wtime;

	if(cnt == 0)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		ini_wtime = MPI_Wtime();
		cnt++;
	}

	return MPI_Wtime() - ini_wtime;
}

uint64_t get_timestamp()
{
	return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

string get_timestamp_str()
{
	char c[32];
	sprintf(c, "%ld", get_timestamp());
	return string(c);
}

void init_worker(int * argc, char*** argv)
{
	int provided;
	MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
	if(provided != MPI_THREAD_MULTIPLE)
	{
		printf("MPI do not Support Multiple thread\n");
		exit(0);
	}
	MPI_Comm_size(MPI_COMM_WORLD, &_num_workers);
	MPI_Comm_rank(MPI_COMM_WORLD, &_my_rank);
}

void worker_finalize()
{
	MPI_Finalize();
}

void worker_barrier()
{
	MPI_Barrier(MPI_COMM_WORLD);
}

//------------------------
// worker parameters
WorkerParams::WorkerParams()
{
	local_root = "/tmp/gminer";
	force_write = true;
	native_dispatcher = false;
}

//============================

int global_step_num;
int global_phase_num;

void* global_message_buffer = NULL;
void* global_combiner = NULL;
void* global_aggregator = NULL;
void* global_agg = NULL; //for aggregator, FinalT of last round

int global_vnum = 0;
int global_active_vnum = 0;

//currently, only 3 bits are used, others can be defined by users
char global_bor_bitmap;

void clear_bits()
{
	global_bor_bitmap = 0;
}

void set_bit(int bit)
{
	global_bor_bitmap |= (2 << bit);
}

int get_bit(int bit, char bitmap)
{
	return ((bitmap & (2 << bit)) == 0) ? 0 : 1;
}

void has_msg()
{
	set_bit(HAS_MSG_ORBIT);
}

void wake_all()
{
	set_bit(WAKE_ALL_ORBIT);
}

void force_terminate()
{
	set_bit(FORCE_TERMINATE_ORBIT);
}

//================================DISK IO (TASKQUEUE)=====================================
string PQUE_DIR;
string MERGE_DIR;

void mk_dir(const char *dir)
{
	char tmp[256];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", dir);
	len = strlen(tmp);
	if(tmp[len - 1] == '/') tmp[len - 1] = '\0';
	for (p = tmp + 1; *p; p++)
	{
		if (*p == '/')
		{
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	}
	mkdir(tmp, S_IRWXU);
}

void rm_dir(string path)
{
	DIR* dir = opendir(path.c_str());
	struct dirent * file;
	while ((file = readdir(dir)) != NULL)
	{
		if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
			continue;
		string filename = path + "/" + file->d_name;
		remove(filename.c_str());
	}
	if (rmdir(path.c_str()) == -1)
	{
		perror ("The following error occurred");
		exit(-1);
	}
}

void check_dir(string path, bool force_write)
{
	if(access(path.c_str(), F_OK) == 0 )
	{
		if (force_write)
		{
			rm_dir(path);
			mk_dir(path.c_str());
		}
		else
		{
			cout << path <<  " already exists!" << endl;
			exit(-1);
		}
	}
	else
	{
		mk_dir(path.c_str());
	}
}

//=========================================================

//=================multi-thread============================
unsigned long long TASK_IN_MEMORY_NUM=0;
unsigned long long TASK_IN_DISK_NUM=0;

mutex global_lock;

void rm_dumped_tasks(string path){
	DIR* dir = opendir(path.c_str());
	struct dirent * file;
	while ((file = readdir(dir)) != NULL)
	{
		if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
			continue;
		string filename = path + "/" + file->d_name;
		remove(filename.c_str());
	}
}

void inc_task_num_in_disk(int num)
{
	lock_guard<mutex> lck(global_lock);
	TASK_IN_DISK_NUM += num;
}

void dec_task_num_in_disk(int num)
{
	lock_guard<mutex> lck(global_lock);
	TASK_IN_DISK_NUM -= num;
}

unsigned long long get_task_num_in_disk()
{
	lock_guard<mutex> lck(global_lock);
	return TASK_IN_DISK_NUM;
}

void inc_task_num_in_memory(int num)
{
	lock_guard<mutex> lck(global_lock);
	TASK_IN_MEMORY_NUM += num;
}

void dec_task_num_in_memory(int num)
{
	lock_guard<mutex> lck(global_lock);
	TASK_IN_MEMORY_NUM -= num;
}

unsigned long long get_task_num_in_memory()
{
	lock_guard<mutex> lck(global_lock);
	return TASK_IN_MEMORY_NUM;
}
//============================HDFS Parameters==========================
string HDFS_HOST_ADDRESS;
int HDFS_PORT;

//==========================System Parameters==========================
// max number of tasks buffered in memory before merging in priority queue
int global_merge_limit=1000;
// max number of tasks buffered in memory in pipeline
int global_taskBuf_size=1000;
// number of dimensions for signature
int global_sign_size=4;
// max number of tasks in data file of priority queue
int global_file_size=100;
// setting for merge_sort in task_seeding stage
int NUM_WAY_OF_MERGE=1000;

// num of threads in threadpool for computation
int NUM_COMP_THREAD=2;
// the size of cache in each worker
int CACHE_SIZE=1000000;
// # of tasks popped out each batch in the pipeline
int PIPE_POP_NUM=100;

// # of tasks popped out for each remote remove
int POP_NUM=100;
// thresholds of tasks to measure whether the task can be moved to other workers
int SUBG_SIZE_T=30;
double LOCAL_RATE=0.5;

// #of seconds for sleep in thread context_sync
double AGG_SLEEP_TIME=0;
double SYS_SLEEP_TIME=0;


//==========================DEMO Parameters==========================
string DEMO_LOG_PATH;
const string RESUME_DEMO_STR_EMPTY="{\"status\": \"empty\"}";

void load_hdfs_config()
{
	dictionary *ini;
	double val, val_not_found = -1;
	char *str, *str_not_found = "null";

	const char* GMINER_HOME = getenv("GMINER_HOME");
	if(GMINER_HOME == NULL)
	{
		fprintf(stderr, "must conf the ENV: GMINER_HOME. exits.\n");
		exit(-1);
	}
	// string conf_path(GMINER_HOME);

	const char* GMINER_INI_NAME = getenv("GMINER_INI_NAME");
	string ini_name;
	if(GMINER_INI_NAME == NULL)
		ini_name = "gminer-conf.ini";
	else
		ini_name = GMINER_INI_NAME;

	string conf_path = ini_name;
	ini = iniparser_load(conf_path.c_str());
	if(ini == NULL)
	{
		fprintf(stderr, "can not open %s. exits.\n", ini_name.c_str());
		exit(-1);
	}

	// [PATH]
	str = iniparser_getstring(ini,"PATH:HDFS_HOST_ADDRESS", str_not_found);
	if(strcmp(str, str_not_found)!=0) HDFS_HOST_ADDRESS = str;
	else
	{
		fprintf(stderr, "must enter the HDFS_HOST_ADDRESS. exits.\n");
		exit(-1);
	}

	val = iniparser_getint(ini, "PATH:HDFS_PORT", val_not_found);
	if(val!=val_not_found) HDFS_PORT=val;
	else
	{
		fprintf(stderr, "must enter the HDFS_PORT. exits.\n");
		exit(-1);
	}

	iniparser_freedict(ini);
}

void load_system_parameters(WorkerParams& param)
{
	dictionary *ini;
	double val, val_not_found = -1;
	char *str, *str_not_found = "null";

	const char* GMINER_HOME = getenv("GMINER_HOME");
	if(GMINER_HOME == NULL)
	{
		fprintf(stderr, "must conf the ENV: GMINER_HOME. exits.\n");
		exit(-1);
	}
	// string conf_path(GMINER_HOME);

	const char* GMINER_INI_NAME = getenv("GMINER_INI_NAME");
	string ini_name;
	if(GMINER_INI_NAME == NULL)
		ini_name = "gminer-conf.ini";
	else
		ini_name = GMINER_INI_NAME;

	// conf_path.append("/" + ini_name);

	string conf_path = ini_name;
	ini = iniparser_load(conf_path.c_str());
	if(ini == NULL)
	{
		fprintf(stderr, "can not open %s. exits.\n", ini_name.c_str());
		exit(-1);
	}

	// [PATH]

	str = iniparser_getstring(ini,"PATH:HDFS_INPUT_PATH", str_not_found);
	if(strcmp(str, str_not_found)!=0) param.input_path = str;
	else
	{
		fprintf(stderr, "must enter the HDFS_INPUT_PATH. exits.\n");
		exit(-1);
	}

	str = iniparser_getstring(ini,"PATH:HDFS_OUTPUT_PATH", str_not_found);
	if(strcmp(str, str_not_found)!=0) param.output_path = str;
	else
	{
		fprintf(stderr, "must enter the HDFS_OUTPUT_PATH. exits.\n");
		exit(-1);
	}

	str = iniparser_getstring(ini,"PATH:LOCAL_TEMP_PATH", str_not_found);
	if(strcmp(str, str_not_found)!=0) param.local_root = str;
	else
	{
		fprintf(stderr, "must enter the LOCAL_TEMP_PATH. exits.\n");
		exit(-1);
	}

	const char* GMINER_START_TIMESTAMP = getenv("GMINER_START_TIMESTAMP");
	string ts_str = "";
	if(GMINER_START_TIMESTAMP == NULL)
	{
		//assign the timestamp manually
		ts_str = get_timestamp_str();
	}
	else
	{
		//convert from env
		ts_str = string(GMINER_START_TIMESTAMP);
	}
	_given_timestamp_str = ts_str;


	const char* GMINER_LOG_PATH = getenv("GMINER_LOG_PATH");
	if(GMINER_LOG_PATH == NULL)
	{
		fprintf(stderr, "must enter the LOCAL_TEMP_PATH. exits.\n");
		exit(-1);
		// DEMO_LOG_PATH = "";
	}
	else
	{
		DEMO_LOG_PATH = string(GMINER_LOG_PATH) + "/" + ts_str + "/";

		string cmd = "mkdir -p ";
		cmd += DEMO_LOG_PATH;
		system(cmd.c_str());
	}

	val = iniparser_getboolean(ini, "", val_not_found);
	if(val!=val_not_found) param.force_write = bool(val);

	param.native_dispatcher = false;

	// [COMPUTING]
	val = iniparser_getint(ini, "COMPUTING:CACHE_SIZE", val_not_found);
	if(val!=val_not_found) CACHE_SIZE=val;

	val = iniparser_getint(ini, "COMPUTING:NUM_COMP_THREAD", val_not_found);
	if(val!=val_not_found) NUM_COMP_THREAD=val;

	val = iniparser_getint(ini, "COMPUTING:PIPE_POP_NUM", val_not_found);
	if(val!=val_not_found) PIPE_POP_NUM=val;

	// [STEALING]
	val = iniparser_getint(ini, "STEALING:POP_NUM", val_not_found);
	if(val!=val_not_found) POP_NUM=val;

	val = iniparser_getint(ini, "STEALING:SUBG_SIZE_T", val_not_found);
	if(val!=val_not_found) SUBG_SIZE_T=val;

	val = iniparser_getdouble(ini, "STEALING:LOCAL_RATE", val_not_found);
	if(val!=val_not_found) LOCAL_RATE=val;

	// [CONTEXT SYNC]
	val = iniparser_getdouble(ini, "SYNC:AGG_SLEEP_TIME", val_not_found);
	if(val!=val_not_found) AGG_SLEEP_TIME=val;
	if (AGG_SLEEP_TIME < 0.0)
		AGG_SLEEP_TIME = 0.0;

	// [SYSTEM SYNC]
	val = iniparser_getdouble(ini, "SYNC:SYS_SLEEP_TIME", val_not_found);
	if(val!=val_not_found) SYS_SLEEP_TIME=val;
	if (SYS_SLEEP_TIME <= 0.0)
		SYS_SLEEP_TIME = 1.0;


	iniparser_freedict(ini);
}

//=====================================================================
