#ifndef RESOURCE_H
#define RESOURCE_H

//all macros in this file up to and including WINDOW_HEIGHT can (and should) be tweaked.

//adds a bunch of logging
#define DEBUG

//define width and height of the window.
#define WINDOW_WIDTH 1500
#define WINDOW_HEIGHT 1000

#define DEFAULT_CORNERS 3



//DANGER ZONE
//please don't mess with this.

#define WINDOW_HWIDTH (WINDOW_WIDTH/2)
#define WINDOW_HHEIGHT (WINDOW_HEIGHT/2)

#define COMPONENT_MAINMENU 101
#define COMPONENT_AUTO_STEP_TOGGLE 9002

#define STRING_AUTO_STEP_TOGGLE "&Automatically Step forward"

#endif
