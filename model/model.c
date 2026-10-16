/* model module. TODO: fill in each function and document assumptions. */
#include "model.h"

/* TODO: document caller-visible behaviour before implementing. */
void initMod(Model *model) {
    model->state = GAME_PLAYING;
    model->currLvl = 1;
    
    rstBall();
}

/* TODO: document caller-visible behaviour before implementing. */
void rstBP(Model *model)
{
    /* TODO: implement. These casts only silence unused-argument warnings. */
    (void)model;
}

/* TODO: document caller-visible behaviour before implementing. */
void nxtLvl(Model *model)
{
    /* TODO: implement. These casts only silence unused-argument warnings. */
    (void)model;
}

/* TODO: document caller-visible behaviour before implementing. */
void endGame(Model *model, GameState outcome)
{
    /* TODO: implement. These casts only silence unused-argument warnings. */
    (void)model;
    (void)outcome;
}

