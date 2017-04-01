#pragma once

#include "TypeFunction.h"

template<typename... Params>
static Variant invoke_noObj(const TypeFunction* function, Params... params) //TODO fix this clash, this is shit
{
	Assert(function);
	return function->m_test->Call(params...);
}

template<typename... Params>
static Variant invoke(Variant& obj, string_char* name, Params... params)
{
	const TypeFunction* func = obj.GetTypeInfo()->type->GetFunction(name);
	Assert(func);

	void* data = nullptr;
	if (obj.GetTypeInfo()->is_pointer)
		data = *((void**)obj.GetData());
	else
		data = (void*)obj.GetData();

	return func->m_test->Call(data, params...);
}

template<typename ReturnType, typename... Params>
struct Invoker<ReturnType(Params...)>
{
	static ReturnType invoke(const TypeFunction* function, Params... params);

	template <typename T>
	static typename std::enable_if<!std::is_void<T>::value, ReturnType>::type
		invoke(T* obj, const TypeFunction* function, Params... params)
	{
		if(function)
			return function->Invoke_SafeObj<T, ReturnType, Params...>(obj, params...);
		else
			return ReturnType();
	}
	
	template <typename T>
	static typename std::enable_if<std::is_void<T>::value, ReturnType>::type
		invoke(T* obj, const TypeFunction* function, Params... params)
	{
		if (function)
			return function->Invoke_UnsafeObj<ReturnType, Params...>(obj, params...);
		else
			return ReturnType();
	}

	static ReturnType invoke(Variant& obj, string_char* name, Params... params)
	{
		const TypeFunction* func = obj.GetTypeInfo()->type->GetFunction(name);
		if (func == nullptr)
		{
			Assert(false);
			return ReturnType();
		}

		if(obj.GetTypeInfo()->is_pointer)
			return func->Invoke_UnsafeObj<ReturnType, Params...>( *((void**)obj.GetData()), params...);
		else
			return func->Invoke_UnsafeObj<ReturnType, Params...>( (void*)obj.GetData(), params...);
	}
};

template<typename ReturnType, typename... Params>
ReturnType Invoker<ReturnType(Params...)>::invoke(const TypeFunction* function, Params... params)
{
	if (function)
		return function->Invoke_NoObj<ReturnType, Params...>(params...);
	else
		return ReturnType();
}