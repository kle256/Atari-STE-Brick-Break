/* cond module. TODO: fill in each function and document assumptions. */
#include "cond.h"

#define SCRWIDTH 640
#define SCRHEIGHT 400
#define OFFSET 10

/* top wall ball collision logic*/
void btopEdge(Model *model) {
    if (model->ball.y <= 0 && model->ball.velo_y < 0) {
        model->ball.y = 0;
        flipVy(&model->ball);
    }
}

/* TODO: document caller-visible behaviour before implementing. */
void blftEdge(Model *model) {
    if (model->ball.x <= 0 && model->ball.velo_x <0) {
        model->ball.x = 0;
        flipVx(&model->ball);
    } 
}

/* TODO: document caller-visible behaviour before implementing. */
void brgtEdge(Model *model) {
    int maxXPos = SCRWIDTH - model->ball.width;

    if (model->ball.x >= maxXPos && model->ball.velo_x > 0) {
        model->ball.x = maxXPos;
        flipVx(&model->ball);
    }
}

/* TODO: document caller-visible behaviour before implementing. */
void outBnds(Model *model) {
    int yOutBnds;

    yOutBnds = model->paddle.y;
    yOutBnds += model->paddle.height;
    yOutBnds += OFFSET;

    if (model->ball.y >= yOutBnds && model->ball.velo_y > 0) {
        loseLife(&model->lives);
        if (model->lives.value <= 0) {
            endGame(model, GAME_LOST);
        } else {
            rstBP(model);
        }
    }
}

/* TODO: document caller-visible behaviour before implementing. */
void plftEdge(Model *model) {
    if (model->paddle.x <= 0) {
        model->paddle.x = 0;
        setPDir(&model->paddle, 0);
    }
}

/* TODO: document caller-visible behaviour before implementing. */
void prgtEdge(Model *model)
{
    /* TODO: implement. These casts only silence unused-argument warnings. */
    (void)model;
}

/* TODO: document caller-visible behaviour before implementing. */
void bpadCol(Model *model)
{
    /* TODO: implement. These casts only silence unused-argument warnings. */
    (void)model;
}

/* TODO: document caller-visible behaviour before implementing. */
void bbrkCol(Model *model, int brick_index)
{
    /* TODO: implement. These casts only silence unused-argument warnings. */
    (void)model;
    (void)brick_index;
}

/* TODO: document caller-visible behaviour before implementing. */
void brksDest(Model *model)
{
    /* TODO: implement. These casts only silence unused-argument warnings. */
    (void)model;
}

/* TODO: document caller-visible behaviour before implementing. */
void lvlsComp(Model *model)
{
    /* TODO: implement. These casts only silence unused-argument warnings. */
    (void)model;
}

