#include "game_types.h"

/* Renders the complete game state to the frame buffer. */
void render(const Model *model, UINT8 *base, const UINT8 *font);

/* Renders the ball at its current position. */
void rendBall(const Ball *ball, UINT8 *base);

/* Renders the paddle at its current position. */
void rendPad(const Paddle *paddle, UINT8 *base);

/* Renders an active brick at its current position. */
void rendBrk(const Brick *brick, UINT8 *base);

/* Renders the player's current score. */
void rendScor(const Score *score, UINT8 *base, const UINT8 *font);

/* Renders the player's remaining lives. */
void rendLife(const Lives *lives, UINT8 *base, const UINT8 *font);

#endif