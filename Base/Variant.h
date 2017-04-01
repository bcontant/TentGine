#pragma once

#include "InstanceTypeInfo.h"

struct Serializer;

//---------------------------------
// VariantBase
//---------------------------------

class VariantBase
{
public:
	template <typename T>
	T& As();

	template <typename T>
	const T& As() const;

	template <typename T>
	operator T() const;

	const InstanceTypeInfo* GetTypeInfo() const { return typeInfo; }
	void* GetData() const { return data; }

	void Serialize(Serializer* in_serializer) const;

protected:
	VariantBase();
	VariantBase(const InstanceTypeInfo* in_typeInfo, void* data);

	template <typename T>
	T& FailedCast() const;

	const InstanceTypeInfo* typeInfo;
	void* data;
};

template <typename T>
T& VariantBase::As()
{
	//T to T
	if (typeInfo == InstanceTypeInfo::Get<T>())
	{
		if (data != nullptr)
			return *reinterpret_cast<T*>(data);
	}

	//T* to any*
	if (typeInfo != nullptr && typeInfo->is_pointer && InstanceTypeInfo::Get<T>()->is_pointer)
	{
		if (data != nullptr)
			return *reinterpret_cast<T*>(data);
	}
	
	//Pointer to value
	/*else if (typeInfo == InstanceTypeInfo::Get<T*>())
	{
		T* pointer = *reinterpret_cast<T**>(data);

		if(pointer != nullptr)
			return *pointer;
		else
			Assert(false);
	}
	//Value to Pointer
	else if (InstanceTypeInfo::Get<T>()->is_pointer && !typeInfo->is_pointer && typeInfo->type == InstanceTypeInfo::Get<T>()->type)
	{
		return *reinterpret_cast<T*>(&data);
	}*/

	return FailedCast<T>();
}

template <typename T>
const T& VariantBase::As() const
{
	//T to T
	if (typeInfo == InstanceTypeInfo::Get<T>())
	{
		if (data != nullptr)
			return *reinterpret_cast<T*>(data);
	}

	//T* to any*
	if (typeInfo != nullptr && typeInfo->is_pointer && InstanceTypeInfo::Get<T>()->is_pointer)
	{
		if (data != nullptr)
			return *reinterpret_cast<T*>(data);
	}

	return FailedCast<T>();
}

template <typename T>
VariantBase::operator T() const
{
	return As<T>();
}

template <typename T>
T& VariantBase::FailedCast() const
{
	AssertMsg(false, L("Variant of type (%s) could not be return as type (%s). Incoming garbage."), typeInfo ? typeInfo->name.text : L("null"), InstanceTypeInfo::Get<T>()->name.text);

	if(data)
		return *reinterpret_cast<T*>(data);
	else
		return *((T*)(&data));
}

//---------------------------------
// Variant
//---------------------------------

class Variant : public VariantBase
{
public:
	template <typename T>
	Variant(const T& value);
	Variant(const InstanceTypeInfo* in_typeInfo, void* in_data);
	Variant();

	template <typename T>
	Variant& operator=(const T& value);
	Variant& operator=(const Variant& rhs);
};

template <typename T>
Variant::Variant(const T& value)
	: VariantBase( InstanceTypeInfo::Get<T>(), nullptr )
{
	data = typeInfo->NewCopy(&value);
}

template <typename T>
Variant& Variant::operator= (const T& value)
{
	const InstanceTypeInfo* newTypeInfo = InstanceTypeInfo::Get<T>();

	if (typeInfo != newTypeInfo)
	{
		if(typeInfo)
			typeInfo->Delete(data);

		typeInfo = newTypeInfo;
		//data = typeInfo->NewCopy(&value); //TODO : Typeless assignement operator and copy constructors
		data = typeInfo->AssignmentOperator<T>(&value);
	}
	else
	{
		typeInfo->Copy(data, &value);
	}
	return *this;
}

//---------------------------------
// RefVariant
//---------------------------------

class RefVariant : public VariantBase
{
public:
	template <typename T>
	RefVariant(const T& value);
	RefVariant(const RefVariant& rhs);
	RefVariant(const Variant& rhs);
	RefVariant(const InstanceTypeInfo* in_typeInfo, void *in_data);
	RefVariant();

	RefVariant& operator=(const RefVariant& rhs);
	RefVariant& operator=(const Variant& rhs);
	template <typename T>
	RefVariant& operator=(const T& rhs);
};

template <typename T>
RefVariant::RefVariant(const T& value) : VariantBase( InstanceTypeInfo::Get<T>(), const_cast<T*>(&value))
{
}

template <typename T>
RefVariant& RefVariant::operator=(const T& rhs)
{
	typeInfo = InstanceTypeInfo::Get<T>();
	data = const_cast<T*>(&rhs);
	return *this;
}