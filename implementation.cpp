#include "resource.h"
#include "implementation.h"
#include <windows.h>
#include <cstdio>
#include <cmath>



//						 XXXX XXXX
//					  80-^||| |||^-01 AUTOMATIC STEP
//					  40--^|| ||^--02
//					  20---^| |^---04
//					  10----^ ^----08
unsigned char toggles = 0;
HMENU mainMenu;

HDC HdDesktop = GetDC(nullptr);
HDC ourMainDc;
HDC MemDc = CreateCompatibleDC(HdDesktop);
HBITMAP HBitmap = CreateCompatibleBitmap(HdDesktop, WINDOW_WIDTH, WINDOW_HEIGHT);

int numXYValues = 0;
int* xValues = nullptr;
int* yValues = nullptr;

int currentPointX = rand() % WINDOW_WIDTH;
int currentPointY = rand() % WINDOW_HEIGHT;

void populateXYValsOnUnitCircle(int);
void doStep(unsigned int);

void init(HWND hwnd) {
	SelectObject(MemDc, HBitmap);
	mainMenu = GetMenu(hwnd);
	ourMainDc = GetDC(hwnd);
	populateXYValsOnUnitCircle(DEFAULT_CORNERS);
#ifdef DEBUG
	printf("init done.\n");
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
	else if (wParam == DO_STEP_KEY && !(lParam & 0x40000000)) {
		doStep(MANUALSTEP_ITERS_PER_PRESS);
		RedrawWindow(hwnd, nullptr, nullptr, RDW_INTERNALPAINT);
	}else if (wParam == CLEAR_CANVAS_KEY && !(lParam & 0x40000000)) {
		DeleteDC(MemDc);
		MemDc = CreateCompatibleDC(HdDesktop);
		DeleteObject(HBitmap);
		HBitmap = CreateCompatibleBitmap(HdDesktop, WINDOW_WIDTH, WINDOW_HEIGHT);
		SelectObject(MemDc, HBitmap);
	}else if (wParam >= 0x32 && wParam <= 0x39) {
		populateXYValsOnUnitCircle((char)wParam - 0x30);
	}
}

void handleWmCommand(HWND, WPARAM wParam, LPARAM) {
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
	for (int i = 0; i < numXYValues; i++) {
		//the corners, in magenta - they're 3x3 pixels.
		for (int x = xValues[i] - 1; x <= xValues[i] + 1; x++) {
			for (int y = yValues[i] - 1; y <= yValues[i] + 1; y++) {
				SetPixel(MemDc, x, y, 0x00FF00FF);
			}
		}
	}
	BitBlt(ourMainDc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MemDc, 0, 0, SRCCOPY);
	if (toggles & 0x01) {
		//...this works!?
		doStep(AUTOSTEP_ITERS_PER_FRAME);
		RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}

void populateXYValsOnUnitCircle(int num) {
	//realloc behaves like malloc if the pointer it's given is null - that is, it just allocates the requested
	//memory somewhere. if the pointer is not null and points to an address allocated my realloc, malloc or calloc,
	//then it either free()s that chunk and allocates a new one or extends the existing one.
	xValues = (int*)realloc(xValues, num * sizeof(int));
	yValues = (int*)realloc(yValues, num * sizeof(int));
	numXYValues = num;
	long double angle = 0;
	long double increment = M_TWOPI / num;
	for (int i = 0; i < num; i++) {
		angle += increment;
		xValues[i] = (int)(WINDOW_HWIDTH + WINDOW_HWIDTH * cosl(angle));
		yValues[i] = (int)(WINDOW_HHEIGHT + WINDOW_HHEIGHT * sinl(angle));
	}
#ifdef DEBUG
	//sanity check, our angle should now be pretty close to 2pi (not exactly cause floating point)
	printf("expected: %f\n     got: %Lf\n", M_TWOPI, angle);
#endif
}

void doStep(unsigned int n) {
	for (int i = 0; i < n; i++) {
		//filled points are an average gray.
		SetPixel(MemDc, currentPointX, currentPointY, 0x00888888);

		int cornerToMoveTowards = rand() % numXYValues;
		currentPointX = (currentPointX + xValues[cornerToMoveTowards]) / 2;
		currentPointY = (currentPointY + yValues[cornerToMoveTowards]) / 2;
	}
	//the "player" (or current pixel) is bright green
	SetPixel(MemDc, currentPointX, currentPointY, 0x0000FF00);
}