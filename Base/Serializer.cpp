#include "precompiled.h"

#include "Serializer.h"

#include "TypeDB.h"
#include "TypeInfo.h"

#include "Logger.h"

#pragma warning(disable:4100)

std_string DumpSerializer::GetIndent()
{
	std_string s;
	for (u32 i = 0; i < indent; i++)
		s += L(" ");
	return s;
}



void DumpSerializer::SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer)
{
	DEBUG_LOG(L("%sSerializeContainer(void*) serializing a container (name = %s) (type = %s) [%s[%d]] at 0x%08x"), GetIndent().c_str(), name, typeInfo->GetName(), in_pContainer->GetContainerName(), in_pContainer->GetCount(in_obj), in_pContainer);

	indent += 2;

	in_pContainer->SerializeContents(this, in_obj);

	indent -= 2;
}

void DumpSerializer::DeserializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer)
{

}

void DumpSerializer::SerializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerializeObject(void*) serializing a pointer (name = %s) (type = %s) at 0x%08x"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);

	if (in_obj != nullptr)
	{
		indent += 2;

		typeInfo->SerializeProperties(this, in_obj);

		indent -= 2;
	}
}

void DumpSerializer::DeserializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo)
{

}
void DumpSerializer::SerializeValue(s8 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerializeValue(s8) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);
}

void DumpSerializer::DeserializeValue(s8& in_obj, const string_char* name, const TypeInfo* typeInfo)
{

}

void DumpSerializer::SerializeValue(s32 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerializeValue(s32) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);
}

void DumpSerializer::DeserializeValue(s32& in_obj, const string_char* name, const TypeInfo* typeInfo)
{

}

void DumpSerializer::SerializeValue(u32 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerializeValue(u32) serializing a fundamental type (name = %s) (type = %s) (value : %d)"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);
}

void DumpSerializer::DeserializeValue(u32& in_obj, const string_char* name, const TypeInfo* typeInfo)
{

}

void DumpSerializer::SerializeValue(float in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerializeValue(float) serializing a fundamental type (name = %s) (type = %s) (value : %f)"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);
}

void DumpSerializer::DeserializeValue(float& in_obj, const string_char* name, const TypeInfo* typeInfo)
{

}

void DumpSerializer::SerializeValue(u64 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerializeValue(u64) serializing a fundamental type (name = %s) (type = %s) (value : %llu)"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);
}

void DumpSerializer::DeserializeValue(u64& in_obj, const string_char* name, const TypeInfo* typeInfo)
{

}

void DumpSerializer::SerializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%sSerializeValue(std_string&) serializing a fundamental type (name = %s) (type = %s) (value : %s)"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj.c_str());
}

void DumpSerializer::DeserializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo)
{

}

void DumpSerializer::SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	const EnumConstant* enumVal = typeInfo->GetEnumConstant(in_obj);
	DEBUG_LOG(L("%sSerializeEnum() serializing an enum (name = %s) (type = %s) (value : %llu : %s)"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj, enumVal ? (const string_char*)enumVal->m_Name : L("INVALID"));
}

void DumpSerializer::DeserializeEnum(u64& in_obj, const string_char* name, const TypeInfo* typeInfo)
{

}

#include "XMLTag.h"


XMLTag* XMLSerializer::FindChildTag(const std_string& tagName, const std_string& name, const std_string& type)
{
	//Find the tag
	if (vTagStack.back()->GetName() != L("Container"))
	{
		XMLTag* pObjectTag = vTagStack.back()->FirstChildNamed(tagName, false);
		while (pObjectTag)
		{
			std_string tagType;
			pObjectTag->GetAttribute(L("type"), tagType);

			if (tagType == type)
			{
				if (std_string(name) != L(""))
				{
					std_string propName;
					pObjectTag->GetAttribute(L("propertyName"), propName);
					if (propName == name)
						break;
				}
				else
				{
					break;
				}
			}
			pObjectTag = pObjectTag->FirstSiblingNamed(tagName);
		}
		Assert(pObjectTag);
		return pObjectTag;
	}
	else
	{
		std_string tagType;
		vNextContainerElementStack.back()->GetAttribute(L("type"), tagType);

		std_string propName;
		vNextContainerElementStack.back()->GetAttribute(L("propertyName"), propName);

		Assert(vNextContainerElementStack.back()->GetName() == tagName && propName == name /*&& tagType == type*/);

		XMLTag* pTag = vNextContainerElementStack.back();
		vNextContainerElementStack.back() = vNextContainerElementStack.back()->GetSibling();
		return pTag;
	}
}

