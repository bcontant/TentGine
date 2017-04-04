#pragma once

template <typename T> struct SizeOf { static constexpr size_t val = sizeof(T); };
template <> struct SizeOf<void> { static constexpr size_t val = 0; };

template <typename T> struct IsVoid { static constexpr bool val = std::is_void<T>::value; };
template <typename T> struct IsEnum { static constexpr bool val = std::is_enum<T>::value; };
template <typename T> struct IsPointer { static constexpr bool val = std::is_pointer<T>::value; };
template <typename T> struct IsClass { static constexpr bool val = std::is_class<T>::value; };
template <typename T> struct IsAbstract { static constexpr bool val = std::is_abstract<T>::value; };
template <typename T> struct IsFundamental { static constexpr bool val = std::is_fundamental<T>::value || IsStdString<T>::val; };
template <typename T> struct IsContainer { static constexpr bool val = IsStdVector<T>::val || IsArray<T>::val; };

//Pointer Information
template <typename T> struct IndirectionCount { static constexpr size_t val = 0; };
template <typename T> struct IndirectionCount<T*> { static constexpr size_t val = 1; };
template <typename T> struct IndirectionCount<T**> { static constexpr size_t val = 2; };
template <typename T> struct IndirectionCount<T***> { static constexpr size_t val = 3; };
template <typename T> struct IndirectionCount<T****> { static constexpr size_t val = 4; };

//Ctor / Dtor Stuff
template <typename T> struct IsDefaultConstructible { static constexpr bool val = std::is_default_constructible<T>::value; };
template <typename T> struct IsDestructible { static constexpr bool val = std::is_destructible<T>::value; };
template <typename T> struct HasCtorAndDtor { static constexpr bool val = IsDestructible<T>::val && IsDefaultConstructible<T>::val; };

//Serializer stuff
template <typename T> struct SerializeAsVoid { static constexpr bool val = IsVoid<T>::val; };
template <typename T> struct SerializeAsPointer { static constexpr bool val = IsPointer<T>::val && !IsVoid<T>::val; };
template <typename T> struct SerializeAsEnum { static constexpr bool val = IsEnum<T>::val && !IsVoid<T>::val && !IsPointer<T>::val; };
template <typename T> struct SerializeAsContainer { static constexpr bool val = IsContainer<T>::val && !IsVoid<T>::val && !IsPointer<T>::val && !IsEnum<T>::val; };
template <typename T> struct SerializeAsFundamental { static constexpr bool val = IsFundamental<T>::val && !IsVoid<T>::val && !IsPointer<T>::val && !IsEnum<T>::val && !IsContainer<T>::val; };
template <typename T> struct SerializeAsObject { static constexpr bool val = IsClass<T>::val && !IsVoid<T>::val && !IsPointer<T>::val && !IsEnum<T>::val && !IsContainer<T>::val && !IsFundamental<T>::val; };

//Container stuff
template <typename T> struct IsStdVector { static constexpr bool val = false; };
template <typename T> struct IsStdVector<std::vector<T>> { static constexpr bool val = true; };
template <typename T> struct StdVectorElement { using Type = void; };
template <typename T> struct StdVectorElement<std::vector<T>> { using Type = T; };

template <typename T> struct IsArray { static constexpr bool val = std::is_array<T>::value; };
template <typename T> struct ArrayElement { using Type = void; };
template <typename T, int U> struct ArrayElement<T[U]> { using Type = T; };
template <typename T> struct ArraySize { static constexpr size_t size = 0; };
template <typename T, int U> struct ArraySize<T[U]> { static constexpr size_t size = U; };

template <typename T> struct IsStdString { static constexpr bool val = false; };
template <> struct IsStdString<std_string> { static constexpr bool val = true; };

//Type stripping
template <typename T> struct Strip { typedef T Type; };
template <typename T> struct Strip<const T> { typedef typename Strip<T>::Type Type; };
template <typename T> struct Strip<T&> { typedef typename Strip<T>::Type Type; };
template <typename T> struct Strip<const T&> { typedef typename Strip<T>::Type Type; };
template <typename T> struct Strip<T&&> { typedef typename Strip<T>::Type Type; };
template <typename T> struct Strip<T*> { typedef typename Strip<T>::Type Type; };
template <typename T> struct Strip<const T*> { typedef typename Strip<T>::Type Type; };

//Container stripping
template <typename T> struct Strip<std::vector<T>> { typedef typename Strip<T>::Type Type; };
template <typename T, int U> struct Strip<T[U]> { typedef typename Strip<T>::Type Type; };

//Strip Reference
template <typename T> struct StripReference { typedef T Type; };
template <typename T> struct StripReference<T&> { typedef typename StripReference<T>::Type Type; };
template <typename T> struct StripReference<const T&> { typedef typename StripReference<T>::Type Type; };
template <typename T> struct StripReference<T&&> { typedef typename StripReference<T>::Type Type; };

//-----------------------------------------
//Utility	
//-----------------------------------------


//Variadic Template Argument type accessor
template <class... Args>
struct get_argument_type
{
	template <std::size_t N>
	using Type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

//Indirection count for pointer types
template <typename T>
size_t GetIndirectionCount(T)
{
	static_assert(IndirectionCount<T>::val < 4, "T**** (or more?) WTF are you doing son?");
	return IndirectionCount<T>::val;
}

std_string GenerateTypeName(const char* in_typeName);

template <typename T> const string_char* GetTypeName()
{
	static std_string type_name = GenerateTypeName(typeid(T).name());
	return type_name.c_str();
}