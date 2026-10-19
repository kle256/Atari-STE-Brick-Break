#ifndef RENDER_H
#define RENDER_H
#include "model.h"
#include "raster.h"

/* Renders the complete game state to the frame buffer. */
void render(Model *model, UINT8 *base);

/* Renders the ball at its current position. */
void rendBall(Ball *ball, UINT8 *base);

/* Renders the paddle at its current position. */
void rendPad(Paddle *paddle, UINT8 *base);

/* Renders an active brick at its current position. */
void rendBrk(Brick *brick, UINT8 *base);

/* Renders the player's current score. */
void rendScor(Score *score, UINT8 *base);

/* Renders the player's remaining lives. */
void rendLife(Lives *lives, UINT8 *base);

/* Renders current game state outcome*/
void rendState(GameState state, UINT8 *base);

#endif