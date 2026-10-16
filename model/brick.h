/* Brick model: scoring tier and whether brick remains on screen. */
#ifndef BRICK_H
#define BRICK_H

typedef enum {
    BRICK_YELLOW,
    BRICK_GREEN,
    BRICK_ORANGE,
    BRICK_RED
} BrickType;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    BrickType type; /* Tier name from spec; display is monochrome. */
    int points;
    int active;
} Brick;

void dstrBrk(Brick *brick);     /* Destroy brick object*/
void rstBrick(Brick *brick);    /* Reset brick placement*/
#endif
