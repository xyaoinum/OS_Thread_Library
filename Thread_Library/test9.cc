#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include "thread.h"

using namespace std;

struct track {
	int reqN;
	int pos;
};

thread* t;

int maxQueueSize = 0;
int numOfFinReq = 0;
int curTrack = 0;
vector<vector<int>> allReq;// all the requester tracks
vector<track> liveTracks;//the tracks that is on the queue -- has been requested but not serviced
vector<bool> lastReqServiced;//if the last request issued from a requester has been serviced

mutex mutex0;
cv cvR;//condition variable for the requester thread to wait
cv cvS;//condition variable for the service thread to wait

void requester(void *a) {
	
	int reqN = (intptr_t)a;
	if(reqN == 3 || reqN == 1) {
		thread::yield();
	}
	mutex0.lock();

	for (int i = 0; i < allReq[reqN].size(); i++) {
		if(reqN == 2) {
			thread::yield();
		}
		//a requester thread must wait if the "queue if full" OR "the last request from that requester hasn't been serviced"
		while (liveTracks.size() >= maxQueueSize || lastReqServiced[reqN] == false) {
			cvR.wait(mutex0);
		}
		cout << "requester " << reqN << " track " << allReq[reqN][i] << endl;
		track tmpT;
		tmpT.reqN = reqN;
		tmpT.pos = i;
		liveTracks.push_back(tmpT);
		lastReqServiced[reqN] = false;
		cvS.signal();
	}
	mutex0.unlock();
}

void service(void *a) {
	mutex0.lock();
//cout << "service" << endl;
	while (1) {
		//a service thread must wait if "the queue do not have the largest possible request" OR "all the requests have been serviced"
		while ((liveTracks.size() < maxQueueSize && allReq.size() - numOfFinReq >= maxQueueSize) || liveTracks.size() < allReq.size() - numOfFinReq && allReq.size() - numOfFinReq < maxQueueSize || liveTracks.size() == 0) {
			//cout << "service wait"<< endl;
			cvS.wait(mutex0);
		}
		int tmpReqN;
		int tmpMin = INT_MAX;
		int tmpPos;
		int tmpTrack;
		int tmpCur;
		//get the track that is closest to the track that has just been serviced
		for (int i = 0; i < liveTracks.size(); i++) {
			if (tmpMin > allReq[liveTracks[i].reqN][liveTracks[i].pos] - curTrack && tmpMin >  curTrack - allReq[liveTracks[i].reqN][liveTracks[i].pos]) {
				tmpCur = allReq[liveTracks[i].reqN][liveTracks[i].pos];
				tmpReqN = liveTracks[i].reqN;
				tmpPos = liveTracks[i].pos;
				tmpTrack = i;
				if (allReq[liveTracks[i].reqN][liveTracks[i].pos] - curTrack > 0) {
					tmpMin = allReq[liveTracks[i].reqN][liveTracks[i].pos] - curTrack;
				}
				else  {
					tmpMin = curTrack - allReq[liveTracks[i].reqN][liveTracks[i].pos];
				}
			}
		}
		curTrack = tmpCur;
		cout << "service requester " << tmpReqN << " track " << curTrack << endl;;
		lastReqServiced[tmpReqN] = true;
		if (tmpPos == allReq[tmpReqN].size() - 1) {
			numOfFinReq++;
		}
		liveTracks.erase(liveTracks.begin() + tmpTrack);
		cvR.broadcast();
	}
	mutex0.unlock();
}

void start(void *a) {
	thread s((thread_startfunc_t)service, (void *)a);
	thread t0((thread_startfunc_t)requester, (void *)0);
	thread t1((thread_startfunc_t)requester, (void *)1);
	thread t2((thread_startfunc_t)requester, (void *)2);
	thread t3((thread_startfunc_t)requester, (void *)3);
	t = &t3;
	t3.join();
	thread t4((thread_startfunc_t)requester, (void *)4);

}

int main(int argc, char *argv[]) {

	//get all the requester infromation from the files
	int tmpTrack;
	vector<int> tmpReq;
	maxQueueSize = 3;


	tmpReq.clear();
	tmpReq.push_back(785);
	tmpReq.push_back(53);
	lastReqServiced.push_back(true);
	allReq.push_back(tmpReq);

	tmpReq.clear();
	tmpReq.push_back(350);
	tmpReq.push_back(914);
	lastReqServiced.push_back(true);
	allReq.push_back(tmpReq);

	tmpReq.clear();
	tmpReq.push_back(827);
	tmpReq.push_back(567);
	lastReqServiced.push_back(true);
	allReq.push_back(tmpReq);

	tmpReq.clear();
	tmpReq.push_back(302);
	tmpReq.push_back(230);
	lastReqServiced.push_back(true);
	allReq.push_back(tmpReq);

	tmpReq.clear();
	tmpReq.push_back(631);
	tmpReq.push_back(11);
	lastReqServiced.push_back(true);
	allReq.push_back(tmpReq);

	cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}







