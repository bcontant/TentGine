#pragma once

#include "Serializer.h"
#include "IContainer.h"
#include "TypeOperators.h"
#include "Name.h"

struct Type;

struct TypeInfo
{
private:
	template <typename T>
	static const TypeInfo* Get_Internal();

public:
	template <typename T>
	static typename std::enable_if<!std::is_reference<T>::value, const TypeInfo*>::type
	Get();

	template <typename T>
	static typename std::enable_if<std::is_reference<T>::value, const TypeInfo*>::type
	Get();

	template <typename T> 
	static const TypeInfo* Get(T);

	bool IsPointer() const;
	bool IsDerivedFrom(const TypeInfo* in_pInfo) const;
	void* GetVTableAddress() const;

private:
	TypeInfo(Name in_name, Type* in_type, const TypeInfo* in_pDereferencedType, int in_size, IContainer* in_container, TypeOperators in_functions);
	~TypeInfo();

	TypeInfo(const TypeInfo&) = delete;
	TypeInfo& operator=(const TypeInfo&) = delete;

public:
	//Interface to access private operators
	void* New() const;
	void Constructor(void* data) const;
	void* CopyConstructor(const void* src) const;
	void AssignmentOperator(void* dst, const void* src) const;

	void Delete(void* data) const;
	void Destructor(void* data) const;

	void Serialize(Serializer* serializer, void* obj, const string_char* in_name) const;

	Name m_Name;

	Type* m_MetaInfo;

	const TypeInfo* m_pDereferencedTypeInfo;

	int m_Size;

	IContainer* m_pContainer;

private:
	const TypeInfo* TypeInfo::GetActualObjectTypeInfo(void* obj) const;

	TypeOperators m_Operators;
};

#include "TypeDB.h"

template <typename T>
const TypeInfo* TypeInfo::Get_Internal()
{
	static const TypeInfo type_info(
		Name(GetTypeName<T>()),
		TypeDB::GetInstance()->GetType<Strip<T>::Type>(),
		::IsPointer<T>::val ? TypeInfo::Get<StripPointer_Once<T>::Type>() : nullptr,
		SizeOf<T>::val,
		GetContainer<T>(),
		TypeOperators::Get<T>()
	);
	return &type_info;
}

//TODO : Can't I just StripReference all this fucking time?
template <typename T>
typename std::enable_if<!std::is_reference<T>::value, const TypeInfo*>::type
TypeInfo::Get()
{
	return Get_Internal<T>();
}

template <typename T>
typename std::enable_if<std::is_reference<T>::value, const TypeInfo*>::type
TypeInfo::Get()
{
	return Get_Internal<StripReference<T>::Type>();
}

template <typename T> 
const TypeInfo* TypeInfo::Get(T)
{ 
	return Get<T>(); 
}