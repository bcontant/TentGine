#include "precompiled.h"

#include "Variant.h"
#include "TypeInfo.h"

//---------------------------------
// VariantBase
//---------------------------------

VariantBase::VariantBase(const TypeInfo* in_typeInfo, void* in_data)
	: typeInfo(in_typeInfo)
	, data(in_data)
{
}

void VariantBase::Serialize(Serializer* in_serializer) const
{
	if(typeInfo)
		typeInfo->Serialize(in_serializer, data, L(""));
}

//---------------------------------
// Variant
//---------------------------------

Variant::Variant() 
	: VariantBase(nullptr, nullptr)
{
}

Variant::Variant(const Variant& rhs)
	: VariantBase( rhs.typeInfo, nullptr )
{
	if(typeInfo)
		data = typeInfo->CopyConstructor(rhs.data);
}

Variant::Variant(const VariantRef& rhs)
	: VariantBase( rhs.GetTypeInfo(), nullptr )
{
	if(typeInfo)
		data = typeInfo->CopyConstructor(rhs.GetData());
}

Variant::Variant(const AutoVariant& rhs)
	: VariantBase( rhs.m_impl->GetTypeInfo(), nullptr )
{
	if (typeInfo)
		data = typeInfo->CopyConstructor(rhs.m_impl->GetData());
}

Variant::Variant(const TypeInfo* in_typeInfo, void* data)
	: VariantBase( in_typeInfo, nullptr )
{
	if (typeInfo)
		data = typeInfo->CopyConstructor(data);
}

Variant::~Variant()
{
	if (typeInfo)
		typeInfo->Delete(data);
}

Variant& Variant::operator=(const Variant& rhs)
{
	if (this == &rhs)
		return *this;

	CopyFrom(rhs.typeInfo, rhs.data);
	return *this;
}

void Variant::CopyFrom(const TypeInfo* in_typeInfo, void* in_data)
{
	if (typeInfo == in_typeInfo)
	{
		if (typeInfo)
			typeInfo->AssignmentOperator(data, in_data);
	}
	else
	{
		if (typeInfo)
			typeInfo->Delete(data);

		typeInfo = in_typeInfo;

		if (typeInfo)
			data = typeInfo->CopyConstructor(in_data);
	}
}


//---------------------------------
// VariantRef
//---------------------------------


VariantRef::VariantRef() : VariantBase(nullptr, nullptr)
{
}

VariantRef::VariantRef(const TypeInfo* in_typeInfo, void *in_data) 
	: VariantBase(in_typeInfo, in_data)
{
}

VariantRef::VariantRef(const Variant& rhs) 
	: VariantBase(rhs.GetTypeInfo(), rhs.GetData())
{
}

VariantRef::VariantRef(const VariantRef& rhs) 
	: VariantBase(rhs.typeInfo, rhs.data)
{
}

VariantRef::VariantRef(const AutoVariant& rhs)
	: VariantBase(rhs.m_impl->GetTypeInfo(), rhs.m_impl->GetData())
{
}

VariantRef& VariantRef::operator=(const VariantRef& rhs)
{
	typeInfo = rhs.typeInfo;
	data = rhs.data;
	return *this;
}

VariantRef& VariantRef::operator=(const Variant& rhs)
{
	typeInfo = rhs.GetTypeInfo();
	data = rhs.GetData();
	return *this;
}

AutoVariant::AutoVariant(const VariantRef&& rhs)
	: m_bIsReference(true)
{
	m_impl = new VariantRef(rhs);
}

AutoVariant::~AutoVariant()
{
	delete m_impl;
	m_impl = nullptr;
}

