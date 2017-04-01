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
	struct FunctionsContainer
	{
		std::shared_ptr<Function_Prototype_Base> no_instance;
		std::shared_ptr<Function_Prototype_Base> safe_instance;
		std::shared_ptr<Function_Prototype_Base> unsafe_instance;
	};

	template <typename T, typename ReturnType, typename... Params>
	TypeFunction(const std::pair<Name, ReturnType(T::*)(Params...)>& func);

//private:
	template<typename ReturnType, typename... Params>
	ReturnType Invoke_NoObj(Params... params) const;

	template<typename T, typename ReturnType, typename... Params>
	ReturnType Invoke_SafeObj(T* obj, Params... params) const;

	template<typename ReturnType, typename... Params>
	ReturnType Invoke_UnsafeObj(void* obj, Params... params) const;

public:
	Name name = L("");
	FunctionsContainer m_functions;

	Function_Prototype_Base_NoStd* m_test;
};

template <typename T, typename ReturnType, typename... Params>
TypeFunction::TypeFunction(const std::pair<Name, ReturnType(T::*)(Params...)>& func)
{
	Type* type = TypeDB::GetInstance()->GetType<T>();

	auto f_without_instance = [func, type](Params... params) -> ReturnType {
		return (((T*)type->default_obj())->*func.second)(std::ref(params)...);
	};

	auto f_with_instance = [func](T* obj, Params... params) -> ReturnType {
		return std::bind(func.second, obj, std::ref(params)...)();
	};

	auto f_with_unsafe_instance = [func](void* obj, Params... params) -> ReturnType {
		return std::bind(func.second, (T*)obj, std::ref(params)...)();
	};

	if (type->default_obj)
		m_functions.no_instance = std::shared_ptr<Function_Prototype_Base>(new Function_Prototype_Without_Object<ReturnType(Params...)>(f_without_instance));
	else
		m_functions.no_instance = nullptr;

	m_functions.safe_instance = std::shared_ptr<Function_Prototype_Base>(new Function_Prototype_With_Object<T, ReturnType(Params...)>(f_with_instance));
	m_functions.unsafe_instance = std::shared_ptr<Function_Prototype_Base>(new Function_Prototype_With_Object<void, ReturnType(Params...)>(f_with_unsafe_instance));

	m_test = new Function_Prototype_NoStd<T, ReturnType(Params...), sizeof...(Params)>(func.second);

	name = func.first;
}


template<typename ReturnType, typename... Params>
ReturnType TypeFunction::Invoke_NoObj(Params... params) const
{
	auto f = dynamic_cast<Function_Prototype_Without_Object<ReturnType(Params...)>*>(m_functions.no_instance.get());
	if (f != nullptr)
		return (*f)(params...);

	return ReturnType();
}

template<typename T, typename ReturnType, typename... Params>
ReturnType TypeFunction::Invoke_SafeObj(T* obj, Params... params) const
{
	auto f = dynamic_cast<Function_Prototype_With_Object<T, ReturnType(Params...)>*>(m_functions.safe_instance.get());
	if (f != nullptr)
		return (*f)(obj, params...);

	return ReturnType();
}

template<typename ReturnType, typename... Params>
ReturnType TypeFunction::Invoke_UnsafeObj(void* obj, Params... params) const
{
	auto f = dynamic_cast<Function_Prototype_With_Object<void, ReturnType(Params...)>*>(m_functions.unsafe_instance.get());
	if (f != nullptr)
		return (*f)(obj, params...);

	return ReturnType();
}
