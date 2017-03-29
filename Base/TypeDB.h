#pragma once

#include <map>

#include "Singleton.h"
#include "Name.h"
#include "Type.h"

//-----------------------------------------
//Utility	
//-----------------------------------------
std_string GenerateTypeName(const char* in_typeName);

template <typename TYPE> const string_char* GetTypeName()
{
	static std_string type_name = GenerateTypeName(typeid(TYPE).name());
	return type_name.c_str();
}

//-----------------------------------------
//TypeDB
//-----------------------------------------

class TypeDB : public StaticSingleton<TypeDB>
{
	MAKE_STATIC_SINGLETON(TypeDB);

	TypeDB();
public:

	template <typename TYPE>
	Type& RegisterType();

	template <typename TYPE>
	Type* GetType();

	Type* GetType(Name name);

private:
	using TypeMap = std::map<Name, Type*>;
	TypeMap mTypes;
};


//-----------------------------------------
//TypeDB
//-----------------------------------------
template <typename TYPE>
Type& TypeDB::RegisterType()
{
	Type* type = 0;
	Name name = GetTypeName<TYPE>();

	TypeMap::iterator type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		type = new Type;
		mTypes[name] = type;
	}
	else
	{
		type = type_i->second;
	}

	// Apply type properties
	type->name = name;
	type->size = sizeof(TYPE);
	type->constructor = ConstructObject<TYPE>;
	type->destructor = DestructObject<TYPE>;
	type->isEnum = IsEnum<TYPE>::val;
	return *type;
}

template <typename TYPE>
Type* TypeDB::GetType()
{
	Name name = GetTypeName<TYPE>();
	TypeMap::iterator type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		return &RegisterType<TYPE>();
	}
	return type_i->second;
}