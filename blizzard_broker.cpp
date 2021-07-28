#include "blizzard_broker.h"
#include "blizzard_node.h"
//#include "blizzard_transaction_conflict.h"
#include <algorithm>
#include <set>
#include <iterator>
#include <stdexcept>
//#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono; 

void Broker::print(){
	cout << "Broker "<<id << " Known Transactions ";
	for (set<int>::iterator it_transaction=known_transactions.begin();it_transaction!=known_transactions.end();it_transaction++){
		cout <<  (*it_transaction) << ","; 
	}
	cout << endl;
	cout << "..... "<< "Queried Transactions ";
	for (set<int>::iterator it_transaction=transactions.begin();it_transaction!=transactions.end();it_transaction++){
		cout <<  (*it_transaction) << ","; 
	}
	cout << endl;
}




void Broker::set_a_new_connection(int id_node,Node* node){
	id_connected_nodes.insert(id_node);	
	connected_nodes.insert(node);	
}
void Broker::delete_all_connections(){
	id_connected_nodes.clear();
	connected_nodes.clear();
}


void Broker::propagate(Transaction* input_tx){
	// both make nodes receive this Tx and made an inquery from them, so the reqult of all connected nodes are ready to be sent back in case of this brkr queried
	//Transaction* input_tx = new Transaction(new_input_tx->id,new_input_tx->utxo,new_input_tx->parent_tx);
	clock_t start, end;
	//int num_votes_saying_is_Pref = 0;
	// if transaction is NOT in the known set, then proceed with receiving it
	if (known_transactions.find(input_tx->id)==known_transactions.end()){
		set<Node*>::iterator it_set_for_nodes;
		for (it_set_for_nodes=connected_nodes.begin();it_set_for_nodes!=connected_nodes.end();it_set_for_nodes++){
			
			// if the connected node has NOT had the Tx, then call receive
			//start = clock();
			if ((*it_set_for_nodes)->known_transactions.find(input_tx->id)==(*it_set_for_nodes)->known_transactions.end()){
				cout << "Propagate Broker - broker " << id << " ----> Node " << (*it_set_for_nodes)->id << " on Tx " << input_tx->id << " SEC " <<(this->table_propagation_transactions_time.find(input_tx->id)->second)+comm_to_node[(*it_set_for_nodes)->id] <<endl;
				// update time to receive Tx at this node
				(*it_set_for_nodes)->table_propagation_transactions_time.insert(make_pair(input_tx->id,(this->table_propagation_transactions_time.find(input_tx->id)->second)+comm_to_node[(*it_set_for_nodes)->id]));
				if((*it_set_for_nodes)->table_propagation_transactions_time.find(input_tx->id)==((*it_set_for_nodes)->table_propagation_transactions_time.end())){
					(*it_set_for_nodes)->table_propagation_transactions_time.insert(make_pair(input_tx->id,comm_to_node[(*it_set_for_nodes)->id]+(this->table_propagation_transactions_time).find(input_tx->id)->second));
				}else{
					float previous_time = (*it_set_for_nodes)->table_propagation_transactions_time.find(input_tx->id)->second;
					(*it_set_for_nodes)->table_propagation_transactions_time.find(input_tx->id)->second = min(previous_time,comm_to_node[(*it_set_for_nodes)->id]+(this->table_propagation_transactions_time).find(input_tx->id)->second);
				}

				(*it_set_for_nodes)->receive_tx(input_tx);
				// if ((*it_set_for_nodes)->isStorngly_Pref(input_tx)){
				// 	cout << "is isStorngly_Pref"<<endl;
				// 	num_votes_saying_is_Pref +=1;
				// 	(*it_set_for_nodes)->table_transactions_isStronglyPref.insert(make_pair(input_tx,true));
				// }else{
				// 	(*it_set_for_nodes)->table_transactions_isStronglyPref.insert(make_pair(input_tx,false));
				// }	

			}
			// else{
			// 	// if already this node check isStronglyPref before, then it sends back the true/false from its saved table
			// 	if ((*it_set_for_nodes)->table_transactions_isStronglyPref.find(input_tx)->second){
			// 		cout << "is isStorngly_Pref"<<endl;
			// 		num_votes_saying_is_Pref +=1;
			// 	}
			// }

			//cout << "In propagate broker" << endl;
			//end = clock();	
			//double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

		}
		// if (num_votes_saying_is_Pref >= connected_nodes.size()/2){
		// 	table_queried_transactions.insert(pair<Transaction*,bool>(input_tx,true));
		// }else{
		// 	table_queried_transactions.insert(pair<Transaction*,bool>(input_tx,false));
		// }	

		known_transactions.insert(input_tx->id);
	}
}
bool Broker::query(Transaction* input_tx){
	if (known_transactions.find(input_tx->id)!=known_transactions.end()){
		if(table_queried_transactions.find(input_tx->id)==table_queried_transactions.end()){
			clock_t start, end;
			Transaction* input_tx_AT_THIS_NODE;
			int num_votes_saying_is_Pref = 0;
			set<Node*>::iterator it_set_for_nodes;
			for (it_set_for_nodes=connected_nodes.begin();it_set_for_nodes!=connected_nodes.end();it_set_for_nodes++){
				
				/* if the connected node has Tx IIN ITS KNOWN, then check is Strongly */
				start = clock();
				if ((*it_set_for_nodes)->known_transactions.find(input_tx->id)!=(*it_set_for_nodes)->known_transactions.end()){
					//cout << "IN Query Broker " << id << " ----> Node " << (*it_set_for_nodes)->id << " on Tx " << input_tx->id << endl;
					input_tx_AT_THIS_NODE = ((*it_set_for_nodes)->table_ID_transaction.find(input_tx->id))->second;
					cout << "Query Broker - broker "<<id << " input_tx_AT_THIS_NODE " << input_tx_AT_THIS_NODE->id ;//(input_tx_AT_THIS_NODE->parent_tx)->id << endl;
					if ((*it_set_for_nodes)->isStorngly_Pref(input_tx_AT_THIS_NODE)){
						//cout << "is isStorngly_Pref"<<endl;
						num_votes_saying_is_Pref +=1;
						(*it_set_for_nodes)->table_transactions_isStronglyPref.insert(make_pair(input_tx->id,true));
					}

					//cout << "NOT strongly pref " <<endl;
					// else{
					// 	(*it_set_for_nodes)->table_transactions_isStronglyPref.insert(make_pair(input_tx->id,false));
					// }	

				}
				else{
					throw invalid_argument( "ERROR: Query Broker on a transaction NOT known bf" );
					//cout << "ERROR: Query Broker on a transaction NOT known bf" <<endl;
					
					// if already this node check isStronglyPref before, then it sends back the true/false from its saved table
					// if ((*it_set_for_nodes)->table_transactions_isStronglyPref.find(input_tx)->second){
					// 	cout << "is isStorngly_Pref"<<endl;
					// 	num_votes_saying_is_Pref +=1;
					// }
				}

				end = clock();	
				double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

				input_tx_AT_THIS_NODE = ((*it_set_for_nodes)->table_ID_transaction.find(input_tx->id))->second;
				(*it_set_for_nodes)->table_isStrongly_transactions_time.insert(make_pair(input_tx_AT_THIS_NODE,time_taken));
				

			}
			//cout << "... ... In Query Broker " << id << " on Tx " << input_tx->id << " has "<< num_votes_saying_is_Pref << " votes" <<endl;

			if (num_votes_saying_is_Pref >= connected_nodes.size()/2){
				table_queried_transactions.insert(pair<int,bool>(input_tx->id,true));
			}else{
				table_queried_transactions.insert(pair<int,bool>(input_tx->id,false));
			}	

			transactions.insert(input_tx->id);
			cout << "END Query Broker - broker " << id << " TX " << input_tx->id << " is  " << table_queried_transactions.find(input_tx->id)->second <<endl;
		}
		return table_queried_transactions.find(input_tx->id)->second;
	// NOT received it before ==> need to propagate it
	}else{
		//update receive time for this tx
		cout << "Decision Broker " << id << " TX " << input_tx->id << " is  " << false <<endl;
		return false;
	}
}


