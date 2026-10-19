/*
 Author: Kevin Le
 Date: October 19, 2026
 Purpose: Handles score updates and reset.
 */

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

