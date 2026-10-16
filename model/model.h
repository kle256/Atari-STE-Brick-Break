/* Whole-game model; no drawing code belongs here. */
#ifndef MODEL_H
#define MODEL_H
#include "ball.h"
#include "paddle.h"
#include "brick.h"
#include "score.h"
#include "lives.h"

#define MAX_BRICKS 128 /* TODO: revise once your brick layouts are fixed. */
#define TOTAL_LEVELS 2

typedef enum {
    GAME_PLAYING,
    GAME_WON,
    GAME_LOST
} GameState;

typedef struct {
    Ball ball;
    Paddle paddle;
    Brick bricks[MAX_BRICKS];
    int brick_count;
    Score score;
    Lives lives;
    int current_level;
    GameState state;
} Model;

void init_model(Model *model);
void reset_ball_and_paddle(Model *model);
void advance_level(Model *model);
void finish_game(Model *model, GameState outcome);
#endif