/*
void Broker::propagate(Transaction* input_tx){
	// both make nodes receive this Tx and made an inquery from them, so the reqult of all connected nodes are ready to be sent back in case of this brkr queried
	clock_t start, end;
	int num_votes_saying_is_Pref = 0;
	// if transaction is NOT in the known set, then proceed with receiving it
	if (known_transactions.find(input_tx)==known_transactions.end()){
		set<Node*>::iterator it_set_for_nodes;
		for (it_set_for_nodes=connected_nodes.begin();it_set_for_nodes!=connected_nodes.end();it_set_for_nodes++){
			
			// if the connected node has NOT had the Tx, then call receive
			start = clock();
			if ((*it_set_for_nodes)->known_transactions.find(input_tx)==(*it_set_for_nodes)->known_transactions.end()){
				cout << "IN Propagate Broker " << id << " ----> Node " << (*it_set_for_nodes)->id << " on Tx " << input_tx->id << endl;
				// update time to receive Tx at this node
				(*it_set_for_nodes)->table_propagation_transactions_time.insert(make_pair(input_tx,(this->table_propagation_transactions_time.find(input_tx)->second)+comm_to_node[(*it_set_for_nodes)->id]));
				
				(*it_set_for_nodes)->receive_tx(input_tx);
				if ((*it_set_for_nodes)->isStorngly_Pref(input_tx)){
					cout << "is isStorngly_Pref"<<endl;
					num_votes_saying_is_Pref +=1;
					(*it_set_for_nodes)->table_transactions_isStronglyPref.insert(make_pair(input_tx,true));
				}else{
					(*it_set_for_nodes)->table_transactions_isStronglyPref.insert(make_pair(input_tx,false));
				}	

			}else{
				// if already this node check isStronglyPref before, then it sends back the true/false from its saved table
				if ((*it_set_for_nodes)->table_transactions_isStronglyPref.find(input_tx)->second){
					cout << "is isStorngly_Pref"<<endl;
					num_votes_saying_is_Pref +=1;
				}
			}

			cout << "In propagate broker" << num_votes_saying_is_Pref <<endl;
			end = clock();	
			double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

		}
		if (num_votes_saying_is_Pref >= connected_nodes.size()/2){
			table_queried_transactions.insert(pair<Transaction*,bool>(input_tx,true));
		}else{
			table_queried_transactions.insert(pair<Transaction*,bool>(input_tx,false));
		}	

		known_transactions.insert(input_tx);
	}
}
bool Broker::query(Transaction* input_tx){
	if (known_transactions.find(input_tx)!=known_transactions.end()){
		transactions.insert(input_tx);
		return table_queried_transactions.find(input_tx)->second;
	// NOT received it before ==> need to propagate it
	}else{
		//update receive time for this tx

		return false;
	}
}
*/













