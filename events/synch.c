/*
Author: Kevin Le
Date: October 19,2026
Purpose: Handles timed ball and paddle movement
*/

#include "synch.h"

/* Update ball position based on ball velocity*/
void bTick(Model *model) {
    movBall(&model->ball);
}

/* Moves paddle and sets direction to 0 after each tick*/
void pTick(Model *model) {
    movPad(&model->paddle);
    setPDir(&model->paddle, 0);
}

