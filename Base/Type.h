#pragma once

#include "Name.h"
#include "EnumConstant.h"
#include "TypeProperty.h"
#include "TypeFunction.h"

template <typename TYPE> struct IsEnum { static constexpr bool val = std::is_enum<TYPE>::value; };
template <typename TYPE> struct IsPointer { static constexpr bool val = false; };
template <typename TYPE> struct IsPointer<TYPE*> { static constexpr bool val = true; };
template <typename TYPE> struct StripPointer { using Type = TYPE; };
template <typename TYPE> struct StripPointer<TYPE*> { using Type = TYPE; };

template <typename TYPE> void ConstructObject(void* object) { new (object) TYPE; }
template <typename TYPE> void DestructObject(void* object) { ((TYPE*)object)->TYPE::~TYPE(); }

using ConstructObjectFunc = void(*)(void*);
using DestructObjectFunc = void(*)(void*);

// The basic type representation
struct Type
{
	Type() {}
	virtual ~Type() {};

	Type& AddProperty(TypeProperty&& property);
	Type& AddEnumConstant(EnumConst&& enumConst);
	Type& AddFunction(TypeFunction&& function);

	template <typename TYPE>
	Type& Base();

	TypeProperty* GetProperty(const string_char* in_name);
	TypeFunction* GetFunction(const string_char* in_name);

	void  Copy(void* dst, const void* src) const;
	void* NewCopy(const void* src) const;
	void* New(void) const;
	void  Delete(void* data) const;

	//Base type
	Type* base_type = nullptr;

	// Scoped C++ name of the type
	Name name = L("");

	// Pointers to the constructor and destructor functions
	ConstructObjectFunc constructor = nullptr;
	DestructObjectFunc destructor = nullptr;

	// Result of sizeof(type) operation
	size_t size = 0;

	// Vector of properties for this type
	std::vector<TypeProperty> vProperties;

	// Vector of functions for this type
	std::vector<TypeFunction> vFunctions;

	// Only used if the type is an enum type
	bool isEnum = false;
	std::vector<EnumConst> vEnumConstants;
};

//-----------------------------------------
//Type
//-----------------------------------------
template <typename TYPE>
Type& Type::Base()
{
	base_type = TypeDB::GetInstance()->GetType<TYPE>();
	return *this;
}