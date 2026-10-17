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

