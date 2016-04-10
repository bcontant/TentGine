#include "precompiled.h"

IWindowMessageCallback::IWindowMessageCallback(Window* pWindow)
	: m_pWindow(pWindow)
{
	if(m_pWindow)
		m_pWindow->RegisterMessageCallback(this);
}
IWindowMessageCallback::~IWindowMessageCallback()
{
	if (m_pWindow)
		m_pWindow->UnregisterMessageCallback(this);
	m_pWindow = 0;
}

void Window::RegisterMessageCallback(IWindowMessageCallback* pCallback)
{
	m_vMessageCallbacks.push_back(pCallback);
}

Window::Window() 
	:IWindowMessageCallback(0)
{
	m_pWindow = this;
	m_pWindow->RegisterMessageCallback(this);
}

void Window::UnregisterMessageCallback(IWindowMessageCallback* pCallback)
{
	auto it = m_vMessageCallbacks.begin();
	while (it != m_vMessageCallbacks.end())
	{
		if (*it == pCallback)
		{
			m_vMessageCallbacks.erase(it);
			break;
		}
		it++;
	}
}

bool Window::NotifyCallbacks(void* hwnd, unsigned int message, uint64_t wParam, int64_t lParam)
{
	bool bHandled = false;
	for (int i = 0; i < m_vMessageCallbacks.size(); i++)
	{
		bHandled = m_vMessageCallbacks[i]->MessageCallback(hwnd, message, wParam, lParam);
	}
	return bHandled;
}

