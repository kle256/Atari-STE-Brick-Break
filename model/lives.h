/* Lives state and updates. */
#ifndef LIVES_H
#define LIVES_H

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int value;
} Lives;

void lose_life(Lives *lives);
void reset_lives(Lives *lives);
#endif
