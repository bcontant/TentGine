#pragma once

#include "Name.h"
#include "EnumConstant.h"
#include "TypeProperty.h"
#include "TypeFunction.h"
#include "Utils.h"

#include "Logger.h"

// The basic type representation
struct Type
{
	Type() {}
	virtual ~Type() {};

	Type& AddProperty(TypeProperty&& property);
	Type& AddEnumConstant(EnumConstant&& enumConst);
	Type& AddFunction(TypeFunction&& function);

	template <typename T>
	Type& Base();

	void SetCtor(ConstructObjectFunc f) { constructor = f; }
	void SetDefaultObject(DefaultObjectFunc f) { default_obj = f; }

	const TypeProperty* GetProperty(const string_char* in_name) const;
	const TypeFunction* GetFunction(const string_char* in_name) const;
	const EnumConstant* GetEnumConstant(u64 in_value) const;

	//Base type
	Type* base_type = nullptr;

	// Scoped C++ name of the type
	Name name = L("");

	// Pointers to the constructor and destructor functions
	ConstructObjectFunc constructor = nullptr;
	DefaultObjectFunc default_obj = nullptr;

	//TODO : Is this even needed? They reside in InstanceTypeInfo and it's not overridable
	DestructObjectFunc destructor = nullptr;
	
	// Result of sizeof(type) operation
	size_t size = 0;

	// Vector of properties for this type
	std::vector<TypeProperty> vProperties;

	// Vector of functions for this type
	std::vector<TypeFunction> vFunctions;

	// Only used if the type is an enum type
	std::vector<EnumConstant> vEnumConstants;
};

//-----------------------------------------
//Type
//-----------------------------------------
template <typename T>
Type& Type::Base()
{
	static_assert( (std::is_same<StripPointer<T>::Type, T>::value), "BaseClass is not a stripped type.  Did you declare a * or a &?" );

	base_type = TypeDB::GetInstance()->GetType<T>();
	return *this;
}
