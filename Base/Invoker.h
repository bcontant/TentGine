#pragma once

#include "TypeFunction.h"

template<typename ReturnType, typename... Params>
struct Invoker<ReturnType(Params...)>
{
	static ReturnType invoke(TypeFunction* function, Params... params);

	template <typename ObjectType>
	static typename std::enable_if<!std::is_void<ObjectType>::value, ReturnType>::type
		invoke(ObjectType* obj, TypeFunction* function, Params... params)
	{
		return function->Invoke_SafeObj<ObjectType, ReturnType, Params...>(obj, params...);
	}


	template <typename ObjectType>
	static typename std::enable_if<std::is_void<ObjectType>::value, ReturnType>::type
		invoke(ObjectType* obj, TypeFunction* function, Params... params)
	{
		return function->Invoke_UnsafeObj<ReturnType, Params...>(obj, params...);
	}
};

template<typename ReturnType, typename... Params>
ReturnType Invoker<ReturnType(Params...)>::invoke(TypeFunction* function, Params... params)
{
	return function->Invoke_NoObj<ReturnType, Params...>(params...);
}