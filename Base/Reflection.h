#pragma once

#include "TypeDB.h"

#include "InstanceTypeInfo.h"
#include "Variant.h"
#include "Invoker.h"

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
		Type* pType = &TypeDB::GetInstance()->RegisterType<T>();

#define ADD_PROP(VAR) \
		pType->AddProperty( TypeProperty(L(#VAR), &T::VAR) );

#define ADD_FUNC(FUNC) \
		pType->AddFunction( TypeFunction( std::make_pair( Name(L(#FUNC)), &T::FUNC) ) );

#define ADD_ENUM_VALUE(ENUM) \
		pType->AddEnumConstant( EnumConstant(L(#ENUM), static_cast<int>(T::ENUM) ) );

#define BASE_CLASS(BASE) \
		pType->Base<BASE>();

#define SET_DEFAULT_CTOR_ARGS(...) \
		struct DefaultConstructor \
		{ \
			static void ConstructObject(void* object) { new (object) T(##__VA_ARGS__); } \
			static void* DefaultObject() { static T obj(##__VA_ARGS__);  obj = T(##__VA_ARGS__);  return &obj; } \
		}; \
		pType->SetCtor(DefaultConstructor::ConstructObject); \
		if(IsDestructible<T>::val) \
			pType->SetDefaultObject(DefaultConstructor::DefaultObject);

#define END_DECLARE(TYPE) \
	} \
	static ReflectionRegisterer<TYPE> s_ReflectionRegisterer##TYPE; 

/*
template <typename TYPE> u32 GetTypeID()
{
	static int type_id = GenerateStringHash(GetTypeName<TYPE>());
	return type_id;
}*/


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