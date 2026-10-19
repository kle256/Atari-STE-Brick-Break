/*
Author: Kevin Le
Date: October 19,2026
Purpose: Handles brick initialization
*/

#include "brick.h"

/* Destroys brick*/
void dstrBrk(Brick *brick)
{
    brick->active = 0;
}

/* Restores brick to active */
void rstBrick(Brick *brick)
{
    brick->active = 1;
}

