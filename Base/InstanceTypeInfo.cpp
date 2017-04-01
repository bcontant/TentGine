#include "precompiled.h"

#include "InstanceTypeInfo.h"
#include "Type.h"

InstanceTypeInfo::InstanceTypeInfo(Name in_name, Type* in_type, bool in_is_pointer, int in_size, IContainer* in_container, ConstructObjectFunc in_ctor, DestructObjectFunc in_dtor, DefaultObjectFunc in_default_obj, SerializeObjectFunc in_serialize_func)
	: name(in_name)
	, type(in_type)
	, is_pointer(in_is_pointer)
	, size(in_size)
	, container(in_container)
	, constructor(in_ctor)
	, destructor(in_dtor)
	, default_obj(in_default_obj)
	, serialize_func(in_serialize_func)
{
}

InstanceTypeInfo::~InstanceTypeInfo()
{
	delete container;
}

bool InstanceTypeInfo::operator== (const InstanceTypeInfo& rhs) const
{
	return type == rhs.type && is_pointer == rhs.is_pointer && size == rhs.size;
}

bool InstanceTypeInfo::operator != (const InstanceTypeInfo& rhs) const
{
	return !(*this == rhs);
}


void InstanceTypeInfo::Copy(void* dst, const void* src) const
{
	memcpy(dst, src, size);
}

void* InstanceTypeInfo::NewCopy(const void* src) const
{
	void* data = New();

	memcpy(data, src, size);
	return data;
}

void* InstanceTypeInfo::New() const
{
	void* data = new u8[size];

	if (constructor)
		constructor(data);
	else if (type && type->constructor && type->name == name)
		type->constructor(data);
	else
		Assert(false);
	
	return data;
}

void* InstanceTypeInfo::DefaultObject() const
{
	if (default_obj)
		return default_obj();
	else if (type && type->default_obj && type->name == name)
		return type->default_obj();
	
	Assert(false);
	return nullptr;
}

void  InstanceTypeInfo::Delete(void* data) const
{
	if (destructor)
		destructor(data);

	delete[] reinterpret_cast<u8*>(data);
	data = nullptr;
}