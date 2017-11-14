#pragma once

#include "Name.h"

class Function_Pointer_Base;
class AutoVariant;
struct TypeInfo;

struct TypeFunction
{
private:
	struct Function_Definition
	{
		std::vector<const TypeInfo*> m_vParamTypes;
		Function_Pointer_Base* m_FunctionPointer = nullptr;
	};

public:
	template <typename T, typename ReturnType, typename... Params>
	TypeFunction(Name name, ReturnType(T::*func)(Params...) );

	template <typename ReturnType, typename... Params>
	TypeFunction(Name name, ReturnType(*func)(Params...));

public:
	void AddDefinition(TypeFunction& in_pDefinition);

	bool IsMemberFunction() const;

	template<typename T, typename... Params>
	Function_Pointer_Base* GetFunctionPointer(T* object, Params&&... params) const;

private:
	template<typename... Params>
	bool ValidateArguments(const Function_Definition& functionDefinition, Params&&... params) const;

	template<typename T>
	bool ValidateObject(T* object) const;

	std_string GetFullFunctionName() const;

	template<int N>
	struct ParamWalk
	{
		template<typename... Params>
		static void Execute(std::vector<const TypeInfo*>& vParamTypes);
	};

public:
	Name m_Name = L("");

private:
	const TypeInfo* m_ReturnType = nullptr;
	const TypeInfo* m_ObjectType = nullptr;

	std::vector<Function_Definition> m_vFunctionDefinitions;
};

#include "Function_Prototype.h"
#include "TypeInfo.h"

template <typename T, typename ReturnType, typename... Params>
TypeFunction::TypeFunction(Name in_name, ReturnType(T::*func)(Params...) )
	: m_Name(in_name)
{
	static_assert(sizeof...(Params) <= 6, "You tried to register a method with more than 6 parameters. Unsupported.");

	m_ReturnType = TypeInfo::Get<ReturnType>();
	m_ObjectType = TypeInfo::Get<T>();

	Function_Definition newDefinition;
	newDefinition.m_vParamTypes.resize(sizeof...(Params));

	ParamWalk<sizeof...(Params)>::Execute<Params...>(newDefinition.m_vParamTypes);

	newDefinition.m_FunctionPointer = new Function_Pointer<T, ReturnType(Params...), sizeof...(Params)>(func);

	m_vFunctionDefinitions.push_back(newDefinition);
}

template <typename ReturnType, typename... Params>
TypeFunction::TypeFunction(Name in_name, ReturnType(*func)(Params...))
	: m_Name(in_name)
{
	static_assert(sizeof...(Params) <= 6, "You tried to register a method with more than 6 parameters. Unsupported.");

	m_ReturnType = TypeInfo::Get<ReturnType>();
	m_ObjectType = nullptr;

	Function_Definition newDefinition;

	newDefinition.m_vParamTypes.resize(sizeof...(Params));
	ParamWalk<sizeof...(Params)>::Execute<Params...>(newDefinition.m_vParamTypes);

	newDefinition.m_FunctionPointer = new Function_Pointer_Static<ReturnType(Params...), sizeof...(Params)>(func);

	m_vFunctionDefinitions.push_back(newDefinition);
}

template<typename T, typename... Params> 
Function_Pointer_Base* TypeFunction::GetFunctionPointer(T* in_object, Params&&... params) const
{
	if(!ValidateObject(in_object))
		return nullptr;

	for (size_t i = 0; i < m_vFunctionDefinitions.size(); i++)
	{
		if(ValidateArguments(m_vFunctionDefinitions[i], params...))
			return m_vFunctionDefinitions[i].m_FunctionPointer;
	}

	return nullptr;
}

template<typename... Params>
bool TypeFunction::ValidateArguments(const Function_Definition& functionDefinition, Params&&... params) const
{
	std::vector<VariantRef> vArgs = { params... };

	auto vParamTypes = functionDefinition.m_vParamTypes;

	if (vArgs.size() != vParamTypes.size())
	{
		//CHECK_ERROR_MSG(ErrorCode::InvalidArgumentCount, false, L("Invalid number of arguments (%d, expected %d) when calling %s"), vArgs.size(), vParamTypes.size(), GetFullFunctionName().c_str());
		return false;
	}

	for (size_t i = 0; i < vArgs.size(); i++)
	{
		if (vArgs[i].GetTypeInfo() != vParamTypes[i])
		{
			if (vArgs[i].GetTypeInfo()->IsPointer() && vParamTypes[i]->IsPointer())
			{
				if (vParamTypes[i] != TypeInfo::Get<void*>() && !vArgs[i].GetTypeInfo()->IsDerivedFrom(vParamTypes[i]))
				{
					//CHECK_ERROR_MSG(ErrorCode::InvalidArgumentPointerType, false, L("Invalid type on argument at index %d (%s, expected %s) when calling %s"), i, vArgs[i].GetTypeInfo()->m_Name.text.c_str(), vParamTypes[i]->m_Name.text.c_str(), GetFullFunctionName().c_str());
					return false;
				}
			}
			else
			{
				//CHECK_ERROR_MSG(ErrorCode::InvalidArgumentType, false, L("Invalid type on argument at index %d (%s, expected %s) when calling %s"), i, vArgs[i].GetTypeInfo()->m_Name.text.c_str(), vParamTypes[i]->m_Name.text.c_str(), GetFullFunctionName().c_str());
				return false;
			}
		}
	}

	//CHECK_ERROR_MSG(ErrorCode::InvalidArgumentCount, false, L("Found no suitable definition when calling %s"), GetFullFunctionName().c_str());
	return true;
}


template<typename T>
bool TypeFunction::ValidateObject(T*) const
{
	if(m_ObjectType == nullptr)
		return true;

	if(TypeInfo::Get<T>() == m_ObjectType)
		return true;

	if (TypeInfo::Get<T>()->IsDerivedFrom(m_ObjectType))
		return true;

	//If you're calling functions on a void*, no typecheck is performed but the code will proceed.  Dangerous.
	if (TypeInfo::Get<T>() == TypeInfo::Get<void>())
		return true;

	CHECK_ERROR_MSG(ErrorCode::InvalidInstanceForMethod, false, L("Invalid object type (%s, expected %s) when calling %s"), TypeInfo::Get<T>()->GetName(), m_ObjectType->GetName(), GetFullFunctionName().c_str());
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