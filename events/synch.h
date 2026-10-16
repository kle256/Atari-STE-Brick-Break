/* Clock-tick event handlers; tests can call them directly. */
#ifndef SYNCH_H
#define SYNCH_H
#include "model.h"

void handle_ball_tick(Model *model);
void handle_paddle_tick(Model *model);
#endif
