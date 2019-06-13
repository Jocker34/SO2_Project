#include "direction.h"
#include "speed.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <ncurses.h>

#pragma once 
class Ball 
{
    private:
        int X;
        int Y; 
        double speedX;
        double speedY;
        int xDirection;
        int yDirection;
        int slowdown;
    public:
        Ball(int,int);
        Ball(int x, int y, speed ballSpeed, directon ballDirection);
        ~Ball();
        void move();
        int getX();
        int getY();
        void setDirection(directon ballDirection);
        void setSpeed(speed ballSpeed);
};
