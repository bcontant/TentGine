#include "precompiled.h"

#include "TypeDB.h"
#include "InstanceTypeInfo.h"
#include "Type.h"

TypeInfo::TypeInfo(Name in_name, Type* in_type, bool in_is_pointer, int in_size, IContainer* in_pContainer, TypeOperators in_operators)
	: m_Name(in_name)
	, m_MetaInfo(in_type)
	, m_bIsPointer(in_is_pointer)
	, m_Size(in_size)
	, m_pContainer(in_pContainer)
	, m_Operators(in_operators)
{
}

TypeInfo::~TypeInfo()
{
	delete m_pContainer;
}

bool TypeInfo::IsDerivedFrom(const TypeInfo* in_pInfo) const
{
	/*if(m_MetaInfo == nullptr || in_pInfo == nullptr || in_pInfo->m_MetaInfo == nullptr)
		return false;

	if(m_MetaInfo->typeInfo == in_pInfo->m_MetaInfo->typeInfo)
		return true;

	if(m_MetaInfo && m_MetaInfo->base_type && m_MetaInfo->base_type->typeInfo)
		return m_MetaInfo->base_type->typeInfo->IsDerivedFrom(in_pInfo);*/

	if(in_pInfo == this)
		return true;

	if(m_MetaInfo == nullptr)
		return false;

	return m_MetaInfo->IsDerivedFrom(in_pInfo);
}

void TypeInfo::Constructor(void* data) const
{
	if (m_MetaInfo && m_MetaInfo->m_Operators.constructor && m_MetaInfo->m_Name == m_Name)
		m_MetaInfo->m_Operators.constructor(data);
	else if (m_Operators.constructor)
		m_Operators.constructor(data);
	else
		CHECK_ERROR(ErrorCode::FailedConstructor, false);
}

void* TypeInfo::CopyConstructor(const void* src) const
{
	if (m_MetaInfo && m_MetaInfo->m_Operators.copy_constructor && m_MetaInfo->m_Name == m_Name)
		return m_MetaInfo->m_Operators.copy_constructor(src);
	else if (m_Operators.copy_constructor)
		return m_Operators.copy_constructor(src);
	else
	{
		//Fallback
		void* dst = New();
		AssignmentOperator(dst, src);
		return dst;
	}
}

void TypeInfo::Destructor(void* data) const
{
	if (m_MetaInfo && m_MetaInfo->m_Operators.destructor && m_MetaInfo->m_Name == m_Name)
		m_MetaInfo->m_Operators.destructor(data);
	else if (m_Operators.destructor)
		m_Operators.destructor(data);
	else
		CHECK_ERROR(ErrorCode::FailedDestructor, false);
}

void TypeInfo::AssignmentOperator(void* dst, const void* src) const
{
	if (m_MetaInfo && m_MetaInfo->m_Operators.assignment_operator && m_MetaInfo->m_Name == m_Name)
		m_MetaInfo->m_Operators.assignment_operator(dst, src);
	else if (m_Operators.assignment_operator)
		m_Operators.assignment_operator(dst, src);
	else
	{
		//Could fallback on New + Memcpy, but that's pretty dangerous
		CHECK_ERROR(ErrorCode::FailedAssignmentOperator, false);
	}
}

void TypeInfo::Serialize(Serializer* serializer, void* obj, const string_char* in_name) const
{
	if (m_MetaInfo && m_MetaInfo->m_Operators.serialize_func && m_MetaInfo->m_Name == m_Name)
		m_MetaInfo->m_Operators.serialize_func(serializer, this, obj, in_name);
	else if(m_Operators.serialize_func)
		m_Operators.serialize_func(serializer, this, obj, in_name);
	else
		CHECK_ERROR(ErrorCode::FailedSerialization, false);
}

void* TypeInfo::New() const
{
	void* data = new u8[m_Size];
	Constructor(data);
	return data;
}

void  TypeInfo::Delete(void* data) const
{
	Destructor(data);

	delete[] reinterpret_cast<u8*>(data);
	data = nullptr;
}