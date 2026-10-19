/*
Author: Kevin Le
Date: October 19,2026
Purpose: Handles ball movement, velocity, and reset
*/
#include "asynch.h"

/* Sets paddle direction to left*/
void movLft(Model *model) {
    setPDir(&model->paddle, -1);
}

/* Sets paddle direction to right*/
void movRgt(Model *model) {
    setPDir(&model->paddle, 1);
}

