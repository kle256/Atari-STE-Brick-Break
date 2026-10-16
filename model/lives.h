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

void loseLife(Lives *lives);  /* Decrease player lives by one*/
void rstLives(Lives *lives);  /* Reset players lives back to three*/
#endif
