#include "Ball.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <iostream>


Ball::Ball(int x, int y, speed ballSpeed, directon ballDirection)
{
    this->X = x;
    this->Y = 1;
    setDirection(ballDirection);
    setSpeed(fast);
}


void Ball::move()
{
    int screenX, screenY; 
    getmaxyx(stdscr, screenY, screenX);
    screenY--;
    
    if (this->X >= screenX  || this->X <= 0) {
            xDirection = -xDirection;
            slowdown = slowdown*2;
        }

        if (this->Y >= screenY || this->Y <= 0) {
            yDirection = -yDirection;
            slowdown = slowdown*2;
        }

        Y += yDirection;
        X += xDirection;      
        std::this_thread::sleep_for(std::chrono::milliseconds(slowdown));   

    return;
}

int Ball::getX()
{
    return this->X;
}

int Ball::getY()
{
    return this->Y;
}

void Ball::setDirection(directon ballDirection) 
{

    switch (ballDirection)
    {
        case up:
            xDirection = 0;
            yDirection = -1;
            break;

        case left:
            xDirection = -1;
            yDirection = 0;
            break;

        case down:
            xDirection = 0;
            yDirection = 1;
            break;

        case right:
            xDirection = 1;
            yDirection = 0;
            break;

        case up_left:
            xDirection = -1;
            yDirection = -1;
            break;

        case up_right:
            xDirection = 1;
            yDirection = -1;
            break;

        case down_left:
            xDirection = -1;
            yDirection = 1;
            break;

        case down_right:
            xDirection = 1;
            yDirection = 1;
            break;

        default:
            xDirection = 0;
            yDirection = 0;

    }
}

void Ball::setSpeed(speed ballSpeed)
{
    switch (ballSpeed)
    {
        case slow:
            slowdown = 200;
            break;

        case medium:
            slowdown = 100;
            break;

        case fast:
            slowdown = 15;
            break;

        default:
            slowdown = 15;

    }
}

Ball::~Ball()
{}



