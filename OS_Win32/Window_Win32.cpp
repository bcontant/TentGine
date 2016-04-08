#include <windows.h>
#include "Window_Win32.h"

Window* Window::Create(int width, int height, bool fullscreen, void* hInstance)
{
	Window* pWindow = new Window_Win32();
	pWindow->InitWindow(width, height, fullscreen, hInstance);
	return pWindow;
}

Window_Win32::~Window_Win32()
{
}

void Window_Win32::InitWindow(int width, int height, bool fullscreen, void* hInstance)
{
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = (HINSTANCE)hInstance;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	wcex.lpszClassName = L"DX11App";

	RegisterClassEx(&wcex);

	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	m_windowHandle = (void*)CreateWindow(L"DX11App", L"DX11 App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, (HINSTANCE)hInstance, this);

	ShowWindow((HWND)m_windowHandle, SW_SHOWNORMAL);
	UpdateWindow((HWND)m_windowHandle);

	m_width = width;
	m_height = height;
}

bool Window_Win32::ProcessMessages()
{
	MSG msg;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

bool Window_Win32::MessageCallback(void* hwnd, unsigned int message, uint64_t wParam, int64_t lParam)
{
	switch (message)
	{
		case WM_SIZE:
		{
			m_width = max(8, LOWORD(lParam));
			m_height = max(8, HIWORD(lParam));
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
	}
	return false;
}

LRESULT CALLBACK Window_Win32::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE)
	{
		LPCREATESTRUCT createStruct;
		createStruct = (LPCREATESTRUCT)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)createStruct->lpCreateParams);
	}

	Window_Win32* pWindow = (Window_Win32*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (pWindow)
		pWindow->NotifyCallbacks(hwnd, message, wParam, lParam);

	return DefWindowProc(hwnd, message, wParam, lParam);
}