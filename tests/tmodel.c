/*
 * Model test driver.
 * Tests basic model and event behaviour.
 */

#include <stdio.h>
#include "model.h"
#include "asynch.h"
#include "synch.h"
#include "cond.h"

static Model game;

void testInit(void);
void testMove(void);
void testWalls(void);
void testBrick(void);
void testGame(void);

int main(void) {
    testInit();
    testMove();
    testWalls();
    testBrick();
    testGame();

    return 0;
}

/* Check starting values. */
void testInit(void) {
    initMod(&game);

    printf("Start: Level %d, Score %d, Lives %d\n",
           game.currLvl, game.score.value, game.lives.value);
    printf("Bricks: %d\n", game.brksLeft);
}

/* Check ball and paddle movement. */
void testMove(void) {
    initMod(&game);
    setVel(&game.ball, 2, -2);
    bTick(&game);

    movLft(&game);
    pTick(&game);

    printf("Move: Ball %d,%d, Paddle %d\n",
           game.ball.x, game.ball.y, game.paddle.x);
}

/* Check top and left wall bounce. */
void testWalls(void) {
    initMod(&game);
    game.ball.x = 0;
    game.ball.y = 0;
    setVel(&game.ball, -2, -2);

    bTick(&game);
    blftEdge(&game);
    btopEdge(&game);

    printf("Walls: Ball %d,%d, Velocity %d,%d\n",
           game.ball.x, game.ball.y,
           game.ball.velo_x, game.ball.velo_y);
}

/* Check brick destruction and score. */
void testBrick(void) {
    initMod(&game);
    game.ball.x = game.bricks[0].x;
    game.ball.y = game.bricks[0].y;
    setVel(&game.ball, 0, 2);

    bbrkCol(&game, 0);

    printf("Brick: Active %d, Score %d, Left %d\n",
           game.bricks[0].active, game.score.value, game.brksLeft);
}

/* Check level progression and game over. */
void testGame(void) {
    initMod(&game);
    game.brksLeft = 0;
    brksDest(&game);

    printf("Next level: %d\n", game.currLvl);

    game.lives.value = 1;
    game.ball.y = 395;
    setVel(&game.ball, 0, 2);
    outBnds(&game);

    printf("Game over: Lives %d, State %d\n",
           game.lives.value, game.state);
}
