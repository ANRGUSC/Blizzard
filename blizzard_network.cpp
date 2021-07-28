
#include "blizzard_network.h"
//#include "blizzard_transaction_conflict.h"
#include <algorithm>
#include <set>
#include <iterator>
#include <vector>

using namespace std;
using namespace std::chrono; 

void Network::print(string state,string model){
/*    void Network::print(string state,string model){
 two possible options for state: either "transactions" or "connections"
 two possible options for model: either "blizzard" or "avalanche"	*/
	cout << "-----------------   Network Print ---------------------------"<<endl;
	if(state == "transactions"){
		if (model == "blizzard"){
			cout << "-----------   Transaction of nodes    ----------------" << endl;
			for (ptr_node= list_nodes.begin(); ptr_node!= list_nodes.end();ptr_node++){
				(*ptr_node)->print_node();
			}
			cout << " =================  Brokers ===============" <<endl;
			for (ptr_brkr= list_brokers.begin(); ptr_brkr!= list_brokers.end();ptr_brkr++){
				(*ptr_brkr)->print();
			}
		}else{
			cout << "-----------   Transaction of nodes    ----------------" << endl;
			for (ptr_node_AVALANCHE= list_nodes_AVALANCHE.begin(); ptr_node_AVALANCHE!= list_nodes_AVALANCHE.end();ptr_node_AVALANCHE++){
				(*ptr_node_AVALANCHE)->print_node();
			}
		}	
	}else{
		cout << "==============================================" <<endl;
		cout << "============ Network Connections =============" <<endl;
		cout << "==============================================" <<endl;
		cout << "Nodes connecting ..."<<endl;
		if (model == "blizzard"){

			for (ptr_node= list_nodes.begin(); ptr_node!= list_nodes.end();ptr_node++){
				cout << (*ptr_node)->id <<":";
				for (set<int>::iterator it=(*ptr_node)->id_connected_brokers.begin();it!=(*ptr_node)->id_connected_brokers.end();it++){
					cout << *it <<",";	
				}
				cout << endl;
			}
			cout << "Brokers connecting..."<<endl;
			for (ptr_brkr= list_brokers.begin(); ptr_brkr!= list_brokers.end();ptr_brkr++){
				cout << (*ptr_brkr)->id <<":";
				for (set<int>::iterator it=(*ptr_brkr)->id_connected_nodes.begin();it!=(*ptr_brkr)->id_connected_nodes.end();it++){
					cout << *it <<",";	
				}
				cout <<endl;
			}
		}else{
			for (ptr_node_AVALANCHE= list_nodes_AVALANCHE.begin(); ptr_node_AVALANCHE!= list_nodes_AVALANCHE.end();ptr_node_AVALANCHE++){
				cout << (*ptr_node_AVALANCHE)->id <<":";
				for (set<int>::iterator it=(*ptr_node_AVALANCHE)->id_connected_nodes.begin();it!=(*ptr_node_AVALANCHE)->id_connected_nodes.end();it++){
					cout << *it <<",";	
				}
				cout << endl;
			}
		}	
		cout << "==============================================" <<endl;	
	}

	cout << "-----------------END Network Print---------------------------"<<endl;
}

