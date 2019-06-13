#include<ncurses.h>
#include <vector>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include "Ball.h"
#include <mutex>
#include <condition_variable>

using namespace std;
 
mutex mtx, mtx2;
condition_variable cv, cv2; 
vector <Ball*> balls;
vector <thread> ballsThreads;
int amountBalls = 30;
int ballsGenerations = 1000;
int windowX, windowY, squareX, squareY;
bool running = true;
int squareHeigh = 8;
bool ballWaitingForMutex = false;
int ballsInMutex=0;

void pressButtonToExit()
{
	while (running) 
	{
		char key = getch();
		if (key == 'x')
		running = false;
		cv2.notify_all();
		cv.notify_all();
	}
	
}


bool ballInMutex(int ball)
{
	int ballX = balls[ball]->getX();
	int ballY = balls[ball]->getY();

	if ( (ballX > squareX && ballX <= squareX + 10) && (ballY > squareY && ballY <= squareY + squareHeigh)) //10 to ilosc znakow z lancuchu ____
		return true;
	else 
		return false;
}

void ballThreadFunction(int ball)
{
	bool ballLeavingMutex = false;
	
	while(running)
	{
		balls[ball]->move();
		this_thread::sleep_for(std::chrono::milliseconds(50));
		if (ballInMutex(ball)) 
		{ 
			if (ballLeavingMutex)
				continue;

			unique_lock<mutex> lck2(mtx2);
			
			if (ballsInMutex > 0) //dla pierwszego watku w prostakacie
				ballWaitingForMutex = true;
			ballsInMutex++;
			if (ballsInMutex == 2)
			{
				//balls[ball]->move();
				//this_thread::sleep_for(std::chrono::milliseconds(50));
				cv.notify_one();
			}
			while (ballsInMutex == 2 && running) {
				cv2.wait(lck2);
			}
			lck2.unlock();
			
			
			
			unique_lock<mutex> lck(mtx);
			
			//cv.wait(lck, shouldBallLeaveThread());
			//unique_lock<mutex> lck(mtx);
			while(!ballWaitingForMutex && running)
			{
				cv.wait(lck);
			}
			ballLeavingMutex = true;
			ballWaitingForMutex = false;
			ballsInMutex--;
			cv2.notify_one();
			lck.unlock();
							
		}
		else 
			ballLeavingMutex = false;
			
	}
}

void generateBalls()
{
	getmaxyx(stdscr, windowY, windowX);
	for (int i = 0 ; i < amountBalls; i++) 
	{
        directon ballDirection = static_cast<directon>(rand() % 8);
        speed ballSpeed = static_cast<speed>(rand() % 2);
		balls.push_back(new Ball(windowX/2, windowY/2, ballSpeed, ballDirection));
		ballsThreads.push_back(std::thread(ballThreadFunction, i));
		if (!running) break;
		this_thread::sleep_for(std::chrono::milliseconds(ballsGenerations));
	}
}

void drawSquare()
{
	getmaxyx(stdscr, windowY, windowX);	
	squareX = windowX/3;
	squareY = windowY/4;
	mvaddstr(squareY, squareX, "__________"); //10 "podlog"
	mvaddstr(squareY + squareHeigh, squareX, "__________");
	for (int i = 1; i <= squareHeigh ; i++) 
	{
		mvaddch(squareY + i, squareX, '|');
		mvaddch(squareY + i, squareX +10, '|');
	}
}

void renderScreen()
{
	while (running)
	{
		clear();
		drawSquare();

		for (int i = 0; i < balls.size(); i++)
		{
			mvprintw(balls[i]->getY(), balls[i]->getX(), "o");	
		}
		refresh();
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}
}

int main(int argc, char const * argv [])
{	

	srand (time(NULL));
	initscr();
	curs_set(0); //Cursor visability
	thread renderScreenThread(renderScreen);
	thread generateBallsThread(generateBalls);
	thread pressButtonToExitThread(pressButtonToExit);
	

	renderScreenThread.join();
	generateBallsThread.join();
	pressButtonToExitThread.join();

	for (int i = 0; i < ballsThreads.size(); i++)
	{
		ballsThreads[i].join();
		
	}
	
	endwin();
	return 0; 
}
