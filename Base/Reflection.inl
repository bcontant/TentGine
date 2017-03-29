#pragma once

//-----------------------------------------
//Utility	
//-----------------------------------------

template <typename TYPE> const string_char* GetTypeName()
{
	static std_string type_name = GenerateTypeName(typeid(TYPE).name());
	return type_name.c_str();
}

template <typename TYPE> u32 GetTypeID()
{
	static int type_id = GenerateStringHash(GetTypeName<TYPE>());
	return type_id;
}

//-----------------------------------------
//VectorContainer
//-----------------------------------------

template<typename TYPE>
size_t VectorContainer<TYPE>::GetCount(void* container)
{
	return ((std::vector<TYPE>*)container)->size();
}

template<typename TYPE>
void* VectorContainer<TYPE>::GetValue(void* container, int index)
{
	return &((std::vector<TYPE>*)container)->at(index);
}

//-----------------------------------------
// TypeProperty
//-----------------------------------------

template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
TypeProperty::TypeProperty(Name name, PROPERTY_TYPE OBJECT_TYPE::*property)
	: name(name)
	, type(TypeDB::GetInstance()->GetType<StripPointer<PROPERTY_TYPE>::Type>())
	, is_pointer(IsPointer<PROPERTY_TYPE>::val)
	, offset(offsetof(OBJECT_TYPE, *property))
{
}

template <typename OBJECT_TYPE, typename PROPERTY_TYPE>
TypeProperty::TypeProperty(Name name, std::vector<PROPERTY_TYPE> OBJECT_TYPE::*property)
	: name(name)
	, type(TypeDB::GetInstance()->GetType<StripPointer<PROPERTY_TYPE>::Type>())
	, is_pointer(IsPointer<PROPERTY_TYPE>::val)
	, offset(offsetof(OBJECT_TYPE, *property))
{
	container = new VectorContainer<PROPERTY_TYPE>();
}

//-----------------------------------------
//Type
//-----------------------------------------

template <int SIZE>
Type& Type::AddProperties(TypeProperty(&properties)[SIZE])
{
	for (int i = 0; i < SIZE; i++)
	{
		TypeProperty f = properties[i];
		vProperties.push_back(f);
	}
	return *this;
}

template <int SIZE>
Type& Type::AddEnumConstants(EnumConst(&enum_consts)[SIZE])
{
	Assert(isEnum);
	for (int i = 0; i < SIZE; i++)
		vEnumConstants.push_back(input_enum_consts[i]);

	return *this;
}

template <typename TYPE>
Type& Type::Base()
{
	base_type = TypeDB::GetInstance()->GetType<TYPE>();
	return *this;
}

//-----------------------------------------
//TypeDB
//-----------------------------------------

template <typename TYPE>
Type& TypeDB::RegisterType()
{
	Type* type = 0;
	Name name = GetTypeName<TYPE>();

	TypeMap::iterator type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		type = new Type;
		mTypes[name] = type;
	}
	else
	{
		type = type_i->second;
	}

	// Apply type properties
	type->name = name;
	type->size = sizeof(TYPE);
	type->constructor = ConstructObject<TYPE>;
	type->destructor = DestructObject<TYPE>;
	type->isEnum = IsEnum<TYPE>::val;
	return *type;
}

template <typename TYPE>
Type* TypeDB::GetType()
{
	Name name = GetTypeName<TYPE>();
	TypeMap::iterator type_i = mTypes.find(name);
	if (type_i == mTypes.end())
	{
		return &RegisterType<TYPE>();
	}
	return type_i->second;
}
