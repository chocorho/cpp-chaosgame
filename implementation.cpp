#include "config.h"
#include "consts.h"
#include "implementation.h"
#include <windows.h>
#include <cstdint> /* necessary for compiling with int32_t on other machines */
#include <cstdio>
#define _USE_MATH_DEFINES /* necessary for finding M_PI on other machines */
#include <cmath>


//						 XXXX XXXX
//					  80-^||| |||^-01 AUTOMATIC STEP
//					  40--^|| ||^--02 DISTINCT CORNERS
//					  20---^| |^---04 RAINBOW MODE
//					  10----^ ^----08
unsigned char toggles = 0;

//values from init
HMENU mainMenu;
HDC ourMainDc;
HBITMAP HBitmap;
RECT clientArea;

//drawing things
HDC HdDesktop = GetDC(nullptr);
HDC MemDc = CreateCompatibleDC(HdDesktop);

//corners
int numCorners = 0;
int *cornerX = nullptr;
int *cornerY = nullptr;
int lastCornerIx = 0;

//current pos
int currentPointX = rand() % WINDOW_WIDTH;
int currentPointY = rand() % WINDOW_HEIGHT;

//utility, definitions are last in file
void populateCorners(int);
void doStep(unsigned int);
void flipToggle(unsigned char, int, const char *);
void Set3x3Px(int, int, int);
int32_t HueToBGR(float);

//basically a constructor, for most intents and purposes
void init(HWND hwnd) {
	mainMenu = GetMenu(hwnd);
	ourMainDc = GetDC(hwnd);
	GetClientRect(hwnd, &clientArea);
	populateCorners(DEFAULT_CORNERS);
	HBitmap = CreateCompatibleBitmap(HdDesktop, clientArea.right, clientArea.bottom);
	SelectObject(MemDc, HBitmap);
#ifdef DEBUG
	printf("init done.\n");
#endif
}

//EVENT HANDLERS
void handleKeyPress(HWND hwnd, WPARAM wParam, LPARAM lParam) {
#ifdef DEBUG
	printf("keycode: %llu, previous state: %lld\n", wParam, lParam & 0x40000000);
#endif
	switch (wParam) {
		case AUTO_STEP_KEY:
			flipToggle(0x01, COMPONENT_AUTO_STEP_TOGGLE, STRING_AUTO_STEP_TOGGLE);
			RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE);
			break;
		case UNIQUE_CORNER_KEY:
			flipToggle(0x02, COMPONENT_UNIQUE_CORNERS_TOGGLE, STRING_UNIQUE_CORNERS_TOGGLE);
			break;
		case RAINBOW_KEY:
			flipToggle(0x04, COMPONENT_RAINBOW_MODE, STRING_RAINBOW_MODE);
			break;
		case DO_STEP_KEY:
			if (lParam & 0x40000000)
				break;
			doStep(MANUALSTEP_ITERS_PER_PRESS);
			RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE);
			break;
		case CLEAR_CANVAS_KEY:
			if (lParam & 0x40000000)
				break;
			DeleteDC(MemDc);
			MemDc = CreateCompatibleDC(HdDesktop);
			DeleteObject(HBitmap);
			HBitmap = CreateCompatibleBitmap(HdDesktop, WINDOW_WIDTH, WINDOW_HEIGHT);
			SelectObject(MemDc, HBitmap);
			break;
		default:
#ifdef DEBUG
			if (wParam < 0x32 || wParam > 0x39)
				printf("unused keycode encountered: %llu\n", wParam);
