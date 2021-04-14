#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H
#include <windows.h>

void init(HWND hwnd);

void handleKeyPress(HWND hwnd, WPARAM wParam, LPARAM lParam);
void handleWmCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
void handlePaint(HWND hwnd);

#endif
