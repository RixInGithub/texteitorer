#ifndef UNICODE
#define UNICODE
#endif
#define EXIT 128
#define ABUT 256
#define SAVE 512
#define FSIZ 1024
#define OPEN 2048
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "src.h"

// for win cbs:
// 128 << idx
// (assuming idx is 0-based)

int rX = 0;
int rY = 0;
int rXV = 5;
int rYV = 5;
HWND hwnd;
HWND edit;
COLORREF winCol;
bool noFonting = false;

int* getSizeOfHwnd() {
	static int wh[2];
	RECT r;
	GetClientRect(hwnd, &r);
	wh[0] = r.right - r.left;
	wh[1] = r.bottom - r.top;
	return wh;
}

COLORREF initWinCol() {
	COLORREF winCol = GetBkColor(GetDC(hwnd));
	return winCol;
}

HBRUSH mkBgHbrush(COLORREF color) {
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 1;
	bmi.bmiHeader.biHeight = 1;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	HDC hdc = GetDC(NULL);
	void* pBits;
	HBITMAP hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	ReleaseDC(NULL, hdc);
	HBRUSH hBg = NULL;
	if (hBmp) {
		COLORREF* pixels = (COLORREF*)pBits;
		*pixels = color;
		hBg = CreatePatternBrush(hBmp);
	}
	return hBg;
}

COLORREF hbrushToColorref(HBRUSH hBrush) {
	COLORREF col = CLR_INVALID;
	LOGBRUSH lb;
	int result = GetObject(hBrush, sizeof(LOGBRUSH), &lb);
	if ((result) && (lb.lbStyle == BS_SOLID)) col = lb.lbColor;
	return col;
}

bool triggerSaveAs() {
	OPENFILENAME ofn;
	wchar_t szFile[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = L"All Files\0*.*\0";
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.nMaxFile = sizeof(szFile);
	ofn.nFilterIndex = 1;  // Default filter index
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL; // Initial directory
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn) != TRUE) {
		return false;
	}
	FILE* file = _wfopen(ofn.lpstrFile, L"w");
	if (file == NULL) {
		return true;
	}
	int len = SendMessageW(edit, WM_GETTEXTLENGTH, 0, 0);
	len++;
	wchar_t* buf = (wchar_t*)malloc(len * sizeof(wchar_t));
	SendMessageW(edit, WM_GETTEXT, len, (LPARAM)buf);
	fwprintf(file, L"%ls", buf);
	fclose(file);
	free(buf); // phatgpt recommendation, dont blame me 4 actually caring abt mem leaks 4 once
	return true;
}

