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

/* Current state of game*/
typedef enum {
    GAME_PLAYING,
    GAME_WON,
    GAME_LOST
} GameState;

/* All game objects and their current states*/
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

void initMod(Model *model); /* Sets all game objects to their starting values*/
void rstBP(Model *model);   /* Resets ball and paddle to starting position*/
void nxtLvl(Model *model);  /* Sets the game state to next level*/
void endGame(Model *model, GameState outcome); /* Ends game with outcome type*/
#endif
