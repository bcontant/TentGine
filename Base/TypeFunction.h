#pragma once

#include "Name.h"
#include "InstanceTypeInfo.h"
#include "Variant.h"

class Function_Pointer_Base;
struct TypeInfo;

struct TypeFunction
{
public:
	template <typename T, typename ReturnType, typename... Params>
	TypeFunction(Name name, ReturnType(T::*func)(Params...) );

	template<typename T, typename... Params>
	AutoVariant invoke(T& object, Params&&... params) const;

	template<typename T, typename... Params>
	AutoVariant invoke(T* object, Params&&... params) const;

private:
	template<typename... Params>
	bool ValidateArguments(Params&&... params) const;
	template<typename T>
	bool ValidateObject(T* object) const;

	template<int N>
	struct ParamWalk
	{
		template<typename... Params>
		static void Execute(std::vector<const TypeInfo*>& vParamTypes);
	};

public:
	Name m_Name = L("");

	const TypeInfo* m_ReturnType = nullptr;
	const TypeInfo* m_ObjectType = nullptr;
	std::vector<const TypeInfo*> m_vParamTypes;

	Function_Pointer_Base* m_FunctionPointer = nullptr;
};

#include "Function_Prototype.h"
#include "InstanceTypeInfo.h"

template <typename T, typename ReturnType, typename... Params>
TypeFunction::TypeFunction(Name in_name, ReturnType(T::*func)(Params...) )
	: m_Name(in_name)
{
	m_ReturnType = TypeInfo::Get<ReturnType>();
	m_ObjectType = TypeInfo::Get<T>();
	
	m_vParamTypes.resize(sizeof...(Params));
	ParamWalk<sizeof...(Params)>::Execute<Params...>(m_vParamTypes);

	static_assert(sizeof...(Params) <= 6, "You tried to register a method with more than 6 parameters   Unsupported.");
	m_FunctionPointer = new Function_Pointer<T, ReturnType(Params...), sizeof...(Params)>(func);
}

template<typename T, typename... Params>
AutoVariant TypeFunction::invoke(T& object, Params&&... params) const
{
	return invoke(&object, params...);
}

template<typename T, typename... Params>
AutoVariant TypeFunction::invoke(T* object, Params&&... params) const
{
	ValidateArguments(params...);
	ValidateObject(object);

	return m_FunctionPointer->Call(object, params...);
}

template<typename... Params>
bool TypeFunction::ValidateArguments(Params&&... params) const 
{
	std::vector<VariantRef> vArgs = { params... };

	if (vArgs.size() != m_vParamTypes.size())
	{
		CHECK_ERROR_MSG(ErrorCode::InvalidArgumentCount, false, L("Invalid number of arguments (%d, expected %d) when calling %s::%s"), vArgs.size(), m_vParamTypes.size(), m_ObjectType->m_MetaInfo->m_Name.text, m_Name.text);
		return false;
	}

	for (size_t i = 0; i < vArgs.size(); i++)
	{
		if ( vArgs[i].GetTypeInfo() != m_vParamTypes[i] )
		{
			if (vArgs[i].GetTypeInfo()->m_bIsPointer && m_vParamTypes[i]->m_bIsPointer)
			{
				if (m_vParamTypes[i] == TypeInfo::Get<void*>())
				{	
				}
				else if (vArgs[i].GetTypeInfo()->IsDerivedFrom(m_vParamTypes[i]))
				{	
				}
				else
				{
					CHECK_ERROR_MSG(ErrorCode::InvalidArgumentPointerType, false, L("Invalid type on argument at index %d (%s, expected %s) when calling %s::%s"), i, vArgs[i].GetTypeInfo()->m_Name.text, m_vParamTypes[i]->m_Name.text, m_ObjectType->m_MetaInfo->m_Name.text, m_Name.text);
				}
			}
			else
			{
				CHECK_ERROR_MSG(ErrorCode::InvalidArgumentType, false, L("Invalid type on argument at index %d (%s, expected %s) when calling %s::%s"), i, vArgs[i].GetTypeInfo()->m_Name.text, m_vParamTypes[i]->m_Name.text, m_ObjectType->m_MetaInfo->m_Name.text, m_Name.text);
				return false;
			}
		}
	}

	return true;
}

template<typename T>
bool TypeFunction::ValidateObject(T*) const
{
	if(TypeInfo::Get<T>() == m_ObjectType)
		return true;

	if (TypeInfo::Get<T>()->IsDerivedFrom(m_ObjectType))
		return true;

	CHECK_ERROR_MSG(ErrorCode::InvalidInstanceForMethod, false, L("Invalid object type (%s, expected %s) when calling %s::%s"), TypeInfo::Get<T>()->m_Name.text, m_ObjectType->m_Name.text, m_ObjectType->m_MetaInfo->m_Name.text, m_Name.text);
	return false;
}


template<int N> template<typename... Params>
void TypeFunction::ParamWalk<N>::Execute(std::vector<const TypeInfo*>& vParamTypes)
{
	vParamTypes[N - 1] = TypeInfo::Get< get_argument_type<Params...>::Type<(N - 1)> >();
	ParamWalk<N - 1>::Execute<Params...>(vParamTypes);
}

template<> template<typename... Params>
void TypeFunction::ParamWalk<1>::Execute(std::vector<const TypeInfo*>& vParamTypes)
{
	vParamTypes[0] = TypeInfo::Get< get_argument_type<Params...>::Type<0> >();
}

template<> template<typename... Params>
void TypeFunction::ParamWalk<0>::Execute(std::vector<const TypeInfo*>&)
{
}