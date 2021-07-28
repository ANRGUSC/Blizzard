#ifndef BLIZZARD_TRANSACTION_CONFLICT
#define BLIZZARD_TRANSACTION_CONFLICT



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




class Transaction
{
public:
	int id;
	int utxo;
	int conf;
	float issued_time;
	double confirmed_time; 
	bool confirmed;
	Transaction* parent_tx;
	void print_transaction(void);
	//void set_up_parants(Transaction*);
	bool operator < (const Transaction & t) const
	{
		return (this->id < t.id);
	}
	Transaction(int input_id,int input_utxo, Transaction* input_parent){
		id = input_id;
		utxo = 	input_utxo;
		parent_tx = input_parent;
		conf = 0;
		confirmed = false;
		confirmed_time = 0.0; 
	}  
};


class ConflictSet{
	public:
		set<Transaction*> conflicting_transactions_set;

		Transaction* pref_tx;
		Transaction* last_seen_tx;
		int counter;
		void add_tx_to_conf(Transaction*);
		ConflictSet(){
			counter=0;
		}
};


typedef std::unordered_map<int, ConflictSet* > conflicts_dict;

#endif