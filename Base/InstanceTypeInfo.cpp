#include "precompiled.h"

#include "TypeDB.h"
#include "InstanceTypeInfo.h"
#include "Type.h"

TypeInfo::TypeInfo(Name in_name, Type* in_type, const TypeInfo* in_pDereferencedType, int in_size, IContainer* in_pContainer, TypeOperators in_operators)
	: m_Name(in_name)
	, m_MetaInfo(in_type)
	, m_pDereferencedTypeInfo(in_pDereferencedType)
	, m_Size(in_size)
	, m_pContainer(in_pContainer)
	, m_Operators(in_operators)
{
	TypeDB::GetInstance()->RegisterTypeInfo(this);
}

TypeInfo::~TypeInfo()
{
	delete m_pContainer;
}

bool TypeInfo::IsPointer() const
{
	return m_pDereferencedTypeInfo != nullptr;
}

bool TypeInfo::IsDerivedFrom(const TypeInfo* in_pInfo) const
{
	if(in_pInfo == this)
		return true;

	if(m_MetaInfo == nullptr)
		return false;

	return m_MetaInfo->IsDerivedFrom(in_pInfo);
}

void* TypeInfo::GetVTableAddress() const
{
	return m_Operators.vtable_address;
}

const TypeInfo* TypeInfo::GetActualObjectTypeInfo(void* obj) const
{
	//If this type represents an object, it's polymorphic and our object data is non-null, we
	//might have a winner.
	if (m_Operators.vtable_address != nullptr && obj != nullptr)
	{
		//Get the object instance's vtable address
		//TODO : This breaks way the fuck down with multiple inheritance
		void* currentVTable = *((void**)(obj));

		//Compare it with the current typeInfo's vtable.  If it's different, we're dealing with a derived object.
		if (currentVTable != nullptr && currentVTable != m_Operators.vtable_address)
		{
			//Find the proper derived object class
			const TypeInfo* pInfo = TypeDB::GetInstance()->FindFromVTable(currentVTable);

			//Sanity check, should never happen
			Assert(pInfo != nullptr && pInfo->IsDerivedFrom(this));

			return pInfo;
		}
	}
	return this;
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
	const TypeInfo* pInfo = GetActualObjectTypeInfo(obj);
	if (pInfo != this)
		pInfo->Serialize(serializer, obj, in_name);
	else if (m_MetaInfo && m_MetaInfo->m_Operators.serialize_func && m_MetaInfo->m_Name == m_Name)
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