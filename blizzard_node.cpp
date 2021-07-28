
#include "blizzard_node.h"
#include "blizzard_broker.h"
//#include "blizzard_transaction_conflict.h"
#include <algorithm>
#include <set>
#include <iterator>
using namespace std;
using namespace std::chrono; 

//set<Transaction*>::iterator it_set; 

//Transaction* genesis_tx; 

//--------------

// it->first is int utxo
// it->second is OBJECT of ConflictSet

void Node::set_a_new_connection(int id_brkr, Broker* brkr){
	id_connected_brokers.insert(id_brkr);	
	connected_brokers.insert(brkr);
}
void Node::delete_all_connections(){
	id_connected_brokers.clear();
	connected_brokers.clear();
}

void Node::print_node(){
	conflicts_dict::iterator it;
	//cout << id <<endl ;
	//cout << "genesis tx : " << genesis_tx->id <<endl ; 
	// cout << "--------Print Conflict Dict -----"<<endl;
	// for(it = conflicts.begin() ; it !=  conflicts.end(); it++){
	// 	cout << "UTXO "<<it->first << " set ";
	// 	cout<< "[";
	// 	for (it_set = ((it->second)->conflicting_transactions_set).begin(); it_set != ((it->second)->conflicting_transactions_set).end(); it_set ++){
	// 		cout << (*it_set)->id << " ," ;
	// 	}  
	// 	cout<< "]" << endl;
	// }
	cout << "------------------- Node " << id << "-----------------------" <<endl;
	cout << "... KNOWN Transactions: ";
	for (set<Transaction*>::iterator it_transaction=OBJ_known_transactions.begin();it_transaction!=OBJ_known_transactions.end();it_transaction++){
		cout <<(*it_transaction)->id <<" , "; 
		//<< table_propagation_transactions_time.find(*it_transaction)->second <<" , " << table_query_AND_append_transactions_time.find(*it_transaction)->second<< ") , "; 
	}
	cout << endl;

	cout << "... Queried Transactions: ";
	for (set<Transaction*>::iterator it_transaction=OBJ_transactions.begin();it_transaction!=OBJ_transactions.end();it_transaction++){
		cout << (*it_transaction)->id <<" , "; 
	}
	cout << endl;
	cout << "... More info regarding all stored TXs at this node "<< endl;
	cout << "... ... Info format- (Tx id, confidence val, receive time(no comm time), appending time(no comm time),finalized time(no comm time)): " << endl;

	for (set<Transaction*>::iterator it_transaction=OBJ_known_transactions.begin();it_transaction!=OBJ_known_transactions.end();it_transaction++){
		
		cout << "... ... ... ("<<(*it_transaction)->id <<" , " << (*it_transaction)->conf << ", " <<
		table_propagation_transactions_time.find((*it_transaction)->id)->second <<" , " << 
		table_query_AND_append_transactions_time.find(*it_transaction)->second<< " , " <<
		(*it_transaction)->confirmed_time<< ") , "; 
		cout << endl;
	}
	cout << "-------------------------------------------------" <<endl;

	


	// cout << " size of comm_to_broker "<< comm_to_broker.size()<<endl;


	// cout << "..... Node " << id << " Tx propagate time: " << endl;
	// for(map<Transaction*, float >::const_iterator it = table_propagation_transactions_time.begin();it != table_propagation_transactions_time.end(); ++it)
	// {
	//     cout << (it->first)->id << " " << it->second << "\n";
	// }
}

void Node::get_latency_transactions(){
	//map<int,double> output;
	for (set<Transaction*>::iterator it_transaction=OBJ_known_transactions.begin();it_transaction!=OBJ_known_transactions.end();it_transaction++){
		if((*it_transaction)->confirmed_time != 0.0){
			double total_time = table_propagation_transactions_time.find((*it_transaction)->id)->second +
			table_query_AND_append_transactions_time.find(*it_transaction)->second +
			(*it_transaction)->confirmed_time;
			ID_transaction_latency.insert(make_pair((*it_transaction)->id, total_time));
		}
	}

}

