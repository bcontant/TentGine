#pragma once

template <class T>
class Rect
{
public:
	T Width() const { return m_right - m_left; }
	T Height() const { return m_bottom - m_top; }

	T m_left;
	T m_top;
	T m_right;
	T m_bottom;
};
