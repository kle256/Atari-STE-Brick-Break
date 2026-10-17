/* Conditional event handlers for collisions and game progression. */
#include "cond.h"

#define SCRWIDTH 640
#define SCRHEIGHT 400
#define OFFSET 10

/* Private helper functions for collision detection*/
static void padBnce(Model *model);
static void brkBnce(Model *model, Brick *brick);

/* Reverses ball vertical velocity at top wall collision*/
void btopEdge(Model *model) {
    if (model->ball.y <= 0 && model->ball.velo_y < 0) {
        model->ball.y = 0;
        flipVy(&model->ball);
    }
}

/* Reverses ball horizontal velocity at left wall collision */
void blftEdge(Model *model) {
    if (model->ball.x <= 0 && model->ball.velo_x <0) {
        model->ball.x = 0;
        flipVx(&model->ball);
    } 
}

/* Reverses ball horizontal velocity at right wall collision */
void brgtEdge(Model *model) {
    int maxXPos = SCRWIDTH - model->ball.width;

    /* Account width of ball to prevent clipping*/
    if (model->ball.x >= maxXPos && model->ball.velo_x > 0) {
        model->ball.x = maxXPos;
        flipVx(&model->ball);
    }
}


/*
 * Logic for missed ball bellow paddle
 * Deducts life and resets ball and paddle position
 * Ends the game when no lives remian
*/
void outBnds(Model *model) {
    int yOutBnds;

    /* Sets boundary for missed ball bellow paddle*/
    yOutBnds = model->paddle.y;
    yOutBnds += model->paddle.height;
    yOutBnds += OFFSET;

    /* trigger only when ball is moveing downwards*/
    if (model->ball.y >= yOutBnds && model->ball.velo_y > 0) {
        loseLife(&model->lives);
        if (model->lives.value <= 0) {
            endGame(model, GAME_LOST);
        } else {
            rstBP(model);
        }
    }
}

/* Prevents Paddle from clipping past left wall */
void plftEdge(Model *model) {
    if (model->paddle.x <= 0) {
        model->paddle.x = 0;
        setPDir(&model->paddle, 0);
    }
}

/* Prevents paddle from clipping past right wall */
void prgtEdge(Model *model) {
    int maxX = SCRWIDTH - model->paddle.width;

    /* Stop paddle movement at boundary point.*/
    if (model->paddle.x >= maxX) {
        model->paddle.x = maxX;
        setPDir(&model->paddle, 0);
    }
    
}

/*
 * Detects ball collision with top of paddle
 * Reposition ball above paddle and
 * bounce direction based on impact position
*/
void bpadCol(Model *model) {
    int ballRgt, ballBot, padBot, padRgt, ballRep, prevBot;

    /* Calculate right and bottom boundaries of both objects*/
    ballRgt = model->ball.x + model->ball.width;
    ballBot = model->ball.y + model->ball.height;
    padRgt = model->paddle.x + model->paddle.width;
    padBot = model->paddle.y + model->paddle.height;
    
    /* Determine ball bottom position before last tick.*/
    prevBot = ballBot - model->ball.velo_y;

    /* Ball Paddle horizontal overlap check*/
    if (ballRgt > model->paddle.x && model->ball.x < padRgt) {

        /* Ball Paddle vertical overlap check*/
        if (ballBot > model->paddle.y && model->ball.y < padBot) {

            /* Only bounce if ball collides with top of paddle*/
            if (model->ball.velo_y > 0 && prevBot <= model->paddle.y) {

                /* Move ball above paddle to remove overlap*/
                ballRep = model->paddle.y - model->ball.height;
                model->ball.y = ballRep;
                
                /* Calclulate bounce direction from impact position*/
                padBnce(model);
            }
        }
    }
}

/* 
 * Detects collision with active brick
 * Update ball velocity, award points, and destroy brick
 * when both ball and brick overlap
*/
void bbrkCol(Model *model, int brick_index) {
    Brick *brick;
    int brkRgt, brkBot, ballRgt, ballBot;

    /* Guard clause: invalid array access*/
    if (brick_index < 0 || brick_index >= model->brkCnt) {
        return;
    }

    /* Get pointer to selected brick*/
    brick = &model->bricks[brick_index];

    /* Destroyed bricks can't be hit again*/
    if(brick->active == 0) {
        return;
    }

    /* Calculate brick and ball boundaries*/
    brkRgt = brick->x + brick->width;
    brkBot = brick->y + brick->height;
    ballRgt = model->ball.x + model->ball.width;
    ballBot = model->ball.y + model->ball.height;

    /* Horizontal overlap check*/
    if (ballRgt > brick->x && model->ball.x < brkRgt) {

        /* Vertical overlap check*/
        if (ballBot > brick->y && model->ball.y < brkBot) {

            /* Determine ball bounce direction*/
            brkBnce(model, brick);

            /* Increment score based on brick score tier*/
            addScore(&model->score, brick->points);

            /* Mark Brick as destroyed*/
            dstrBrk(brick);

            /* Update remaining bricks in level*/
            model->brksLeft -= 1;
        }
    }

}

/* 
 * Check whether all bricks in current level are destroyed
 * Advance to next level or complete game
*/
void brksDest(Model *model) {
    /* Do not advance level if game is ended*/
    if (model->state != GAME_PLAYING ) {
        return;
    }

    /* Do not advance level if bricks remain*/
    if (model->brksLeft > 0) {
        return;
    }

    /* Advance if another level exists, else game ends*/
    if (model->currLvl < TOTAL_LEVELS) {
        nxtLvl(model);
    } else {
        lvlsComp(model);
    }
}

/* Mark game is finished after final level */
void lvlsComp(Model *model) {
    endGame(model, GAME_WON);
}

/* 
 * Determine ball bounce direction based on where ball lands on paddle
 *
 * Paddle divided in five equal zones
 */
static void padBnce(Model *model) {
    int bCentX, hlfWdth, impctX, zoneWdth, zone, veloX;

    /* Calculate ball horizontal center*/
    bCentX = model->ball.x;
    hlfWdth = model->ball.width / 2;
    bCentX += hlfWdth;

    /* Calculate impact position based on paddle left edge*/
    impctX = bCentX - model->paddle.x;

    /* Divide paddle into five zone*/
    zoneWdth = model->paddle.width / 5;
    zone = impctX / zoneWdth;

    /* Keep zone within valid range for edge collisions*/
    if (zone < 0) {
        zone = 0;
    }

    if (zone > 4) {
        zone = 4;
    }

    /* Convert zone to horizontal velocity*/
    veloX = zone - 2;

    /* Send ball upwards with new horizontal velocity*/
    setVel(&model->ball, veloX, -2);

}

/* Determine which brick side was hit. And updates ball velocity*/
static void brkBnce(Model *model, Brick *brick) {
    Ball *ball;
    int prevX, prevY, prevBot, prevRgt, brkRgt, brkBot;

    ball = &model ->ball;

    /* Calculate ball's previous position*/
    prevX = ball->x - ball->velo_x;
    prevY = ball->y - ball->velo_y;
    
    /* Calculate ball previous boundaries*/
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