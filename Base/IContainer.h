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
typename std::enable_if<IsStaticArray<T>::val, IContainer*>::type
GetContainer()
{
	return new StaticArrayContainer<ArrayElement<T>::Type>(ArraySize<T>::size);
}

template <typename T>
typename std::enable_if<IsDynamicArray<T>::val, IContainer*>::type
GetContainer()
{
	return new DynamicArrayContainer<StripPointer_Once<T>::Type>();
}


template <typename T>
typename std::enable_if<!IsContainer<T>::val, IContainer*>::type
GetContainer()
{
	return nullptr;
}

struct TypeInfo;
struct Serializer;

struct IContainer
{
	IContainer(const TypeInfo* in_pInfo) : m_pTypeInfo(in_pInfo) {}

	virtual void Clear(void* in_pContainer) = 0;

	virtual void InsertValue(void* in_pContainer, void* in_pValue, size_t in_index = 0) = 0;
		
	virtual size_t GetCount(void* in_pContainer) = 0;
	virtual void* GetValue(void* in_pContainer, size_t in_Index) = 0;
	
	virtual const string_char* GetContainerName() = 0;

	virtual void SerializeContents(Serializer* in_pSerializer, void* in_pData);
	virtual void DeserializeContents(Serializer* in_pSerializer, void* in_pData, const TypeInfo* in_pElementsType);

protected:
	const TypeInfo* m_pTypeInfo;
};

template <typename T>
struct VectorContainer : public IContainer
{
	VectorContainer() : IContainer(TypeInfo::Get<T>()) {}

	virtual void Clear(void* in_pContainer)
	{
		((std::vector<T>*)in_pContainer)->clear();
	}

	virtual void InsertValue(void* in_pContainer, void* in_pValue, size_t)
	{
		((std::vector<T>*)in_pContainer)->push_back(*((T*)in_pValue));
	}

	size_t GetCount(void* in_pContainer)
	{
		return ((std::vector<T>*)in_pContainer)->size();
	}

	void* GetValue(void* in_pContainer, size_t in_Index)
	{
		CHECK_ERROR_MSG(ErrorCode::InvalidContainerIndex, in_Index >= 0 && in_Index < GetCount(in_pContainer), L("Index out of bounds on VectorContainer"));

		return &((std::vector<T>*)in_pContainer)->at(in_Index);
	}

	virtual const string_char* GetContainerName() { return L("Vector"); }
};

template <typename T>
struct StaticArrayContainer : public IContainer
{
	StaticArrayContainer(size_t in_ArraySize) : IContainer(TypeInfo::Get<T>()), m_ArraySize(in_ArraySize) {}

	virtual void Clear(void* in_pContainer)
	{
		memset(in_pContainer, 0, GetCount(in_pContainer) * sizeof(T));
	}

	virtual void InsertValue(void* in_pContainer, void* in_pValue, size_t in_Index)
	{
		((T*)in_pContainer)[in_Index] = *((T*)in_pValue);
	}

	size_t GetCount(void*) { return m_ArraySize; }
	void* GetValue(void* in_pContainer, size_t in_Index)
	{
		CHECK_ERROR_MSG(ErrorCode::InvalidContainerIndex, in_Index >= 0 && in_Index < m_ArraySize, L("Index out of bounds on StaticArrayContainer"));

		if(in_Index >= 0 && in_Index < m_ArraySize)
			return (void*)&((T*)in_pContainer)[in_Index];

		return nullptr;
	}

	virtual const string_char* GetContainerName() { return L("StaticArray"); }

	size_t m_ArraySize;
};

template <typename T>
struct DynamicArrayContainer : public IContainer
{
	DynamicArrayContainer() : IContainer(TypeInfo::Get<T>()) {}

	virtual void Clear(void* in_pContainer)
	{
		Assert(in_pContainer);
		Assert(GetCount(in_pContainer) == 1);

		if(GetCount(in_pContainer) == 1)
			m_pTypeInfo->Delete( *((T**)in_pContainer) );

		*((T**)in_pContainer) = nullptr;
	}

	virtual void InsertValue(void* in_pContainer, void* in_pValue, size_t)
	{	
		Assert(GetCount(in_pContainer) == 1);

		if(*((T**)in_pContainer) == nullptr)
			*((T**)in_pContainer) = (T*) m_pTypeInfo->New();

		m_pTypeInfo->AssignmentOperator( (*((T**)in_pContainer)), ((T*)in_pValue) );
	}

	//TODO : Implement for real, with flags and all
	size_t GetCount(void*) { return 1; }
	void* GetValue(void* in_pContainer, size_t in_Index)
	{
		CHECK_ERROR_MSG(ErrorCode::InvalidContainerIndex, in_Index >= 0 && in_Index < 1, L("Index out of bounds on DynamicArrayContainer"));
		
		T* pDynamicArray = *((T**)in_pContainer);
		if (in_Index >= 0 && in_Index < 1 && pDynamicArray)
			return (void*)&(pDynamicArray[in_Index]);

		return nullptr;
	}

	virtual const string_char* GetContainerName() { return L("DynamicArray"); }

	virtual void DeserializeContents(Serializer* in_pSerializer, void* in_pContainer, const TypeInfo* in_pElementsType)
	{
		{
			const TypeInfo* pActualType = m_pTypeInfo;

			if (in_pElementsType)
			{
				Assert(in_pElementsType->IsDerivedFrom(m_pTypeInfo));
				pActualType = in_pElementsType;
			}

			//size_t index = 0;
			//while (in_pSerializer->DeserializingContainerElements())
			{
				*((T**)in_pContainer) = (T*) pActualType->New();

				if (pActualType->IsPointer())
					**(void***)in_pContainer = nullptr;

				pActualType->Deserialize(in_pSerializer, *(void**)in_pContainer, L(""));
			}
		}
	}
};

