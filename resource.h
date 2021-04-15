#ifndef RESOURCE_H
#define RESOURCE_H

//all macros in this file up to and including CLEAR_CANVAS_KEY can (and should) be tweaked.

//adds a bunch of logging
#define DEBUG

//define width and height of the window.
#define WINDOW_WIDTH 1500
#define WINDOW_HEIGHT 1000

#define DEFAULT_CORNERS 3

#define AUTOSTEP_ITERS_PER_FRAME 50
#define MANUALSTEP_ITERS_PER_PRESS 10000

//key codes according to https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
//the number of corners can be set between 2 and 9 with the numrow keys.
#define DO_STEP_KEY 0x53 //space bar
#define CLEAR_CANVAS_KEY 0x43 //c



//DANGER ZONE
//please don't mess with this.

#define WINDOW_HWIDTH (WINDOW_WIDTH/2)
#define WINDOW_HHEIGHT (WINDOW_HEIGHT/2)

#define COMPONENT_MAINMENU 101
#define COMPONENT_AUTO_STEP_TOGGLE 9002

#define STRING_AUTO_STEP_TOGGLE "&Automatically Step forward"

#endif
