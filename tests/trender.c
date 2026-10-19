/*
 * Renderer test driver.
 * Displays basic game states in STEEM.
 */

#include <osbind.h>
#include <linea.h>
#include "render.h"

static Model game;

int main(void) {
    UINT8 *base;

    base = (UINT8 *)Physbase();
    linea0();
    initMod(&game);

    /* Initial game screen. */
    render(&game, base);
    Cnecin();

    /* Brick removed, score and lives changed. */
    game.bricks[0].active = 0;
    game.brksLeft -= 1;
    game.score.value = 40;
    game.lives.value = 2;
    render(&game, base);
    Cnecin();

    /* Victory and defeat messages. */
    game.state = GAME_WON;
    render(&game, base);
    Cnecin();

    game.state = GAME_LOST;
    render(&game, base);
    Cnecin();

    return 0;
}
