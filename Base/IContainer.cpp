#include "precompiled.h"

#include "InstanceTypeInfo.h"

void IContainer::Serialize(Serializer* in_pSerializer, void* in_pData, const string_char* in_Name)
{
	//m_pTypeInfo->Serialize(in_pSerializer, in_pData, in_Name);
	for (size_t i = 0; i < GetCount(in_pData); i++)
	{
		void* value = GetValue(in_pData, i);
		m_pTypeInfo->Serialize(in_pSerializer, value, in_Name);
	}
}