#include "precompiled.h"

#include "TypeFunction.h"

std_string TypeFunction::GetFullFunctionName() const
{
	//TODO : Static methods should still have their Object:: in their name
	if (m_ObjectType)
		return std_string(m_ObjectType->m_Name.text) + "::" + m_Name.text;
	else
		return m_Name.text;
}