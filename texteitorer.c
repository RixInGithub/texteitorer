#ifndef UNICODE
#define UNICODE
#endif
#define ID_FILE_EXIT 128
#include <windows.h>
#include "src.h"

// for win cbs:
// 128 << idx

int rX = 0;
int rY = 0;
int rXV = 5;
int rYV = 5;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	const wchar_t CLASS_NAME[] = L"TextEitorer";
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	if (!RegisterClass(&wc)) {
		return 0;
	}
	HMENU menu = CreateMenu();
	HMENU file = CreateMenu();
	AppendMenu(file, MF_STRING, ID_FILE_EXIT, L"exit the furry convention");
	AppendMenu(menu, MF_POPUP, (UINT_PTR)file, L"filet");
	HWND hwnd = CreateWindowEx(
		0, // window styles
		CLASS_NAME, // class
		L"text eitorer", // title
		WS_OVERLAPPEDWINDOW, // moar window styles ig
		CW_USEDEFAULT, CW_USEDEFAULT, // xy
		600, // width
		380, // height
		NULL, // parent
		menu,
		hInstance, // an instance of h (100% NOT CLICKBAIT!!!!)
		NULL // useless app data
	);
	if (hwnd == NULL) {
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);
	SendMessage(hwnd, WM_SETICON, 0, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON)));
	SetTimer(hwnd, 1, 1000 / 60, NULL);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_FILE_EXIT:
					PostQuitMessage(0);
					break;
			}
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			// rectangle ig
			HBRUSH hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
			HPEN hPenInvisible = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOW));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushRed);
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPenInvisible);
			Rectangle(hdc, rX, rY, 100 + rX, 100 + rY);
			SelectObject(hdc, hOldBrush);
			SelectObject(hdc, hOldPen);
			DeleteObject(hBrushRed);
			DeleteObject(hPenInvisible);
			// rectangle ig
			EndPaint(hwnd, &ps);
			return 0;
		case WM_TIMER:
			rX += rXV;
			rY += rYV;
			InvalidateRect(hwnd, NULL, 1);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}