map<int, double> Network::get_latency(string model){
	/* calculate latency of all transactions at EACH node*/
	map<int, double> output;
	if(model=="blizzard"){
		/*BLIZZARD*/
		for (ptr_node= list_nodes.begin(); ptr_node!= list_nodes.end();ptr_node++){
			(*ptr_node)->get_latency_transactions();
		}

		
		Node* node = ID_to_node_obj_map.find(0)->second;
		for (set<Transaction*>::iterator it_transaction=(node->OBJ_known_transactions).begin();it_transaction!=(node->OBJ_known_transactions).end();it_transaction++){
			int ID_transaction = (*it_transaction)->id;
			for (ptr_node= list_nodes.begin(); ptr_node!= list_nodes.end();ptr_node++){
				/* if this node does NOT have it finalized, put -1 for this Tx in the final table*/
				if((*ptr_node)->ID_transaction_latency.find(ID_transaction)==(*ptr_node)->ID_transaction_latency.end()){
					output.insert(make_pair(ID_transaction,-1.0));
					break;
				}else{
					double node_latency_for_this = (*ptr_node)->ID_transaction_latency.find(ID_transaction)->second; 
					/*if it not inserted before, else is for it was already added*/
					if(output.find(ID_transaction)==output.end()){
						output.insert(make_pair(ID_transaction,node_latency_for_this));
					}else{
						output.find(ID_transaction)->second = max(output.find(ID_transaction)->second, node_latency_for_this);
					}
				}	
			}
		}
	}else{
		/*AVALANCHE*/
		for (ptr_node_AVALANCHE= list_nodes_AVALANCHE.begin(); ptr_node_AVALANCHE!= list_nodes_AVALANCHE.end();ptr_node_AVALANCHE++){
			(*ptr_node_AVALANCHE)->get_latency_transactions();
		}
		
		Avalanche_Node* node = ID_to_node_AVALANCHE_obj_map.find(0)->second;
		for (set<Transaction*>::iterator it_transaction=(node->OBJ_known_transactions).begin();it_transaction!=(node->OBJ_known_transactions).end();it_transaction++){
			int ID_transaction = (*it_transaction)->id;
			for (ptr_node_AVALANCHE= list_nodes_AVALANCHE.begin(); ptr_node_AVALANCHE!= list_nodes_AVALANCHE.end();ptr_node_AVALANCHE++){
				/* if this node does NOT have it finalized, put -1 for this Tx in the final table*/
				if((*ptr_node_AVALANCHE)->ID_transaction_latency.find(ID_transaction)==(*ptr_node_AVALANCHE)->ID_transaction_latency.end()){
					output.insert(make_pair(ID_transaction,-1.0));
					break;
				}else{
					double node_latency_for_this = (*ptr_node_AVALANCHE)->ID_transaction_latency.find(ID_transaction)->second; 
					/*if it not inserted before, else is for it was already added*/
					if(output.find(ID_transaction)==output.end()){
						output.insert(make_pair(ID_transaction,node_latency_for_this));
					}else{
						output.find(ID_transaction)->second = max(output.find(ID_transaction)->second, node_latency_for_this);
					}
				}	
			}
		}
	}
	map<int, double>::iterator it;
	cout << "--------- LATENCY ------------" <<endl;
	for(it=output.begin();it!=output.end();it++){
		cout << (it)->first << " , " << (it)->second <<endl;
	}
	return output;
}

void Network::node_broker_matching(){
// moodel is either "blizzard" or "avalanche"
	cout << "==============================================" <<endl;
	cout << "============ Network Connections: Blizzard  ==" <<endl;
	cout << "==============================================" <<endl;
	cout << "Nodes connecting ..."<<endl;
		
	//delete previous assignments
	for (ptr_node= list_nodes.begin(); ptr_node!= list_nodes.end();ptr_node++){
		(*ptr_node)->delete_all_connections();
	}
	for (ptr_brkr= list_brokers.begin(); ptr_brkr!= list_brokers.end();ptr_brkr++){
		(*ptr_brkr)->delete_all_connections();
	}	

	//srand((unsigned) time(0));
	for (ptr_node= list_nodes.begin(); ptr_node!= list_nodes.end();ptr_node++){
		while ((*ptr_node)->id_connected_brokers.size()!=k_num){
			int generated_random_number = (rand() % num_brokers);		// random connection	
			(*ptr_node)->set_a_new_connection(generated_random_number,ID_to_brkr_obj_map.find(generated_random_number)->second); // add this random broker id to set of connected brokers of this node
			(ID_to_brkr_obj_map.find(generated_random_number)->second)->set_a_new_connection((*ptr_node)->id,(*ptr_node)); // add the node to set of connected nodes for this broker as well
		}
		cout << (*ptr_node)->id <<":";
		for (set<int>::iterator it=(*ptr_node)->id_connected_brokers.begin();it!=(*ptr_node)->id_connected_brokers.end();it++){
			cout << *it <<",";	
		}
		cout << endl;
	}
	cout << "Brokers connecting..."<<endl;
	for (ptr_brkr= list_brokers.begin(); ptr_brkr!= list_brokers.end();ptr_brkr++){
		cout << (*ptr_brkr)->id <<":";
		for (set<int>::iterator it=(*ptr_brkr)->id_connected_nodes.begin();it!=(*ptr_brkr)->id_connected_nodes.end();it++){
			cout << *it <<",";	
		}
		cout <<endl;
	}	
	cout << "==============================================" <<endl;
}

