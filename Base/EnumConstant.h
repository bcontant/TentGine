#pragma once

#include "Name.h"

struct EnumConst
{
	EnumConst(Name name, int value) : name(name), value(value) { }
	Name name;
	int value = 0;
};