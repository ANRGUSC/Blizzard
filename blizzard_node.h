#ifndef BLIZZARD_NODE
#define BLIZZARD_NODE

#include "blizzard_transaction_conflict.h"
//#include "blizzard_broker.h"



#include <iostream>
#include <string>
#include <set>
#include <stack>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <vector>
#include <map>
#include <tuple>

using namespace std;
class Broker;
//class Node;
class Node
{
public:
	set <Transaction*> OBJ_transactions; // set <Transaction*> transactions;
	set <Transaction*> OBJ_known_transactions; //set <Transaction*> known_transactions

	set <int> transactions; // set <Transaction*> transactions;
	set <int> known_transactions; //set <Transaction*> known_transactions
	Transaction* genesis_tx; // everynode should know the global transaction

	set<int> id_connected_brokers;
	//Broker* b2;
	set<Broker*> connected_brokers;
	int id;
	int beta1;
	int beta2;
	double time_spent_updating_DAG;

	//tuple <int,int,Transaction*>  tuple_known_transactions;
	//map<Transaction*, bool> table_transactions_sent_to_connected_brkrs;
	//Transaction* genesis_tx;

	//map<Transaction*, float> table_propagation_transactions_time;// table for time taking to check is Strongly Prefered 
	map<int, bool> table_transactions_isStronglyPref; // (id Tx,bool)

	vector<float> comm_to_broker; 
	map<int, float> table_propagation_transactions_time; // table for time taken to receive Txs (id TX, time to receive)
	map<Transaction*, float> table_isStrongly_transactions_time; // table for time taken to check isStrongly for Txs
	map<Transaction*, float> table_query_AND_append_transactions_time; 

	map<int, Transaction*> table_ID_transaction;


	map<int, double> ID_transaction_latency;
	conflicts_dict conflicts;	

	set<Transaction*> set_finalized_transactions;

	void receive_tx(Transaction*);
	
	bool isStorngly_Pref(Transaction*);
	bool preprocessing(Transaction*);
	bool is_accepted(Transaction*);
	void update_Dag_AND_conflictSets_as_attaching_new_tx(Transaction*);
	//void receive_tx(Transaction*,Transaction*);
	void node_issue_transaction(Transaction*);
	
	bool has_node_propagated(int);
	// bool query_broker(Broker*);
	// bool query_brokers();
	void query(Transaction*);
	bool run_node(void);
	//void query_all_connected_brokers(Transaction*,Transaction*,vector<Broker*>);

	void print_node(void);
	void set_a_new_connection(int, Broker*);
	void get_latency_transactions(void);
	void delete_all_connections();
	bool operator < (const Node & t) const
	{
		return (this->id < t.id);
	}
	Node(int input_id, int beta_conf, int beta_cons, vector<float> comm_delay_Node_to_Broker){
		id = input_id;
		beta1 = beta_conf;
		beta2 = beta_cons;
		comm_to_broker = comm_delay_Node_to_Broker;
		time_spent_updating_DAG = 0.0;
		genesis_tx = NULL;
		// for (int i=0;i!=comm_to_broker.size();i++){
		// 	cout <<"N " << id << " " << comm_to_broker[i] <<endl;
		// }
	}
};


#endif