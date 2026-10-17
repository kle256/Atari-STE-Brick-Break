/* asynch module. TODO: fill in each function and document assumptions. */
#include "asynch.h"

/* Sets paddle direction to left*/
void movLft(Model *model) {
    setPDir(&model->paddle, -1);
}

/* Sets paddle direction to right*/
void movRgt(Model *model) {
    setPDir(&model->paddle, 1);
}

