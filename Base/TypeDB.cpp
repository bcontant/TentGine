#include "precompiled.h"

#include "TypeDB.h"
#include "TypeInfo.h"

//-----------------------------------------
// TypeDB
//-----------------------------------------

TypeDB::TypeDB()
{
}

TypeDB::~TypeDB()
{
}

void TypeDB::Clear()
{
	mTypeInfos.clear();
}

void TypeDB::RegisterTypeInfo(const TypeInfo* in_pInfo)
{
	//TODO : Filter?  Useful map + complete map for debugging?

	auto type_i = mTypeInfos.find(in_pInfo->m_Name);
	if (type_i == mTypeInfos.end())
	{
		mTypeInfos[in_pInfo->m_Name] = in_pInfo;
	}
	else
	{
		CHECK_ERROR_MSG(ErrorCode::HashCollision, STRCMP(type_i->first, in_pInfo->m_Name) == 0, L("Hash Collision occured inside mTypesInfos map"));
	}
}

const TypeInfo* TypeDB::GetType(Name name) const
{
	auto type_i = mTypeInfos.find(name);
	if (type_i == mTypeInfos.end())
	{
		return nullptr;
	}
	return type_i->second;
}


void TypeDB::AddFunction( TypeFunction&& in_function )
{
	mGlobalFunctions.push_back(in_function);
}