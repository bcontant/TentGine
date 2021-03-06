#pragma once

#include "Utils.h"
#include "TypeDB.h"

//Destruction
using DestructObjectFunc = void(*)(void*);

template <typename T>
void DestructObject(void* object) { ((T*)object)->T::~T(); }

template <typename T>
typename std::enable_if<std::is_destructible<T>::value, DestructObjectFunc>::type
GetDestructor()
{
	return DestructObject<T>;
}

template <typename T>
typename std::enable_if<!std::is_destructible<T>::value, DestructObjectFunc>::type
GetDestructor()
{
	return nullptr;
}

//Construction
using ConstructObjectFunc = void(*)(void*);

template <typename T>
struct DefaultConstructor
{
	static void ConstructObject(void* object) { new (object) T; }
};

//Specialize for array types
template <typename T, int U>
struct DefaultConstructor<T[U]>
{
	static void ConstructObject(void* object) { new (object) T[U]; }
};

template <typename T>
typename std::enable_if<std::is_default_constructible<T>::value, ConstructObjectFunc>::type
GetDefaultConstructor()
{
	return DefaultConstructor<T>::ConstructObject;
}

template <typename T>
typename std::enable_if<!std::is_default_constructible<T>::value, ConstructObjectFunc>::type
GetDefaultConstructor()
{
	return nullptr;
}

//Assignment Operator
using AssignmentOperatorFunc = void(*)(void*& dst, const void* src);

template <typename T>
struct DefaultAssignmentOperator
{
	static void AssigmentOperator(void*& dst, const void* src)
	{ 
		Assert(dst != nullptr && src != nullptr);

		if(dst != nullptr && src != nullptr)
			*((T*)dst) = *((T*)src);
	}
};

template <typename T>
typename std::enable_if<std::is_copy_assignable<T>::value, AssignmentOperatorFunc>::type
GetDefaultAssignmentOperator()
{
	return DefaultAssignmentOperator<T>::AssigmentOperator;
}

template <typename T>
typename std::enable_if<!std::is_copy_assignable<T>::value, AssignmentOperatorFunc>::type
GetDefaultAssignmentOperator()
{
	return nullptr;
}

//Copy Constructor
using CopyConstructorFunc = void*(*)(const void* src);

template <typename T>
struct DefaultCopyConstructor
{
	static void* CopyConstructor(const void* src)
	{
		Assert(src != nullptr);

		if(src != nullptr)
			return (void*) new T( (const T&) (*((T*)src)) );
			
		return nullptr;
	}
};

template <typename T>
typename std::enable_if<std::is_copy_constructible<T>::value, CopyConstructorFunc>::type
GetDefaultCopyConstructor()
{
	return DefaultCopyConstructor<T>::CopyConstructor;
}

template <typename T>
typename std::enable_if<!std::is_copy_constructible<T>::value, CopyConstructorFunc>::type
GetDefaultCopyConstructor()
{
	return nullptr;
}

//Serialization
struct Serializer;
struct IContainer;

using SerializeObjectFunc = void(*)(Serializer*, const TypeInfo*, void*, const string_char* name);
using DeserializeObjectFunc = void(*)(Serializer*, const TypeInfo*, void*, const string_char* name);

template <typename T>
struct SerializeVisitor
{
	static void SerializePointer(Serializer* /*s*/, const TypeInfo* /*typeInfo*/, void* /*in_obj*/, const string_char* /*name*/)
	{
		//TODO : Pointer of fundamentals?  Even a pointer on an object, could be an array.
		// + Flags on typeinfo?  I'd need to remove the nice staticeness of the InstanceTypeInfos
		// Edit : Flags go on the TypeProperty, idiot!

		// Flags : eArray w/ size on another member?
		//         eNullTerminated (for c strings)
		

		static_assert(IsPointer<T>::val == true, "Something is wrong.");
		static_assert(IsFundamental<T>::val == false, "Something is wrong.");

		Assert(false); // This is disabled right now.  Pointers are Containers (Dynamic Array of 1, for now).
		/*CHECK_ERROR(ErrorCode::NullDereferencedTypeInfo, typeInfo->m_pDereferencedTypeInfo != nullptr);

		//Serialize as an object
		if (typeInfo->m_pDereferencedTypeInfo)
		{
			typeInfo->m_pDereferencedTypeInfo->Serialize(s, *((void**)in_obj), name);
			//s->Serialize(*((void**)in_obj), name, typeInfo->m_pDereferencedTypeInfo);
		}*/
	}

		static void DeserializePointer(Serializer* /*s*/, const TypeInfo* /*typeInfo*/, void* /*in_obj*/, const string_char* /*name*/)
	{
	}

	static void SerializeObject(Serializer* s, const TypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");

		s->SerializeObject(in_obj, name, typeInfo);
	}

	static void DeserializeObject(Serializer* s, const TypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");

		s->DeserializeObject(in_obj, name, typeInfo);
	}

	static void SerializeFundamental(Serializer* s, const TypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");
		AssertMsg(in_obj != nullptr, L("Something is wrong."));

		s->SerializeValue(*((T*)in_obj), name, typeInfo);
	}

