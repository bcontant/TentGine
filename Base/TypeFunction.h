#pragma once

#include "Function_Prototype.h"
#include "Name.h"

template<typename ReturnType, typename... Params>
struct Invoker;

struct TypeFunction
{
	template<typename ReturnType, typename... Params>
	friend struct Invoker;

public:
	//TODO : Proper way to delete these (just use smart pointers and forget about it?)
	struct FunctionsContainer
	{
		Function_Prototype_Base* no_instance;
		Function_Prototype_Base* safe_instance;
		Function_Prototype_Base* unsafe_instance;
	};

	template <typename TYPE, typename ReturnType, typename... Params>
	TypeFunction(const std::pair<Name, ReturnType(TYPE::*)(Params...)>& func);

private:
	template<typename ReturnType, typename... Params>
	ReturnType Invoke_NoObj(Params... params);

	template<typename ClassType, typename ReturnType, typename... Params>
	ReturnType Invoke_SafeObj(ClassType* obj, Params... params);

	template<typename ReturnType, typename... Params>
	ReturnType Invoke_UnsafeObj(void* obj, Params... params);

public:
	Name name = L("");
	FunctionsContainer m_functions;
};

template <typename TYPE, typename ReturnType, typename... Params>
TypeFunction::TypeFunction(const std::pair<Name, ReturnType(TYPE::*)(Params...)>& func)
{
	auto f_without_instance = [func](Params... params) -> ReturnType {
		return (TYPE().*func.second)(std::ref(params)...);
	};

	auto f_with_instance = [func](TYPE* obj, Params... params) -> ReturnType {
		return std::bind(func.second, obj, std::ref(params)...)();
	};

	auto f_with_unsafe_instance = [func](void* obj, Params... params) -> ReturnType {
		return std::bind(func.second, (TYPE*)obj, std::ref(params)...)();
	};

	m_functions.no_instance = new Function_Prototype_Without_Object<ReturnType(Params...)>(f_without_instance);
	m_functions.safe_instance = new Function_Prototype_With_Object<TYPE, ReturnType(Params...)>(f_with_instance);
	m_functions.unsafe_instance = new Function_Prototype_With_Object<void, ReturnType(Params...)>(f_with_unsafe_instance);

	name = func.first;
}


template<typename ReturnType, typename... Params>
ReturnType TypeFunction::Invoke_NoObj(Params... params)
{
	auto f = dynamic_cast<Function_Prototype_Without_Object<ReturnType(Params...)>*>(m_functions.no_instance);
	if (f != nullptr)
		return (*f)(params...);

	return ReturnType();
}

template<typename ClassType, typename ReturnType, typename... Params>
ReturnType TypeFunction::Invoke_SafeObj(ClassType* obj, Params... params)
{
	auto f = dynamic_cast<Function_Prototype_With_Object<ClassType, ReturnType(Params...)>*>(m_functions.safe_instance);
	if (f != nullptr)
		return (*f)(obj, params...);

	return ReturnType();
}

template<typename ReturnType, typename... Params>
ReturnType TypeFunction::Invoke_UnsafeObj(void* obj, Params... params)
{
	auto f = dynamic_cast<Function_Prototype_With_Object<void, ReturnType(Params...)>*>(m_functions.unsafe_instance);
	if (f != nullptr)
		return (*f)(obj, params...);

	return ReturnType();
}
