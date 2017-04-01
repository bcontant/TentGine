#pragma once

//#include "Type.h"
#include "Serializer.h"
#include "IContainer.h"
#include "Name.h"

struct Type;

struct InstanceTypeInfo
{
	template <typename T>
	static const InstanceTypeInfo* Get();

	template <typename T> 
	static const InstanceTypeInfo* Get(T);

	InstanceTypeInfo(Name in_name, Type* in_type, bool in_is_pointer, int in_size, IContainer* in_container, ConstructObjectFunc in_ctor, DestructObjectFunc in_dtor, DefaultObjectFunc in_default_obj, SerializeObjectFunc in_serialize_func);
	~InstanceTypeInfo();

	bool operator == (const InstanceTypeInfo& rhs) const;
	bool operator != (const InstanceTypeInfo& rhs) const;

	//TODO : These could be better, safer, and elsewhere.  Investigate.
	template<typename T>
	void* AssignmentOperator(const void* src) const;

	void  Copy(void* dst, const void* src) const;
	void* NewCopy(const void* src) const;
	
	void* New() const;
	void* DefaultObject() const;
	void  Delete(void* data) const;

	Type* type;
	Name name;

	bool is_pointer;
	int size;

	IContainer* container;

	//TODO : Add Assignment Operator and Copy Constructor
	//TODO : Group the "virtual" methods
	ConstructObjectFunc constructor;
	DestructObjectFunc destructor;
	DefaultObjectFunc default_obj;
	SerializeObjectFunc serialize_func;
};

template <typename T>
const InstanceTypeInfo* InstanceTypeInfo::Get()
{
	static const InstanceTypeInfo type_info(
		Name(GetTypeName<T>()),
		TypeDB::GetInstance()->GetType<StripPointer<T>::Type>(),
		IsPointer<T>::val,
		SizeOf<T>::val,
		GetContainer<T>(),
		GetDefaultConstructor<T>(),
		GetDestructor<T>(),
		GetDefaultObject<T>(),
		GetSerializeFunc<T>()
	);
	return &type_info;
}

template <typename T> 
const InstanceTypeInfo* InstanceTypeInfo::Get(T)
{ 
	return Get<T>(); 
}

template<typename T>
void* InstanceTypeInfo::AssignmentOperator(const void* src) const
{
	void* data = New();

	*((T*)data) = *((T*)src);
	return data;
}
