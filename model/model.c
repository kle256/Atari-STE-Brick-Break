/* model module. TODO: fill in each function and document assumptions. */
#include "model.h"

/* Ball dimensions constants*/
#define BWIDTH 8
#define BHEIGHT 8

/* Ball starting height and velocity*/
#define BGAP 20
#define BVELX 0
#define BVELY 2

/* Paddle dimension and speed*/
#define PWIDTH 80
#define PHEIGHT 12
#define PSPEED 4

/* Level 1 Brick layout*/
#define BRKROWS 4
#define BRKCOLS 8

#define BRKWIDTH 64
#define BRKHEIGHT 16

#define BRKGAPX 4
#define BRKGAPY 4

#define BRKSTRTX 50
#define BRKSTRTY 50

#define BRKSHIFT 16

static void loadBrks(Model *model, int level);
static void initBrk(Brick *brick, int x, int y, int row);

/* Initializes the models for the game*/
void initMod(Model *model) {
    /* Ini game state*/
    model->state = GAME_PLAYING;
    model->currLvl = 1;
    
    /* Ini ball*/
    model->ball.width = BWIDTH;
    model->ball.height = BHEIGHT;
    
    /* Ini Paddle*/
    model->paddle.width = PWIDTH;
    model->paddle.height = PHEIGHT;
    model->paddle.speed = PSPEED;
    
    rstBP(model);
    rstScore(&model->score);
    rstLives(&model->lives);

    loadBrks(model, model->currLvl);

    /* TODO: Initialize score and lives display properties */
    /* TODO: Initialize Level 1 bricks and brkCnt */
}

/* TODO: document caller-visible behaviour before implementing. */
void rstBP(Model *model) {
    int x, y, pCent;

    rstPad(&model->paddle);

    /* Center ball directly above paddle*/
    x = model->paddle.x;
    pCent = model->paddle.width - model->ball.width;
    pCent = pCent / 2;
    x += pCent;

    /* Set ball above paddle*/
    y = model->paddle.y;
    y -= model->ball.height;
    y -= BGAP;

    rstBall(&model->ball, x, y);
    setVel(&model->ball, BVELX, BVELY);
}

/* TODO: document caller-visible behaviour before implementing. */
void nxtLvl(Model *model) {
    if (model->currLvl < TOTAL_LEVELS) {
        model->currLvl += 1;
        loadBrks(model, model->currLvl);
        rstBP(model);
    } else {
        endGame(model, GAME_WON);
    }
    
}

/* TODO: document caller-visible behaviour before implementing. */
void endGame(Model *model, GameState outcome) {
    model->state = outcome;
}

static void loadBrks(Model *model, int level) {
    int row, col, index, x, y, isOdd;

    model->brkCnt = BRKROWS * BRKCOLS;
    model->brksLeft = model->brkCnt;

    for (row = 0; row < BRKROWS; row++) {
        y = BRKHEIGHT;
        y += BRKGAPY;
        y = y * row;
        y += BRKSTRTY;
        isOdd = row % 2;
        
        for (col = 0; col < BRKCOLS; col++) {
            index = row * BRKCOLS + col;
            
            /* Calculate brick's horizontal position */
            x = BRKWIDTH;
            x += BRKGAPX;
            x = x * col;
            x += BRKSTRTX;

            if (level == 2 && isOdd == 1) {
                x += BRKSHIFT;
            }

            initBrk(&model->bricks[index], x, y, row);
            
        }

    }

}

static void initBrk(Brick *brick, int x, int y, int row) {
    int points;

    /* Set position of brick*/
    brick-> x = x;
    brick-> y = y;

    brick-> width = BRKWIDTH;
    brick-> height = BRKHEIGHT;
    brick-> type = (BrickType)row;

    /* Calculate and assign points*/
    points = 4;
    points -= row;
    points = points * 10;
     brick-> points = points;

    /* Set brick to alive*/
    rstBrick(brick);
}