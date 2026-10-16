#include "score.h"

/* Increments Score */
void addScore(Score *score, int points)
{
    score->value += points;
}

/* Resets Score to 0 */
void rstScore(Score *score)
{
    score->value = 0;
}

