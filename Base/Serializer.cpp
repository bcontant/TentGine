#include "precompiled.h"

#include "Serializer.h"
#include "InstanceTypeInfo.h"
#include "Type.h"

#include "Logger.h"

std_string DumpSerializer::GetIndent()
{
	std_string s;
	for (u32 i = 0; i < indent; i++)
		s += L(" ");
	return s;
}

void DumpSerializer::SerializeContainer(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo, IContainer* container)
{
	DEBUG_LOG(L("%sSerialize(void*) serializing a container (name = %s) (type = %s) [%s[%d]] at 0x%08x"), GetIndent().c_str(), name, typeInfo->name.text, container->GetContainerName(), container->GetCount(in_obj), container);

	indent += 2;

	for (size_t i = 0; i < container->GetCount(in_obj); i++)
	{
		void* value = container->GetValue(in_obj, i);
		container->typeInfo->serialize_func(this, container->typeInfo, value, L(""));
	}

	indent -= 2;
}

void DumpSerializer::Serialize(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(void*) serializing a pointer (name = %s) (type = %s) at 0x%08x"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);

	if (in_obj != nullptr)
	{
		indent += 2;

		for (auto prop : typeInfo->type->vProperties)
		{
			prop.type_info->serialize_func(this, prop.type_info, prop.GetPtr(in_obj), prop.name.text);
		}

		indent -= 2;
	}
}

void DumpSerializer::Serialize(s8 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(s8) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void DumpSerializer::Serialize(s32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(s32) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void DumpSerializer::Serialize(u32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(u32) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void DumpSerializer::Serialize(float in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(float) serializing a fundamental type (name = %s) (type = %s) (value : %f)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void DumpSerializer::Serialize(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(u64) serializing a fundamental type (name = %s) (type = %s) (value : %llu)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void DumpSerializer::Serialize(std_string& in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(std_string&) serializing a fundamental type (name = %s) (type = %s) (value : %s)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj.c_str());
}

void DumpSerializer::SerializeEnum(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	const EnumConstant* enumVal = typeInfo->type->GetEnumConstant(in_obj);
	DEBUG_LOG(L("%sSerializeEnum() serializing an enum (name = %s) (type = %s) (value : %llu : %s)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj, enumVal ? enumVal->name.text : L("INVALID"));
}


void XMLSerializer::SerializeContainer(void* in_obj, const string_char* name, const InstanceTypeInfo* /*typeInfo*/, IContainer* container)
{
	DEBUG_LOG(L("%s<Container name=\"%s\" type=\"%s\">"), GetIndent().c_str(), name, container->GetContainerName());

	indent += 2;

	for (size_t i = 0; i < container->GetCount(in_obj); i++)
	{
		void* value = container->GetValue(in_obj, i);
		container->typeInfo->serialize_func(this, container->typeInfo, value, L(""));
	}

	indent -= 2;

	DEBUG_LOG(L("%s</Container>"), GetIndent().c_str());
}

void XMLSerializer::Serialize(void* in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	if (in_obj != nullptr)
	{
		DEBUG_LOG(L("%s<Object name=\"%s\" type=\"%s\">"), GetIndent().c_str(), name, typeInfo->type->name.text);

		indent += 2;

		for (auto prop : typeInfo->type->vProperties)
		{
			prop.type_info->serialize_func(this, prop.type_info, prop.GetPtr(in_obj), prop.name.text);
		}

		indent -= 2;

		DEBUG_LOG(L("%s</Object>"), GetIndent().c_str());
	}
}

void XMLSerializer::Serialize(s8 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%d</Value>"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void XMLSerializer::Serialize(s32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	//DEBUG_LOG(L("%sSerialize(s32) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%d</Value>"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void XMLSerializer::Serialize(u32 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	//DEBUG_LOG(L("%sSerialize(u32) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%d</Value>"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void XMLSerializer::Serialize(float in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	//DEBUG_LOG(L("%sSerialize(float) serializing a fundamental type (name = %s) (type = %s) (value : %f)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%f</Value>"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void XMLSerializer::Serialize(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	//DEBUG_LOG(L("%sSerialize(u64) serializing a fundamental type (name = %s) (type = %s) (value : %llu)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%llu</Value>"), GetIndent().c_str(), name, typeInfo->name.text, in_obj);
}

void XMLSerializer::Serialize(std_string& in_obj, const string_char* name, const InstanceTypeInfo* /*typeInfo*/)
{
	//DEBUG_LOG(L("%sSerialize(std_string&) serializing a fundamental type (name = %s) (type = %s) (value : %s)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj.c_str());
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%s</Value>"), GetIndent().c_str(), name, L("string"), in_obj.c_str());
}

void XMLSerializer::SerializeEnum(u64 in_obj, const string_char* name, const InstanceTypeInfo* typeInfo)
{
	const EnumConstant* enumVal = typeInfo->type->GetEnumConstant(in_obj);
	//DEBUG_LOG(L("%sSerializeEnum() serializing an enum (name = %s) (type = %s) (value : %llu : %s)"), GetIndent().c_str(), name, typeInfo->name.text, in_obj, enumVal ? enumVal->name.text : L("INVALID"));

	DEBUG_LOG(L("%s<EnumValue name=\"%s\" type=\"%s\">%s</EnumValue>"), GetIndent().c_str(), name, typeInfo->name.text, enumVal ? enumVal->name.text : L("INVALID"));
}
