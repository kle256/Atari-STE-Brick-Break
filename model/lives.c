/*
Author: Kevin Le
Date: October 19,2026
Purpose: Handles remaining lives and reset
*/

#include "lives.h"

#define STRTLVS 3

/* Decreases lives by 1 */
void loseLife(Lives *lives)
{
    if (lives->value > 0) {
    lives->value -= 1;
    }
}

/* Resets life back to original lives */
void rstLives(Lives *lives)
{
    lives->value = STRTLVS;
}

