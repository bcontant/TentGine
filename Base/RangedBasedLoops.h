#pragma once

#include "Types.h"

class range_iter
{
public:
	range_iter(u32 value) : m_value(value) {}

	bool operator!= (const range_iter& it) { return it.m_value != m_value; }
	range_iter& operator++ () { m_value++; return *this; }

	u32 operator*() { return m_value; }
private:
	u32 m_value;
};

class range
{
public:
	range(u32 count) : m_start(0), m_end(count) {}
	range(u32 start, u32 end) : m_start(start), m_end(end + 1) {}

	range_iter begin() { return range_iter(m_start); }
	range_iter end() { return range_iter(m_end); }

private:
	u32 m_start;
	u32 m_end;
};