void Network::network_propagate(Transaction* input_tx,Node* node){
	node->node_issue_transaction(input_tx);
	this->print("transactions","blizzard");
	bool no_more_propagation_by_nodes = false;
    while( !no_more_propagation_by_nodes){    
        bool all_nodes_have_propagated = true;
        for (ptr_node= list_nodes.begin(); ptr_node!= list_nodes.end();ptr_node++){
        	//cout << "Network Propagate - NODE "<< (*ptr_node)->id << "INFO---------------------"<<endl; 
        	if ((*ptr_node)->known_transactions.find(input_tx->id)!=(*ptr_node)->known_transactions.end()){
            	all_nodes_have_propagated &= (*ptr_node)->has_node_propagated(input_tx->id);
            }
        }    
        if(!all_nodes_have_propagated){ 
            break;  
        }    
    }        
}


void Network::run_network(){
	int m = 0;
    bool no_querry_any_more_by_nodes = false;
    while( !no_querry_any_more_by_nodes || m <1000000){    
        m += 1;
        bool all_nodes_have_empty_querried = false;
        for (ptr_node= list_nodes.begin(); ptr_node!= list_nodes.end();ptr_node++){
        	//cout << "Network running ... " ; 
            all_nodes_have_empty_querried |= (*ptr_node)->run_node();
        }    
        if(!all_nodes_have_empty_querried){ 
            break;  
        }    
    }        
}

map<int, double> Network::Blizzard_Latency(int number_of_transactions){
	this->node_broker_matching();
	Transaction* t1 = new Transaction(1,1,NULL);
	Node* node = this->ID_to_node_obj_map.find(0)->second;
	this->network_propagate(t1,node);
	this->print("transactions","blizzard");
	this->run_network();

	Transaction* old_tx = t1;
	for (int i=2; i < number_of_transactions+1; i++){
		this->node_broker_matching();
		Transaction* new_tx = new Transaction(i,i,old_tx);
		//int generated_random_number = (rand() % num_nodes);
		//node = network->ID_to_node_obj_map.find(generated_random_number)->second;
		this->network_propagate(new_tx,node);
		this->print("transactions","blizzard");
		this->run_network();
		this->print("transactions","blizzard");
		old_tx = new_tx;
	}
	this->print("connections","blizzard");
	return this->get_latency("blizzard");
}	

void Network::node_matching(){
/* for AVALANCHE only */
	for (ptr_node_AVALANCHE= list_nodes_AVALANCHE.begin(); ptr_node_AVALANCHE!= list_nodes_AVALANCHE.end();ptr_node_AVALANCHE++){
			(*ptr_node_AVALANCHE)->delete_all_connections();
	}
	for (ptr_node_AVALANCHE= list_nodes_AVALANCHE.begin(); ptr_node_AVALANCHE!= list_nodes_AVALANCHE.end();ptr_node_AVALANCHE++){
		while ((*ptr_node_AVALANCHE)->id_connected_nodes.size()!=k_num){
			int generated_random_number = (rand() % num_nodes);		// random connection
			// attach if it node itself!
			if(generated_random_number!= (*ptr_node_AVALANCHE)->id){	
				(*ptr_node_AVALANCHE)->set_a_new_connection(generated_random_number,ID_to_node_AVALANCHE_obj_map.find(generated_random_number)->second); // add this random node id to set of connected nodes of this node
			}
		}
		cout << (*ptr_node_AVALANCHE)->id <<":";
		for (set<int>::iterator it=(*ptr_node_AVALANCHE)->id_connected_nodes.begin();it!=(*ptr_node_AVALANCHE)->id_connected_nodes.end();it++){
			cout << *it <<",";	
		}
		cout << endl;
	}
	cout << "END MATCHING " <<endl;
}	


