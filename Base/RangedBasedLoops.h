#pragma once

class range_iter
{
public:
	range_iter(unsigned int value) : m_value(value) {}

	bool operator!= (const range_iter& it) { return it.m_value != m_value; }
	range_iter& operator++ () { m_value++; return *this; }

	unsigned int operator*() { return m_value; }
private:
	unsigned int m_value;
};

class range
{
public:
	range(unsigned int count) : m_start(0), m_end(count) {}
	range(unsigned int start, unsigned int end) : m_start(start), m_end(end + 1) {}

	range_iter begin() { return range_iter(m_start); }
	range_iter end() { return range_iter(m_end); }

private:
	unsigned int m_start;
	unsigned int m_end;
};
