//Copyright 2018 Husky Data Lab, CUHK
//Authors: Hongzhi Chen, Miao Liu


//PART 1 =======================================================
//constructor

template <class TaskT,  class AggregatorT>
Slave<TaskT, AggregatorT>::Slave()
{
	cache_size_ = CACHE_SIZE;
	cache_overflow_ = false;
	merged_file_num_ = 0;
	is_end_ = false;
	report_end_ = false;
	threadpool_size_ = NUM_COMP_THREAD;
	vtx_req_count_=vtx_resp_count_=0;
	task_count_ = 0;
	task_counter_ = 0;
	pthread_spin_init(&task_counter_lock_, 0);
	resume_task_ = false;
	computing_task_count_ = 1;

	stealing_finished_ = false;
	global_stealing_finished_ = false;
	to_pause_ = false;

	resume_task_ = false;

	task_finished_count_ = 0;
	task_recycle_count_ = 0;
	task_to_cmq_count_ = 0;
	task_to_cpq_count_ = 0;
}

//PART 2 =======================================================
//user-defined functions

template <class TaskT,  class AggregatorT>
inline typename Slave<TaskT, AggregatorT>::VertexT* Slave<TaskT, AggregatorT>::respond(const typename Slave<TaskT, AggregatorT>::VertexT * v)
{
	return NULL;
}

