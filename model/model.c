/*
 Author: Kevin Le
 Date: October 19, 2026
 Purpose: Initializes the game model and manages level progression.
 */
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

/*HUD positions and dimensions*/
#define SCOREX 16
#define LIVEX 520
#define HUDY 16
#define HUDWIDTH 80
#define HUDHEIGHT 16

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

    /* Initialize score display */
    model->score.x = SCOREX;
    model->score.y = HUDY;
    model->score.width = HUDWIDTH;
    model->score.height = HUDHEIGHT;

    /* Initialize lives display */
    model->lives.x = LIVEX;
    model->lives.y = HUDY;
    model->lives.width = HUDWIDTH;
    model->lives.height = HUDHEIGHT;

    loadBrks(model, model->currLvl);
}

/* Resets ball and paddle to start position and velocity */
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

/* Advance game to next level or end game if all levels done */
void nxtLvl(Model *model) {
    if (model->currLvl < TOTAL_LEVELS) {
        model->currLvl += 1;
        loadBrks(model, model->currLvl);
        rstBP(model);
    } else {
        /* All levels done*/
        endGame(model, GAME_WON);
    }
    
}

/* Sets game state to outcome */
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