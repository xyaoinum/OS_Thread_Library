//version 8.8
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include "thread.h"

using namespace std;

mutex mutex1;
cv cv1;
cv cv2;

bool queueisfull;
//bool initial;
int queuemaxsize;				//max size of queue
int trackcount;

vector<vector<int>> diskque;		//diskque have all the tracks waiting to be issued
vector<int> tracknum;			//tracknum have records for which track to be issued
vector<vector<int>> tracks;				//queue
vector<bool> trackinside;


void issue(void *a){
	intptr_t arg = (intptr_t) a;
	mutex1.lock();
	while(tracknum[arg] < diskque[arg].size()){	
		int temptrack = diskque[arg][tracknum[arg]];
		while(trackinside[arg] || tracks.size() == queuemaxsize){				
			cv1.wait(mutex1);
		}
		if(tracks.size() < queuemaxsize && !trackinside[arg]){
			cout<<"requester "<<arg<<" track "<<temptrack<<endl;
			tracknum[arg]++;
			vector<int> temp;
			temp.push_back(temptrack);
			temp.push_back(arg);					
			tracks.push_back(temp);
			trackinside[arg] = 1;
			cv2.signal();	
		}
		while(trackinside[arg] || queueisfull){						
			cv1.wait(mutex1);
		}	
	}
	mutex1.unlock();	
}


void service(void *a){
	int curtrack = 0;
	for(int i=0;i<diskque.size();i++){
		thread t1((thread_startfunc_t) issue, (void *) i);
	}
	mutex1.lock();
	
//cout<<"service hold the lock"<<endl;

	while(trackcount){
		while(1){
			if(tracks.size() != queuemaxsize){		
				for(int i=0;i<diskque.size();i++){
					if(tracknum[i] < diskque[i].size() && trackinside[i] == 0){ 
						queueisfull = 0;
						break;		
					}
					if(i == diskque.size()-1) queueisfull = 1; 
				}
			}else{
				queueisfull = 1;
			}
			if(queueisfull == 0){					
				cv2.wait(mutex1);	
			}else{
				break;
			}
		}
		
		int min = 0;
		int mintrack = 0;
		
		for(int n=0;n<tracks.size();n++){
			int temp = abs(curtrack - tracks[n][0]);
			if(n==0){
				min = temp;
			}else if(min > temp){
				min = temp;
				mintrack = n;
			}
		}
		curtrack = tracks[mintrack][0];
		cout<<"service requester "<<tracks[mintrack][1]<<" track "<<tracks[mintrack][0]<<endl;
		trackinside[tracks[mintrack][1]] = 0;	
		trackcount--;
		tracks.erase(tracks.begin() + mintrack);				
		cv1.broadcast();
//cout<<"broadcast"<<endl;
	}
	mutex1.unlock();
	
}


int main(int argc, const char* argv[]){
	queuemaxsize = 10;
	queueisfull = 0;
	//initial = 1;
	trackcount = 0;
	

	vector<int> temp;
	temp.push_back(785);
	temp.push_back(53);
	trackcount+=2;
	trackinside.push_back(0);
	tracknum.push_back(0);
	diskque.push_back(temp);

	temp.clear();
	temp.push_back(350);
	temp.push_back(914);
	trackcount+=2;
	trackinside.push_back(0);
	tracknum.push_back(0);
	diskque.push_back(temp);

	temp.clear();
	temp.push_back(827);
	temp.push_back(567);
	trackcount+=2;
	trackinside.push_back(0);
	tracknum.push_back(0);
	diskque.push_back(temp);


	temp.clear();
	temp.push_back(302);
	temp.push_back(230);
	trackcount+=2;
	trackinside.push_back(0);
	tracknum.push_back(0);
	diskque.push_back(temp);


	temp.clear();
	temp.push_back(631);
	temp.push_back(11);
	trackcount+=2;
	trackinside.push_back(0);
	tracknum.push_back(0);
	diskque.push_back(temp);
	
/*
	for(int i=0;i<3;i++){
		vector<int> temp;		
		int num;	
		//ifstream input;
		//string filename = argv[i+2];		
		//input.open(filename);
		temp.push_back(2424);
		temp.push_back(336);
		temp.push_back(6336);
		trackcount+=3;

		while(input>>num){
			trackcount++;			
			temp.push_back(num);
		}
		trackinside.push_back(0);
		tracknum.push_back(0);
		diskque.push_back(temp);	
		//input.close();
	}
*/
	cpu::boot(1, (thread_startfunc_t) service, (void *) 100, false, false, 0);
}
