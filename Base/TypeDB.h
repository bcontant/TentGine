#pragma once

#include <map>

#include "Singleton.h"
#include "Name.h"

struct Type;

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

	/*template <typename T>
	Type& SoftRegisterType();*/

	template <typename T>
	Type* GetType();

	Type* GetType(Name name);

	void Clear();

private:
	std::map<Name, Type*> mTypes;
};

#include "Type.h"

//-----------------------------------------
//TypeDB
//-----------------------------------------
template <typename T>
Type& TypeDB::RegisterType()
{
	Type* type = nullptr;
	Name name = GetTypeName<T>();

	auto type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		type = new Type;
		mTypes[name] = type;
	}
	else
	{
		CHECK_ERROR(ErrorCode::HashCollision, STRCMP(type_i->first.text, name.text) == 0);
		type = type_i->second;
	}

	// Apply type properties
	//type->typeInfo = TypeInfo::Get<T>();
	type->m_Name = name;
	type->m_Operators = { 0 };

	//Assert(type->typeInfo->m_Name == type->m_Name);

	return *type;
}

/*template <typename T>
Type& TypeDB::SoftRegisterType()
{
	Name name = GetTypeName<T>();

	CHECK_ERROR(ErrorCode::Undefined, mTypes.find(GetTypeName<T>()) == mTypes.end());

	Type* type = new Type;
	mTypes[name] = type;
	type->m_Name = name;
	type->m_Operators = TypeOperators::Get<T>();

	return *type;
}*/

template <typename T>
Type* TypeDB::GetType()
{
	Name name = GetTypeName<T>();
	auto type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		return &RegisterType<T>();
	}
	return type_i->second;
}