#ifdef WIN32

#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif
#include <Windows.h>
#include <windowsx.h>
#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

#include <rund/wrapper.h>
#include <stdio.h>

static HWND window;
static uint32_t* buffer;
static size_t width, height;

static events_t handlers;

static uint16_t translate_keycode(uint64_t win32_keycode);

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
	case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
		if(handlers.mouse_down)
			handlers.mouse_down(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (uMsg == WM_LBUTTONDOWN) ? 0 : (uMsg == WM_RBUTTONDOWN) ? 1 : 2);
		break;
	case WM_LBUTTONUP: case WM_RBUTTONUP: case WM_MBUTTONUP:
		if(handlers.mouse_up)
			handlers.mouse_up(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (uMsg == WM_LBUTTONUP) ? 0 : (uMsg == WM_RBUTTONUP) ? 1 : 2);
		break;
	case WM_KEYDOWN:
		if(handlers.key_down)
			handlers.key_down(translate_keycode(wParam));
		break;
	case WM_KEYUP:
		if(handlers.key_up)
			handlers.key_up(translate_keycode(wParam));
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void create_window(const rund_app_t* app, events_t _handlers)
{
	handlers = _handlers;

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

static uint16_t translate_keycode(uint64_t win32_keycode)
{
	switch (win32_keycode)
	{
	case 0x41: return !GetKeyState(VK_SHIFT) ? RK_A : RK_a;
	case 0x42: return !GetKeyState(VK_SHIFT) ? RK_B : RK_b;
	case 0x43: return !GetKeyState(VK_SHIFT) ? RK_C : RK_c;
	case 0x44: return !GetKeyState(VK_SHIFT) ? RK_D : RK_d;
	case 0x45: return !GetKeyState(VK_SHIFT) ? RK_E : RK_e;
	case 0x46: return !GetKeyState(VK_SHIFT) ? RK_F : RK_f;
	case 0x47: return !GetKeyState(VK_SHIFT) ? RK_G : RK_g;
	case 0x48: return !GetKeyState(VK_SHIFT) ? RK_H : RK_h;
	case 0x49: return !GetKeyState(VK_SHIFT) ? RK_I : RK_i;
	case 0x4A: return !GetKeyState(VK_SHIFT) ? RK_J : RK_j;
	case 0x4B: return !GetKeyState(VK_SHIFT) ? RK_K : RK_k;
	case 0x4C: return !GetKeyState(VK_SHIFT) ? RK_L : RK_l;
	case 0x4D: return !GetKeyState(VK_SHIFT) ? RK_M : RK_m;
	case 0x4E: return !GetKeyState(VK_SHIFT) ? RK_N : RK_n;
	case 0x4F: return !GetKeyState(VK_SHIFT) ? RK_O : RK_o;
	case 0x50: return !GetKeyState(VK_SHIFT) ? RK_P : RK_p;
	case 0x51: return !GetKeyState(VK_SHIFT) ? RK_Q : RK_q;
	case 0x52: return !GetKeyState(VK_SHIFT) ? RK_R : RK_r;
	case 0x53: return !GetKeyState(VK_SHIFT) ? RK_S : RK_s;
	case 0x54: return !GetKeyState(VK_SHIFT) ? RK_T : RK_t;
	case 0x55: return !GetKeyState(VK_SHIFT) ? RK_U : RK_u;
	case 0x56: return !GetKeyState(VK_SHIFT) ? RK_V : RK_v;
	case 0x57: return !GetKeyState(VK_SHIFT) ? RK_W : RK_w;
	case 0x58: return !GetKeyState(VK_SHIFT) ? RK_X : RK_x;
	case 0x59: return !GetKeyState(VK_SHIFT) ? RK_Y : RK_y;
	case 0x5A: return !GetKeyState(VK_SHIFT) ? RK_Z : RK_z;

	case 0x30: return RK_0;
	case 0x31: return RK_1;
	case 0x32: return RK_2;
	case 0x33: return RK_3;
	case 0x34: return RK_4;
	case 0x35: return RK_5;
	case 0x36: return RK_6;
	case 0x37: return RK_7;
	case 0x38: return RK_8;
	case 0x39: return RK_9;

	case VK_SPACE: return RK_space;
	}

    fprintf(stderr, "Unmapped keycode: %d\n", win32_keycode);
    exit(-1);
}

#endif