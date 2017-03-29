#include "precompiled.h"

#include "Variant.h"

Variant::Variant(const Type* in_type, void* in_data) 
	:type(in_type)
	,data(in_data) 
{
}

const Type* Variant::GetType() const 
{ 
	return type; 
}

Variant& Variant::operator=(const Variant& rhs)
{
	if (this == &rhs)
		return *this;

	if (type)
	{
		if (type == rhs.type)
		{
			type->Copy(data, rhs.data);
		}
		else
		{
			type->Delete(data);
			type = rhs.GetType();

			if (type)
			{
				data = type->NewCopy(&rhs.data);
			}
		}
	}

	return *this;
}
