#include "events.h"
#include "asynch.h"
#include "synch.h"
#include "cond.h"


printf("TEST DRIVER STARTED\n");
fflush(stdout);

int main(void)
{
    /* Test model and failure counter */
static Model game;
static int failures = 0;

/* Reports whether a test passed or failed. */
static void chk(char *name, int condition) {
    if (condition) {
        printf("PASS: %s\n", name);
    } else {
        printf("FAIL: %s\n", name);
        failures++;
    }
}

int main(void) {
    int oldY;

    /* Initialize game model */
    initMod(&game);

    /* Test initial game values */
    chk("Initial game state", game.state == GAME_PLAYING);
    chk("Initial level", game.currLvl == 1);
    chk("Initial score", game.score.value == 0);
    chk("Initial lives", game.lives.value == 3);
    chk("Total bricks", game.brkCnt == 32);
    chk("Remaining bricks", game.brksLeft == 32);
    chk("First brick active", game.bricks[0].active == 1);

    /* Test ball movement */
    oldY = game.ball.y;
    bTick(&game);

    chk("Ball movement", game.ball.y == oldY + 2);

    /* Display results */
    printf("\nTests failed: %d\n", failures);


    printf("Press Enter to exit...\n");
    fflush(stdout);
    getchar();
    return failures;
}
}
