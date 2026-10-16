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

void add_score(Score *score, int points);
void reset_score(Score *score);
#endif
