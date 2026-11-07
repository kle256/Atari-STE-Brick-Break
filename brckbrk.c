/*
Author: Kevin Le
Date: November 4, 2026
Purpose: Initializes game and handles main game loop
*/
#include <osbind.h>
#include <linea.h>
#include "raster.h"
#include "model.h"
#include "render.h"
#include "events.h"
#include "input.h"
#include "clock.h"

/* Stores current game state*/
static Model game;
static Model prev;

int main(void) {
    UINT8 *base;
    UINT32 timeThen, timeNow;
    char input;
    boolean quit,
            pddlMved,
            ballMved;
    int oldPdleX, 
        oldBallX,
        oldBallY,
        i;

    /* Set game objects to their starting values*/
    initMod(&game);

    /* Get screen address and font support*/
    base = (UINT8*)Physbase();
    linea0();

    /* Draw first frame */
    render(&game, base);
    prev = game;

    quit = FALSE;

    /* Check for clock tick*/
    timeThen = getTime();

    while (quit == FALSE) {

        /* Check for key input*/
        if (hasInput()) {
            input = getInput();

            if (input == 'q' || input == 'Q') {
                quit = TRUE;
            } 
            else if (input == 'a' || input == 'A') {
                movLft(&game);
            }
            else if (input == 'd' || input == 'D') {
                movRgt(&game);
            }
        }

        /* Check for clock tick*/
        timeNow = getTime();
        if (quit == FALSE && timeNow != timeThen) {
            timeThen = timeNow;

            oldPdleX = game.paddle.x;
            oldBallX = game.ball.x;
            oldBallY = game.ball.y;

            if (game.state == GAME_PLAYING) {
                pTick(&game);
                bTick(&game);

                plftEdge(&game);
                prgtEdge(&game);
                blftEdge(&game);
                brgtEdge(&game);
                btopEdge(&game);

                bpadCol(&game);

                /* Check ball collision with each brick */
                for (i = 0; i < game.brkCnt; i++) {
                    bbrkCol(&game, i);
                }

                
                outBnds(&game);
                brkDest(&game);
            }

            /* Redraw if paddle or ball changed position */
            pddlMved = (game.paddle.x != oldPdleX);
            ballMved = (game.ball.x != oldBallX || game.ball.y != oldBallY);

            if (pddlMved || ballMved) {
                render(&game, base);
            }
        }
    }

    return 0;
}