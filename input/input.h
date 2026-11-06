#ifndef INPUT_H
#define INPUT_H

typedef enum { FALSE, TRUE } boolean;

boolean hasInput(void); /* Returns TRUE if keyboard input is waiting */
char getInput(void);    /* Reads next character; requires pending input */

#endif