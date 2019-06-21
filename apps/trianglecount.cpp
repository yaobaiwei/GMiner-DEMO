//Copyright 2018 Husky Data Lab, CUHK
//Authors: Hongzhi Chen, Miao Liu


#include "core/subg-dev.hpp"

using namespace std;

struct TriangleContext
{
	int count;
	VertexID last_id;

	VertexID creator_id;
};

ibinstream& operator << (ibinstream& m, const TriangleContext& v)
{
	m << v.count;
	m << v.last_id;
	m << v.creator_id;
	return m;
}

obinstream& operator >> (obinstream& m, TriangleContext& v)
{
	m >> v.count;
	m >> v.last_id;
	m >> v.creator_id;
	return m;
}

ifbinstream& operator << (ifbinstream& m, const TriangleContext& v)
{
	m << v.count;
	m << v.last_id;
	m << v.creator_id;
	return m;
}

ofbinstream& operator >> (ofbinstream& m, TriangleContext & v)
{
	m >> v.count;
	m >> v.last_id;
	m >> v.creator_id;
	return m;
}


class CountAgg :public Aggregator<TriangleContext, unsigned long long, unsigned long long>
{
public:

	virtual ~CountAgg() {}

	virtual void init()
	{
		count_ = 0;
	}

	virtual void step_partial(TriangleContext&  v)
	{
		count_ += v.count;
	}

	virtual void step_final(unsigned long long* part)
	{
		count_ += *part;
	}

	virtual unsigned long long* finish_partial()
	{
		return &count_;
	}
	virtual unsigned long long* finish_final()
	{
		return &count_;
	}

	string get_agg_str(const vector<unsigned long long>& parts) override
	{
		unsigned long long total_parts = 0;

		for(auto v : parts)
		{
			total_parts += v;
		}

		char c[64];

		sprintf(c, "%ld", total_parts);

		return string(c);
	}

	string app_name() override
	{
		return "TC";
	}

	bool agg_sync_disabled() override
	{
		return true;
	}
	
	bool sys_agg_disabled() override
	{
		return false;
	}

	string sys_print_header() override
	{
		return "Current triangle count: ";
	}


private:
	unsigned long long count_;
};


class TriangleTask :public Task<VertexID, TriangleContext>
{
public:
	static int sample_min_, sample_max_;

	virtual bool compute(SubgraphT & g, ContextType & context, vector<VertexT *> & frontier)
	{
		VertexT last_v;
		last_v.id = context.last_id;
		frontier.push_back(&last_v);
		int size = frontier.size();

		for (int i = 0; i < size - 1; i++)
		{
			AdjVtxList & adj = frontier[i]->get_adjlist();
			int size_i = 0;  //pos in frontier[i]’s adj to search next
			int size_j = adj.size();
			for (int j = i + 1; j < size; j++)
			{
				VertexID id_j = frontier[j]->id;  ////frontier[j]’s ID
				while ((size_i < size_j) && (adj[size_i].id < id_j))
					size_i++;
				if (size_i >= size_j)
					break;

				//my neighbor is my neighbor's neighbor
				if (adj[size_i].id == id_j)
				{
					//the triangle is: frontier[i], context.creator_id, id_j
					context.count++;
					size_i++;
				}
			}
		}

		if(if_filtered_for_demo())
		{
			frontier_to_dump_ = &frontier;
		}

		return false;
	}

	vector<VertexT *>* frontier_to_dump_;

	bool if_filtered_for_demo() override
	{
        if(resume_task_)
            return true;
		if(context.count >= sample_min_ && context.count <= sample_max_)
		{
			return true;
		}
		return false;
	}