void Node::receive_tx(Transaction* input_tx){
		//create identical OBJ trans for this node
			
		if (known_transactions.find(input_tx->id)==known_transactions.end()){
			 
			//cout << "In rece Node: "<< id << " for Tx " << input_tx->id << endl;
			// Transaction* genesis_tx;
			conflicts_dict::iterator it;

			// create new IDENTICAL Tx with "similar" parent to be stored at THIS node
			Transaction* new_input_tx;
			if(input_tx->parent_tx == NULL){
				new_input_tx = new Transaction(input_tx->id,input_tx->utxo,input_tx->parent_tx);
				genesis_tx = new_input_tx;
			}else{
				new_input_tx = new Transaction(input_tx->id,input_tx->utxo,table_ID_transaction.find( (input_tx->parent_tx)->id)->second);
			}
			table_ID_transaction.insert(make_pair(input_tx->id,new_input_tx));


				if(conflicts.find(new_input_tx->utxo)==conflicts.end()){
					ConflictSet* new_conflict = new ConflictSet;
					new_conflict->add_tx_to_conf(new_input_tx);
					conflicts.insert(make_pair(new_input_tx->utxo,new_conflict));
					// for (it_set = new_conflict->conflicting_transactions_set.begin(); it_set != new_conflict->conflicting_transactions_set.end(); it_set ++){
					// 	cout << "In Rece func: New Conflict Created! no conflict for Tx "<<(*it_set)->id <<endl;
					// }  
					
				}else{
					it = conflicts.find(new_input_tx->utxo);
					// cout<< "In Rece func: Confilict was established bf: Tx "<< input_tx->id << " has conf with "<< (*it).first <<endl;
					(((it)->second)->conflicting_transactions_set).insert(new_input_tx);
				}
			//}
			known_transactions.insert(input_tx->id);

			OBJ_known_transactions.insert(new_input_tx);
			// ============================================
			// Now let connected Brokers know about this Tx
			// ============================================
			// set<Broker*>::iterator it_set_for_brkrs;
			// for (it_set_for_brkrs=connected_brokers.begin();it_set_for_brkrs!=connected_brokers.end();it_set_for_brkrs++){
			// 	if ((*it_set_for_brkrs)->known_transactions.find(input_tx)==(*it_set_for_brkrs)->known_transactions.end()){
			// 		cout << "In Receive Node " << id << " -----> BROKER " << (*it_set_for_brkrs)->id << " on Tx " << input_tx->id << " SEC " <<comm_to_broker[(*it_set_for_brkrs)->id]+table_propagation_transactions_time.find(input_tx)->second << endl;
			// 		if((*it_set_for_brkrs)->table_propagation_transactions_time.find(input_tx)==((*it_set_for_brkrs)->table_propagation_transactions_time.end())){
			// 			(*it_set_for_brkrs)->table_propagation_transactions_time.insert(make_pair(input_tx,comm_to_broker[(*it_set_for_brkrs)->id]+(this->table_propagation_transactions_time).find(input_tx)->second));
			// 		}else{
			// 			float prev_time = (*it_set_for_brkrs)->table_propagation_transactions_time.find(input_tx)->second;
			// 			(*it_set_for_brkrs)->table_propagation_transactions_time.find(input_tx)->second = min(prev_time,comm_to_broker[(*it_set_for_brkrs)->id]+(this->table_propagation_transactions_time).find(input_tx)->second);
			// 		}
			// 		(*it_set_for_brkrs)->propagate(input_tx);
			// 	}	
			// }
		}	

}





bool Node::preprocessing(Transaction* input_tx){
	if(known_transactions.find((input_tx->parent_tx)->id) == known_transactions.end()){
		return false;
	}else{
		return true;
	}
}                


//NOTE********* correct transactions to known transactions        	
bool Node::isStorngly_Pref(Transaction* input_tx){
	//cout << "IN isStorngly_Pref" <<endl;
	if(input_tx != genesis_tx && input_tx->parent_tx == NULL){
		//cout << "SHOULDNT" <<endl;
		return false;
	}else{
		bool strongly_preferred = true;
        //reflexive transitive closure
        Transaction* parent = input_tx->parent_tx;
        while(parent && (set_finalized_transactions.find(input_tx)==set_finalized_transactions.end()) ){
        	//cout<< "computing "<<parent->id <<endl;
            if (transactions.find(parent->id)== transactions.end() ){
                return false;
            }
            //cout << "tx " << parent->id <<"was in list";
            strongly_preferred = strongly_preferred && (conflicts[parent->utxo]->pref_tx == parent);
            //cout << "strongly pref" << strongly_preferred <<endl;
            if (strongly_preferred == false){
            	break;
            }
            if (parent->parent_tx == NULL){
            	//cout << "break" <<endl;
            	break;
            }else{
            	parent = parent->parent_tx;
            }	
            //parent_set = parent_set.union(parent.parents)
        }
        return strongly_preferred;
	}
}

