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
          *bckBase,
          *tempBase;
    UINT32 timeThen, 
           timeNow,
           bffrAddr;
    Model *frntPrev, 
          *bckPrev,
          *tmpPrev;
    char input;
    boolean quit;
    int i;
    long kpSttng;

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

    /* Match with screen buffers */
    frntPrev = &prev;
    bckPrev = &prevBck;

    kpSttng = -1;
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

            /* Update back buffer and save drawn state */
            rndUpdt(&game, bckPrev, bckBase);
            *bckPrev = game;

            /* Display completed back buffer and wait for vertical blank */
            Setscreen((UINT8 *)kpSttng, bckBase, kpSttng);
            Vsync();

            /* Swap pointers of screen buffer */
            tempBase = frntBase;
            frntBase = bckBase;
            bckBase = tempBase;

            /* Swap snapshots */
            tmpPrev = frntPrev;
            frntPrev = bckPrev;
            bckPrev = tmpPrev;
        }
    }

    /* Restore original screen */
    Setscreen((UINT8 *)kpSttng, origBase, kpSttng);
    Vsync();
    return 0;
}