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

int main(void) {
    UINT8 *base;
    char input;
    boolean quit;

    /* Set game objects to their starting values*/
    initMod(&game);

    /* Get screen address and font support*/
    base = (UINT8*)Physbase();
    linea0();

    /* Draw first frame */
    render(&game, base);

    quit = FALSE;

    while (quit == FALSE) {

        /* Check for key input*/
        if (hasInput()) {
            input = getInput();

            if (input == 'q' || input == 'Q') {
                quit = TRUE;
            } 
        }
    }

    return 0;
}