

#ifndef BLIZZARD_NETWORK
#define BLIZZARD_NETWORK

#include "blizzard_transaction_conflict.h"
#include "blizzard_broker.h"
#include "blizzard_node.h"
#include "avalanche_node.h"


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



class Network
{
public:	
	int num_nodes;
	int num_brokers;
	int k_num;//k is num of connections
	int security_param_beta_1;
	int security_param_beta_2;
	vector<Avalanche_Node*> list_nodes_AVALANCHE;
	vector<Avalanche_Node*>::iterator ptr_node_AVALANCHE;

	vector<Node*> list_nodes;
	vector<Node*>::iterator ptr_node;

	vector<Broker*> list_brokers;
	vector<Broker*>::iterator ptr_brkr;
	map<int, Broker*> ID_to_brkr_obj_map;
	map<int, Node*> ID_to_node_obj_map;
	map<int, Avalanche_Node*> ID_to_node_AVALANCHE_obj_map;

	map<int, double> ID_transaction_latency;

	map<int, double> get_latency(string);
	void node_broker_matching(void); // for blizzard only
	void node_matching(void); // for avalanche only
	void network_propagate(Transaction*,Node*);
	void network_propagate_AVALANCHE(Transaction*,Avalanche_Node*);
	void run_network(void);
	void run_network_AVALANCHE(void);
	void print(string,string);

	map<int, double> Avalanche_Latency(int);
	map<int, double> Blizzard_Latency(int);

	bool check_if_a_node_connected_to_all_nodes(int);
	bool check_connectivity_is_OK(void);
	Network(int input_num_nodes,int input_num_brokers, int input_k_num, int beta_1,int beta_2){
		num_nodes = input_num_nodes;
		num_brokers = input_num_brokers;
		k_num = input_k_num;
		security_param_beta_1 = beta_1;
		security_param_beta_2 = beta_2;
		double random_val;
		int coeff_random = 5;
		int width_random = 100;

		vector< vector<float> > node_to_broker_comm;
		vector< vector<float> > broker_to_node_comm;

		/*setting communication delay FROM node To broker*/
		for (int i = 0; i < num_nodes; i++)
	    {
	    	vector<float> temp_1;	
	        for (int j = 0; j < num_brokers; j++)
	        {
	        	random_val = ((rand() % coeff_random + 1)*1.0- 1.0*coeff_random/2.0)/(1.0*width_random);
	            temp_1.push_back(0.1+random_val);
	        }   
	    	node_to_broker_comm.push_back(temp_1);
	    	temp_1.clear();    
	    }	
	    /*setting communication delay FROM broker To node*/
		for (int i = 0; i < num_brokers; i++)
	    {
	    	vector<float> temp_2;	
	        for (int j = 0; j < num_nodes; j++)
	        {
	        	random_val = ((rand() % coeff_random + 1)*1.0- 1.0*coeff_random/2.0)/(1.0*width_random);
	            temp_2.push_back(0.1+random_val);
	        }   
	    	broker_to_node_comm.push_back(temp_2);
	    	temp_2.clear();    
	    }	



	    /* create nodes */
		for (int i=0; i < num_nodes; i++){
			// numbers of beta1 (conf) 11
			Node* node = new Node(i,security_param_beta_1,security_param_beta_2,node_to_broker_comm[i]);
			//cout << " NETWORK makde node " << node->id << " has size " << node->comm_to_broker.size() <<endl; 
			
			node->id = i;
			list_nodes.push_back(node);
			ID_to_node_obj_map.insert(pair<int,Node*>(i,node));

		}
		/* create brokers */
		for (int i=0; i < num_brokers; i++){
			Broker* broker = new Broker(i,broker_to_node_comm[i]);
			broker->id = i;
			list_brokers.push_back(broker);
			ID_to_brkr_obj_map.insert(pair<int,Broker*>(i,broker));
		}


		/*********  AVALANCHE ***********/
		vector< vector<float> > node_to_node_comm;
		/*setting communication delay FROM node To node*/
		for (int i = 0; i < num_nodes; i++)
	    {
	    	vector<float> temp_3;	
	        for (int j = 0; j < num_nodes; j++)
	        {
	        	random_val = ((rand() % coeff_random + 1)*1.0- 1.0*coeff_random/2.0)/(1.0*width_random);
	            temp_3.push_back(0.1+random_val);
	        }   
	    	node_to_node_comm.push_back(temp_3);
	    	temp_3.clear();    
	    }
		/* create AVALANCHE nodes */
		for (int i=0; i < num_nodes; i++){
			// numbers of beta1 (conf) 11
			Avalanche_Node* Node = new Avalanche_Node(i,security_param_beta_1,security_param_beta_2,node_to_node_comm[i]);
			//cout << " NETWORK makde node " << Node->id << " has size " << Node->comm_to_node.size() <<endl; 
			
			Node->id = i;
			list_nodes_AVALANCHE.push_back(Node);
			ID_to_node_AVALANCHE_obj_map.insert(pair<int,Avalanche_Node*>(i,Node));

		}

	}  
};

#endif

