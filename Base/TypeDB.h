#pragma once

#include <map>

#include "Singleton.h"
#include "Name.h"

struct Type;
struct TypeInfo;

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

	void Clear();

	void RegisterTypeInfo(const TypeInfo* in_pInfo);
	const TypeInfo* FindFromVTable(void* in_pVTable) const;

private:
	std::map<Name, Type*> mTypes;
	std::map<Name, const TypeInfo*> mTypeInfos;
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
	type->m_Name = name;
	//type->m_Operators = TypeOperators::Get<T>();
	type->m_Operators = { 0 };
	type->m_Operators.vtable_address = TypeOperators::Get<T>().vtable_address;

	return *type;
}

template <typename T>
Type* TypeDB::GetType()
{
	Name name = GetTypeName<T>();
	auto type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		if(IsUserType<T>::val)
			return &RegisterType<T>();
		else
			return nullptr;
	}
	return type_i->second;
}