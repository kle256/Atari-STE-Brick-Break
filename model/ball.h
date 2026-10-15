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
void move_ball(Ball *ball);

/* Ball horizontal and vertical bounce*/
void bounce_horiz(Ball *ball);
void bounce_vert(Ball *ball);

/* Ball velocity*/
void set_veloc(Ball *ball, int vx, int vy);

/* Reset ball */
void reset_ball(Ball *ball, int x, int y);

#endif