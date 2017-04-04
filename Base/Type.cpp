#include "precompiled.h"

#include "TypeDB.h"
#include "Type.h"


//-----------------------------------------
// Type
//-----------------------------------------
Type& Type::AddProperty(TypeProperty&& field)
{
	CHECK_ERROR(ErrorCode::PropertyAlreadyAdded, GetProperty(field.m_Name.text) == nullptr);

	if (GetProperty(field.m_Name.text) == nullptr)
		vProperties.push_back(field);

	return *this;
}

Type& Type::AddEnumConstant(EnumConstant&& enumConst)
{
	CHECK_ERROR(ErrorCode::EnumValueAlreadyAdded, GetEnumConstant(enumConst.value) == nullptr);

	if (GetEnumConstant(enumConst.value) == nullptr)
		vEnumConstants.push_back(enumConst);

	return *this;
}

Type& Type::AddFunction(TypeFunction&& function)
{
	CHECK_ERROR(ErrorCode::FunctionAlreadyAdded, GetFunction(function.m_Name.text) == nullptr);

	if (GetFunction(function.m_Name.text) == nullptr)
		vFunctions.push_back(function);

	return *this;
}

bool Type::IsDerivedFrom(const TypeInfo* in_TypeInfo)
{
	if(this == in_TypeInfo->m_MetaInfo)
		return true;

	if(this->base_type == nullptr)
		return false;

	return this->base_type->IsDerivedFrom(in_TypeInfo);
}

const TypeProperty* Type::GetProperty(const string_char* in_name) const
{
	for (size_t i = 0; i < vProperties.size(); i++)
	{
		// This is just a hash comparison
		if (Name(in_name) == vProperties[i].m_Name)
			return &vProperties[i];
	}

	// Recurse up through base type
	if (base_type)
		return base_type->GetProperty(in_name);

	return nullptr;
}

const TypeFunction* Type::GetFunction(const string_char* in_name) const
{
	for (size_t i = 0; i < vFunctions.size(); i++)
	{
		// This is just a hash comparison
		if (Name(in_name) == vFunctions[i].m_Name)
			return &vFunctions[i];
	}

	// Recurse up through base type
	if (base_type)
		return base_type->GetFunction(in_name);

	return nullptr;
}

const EnumConstant* Type::GetEnumConstant(u64 in_value) const
{
	for (size_t i = 0; i < vEnumConstants.size(); i++)
	{
		if (in_value == vEnumConstants[i].value)
			return &(vEnumConstants[i]);
	}
	return nullptr;
}