void XMLSerializer::SerializeContainer(void* in_obj, const string_char* name, const TypeInfo* /*typeInfo*/, IContainer* in_pContainer)
{
	XMLTag* pContainerTag = new XMLTag;
	pContainerTag->SetName(L("Container"));
	pContainerTag->AddAttribute(L("propertyName"), std_string(name));
	pContainerTag->AddAttribute(L("type"), std_string(in_pContainer->GetContainerName()));

	vTagStack.back()->AddChild(pContainerTag);
	vTagStack.push_back(pContainerTag);
	
	DEBUG_LOG(L("%s<Container name=\"%s\" type=\"%s\">"), GetIndent().c_str(), name, in_pContainer->GetContainerName());

	indent += 2;

	in_pContainer->SerializeContents(this, in_obj);

	indent -= 2;

	DEBUG_LOG(L("%s</Container>"), GetIndent().c_str());

	vTagStack.pop_back();	
}

void XMLSerializer::DeserializeContainer(void* in_obj, const string_char* name, const TypeInfo* typeInfo, IContainer* in_pContainer)
{
	//Find the tag
	XMLTag* pObjectTag = FindChildTag(L("Container"), name, in_pContainer->GetContainerName());
	if (pObjectTag == nullptr)
		return;

	in_pContainer->Clear(in_obj);

	if (pObjectTag->GetChild())
	{
		vTagStack.push_back(pObjectTag);

		vNextContainerElementStack.push_back(pObjectTag->GetChild());

		std_string objectType;
		pObjectTag->GetChild()->GetAttribute("type", objectType);
		const TypeInfo* pChildType = TypeDB::GetInstance()->GetType(objectType);

		in_pContainer->DeserializeContents(this, in_obj, pChildType);

		vNextContainerElementStack.pop_back();

		vTagStack.pop_back();
	}
}


void XMLSerializer::SerializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	if (in_obj != nullptr)
	{
		XMLTag* pObjectTag = new XMLTag;
		pObjectTag->SetName(L("Object"));
		pObjectTag->AddAttribute(L("propertyName"), std_string(name));
		pObjectTag->AddAttribute(L("type"), std_string(typeInfo->GetName()));

		vTagStack.back()->AddChild(pObjectTag);
		vTagStack.push_back(pObjectTag);

		DEBUG_LOG(L("%s<Object name=\"%s\" type=\"%s\">"), GetIndent().c_str(), name, typeInfo->GetName());

		indent += 2;

		typeInfo->SerializeProperties(this, in_obj);

		indent -= 2;

		DEBUG_LOG(L("%s</Object>"), GetIndent().c_str());

		vTagStack.pop_back();	
	}
}

void XMLSerializer::DeserializeObject(void* in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	//Find the tag
	XMLTag* pObjectTag = FindChildTag(L("Object"), name, typeInfo->GetName());
	if(pObjectTag == nullptr)
		return;

	vTagStack.push_back(pObjectTag);

	typeInfo->DeserializeProperties(this, in_obj);

	vTagStack.pop_back();
}


void XMLSerializer::SerializeValue(s8 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%d</Value>"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);

	XMLTag* pValueTag = new XMLTag;
	pValueTag->SetName(L("Value"));
	pValueTag->SetTagType(XMLTag::ETagType::eTT_SelfCloseTag);
	pValueTag->AddAttribute(L("propertyName"), std_string(name));
	pValueTag->AddAttribute(L("type"), std_string(typeInfo->GetName()));
	pValueTag->AddAttribute(L("value"), in_obj);

	vTagStack.back()->AddChild(pValueTag);
}

void XMLSerializer::DeserializeValue(s8& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	XMLTag* pObjectTag = FindChildTag(L("Value"), name, typeInfo->GetName());

	if (pObjectTag == nullptr)
		return;

	const XMLAttribute* pRet = pObjectTag->GetAttribute(L("value"), in_obj);
	Assert(pRet);
}


void XMLSerializer::SerializeValue(s32 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%d</Value>"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);

	XMLTag* pValueTag = new XMLTag;
	pValueTag->SetName(L("Value"));
	pValueTag->SetTagType(XMLTag::ETagType::eTT_SelfCloseTag);
	pValueTag->AddAttribute(L("propertyName"), std_string(name));
	pValueTag->AddAttribute(L("type"), std_string(typeInfo->GetName()));
	pValueTag->AddAttribute(L("value"), in_obj);

	vTagStack.back()->AddChild(pValueTag);
}

void XMLSerializer::DeserializeValue(s32& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	XMLTag* pObjectTag = FindChildTag(L("Value"), name, typeInfo->GetName());

	if (pObjectTag == nullptr)
		return;

	const XMLAttribute* pRet = pObjectTag->GetAttribute(L("value"), in_obj);
	Assert(pRet);
}
void XMLSerializer::SerializeValue(u32 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%u</Value>"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);

	XMLTag* pValueTag = new XMLTag;
	pValueTag->SetName(L("Value"));
	pValueTag->SetTagType(XMLTag::ETagType::eTT_SelfCloseTag);
	pValueTag->AddAttribute(L("propertyName"), std_string(name));
	pValueTag->AddAttribute(L("type"), std_string(typeInfo->GetName()));
	pValueTag->AddAttribute(L("value"), in_obj);

	vTagStack.back()->AddChild(pValueTag);
}

