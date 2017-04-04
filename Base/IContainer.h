#pragma once

#include "Utils.h"
#include "ErrorHandler.h"

struct IContainer;

template <typename T>
typename std::enable_if<IsStdVector<T>::val, IContainer*>::type
GetContainer()
{
	return new VectorContainer<StdVectorElement<T>::Type>();
}

template <typename T>
typename std::enable_if<IsArray<T>::val, IContainer*>::type
GetContainer()
{
	return new ArrayContainer<ArrayElement<T>::Type>(ArraySize<T>::size);
}

template <typename T>
typename std::enable_if<!IsContainer<T>::val, IContainer*>::type
GetContainer()
{
	return nullptr;
}

struct TypeInfo;

struct IContainer
{
	IContainer(const TypeInfo* in_pInfo) : m_pTypeInfo(in_pInfo) {}

	virtual size_t GetCount(void* in_pContainer) = 0;
	virtual void* GetValue(void* in_pContainer, size_t in_Index) = 0;

	virtual const string_char* GetContainerName() = 0;

	void Serialize(Serializer* in_pSerializer, void* in_pData, const string_char* in_Name);
private:
	const TypeInfo* m_pTypeInfo;
};

template <typename T>
struct VectorContainer : public IContainer
{
	VectorContainer() : IContainer(TypeInfo::Get<T>()) {}

	size_t GetCount(void* in_pContainer)
	{
		return ((std::vector<T>*)in_pContainer)->size();
	}

	void* GetValue(void* in_pContainer, size_t in_Index)
	{
		CHECK_ERROR(ErrorCode::InvalidContainerIndex, in_Index >= 0 && in_Index < GetCount(in_pContainer));

		return &((std::vector<T>*)in_pContainer)->at(in_Index);
	}

	virtual const string_char* GetContainerName() { return L("Vector"); }
};

template <typename T>
struct ArrayContainer : public IContainer
{
	ArrayContainer(size_t in_ArraySize) : IContainer(TypeInfo::Get<T>()), m_ArraySize(in_ArraySize) {}

	size_t GetCount(void*) { return m_ArraySize; }
	void* GetValue(void* in_pContainer, size_t in_Index)
	{
		CHECK_ERROR(ErrorCode::InvalidContainerIndex, in_Index >= 0 && in_Index < m_ArraySize);

		if(in_Index >= 0 && in_Index < m_ArraySize)
			return (void*)&((T*)in_pContainer)[in_Index];

		return nullptr;
	}

	virtual const string_char* GetContainerName() { return L("Array"); }

	size_t m_ArraySize;
};

