#include "resource.h"
#include "implementation.h"
#include <windows.h>
#include <cstdio>



//						 XXXX XXXX
//					  80-^||| |||^-01 AUTOMATIC STEP
//					  40--^|| ||^--02
//					  20---^| |^---04
//					  10----^ ^----08
unsigned char toggles = 0;
HMENU mainMenu;

HDC HdDesktop = GetDC(nullptr);
HDC MemDc = CreateCompatibleDC(HdDesktop);
HBITMAP HBitmap = CreateCompatibleBitmap(HdDesktop, WINDOW_WIDTH, WINDOW_HEIGHT);

void init(HWND hwnd) {
	SelectObject(MemDc, HBitmap);
	mainMenu = GetMenu(hwnd);
#ifdef DEBUG
	printf("Graphics and menu ref initialized\n");
#endif
}

void flipToggle(unsigned char bit, int component, const char* text) {
#ifdef DEBUG
	printf("flipping bit %d, previous value: %d, component id: %d, text: %s\n", bit, toggles & bit, component, text);
#endif
	toggles ^= bit;
	ModifyMenuA(mainMenu, component, MF_BYCOMMAND | (toggles & bit ? MF_CHECKED : MF_UNCHECKED), component, text);
}

void handleKeyPress(HWND hwnd, WPARAM wParam, LPARAM lParam) {
#ifdef DEBUG
	printf("keycode: %llu, previous state: %lld\n", wParam, lParam & 0x40000000);
#endif
	if (wParam == VK_SPACE)
		flipToggle(0x01, COMPONENT_AUTO_STEP_TOGGLE, STRING_AUTO_STEP_TOGGLE);
}

void handleWmCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
		case COMPONENT_AUTO_STEP_TOGGLE:
			flipToggle(0x01, COMPONENT_AUTO_STEP_TOGGLE, STRING_AUTO_STEP_TOGGLE);
			//DrawMenuBar(hwnd);
			break;
		default:
			//should never happen?
			printf("UNDEFINED WM_COMMAND wParam: %d\n", LOWORD(wParam));
	}
}

void handlePaint(HWND hwnd) {
	SetPixel(MemDc, 250, 250, 0x000000FF);
	BitBlt(GetDC(hwnd), 0, 0, 700, 700, MemDc, 0, 0, SRCCOPY);
}