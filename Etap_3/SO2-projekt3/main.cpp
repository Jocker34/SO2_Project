#include "Window.h"
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <vector>

bool working = true;
Window *window;
Shared *shared;
const int nHuman = 3; //ile pasazerow
const int nPM = 3; //ile samolotów 
int dest = 69;
int destPM[nPM];
std::queue <Human*> queue_to_plane[nPM]; //kolejka do samolotu
std::queue <Human*> queue_to_check_in; //kolejka do obslugi
std::mutex human_mtx;
std::mutex human_q_mtx;
std::mutex PM_mtx;
std::mutex checkin_mtx;
std::condition_variable human_cv;
std::condition_variable PM_cv;
std::condition_variable checkin_cv;

void stop() {
    while (working) {
        char c = getchar();
        if(c == ' ')
            working = false;
    }
}

void humanMoving(int id) {
    while(working) {
	usleep(shared->humans[id]->humanSpeed);

	if(shared->humans[id]->nPacks == 0){
		window->clearHuman(shared->humans[id]->currX,shared->humans[id]->currY);
		break;
	}

	if(shared->humans[id]->currX != shared->humans[id]->destX || shared->humans[id]->currY != shared->humans[id]->destY) {
	    shared->humans[id]->moveFun();
	    continue;
  	}
	else {
	    if(shared->humans[id]->currX == dest && shared->humans[id]->destX == dest) {
		human_q_mtx.lock();
		if(!shared->humans[id]->inQ){
		    queue_to_check_in.push(shared->humans[id]);
		    shared->humans[id]->inQ=true;
		    dest-=7;
		    for(int i=0; i<nHuman; i++)
			if(shared->humans[i]->destY==13 && !shared->humans[i]->inQ)
			    shared->humans[i]->destX=dest;

		    checkin_cv.notify_one();
		}
		human_q_mtx.unlock();
	    }
	    else {
		for(int i=0; i<nPM; i++){
		    if(shared->humans[id]->currY == 2+10*i) {
			human_q_mtx.lock();
			if(!shared->humans[id]->inQ){
			    queue_to_plane
			[i].push(shared->humans[id]);
			    shared->humans[id]->inQ=true;
			    destPM[i]+=7;
			    for(int j=0; j<nHuman; j++)
				if(shared->humans[j]->destY==2+i*10 && !shared->humans[j]->inQ)
			    	    shared->humans[j]->destX=destPM[i];
			    PM_cv.notify_all();
			}
			human_q_mtx.unlock();
		    }
		}
	    }
	    std::unique_lock<std::mutex> wait_human(human_mtx);
	    	human_cv.wait(wait_human);
	    wait_human.unlock();
	}
    }
    human_cv.notify_all();
    PM_cv.notify_all();
    checkin_cv.notify_one();
}

void updateWindow() {
    window->scene(nPM, nHuman);
    while(working) {
	window->update(shared);
	usleep(5000);
    }
}


void giveTicket() {
    while(working) {
	usleep(200000);
	if(!queue_to_check_in.empty()) {
	    if(queue_to_check_in.front()->nPacks >= 49) {
		queue_to_check_in.front()->destX = destPM[0];
		queue_to_check_in.front()->destY = 2;
		queue_to_check_in.front()->inQ=false;
		queue_to_check_in.pop();
		dest+=7;
		for(int i=0; i<nHuman; i++)
		    if(shared->humans[i]->destY==13)
			shared->humans[i]->destX+=7;
		human_cv.notify_all();
		continue;
	    }
	    if(queue_to_check_in.front()->currX!=69) continue;
	    queue_to_check_in.front()->nPacks+=5;
	}
	else {
	    std::unique_lock<std::mutex> wait_firm(checkin_mtx
	);
		checkin_cv.wait(wait_firm);
	    wait_firm.unlock();
	}
    }
}

void takeTicket(int id) {
    shared->packs[id]=0;
    while(working) {
	usleep(40000);
	if(queue_to_plane
[id].empty()) {
	    std::unique_lock<std::mutex> wait_pm(PM_mtx);
	    	PM_cv.wait(wait_pm);
	    wait_pm.unlock();
	}
	else {
	    if(queue_to_plane
	[id].front()->nPacks==0) {
		queue_to_plane
	[id].front()->destX=dest;
		queue_to_plane
	[id].front()->destY=13;
		queue_to_plane
	[id].front()->inQ=false;
		queue_to_plane
	[id].pop();
		destPM[id]-=7;
		for(int j=0; j<nHuman; j++)
		    if(shared->humans[j]->destY==2+10*id)
			shared->humans[j]->destX-=7;
		human_cv.notify_all();
		continue;
	    }
	    if(shared->packs[id]>=51) {
		if(id==nPM-1) {
		    std::unique_lock<std::mutex> wait_pm(PM_mtx);
	    		PM_cv.wait(wait_pm);
	    	    wait_pm.unlock();
		    continue;
		}
		else {
		    queue_to_plane
		[id].front()->destX=destPM[id+1];
		    queue_to_plane
		[id].front()->destY=2+10*(id+1);
		    queue_to_plane
		[id].front()->inQ=false;
		    queue_to_plane
		[id].pop();
		    destPM[id]-=7;
		    for(int j=0; j<nHuman; j++)
		    	if(shared->humans[j]->destY==2+10*id)
			    shared->humans[j]->destX-=7;
		    human_cv.notify_all();
		    continue;
		}
	    }
	    if(queue_to_plane
	[id].front()->currX!=16) continue;
	    PM_mtx.lock();
	    shared->packs[id]+=1;
	    queue_to_plane
	[id].front()->nPacks-=1;
	    PM_mtx.unlock();
	}
    }
    human_cv.notify_all();
    PM_cv.notify_all();

    checkin_cv.notify_one();
}

int main() {
    srand(time(NULL));
    window = new Window();
    shared = new Shared(nPM);

    for(int i=0; i<nPM; i++) 
	destPM[i]=16;

    std::vector<std::thread> humans;
    std::vector<std::thread> clients;
    std::vector<std::thread> PMs;

    std::thread winThread(updateWindow);
    std::thread exitThread(stop);
    std::thread firmThread(giveTicket);
    for(int i=0; i<nHuman; i++) {
	shared->addHuman(rand()% 20000 + 50000);
	humans.push_back(std::thread(humanMoving, i));
	
    }
   
    for(int i=0; i<nPM; i++)
	PMs.push_back(std::thread(takeTicket, i));


    for(int i=0; i<nHuman; i++) humans[i].join();
    for(int i=0; i<nPM; i++) PMs[i].join();
    for(int i=0; i<nPM; i++) clients[i].join();
    firmThread.join();
    winThread.join();
    exitThread.join();
    delete window;
    delete shared;
    return 0;
}






















