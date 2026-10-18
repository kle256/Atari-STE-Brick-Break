/*
 * Renderer test driver.
 * Tests ball and paddle rendering in STEEM.
 * Author: Kevin Le
 */

#include <osbind.h>
#include "render.h"
#include "raster.h"

#define BSIZE 8
#define PWIDTH 80
#define PHEIGHT 12

int main(void) {
    UINT8 *base;
    UINT32 *drawBase;
    Ball ball;
    Paddle paddle;

    base = (UINT8 *)Physbase();
    drawBase = (UINT32 *)base;

    /* Initialize ball. */
    ball.x = 316;
    ball.y = 196;
    ball.width = BSIZE;
    ball.height = BSIZE;
    ball.velo_x = 0;
    ball.velo_y = 0;

    /* Initialize paddle. */
    paddle.x = 280;
    paddle.y = 370;
    paddle.width = PWIDTH;
    paddle.height = PHEIGHT;
    paddle.speed = 0;
    paddle.direction = 0;

    /* Render ball and paddle. */
    clear_screen(drawBase);
    rendBall(&ball, base);
    rendPad(&paddle, base);

    /* Wait for keypress. */
    Cnecin();

    clear_screen(drawBase);

    return 0;
}