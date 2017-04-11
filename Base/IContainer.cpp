#include "precompiled.h"

#include "TypeInfo.h"

void IContainer::SerializeContents(Serializer* in_pSerializer, void* in_pContainerPtr)
{
	for (size_t i = 0; i < GetCount(in_pContainerPtr); i++)
	{
		void* value = GetValue(in_pContainerPtr, i);
		m_pTypeInfo->Serialize(in_pSerializer, value, L(""));
	}
}

void IContainer::DeserializeContents(Serializer* in_pSerializer, void* in_pContainerPtr, const TypeInfo* in_pElementsType)
{
	const TypeInfo* pActualType = m_pTypeInfo;

	if (in_pElementsType)
	{
		Assert(in_pElementsType->IsDerivedFrom(m_pTypeInfo));
		pActualType = in_pElementsType;
	}

	size_t index = 0;
	while(in_pSerializer->DeserializingContainerElements())
	{
		//TODO : Polymorphism
		void* tmpElement = nullptr;
		//tmpElement = m_pTypeInfo->New();
		tmpElement = pActualType->New();

		//if(m_pTypeInfo->IsPointer())
		if(pActualType->IsPointer())
			*(void**)tmpElement = nullptr;

		//m_pTypeInfo->Deserialize(in_pSerializer, tmpElement, L(""));
		pActualType->Deserialize(in_pSerializer, tmpElement, L(""));
		InsertValue(in_pContainerPtr, tmpElement, index++);

		//m_pTypeInfo->Delete(tmpElement);
		pActualType->Delete(tmpElement);
	}
}
