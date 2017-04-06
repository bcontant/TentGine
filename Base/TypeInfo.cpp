#include "precompiled.h"

#include "TypeDB.h"
#include "TypeInfo.h"

TypeInfo::TypeInfo(Name in_name, const TypeInfo* in_pDereferencedType, size_t in_size, IContainer* in_pContainer, TypeOperators in_operators)
	: m_Name(in_name)
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


void TypeInfo::SetDefaultConstructor(ConstructObjectFunc f)
{
	m_Operators.constructor = f;
}


void TypeInfo::AddProperty(TypeProperty&& field)
{
	CHECK_ERROR_MSG(ErrorCode::PropertyAlreadyAdded, GetProperty(field.m_Name.text) == nullptr, L("Adding a Property which was already there!"));

	if (GetProperty(field.m_Name.text) == nullptr)
		vProperties.push_back(field);
}

void TypeInfo::AddEnumConstant(EnumConstant&& enumConst)
{
	CHECK_ERROR_MSG(ErrorCode::EnumValueAlreadyAdded, GetEnumConstant(enumConst.value) == nullptr, L("Adding an Enum Value which was already there!"));

	if (GetEnumConstant(enumConst.value) == nullptr)
		vEnumConstants.push_back(enumConst);
}

void TypeInfo::AddFunction(TypeFunction&& function)
{
	//CHECK_ERROR(ErrorCode::FunctionAlreadyAdded, GetFunction(function.m_Name.text) == nullptr);

	//if (GetFunction(function.m_Name.text) == nullptr)
		vFunctions.push_back(function);
}

bool TypeInfo::IsPointer() const
{
	return m_pDereferencedTypeInfo != nullptr;
}

bool TypeInfo::IsDerivedFrom(const TypeInfo* in_TypeInfo) const
{
	if (this == in_TypeInfo)
		return true;

	if(m_pDereferencedTypeInfo != nullptr)
		return m_pDereferencedTypeInfo->IsDerivedFrom(in_TypeInfo);

	if(in_TypeInfo->m_pDereferencedTypeInfo != nullptr)
		return IsDerivedFrom(in_TypeInfo->m_pDereferencedTypeInfo);

	if (base_type == nullptr)
		return false;

	return base_type->IsDerivedFrom(in_TypeInfo);
}

bool TypeInfo::IsCastableAs(void* in_pObj, const TypeInfo* in_pInfo) const
{
	if (this == in_pInfo)
		return true;

	if(in_pObj == nullptr || this == nullptr)
		return false;

	//T* to any*
	if (IsPointer() && in_pInfo->IsPointer())
	{
		//Dynamic cast (upcasting to an actual class)
		const TypeInfo* pActualTypeInfo = m_pDereferencedTypeInfo->GetActualObjectTypeInfo(*(void**)in_pObj);

		//Allow down casting to a parent class
		if (pActualTypeInfo->IsDerivedFrom(in_pInfo))
			return true;

		//Allow casting to void*
		if(in_pInfo == TypeInfo::Get<void*>())
			return true;
	}

	return false;
}

const TypeProperty* TypeInfo::GetProperty(const string_char* in_name) const
{
	for (size_t i = 0; i < vProperties.size(); i++)
	{
		// This is just a hash comparison
		if (Name(in_name) == vProperties[i].m_Name)
			return &vProperties[i];
	}

	// Recurse up through base type
	if (base_type)
		return base_type->GetProperty(in_name);

	return nullptr;
}

const TypeFunction* TypeInfo::GetFunction(const string_char* in_name) const
{
	for (size_t i = 0; i < vFunctions.size(); i++)
	{
		// This is just a hash comparison
		if (Name(in_name) == vFunctions[i].m_Name)
			return &vFunctions[i];
	}

	// Recurse up through base type
	if (base_type)
		return base_type->GetFunction(in_name);

	return nullptr;
}

const EnumConstant* TypeInfo::GetEnumConstant(u64 in_value) const
{
	for (size_t i = 0; i < vEnumConstants.size(); i++)
	{
		if (in_value == vEnumConstants[i].value)
			return &(vEnumConstants[i]);
	}
	return nullptr;
}

void* TypeInfo::New() const
{
	void* data = new u8[m_Size];
	Constructor(data);
	return data;
}

void TypeInfo::Constructor(void* data) const
{
	if (m_Operators.constructor)
		m_Operators.constructor(data);
	else
		CHECK_ERROR_MSG(ErrorCode::FailedConstructor, false, L("Failed to call constructor for object."));
}

void* TypeInfo::CopyConstructor(const void* src) const
{
	if (m_Operators.copy_constructor)
		return m_Operators.copy_constructor(src);
	else
	{
		//Fallback
		void* dst = New();
		AssignmentOperator(dst, src);
		return dst;
	}
}

void TypeInfo::AssignmentOperator(void* dst, const void* src) const
{
	if (m_Operators.assignment_operator)
		m_Operators.assignment_operator(dst, src);
	else
	{
		//Could fallback on New + Memcpy, but that's pretty dangerous
		CHECK_ERROR_MSG(ErrorCode::FailedAssignmentOperator, false, L("Failed to call assignement operator for object."));
	}
}

void  TypeInfo::Delete(void* data) const
{
	Destructor(data);

	delete[] reinterpret_cast<u8*>(data);
	data = nullptr;
}

void TypeInfo::Destructor(void* data) const
{
	if (m_Operators.destructor)
		m_Operators.destructor(data);
	else
		CHECK_ERROR_MSG(ErrorCode::FailedDestructor, false, L("Failed to call destructor for object."));
}

void TypeInfo::SerializeProperties(Serializer* s, void* in_obj) const
{
	for (auto prop : vProperties)
	{
		prop.m_pTypeInfo->Serialize(s, prop.GetPtr(in_obj), prop.m_Name.text);
	}

	if (base_type)
		return base_type->SerializeProperties(s, in_obj);
}

void TypeInfo::Serialize(Serializer* serializer, void* obj, const string_char* in_name) const
{
	const TypeInfo* pInfo = GetActualObjectTypeInfo(obj);
	if (pInfo != this)
		pInfo->Serialize(serializer, obj, in_name);
	else if(m_Operators.serialize_func)
		m_Operators.serialize_func(serializer, this, obj, in_name);
	else
		CHECK_ERROR_MSG(ErrorCode::FailedSerialization, false, L("Failed to call serialize function for object."));
}



const TypeInfo* TypeInfo::GetActualObjectTypeInfo(void* obj) const
{
	const TypeInfo* pInfo = m_Operators.object_type_func(obj);
	Assert(pInfo != nullptr);
	return pInfo;
}