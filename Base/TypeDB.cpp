#include "precompiled.h"

#include "TypeDB.h"

//-----------------------------------------
// TypeDB
//-----------------------------------------

TypeDB::TypeDB()
{
}

TypeDB::~TypeDB()
{
	for (auto it : mTypes)
	{
		delete it.second;
	}
}

void TypeDB::Clear()
{
	mTypes.clear();
}

Type* TypeDB::GetType(Name name)
{
	auto type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		Assert(false);
		return nullptr;
	}
	return type_i->second;
}
