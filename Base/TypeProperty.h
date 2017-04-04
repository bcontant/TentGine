#pragma once

#include "Name.h"

struct TypeInfo;

struct TypeProperty
{
	TypeProperty() {}

	template <typename T, typename OBJECT_TYPE>
	TypeProperty(Name name, T OBJECT_TYPE::*property);

	void* TypeProperty::GetPtr(void* object) const;

	// C++ name of the property, unscoped
	Name m_Name = L("");

	//Type of the property
	const TypeInfo* type_info;

	// Offset of this property within the type
	size_t offset = 0;
};

#include "InstanceTypeInfo.h"

//-----------------------------------------
// TypeProperty
//-----------------------------------------

template <typename T, typename OBJECT_TYPE>
TypeProperty::TypeProperty(Name name, T OBJECT_TYPE::*property)
	: m_Name(name)
	, type_info(TypeInfo::Get<T>())
	, offset(offsetof(OBJECT_TYPE, *property))
{
}