bool Node::is_accepted(Transaction* input_tx){
	if(set_finalized_transactions.find(input_tx)==set_finalized_transactions.end()){
        bool early_commit = true;
        //cout << "TX "<< input_tx->id << "is being CHECKED if ACCEPTED by Node " << id << " size conflicts,conf " << (conflicts[input_tx->utxo]->conflicting_transactions_set).size()<<
        //" , " << input_tx->conf << endl;
        early_commit = early_commit && ((conflicts[input_tx->utxo]->conflicting_transactions_set).size() == 1);
        early_commit = early_commit && (input_tx->conf > beta1);
        if (early_commit && input_tx->parent_tx != NULL){
        	early_commit = early_commit && this->is_accepted(input_tx->parent_tx);
        }
        if( early_commit==false &&  conflicts[input_tx->utxo]->pref_tx == input_tx){
            bool alaki = conflicts[input_tx->utxo]->counter > beta2;
			//cout << "values " <<conflicts[input_tx->utxo]->counter << " , " <<  beta2  << "," << alaki <<endl;
            if(conflicts[input_tx->utxo]->counter > beta2){
            	//cout << "TX "<< input_tx->id << "GOT consequeted one!!!! Node " << id <<endl;
            	
            	set_finalized_transactions.insert(input_tx);
            	return true;
            }else{
            	//cout << "TX "<< input_tx->id << "NOT ACCEPTED one!!!! Node " << id <<endl;
            
            	return false;
            }
            //return conflicts[input_tx->utxo]->counter > beta2;
        }
        if(early_commit){
        	set_finalized_transactions.insert(input_tx);
        	//cout << "TX "<< input_tx->id << "GOT early_commit one!!!! Node " << id <<endl;
        }else{
        	//cout << "TX "<< input_tx->id << "NOT ACCEPTED one!!!! Node " << id <<endl;
        }

        //cout << "TX "<< input_tx->id << "got acc one!!!! Node " << id <<endl;
        return early_commit;
    }else{
    	return true;
    } 
}        

// WHEN it is STRONGLY_Pref, then update all parents conf
void Node::update_Dag_AND_conflictSets_as_attaching_new_tx(Transaction* input_tx){
		clock_t start, end;
		start = clock();
		conflicts_dict::iterator it;
		if(input_tx == genesis_tx){
			//cout << "Genesis "  << endl;
		}else{
			//cout << (input_tx->parent_tx)->id << endl;
			Transaction* parent = input_tx->parent_tx;
			while(parent!= NULL){
	        	parent->conf++;
	        	//cout << " update_Dag - for Transaction " << input_tx->id << "inside while " << parent->id<< " conf "<< parent->conf << endl;
	        	it = conflicts.find(parent->utxo);
	        	if( (((it)->second)->pref_tx)->conf < parent->conf  ){
	        		(((it)->second)->pref_tx) = parent;
	        	} 
	        	if((((it)->second)->last_seen_tx)!= parent){
	        		(((it)->second)->last_seen_tx)= parent;
	        		(((it)->second)->counter) = 0;
	        	}else{
	        		(((it)->second)->counter)++;
	        	}
	        	if (this->is_accepted(parent)){
	        			//cout << "Transaction " << parent->id << " got accepted!" << endl;
	        			end = clock();
	        			parent->confirmed_time = (double(end - start) / double(CLOCKS_PER_SEC)) + time_spent_updating_DAG;
	        			parent->confirmed = true;
	        	}
	        	//cout << "injaaaa" <<endl;
	        	parent = parent->parent_tx;
	        }
	    }
	    end = clock();	
		double time_taken = double(end - start) / double(CLOCKS_PER_SEC);    	
	    time_spent_updating_DAG += time_taken;
}

void Node::query(Transaction* input_tx){
	if (transactions.find(input_tx->id)==transactions.end()){
		clock_t start, end;
		start = clock();
		int num_votes_saying_is_Pref = 0;
		//cout << "In Query Node "<<id<< " on Tx " << input_tx->id <<endl;
		for (set<Broker*>::iterator it_set_for_brkrs=connected_brokers.begin();it_set_for_brkrs!=connected_brokers.end();it_set_for_brkrs++){
			// update time to receive Tx at this broker
			if((*it_set_for_brkrs)->table_propagation_transactions_time.find(input_tx->id)==(*it_set_for_brkrs)->table_propagation_transactions_time.end()){
				//cout << "In Query Node "<<id<< " trans " << input_tx->id << " add " << 0.1 << " to " <<  (this->table_propagation_transactions_time.find(input_tx)->second) << endl;
				(*it_set_for_brkrs)->table_propagation_transactions_time.insert(make_pair(input_tx->id,(this->table_propagation_transactions_time.find(input_tx->id)->second)+comm_to_broker[(*it_set_for_brkrs)->id]));
			}		
			//cout << "... Broker " << (*it_set_for_brkrs)->id <<" is about ot be queried in this Node" << endl;//id_connected_brokers<< endl;
			if( (*it_set_for_brkrs)->query(input_tx)){
				num_votes_saying_is_Pref += 1;
			}
			
		}
		//whenever reaches majority, update DAG and Tx to it
		if(num_votes_saying_is_Pref >= connected_brokers.size()/2){
			//cout << "Query Node - MAJORITY ACHIVED - node " << id << " -Tx "<<input_tx->id <<endl;
			this->update_Dag_AND_conflictSets_as_attaching_new_tx(input_tx);
			// only add to set of QUERIED ones if it gets the majority
			transactions.insert(input_tx->id);
			OBJ_transactions.insert(input_tx);
			//break;
		}

		//cout << "Query Node -  Node " << id << " Votes: "<< num_votes_saying_is_Pref << " out of " << connected_brokers.size() << endl;
			  
		end = clock();	
		double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
		table_query_AND_append_transactions_time.insert(make_pair(input_tx,time_taken));
	}
}



