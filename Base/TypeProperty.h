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
	const TypeInfo* m_pTypeInfo;

	// Offset of this property within the type
	size_t m_Offset = 0;
};

#include "TypeInfo.h"

//-----------------------------------------
// TypeProperty
//-----------------------------------------

template <typename T, typename OBJECT_TYPE>
TypeProperty::TypeProperty(Name name, T OBJECT_TYPE::*property)
	: m_Name(name)
	, m_pTypeInfo(TypeInfo::Get<T>())
	, m_Offset(offsetof(OBJECT_TYPE, *property))
{
}
