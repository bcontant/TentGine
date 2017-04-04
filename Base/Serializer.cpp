#include "precompiled.h"

#include "Serializer.h"

#include "TypeDB.h"
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

void DumpSerializer::SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer)
{
	DEBUG_LOG(L("%sSerialize(void*) serializing a container (name = %s) (type = %s) [%s[%d]] at 0x%08x"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_pContainer->GetContainerName(), in_pContainer->GetCount(in_obj), in_pContainer);

	indent += 2;

	for (size_t i = 0; i < in_pContainer->GetCount(in_obj); i++)
	{
		void* value = in_pContainer->GetValue(in_obj, i);
		in_pContainer->Serialize(this, value, L(""));
	}

	indent -= 2;
}

void DumpSerializer::Serialize(void* in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(void*) serializing a pointer (name = %s) (type = %s) at 0x%08x"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);

	if (in_obj != nullptr)
	{
		indent += 2;

		for (auto prop : typeInfo->m_MetaInfo->vProperties)
		{
			prop.type_info->Serialize(this, prop.GetPtr(in_obj), prop.m_Name.text);
		}

		indent -= 2;
	}
}

void DumpSerializer::Serialize(s8 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(s8) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void DumpSerializer::Serialize(s32 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(s32) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void DumpSerializer::Serialize(u32 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(u32) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void DumpSerializer::Serialize(float in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(float) serializing a fundamental type (name = %s) (type = %s) (value : %f)"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void DumpSerializer::Serialize(u64 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(u64) serializing a fundamental type (name = %s) (type = %s) (value : %llu)"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void DumpSerializer::Serialize(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerialize(std_string&) serializing a fundamental type (name = %s) (type = %s) (value : %s)"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj.c_str());
}

void DumpSerializer::SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	const EnumConstant* enumVal = typeInfo->m_MetaInfo->GetEnumConstant(in_obj);
	DEBUG_LOG(L("%sSerializeEnum() serializing an enum (name = %s) (type = %s) (value : %llu : %s)"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj, enumVal ? enumVal->m_Name.text : L("INVALID"));
}


void XMLSerializer::SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* /*typeInfo*/, IContainer* in_pContainer)
{
	DEBUG_LOG(L("%s<Container name=\"%s\" type=\"%s\">"), GetIndent().c_str(), name, in_pContainer->GetContainerName());

	indent += 2;

	for (size_t i = 0; i < in_pContainer->GetCount(in_obj); i++)
	{
		void* value = in_pContainer->GetValue(in_obj, i);
		in_pContainer->Serialize(this, value, L(""));
	}

	indent -= 2;

	DEBUG_LOG(L("%s</Container>"), GetIndent().c_str());
}

void XMLSerializer::Serialize(void* in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	if (in_obj != nullptr)
	{
		DEBUG_LOG(L("%s<Object name=\"%s\" type=\"%s\">"), GetIndent().c_str(), name, typeInfo->m_MetaInfo->m_Name.text);

		indent += 2;

		for (auto prop : typeInfo->m_MetaInfo->vProperties)
		{
			prop.type_info->Serialize(this, prop.GetPtr(in_obj), prop.m_Name.text);
		}

		indent -= 2;

		DEBUG_LOG(L("%s</Object>"), GetIndent().c_str());
	}
}

void XMLSerializer::Serialize(s8 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%d</Value>"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void XMLSerializer::Serialize(s32 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%d</Value>"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void XMLSerializer::Serialize(u32 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%d</Value>"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void XMLSerializer::Serialize(float in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%f</Value>"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void XMLSerializer::Serialize(u64 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%llu</Value>"), GetIndent().c_str(), name, typeInfo->m_Name.text, in_obj);
}

void XMLSerializer::Serialize(std_string& in_obj, const string_char* name, const TypeInfo* /*typeInfo*/)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%s</Value>"), GetIndent().c_str(), name, L("string"), in_obj.c_str());
}

void XMLSerializer::SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	const EnumConstant* enumVal = typeInfo->m_MetaInfo->GetEnumConstant(in_obj);
	DEBUG_LOG(L("%s<EnumValue name=\"%s\" type=\"%s\">%s</EnumValue>"), GetIndent().c_str(), name, typeInfo->m_Name.text, enumVal ? enumVal->m_Name.text : L("INVALID"));
}
