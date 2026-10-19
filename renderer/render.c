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


/* Renders the entire game model to the screen. */
void render(Model *model, UINT8 *base) {
    UINT32 *drawBase;
    Brick * brick;
    int i;

    drawBase = (UINT32 *)base;
    clear_screen(drawBase);

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