#pragma once

#include "../Base/Types.h"
#include "../Base/XMLFile.h"
#include "../Base/XMLTag.h"
#include "../Base/Path.h"
#include "../Base/Profiler.h"
#include "../Base/Singleton.h"


/////////////////////////////////////////////////
//GENERIC
using ObjCreationFunction = void*(*)();

//Factory for creation / dynamic type checking
class StreamableObjectFactory : public StaticSingleton<StreamableObjectFactory>
{
	MAKE_STATIC_SINGLETON(StreamableObjectFactory);

	struct StreamableObject
	{
		std_string m_Name;
		ObjCreationFunction pCreationFunc;
		std::vector<StreamableObject*> vChildren;

		bool HasChildNamed(const std_string& in_ChildName);
	};
	
public:
	void* CreateStreamableObject(const std_string& in_objectName, const std_string& in_baseClass);
	void RegisterStreamableObject(const std_string& in_objName, ObjCreationFunction in_creationFunc, const std_string& in_parentName = L(""));

	//TODO CLEANUP 
	std::map<std_string, StreamableObject*> m_mStreamableObjects;
};

extern StreamableObjectFactory s_StreamableObjectFactory;

template<class T>
struct StreamableObjectRegisterer
{
	StreamableObjectRegisterer()
	{
		StreamableObjectFactory::GetInstance()->RegisterStreamableObject(T::GetStreamableClassName(), T::StreamableObjectCreator, T::GetStreamableParentClassName());
	}
};

//Property Class
using EnumStrings = std::vector<std::pair<int, std_string>>;

template<typename Class, typename T>
struct Property 
{
	constexpr Property(T Class::*aMember, const string_char* aName, EnumStrings* aEnumStrings) : member{ aMember }, name{ aName }, enum_strings{ aEnumStrings } {}

	T Class::*member;
	const string_char* name;
	EnumStrings* enum_strings;
};

template<typename Class, typename T>
constexpr auto makeProperty(T Class::*member, const string_char* name, EnumStrings* enumStrings = nullptr)
{
	return Property<Class, T>{member, name, enumStrings};
}

