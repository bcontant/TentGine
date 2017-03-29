#pragma once

#include "Name.h"
#include "IContainer.h"

struct Type;

struct TypeProperty
{
	TypeProperty() {}

	template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
	TypeProperty(Name name, PROPERTY_TYPE OBJECT_TYPE::*property);

	// Overload for std::vector container TypeProperties
	template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
	TypeProperty(Name name, std::vector<PROPERTY_TYPE> OBJECT_TYPE::*property);

	void* TypeProperty::GetPtr(void* object);

	// C++ name of the property, unscoped
	Name name = L("");

	//Type of the property
	Type* type = nullptr;

	// Is this a pointer property? Note that this becomes a flag later on...
	bool is_pointer = false;

	// Offset of this property within the type
	size_t offset = 0;

	IContainer* container = nullptr;
};

//-----------------------------------------
// TypeProperty
//-----------------------------------------

template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
TypeProperty::TypeProperty(Name name, PROPERTY_TYPE OBJECT_TYPE::*property)
	: name(name)
	, type(TypeDB::GetInstance()->GetType<StripPointer<PROPERTY_TYPE>::Type>())
	, is_pointer(IsPointer<PROPERTY_TYPE>::val)
	, offset(offsetof(OBJECT_TYPE, *property))
{
}

template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
TypeProperty::TypeProperty(Name name, std::vector<PROPERTY_TYPE> OBJECT_TYPE::*property)
	: name(name)
	, type(TypeDB::GetInstance()->GetType<StripPointer<PROPERTY_TYPE>::Type>())
	, is_pointer(IsPointer<PROPERTY_TYPE>::val)
	, offset(offsetof(OBJECT_TYPE, *property))
{
	container = new VectorContainer<PROPERTY_TYPE>();
}
