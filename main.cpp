
#include "blizzard_network.h"
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;
using namespace std::chrono; 

void write_in_csv_file(map<int, double> output_blizzard,map<int, double> output_avalanche,string filename){
	/* save outputs into a csv file*/
	ofstream myFile(filename);
	cout << "--------- Latency: (ID, Time Blizz, Time Ava) ------------" <<endl;
	// for(map<int, double>::iterator it=output_blizzard.begin();it!=output_blizzard.end();it++){
	// 	if((it)->second != -1 && (it)->second !=0.0 && output_avalanche.find((it)->first)!= output_avalanche.end() && output_avalanche[(it)->first]>0.0){

	// 		cout << "ID " << (it)->first << ", Time " << (it)->second << "," << output_avalanche[(it)->first] <<endl;
	// 		myFile << (it)->second << "," << output_avalanche[(it)->first] <<"\n";
	// 	}
	// }
	myFile << "Model" << "," << "Latency" <<endl;
	for(map<int, double>::iterator it=output_blizzard.begin();it!=output_blizzard.end();it++){
		if((it)->second != -1 && (it)->second !=0.0 && output_avalanche.find((it)->first)!= output_avalanche.end() && output_avalanche[(it)->first]>0.0){
			myFile << "Blizzard" << "," << (it)->second << endl;//"," << output_avalanche[(it)->first] <<"\n";
		}
	}
	for(map<int, double>::iterator it=output_avalanche.begin();it!=output_avalanche.end();it++){
		if((it)->second != -1 && (it)->second !=0.0 && output_blizzard.find((it)->first)!= output_blizzard.end() && output_blizzard[(it)->first]>0.0){
			myFile << "Avalanche" << "," << (it)->second << endl;//"," << output_avalanche[(it)->first] <<"\n";
		}
	}
	myFile.close();
}

int main()
{
	bool aggregate_vote_nodes;
	bool bool_preprocessing;
	bool bool_isStorngly_Pref;
	int number_of_transactions = 100;//400;
	
	int num_nodes = 100;
	int num_brokers = 8;//10;//4;
	int num_connections_k = 3;
	int beta_1 = 11; // confidence level above this is gonna be finalized
	int beta_2 = 150; // consequent 
	string filename_case1 = "Blizzard_Avalanche_Latency1.csv";
	string filename_case2 = "Blizzard_Avalanche_Latency2.csv";


	Network* network = new Network(num_nodes,num_brokers, num_connections_k, beta_1, beta_2);
	map<int, double> output_blizzard_latency  = network->Blizzard_Latency(number_of_transactions);
	map<int, double> output_avalanche_latency = network->Avalanche_Latency(number_of_transactions);
	write_in_csv_file(output_blizzard_latency,output_avalanche_latency,filename_case1);

	Network* network2 = new Network(num_nodes*2,num_brokers+3, num_connections_k+3, beta_1, beta_2);
	map<int, double> output_blizzard_latency2  = network2->Blizzard_Latency(number_of_transactions);
	map<int, double> output_avalanche_latency2 = network2->Avalanche_Latency(number_of_transactions);
	write_in_csv_file(output_blizzard_latency2,output_avalanche_latency2,filename_case2);
	
	return 0;
}
  