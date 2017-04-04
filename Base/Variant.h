#pragma once

struct Serializer;
struct TypeInfo;

//---------------------------------
// VariantBase
//---------------------------------
class AutoVariant;

class VariantBase
{
	friend class AutoVariant;

public:
	template <typename T>
	T& As();

	template <typename T>
	operator T&();

	template <typename T>
	const T& As() const;

	template <typename T>
	operator const T&() const;
	
	const TypeInfo* GetTypeInfo() const { return typeInfo; }
	void* GetData() const { return data; }

	void Serialize(Serializer* in_serializer) const;

protected:
	VariantBase() = delete;
	VariantBase(const TypeInfo* in_typeInfo, void* data);
	virtual ~VariantBase() { typeInfo = nullptr; data = nullptr; }

	template <typename T>
	T& FailedCast() const;

	const TypeInfo* typeInfo;
	void* data;
};


//---------------------------------
// Variant
//---------------------------------
class VariantRef;

class Variant : public VariantBase
{
public:
	template <typename T>
	Variant(const T& value);
	Variant(const Variant& rhs);
	Variant(const VariantRef& rhs);
	Variant(const AutoVariant& rhs);
	Variant(const TypeInfo* in_typeInfo, void* data);
	Variant();

	virtual ~Variant();

	template <typename T>
	Variant& operator=(const T& value);
	Variant& operator=(const Variant& rhs);

private:
	void CopyFrom(const TypeInfo* in_typeInfo, void* in_data);
};

//---------------------------------
// VariantRef
//---------------------------------

class VariantRef : public VariantBase
{
public:
	template <typename T>
	VariantRef(const T& value);
	VariantRef(const Variant& rhs);
	VariantRef(const VariantRef& rhs);
	VariantRef(const AutoVariant& rhs);
	VariantRef(const TypeInfo* in_typeInfo, void *in_data);
	VariantRef();

	VariantRef& operator=(const VariantRef& rhs);
	VariantRef& operator=(const Variant& rhs);
	template <typename T>
	VariantRef& operator=(const T& rhs);
};


//---------------------------------
// AutoVariant
//---------------------------------

//Auto selects variant type base on init type.
//Used for proper ReturnType forwarding with a single Prototype
class AutoVariant
{
	friend class VariantRef;
	friend class Variant;

public:
	template <typename T>
	AutoVariant(const T& value);

	template <typename T>
	AutoVariant(const T&& value);
	AutoVariant(const VariantRef&& rhs);

	virtual ~AutoVariant();

	template <typename T> 
	operator T&();
private:
	AutoVariant() = delete;

	bool m_bIsReference;
	VariantBase* m_impl = nullptr;
};

#include "InstanceTypeInfo.h"
#include "ErrorHandler.h"

//---------------------------------
// VariantBase
//---------------------------------
template <typename T>
T& VariantBase::As()
{
	//T to T
	if (typeInfo == TypeInfo::Get<T>())
	{
		if (data != nullptr)
			return *reinterpret_cast<StripReference<T>::Type*>(data);
	}

	//T* to any*
	if (typeInfo != nullptr && typeInfo->m_bIsPointer && TypeInfo::Get<T>()->m_bIsPointer)
	{
		//Allow down casting to a parent class
		if (data != nullptr && typeInfo->IsDerivedFrom(TypeInfo::Get<T>()))
			return *reinterpret_cast<StripReference<T>::Type*>(data);

		//Allow casting to void*
		if (data != nullptr && TypeInfo::Get<T>() == TypeInfo::Get<void*>())
			return *reinterpret_cast<StripReference<T>::Type*>(data);
	}
	
	return FailedCast<T>();
}

template <typename T>
VariantBase::operator T&()
{
	return As<T>();
}

template <typename T>
const T& VariantBase::As() const
{
	return ((VariantBase*)this)->As<T>();
}

template <typename T>
VariantBase::operator const T&() const
{
	return As<T>();
}

template <typename T>
T& VariantBase::FailedCast() const
{
	CHECK_ERROR_MSG(ErrorCode::FailedCast, false, L("Variant of type (%s) could not be returned as type (%s). Incoming garbage."), typeInfo ? typeInfo->m_Name.text : L("null"), TypeInfo::Get<T>()->m_Name.text);

	if(data)
		return *reinterpret_cast<StripReference<T>::Type*>(data);
	else
	{
		static size_t sNullData = 0;
		sNullData = 0;
		return (T&)sNullData;
	}
}



//---------------------------------
// Variant
//---------------------------------


template <typename T>
Variant::Variant(const T& value)
	: VariantBase( TypeInfo::Get<T>(), nullptr )
{
	if(typeInfo)
		data = typeInfo->CopyConstructor(&value);
}

template <typename T>
Variant& Variant::operator= (const T& value)
{
	CopyFrom(TypeInfo::Get<T>(), (void*)&value);
	return *this;
}


//---------------------------------
// VariantRef
//---------------------------------

template <typename T>
VariantRef::VariantRef(const T& value) : VariantBase( TypeInfo::Get<T>(), const_cast<T*>(&value))
{
}

template <typename T>
VariantRef& VariantRef::operator=(const T& rhs)
{
	typeInfo = TypeInfo::Get<T>();
	data = const_cast<T*>(&rhs);
	return *this;
}


// AutoVariant

template <typename T>
AutoVariant::AutoVariant(const T& value)
	: m_bIsReference(true)
{
	m_impl = new VariantRef(value);	
}

template <typename T>
AutoVariant::AutoVariant(const T&& value)
	: m_bIsReference(false)
{
	if (TypeInfo::Get<AutoVariant>() == TypeInfo::Get<T>() && ((AutoVariant&)value).m_bIsReference)
	{
		m_impl = new VariantRef(value);
		m_bIsReference = true;
	}
	else
	{
		m_impl = new Variant(value);
		m_bIsReference = false;
	}
}

template <typename T> 
AutoVariant::operator T&() 
{ 
	return m_impl->As<T>(); 
}