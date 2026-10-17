/* Condition-triggered event handlers from the game spec. */
#ifndef COND_H
#define COND_H
#include "model.h"

/* Ball and wall collisions*/
void btopEdge(Model *model); /* Ball top wall collision*/
void blftEdge(Model *model); /* Ball left wall collision*/
void brgtEdge(Model *model); /* Ball right wall collision*/
void outBnds(Model *model); /* Ball moves past paddle*/

/* Paddle and wall collisions*/
void plftEdge(Model *model);  /* Paddle left wall collision*/
void prgtEdge(Model *model);  /* Paddle right wall collsion*/

/* Ball and object collisions*/
void bpadCol(Model *model);                   /* Ball paddle collision*/  
void bbrkCol(Model *model, int brick_index);  /* Ball brick collision*/

/* Game progress*/
void brksDest(Model *model);  /* All bricks destroyed*/
void lvlsComp(Model *model);  /* All levels completed*/
#endif
