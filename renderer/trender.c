
/*
 * Renderer test driver.
 * Tests the complete game renderer in STEEM.
 * Author: Kevin Le
 */

#include <osbind.h>
#include <linea.h>
#include "render.h"
#include "raster.h"
#include "model.h"

int main(void) {
    static Model model;
    UINT8 *base;

    base = (UINT8 *)Physbase();
    linea0();

    /* Initialize complete game model. */
    initMod(&model);

    /* Initialize score display. */
    model.score.x = 16;
    model.score.y = 16;
    model.score.width = 80;
    model.score.height = 16;

    /* Initialize lives display. */
    model.lives.x = 520;
    model.lives.y = 16;
    model.lives.width = 80;
    model.lives.height = 16;

    /* Test 1: Initial Level 1. */
    render(&model, base);
    Cnecin();

    /* Test 2: Destroy bricks and update HUD. */
    model.bricks[0].active = 0;
    model.bricks[1].active = 0;
    model.brksLeft = 30;
    model.score.value = 80;
    model.lives.value = 2;

    render(&model, base);
    Cnecin();

    /* Test 3: Advance to Level 2. */
    nxtLvl(&model);

    render(&model, base);
    Cnecin();

    /* Test 4: Victory screen. */
    model.state = GAME_WON;

    render(&model, base);
    Cnecin();

    /* Test 5: Game over screen. */
    model.state = GAME_LOST;

    render(&model, base);
    Cnecin();

    return 0;
}