void XMLSerializer::DeserializeValue(u32& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	XMLTag* pObjectTag = FindChildTag(L("Value"), name, typeInfo->GetName());

	if (pObjectTag == nullptr)
		return;

	const XMLAttribute* pRet = pObjectTag->GetAttribute(L("value"), in_obj);
	Assert(pRet);
}


void XMLSerializer::SerializeValue(float in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%f</Value>"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);

	XMLTag* pValueTag = new XMLTag;
	pValueTag->SetName(L("Value"));
	pValueTag->SetTagType(XMLTag::ETagType::eTT_SelfCloseTag);
	pValueTag->AddAttribute(L("propertyName"), std_string(name));
	pValueTag->AddAttribute(L("type"), std_string(typeInfo->GetName()));
	pValueTag->AddAttribute(L("value"), in_obj);

	vTagStack.back()->AddChild(pValueTag);
}

void XMLSerializer::DeserializeValue(float& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	XMLTag* pObjectTag = FindChildTag(L("Value"), name, typeInfo->GetName());

	if (pObjectTag == nullptr)
		return;

	const XMLAttribute* pRet = pObjectTag->GetAttribute(L("value"), in_obj);
	Assert(pRet);
}

void XMLSerializer::SerializeValue(u64 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%llu</Value>"), GetIndent().c_str(), name, typeInfo->GetName(), in_obj);

	XMLTag* pValueTag = new XMLTag;
	pValueTag->SetName(L("Value"));
	pValueTag->SetTagType(XMLTag::ETagType::eTT_SelfCloseTag);
	pValueTag->AddAttribute(L("propertyName"), std_string(name));
	pValueTag->AddAttribute(L("type"), std_string(typeInfo->GetName()));
	pValueTag->AddAttribute(L("value"), (s64)in_obj);

	vTagStack.back()->AddChild(pValueTag);
}

void XMLSerializer::DeserializeValue(u64& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	XMLTag* pObjectTag = FindChildTag(L("Value"), name, typeInfo->GetName());

	if (pObjectTag == nullptr)
		return;

	const XMLAttribute* pRet = pObjectTag->GetAttribute(L("value"), (s64&)in_obj);
	Assert(pRet);
}

void XMLSerializer::SerializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	DEBUG_LOG(L("%s<Value name=\"%s\" type=\"%s\">%s</Value>"), GetIndent().c_str(), name, L("string"), in_obj.c_str());

	XMLTag* pValueTag = new XMLTag;
	pValueTag->SetName(L("Value"));
	pValueTag->SetTagType(XMLTag::ETagType::eTT_SelfCloseTag);
	pValueTag->AddAttribute(L("propertyName"), std_string(name));
	pValueTag->AddAttribute(L("type"), std_string(L("string")));
	pValueTag->AddAttribute(L("value"), in_obj);

	vTagStack.back()->AddChild(pValueTag);
}

void XMLSerializer::DeserializeValue(std_string& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	XMLTag* pObjectTag = FindChildTag(L("Value"), name, L("string"));

	if (pObjectTag == nullptr)
		return;

	const XMLAttribute* pRet = pObjectTag->GetAttribute(L("value"), in_obj);
	Assert(pRet);
}

void XMLSerializer::SerializeEnum(u64 in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	const EnumConstant* enumVal = typeInfo->GetEnumConstant(in_obj);
	DEBUG_LOG(L("%s<EnumValue name=\"%s\" type=\"%s\">%s</EnumValue>"), GetIndent().c_str(), name, typeInfo->GetName(), enumVal ? (const string_char*)enumVal->m_Name : L("INVALID"));

	XMLTag* pValueTag = new XMLTag;
	pValueTag->SetName(L("Value"));
	pValueTag->SetTagType(XMLTag::ETagType::eTT_SelfCloseTag);
	pValueTag->AddAttribute(L("propertyName"), std_string(name));
	pValueTag->AddAttribute(L("type"), std_string(typeInfo->GetName()));
	pValueTag->AddAttribute(L("value"), std_string(enumVal ? (const string_char*)enumVal->m_Name : L("INVALID")));

	vTagStack.back()->AddChild(pValueTag);
}

void XMLSerializer::DeserializeEnum(u64& in_obj, const string_char* name, const TypeInfo* typeInfo)
{
	XMLTag* pObjectTag = FindChildTag(L("Value"), name, typeInfo->GetName());

	if (pObjectTag == nullptr)
		return;

	std_string enumValue;
	const XMLAttribute* pRet = pObjectTag->GetAttribute(L("value"), enumValue);
	Assert(pRet);

	const EnumConstant* pEnumConstant = typeInfo->GetEnumConstant(enumValue.c_str());
	if(pEnumConstant)
		in_obj = pEnumConstant->value;
	else
		in_obj = 0;
}


#pragma warning(default:4100)