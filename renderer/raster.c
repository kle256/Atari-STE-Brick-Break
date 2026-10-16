
/*
 * Renderer module.
 * Draws the current game model using the raster library.
 * Author: Kevin Le
 */

#include "render.h"
#include "raster.h"

/* Renders the entire game model to the screen. */
void render(const Model *model, UINT8 *base, const UINT8 *font)
{
    /* TODO: Render all game objects. */
    (void)model;
    (void)base;
    (void)font;
}

/* Draws the ball using its position and size. */
void rendBall(const Ball *ball, UINT8 *base)
{
    /* TODO: Render ball. */
    (void)ball;
    (void)base;
}

/* Draws the paddle using its position and size. */
void rendPad(const Paddle *paddle, UINT8 *base)
{
    /* TODO: Render paddle. */
    (void)paddle;
    (void)base;
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