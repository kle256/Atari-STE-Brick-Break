/*
 Author: Kevin Le
 Date: October 19, 2026
 Purpose: Draws game objects using raster library
 */

#include "render.h"
#include "raster.h"
#include <stdio.h>


#define BSIZE 8
#define STXTLEN 24
#define MSGROW 192
#define MSGCOL 280

/* 8x8 ball bitmap */
static const UINT8 ballBmp[BSIZE] = {
    0x3C,
    0x7E,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x7E,
    0x3C
};

static int bOvrLapBrk(Ball *ball, Brick *brick);
static void updtScor(Model *model, Model *prev, UINT8 *base);
static void updtLfe(Model *model, Model *prev, UINT8 *base);


/* Renders the entire game model to the screen. */
void render(Model *model, UINT8 *base) {
    UINT32 *drawBase;
    Brick * brick;
    int i;

    drawBase = (UINT32 *)base;
    clear_screen(drawBase);

    /* Draw top edge below score and lives */
    plot_horizontal_line(drawBase, 39, 0, 640);

    /* Render all active bricks*/
    for (i = 0; i < model->brkCnt; i++) {
        brick = &model->bricks[i];
        rendBrk(brick, base);
    }

    /* Render game objects*/
    rendBall(&model->ball, base);
    rendPad(&model->paddle, base);
    rendScor(&model->score, base);
    rendLife(&model->lives, base);
    rendState(model->state, base);
   
}

/* Draws the ball using its position and size. */
void rendBall(Ball *ball, UINT8 *base) {
    plot_8bit_bitmap(base, ball->y, ball->x, ballBmp, BSIZE);
    
}

/* Draws the paddle using its position and size. */
void rendPad(Paddle *paddle, UINT8 *base) {
    UINT32 *drawBase;
    int row, drawRow;

    drawBase = (UINT32 *)base;

    for (row = 0; row < paddle->height; row++) {
        drawRow = paddle->y + row;
        plot_horizontal_line(drawBase, drawRow, paddle->x, paddle->width);
    }
}

/* Draws a brick if it has not been destroyed. */
void rendBrk(Brick *brick, UINT8 *base) {
    UINT32 *drawBase;
    int row, drawRow;

    if (brick->active == 0) {
        return;
    }

    drawBase = (UINT32 *)base;

    for (row = 0; row < brick->height; row++) {
        drawRow = brick->y + row;
        plot_horizontal_line(drawBase, drawRow, brick->x, brick->width);
    }
}

/* Draws the current score using the bitmap font. */
void rendScor(Score *score, UINT8 *base) {
    char scoreTxt[STXTLEN];

    sprintf(scoreTxt, "Score: %d", score->value);

    plot_string(base, score->y, score->x, scoreTxt);
}

/* Draws the remaining lives using the bitmap font. */
void rendLife(Lives *lives, UINT8 *base) {
    char lifeTxt[STXTLEN];

    sprintf(lifeTxt, "Lives: %d", lives->value);

    plot_string(base, lives->y, lives->x, lifeTxt);
}

/* Draw game won or game over message on screen*/
void rendState(GameState state, UINT8 *base) {
    if (state == GAME_WON) {
        plot_string(base, MSGROW, MSGCOL, "YOU WIN!");
    
    } else if (state == GAME_LOST) {
        plot_string(base, MSGROW, MSGCOL, "GAME OVER");
    }
    
}

/* Update from using current and previous drawn models*/
void rndUpdt(Model *model, Model *prev, UINT8 *base) {
    UINT32 *drawBase;
    Brick *brick;
    int i;

    drawBase = (UINT32 *)base;

    /*Draw full frame on level update */
    if (model->currLvl != prev->currLvl) {
        render(model, base);

        return;
    }

    /* Draw full frame when game state changes */
    if (model->state != prev->state) {
        render(model, base);
        return;
    }

    clear_region(drawBase, prev->ball.y, prev->ball.x, prev->ball.height,
                prev->ball.width);
    clear_region(drawBase, prev->paddle.y, prev->paddle.x, prev->paddle.height,
                prev->paddle.width);

    /* Clear bricks destroyed from previous screen */
    for (i =0; i < prev->brkCnt; i++) {
        if (prev->bricks[i].active && !model->bricks[i].active) {
            clear_region(drawBase, prev->bricks[i].y, prev->bricks[i].x,
                        prev->bricks[i].height, prev->bricks[i].width);
        }
    }

    /* Restore bricks touched by previous ball*/
    for (i = 0; i < model->brkCnt; i++) {
        brick = &model->bricks[i];

        if (brick->active) {
            if (bOvrLapBrk(&prev->ball, brick)) {
                rendBrk(brick, base);
            }
        }
    }
    updtScor(model, prev, base);
    updtLfe(model, prev, base);

    /* Draw ball and paddle at current postions */
    rendBall(&model->ball, base);
    rendPad(&model->paddle, base);

    
}

/* Checks whether ball and brick overlap */
static int bOvrLapBrk(Ball *ball, Brick *brick) {
    int ovrLapX, 
        ovrLapY,
        bRgt,
        bBot,
        brkRgt,
        brkBot;

        /* Calc ball right and bottom edges*/
        bRgt = ball->x + ball->width;
        bBot = ball->y + ball->height;

        /* Calc brick right and bottom edges*/
        brkRgt = brick->x + brick->width;
        brkBot = brick->y + brick->height;

        /* Check horizontal overlap*/
        ovrLapX = ball->x < brkRgt;
        ovrLapX = ovrLapX && bRgt > brick->x;

        /* Check vertical overlap*/
        ovrLapY = ball->y < brkBot;
        ovrLapY = ovrLapY && bBot > brick->y;

        /* Return true if both directions overlap*/
        return ovrLapX && ovrLapY;



}

/* Updates score if value changed*/
static void updtScor(Model *model, Model *prev, UINT8 *base) {
    UINT32 *drawBase;
    drawBase = (UINT32 *)base;

    if (model->score.value != prev->score.value) {
        clear_region(drawBase, prev->score.y, prev->score.x, prev->score.height,
                    prev->score.width);
        
        rendScor(&model->score, base);
    }
}
/* Updates life if value changed*/
static void updtLfe(Model *model, Model *prev, UINT8 *base) {
    UINT32 *drawBase;
    drawBase = (UINT32 *)base;

    if(model->lives.value != prev->lives.value) {
        clear_region(drawBase, prev->lives.y, prev->lives.x, prev->lives.height,
                    prev->lives.width);

        rendLife(&model->lives, base);
    }
}