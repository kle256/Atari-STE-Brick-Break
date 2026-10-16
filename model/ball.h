#ifndef BALL_H
#define BALL_H

typedef struct { /* Struct for ball*/
    unsigned int x, y;
    int velo_x;
    int velo_y;
    int width;
    int height;
} Ball;

/*Ball movement*/
void movBall(Ball *ball); /* Update ball position based on velocity*/

/* Ball horizontal and vertical bounce*/
void flipVx(Ball *ball);   /* Reverse ball horizontal velocity*/
void flipVy(Ball *ball);   /* Reverse ball vertical velocity*/     

/* Ball velocity*/
void setVel(Ball *ball, int vx, int vy); /* Set the ball horizontal or vertical velocity*/

/* Reset ball */
void rstBall(Ball *ball, int x, int y); /* Reset balls coordinates to set coordinates*/

#endif