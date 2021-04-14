#include <windows.h>
#include <cstdio>
#include "resource.h"
#include "implementation.h"

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			handleWmCommand(hwnd, wParam, lParam);
			break;
		case WM_KEYDOWN:
			handleKeyPress(hwnd, wParam, lParam);
			break;
		case WM_PAINT: {
			handlePaint(hwnd);
			break;
		}
		default:
			// msg not implemented - use default impl (ie, WM_CLOSE just calls DestroyWindow(hwnd) by default)
#ifdef DEBUG
			//this might be useful, but it gets very spammy very fast, so would rather leave it out in favour of seeing other debug messages too.
			//printf("message %d received, not handled\n", msg);
#endif
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow) {
	const char *g_szClassName = "CSProject";
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = MAKEINTRESOURCE(COMPONENT_MAINMENU);
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		printf("Could not create Window.\n");
		return 1;
	}

	HWND hwnd = CreateWindowEx(
			//border style
			0,
			//window class (NOT related to oop!)
			g_szClassName,
			//window title
			"Chaos Game",
			//window style
			WS_MINIMIZEBOX | WS_SYSMENU,
			//x pos, y pos, width, height
			CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
			//parent window, menu, application instance, window creation data
			nullptr, nullptr, hInstance, nullptr);

	if (hwnd == nullptr) {
		printf("Could not create Window.\n");
		return 1;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	init(hwnd);

	MSG Msg;
	//GetMessage blocks if nothing is available
	while (GetMessage(&Msg, nullptr, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

#ifdef DEBUG
	printf("exiting.\n");
#endif

	return Msg.wParam;
}