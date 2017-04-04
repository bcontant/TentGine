#pragma once

#include "Utils.h"

struct TypeInfo;
struct IContainer;

struct Serializer
{
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer) = 0;

	//Objects
	virtual void Serialize(void* in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	//Fundamentals
	virtual void Serialize(s8 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void Serialize(s32 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void Serialize(u32 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void Serialize(float in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void Serialize(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void Serialize(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
};

struct DumpSerializer : public Serializer
{
	std_string GetIndent();
		
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer);

	//Objects
	virtual void Serialize(void* in_obj, const string_char* name, const TypeInfo* typeInfo);

	//Fundamentals
	virtual void Serialize(s8 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(s32 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(u32 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(float in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(u64 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo);

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo);

	u32 indent = 0;
};

struct XMLSerializer : public DumpSerializer
{
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer);

	//Objects
	virtual void Serialize(void* in_obj, const string_char* name, const TypeInfo* typeInfo);

	//Fundamentals
	virtual void Serialize(s8 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(s32 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(u32 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(float in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(u64 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void Serialize(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo);

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo);
};

