#pragma once

#include "Utils.h"

struct TypeInfo;
struct IContainer;

struct Serializer
{
	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer) = 0;
	virtual void DeserializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer) = 0;

	virtual bool DeserializingContainerElements() = 0;

	//Objects
	virtual void SerializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void DeserializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	//Fundamentals
	virtual void SerializeValue(s8 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void DeserializeValue(s8& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	virtual void SerializeValue(s32 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void DeserializeValue(s32& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	virtual void SerializeValue(u32 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void DeserializeValue(u32& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	virtual void SerializeValue(float in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void DeserializeValue(float& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	virtual void SerializeValue(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void DeserializeValue(u64& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	virtual void SerializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void DeserializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
	virtual void DeserializeEnum(u64& in_obj, const string_char* name, const TypeInfo* typeInfo) = 0;
};

struct DumpSerializer : public Serializer
{
	std_string GetIndent();

	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer) override;
	virtual void DeserializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer) override;

	virtual bool DeserializingContainerElements() { return false; }

	//Objects
	virtual void SerializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	//Fundamentals
	virtual void SerializeValue(s8 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(s8& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(s32 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(s32& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(u32 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(u32& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(float in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(float& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(u64& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeEnum(u64& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	u32 indent = 0;
};

#include "XMLFile.h"
#include "XMLTag.h"

struct XMLSerializer : public DumpSerializer
{
	XMLFile m_XMLFile;
	std::vector<XMLTag*> vTagStack;
	std::vector<XMLTag*> vNextContainerElementStack;
	
	XMLSerializer()
	{
		m_XMLFile.SetRoot(nullptr);
		XMLTag* pRoot = new XMLTag;
		pRoot->SetName(L("DummyRoot"));
		vTagStack.push_back(pRoot);

		m_XMLFile.SetRoot(pRoot);
	}

	XMLTag* FindChildTag(const std_string& tagName, const std_string& name, const std_string& type);

	//Containers
	virtual void SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer) override;
	virtual void DeserializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer) override;

	virtual bool DeserializingContainerElements() { return vNextContainerElementStack.back() != nullptr; }

	//Objects
	virtual void SerializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	//Fundamentals
	virtual void SerializeValue(s8 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(s8& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(s32 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(s32& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(u32 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(u32& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(float in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(float& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(u64& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	virtual void SerializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo) override;

	//Enums
	virtual void SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo) override;
	virtual void DeserializeEnum(u64& in_obj, const string_char* name, const TypeInfo* typeInfo) override;
};

