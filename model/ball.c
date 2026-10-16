/* 
File: ball.c
Purpose: Implements ball behavior and movement
Author: Kevin Le
*/
#include "ball.h"

/* Moves ball by adding velocity x and velocity y to x and y*/
void movBall(Ball *ball) {
    ball->x += ball->velo_x;
    ball->y += ball->velo_y;

}

/*reverses ball's horizontal velocity*/
void flipVx(Ball *ball) {
    ball->velo_x = ball->velo_x * -1;

}

/* Reverses ball's vertical velocity*/
void flipVy(Ball *ball) {
    ball->velo_y = ball->velo_y * -1;

}

/* Set ball's x and y velocity*/
void setVel(Ball *ball, int vx, int vy) {
    ball->velo_x = vx;
    ball->velo_y = vy;
}

/* resets ball to starting position*/
void rstBall(Ball *ball, int x, int y) {
    ball->x = x;
    ball->y = y;
}