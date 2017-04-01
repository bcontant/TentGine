#pragma once

#include <map>

#include "Singleton.h"
#include "Name.h"
#include "Type.h"

//-----------------------------------------
//Utility	
//-----------------------------------------
std_string GenerateTypeName(const char* in_typeName);

template <typename T> const string_char* GetTypeName()
{
	static std_string type_name = GenerateTypeName(typeid(T).name());
	return type_name.c_str();
}

//-----------------------------------------
//TypeDB
//-----------------------------------------

class TypeDB : public StaticSingleton<TypeDB>
{
	MAKE_STATIC_SINGLETON(TypeDB);

	TypeDB();
	~TypeDB();
public:

	template <typename T>
	Type& RegisterType();

	template <typename T>
	Type* GetType();

	Type* GetType(Name name);

private:
	using TypeMap = std::map<Name, Type*>;
	TypeMap mTypes;
};


//-----------------------------------------
//TypeDB
//-----------------------------------------
template <typename T>
Type& TypeDB::RegisterType()
{
	Type* type = nullptr;
	Name name = GetTypeName<T>();

	TypeMap::iterator type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		type = new Type;
		mTypes[name] = type;
	}
	else
	{
		Assert(STRCMP(type_i->first.text, name.text) == 0);
		type = type_i->second;
	}

	// Apply type properties
	type->name = name;
	type->size = SizeOf<T>::val;

	type->constructor = GetDefaultConstructor<T>();
	type->destructor = GetDestructor<T>();
	type->default_obj = GetDefaultObject<T>();

	return *type;
}

template <typename T>
Type* TypeDB::GetType()
{
	Name name = GetTypeName<T>();
	TypeMap::iterator type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		//TODO: This works but is it really necessary?
		if(!IsFundamental<T>::val) 
			return &RegisterType<T>();
		else
			return nullptr;
	}
	return type_i->second;
}