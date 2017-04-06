#include "precompiled.h"

#include "TypeFunction.h"

std_string TypeFunction::GetFullFunctionName() const
{
	if (m_ObjectType)
		return std_string(m_ObjectType->m_Name.text) + L("::") + m_Name.text;
	else
		return m_Name.text;
}