bool Network::check_if_a_node_connected_to_all_nodes(int node_id_source){
/* for AVALANCHE only */
	set <int> seen_id_nodes;
	vector<int> queue;
	Avalanche_Node* node_avalanche;
	int taken_out_from_queue;

	queue.push_back(node_id_source);
	seen_id_nodes.insert(node_id_source);
	while(!queue.empty()){
		taken_out_from_queue = queue.back();
		queue.pop_back();
		node_avalanche = ID_to_node_AVALANCHE_obj_map.find(taken_out_from_queue)->second;
		for (set<int>::iterator index_node= node_avalanche->id_connected_nodes.begin(); index_node != node_avalanche->id_connected_nodes.end();index_node++){
			if(seen_id_nodes.find(*index_node)==seen_id_nodes.end()){
				queue.push_back(*index_node);
				seen_id_nodes.insert(*index_node);
			}	
		}
	}
	if(seen_id_nodes.size()==list_nodes_AVALANCHE.size()){
		return true;
	}
	return false;
}

bool Network::check_connectivity_is_OK(){
	bool all_nodes_have_path_to_all_nodes = true;
    for (int index_node = 0; index_node!= num_nodes;index_node++){
        all_nodes_have_path_to_all_nodes &= this->check_if_a_node_connected_to_all_nodes(index_node);
        if(!all_nodes_have_path_to_all_nodes){
        	cout << "connectivity: FALSE "<< endl;
        	return false;
        }
    }
    cout << "connectivity: TRUE" << endl;
	return true;  
}

void Network::network_propagate_AVALANCHE(Transaction* input_tx,Avalanche_Node* node){
	node->node_issue_transaction(input_tx);
}

void Network::run_network_AVALANCHE(){
	int m = 0;
    bool no_querry_any_more_by_nodes = false;
    while( !no_querry_any_more_by_nodes || m <1000000){    
        m += 1;
        bool all_nodes_have_empty_querried = false;
        for (ptr_node_AVALANCHE= list_nodes_AVALANCHE.begin(); ptr_node_AVALANCHE!= list_nodes_AVALANCHE.end();ptr_node_AVALANCHE++){
        	// cout << "Network running ... " << (*ptr_node_AVALANCHE)->id <<endl ; 
            all_nodes_have_empty_querried |= (*ptr_node_AVALANCHE)->run_node();
        }    
        if(!all_nodes_have_empty_querried){ 
            break;  
        }    
    }        

}

map<int, double> Network::Avalanche_Latency(int number_of_transactions){
	srand((unsigned) time(0));
	bool status_well_connectivity;	
	this->node_matching();
	// make sure connectivity of all nodes are fine
	status_well_connectivity = this->check_connectivity_is_OK();
	while(!status_well_connectivity){
		this->node_matching();
		status_well_connectivity = this->check_connectivity_is_OK();
	}

	Transaction* t1 = new Transaction(1,1,NULL);
	Avalanche_Node* node = ID_to_node_AVALANCHE_obj_map.find(0)->second;
	this->network_propagate_AVALANCHE(t1,node);
	this->run_network_AVALANCHE();
	
	Transaction* old_tx = t1;
	for (int i=2; i < number_of_transactions+1; i++){
		cout << "Tx id: " << i << endl;
		this->node_matching();
		// make sure connectivity of all nodes are fine
		status_well_connectivity = this->check_connectivity_is_OK();
		while(!status_well_connectivity){
			cout << "NOT well connected !!" <<endl;
			this->node_matching();
			status_well_connectivity = this->check_connectivity_is_OK();
		}

		Transaction* new_tx = new Transaction(i,i,old_tx);
		//int generated_random_number = (rand() % num_nodes);
		//node = network->ID_to_node_obj_map.find(generated_random_number)->second;
		//this->node_matching();
		this->network_propagate_AVALANCHE(new_tx,node);
		this->run_network_AVALANCHE();
		old_tx = new_tx;
	}
	return this->get_latency("avalanche");

} 