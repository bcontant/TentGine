#pragma once

#include <vector>

class Window;

//--------------------------------------------------------------------------------
class IWindowMessageCallback
{
public:
	IWindowMessageCallback(Window* pWindow);
	virtual ~IWindowMessageCallback();
	virtual bool MessageCallback(void* hwnd, unsigned int message, uint64_t wParam, int64_t lParam) = 0;

protected:
	Window* m_pWindow;
};

//--------------------------------------------------------------------------------
class Window : public IWindowMessageCallback
{
public:
	static Window* Create(int width, int height, bool fullscreen, void* hInstance);

public:
	Window();

	void* GetWindowHandle() const { return m_windowHandle; }
	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }

	void RegisterMessageCallback(IWindowMessageCallback* pCallback);
	void UnregisterMessageCallback(IWindowMessageCallback* pCallback);
	
	virtual bool ProcessMessages() = 0;

protected:
	virtual void InitWindow(int width, int height, bool fullscreen, void* hInstance) = 0;
	virtual bool NotifyCallbacks(void* hwnd, unsigned int message, uint64_t wParam, int64_t lParam);

	void* m_windowHandle;
	int m_width;
	int m_height;
	
private:
	std::vector<IWindowMessageCallback*> m_vMessageCallbacks;
};

