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

void destroy_brick(Brick *brick);
void reset_brick(Brick *brick);
#endif
