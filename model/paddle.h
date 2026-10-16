/* Paddle model: player-directed horizontal movement. */
#ifndef PADDLE_H
#define PADDLE_H

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int speed;
    int direction;  /* -1 = left, 0 = still, 1 = right. */
} Paddle;

void setPDir(Paddle *paddle, int direction); /* Set paddles horizontal movement*/
void movPad(Paddle *paddle);                 /* Updates paddle position*/
void rstPad(Paddle *paddle);                 /* Reset paddle to start position*/
#endif
