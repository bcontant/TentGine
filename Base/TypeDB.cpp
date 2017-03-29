#include "precompiled.h"

#include "TypeDB.h"

//-----------------------------------------
// Utils
//-----------------------------------------

const char* ENUM_PREFIX = "enum ";
const char* STRUCT_PREFIX = "struct ";
const char* CLASS_PREFIX = "class ";


std_string GenerateTypeName(const char* in_typeName)
{
	if (strstr(in_typeName, ENUM_PREFIX) != 0)
		return FROM_STRING(in_typeName + strlen(ENUM_PREFIX));

	if (strstr(in_typeName, STRUCT_PREFIX) != 0)
		return FROM_STRING(in_typeName + strlen(STRUCT_PREFIX));

	if (strstr(in_typeName, CLASS_PREFIX) != 0)
		return FROM_STRING(in_typeName + strlen(CLASS_PREFIX));

	return FROM_STRING(in_typeName);
}

//-----------------------------------------
// TypeDB
//-----------------------------------------

TypeDB::TypeDB()
{
}

Type* TypeDB::GetType(Name name)
{
	TypeMap::iterator type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		Assert(false);
		return nullptr;
	}
	return type_i->second;
}
