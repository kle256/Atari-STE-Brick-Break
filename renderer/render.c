
/*
 * Renderer module.
 * Draws the current game model using the raster library.
 * Author: Kevin Le
 */

#include "render.h"
#include "raster.h"

#define BSIZE 8

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
void render(const Model *model, UINT8 *base, const UINT8 *font) {


   
}

/* Draws the ball using its position and size. */
void rendBall(const Ball *ball, UINT8 *base) {
    plot_8bit_bitmap(base, ball->y, ball->x, ballBmp, BSIZE);
    
}

/* Draws the paddle using its position and size. */
void rendPad(const Paddle *paddle, UINT8 *base) {
    UINT32 *drawBase;
    int row, drawRow;

    drawBase = (UINT32 *)base;

    for (row = 0; row < paddle->height; row++) {
        drawRow = paddle->y + row;
        plot_horizontal_line(drawBase, drawRow, paddle->x, paddle->width);
    }
}

/* Draws a brick if it has not been destroyed. */
void rendBrk(const Brick *brick, UINT8 *base)
{
    /* TODO: Render brick. */
    (void)brick;
    (void)base;
}

/* Draws the current score using the bitmap font. */
void rendScor(const Score *score, UINT8 *base, const UINT8 *font)
{
    /* TODO: Render score. */
    (void)score;
    (void)base;
    (void)font;
}

/* Draws the remaining lives using the bitmap font. */
void rendLife(const Lives *lives, UINT8 *base, const UINT8 *font)
{
    /* TODO: Render lives. */
    (void)lives;
    (void)base;
    (void)font;
}