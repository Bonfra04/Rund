#ifdef WIN32

#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif
#include <Windows.h>
#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

#include "wrapper.h"
#include <stdio.h>

static uint32_t* buffer;
static size_t width, height;

static LRESULT CALLBACK window_event(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static HWND window;
void create_window(const rund_app_t* app)
{
	buffer = (uint32_t*)malloc(app->width * app->height * sizeof(uint32_t));
	memset((void*)buffer, 0x000000, app->width * app->height * sizeof(uint32_t));
	width = app->width;
	height = app->height;

	WNDCLASS wc;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = window_event;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszMenuName = NULL;
	wc.hbrBackground = NULL;
	wc.lpszClassName = L"RUND_APPLICATION";
	RegisterClass(&wc);

	// Define window furniture
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

	// Keep client size as requested
	RECT rWndRect = { 0, 0, app->width, app->height };
	AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
	int width = rWndRect.right - rWndRect.left;
	int height = rWndRect.bottom - rWndRect.top;

	wchar_t wide_title[100];
	swprintf(wide_title, 100, L"%hs", app->title);

	window = CreateWindowEx(dwExStyle, L"RUND_APPLICATION", wide_title, dwStyle,
		(int)app->x, (int)app->y, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
}

static MSG msg;
bool event_loop()
{
	BOOL open = GetMessage(&msg, NULL, 0, 0);
	if (open <= 0)
		return false;

	TranslateMessage(&msg);
	DispatchMessage(&msg);
	
	return true;
}

void put_pixel(size_t x, size_t y, uint32_t color)
{
	y = height - y - 1;
	if (x > width || y > height)
		return;
	buffer[y * width + x] = color;
}

void flush()
{
	HDC hdc = GetDC(window);

	BITMAPINFO bitmap_info;
	bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
	bitmap_info.bmiHeader.biWidth = width;
	bitmap_info.bmiHeader.biHeight = height;
	bitmap_info.bmiHeader.biPlanes = 1;
	bitmap_info.bmiHeader.biBitCount = 32;
	bitmap_info.bmiHeader.biCompression = BI_RGB;

	StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, buffer, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}

#endif