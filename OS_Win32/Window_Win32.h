#pragma once

#include "../OS_Base/Window.h"
#include <windows.h>

//--------------------------------------------------------------------------------
class Window_Win32 : public Window
{
public:
	virtual ~Window_Win32();

	virtual bool ProcessMessages();
	virtual bool MessageCallback(void* hwnd, u32 message, u64 wParam, s64 lParam);

protected:
	virtual void InitWindow(s32 width, s32 height, bool fullscreen, void* hInstance);

private:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};
