#pragma once

#include "Utils.h"

struct InstanceTypeInfo;
struct Serializer;
struct IContainer;

using SerializeObjectFunc = void(*)(Serializer*, const InstanceTypeInfo*, void*, const string_char* name);

template <typename T>
struct SerializeVisitor
{
	static void SerializePointer(Serializer* s, const InstanceTypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		//TODO : Pointer of fundamentals?  Even a pointer on an object, could be an array.
		// + Flags on typeinfo?  I'd need to remove the nice staticeness of the InstanceTypeInfos
		// Flags : eArray w/ size on another member?
		//         eNullTerminated (for c strings)

		static_assert(IsPointer<T>::val == true, "Something is wrong.");
		static_assert(IsFundamental<T>::val == false, "Something is wrong.");

		s->Serialize(*((void**)in_obj), name, typeInfo);
	}

	static void SerializeObject(Serializer* s, const InstanceTypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");

		s->Serialize(in_obj, name, typeInfo);
	}

	static void SerializeFundamental(Serializer* s, const InstanceTypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");

		s->Serialize(*((T*)in_obj), name, typeInfo);
	}

	static void SerializeEnum(Serializer* s, const InstanceTypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");
		static_assert(sizeof(T) <= 8, "Something is wrong.");

		u64 enumVal = 0;
		memcpy(&enumVal, in_obj, typeInfo->size);
		s->SerializeEnum(enumVal, name, typeInfo);
	}

	static void SerializeContainer(Serializer* s, const InstanceTypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");
		AssertMsg(typeInfo->container != nullptr, L("Something is wrong."));

		s->SerializeContainer(in_obj, name, typeInfo, typeInfo->container);
	}
};


template <typename T>
typename std::enable_if<SerializeAsPointer<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializePointer;
}

template <typename T>
typename std::enable_if<SerializeAsObject<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeObject;
}

template <typename T>
typename std::enable_if<SerializeAsFundamental<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeFundamental;
}

template <typename T>
typename std::enable_if<SerializeAsEnum<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeEnum;
}

template <typename T>
typename std::enable_if<SerializeAsContainer<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeContainer;
}

struct Serializer
{
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo, IContainer* container) = 0;

	//Objects
	virtual void Serialize(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo) = 0;

	//Fundamentals
	virtual void Serialize(s8 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo) = 0;
	virtual void Serialize(s32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo) = 0;
	virtual void Serialize(u32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo) = 0;
	virtual void Serialize(float in_obj, const string_char* name, const InstanceTypeInfo* typeInfo) = 0;
	virtual void Serialize(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo) = 0;
	virtual void Serialize(std_string& in_obj, const string_char* name, const InstanceTypeInfo* typeInfo) = 0;

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo) = 0;
};

struct DumpSerializer : public Serializer
{
	std_string GetIndent();
		
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo, IContainer* container);

	//Objects
	virtual void Serialize(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);

	//Fundamentals
	virtual void Serialize(s8 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(s32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(u32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(float in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(std_string& in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);


	u32 indent = 0;
};

struct XMLSerializer : public DumpSerializer
{
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo, IContainer* container);

	//Objects
	virtual void Serialize(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);

	//Fundamentals
	virtual void Serialize(s8 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(s32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(u32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(float in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
	virtual void Serialize(std_string& in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo);
};

