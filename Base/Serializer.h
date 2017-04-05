#pragma once

#include "Utils.h"

struct TypeInfo;
struct IContainer;

struct Serializer
{
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer) = 0;

	//Objects
	virtual void SerializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	//Fundamentals
	virtual void SerializeValue(s8 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void SerializeValue(s32 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void SerializeValue(u32 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void SerializeValue(float in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void SerializeValue(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void SerializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
};

struct DumpSerializer : public Serializer
{
	std_string GetIndent();
		
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer);

	//Objects
	virtual void SerializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo);

	//Fundamentals
	virtual void SerializeValue(s8 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(s32 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(u32 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(float in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(u64 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo);

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo);

	u32 indent = 0;
};

struct XMLSerializer : public DumpSerializer
{
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer);

	//Objects
	virtual void SerializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo);

	//Fundamentals
	virtual void SerializeValue(s8 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(s32 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(u32 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(float in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(u64 in_obj, const string_char* name, const TypeInfo* typeInfo);
	virtual void SerializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo);

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo);
};

