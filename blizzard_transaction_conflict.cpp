#include "blizzard_transaction_conflict.h"
//#include "blizzard_transaction_conflict.h"
#include <algorithm>
#include <set>
#include <iterator>
using namespace std;
using namespace std::chrono; 

//set<Transaction*>::iterator it_set; 

void Transaction::print_transaction(){
	if(parent_tx != NULL){
		cout << "id: " << id << "utxo: " << utxo << parent_tx->id << endl;
	}else{
		cout << "id: " << id << "utxo: " << utxo << "no parent_tx" << endl;
	}	
}

void ConflictSet::add_tx_to_conf(Transaction* input_tx){
	if(conflicting_transactions_set.empty()){
		conflicting_transactions_set.insert(input_tx);
		pref_tx = input_tx;
		last_seen_tx = input_tx;
		counter++;
	}else{
		conflicting_transactions_set.insert(input_tx);
	}
}