/* Score state and updates. */
#ifndef SCORE_H
#define SCORE_H

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int value;
} Score;

void addScore(Score *score, int points); /* Increments displayed score*/
void rstScore(Score *score);             /* Resets score back to zero*/
#endif
