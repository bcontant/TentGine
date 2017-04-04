#pragma once

#include "Name.h"

struct EnumConstant
{
	EnumConstant(Name m_Name, u64 value) : m_Name(m_Name), value(value) { }
	Name m_Name;
	u64 value = 0;
};