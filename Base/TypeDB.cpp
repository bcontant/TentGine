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
	mTypeInfos.clear();
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

void TypeDB::RegisterTypeInfo(const TypeInfo* in_pInfo)
{
	mTypeInfos[in_pInfo->m_Name] = in_pInfo;
}

const TypeInfo* TypeDB::FindFromVTable(void* in_pVTable) const
{
	for (auto it : mTypeInfos)
	{
		if(it.second->GetVTableAddress() == in_pVTable)
			return it.second;
	}

	/*for (auto it : mTypes)
	{
		if (it.second->m_Operators.vtable_address == in_pVTable)
			return it.second;
	}*/
	return nullptr;
}