//PART 3 =======================================================
//communication primitives

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::set_req(ibinstream & m, vector<KeyT>& request)
{
	m << REQUEST_VERTEX;
	m << _my_rank;
	m << request;
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::set_end_tag(ibinstream & m) //negative worker ID means that work has finished its computation
{
	m << REQUEST_END; //one may get the ID of the worker by doing - x - 1 again (e.g., may be useful for work stealing)
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::set_steal(ibinstream & m)
{
	m << REQUEST_TASK;
	m << _my_rank;
}

template <class TaskT,  class AggregatorT>
int Slave<TaskT, AggregatorT>::set_resp(ibinstream & m, obinstream & um)//return requester machine ID, -1 means end-tag
{
	int tag;
	um >> tag;
	switch(tag)
	{
	case REQUEST_END:
		return -1;
	case REQUEST_VERTEX:
	{
		int tgt;
		vector<KeyT> ids;

		um >> tgt;
		um >> ids;

		int num = ids.size();//number of vertices in response
		m << num;
		for(int i=0; i<num; i++)
		{
			KeyT id = ids[i];
			VertexT * tmp = respond(local_table_.get(id));
			if(tmp != NULL)
			{
				m << tmp;
				delete tmp;
			}
			else
			{
				m << local_table_.get(id);
			}
		}
		return tgt;
	}
	case REQUEST_TASK:
	{
		int tgt;
		um >> tgt;
		TaskVec remote_tasks;
		task_pipeline_.pq_pop_front_to_remote(remote_tasks);
		int num = remote_tasks.size();
		m << num;
		for(int i = 0; i < num; i++)
		{
			m << remote_tasks[i];
			delete remote_tasks[i];
		}
		return tgt;
	}
	}
}

//PART 4 =======================================================
//high level functions for system


template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::WaitDuringPause()
{
	bool to_pause = to_pause_;
	if (to_pause) {
		unique_lock<mutex> lck(pause_mutex_);
		while (to_pause) {
			to_pause = to_pause_;
			pause_condition_variable_.wait(lck);
		}
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::NotifyWhenPauseFinished()
{
	unique_lock<mutex> lck(pause_mutex_);
	pause_condition_variable_.notify_all();
}

template <class TaskT,  class AggregatorT>
bool Slave<TaskT, AggregatorT>::wait_to_start()
{
	MSG cmd = slave_bcastCMD();
	if(cmd == START)
		return true;
	return false;
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::load_graph(const char* inpath)
{
	hdfsFS fs = get_hdfs_fs();
	hdfsFile in = get_r_handle(inpath, fs);
	LineReader reader(fs, in);
	while(true)
	{
		reader.read_line();
		if (!reader.eof())
		{
			VertexT * v = to_vertex(reader.get_line());
			local_table_.set(v->id, v);
		}
		else
			break;
	}
	hdfsCloseFile(fs, in);
	hdfsDisconnect(fs);
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::demo_resume_handle() 
{
	{
		unique_lock<mutex> lck(resume_signal_mx_);
		resume_signal_cond_.wait(lck);
	}
	if(is_end_){
		if (debug_)
			cout << "[demo_resume_handle] slave id: " << _my_rank<<" is_end_==true " << endl;
		return;
	}

	KeyT seed_id;
	seed_id = recv_data<KeyT>(MASTER_RANK, DEMO_RESUME_CHANNEL);

	VertexT* seed_vertex = local_table_.get(seed_id);
	if(seed_vertex == NULL){
		// cout << "[demo_resume_handle] slave id: " << _my_rank<<" exit " << endl;
		return;
	}
	else{
		send_data(_my_rank, MASTER_RANK, DEMO_RESUME_CHANNEL);
		resume_info_ = recv_data<map<string, vector<KeyT>>>(MASTER_RANK, DEMO_RESUME_CHANNEL);
		cout << "[demo_resume_handle] slave id: " << _my_rank<<" recv seed id " << seed_id << " and resume info" << endl;
		// recv resume info
  
		TaskT * t = create_task(seed_vertex);
		if(t != NULL)
		{
			t->resume_task_ = true;
			t->set_to_request();
			//all adjacents are in cache
			if(t->is_request_empty())
			{
				t->task_counter_ = GetAndIncreaseCounter();
				IncreaseComputingTaskCount();
				t = recursive_compute(t, 0);
				DecreaseComputingTaskCount();
			}
			if(t != NULL)
			{
				TaskVec tasks;
				tasks.push_back(t);
				vector<KTpair> signed_tasks;
				task_sorter_.sign_and_sort_tasks(tasks, signed_tasks);
				task_pipeline_.pq_push_back(signed_tasks);
			}
		}
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::grow_tasks(int tid)
{
	TaskVec tasks;
	VertexT* v = local_table_.next();
	while(v)
	{
		task_count_ ++;
		TaskT * t = create_task(v);
		if(t != NULL)
		{
			t->set_to_request();
			//all adjacents are in cache
			if(t->is_request_empty())
			{
				t->task_counter_ = GetAndIncreaseCounter();
				IncreaseComputingTaskCount();
				t = recursive_compute(t, tid);
				DecreaseComputingTaskCount();
			}
			if(t != NULL)
			{
				tasks.push_back(t);
				if(tasks.size() >=  global_merge_limit)
					dump_tasks(tasks);
			}
		}
		v = local_table_.next();
	}
	if(!tasks.empty())
		dump_tasks(tasks);
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::dump_tasks(TaskVec& tasks)
{
	char num[10], fname[100];
	sprintf(num, "/%d", merged_file_num_++);
	strcpy(fname, MERGE_DIR.c_str());
	strcat(fname, num);
	vector<KTpair> signed_tasks;
	ifbinstream fin;

	task_sorter_.sign_and_sort_tasks(tasks, signed_tasks);

	fin.open(fname);
	for (int i = 0; i < signed_tasks.size(); i++)
	{
		fin << signed_tasks[i];
		delete signed_tasks[i].task;
	}
	fin.close();

	inc_task_num_in_disk(tasks.size());
	tasks.clear();
}

/* -------------- DEMO Counter --------------- */
template <class TaskT,  class AggregatorT>
int Slave<TaskT, AggregatorT>::GetAndIncreaseCounter()
{
	int task_counter_before = task_counter_++;
	return (task_counter_before) * (_num_workers - 1) + _my_rank;
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::IncreaseComputingTaskCount()
{
	computing_task_count_++;
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::DecreaseComputingTaskCount()
{
	computing_task_count_--;
}

/* -------------- DEMO STR -------------- */
template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::thread_demo_str_init()
{
	thread_demo_var_.resize(threadpool_size_);

	for(int i = 0; i < threadpool_size_; i++)
	{
		if(DEMO_LOG_PATH.size() > 0)
		{
			string file_name = DEMO_LOG_PATH + "demo_node_" + to_string(_my_rank) + "_thread_" + to_string(i) + "_part_" + to_string(filename_part_) + ".log";
			thread_demo_var_[i].log_file = fopen(file_name.c_str(), "w");
		}
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::thread_demo_str_period()
{
	//switch filename periodically

	bool need_to_switch = false;
	int single_file_max_sync = 5;

	if((filename_part_ + 1) * single_file_max_sync == sys_sync_time_)
	{
		filename_part_++;
		//for example, sys_sync_time = 5, filename_part_ 0 -> 1
		need_to_switch = true;
	}

	int i = 0;
	for(auto & v : thread_demo_var_)
	{
		pthread_spin_lock(&(v.file_lock));

		if(v.log_file)
		{
			fflush(v.log_file);

			if(need_to_switch)
			{
				fclose(v.log_file);
				string file_name = DEMO_LOG_PATH + "demo_node_" + to_string(_my_rank) + "_thread_" + to_string(i) + "_part_" + to_string(filename_part_) + ".log";
				v.log_file = fopen(file_name.c_str(), "w");
			}
		}

		pthread_spin_unlock(&(v.file_lock));
		i++;
	}

	if(need_to_switch)
	{
		//write a flag file indicates that switch complete
		string file_name = DEMO_LOG_PATH + "demo_node_" + to_string(_my_rank) + "_part_" + to_string(filename_part_ - 1) + "_finish.log";
		FILE* f = fopen(file_name.c_str(), "w");
		fprintf(f, "0\n");
		fclose(f);
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::thread_demo_str_compute(const string& demo_str, int tid)
{
	pthread_spin_lock(&(thread_demo_var_[tid].file_lock));
	if(thread_demo_var_[tid].log_file)
		fprintf(thread_demo_var_[tid].log_file, demo_str.c_str());

	pthread_spin_unlock(&(thread_demo_var_[tid].file_lock));
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::thread_demo_str_finalize()
{
	for(auto & v : thread_demo_var_)
	{
		pthread_spin_lock(&(v.file_lock));

		if(v.log_file)
			fclose(v.log_file);

		pthread_spin_unlock(&(v.file_lock));
	}
}
/* ------------------------------------------------------*/
template <class TaskT,  class AggregatorT>
TaskT* Slave<TaskT, AggregatorT>::recursive_compute(TaskT* task, int tid)
{
	if(resume_task_ && !task->resume_task_)
	{
		delete task;
		return NULL;
	}
	//set frontier for UDF compute
	vector<VertexT *> frontier;
	for(int i = 0 ; i < task->to_pull.size(); i++)
	{
		AdjVertex& v=task->to_pull[i];
		if(resume_task_ && task->resume_task_ && (
			find(resume_info_["nodes"].begin(), resume_info_["nodes"].end(), v.id) != resume_info_["nodes"].end()
			// || find(resume_info_["src"].begin(), resume_info_["src"].end(), v.id) != resume_info_["src"].end()
			// || find(resume_info_["dst"].begin(), resume_info_["dst"].end(), v.id) != resume_info_["dst"].end()
		))
		{
			continue;
		}
		VertexT *pvtx=NULL;
		v.wid == _my_rank? pvtx=local_table_.get(v.id) : pvtx=cache_table_.get(v.id);
		if(resume_task_ && task->resume_task_)
		{
			for(auto id: resume_info_["nodes"])
				pvtx->del_neighbor(id);

			for(int j = 0; j < resume_info_["src"].size(); ++ j)
			{
				if(resume_info_["src"][j] == pvtx->id)
				{
					pvtx->del_neighbor(resume_info_["dst"][j]);
				}
				if(resume_info_["dst"][j] == pvtx->id)
				{
					pvtx->del_neighbor(resume_info_["src"][j]);
				}
			}
		}
		frontier.push_back(pvtx);
	}
	// delete resume nodes and edges in subgraph
	if(resume_task_ && task->resume_task_)
	{
		for(int i = 0; i < resume_info_["src"].size(); ++ i)
		{
			auto* src = task->subG.get_node(resume_info_["src"][i]);
			if (src != nullptr)
				src->del_neighbor_by_id(resume_info_["dst"][i]);
			auto* dst = task->subG.get_node(resume_info_["dst"][i]);
			if (dst != nullptr)
				dst->del_neighbor_by_id(resume_info_["src"][i]);
		}
		for(int i = 0; i < resume_info_["nodes"].size(); ++ i){
			task->subG.del_node_fully(resume_info_["nodes"][i]);
		}
	}

	if(task->compute(task->subG, task->context, frontier))
	{
		task->set_to_request();
		//if to_request is not empty, add the current task into pque
		if(task->to_request.empty())
		{
			//continue to compute until find remote vertexes needed to pull
			return recursive_compute(task, tid);
		}
		return task;
	}
	else
	{
		//The current task is done, delete it after storing the result of it through aggregator
		AggregatorT* agg = (AggregatorT*)get_aggregator();
		if (agg != NULL)
		{
			agg_lock_.lock();
			agg->step_partial(task->context);
			agg_lock_.unlock();
		}

		//for demo, or debug usage
		if(task->demo_str_.size() != 0)
		{
			thread_demo_str_compute(task->demo_str_, tid);
		}

		if(task->if_filtered_for_demo())
		{
			task->filtered_task_counter_ = thread_demo_var_[tid].GetAndIncreaseFilteredCounter();
			task->demo_str_ = "";
			task->dump_context_for_demo();
			if(task->demo_str_.size() != 0)
			{
				thread_demo_str_compute(task->demo_str_, tid);
			}
		}

		if(task->resume_task_){
			task->dump_context_for_demo();
			if(task->demo_str_.size())
				send_data<string>(task->demo_str_, MASTER_RANK, DEMO_RESUME_CHANNEL);
			else
				send_data<string>(RESUME_DEMO_STR_EMPTY, MASTER_RANK, DEMO_RESUME_CHANNEL);
		}
		delete task;
		return NULL;
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::pull_PQ_to_CMQ()
{
	queue<ibinstream> streams;
	queue<MPI_Request> mpi_requests;
	int test_flag;

	while(1)
	{
		TaskVec tasks, filtered_tasks;
		CountMap ref_count;
		CountMap worker_map;
		CountMap to_pull;

		WaitDuringPause();
		bool status = task_pipeline_.pq_pop_front(tasks);
		if(!status)
			break;

		for(int i = 0; i < tasks.size(); i++)
		{
			TaskT * t = tasks[i];
			vector<int> & reqs = t->to_request;
			AdjVtxList & nbs = t->to_pull;
			for(int j=0; j< reqs.size(); j++)
			{
				AdjVertex & item = nbs[reqs[j]];
				CountMapIter mIter = ref_count.find(item.id);
				if(mIter == ref_count.end())
				{
					ref_count[item.id] = 1;
					worker_map[item.id] = item.wid;
				}
				else
				{
					mIter->second++;
				}
			}
		}
		for(CountMapIter mIter = ref_count.begin(); mIter != ref_count.end(); mIter++)
		{
			if(!cache_table_.try_to_get(mIter->first, mIter->second))
			{
				to_pull[mIter->first] = worker_map[mIter->first];
			}
		}

		if(cache_table_.size() > cache_size_)
		{
			//guarantee the cache is locked when checking
			unique_lock<mutex> lck(commun_lock_);

			//delete the items whose ref are 0, the num is at most to_pull.size()
			int diff = cache_table_.size() - cache_size_;
			cache_table_.batch_clear(diff);

			while(cache_table_.size() > cache_size_)
			{
				if(cache_table_.size() == to_pull.size())
				{
					cache_overflow_ = true;
					cache_table_.resize(to_pull.size());
					break;
				}
				else
				{
					commun_cond_.wait(lck);
					int diff = cache_table_.size() - cache_size_;
					cache_table_.batch_clear(diff);
				}
			}
		}

		vector<vector<KeyT> > requests(_num_workers - 1);
		for(CountMapIter it = to_pull.begin(); it != to_pull.end(); it++)
		{
			requests[it->second].push_back(it->first);
		}

		vector<int> dsts;
		for(int i = 1; i <_num_workers; i++)
		{
			int dst = (_my_rank + i) % _num_workers; //to avoid receiver-side bottleneck
			if(dst != MASTER_RANK && requests[dst].size() > 0) //only send if there are requests //essentially, won't request to itself
			{
				streams.push(ibinstream());
				mpi_requests.push(MPI_Request());
				set_req(streams.back(), requests[dst]);
				send_ibinstream_nonblock(streams.back(), dst, REQUEST_CHANNEL, mpi_requests.back());
				dsts.push_back(dst);
			}

			if(mpi_requests.size() > 0)
			{
				MPI_Test(&(mpi_requests.front()), &test_flag, MPI_STATUS_IGNORE);
				if(test_flag)
				{
					mpi_requests.pop();
					streams.pop();
				}
			}
		}
		TaskPackage<TaskT> pkg(tasks, dsts);
		WaitDuringPause();
		task_pipeline_.cmq_push_back(pkg);
		task_to_cmq_count_ += tasks.size();
		vtx_req_count_++;

		std::unique_lock<std::mutex> lck(vtx_req_lock_);
		if((vtx_req_count_-vtx_resp_count_)>=VTX_REQ_MAX_GAP)
			vtx_req_cond_.wait(lck);
	}

	// wait for all send finish in order to avoid stream destroy
	while(mpi_requests.size() > 0){
		MPI_Wait(&(mpi_requests.front()), MPI_STATUS_IGNORE);
		mpi_requests.pop();
		streams.pop();
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::pull_CMQ_to_CPQ()
{
	while(1)
	{
		TaskPackage<TaskT> pkg;
		WaitDuringPause();
		bool status = task_pipeline_.cmq_pop_front(pkg);
		if(!status)
			break;

		vector<int>& dsts = pkg.dsts;
		for(int i = 0; i < dsts.size(); i++)
		{
			obinstream um = recv_obinstream(dsts[i], RESPOND_CHANNEL);
			int num;
			um >> num;
			for(int i=0; i<num; i++)
			{
				VertexT * v = new VertexT;
				um >> *v;
				cache_table_.set(v->id, v);
			}
		}
		WaitDuringPause();
		task_pipeline_.cpq_push_back(pkg.tasks);
		task_to_cpq_count_ += pkg.tasks.size();
		vtx_resp_count_++;

		if((vtx_req_count_-vtx_resp_count_)<VTX_REQ_MAX_GAP)
		{
			std::unique_lock<std::mutex> lck(vtx_req_lock_);
			vtx_req_cond_.notify_one();
		}
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::pull_CPQ_to_taskbuf(int tid)
{
	while(1)
	{
		TaskT* t;
		WaitDuringPause();
		bool status = task_pipeline_.cpq_pop_front(t);
		if(!status)
			break;

		vector<int> & reqs = t->to_request;
		AdjVtxList & nbs = t->to_pull;
		AdjVtxList refs;
		for(int j=0; j< reqs.size(); j++)
		{
			refs.push_back(nbs[reqs[j]]);
		}
		t->to_request.clear();

		t->task_counter_ = GetAndIncreaseCounter();
		IncreaseComputingTaskCount();
		t = recursive_compute(t, tid);
		DecreaseComputingTaskCount();

		commun_lock_.lock();
		//clear the items in cache with refs
		for(int j = 0 ; j < refs.size(); j++)
		{
			cache_table_.dec_item_ref(refs[j].id);
		}

		if(cache_overflow_ && (!resume_task_))
		{
			cache_overflow_ = false;
			cache_table_.resize(cache_size_);
		}
		commun_lock_.unlock();
		commun_cond_.notify_one();

		WaitDuringPause();
		compute_lock_.lock();
		if(t != NULL)
		{
			if (t->resume_task_) {
				TaskVec tasks;
				tasks.push_back(t);
				vector<KTpair> signed_tasks;
				task_sorter_.sign_and_sort_tasks(tasks, signed_tasks);
				task_pipeline_.pq_push_back(signed_tasks);
			} else {
				task_pipeline_.taskbuf_push_back(t);
				task_recycle_count_++;
			}
		}
		else
		{
			dec_task_num_in_memory(1);
			task_finished_count_++;
		}
		compute_lock_.unlock();
		compute_cond_.notify_one();
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::run_to_no_task()
{
	//check whether there is any task in system
	while(get_task_num_in_memory() || get_task_num_in_disk())
	{
		{
			unique_lock<mutex> lck(compute_lock_);
			while(task_pipeline_.taskbuf_size() < global_taskBuf_size && get_task_num_in_disk())
			{
				compute_cond_.wait(lck);
			}
		}
		TaskVec tasks;
		vector<KTpair> signed_tasks;
		task_pipeline_.taskbuf_content(tasks);
		task_sorter_.sign_and_sort_tasks(tasks, signed_tasks);
		task_pipeline_.pq_push_back(signed_tasks);
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::recv_run()
{
	int num_end_tag = 0;
	queue<ibinstream> streams;
	queue<MPI_Request> requests;
	while(num_end_tag < (_num_workers-1))
	{
		obinstream um = recv_obinstream(MPI_ANY_SOURCE, REQUEST_CHANNEL);
		streams.push(ibinstream());
		int dst = set_resp(streams.back(), um);
		if(dst < 0)
			num_end_tag ++;
		else{
			requests.push(MPI_Request());
			send_ibinstream_nonblock(streams.back(), dst, RESPOND_CHANNEL, requests.back());
		}

		if(requests.size() > 0){
			int test_flag;
			MPI_Test(&requests.front(), &test_flag, MPI_STATUS_IGNORE);
			if(test_flag){
				streams.pop();
				requests.pop();
			}
		}
	}
	while(requests.size() > 0){
		MPI_Wait(&(requests.front()), MPI_STATUS_IGNORE);
		requests.pop();
		streams.pop();
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::end_recver()
{
	ibinstream m;
	set_end_tag(m);
	for(int i = 0; i< _num_workers; i++)
	{
		int dst = (_my_rank + i) % _num_workers; //to avoid receiver-sided bottleneck
		if( dst != MASTER_RANK)
			send_ibinstream(m, dst, REQUEST_CHANNEL);
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::report_to_master(unsigned long long mem_tasks, unsigned long long disk_tasks)
{
	vector<unsigned long long> progress;
	progress.push_back(_my_rank);
	progress.push_back(mem_tasks);
	progress.push_back(disk_tasks);
	send_data(progress, MASTER_RANK, SCHEDULE_HEARTBEAT_CHANNEL);
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::report()
{
	while (!report_end_)
	{
		report_to_master(get_task_num_in_memory(), get_task_num_in_disk());
		sleep(COMMUN_TIME);
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::end_report()
{
	report_to_master(-1,-1);
	report_end_ = true;
	send_data(DONE, MASTER_RANK, MSCOMMUN_CHANNEL);
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::sys_sync()
{
	SysSyncGatherInfoT sys_sync_info;

	//basic queue info
	sys_sync_info.queue.task_num_in_memory = get_task_num_in_memory();
	sys_sync_info.queue.task_num_in_disk = get_task_num_in_disk();
	sys_sync_info.queue.cmq_size = task_to_cmq_count_ - task_to_cpq_count_;
	sys_sync_info.queue.cpq_size = task_pipeline_.cpq_size() + computing_task_count_;
	sys_sync_info.queue.taskbuf_size = task_pipeline_.taskbuf_size();
	sys_sync_info.queue.task_store_to_cmq = task_to_cmq_count_ - last_task_to_cmq_;
	sys_sync_info.queue.cmq_to_cpq = task_to_cpq_count_ - last_task_to_cpq_;
	sys_sync_info.queue.cpq_to_task_store = task_recycle_count_ - last_task_recycle_;
	sys_sync_info.queue.cpq_finished = task_finished_count_ - last_task_finished_;

	last_task_finished_ = task_finished_count_;
	last_task_recycle_ = task_recycle_count_;
	last_task_to_cmq_ = task_to_cmq_count_;
	last_task_to_cpq_ = task_to_cpq_count_;

	sys_sync_info.status.stealing_finished = stealing_finished_;

	slave_gather(sys_sync_info);
	
	AggregatorT* agg = (AggregatorT*)get_aggregator();
	if (agg != NULL)
	{
		if(!agg->sys_agg_disabled())
		{
			agg_lock_.lock();
			PartialT* part = agg->finish_partial();
			agg_lock_.unlock();
			//gathering
			MPI_Barrier(MPI_COMM_WORLD);
			slave_gather(*part);
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}

	SysSyncBcastInfoT bcast_info;
	slave_bcast(bcast_info);
	global_stealing_finished_ = bcast_info.global_stealing_finished;
	resume_task_ = bcast_info.resume_task;
	to_pause_ = bcast_info.to_pause;

	thread_demo_str_period();

	if(resume_task_)
		resume_signal_cond_.notify_one();

	if (!to_pause_)
		NotifyWhenPauseFinished();

	sys_sync_time_++;
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::agg_sync()
{
	AggregatorT* agg = (AggregatorT*)get_aggregator();
	if (agg != NULL)
	{
		agg_lock_.lock();
		PartialT* part = agg->finish_partial();
		agg_lock_.unlock();
		slave_gather(part);
		FinalT final;
		slave_bcast(final);
		*((FinalT*)global_agg) = final;
	}
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::context_sync()
{
	AggregatorT* agg = (AggregatorT*)get_aggregator();
	if(AGG_SLEEP_TIME == 0.0 && SYS_SLEEP_TIME == 0.0)
	{
		//do agg_sync when all the tasks have been processed
		unique_lock<mutex> lck(end_lock_);
		while (!is_end_)
		{
			end_cond_.wait(lck);  //block the thread until end_cond is notified.
		}
	}
	else if(SYS_SLEEP_TIME == 0.0)//just agg_sync
	{
		while (!all_land(is_end_))  //do agg_sync periodically when at least one worker is still computing
		{
			this_thread::sleep_for(chrono::nanoseconds(uint64_t(AGG_SLEEP_TIME * (1000 * 1000 * 1000))));
			if(!agg->agg_sync_disabled())
				agg_sync();
		}
	}
	else if(AGG_SLEEP_TIME == 0.0)
	{
		while (!all_land(is_end_))  //do agg_sync periodically when at least one worker is still computing
		{
			this_thread::sleep_for(chrono::nanoseconds(uint64_t(SYS_SLEEP_TIME * (1000 * 1000 * 1000))));
			sys_sync();
		}
	}
	else
	{
		//do both sys_sync and agg_sync
		//if all worker runs on the same model of CPU, then the sleep_counter counter will be identical
		double sleep_counter = 0.0;
		double last_sys_sync = 0.0;
		double last_agg_sync = 0.0;
		double time_to_sleep = 0.0;

		while (!all_land(is_end_))
		{
			//decide how long to sleep, and which to run
			if(last_sys_sync - last_agg_sync > AGG_SLEEP_TIME - SYS_SLEEP_TIME)
			{
				//run agg
				last_agg_sync += AGG_SLEEP_TIME;

				time_to_sleep  = last_agg_sync - sleep_counter;
				this_thread::sleep_for(chrono::nanoseconds(uint64_t(time_to_sleep * (1000 * 1000 * 1000))));
				MPI_Barrier(MPI_COMM_WORLD);
				if(!agg->agg_sync_disabled())
					agg_sync();

				sleep_counter = last_agg_sync;
			}
			else
			{
				//run sys
				last_sys_sync += SYS_SLEEP_TIME;

				time_to_sleep  = last_sys_sync - sleep_counter;
				this_thread::sleep_for(chrono::nanoseconds(uint64_t(time_to_sleep * (1000 * 1000 * 1000))));
				MPI_Barrier(MPI_COMM_WORLD);
				sys_sync();

				sleep_counter = last_sys_sync;
			}
		}
	}
	agg_sync();
	sys_sync();
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::end_sync()
{
	if (debug_)
		printf("slave %d calls end_sync\n", _my_rank);
	if(AGG_SLEEP_TIME == 0.0)
	{
		end_lock_.lock();
		is_end_ = true;
		end_lock_.unlock();
		end_cond_.notify_all();
	}
	else
	{
		is_end_ = true;
	}
	resume_signal_cond_.notify_one();
}


template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::debug()
{
	while(!is_end_)
	{
		string to_print;

		to_print +=  to_string(_my_rank) + " => Tasks in MEM = " + to_string(get_task_num_in_memory()) \
			 + " | Tasks in DISK = " + to_string(get_task_num_in_disk()) \
			 + " | CommunQ.size = " + to_string(task_pipeline_.cmq_size()*PIPE_POP_NUM) \
			 + " | ComputeQ.size = " + to_string(task_pipeline_.cpq_size()) \
			 + " | TaskBuff.size = " + to_string(task_pipeline_.taskbuf_size()) + "\n";

		// printf(to_print.c_str());

		sleep(10);
	}
}

// Send information to the Master,
// and the Master will write the signal file.
template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::WriteSignalFile()
{
	bool to_exit = false;
	slave_bcast(to_exit);

	if(to_exit)
		exit(0);

	int name_len;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(hostname, &name_len);

	string hn_s(hostname);

	slave_gather(hn_s);
}

template <class TaskT,  class AggregatorT>
bool Slave<TaskT, AggregatorT>::CheckIfGlobalStealingFinished()
{
	return true;
	bool global_stealing_finished = global_stealing_finished_;
	this_thread::sleep_for(chrono::milliseconds(10));
	return global_stealing_finished;
}

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::WaitingForGlobalStealingFinished()
{
	while (true) {
		bool global_stealing_finished = global_stealing_finished_;
		if (global_stealing_finished)
			break;

		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

//PART 1 =======================================================
//public run

template <class TaskT,  class AggregatorT>
void Slave<TaskT, AggregatorT>::run(const WorkerParams& params)
{
	WriteSignalFile();

	bool start = wait_to_start();
	if(!start)
	{
		cout << _my_rank << "=> Error: Failed in parse input data." << endl;
		return;
	}

	//================ Load graph from HDFS =================
	init_timers();
	ResetTimer(WORKER_TIMER);

	string rank = to_string(_my_rank);
	string myfile = params.input_path + "/part_" + rank;
	load_graph(myfile.c_str());

	StopTimer(WORKER_TIMER);
	PrintTimer("Load Time", WORKER_TIMER);
	//=======================================================

	//============== Check local disk path ==================
	PQUE_DIR = params.local_root+ "/" + rank + "/queue";
	MERGE_DIR = params.local_root+ "/" + rank + "/merge";

	//check disk folder
	check_dir(PQUE_DIR, params.force_write);
	check_dir(MERGE_DIR, params.force_write);
	//=======================================================

	//======================== init =========================
	AggregatorT* agg = (AggregatorT*)get_aggregator();
	if (agg != NULL)
		agg->init();
	cache_table_.init(cache_size_);
	task_sorter_.init(global_sign_size, slave_all_sum(local_table_.size()));
	task_pipeline_.init(PQUE_DIR, global_file_size);

	get_running_wtime();
	//=======================================================

	//========================= RUN =========================
	//start the sync thread for each process
	thread sync(&Slave::context_sync, this);

	//start listening thread for vertex passing
	thread recver(&Slave::recv_run, this);

	//start master-slaves task_schedule report
	thread reporter(&Slave::report, this);

	//for debugging
	thread debugger(&Slave::debug, this);

	// for demo resume
	thread demo_resumer(&Slave::demo_resume_handle, this);
	//seeding tasks in parallel
	local_table_.load();

	thread_demo_str_init();

	ResetTimer(WORKER_TIMER);
	for( unsigned i = 0; i < threadpool_size_; ++i )
		threadpool_.emplace_back(std::thread(&Slave::grow_tasks, this, i));
	for( auto &t : threadpool_ ) t.join();
	threadpool_.clear();
	StopTimer(WORKER_TIMER);
	cout << _my_rank << ": Task Seeding is Finished. | Total Time: " << get_timer(WORKER_TIMER) << endl;

	//Merging the tasks in a global sorted order;
	ResetTimer(WORKER_TIMER);
	task_sorter_.merge_sort_seed_tasks(task_pipeline_.get_pq_instance(), merged_file_num_);
	StopTimer(WORKER_TIMER);
	cout << _my_rank << ": Task Merging is Finished. | Total Time: " << get_timer(WORKER_TIMER) << endl;

	//Task_Pipline
	thread reqThread(&Slave::pull_PQ_to_CMQ, this);
	thread respThread(&Slave::pull_CMQ_to_CPQ, this);
	for( unsigned i = 0; i < threadpool_size_; ++i )
		threadpool_.emplace_back(std::thread(&Slave::pull_CPQ_to_taskbuf, this, i));
	
	run_to_no_task();

	cout << _my_rank << ": Regular Work Pipeline is Done, Start Task Stealing Stage." << endl;


	//Task Stealing Step
	queue<ibinstream> streams;
	queue<MPI_Request> requests;
	while(1)
	{
		send_data(get_worker_id(), MASTER_RANK, SCHEDULE_REPORT_CHANNEL);
		int tgt_wid = recv_data<int>(MASTER_RANK, SCHEDULE_REPORT_CHANNEL);
		if(tgt_wid == NO_WORKER_BUSY)
		{
			break;
		}
		else
		{
			//request a task from the corresponding worker
			//and
			//receive the task, recompute the remote vertices and do computation
			streams.push(ibinstream());
			requests.push(MPI_Request());
			set_steal(streams.back());
			send_ibinstream_nonblock(streams.back(), tgt_wid, REQUEST_CHANNEL, requests.back());

			TaskVec tasks;
			int num;

			obinstream um = recv_obinstream(tgt_wid, RESPOND_CHANNEL);
			um >> num;
			for(int i=0; i<num; i++)
			{
				TaskT * t = new TaskT;
				um >> *t;
				tasks.push_back(t);
			}
			inc_task_num_in_memory(num);
			vector<KTpair> signed_tasks;
			task_sorter_.sign_and_sort_tasks(tasks, signed_tasks);
			task_pipeline_.pq_push_back(signed_tasks);
			run_to_no_task();

			if(requests.size() > 0){
				int test_flag;
				MPI_Test(&requests.front(), &test_flag, MPI_STATUS_IGNORE);
				if(test_flag){
					streams.pop();
					requests.pop();
				}
			}
		}
	}
	stealing_finished_ = true;

	WaitingForGlobalStealingFinished();

	//close the queues in task_pipeline
	task_pipeline_.close();

	//end the pipeline threads
	reqThread.join();
	respThread.join();
	for( auto &t : threadpool_ ) t.join();
	threadpool_.clear();

	//end the computation
	end_recver();
	end_report();
	end_sync();

	//join the left threads
	recver.join();
	reporter.join();
	sync.join();
	thread_demo_str_finalize();

	//join the debugger threads
	debugger.join();
	demo_resumer.join();
}