#endif
			break;
	}
	//annoying to do in a switch - not the cleanest way to solve it either but it works
	if (wParam >= 0x32 && wParam <= 0x39) {
		populateCorners((char)wParam - 0x30);
	}
}
void handleWmCommand(HWND hwnd, WPARAM wParam, LPARAM) {
	switch (LOWORD(wParam)) {
		case COMPONENT_AUTO_STEP_TOGGLE:
			flipToggle(0x01, COMPONENT_AUTO_STEP_TOGGLE, STRING_AUTO_STEP_TOGGLE);
			RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE);
			break;
		case COMPONENT_UNIQUE_CORNERS_TOGGLE:
			flipToggle(0x02, COMPONENT_UNIQUE_CORNERS_TOGGLE, STRING_UNIQUE_CORNERS_TOGGLE);
			break;
		case COMPONENT_RAINBOW_MODE:
			flipToggle(0x04, COMPONENT_RAINBOW_MODE, STRING_RAINBOW_MODE);
			break;
		default:
			//should never happen?
			printf("UNDEFINED WM_COMMAND wParam: %d\n", LOWORD(wParam));
	}
}
void handlePaint(HWND hwnd) {
#ifdef DRAW_CORNERS
	for (int i = 0; i < numCorners; i++) {
		Set3x3Px(cornerX[i], cornerY[i], CORNER_COL);
	}
#endif
#ifdef DEBUG
	Set3x3Px(1, 1, 0x00FF0000);
	Set3x3Px(1, clientArea.bottom - 2, 0x0000FF00);
	Set3x3Px(clientArea.right - 2, 1, 0x000000FF);
	Set3x3Px(clientArea.right - 2, clientArea.bottom - 2, 0x00FFFFFF);
#endif
	BitBlt(ourMainDc, 0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, MemDc, 0, 0, SRCCOPY);
	if (toggles & 0x01) {
		//...this works!?
		doStep(AUTOSTEP_ITERS_PER_FRAME);
		RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}

//utility funcs
void populateCorners(int num) {
	//realloc behaves like malloc if the pointer it's given is null - that is, it just allocates the requested
	//memory somewhere. if the pointer is not null and points to an address allocated my realloc, malloc or calloc,
	//then it either free()s that chunk and allocates a new one or extends the existing one.
	cornerX = (int *)realloc(cornerX, num * sizeof(int));
	cornerY = (int *)realloc(cornerY, num * sizeof(int));
	numCorners = num;

	//special cases due to performance, as well as preventing one corner from infuriatingly being a pixel lower than
	//the one it should be on the same level with, due to the truncation
	if (num == 3) {
		cornerX[0] = clientArea.right / 2;
		cornerX[1] = clientArea.right - 1;
		cornerX[2] = 0;
		cornerY[0] = 0;
		cornerY[1] = cornerY[2] = clientArea.bottom - 1;
		return;
	} else if (num == 4) {
		cornerX[0] = cornerX[3] = 0;
		cornerX[1] = cornerX[2] = clientArea.right - 1;
		cornerY[0] = cornerY[1] = 0;
		cornerY[2] = cornerY[3] = clientArea.bottom - 1;
		return;
	}

	long double angle = 0;
	long double increment = 2.0*M_PI / num;
	for (int i = 0; i < num; i++) {
		angle += increment;
		cornerX[i] = (int)((clientArea.right / 2) + (clientArea.right / 2) * cosl(angle));
		cornerY[i] = (int)((clientArea.bottom / 2) + (clientArea.bottom / 2) * sinl(angle));
	}
#ifdef DEBUG
	//sanity check, our angle should now be pretty close to 2pi (not exactly cause floating point)
	printf("expected: %f\n     got: %Lf\n", 2.0*M_PI, angle);
#endif
}
void doStep(unsigned int n) {
	for (int i = 0; i < n; i++) {
		int col = PREV_POINTS_COL;
#ifdef RAINBOW_RADIAL
		if (toggles & 0x04)
			col = HueToBGR((pow(abs(currentPointX - clientArea.right / 2), 2) +
							pow(abs(currentPointY - clientArea.bottom / 2), 2)) /
						   (pow(clientArea.right / 2, 2) + pow(clientArea.bottom / 2, 2)) * 360);
#else
		if (toggles & 0x04)
			col = HueToBGR((float)(pow(currentPointX, 2) + pow(currentPointY, 2)) /
						   (pow(clientArea.right, 2) + pow(clientArea.bottom, 2)) * 360);
#endif
		SetPixel(MemDc, currentPointX, currentPointY, col);

		int cornerToMoveTowards = rand() % numCorners;
		if (toggles & 0x02)
			while (cornerToMoveTowards == lastCornerIx)
				cornerToMoveTowards = rand() % numCorners;
		lastCornerIx = cornerToMoveTowards;
		currentPointX = (currentPointX + cornerX[cornerToMoveTowards]) / 2;
		currentPointY = (currentPointY + cornerY[cornerToMoveTowards]) / 2;
	}
	SetPixel(MemDc, currentPointX, currentPointY, CURRENT_POINT_COL);
}
void flipToggle(unsigned char bit, int component, const char *text) {
#ifdef DEBUG
	printf("flipping bit %d, previous value: %d, component id: %d, text: %s\n", bit, toggles & bit, component, text);
#endif
	toggles ^= bit;
	ModifyMenuA(mainMenu, component, MF_BYCOMMAND | (toggles & bit ? MF_CHECKED : MF_UNCHECKED), component, text);
}
void Set3x3Px(int x, int y, int col) {
	for (int xmod = -1; xmod < 2; xmod++)
		for (int ymod = -1; ymod < 2; ymod++)
			SetPixel(MemDc, x + xmod, y + ymod, col);
}
int32_t HueToBGR(float HueDegrees) {
	// https://www.codespeedy.com/hsv-to-rgb-in-cpp/
	float X = 1 - abs(fmod(HueDegrees / 60.0, 2) - 1);
	float r, g, b;
	if (HueDegrees >= 0 && HueDegrees < 60) {
		r = 1, g = X, b = 0;
	} else if (HueDegrees >= 60 && HueDegrees < 120) {
		r = X, g = 1, b = 0;
	} else if (HueDegrees >= 120 && HueDegrees < 180) {
		r = 0, g = 1, b = X;
	} else if (HueDegrees >= 180 && HueDegrees < 240) {
		r = 0, g = X, b = 1;
	} else if (HueDegrees >= 240 && HueDegrees < 300) {
		r = X, g = 0, b = 1;
	} else {
		r = 1, g = 0, b = X;
	}
	int32_t R = r * 255;
	int32_t G = g * 255;
	int32_t B = b * 255;

	return (B << 16) | (G << 8) | R;
}