void Node::node_issue_transaction(Transaction* input_tx){
		table_propagation_transactions_time.insert(make_pair(input_tx->id,0.0));
	    /* **************************
		IF CANNOT FIND AN ELEMENT OF MAP YOU ASKED TO DO STH WITH THE SECOND PART, ====> IT IS GONNA GIVE NONSENCE ANSWER!!!!!!!!!
	    //table_transactions_sent_to_connected_brkrs.find(input_tx)->second = false;
		*/
		this->receive_tx(input_tx);
}


bool Node::has_node_propagated(int id_input_tx){
	//if (known_transactions.find(input_tx)!=known_transactions.end()){
		//cout << "In *has_node_propagated* node "<< id << endl;
		
		Transaction* input_tx = table_ID_transaction.find(id_input_tx)->second;


		bool any_brkr_not_heard_this_Tx = false;
		//cout << "In has_node_propagated .... of node "<< id <<endl;
		for (set<Broker*>::iterator it_set_for_brkrs=connected_brokers.begin();it_set_for_brkrs!=connected_brokers.end();it_set_for_brkrs++)
		{

			if((*it_set_for_brkrs)->known_transactions.find(id_input_tx)==(*it_set_for_brkrs)->known_transactions.end()){
				//cout << " .... Broker " << (*it_set_for_brkrs)->id << " gonna let :" <<endl;
				any_brkr_not_heard_this_Tx = true;
				for (set<Node*>::iterator it_set_for_nodes=(*it_set_for_brkrs)->connected_nodes.begin();it_set_for_nodes!=(*it_set_for_brkrs)->connected_nodes.end();it_set_for_nodes++){
					//set comm time
					if((*it_set_for_nodes)->table_propagation_transactions_time.find(id_input_tx)==((*it_set_for_nodes)->table_propagation_transactions_time.end())){
						//cout << "In ****** "<<endl;
						// for (int k=0;k!=comm_to_broker.size();k++){
						// 	cout << this->comm_to_broker[k] << ","; 
						// }
						// cout <<endl;

						float comm_time_to_broker = comm_to_broker[(*it_set_for_brkrs)->id];
						float comm_time_to_node = (*it_set_for_brkrs)->comm_to_node[(*it_set_for_nodes)->id];
						//cout << "node "<<id << " "<< (*it_set_for_brkrs)->id <<endl; 
						//cout << "comm_time_to_broker " << comm_time_to_broker <<comm_to_broker.size()<< " comm_time_to_node "<< comm_time_to_node <<endl;
						(*it_set_for_nodes)->table_propagation_transactions_time.insert(make_pair(id_input_tx,comm_time_to_broker+comm_time_to_node+(table_propagation_transactions_time).find(id_input_tx)->second));
					}
					//cout << " ..... ..... .... " << (*it_set_for_nodes)->id << "receive Tx" << input_tx->id <<endl;
					(*it_set_for_nodes)->receive_tx(input_tx);
				}
				(*it_set_for_brkrs)->known_transactions.insert(id_input_tx);

			}
		}
		if(any_brkr_not_heard_this_Tx){
			return false;
		}
		return true;
	//}
}		

bool Node::run_node(){
	set<Transaction*> known_but_NOT_queried;
	//cout<<"In Run_Node size Known Txs "<<(this->known_transactions).size()<<endl;
	set_difference(OBJ_known_transactions.begin(), OBJ_known_transactions.end(), OBJ_transactions.begin(), OBJ_transactions.end(),std::inserter(known_but_NOT_queried, known_but_NOT_queried.end()));
    //cout<<" ... ... node "<<id <<" has "<<known_but_NOT_queried.size()<<" UNQUERIED and " <<known_transactions.size()<< " known transactions" << endl;
    for (set<Transaction*>::iterator it_transaction=known_but_NOT_queried.begin();it_transaction!=known_but_NOT_queried.end();it_transaction++){
		//cout << "RUN NODE - node "<< id << " Gonna query Tx "<< (*it_transaction)->id <<endl; 
		this->query(*it_transaction);
	}
	return false;
}

