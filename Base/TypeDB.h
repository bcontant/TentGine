#pragma once

#include <map>

#include "Singleton.h"
#include "Name.h"
#include "TypeFunction.h"

struct TypeInfo;

//-----------------------------------------
//TypeDB
//-----------------------------------------

class TypeDB : public StaticSingleton<TypeDB>
{
	friend struct TypeInfo;

	MAKE_STATIC_SINGLETON(TypeDB);

	TypeDB();
	~TypeDB();
public:
	template <typename T>
	const TypeInfo* GetType() const;
	const TypeInfo* GetType(Name name) const;

	void AddFunction( TypeFunction&& in_function );

	void Clear();

private:
	void RegisterTypeInfo(const TypeInfo* in_pInfo);

	std::map<Name, const TypeInfo*> mTypeInfos;

	std::vector<TypeFunction> mGlobalFunctions;
};

//-----------------------------------------
//TypeDB
//-----------------------------------------

template <typename T>
const TypeInfo* TypeDB::GetType() const
{
	return TypeInfo::Get<T>();
}