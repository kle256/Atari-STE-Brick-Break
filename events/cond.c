/* cond module. TODO: fill in each function and document assumptions. */
#include "cond.h"

#define SCRWIDTH 640
#define SCRHEIGHT 400
#define OFFSET 10

static void padBnce(Model *model);
static void brkBnce(Model *model, Brick *brick);

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
void prgtEdge(Model *model) {
    int maxX = SCRWIDTH - model->paddle.width;

    if (model->paddle.x >= maxX) {
        model->paddle.x = maxX;
        setPDir(&model->paddle, 0);
    }
    
}

/* TODO: document caller-visible behaviour before implementing. */
void bpadCol(Model *model) {
    int ballRgt, ballBot, padBot, padRgt, ballRep, prevBot;

    ballRgt = model->ball.x + model->ball.width;
    ballBot = model->ball.y + model->ball.height;
    padRgt = model->paddle.x + model->paddle.width;
    padBot = model->paddle.y + model->paddle.height;
    prevBot = ballBot - model->ball.velo_y;

    if (ballRgt > model->paddle.x && model->ball.x < padRgt) {
        if (ballBot > model->paddle.y && model->ball.y < padBot) {
            if (model->ball.velo_y > 0 && prevBot <= model->paddle.y) {
                ballRep = model->paddle.y - model->ball.height;
                model->ball.y = ballRep;
                
                padBnce(model);
            }
        }
    }
}

/* TODO: document caller-visible behaviour before implementing. */
void bbrkCol(Model *model, int brick_index) {
    Brick *brick;
    int brkRgt, brkBot, ballRgt, ballBot;


    if (brick_index < 0 || brick_index >= model->brkCnt) {
        return;
    }

    brick = &model->bricks[brick_index];

    if(brick->active == 0) {
        return;
    }

    brkRgt = brick->x + brick->width;
    brkBot = brick->y + brick->height;
    ballRgt = model->ball.x + model->ball.width;
    ballBot = model->ball.y + model->ball.height;

    if (ballRgt > brick->x && model->ball.x < brkRgt) {
        if (ballBot > brick->y && model->ball.y < brkBot) {

            brkBnce(model, brick);
            addScore(&model->score, brick->points);
            dstrBrk(brick);
            model->brksLeft -= 1;
        }
    }

}

/* TODO: document caller-visible behaviour before implementing. */
void brksDest(Model *model) {
    if (model->state != GAME_PLAYING ) {
        return;
    }

    if (model->brksLeft > 0) {
        return;
    }

    if (model->currLvl < TOTAL_LEVELS) {
        nxtLvl(model);
    } else {
        lvlsComp(model);
    }
}

/* TODO: document caller-visible behaviour before implementing. */
void lvlsComp(Model *model) {
    endGame(model, GAME_WON);
}

static void padBnce(Model *model) {
    int bCentX, hlfWdth, impctX, zoneWdth, zone, veloX;
    bCentX = model->ball.x;
    hlfWdth = model->ball.width / 2;
    bCentX += hlfWdth;

    impctX = bCentX - model->paddle.x;
    zoneWdth = model->paddle.width / 5;
    zone = impctX / zoneWdth;

    if (zone < 0) {
        zone = 0;
    }

    if (zone > 4) {
        zone = 4;
    }

    veloX = zone - 2;

    setVel(&model->ball, veloX, -2);

}

static void brkBnce(Model *model, Brick *brick) {
    Ball *ball;
    int prevX, prevY, prevBot, prevRgt, brkRgt, brkBot;

    ball = &model ->ball;

    /* Calculate ball's previous position*/
    prevX = ball->x - ball->velo_x;
    prevY = ball->y - ball->velo_y;
    
    prevBot = prevY + ball->height;
    prevRgt = prevX + ball->width;

    brkRgt = brick->x + brick->width;
    brkBot = brick->y + brick->height;

    /* Top collision*/
    if (ball->velo_y > 0 && prevBot <= brick-> y) {
        ball->y = brick->y - ball->height;
        flipVy(ball);

    /* Bottom Collision*/
    } else if (ball->velo_y < 0 && prevY >= brkBot) {
        ball->y = brkBot;
        flipVy(ball);

    /* Left collision*/
    } else if (ball->velo_x > 0 && prevRgt <= brick->x) {
        ball->x = brick->x - ball->width;
        flipVx(ball);

    /* Right collision*/
    } else if (ball->velo_x < 0 && prevX >= brkRgt) {
        ball->x = brkRgt;
        flipVx(ball);

    /* Corner collision*/
    } else {
        flipVy(ball);
    }

}