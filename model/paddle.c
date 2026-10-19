/*
 Author: Kevin Le
 Date: October 19, 2026
 Purpose: Handles paddle movement, direction, and reset.
 */
#include "paddle.h"

#define PSTRTX 280 /* paddle starting position for x*/
#define PSTRTY 370 /* paddle starting position for y*/

/* Sets paddle direction if input valid*/
void setPDir(Paddle *paddle, int direction)
{
    if (direction >= -1 && direction <= 1) {
        paddle->direction = direction;
    }
}

/* Updates paddle horizontal position*/
void movPad(Paddle *paddle)
{
    int temp;
    temp = paddle->speed * paddle->direction;
    paddle->x += temp;
}

/* Resets paddle to starting position and stops movement */
void rstPad(Paddle *paddle)
{
    paddle->x = PSTRTX;
    paddle->y = PSTRTY;
    paddle->direction = 0;
}