//MACROS
#define DECLARE_PROPERTIES_BEGIN(T) \
	using Type = T; \
	static void* StreamableObjectCreator() { return new Type; } \
	static StreamableObjectRegisterer<Type> ms_StreamableObjectRegisterer; \
	static const string_char* GetStreamableClassName() { return L(#T); } \
	static const string_char* GetStreamableParentClassName() { return L(""); } \
	static auto& streamables() \
	{ \
		static auto streamables_props = std::make_tuple( 

#define DECLARE_DERIVED_PROPERTIES_BEGIN(T, Base) \
	using Type = T; \
	static void* StreamableObjectCreator() { return new Type; } \
	static StreamableObjectRegisterer<Type> ms_StreamableObjectRegisterer; \
	static const string_char* GetStreamableClassName() { return L(#T); } \
	static const string_char* GetStreamableParentClassName() { return L(#Base); } \
	static auto& streamables() \
	{ \
		static auto streamables_props = std::tuple_cat(ShaderConstant::streamables(), \
			std::make_tuple( \


#define ADD_PROPERTY(var) \
			makeProperty(&Type::var, L(#var)) \

#define ADD_ENUM_PROPERTY(var, enumStrings) \
			makeProperty(&Type::var, L(#var), &enumStrings) \

#define DECLARE_PROPERTIES_END() \
		); \
		return streamables_props; \
	} \
	virtual void Deserialize_XML(XMLTag* in_pTag) \
	{ \
		XMLSerializer::setStreamableProperties(std::make_index_sequence<std::tuple_size<std::remove_reference<decltype(Type::streamables())>::type>::value>{}, *this, in_pTag); \
	}

#define DECLARE_DERIVED_PROPERTIES_END() \
		)); \
		return streamables_props; \
	} \
	virtual void Deserialize_XML(XMLTag* in_pTag) \
	{ \
		XMLSerializer::setStreamableProperties(std::make_index_sequence<std::tuple_size<std::remove_reference<decltype(Type::streamables())>::type>::value>{}, *this, in_pTag); \
	}

#define DEFINE_PROPERTIES(T) \
	StreamableObjectRegisterer<T> T::ms_StreamableObjectRegisterer;


//PER-SERIALIZATION STYLE (xml, json, binary, plain text, etc)
class XMLSerializer
{
private:
	template <class T>
	struct ToAny_Impl;

	template<typename T>
	static typename std::enable_if<!std::is_enum<T>::value, void>::type
		ToAny(const XMLTag* in_pTag, const string_char* in_Name, T& in_Object, EnumStrings*)
	{
		ToAny_Impl<T>::f(in_pTag, in_Name, in_Object);
	}

	template<typename T>
	static typename std::enable_if<std::is_enum<T>::value, void>::type
		ToAny(const XMLTag* in_pTag, const string_char* in_Name, T& in_Object, EnumStrings* enum_strings)
	{
		std_string enumValue;
		if (enum_strings == nullptr)
			return;

		if (in_pTag->GetAttribute(in_Name, enumValue) == nullptr)
			return;

		for (auto it : *enum_strings)
		{
			if (it.second == enumValue)
				in_Object = static_cast<T>(it.first);
		}
	}


	//Specialization for Objects : Get the child tag with the proper name and load it as a streamable object
	template <class T>
	struct ToAny_Impl
	{
		static void f(const XMLTag* in_pTag, const string_char* in_Name, T& in_Object)
		{
			PROFILE_BLOCK;

			XMLTag* pTag = in_pTag->FirstChildNamed(in_Name, false);
			if (pTag)
				setStreamableProperties(std::make_index_sequence<std::tuple_size<std::remove_reference<decltype(std::decay_t<T>::streamables())>::type>::value>{}, in_Object, pTag);
		}
	};

	//Specialization for Object Pointers : Get the child tag with the proper name and load it as a streamable object
	template <class T>
	struct ToAny_Impl<T*>
	{
		static void f(const XMLTag* in_pTag, const string_char* in_Name, T*& in_Object)
		{
			PROFILE_BLOCK;

			in_Object = new T;
			ToAny_Impl<T>::f(in_pTag, in_Name, *in_Object);
		}
	};

	//Specialization for vectors of objects : Get the child tag with the vector name and load its children as a streamable objects
	template <class T>
	struct ToAny_Impl<std::vector<T>>
	{
		static void f(const XMLTag* in_pTag, const string_char* in_Name, std::vector<T>& in_Object)
		{
			PROFILE_BLOCK;

			in_Object.clear();

			XMLTag* pVectorTag = in_pTag->FirstChildNamed(in_Name, false);
			if (pVectorTag == nullptr)
				return;

			XMLTag* pObjectTag = pVectorTag->GetChild();
			while (pObjectTag != nullptr)
			{
				AssertMsg(pObjectTag->GetName() == T::GetStreamableClassName(), L("vector<%s> cannot contains an object of type %s"), T::GetStreamableClassName(), pObjectTag->GetName().c_str());

				T p;
				setStreamableProperties(std::make_index_sequence<std::tuple_size<std::remove_reference<decltype(std::decay_t<T>::streamables())>::type>::value>{}, p, pObjectTag);
				in_Object.push_back(p);

				pObjectTag = pObjectTag->GetSibling();
			}
		}
	};


	//Specialization for vectors of object pointers : Get the child tag with the vector name and load its children as a streamable objects
	template <class T>
	struct ToAny_Impl<std::vector<T*>>
	{
		static void f(const XMLTag* in_pTag, const string_char* in_Name, std::vector<T*>& in_Object)
		{
			PROFILE_BLOCK;

			in_Object.clear();

			XMLTag* pVectorTag = in_pTag->FirstChildNamed(in_Name, false);
			if (pVectorTag == nullptr)
				return;

			XMLTag* pObjectTag = pVectorTag->GetChild();
			while (pObjectTag != nullptr)
			{
				std_string tagName = pObjectTag->GetName();

				T* p = (T*)StreamableObjectFactory::GetInstance()->CreateStreamableObject(tagName, T::GetStreamableClassName());
				if (p != nullptr)
				{
					p->Deserialize_XML(pObjectTag);
					in_Object.push_back(p);
				}

				pObjectTag = pObjectTag->GetSibling();
			}
		}
	};


	//Specialization for Path class.  Load as an attribute on the current tag
	template <>
	struct ToAny_Impl<Path>
	{
		static void f(const XMLTag* in_pTag, const string_char* in_Name, Path& in_Object)
		{
			PROFILE_BLOCK;

			in_pTag->GetAttribute(in_Name, in_Object);
		}
	};

	//Specialization for std_string class.  Load as an attribute on the current tag
	template <>
	struct ToAny_Impl<std_string>
	{
		static void f(const XMLTag* in_pTag, const string_char* in_Name, std_string& in_Object)
		{
			PROFILE_BLOCK;

			in_pTag->GetAttribute(in_Name, in_Object);
		}
	};

	//Meta programming template voodoo to get to iterate through the streamables() std::tuple
	template<std::size_t iteration, typename T>
	static void setStreamableProperty(T&& object, const XMLTag* in_pTag)
	{
		PROFILE_BLOCK;

		// get the property
		auto& property = std::get<iteration>(std::decay_t<T>::streamables());

		// set the value to the member
		ToAny(in_pTag, property.name, object.*(property.member), property.enum_strings);
	}

public:

	//Meta programming template voodoo to get to iterate through the streamables() std::tuple
	template<std::size_t... S, typename T>
	static void setStreamableProperties(std::index_sequence<S...>, T&& object, const XMLTag* pTag)
	{
		PROFILE_BLOCK;

		int unpack[] = { (static_cast<void>(XMLSerializer::setStreamableProperty<S>(object, pTag)), 0)..., 0 };
		(void)unpack;
	}

	template<typename T>
	static T Deserialize(const Path& in_file)
	{
		T object;
		Deserialize(&object, in_file);
		return object;
	}

	template<typename T>
	static T& Deserialize(T& in_object, const Path& in_file)
	{
		Deserialize(&in_object, in_file);
		return in_object;
	}

	template<typename T>
	static T* Deserialize(T** in_object, const Path& in_file)
	{
		*in_object = new T;
		Deserialize(*in_object, in_file);
		return *in_object;
	}

	template<typename T>
	static T* Deserialize(T* in_object, const Path& in_file)
	{
		PROFILE_BLOCK;

		XMLFile xmlfile;
		if (!xmlfile.Parse(in_file))
			return false;

		XMLTag* pRoot = xmlfile.GetRoot();

		if (pRoot->GetName() != T::GetStreamableClassName())
			return nullptr;

		setStreamableProperties(std::make_index_sequence<std::tuple_size<std::remove_reference<decltype(std::decay_t<T>::streamables())>::type>::value>{}, *in_object, pRoot);
		return in_object;
	}
};