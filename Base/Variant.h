#pragma once

#include "Type.h"

class Variant
{
public:
	template <typename TYPE>
	Variant(const TYPE& value);

	Variant(const Type* in_type, void* in_data);

	template <typename TYPE>
	TYPE& GetValue(void);

	template <typename TYPE>
	const TYPE& GetValue(void) const;

	const Type* GetType() const;

	template <typename TYPE>
	Variant& operator= (const TYPE& value);

	Variant& operator=(const Variant& rhs);

private:
	const Type* type;
	void* data;
};

template <typename TYPE>
Variant::Variant(const TYPE& value)
	: type(TypeDB::GetInstance()->GetType<TYPE>())
	, data(nullptr)
{
	data = type->NewCopy(&value);
}

template <typename TYPE>
TYPE& Variant::GetValue(void)
{
	return *reinterpret_cast<TYPE*>(data);
}

template <typename TYPE>
const TYPE& Variant::GetValue(void) const
{
	return reinterpret_cast<TYPE*>(data);
}


template <typename TYPE>
Variant& Variant::operator= (const TYPE& value)
{
	Type* newType = TypeDB::GetInstance()->GetType<TYPE>();

	if (type != newType)
	{
		if (type)
			type->Delete(data);

		type = newType;
		data = type->NewCopy(&value);
	}
	else
	{
		type->Copy(data, &value);
	}
	return *this;
}