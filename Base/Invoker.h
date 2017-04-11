#pragma once

#include "Variant.h"
#include "TypeFunction.h"

template<typename T, typename... Params>
AutoVariant invoke(T& object, const TypeFunction* f, Params&&... params)
{
	return invoke(&object, f, params...);
}

template<typename T, typename... Params>
AutoVariant invoke(T* object, const TypeFunction* f, Params&&... params)
{
	if (f == nullptr)
		return AutoVariant(0);

	Function_Pointer_Base* pFunc = f->GetFunctionPointer(object, params...);

	if(pFunc)
		return pFunc->Call(object, params...);

	return AutoVariant(0);
}

template<typename T, typename... Params>
AutoVariant invoke(T& object, const string_char* in_FunctionName, Params&&... params)
{
	return invoke(&object, in_FunctionName, params...);
}

template<typename T, typename... Params>
AutoVariant invoke(T* object, const string_char* in_FunctionName, Params&&... params)
{
	const TypeInfo* pTypeInfo = TypeInfo::Get<T>();

	//TODO : Find best fit for params
	const TypeFunction* f = pTypeInfo->GetFunction(in_FunctionName);

	return invoke(object, f, params...);
}


template<typename... Params>
AutoVariant invoke_global(const string_char* in_FunctionName, Params&&... params)
{
	return invoke<void>(nullptr, in_FunctionName, params...);
}
