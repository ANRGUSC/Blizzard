#ifndef BLIZZARD_BROKER
#define BLIZZARD_BROKER

#include "blizzard_transaction_conflict.h"
//#include "blizzard_node.h"

#include <iostream>
#include <string>
#include <set>
#include <stack>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <vector>
#include <map>

using namespace std;
class Node;
//class Broker;
class Broker
{
public:	
	int id;
	set<int> id_connected_nodes;
	set<Node*> connected_nodes;
	//Node* n1;
	set <Transaction*> OBJ_transactions; // set <Transaction*> transactions;
	set <Transaction*> OBJ_known_transactions; //set <Transaction*> known_transactions

	set <int> transactions; // set <Transaction*> transactions;
	set <int> known_transactions; //set <Transaction*> known_transactions

	map<int, bool> table_queried_transactions; // (id Tx,bool)
	vector<float> comm_to_node; 
	map<int, float> table_propagation_transactions_time;// (id Tx,time)

	void set_a_new_connection(int, Node*);
	void delete_all_connections();
	void propagate(Transaction*);
	bool query(Transaction*);
	void print();
	bool operator < (const Broker & t) const
	{
		return (this->id < t.id);
	}
	Broker(int input_id, vector<float> comm_delay_Broker_to_Node){
		id = input_id;
		comm_to_node =  comm_delay_Broker_to_Node;
		// for (int i=0;i!=comm_to_node.size();i++){
		// 	cout <<"B " << id << " " << comm_to_node[i] <<endl;
		// }
	}
	// bool query_node(Node*);
	// bool query_nodes();
	//void query_all_connected_brokers(Transaction*,Transaction*,vector<Node*>);

};


#endif