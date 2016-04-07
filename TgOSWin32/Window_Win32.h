#pragma once

#include "Window.h"

class Window_Win32 : public Window
{
public:
	virtual void InitWindow(int width, int height, bool fullscreen, void* hInstance);
};
