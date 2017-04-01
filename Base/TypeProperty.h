#pragma once

#include "Name.h"

struct InstanceTypeInfo;

struct TypeProperty
{
	TypeProperty() {}

	template <typename T, typename OBJECT_TYPE>
	TypeProperty(Name name, T OBJECT_TYPE::*property);

	void* TypeProperty::GetPtr(void* object) const;

	// C++ name of the property, unscoped
	Name name = L("");

	//Type of the property
	const InstanceTypeInfo* type_info;

	// Offset of this property within the type
	size_t offset = 0;
};

//-----------------------------------------
// TypeProperty
//-----------------------------------------

template <typename T, typename OBJECT_TYPE>
TypeProperty::TypeProperty(Name name, T OBJECT_TYPE::*property)
	: name(name)
	, type_info(InstanceTypeInfo::Get<T>())
	, offset(offsetof(OBJECT_TYPE, *property))
{
}
