#include "precompiled.h"

#include "Reflection.h"

//-----------------------------------------
// Utils
//-----------------------------------------

const char* ENUM_PREFIX = "enum ";
const char* STRUCT_PREFIX = "struct ";
const char* CLASS_PREFIX = "class ";

void AssignFloat(void* pDst, void* pSrc)
{
	*((float*)pDst) = *((float*)pSrc);
};


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

u32 GenerateStringHash(const string_char* in_string)
{
	std::string str = TO_STRING(in_string);

	u32 hash = 0;
	for(string_char c : str)
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

//-----------------------------------------
// Type
//-----------------------------------------
Type& Type::AddProperty(TypeProperty&& field)
{
	vProperties.push_back(field);
	return *this;
}

Type& Type::AddEnumConstant(EnumConst&& enumConst)
{
	Assert(isEnum);
	vEnumConstants.push_back(enumConst);
	return *this;
}

TypeProperty* Type::GetProperty(const string_char* in_name)
{
	for (size_t i = 0; i < vProperties.size(); i++)
	{
		// This is just a hash comparison
		if (Name(in_name) == vProperties[i].name)
			return &vProperties[i];
	}

	// Recurse up through base type
	if (base_type)
		return base_type->GetProperty(in_name);

	return 0;
}

//-----------------------------------------
// TypeDB
//-----------------------------------------

TypeDB::TypeDB()
{
	/*RegisterType<bool>();
	RegisterType<u8>();
	RegisterType<s8>();
	RegisterType<u16>();
	RegisterType<s16>();
	RegisterType<u32>();
	RegisterType<s32>();
	RegisterType<float>();*/
	//etc
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
