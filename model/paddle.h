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

void set_paddle_direction(Paddle *paddle, int direction);
void move_paddle(Paddle *paddle);
void reset_paddle(Paddle *paddle);
#endif
