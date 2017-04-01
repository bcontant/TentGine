#include "precompiled.h"

#include "Variant.h"
#include "InstanceTypeInfo.h"

//---------------------------------
// VariantBase
//---------------------------------

VariantBase::VariantBase() 
	: typeInfo(nullptr)
	, data(nullptr)
{
}

VariantBase::VariantBase(const InstanceTypeInfo* in_typeInfo, void* in_data)
	: typeInfo(in_typeInfo)
	, data(in_data)
{
}

void VariantBase::Serialize(Serializer* in_serializer) const
{
	if(typeInfo)
		typeInfo->serialize_func(in_serializer, typeInfo, data, L(""));
}

//---------------------------------
// Variant
//---------------------------------

Variant::Variant(const InstanceTypeInfo* in_typeInfo, void* in_data)
	: VariantBase(in_typeInfo, in_data)
{
}

Variant::Variant() 
	: VariantBase(nullptr, nullptr)
{
}

Variant& Variant::operator=(const Variant& rhs)
{
	if (this == &rhs)
		return *this;

	if (typeInfo == rhs.typeInfo)
	{	
		if(typeInfo)
			typeInfo->Copy(data, rhs.data);
	}
	else
	{
		if(typeInfo)
			typeInfo->Delete(data);

		typeInfo = rhs.typeInfo;

		if (typeInfo)
			data = typeInfo->NewCopy(rhs.data);
		else
			data = nullptr;
	}

	return *this;
}


//---------------------------------
// Variant
//---------------------------------


RefVariant::RefVariant() : VariantBase(nullptr, nullptr)
{
}

RefVariant::RefVariant(const InstanceTypeInfo* in_typeInfo, void *in_data) : VariantBase(in_typeInfo, in_data)
{
}

RefVariant::RefVariant(const RefVariant& rhs) : VariantBase(rhs.typeInfo, rhs.data)
{
}

RefVariant::RefVariant(const Variant& rhs) : VariantBase(rhs.GetTypeInfo(), rhs.GetData())
{
}

RefVariant& RefVariant::operator=(const RefVariant& rhs)
{
	typeInfo = rhs.typeInfo;
	data = rhs.data;
	return *this;
}

RefVariant& RefVariant::operator=(const Variant& rhs)
{
	typeInfo = rhs.GetTypeInfo();
	data = rhs.GetData();
	return *this;
}