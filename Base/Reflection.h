#pragma once

using declare_func = void(*)();

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
		TypeDB::GetInstance()->RegisterType<T>()

#define ADD_PROP(VAR) \
			.AddProperty( TypeProperty(L(#VAR), &T::VAR) )

#define ADD_ENUM_VALUE(ENUM) \
			.AddEnumConstant( EnumConst(L(#ENUM), static_cast<int>(T::ENUM) ) )

#define BASE_CLASS(BASE) \
			.Base<BASE>()

#define END_DECLARE_TYPE(TYPE) \
		; \
	} \
	static ReflectionRegisterer<TYPE> s_ReflectionRegisterer##TYPE; 


u32 GenerateStringHash(const string_char*);
std_string GenerateTypeName(const char* in_typeName);

template <typename TYPE> struct IsEnum				{ static constexpr bool val = std::is_enum<TYPE>::value; };
template <typename TYPE> struct IsPointer			{ static constexpr bool val = false; };
template <typename TYPE> struct IsPointer<TYPE*>	{ static constexpr bool val = true; };
template <typename TYPE> struct StripPointer		{ using Type = TYPE; };
template <typename TYPE> struct StripPointer<TYPE*> { using Type = TYPE; };

template <typename TYPE> const string_char* GetTypeName();
template <typename TYPE> u32 GetTypeID();

template <typename TYPE> void ConstructObject(void* object) { new (object) TYPE; }
template <typename TYPE> void DestructObject(void* object) { ((TYPE*)object)->TYPE::~TYPE(); }

using ConstructObjectFunc = void(*)(void*);
using DestructObjectFunc = void(*)(void*);

struct Name
{
	Name() {}
	Name(const string_char* name) : hash(GenerateStringHash(name)), text(name) {}

	unsigned int hash = 0;
	const string_char* text = nullptr;

	bool operator< (const Name& in_other) const { return hash < in_other.hash; }
	bool operator== (const Name& in_other) const { return hash == in_other.hash; }
};

struct EnumConst
{
	EnumConst(Name name, int value) : name(name), value(value) { }
	Name name;
	int value = 0;
};

struct IContainer
{
	virtual size_t GetCount(void* container) = 0;
	virtual void* GetValue(void* container, int index) = 0;
};

template <typename TYPE>
struct VectorContainer : public IContainer
{
	size_t GetCount(void* container);
	void* GetValue(void* container, int index);
};

struct Type;


struct TypeFunc_Base
{
	TypeFunc_Base() {}

	virtual void Invoke_WithRet(void*, void*, ...) {};
};

template <typename OBJECT_TYPE, typename ReturnType, typename... PARAMS>
struct TypeFunc : public TypeFunc_Base
{
	/*using f2p = ReturnType(OBJECT_TYPE::*)(PARAMS...);
	TypeFunc(Name, f2p func)
	{
		m_func = func;
	}

	void Invoke(void* object, ...) { (((OBJECT_TYPE*)object)->*m_func)(); }

	f2p m_func;*/
};

template <typename OBJECT_TYPE, typename ReturnType, typename... PARAMS>
struct Helper
{
	using func_type = ReturnType(OBJECT_TYPE::*)(PARAMS...);
};

template <typename OBJECT_TYPE, typename ReturnType, typename... PARAMS>
struct Helper<OBJECT_TYPE, ReturnType(PARAMS...)>
{
	using func_type = ReturnType(OBJECT_TYPE::*)(PARAMS...);
};

template <typename OBJECT_TYPE, typename ReturnType, typename... PARAMS>
struct TypeFunc<OBJECT_TYPE, ReturnType(PARAMS...)> : public TypeFunc_Base
{
private:
	TypeFunc() = delete;
	TypeFunc(const TypeFunc<OBJECT_TYPE, ReturnType(PARAMS...)>&) = delete;

public:
	TypeFunc(Name, typename Helper<OBJECT_TYPE, ReturnType, PARAMS...>::func_type func)
	{
		m_func = func;
	}

	void Invoke_WithRet(void* object, void* ret_val, Args&&... args) 
	{
		*((ReturnType*)ret_val) = (((OBJECT_TYPE*)object)->*m_func)(std::forward<Args>(args)...);
	}

	typename Helper<OBJECT_TYPE, ReturnType, PARAMS...>::func_type m_func;
};

/*

template <typename OBJECT_TYPE, typename ReturnType, typename... PARAMS>
struct TypeFunc<OBJECT_TYPE, ReturnType(PARAMS...)> : public TypeFunc_Base
{
	TypeFunc(const std::function<ReturnType(OBJECT_TYPE*, PARAMS...)>& call_on_given_instance)
	{
		m_fct = call_on_given_instance;
	}

	ReturnType operator()(void* obj, PARAMS... params) {
		return m_fct(obj, params...);
	}

	std::function<ReturnType(OBJECT_TYPE*, PARAMS...)> m_fct;
};*/



template <typename RETURN_TYPE, typename... PARAMS>
struct Invoker;

template <typename RETURN_TYPE, typename... PARAMS>
struct Invoker<RETURN_TYPE(PARAMS...)>
{
	static RETURN_TYPE Invoke(TypeFunc_Base* pFunc, void* object, PARAMS&&... params)
	{
		RETURN_TYPE ret;
		pFunc->Invoke_WithRet(object, &ret, std::forward<PARAMS>(params)...);
		return ret;
	}
};

enum Protocol
{
	Float,
	Int32,
	Int16,
	Int8
};
using AssignFunc = void(*)(void* pDst, void* pSrc);

void AssignFloat(void* pDst, void* pSrc);

struct TypeProperty
{
	template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
	TypeProperty(Name name, PROPERTY_TYPE OBJECT_TYPE::*property);

	// Overload for std::vector container TypeProperties
	template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
	TypeProperty(Name name, std::vector<PROPERTY_TYPE> OBJECT_TYPE::*property);

	void* TypeProperty::GetPtr(void* object) { return (u8*)object + offset; }

	void Assign(Protocol, void* object, void* data) { AssignFloat(object, data); }
	void Assign(void* object, float&& data) { AssignFloat(object, &data); }

	// C++ name of the property, unscoped
	Name name = L("");

	//Type of the property
	Type* type = nullptr;

	// Is this a pointer property? Note that this becomes a flag later on...
	bool is_pointer = false;

	// Offset of this property within the type
	size_t offset = 0;

	IContainer* container = nullptr;
};


// The basic type representation
struct Type
{
	Type::Type() {}

	template <int SIZE>
	Type& AddProperties(TypeProperty(&properties)[SIZE]);
	Type& AddProperty(TypeProperty&& property);

	template <int SIZE>
	Type& AddEnumConstants(EnumConst(&enum_consts)[SIZE]);
	Type& AddEnumConstant(EnumConst&& enumConst);

	template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
	/*Type& AddFunction(Name name, PROPERTY_TYPE OBJECT_TYPE::* func) 
	{
		TypeFunc_Base* typefunc = new TypeFunc<OBJECT_TYPE, PROPERTY_TYPE>(name, func);
		
		return *this;
	};*/

	TypeFunc_Base* AddFunction(Name name, PROPERTY_TYPE OBJECT_TYPE::* func)
	{
		TypeFunc_Base* typefunc = new TypeFunc<OBJECT_TYPE, PROPERTY_TYPE>(name, func);

		return typefunc;
	};

	template <typename TYPE>
	Type& Base();

	TypeProperty* GetProperty(const string_char* in_name);
	
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

	// Only used if the type is an enum type
	bool isEnum = false;
	std::vector<EnumConst> vEnumConstants;
};

class TypeDB : public StaticSingleton<TypeDB>
{
	MAKE_STATIC_SINGLETON(TypeDB);

	TypeDB();
public:
	
	template <typename TYPE> 
	Type& RegisterType();

	template <typename TYPE> 
	Type* GetType();

	Type* GetType(Name name);

private:
	using TypeMap = std::map<Name, Type*>;
	TypeMap mTypes;
};

#include "Reflection.inl"