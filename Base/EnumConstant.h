#pragma once

#include "Name.h"

struct EnumConstant
{
	EnumConstant(Name name, u64 value) : name(name), value(value) { }
	Name name;
	u64 value = 0;
};