/* Clock-tick event handlers; tests can call them directly. */
#ifndef SYNCH_H
#define SYNCH_H
#include "model.h"

void bTick(Model *model); /* Clock ticks to update ball position*/
void pTick(Model *model); /* Clock ticks to update paddle position*/
#endif