	void dump_context_for_demo() override
	{
		//filter
		if(!resume_task_ && filtered_task_counter_ % 20 != 0)
		{
			return;
		}

		vector<VertexT *> & frontier = *frontier_to_dump_;
		SubgraphT & g = subG;

		int size = frontier.size();

		demo_str_ = "{\"seed_id\":" + to_string(this->seed_key);
		demo_str_ += ",\"subg\":[";
		unsigned long long count = 0;

		map<VertexID, set<VertexID>> edges_map;

		edges_map[context.creator_id] = set<VertexID>();

		for (int i = 0; i < size - 1; i++)
		{
			AdjVtxList & adj = frontier[i]->get_adjlist();
			int size_i = 0;  //pos in frontier[i]’s adj to search next
			int size_j = adj.size();
			for (int j = i + 1; j < size; j++)
			{
				VertexID id_j = frontier[j]->id;  ////frontier[j]’s ID
				while ((size_i < size_j) && (adj[size_i].id < id_j))
					size_i++;
				if (size_i >= size_j)
					break;

				//my neighbor is my neighbor's neighbor
				if (adj[size_i].id == id_j)
				{
					string to_append;

					if(count != 0)
					{
						to_append = ",[";
					}
					else
					{
						to_append = "[";
					}

					to_append += to_string(context.creator_id) + ",";
					to_append += to_string(frontier[i]->id) + ",";
					to_append += to_string(id_j);
					to_append += "]";
					demo_str_ += to_append;

					if(edges_map.count(frontier[i]->id) == 0)
						edges_map[frontier[i]->id] = set<VertexID>();
					if(edges_map.count(id_j) == 0)
						edges_map[id_j] = set<VertexID>();

					edges_map[context.creator_id].insert(frontier[i]->id);
					edges_map[context.creator_id].insert(id_j);
					edges_map[frontier[i]->id].insert(id_j);
					//the triangle is: frontier[i], context.creator_id, id_j
					count++;
					size_i++;
				}
			}
		}

		demo_str_ += "], \"count\" : " + to_string(count);
		context.count = count;
		if (!if_filtered_for_demo())
		{
			demo_str_ = "";
			return;
		}

		list<NodeT> & nodes = subG.get_nodes();

		//subG node list
		int tmp_cnt = 0;
		int nodes_sz = edges_map.size();
		vector<pair<VertexID, VertexID>> edges;

		demo_str_ += ", \"subg_size\" : " + to_string(nodes_sz) + ", \"subg_list\" : [";
		for (auto edge_kv : edges_map)
		{
			demo_str_ += to_string(edge_kv.first);
			if(tmp_cnt != nodes_sz - 1)
			{
				demo_str_ += ",";
			}
			tmp_cnt++;

			for(auto dst_id : edge_kv.second)
			{
				edges.push_back(make_pair(edge_kv.first, dst_id));
			}
		}
		demo_str_ += "], \"conn_list\" : [";

		for(int i = 0; i < edges.size(); i++)
		{
			demo_str_ += "[" + to_string(edges[i].first) + "," + to_string(edges[i].second) + "]";
			if(i != edges.size() - 1)
			{
				demo_str_ += ",";
			}
		}

		demo_str_ += "], \"conn_size\" : " + to_string(edges.size());
		demo_str_ += ", \"task_id\" : " + to_string(task_counter_);

		demo_str_ += "}\n";
	}

};

int TriangleTask::sample_min_ = 4;
int TriangleTask::sample_max_ = 10;

class TriangleSlave :public Slave<TriangleTask, CountAgg>
{
public:

	virtual VertexT* respond(VertexT * v)
	{
		KeyT vid = v->id;

		VertexT * tmp = new VertexT;
		tmp->id = vid;

		AdjVtxList & adjlist = v->get_adjlist();
		AdjVtxIter v_iter = adjlist.begin();
		while ((v_iter < adjlist.end()) && (v_iter->id < vid))
			v_iter++;
		tmp->adjlist.insert(tmp->adjlist.end(), v_iter, adjlist.end());

		return tmp;
	}

	virtual TriangleTask * create_task(VertexT * v)
	{
		AdjVtxList & adjlist = v->get_adjlist();
		if (adjlist.size() <= 1)
			return NULL;

		AdjVtxList candidates; //the vertexes to be pulled in the next round
		VertexID vid = v->id;
		AdjVtxIter vIter = adjlist.begin();
		while ((vIter < adjlist.end() - 1) && (vIter->id <= vid))
			vIter++;

		candidates.insert(candidates.end(), vIter, adjlist.end() - 1);
		if (candidates.size() >= 1)
		{
			TriangleTask * task = new TriangleTask;
			task->pull(candidates);
			task->context.count = 0;
			task->context.last_id = adjlist[adjlist.size() - 1].id;
			task->context.creator_id = v->id;
			task->seed_key = v->id;
			return task;
		}
		return NULL;
	}

	virtual VertexT* to_vertex(char* line)
	{
		VertexT* v = new VertexT;
		char * pch;
		pch = strtok(line, " ");
		v->id = atoi(pch);
		strtok(NULL, "\t");
		while ((pch = strtok(NULL, " ")) != NULL)
		{
			AdjVertex item;
			item.id = atoi(pch);
			pch = strtok(NULL, " ");
			item.wid = atoi(pch);
			v->adjlist.push_back(item);
		}
		sort(v->adjlist.begin(), v->adjlist.end());

		return v;
	}
};


class TriangleMaster :public Master<CountAgg>
{
public:

	virtual void print_result()
	{
		unsigned long long* agg = (unsigned long long*)get_agg();
		cout << "The sum of all triangles is " << *agg << endl;
	}
};

class TriangleWorker :public Worker<TriangleMaster, TriangleSlave, CountAgg> {};


int main(int argc, char* argv[])
{
	const char* TC_SAMPLING_MIN = getenv("TC_SAMPLING_MIN");
	const char* TC_SAMPLING_MAX = getenv("TC_SAMPLING_MAX");
	if (TC_SAMPLING_MIN != nullptr)
	{
		TriangleTask::sample_min_ = atoi(TC_SAMPLING_MIN);
	}
	if (TC_SAMPLING_MAX != nullptr)
	{
		TriangleTask::sample_max_ = atoi(TC_SAMPLING_MAX);
	}

	init_worker(&argc, &argv);

	WorkerParams param;
	load_system_parameters(param);
	load_hdfs_config();

	TriangleWorker worker;
	CountAgg agg;
	worker.set_aggregator(&agg, 0);
	worker.run(param);

	worker_finalize();
	return 0;
}
