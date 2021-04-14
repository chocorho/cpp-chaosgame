#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H
#include <windows.h>

void init(HWND);

void handleKeyPress(HWND, WPARAM, LPARAM);
void handleWmCommand(HWND, WPARAM, LPARAM);
void handlePaint(HWND);

#endif
