/*
Author: Kevin Le
Date: November 4, 2026
Purpose: Provides access to the Atari clock counter
*/

#include "clock.h"
#include <osbind.h>

/* Returns current value of the clock */
UINT32 getTime(void) {
    volatile UINT32 *timer;
    UINT32 currTime;
    long oldSsp;

    timer = (volatile UINT32 *)0x462;

    oldSsp = Super(0); /* Enter supervisor mode */
    currTime = *timer; /* Get current clock tick */
    Super(oldSsp);     /* Return to user mode */

    return currTime;
}