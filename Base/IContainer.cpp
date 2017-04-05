#include "precompiled.h"

#include "TypeInfo.h"

void IContainer::SerializeContents(Serializer* in_pSerializer, void* in_pData, const string_char* in_Name)
{
	for (size_t i = 0; i < GetCount(in_pData); i++)
	{
		void* value = GetValue(in_pData, i);
		m_pTypeInfo->Serialize(in_pSerializer, value, in_Name);
	}
}