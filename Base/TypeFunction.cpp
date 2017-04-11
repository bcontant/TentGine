#include "precompiled.h"

#include "TypeFunction.h"

std_string TypeFunction::GetFullFunctionName() const
{
	if (m_ObjectType)
		return std_string(m_ObjectType->GetName()) + L("::") + std_string(m_Name);
	else
		return m_Name;
}

void TypeFunction::AddDefinition(TypeFunction& in_pNewDefinition)
{
	Assert(in_pNewDefinition.m_Name == m_Name);
	Assert(in_pNewDefinition.m_ReturnType == m_ReturnType);
	Assert(in_pNewDefinition.m_ObjectType == m_ObjectType);

	m_vFunctionDefinitions.push_back(in_pNewDefinition.m_vFunctionDefinitions[0]);
}

bool TypeFunction::IsMemberFunction() const
{
	//TODO : Can overloads be static and non-static? Yes they can, mother fucker
	return !m_vFunctionDefinitions[0].m_FunctionPointer->IsStatic();
}