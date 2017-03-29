#include "precompiled.h"

#include "Name.h"

u32 GenerateStringHash(const string_char* in_string)
{
	//TODO : Lazy Hash, replace with something better
	//TODO : Detect collisions

	std::string str = TO_STRING(in_string);

	u32 hash = 0;
	for (string_char c : str)
	{
		hash += c;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

Name::Name()
	:hash(0)
	,text(nullptr)
{
}

Name::Name(const string_char* name)
	:hash(GenerateStringHash(name))
	,text(name)
{
}

bool Name::operator< (const Name& in_other) const 
{ 
	return hash < in_other.hash; 
}

bool Name::operator== (const Name& in_other) const 
{ 
	return hash == in_other.hash; 
}