//*************************************************************************************

// void Broker::broker_receive_tx(Transaction* input_tx){
// 	// if transaction is NOT in the known set, then proceed with receiving it
// 	if (known_transactions.find(input_tx)==known_transactions.end()){
// 		set<Node*>::iterator it_set_for_nodes;
// 		for (it_set_for_nodes=connected_nodes.begin();it_set_for_nodes!=connected_nodes.end();it_set_for_nodes++){
			
// 			// if the connected node has NOT had the Tx, then call receive
// 			if ((*it_set_for_nodes)->known_transactions.find(input_tx)==(*it_set_for_nodes)->known_transactions.end()){
// 				cout << "Broker " << id << " ----> Node " << (*it_set_for_nodes)->id << " on Tx " << input_tx->id << endl;
// 				(*it_set_for_nodes)->receive_tx(input_tx);
// 			}	
// 		}
// 		known_transactions.insert(input_tx);
// 	}
// }
// bool Broker::query(Transaction* input_tx){
// 	if (transactions.find(input_tx)==transactions.end()){
// 		clock_t start, end;
// 		int num_votes_saying_is_Pref = 0;
// 		for (set<Node*>::iterator it_set_for_nodes=connected_nodes.begin();it_set_for_nodes!=connected_nodes.end();it_set_for_nodes++){
// 			// if the connected node has NOT had the Tx, then call receive
// 			//auto start = high_resolution_clock::now();
// 			//start = high_resolution_clock::now();
			
// 			start = clock();
// 			if ((*it_set_for_nodes)->known_transactions.find(input_tx)==(*it_set_for_nodes)->known_transactions.end()){
// 				//if ((*it_set_for_nodes)->preprocessing(input_tx)){
// 					(*it_set_for_nodes)->receive_tx(input_tx);	
// 				//}
				
// 			}
// 			if ((*it_set_for_nodes)->isStorngly_Pref(input_tx)){
// 				num_votes_saying_is_Pref +=1;
// 			}	
// 			end = clock();
// 			double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
// 			//cout << time_taken <<endl;
// 		}
// 		if (num_votes_saying_is_Pref >= connected_nodes.size()/2){
// 			table_queried_transactions.insert(pair<Transaction*,bool>(input_tx,true));
// 			return true;
// 		}else{
// 			table_queried_transactions.insert(pair<Transaction*,bool>(input_tx,false));
// 			return false;
// 		}
// 	}else{
// 		return table_queried_transactions.find(input_tx)->second;
// 	}
// }
