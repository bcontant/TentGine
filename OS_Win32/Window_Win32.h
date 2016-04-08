#pragma once

#include "../OS_Base/Window.h"
#include <windows.h>

class Window_Win32 : public Window
{
public:
	virtual ~Window_Win32();

	virtual bool ProcessMessages();
	virtual bool MessageCallback(void* hwnd, unsigned int message, uint64_t wParam, int64_t lParam);

protected:
	virtual void InitWindow(int width, int height, bool fullscreen, void* hInstance);

private:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};
