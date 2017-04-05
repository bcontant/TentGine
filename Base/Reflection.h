#pragma once

#include "TypeDB.h"
#include "InstanceTypeInfo.h"
#include "Variant.h"

template <class T>
class ReflectionRegisterer
{
public:
	ReflectionRegisterer::ReflectionRegisterer() {}
};

#define REFLECTABLE(TYPE) friend class ReflectionRegisterer<TYPE>;

#define DECLARE_TYPE(TYPE) \
	template <> \
	ReflectionRegisterer<TYPE>::ReflectionRegisterer() \
	{ \
		using T = TYPE; \
		TypeInfo::Get<T>(); \
		Type* pType = &TypeDB::GetInstance()->RegisterType<T>();  pType ? (void)0 : (void)0;

#define ADD_PROP(VAR) \
		pType->AddProperty( TypeProperty(L(#VAR), &T::VAR) );

#define ADD_FUNC(FUNC) \
		pType->AddFunction( TypeFunction( Name(L(#FUNC)), &T::FUNC) );

#define ADD_ENUM_VALUE(ENUM) \
		pType->AddEnumConstant( EnumConstant(L(#ENUM), static_cast<int>(T::ENUM) ) );

#define BASE_CLASS(BASE) \
		pType->Base<BASE>();

#define SET_DEFAULT_CTOR_ARGS(...) \
		struct DefaultConstructor \
		{ \
			static void ConstructObject(void* object) { new (object) T(##__VA_ARGS__); } \
		}; \
		pType->SetDefaultConstructor(DefaultConstructor::ConstructObject); \

#define END_DECLARE(TYPE) \
	} \
	static ReflectionRegisterer<TYPE> s_ReflectionRegisterer##TYPE; 

// INSPIRATION GOES HERE

// PONDER
/*static void declare()
{
	// Declare the class members to Ponder
	ponder::Class::declare<Person>("Person")
		.constructor<std::string>()
		.property("name", &Person::name)
		.property("age", &Person::age, &Person::setAge)
		.function("dump", &Person::dump)
		;
}

// An example of how you might use Ponder:
static void use()
{
	// Retrieve the metaclass (containing the member data)
	const ponder::Class& metaclass = ponder::classByType<Person>();

	// Use the metaclass to construct a new person named John
	ponder::runtime::ObjectFactory factory(metaclass);
	ponder::UserObject john = factory.create("John");

	// Set John's age
	john.set("age", 97);

	// Dump John's info
	ponder::runtime::call(metaclass.function("dump"), john);

	// Kill John (not really)
	factory.destroy(john);
}*/