#pragma once

#include "EnumConstant.h"
#include "TypeProperty.h"
#include "TypeFunction.h"
#include "TypeOperators.h"
#include "Name.h"

struct TypeInfo
{
public:
	template <typename T>
	static const TypeInfo* Get();

	template <typename T> 
	static const TypeInfo* Get(T);

	template <typename T>
	void SetBaseClass();

	void SetDefaultConstructor(ConstructObjectFunc f);

	void AddProperty(TypeProperty&& property);
	void AddEnumConstant(EnumConstant&& enumConst);
	void AddFunction(TypeFunction&& function);

public:
	const string_char* GetName() const { return m_Name;	}

	bool IsPointer() const;
	bool IsDerivedFrom(const TypeInfo* in_pInfo) const;
	bool IsCastableAs(void* in_pObj, const TypeInfo* in_pInfo) const;
	size_t GetSize() const { return m_Size; }
	IContainer* GetContainer() const { return m_pContainer; }

	const TypeProperty* GetProperty(const string_char* in_name) const;
	const TypeFunction* GetFunction(const string_char* in_name) const;
	const EnumConstant* GetEnumConstant(u64 in_value) const;
	const EnumConstant* GetEnumConstant(const string_char* in_name) const;

public:
	void* New() const;
	void Constructor(void* data) const;
	void* CopyConstructor(const void* src) const;
	void AssignmentOperator(void* dst, const void* src) const;
	void Delete(void* data) const;
	void Destructor(void* data) const;

	void SerializeProperties(Serializer* serializer, void* obj) const;
	void DeserializeProperties(Serializer* serializer, void* obj) const;
	void Serialize(Serializer* serializer, void* obj, const string_char* in_name) const;
	void Deserialize(Serializer* serializer, void* obj, const string_char* in_name) const;

	//TypeInfo Name
	Name m_Name;

private:
	TypeInfo(Name in_name, const TypeInfo* in_pDereferencedType, size_t in_size, IContainer* in_container, TypeOperators in_functions);
	~TypeInfo();

	TypeInfo(const TypeInfo&) = delete;
	TypeInfo& operator=(const TypeInfo&) = delete;

	template <typename T>
	static const TypeInfo* Get_Internal();

	const TypeInfo* TypeInfo::GetActualObjectTypeInfo(void* obj) const;

private:

	//Base type
	const TypeInfo* base_type = nullptr;

	//Dereferenced type (if this is a pointer)
	const TypeInfo* m_pDereferencedTypeInfo = nullptr;

	size_t m_Size;
	IContainer* m_pContainer;

	// Pointers to the constructor and destructor functions
	TypeOperators m_Operators;

	// Vector of properties for this type
	std::vector<TypeProperty> vProperties;

	// Vector of functions for this type
	std::vector<TypeFunction> vFunctions;

	// Only used if the type is an enum type
	std::vector<EnumConstant> vEnumConstants;
};

#include "Serializer.h"
#include "IContainer.h"

template <typename T>
const TypeInfo* TypeInfo::Get()
{
	return Get_Internal<StripReference<T>::Type>();
}

template <typename T> 
const TypeInfo* TypeInfo::Get(T)
{ 
	return Get<T>(); 
}

template <typename T>
void TypeInfo::SetBaseClass()
{
	static_assert((std::is_same<Strip<T>::Type, T>::value), "BaseClass is not a stripped type.  Did you declare a * or a &?");

	base_type = TypeInfo::Get<T>();
}

template <typename T>
const TypeInfo* TypeInfo::Get_Internal()
{
	static const TypeInfo type_info(
		Name(GetTypeName<T>()),
		::IsPointer<T>::val ? TypeInfo::Get<StripPointer_Once<T>::Type>() : nullptr,
		SizeOf<T>::val,
		::GetContainer<T>(),
		TypeOperators::Get<T>()
	);
	return &type_info;
}

