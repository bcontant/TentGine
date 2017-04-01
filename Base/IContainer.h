#pragma once

#include "Utils.h"

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

struct InstanceTypeInfo;

struct IContainer
{
	IContainer(const InstanceTypeInfo* info) : typeInfo(info) {}

	virtual size_t GetCount(void* container) = 0;
	virtual void* GetValue(void* container, size_t index) = 0;

	virtual const string_char* GetContainerName() = 0;

	const InstanceTypeInfo* typeInfo;
};

template <typename T>
struct VectorContainer : public IContainer
{
	VectorContainer() : IContainer(InstanceTypeInfo::Get<T>()) {}

	size_t GetCount(void* container)
	{
		return ((std::vector<T>*)container)->size();
	}

	void* GetValue(void* container, size_t index)
	{
		return &((std::vector<T>*)container)->at(index);
	}

	virtual const string_char* GetContainerName() { return L("Vector"); }
};

template <typename T>
struct ArrayContainer : public IContainer
{
	ArrayContainer(size_t arraySize) : IContainer(InstanceTypeInfo::Get<T>()), m_ArraySize(arraySize) {}

	size_t GetCount(void*) { return m_ArraySize; }
	void* GetValue(void* container, size_t index)
	{
		Assert(index >= 0 && index < m_ArraySize);
		return (void*)&((T*)container)[index];
	}

	virtual const string_char* GetContainerName() { return L("Array"); }

	size_t m_ArraySize;
};

