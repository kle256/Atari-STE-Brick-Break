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

