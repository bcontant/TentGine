#include <windows.h>
#include "Window_Win32.h"

Window* Window::Create(int width, int height, bool fullscreen, void* hInstance)
{
	Window_Win32* pWindow = new Window_Win32();
	pWindow->InitWindow(width, height, fullscreen, hInstance);
	return pWindow;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	bool wasHandled = false;
	switch (message)
	{
	case WM_SIZE:
	{
		int windowWidth = LOWORD(lParam);
		int windowHeight = HIWORD(lParam);

		/*if (g_Renderer != NULL)
		{
			wchar_t debug[512];
			swprintf_s(debug, 512, L"Resizing. [Backbuffer : %d x %d] [ClientRect : %d x %d]\n", g_Renderer->GetBackBuffer()->GetWidth(), g_Renderer->GetBackBuffer()->GetHeight(), windowWidth, windowHeight);
			OutputDebugString(debug);

			g_Renderer->GetBackBuffer()->pBackBufferView->Release();
			g_Renderer->GetBackBuffer()->pBackBuffer->Release();

			HRESULT hr = S_OK;
			hr = g_Renderer->pSwapChain->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
			hr = g_Renderer->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&g_Renderer->GetBackBuffer()->pBackBuffer);
			hr = g_Renderer->pD3D11Device->CreateRenderTargetView(g_Renderer->GetBackBuffer()->pBackBuffer, NULL, &g_Renderer->GetBackBuffer()->pBackBufferView);

			g_Renderer->pD3D11DeviceContext->OMSetRenderTargets(1, &g_Renderer->GetBackBuffer()->pBackBufferView, NULL);

			D3D11_VIEWPORT viewport = {};
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = (FLOAT)windowWidth;
			viewport.Height = (FLOAT)windowHeight;

			g_Renderer->pD3D11DeviceContext->RSSetViewports(1, &viewport);
		}*/

		result = 1;
		wasHandled = true;
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		result = 1;
		wasHandled = true;
		break;
	}
	}

	if (!wasHandled)
		result = DefWindowProc(hwnd, message, wParam, lParam);

	return result;
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

	m_windowHandle = (void*)CreateWindow(L"DX11App", L"DX11 App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, (HINSTANCE)hInstance, NULL);

	ShowWindow((HWND)m_windowHandle, SW_SHOWNORMAL);
	UpdateWindow((HWND)m_windowHandle);

	m_width = width;
	m_height = height;
}
