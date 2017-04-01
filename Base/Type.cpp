#include "precompiled.h"

#include "Type.h"


//-----------------------------------------
// Type
//-----------------------------------------
Type& Type::AddProperty(TypeProperty&& field)
{
	//TODO : Check duplicate
	vProperties.push_back(field);
	return *this;
}

Type& Type::AddEnumConstant(EnumConstant&& enumConst)
{
	//TODO : Check duplicate
	vEnumConstants.push_back(enumConst);
	return *this;
}

Type& Type::AddFunction(TypeFunction&& function)
{
	//TODO : Check duplicate
	vFunctions.push_back(function);
	return *this;
}

const TypeProperty* Type::GetProperty(const string_char* in_name) const
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

	return nullptr;
}

const TypeFunction* Type::GetFunction(const string_char* in_name) const
{
	for (size_t i = 0; i < vFunctions.size(); i++)
	{
		// This is just a hash comparison
		if (Name(in_name) == vFunctions[i].name)
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
