#include "Window.h"
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <time.h>

Window::Window() {
	initWindow();
}

Window::~Window() {
	endwin();
}

void Window::initWindow() {
    initscr();
    noecho();
    curs_set(FALSE);
    clear();
    refresh();
}

void Window::human(int id, int dir, int x, int y, int packs) {
	if(packs == 1 ) {
	mvprintw(y-2, x-1, "( )");
	mvprintw(y-1, x-1, "/|\\");
	mvprintw(y, x, "|");
	mvprintw(y+1, x-1, "/ \\[]");
	}
	else {
	mvprintw(y-2, x-1, "( )");
	mvprintw(y-1, x-1, "/|\\");
	mvprintw(y, x, "|");
	mvprintw(y+1, x-1, "/ \\");
	}
	
}

void Window::clearHuman(int x, int y) {
	for(int i=x-3; i<=x+4; i++)
		for(int j=y-2; j<=y+1; j++)
			mvprintw(j, i, " ");
}

void Window::plane(int id) {
	int y=2+10*id, x=10;
	id+=1;
	mvprintw(y-1, x-8, "\\\\\\\\");
	mvprintw(y, x-6, "\\\\\\\\");
	mvprintw(y+1, x-4, "XXXXXX>");
	mvprintw(y+2, x-6, "////");
	mvprintw(y+3, x-8, "////");


	// for(int i=x-2; i<=x+2; i++) {
	// 	for(int j=y-1; j<=y+1; j++){
	// 		if(i==x && j==y) mvprintw(j, i, "%d",id);
	// 		else mvprintw(j, i, "X");
	// 	}
	// }
}

void Window::firm() {
	int y=13, x=80;
	mvprintw(y-2, x-6, "|||||||||||||");
	for(int i=1; i>=-1; i--) {
		mvprintw(y-i, x-6, "||");
		mvprintw(y-i, x+5, "||");
	}
	mvprintw(y+2, x-6, "|||||||||||||");
	mvprintw(y, x-3, "ODPRAWA");


	y=0, x=100;
	for(int i=0; i<=30; i++) {
		mvprintw(y+i, x-6, "H");
		//mvprintw(y-i, x+5, "||");
	}

	y=30, x=100;
	for(int i=0; i<=50; i++) {
		mvprintw(y, x-6-i, "H");
		//mvprintw(y-i, x+5, "||");
	}

	y=30, x=50;
	for(int i=26; i>=0; i--) {
		mvprintw(y-i, x-6, "H");
		//mvprintw(y-i, x+5, "||");
	}
	refresh();
}

	

void Window::scene(int nPM, int nCar) {
	firm();
	for(int i=0; i<nPM; i++)
		plane(i);
	refresh();
}

void Window::update(Shared *shared) {
	int dir;
	//ruch samochodu
	for(int i=0; i<shared->humanCounter; i++)
		clearHuman(shared->humans[i]->lastX, shared->humans[i]->lastY);
	for(int i=0; i<shared->humanCounter; i++) {
		if(shared->humans[i]->destY==13) dir=0;
		else dir=1;
		human(shared->humans[i]->id+1, dir, shared->humans[i]->currX, shared->humans[i]->currY, shared->humans[i]->nPacks);
		//mvprintw(3, 20+i*2, "%d", shared->humans[i]->nPacks);
	}


	refresh();
}



















