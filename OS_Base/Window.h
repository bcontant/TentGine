#pragma once

#include <vector>

class Window;

//--------------------------------------------------------------------------------
class IWindowMessageCallback
{
public:
	IWindowMessageCallback(Window* pWindow);
	virtual ~IWindowMessageCallback();
	virtual bool MessageCallback(void* hwnd, u32 message, u64 wParam, s64 lParam) = 0;

protected:
	Window* m_pWindow;
};

//--------------------------------------------------------------------------------
class Window : public IWindowMessageCallback
{
public:
	static Window* Create(s32 width, s32 height, bool fullscreen, void* hInstance);

public:
	Window();

	void* GetWindowHandle() const { return m_windowHandle; }
	u32 GetWidth() const { return m_width; }
	u32 GetHeight() const { return m_height; }

	void RegisterMessageCallback(IWindowMessageCallback* pCallback);
	void UnregisterMessageCallback(IWindowMessageCallback* pCallback);
	
	virtual bool ProcessMessages() = 0;

protected:
	virtual void InitWindow(s32 width, s32 height, bool fullscreen, void* hInstance) = 0;
	virtual bool NotifyCallbacks(void* hwnd, u32 message, u64 wParam, s64 lParam);

	void* m_windowHandle;
	s32 m_width;
	s32 m_height;
	
private:
	std::vector<IWindowMessageCallback*> m_vMessageCallbacks;
};

