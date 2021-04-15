#ifndef CONFIG_H
#define CONFIG_H

//all macros in this file can (and should) be tweaked.

//adds a bunch of logging and a square to each corner of the client area, to check if it is correct.
#define DEBUG

//shouldn't be important to end-user - this is used by windows to detect if 2 windows are the same program
#define WINDOW_CLASSNAME "CSProject"

#define WINDOW_TITLE "Chaos Game"

//define width and height of the window.
#define WINDOW_WIDTH 1500
#define WINDOW_HEIGHT 1000

#define DEFAULT_CORNERS 3

//if defined, rainbow fades radially out from the middle of the canvas. otherwise, fades from top left to bottom right.
#define RAINBOW_RADIAL

#define AUTOSTEP_ITERS_PER_FRAME 5000
#define MANUALSTEP_ITERS_PER_PRESS 10000

//key codes according to https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
//the number of corners can be set between 2 and 9 with the numrow keys.
#define DO_STEP_KEY 0x53 //S
#define CLEAR_CANVAS_KEY 0x43 //C
#define AUTO_STEP_KEY VK_SPACE //spacebar
#define UNIQUE_CORNER_KEY 0x55 //U
#define RAINBOW_KEY 0x52 //R

//BGR format, 1 byte per colour
#define PREV_POINTS_COL 0x888888
#define CORNER_COL 0xFF00FF
#define CURRENT_POINT_COL 0x00FF00

#define DRAW_CORNERS

#endif