	static void DeserializeFundamental(Serializer* s, const TypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");
		AssertMsg(in_obj != nullptr, L("Something is wrong."));

		s->DeserializeValue((T&)*((T*)in_obj), name, typeInfo);
	}

	static void SerializeEnum(Serializer* s, const TypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");
		static_assert(sizeof(T) <= 8, "Something is wrong.");

		u64 enumVal = 0;
		memcpy(&enumVal, in_obj, typeInfo->GetSize());
		s->SerializeEnum(enumVal, name, typeInfo);
	}

	static void DeserializeEnum(Serializer* s, const TypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		static_assert(IsPointer<T>::val == false, "Something is wrong.");
		static_assert(sizeof(T) <= 8, "Something is wrong.");

		u64 enumVal = 0;
		s->DeserializeEnum(enumVal, name, typeInfo);
		memcpy(in_obj, &enumVal, typeInfo->GetSize());
	}

	static void SerializeContainer(Serializer* s, const TypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		AssertMsg(typeInfo->GetContainer() != nullptr, L("Something is wrong."));

		s->SerializeContainer(in_obj, name, typeInfo, typeInfo->GetContainer());
	}

	static void DeserializeContainer(Serializer* s, const TypeInfo* typeInfo, void* in_obj, const string_char* name)
	{
		AssertMsg(typeInfo->GetContainer() != nullptr, L("Something is wrong."));

		s->DeserializeContainer(in_obj, name, typeInfo, typeInfo->GetContainer());
	}

	static void SerializeVoid(Serializer*, const TypeInfo*, void*, const string_char*)
	{
		CHECK_ERROR_MSG(ErrorCode::SerializingVoid, false, L("If you're serializing void something weird happened."));
	}

	static void DeserializeVoid(Serializer*, const TypeInfo*, void*, const string_char*)
	{
		CHECK_ERROR_MSG(ErrorCode::SerializingVoid, false, L("If you're serializing void something weird happened."));
	}
};

template <typename T>
typename std::enable_if<SerializeAsVoid<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeVoid;
}

template <typename T>
typename std::enable_if<SerializeAsPointer<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializePointer;
}

template <typename T>
typename std::enable_if<SerializeAsObject<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeObject;
}

template <typename T>
typename std::enable_if<SerializeAsFundamental<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeFundamental;
}

template <typename T>
typename std::enable_if<SerializeAsEnum<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeEnum;
}

template <typename T>
typename std::enable_if<SerializeAsContainer<T>::val, SerializeObjectFunc>::type
GetSerializeFunc()
{
	return SerializeVisitor<T>::SerializeContainer;
}


template <typename T>
typename std::enable_if<SerializeAsVoid<T>::val, SerializeObjectFunc>::type
GetDeserializeFunc()
{
	return SerializeVisitor<T>::DeserializeVoid;
}

template <typename T>
typename std::enable_if<SerializeAsPointer<T>::val, SerializeObjectFunc>::type
GetDeserializeFunc()
{
	return SerializeVisitor<T>::DeserializePointer;
}

template <typename T>
typename std::enable_if<SerializeAsObject<T>::val, SerializeObjectFunc>::type
GetDeserializeFunc()
{
	return SerializeVisitor<T>::DeserializeObject;
}

template <typename T>
typename std::enable_if<SerializeAsFundamental<T>::val, SerializeObjectFunc>::type
GetDeserializeFunc()
{
	return SerializeVisitor<T>::DeserializeFundamental;
}

template <typename T>
typename std::enable_if<SerializeAsEnum<T>::val, SerializeObjectFunc>::type
GetDeserializeFunc()
{
	return SerializeVisitor<T>::DeserializeEnum;
}

template <typename T>
typename std::enable_if<SerializeAsContainer<T>::val, SerializeObjectFunc>::type
GetDeserializeFunc()
{
	return SerializeVisitor<T>::DeserializeContainer;
}


//GetObjectType
using GetObjectTypeFunc = const TypeInfo*(*)(void*);

template <typename T>
typename std::enable_if<!IsVoid<T>::val, const TypeInfo*>::type
GetObjectType(void* obj) 
{ 
	if(obj == nullptr)
		return nullptr;

	std_string str = GenerateTypeName( typeid( *((T*)obj) ).name() );
	return TypeDB::GetInstance()->GetType(Name(str.c_str()));
}

template <typename T>
typename std::enable_if<IsVoid<T>::val, const TypeInfo*>::type
GetObjectType(void*)
{
	return nullptr;
}

struct TypeOperators
{
	template <typename T>
	static TypeOperators Get()
	{
		TypeOperators op;
		op.constructor = GetDefaultConstructor<T>();
		op.copy_constructor = GetDefaultCopyConstructor<T>();
		op.assignment_operator = GetDefaultAssignmentOperator<T>();
		op.destructor = GetDestructor<T>();
		op.serialize_func = GetSerializeFunc<T>();
		op.deserialize_func = GetDeserializeFunc<T>();
		op.object_type_func = GetObjectType<T>;
		return op;
	};

	ConstructObjectFunc constructor;
	CopyConstructorFunc copy_constructor;
	AssignmentOperatorFunc assignment_operator;
	DestructObjectFunc destructor;
	SerializeObjectFunc serialize_func;
	DeserializeObjectFunc deserialize_func;
	GetObjectTypeFunc object_type_func;
};