#include "precompiled.h"

#include "Type.h"


//-----------------------------------------
// Type
//-----------------------------------------
Type& Type::AddProperty(TypeProperty&& field)
{
	Assert(!isEnum);
	vProperties.push_back(field);
	return *this;
}

Type& Type::AddEnumConstant(EnumConst&& enumConst)
{
	Assert(isEnum);
	vEnumConstants.push_back(enumConst);
	return *this;
}

Type& Type::AddFunction(TypeFunction&& function)
{
	Assert(!isEnum);
	vFunctions.push_back(function);
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

	return nullptr;
}

TypeFunction* Type::GetFunction(const string_char* in_name)
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

void  Type::Copy(void* dst, const void* src) const
{
	memcpy(dst, src, size);
}
void* Type::NewCopy(const void* src) const
{
	void* data = new u8[size];
	constructor(data);
	memcpy(data, src, size);
	return data;
}

void* Type::New(void) const
{
	void* data = new u8[size];
	constructor(data);
	return data;
}

void  Type::Delete(void* data) const
{
	destructor(data);
	delete[] reinterpret_cast<u8*>(data);
	data = nullptr;
}
