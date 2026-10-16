/* Condition-triggered event handlers from the game spec. */
#ifndef COND_H
#define COND_H
#include "model.h"

void handle_ball_top_edge(Model *model);
void handle_ball_left_edge(Model *model);
void handle_ball_right_edge(Model *model); /* Needed for right-wall bounce. */
void handle_ball_bottom_edge(Model *model);
void handle_paddle_left_edge(Model *model);
void handle_paddle_right_edge(Model *model);
void handle_ball_paddle_collision(Model *model);
void handle_ball_brick_collision(Model *model, int brick_index);
void handle_all_bricks_destroyed(Model *model);
void handle_all_levels_completed(Model *model);
#endif
