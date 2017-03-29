#pragma once

#include "Types.h"

u32 GenerateStringHash(const string_char*);

struct Name
{
	Name();
	Name(const string_char* name);

	const string_char* text = nullptr;
	unsigned int hash = 0;

	bool operator< (const Name& in_other) const;
	bool operator== (const Name& in_other) const;
};
