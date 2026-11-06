/*
Author: Kevin Le
Date: November 4,2026
Purpose: Handles input
*/
#include "input.h"
#include <stdio.h>
#include <osbind.h>

/* Checks whether keyboard input is waiting*/
boolean hasInput(void) {
    if (Cconis() != 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* Reads next char and requires a pending keyboard input*/
char getInput(void) {
    char input;
    input = (char)Cnecin();

    return input;
}