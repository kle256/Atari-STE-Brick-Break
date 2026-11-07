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

#define SCRSIZE 32000
#define SCRALGN 256

/* Stores current game state*/
static Model game;
static Model prev;
static Model prevBck;

/* Extra screen memory with room for alignment */
static UINT8 screenMem[SCRSIZE + SCRALGN - 1];

int main(void) {
    UINT8 *base,
          *origBase,
          *frntBase,
          *bckBase;
    UINT32 timeThen, 
           timeNow,
           bffrAddr;
    char input;
    boolean quit;
    int i;

    /* Set game objects to their starting values*/
    initMod(&game);

    /* save screen address and set front buffer */
    origBase = (UINT8*)Physbase();
    frntBase = origBase;
    base = frntBase;

    /* Align extra screen memory to 256 byte boundary */
    bffrAddr = (UINT32)screenMem;
    bffrAddr += SCRALGN - 1;
    bffrAddr &= 0xFFFFFF00UL;
    bckBase = (UINT8 *)bffrAddr;

    linea0();

    /* Draw first frame */
    render(&game, base);
    prev = game;

    /* Draw starting frame into extra buffer */
    render(&game, bckBase);
    prevBck = game;


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
                brksDest(&game);
            }

            /* Update changed parts of screen */
            rndUpdt(&game, &prev, base);
            prev = game;
        }
    }

    return 0;
}