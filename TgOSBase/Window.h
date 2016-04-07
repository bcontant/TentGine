#pragma once

class Window
{
public:
	void* GetWindowHandle() const { return m_windowHandle; }
	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }

	virtual void InitWindow(int width, int height, bool fullscreen, void* hInstance) = 0;

protected:
	void* m_windowHandle;
	int m_width;
	int m_height;

public:
	static Window* Create(int width, int height, bool fullscreen, void* hInstance);
};

