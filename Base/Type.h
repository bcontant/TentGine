#pragma once

#include "Name.h"
#include "EnumConstant.h"
#include "TypeProperty.h"
#include "TypeFunction.h"
#include "TypeOperators.h"
#include "Utils.h"

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
	bool IsDerivedFrom(const TypeInfo* in_TypeInfo);

	void SetCtor(ConstructObjectFunc f) { m_Operators.constructor = f; }

	const TypeProperty* GetProperty(const string_char* in_name) const;
	const TypeFunction* GetFunction(const string_char* in_name) const;
	const EnumConstant* GetEnumConstant(u64 in_value) const;

	// Scoped C++ name of the type
	Name m_Name = L("");

	//InstanceTypeInfo for this type
	//const TypeInfo* typeInfo = nullptr;

	//Base type
	Type* base_type = nullptr;
	
	// Pointers to the constructor and destructor functions
	TypeOperators m_Operators = { 0 };

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
	static_assert( (std::is_same<Strip<T>::Type, T>::value), "BaseClass is not a stripped type.  Did you declare a * or a &?" );

	base_type = TypeDB::GetInstance()->GetType<T>();
	return *this;
}