HMENU populateMenu() {
	HMENU res = CreateMenu();
	HMENU fileM = CreateMenu();
	HMENU editM = CreateMenu();
	// file
	AppendMenu(fileM, MF_STRING, EXIT, L"excite");
	AppendMenu(fileM, MF_STRING, ABUT, L"aboot");
	AppendMenu(fileM, MF_SEPARATOR, 0, NULL);
	AppendMenu(fileM, MF_STRING, SAVE, L"sane");
	AppendMenu(fileM, MF_STRING, OPEN, L"oven");
	// file
	// edit
	AppendMenu(editM, MF_STRING, FSIZ, L"clange fort....");
	// edit
	AppendMenu(res, MF_POPUP, (UINT_PTR)fileM, L"filet");
	AppendMenu(res, MF_POPUP, (UINT_PTR)editM, L"emit");
	return res;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_GETMINMAXINFO:
			MINMAXINFO* mmi = (MINMAXINFO*)lParam;
			int* wh_ = getSizeOfHwnd();
			mmi->ptMinTrackSize.x = 600;
			mmi->ptMinTrackSize.y = 380;
			return 0;
		case WM_SIZE:
			// break; // wip
			int* wh = getSizeOfHwnd();
			MoveWindow(edit, 0, 0, wh[0], wh[1], true); //  - (GetSystemMetrics(SM_CXVSCROLL) / 2) //  - (GetSystemMetrics(SM_CYVSCROLL) * 3)
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case EXIT:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case ABUT:
					MessageBox(hwnd, L"text eitorer\n\nmade by grad man 2 be silly", L"aboot", MB_OK | MB_ICONINFORMATION);
					break;
				case SAVE:
					triggerSaveAs();
					break;
				case FSIZ:
					if (noFonting) {
						MessageBox(hwnd, L"fuck you, not gonna do it now!", L"clange fort....", MB_OK | MB_ICONERROR);
						break;
					}
					CHOOSEFONT cf;
					HFONT hf;
					LOGFONT lf;
					ZeroMemory(&cf, sizeof(cf));
					ZeroMemory(&lf, sizeof(lf));
					GetObject((HFONT)SendMessage(edit, WM_GETFONT, 0, 0), sizeof(LOGFONT), &lf);
					cf.lStructSize = sizeof(cf);
					cf.hwndOwner = hwnd;
					// cf.hInstance = 
					cf.lpLogFont = &lf;
					cf.Flags = CF_SCREENFONTS | CF_SHOWHELP | CF_SCALABLEONLY | CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT;
					BOOL font = ChooseFont(&cf);
					SendMessage(edit, WM_SETFONT, (WPARAM)(CreateFontIndirect(&lf)), true);
					noFonting = rand() <= (RAND_MAX / 27.);
					if (noFonting) MessageBox(hwnd, L"actually this setting got sentient so now im not gonna work anymoar\ngl trying to set the font now, ur stuck like this.", L"clange fort....", MB_OK | MB_ICONWARNING);
					break;
				case OPEN: // stub
					break;
			}
			return 0;
		case WM_CLOSE:
			int result = MessageBoxW(hwnd, L"doh yo went two sane beform exciting text eitorer?", L"text eitorer", MB_YESNOCANCEL | MB_ICONWARNING);
			if ((result == IDCANCEL) || ((result == IDYES) && (!(triggerSaveAs())))) return 0;
			DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_CTLCOLOREDIT:
			break;
			HDC hdcEdit = (HDC)wParam;
			initWinCol();
			SetBkColor(hdcEdit, TRANSPARENT);
			return (INT_PTR)(initWinCol());
		/*
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
			case WM_KEYDOWN:
				if ((wParam == 'A') && (GetKeyState(VK_CONTROL) & 0x8000)) { // Check if Ctrl is pressed
					SendMessage(edit, EM_SETSEL, 0, -1); // Select all text in the edit control
					return 0;
				}
				break;
		*/
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	time_t t;
	time(&t);
	srand((unsigned int)(GetCurrentProcessId() + (t >> 16) + t));
	const wchar_t CLASS_NAME[] = L"TextEitorer";
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = CLASS_NAME;
	if (!RegisterClass(&wc)) {
		return 0;
	}
	HMENU menu = populateMenu();
	hwnd = CreateWindowEx(
		0, // window styles
		CLASS_NAME, // class
		L"text eitorer", // title
		WS_OVERLAPPEDWINDOW, // WS_TILED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, moar window styles ig
		CW_USEDEFAULT, CW_USEDEFAULT, // xy
		600, // width
		380, // height
		NULL, // parent
		menu,
		hInstance, // an instance of h (100% NOT CLICKBAIT!!!!)
		NULL // useless app data
	);
	// winCol = ;
	if (hwnd == NULL) {
		return 0;
	}
	edit = CreateWindowEx(
		0,
		L"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd, NULL, hInstance, NULL
	);
	HFONT hFont = CreateFont(
		24, // is font tall?..
		0,
		0,
		0,
		FW_NORMAL, // ...is it fat?
		0, // italicisation
		0, // underline
		0, // strike
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DRAFT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"comic sans ms"
	);
	SendMessage(edit, WM_SETFONT, (WPARAM)hFont, 1);
	// SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX); // fuck you past me
	ShowWindow(hwnd, nCmdShow);
	SendMessage(hwnd, WM_SETICON, 0, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON)));
	// SetTimer(hwnd, 1, 1000 / 60, NULL);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}