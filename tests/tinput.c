/*
Author: Kevin Le
Date: November 4, 2026
Purpose: Tests keyboard input availability and reading
*/

#include <stdio.h>
#include "input.h"

int main(void) {
    char input;
    boolean quit;

    quit = FALSE;

    printf("press A or D. Press Q to quit. \n");

    while (quit == FALSE) {
        if (hasInput()) {
            input = getInput();
            printf("key pressed: %c\n", input);

            if (input == 'q' || input == 'Q') {
                quit = TRUE;
            }
        }
    }

}