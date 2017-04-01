#pragma once

#include "Types.h"
#include "Assert.h"
#include "StringUtils.h"

//--------------------------------------------------------------------------------
#define MAKE_SINGLETON(CLASS)                       \
    friend class Singleton<CLASS>;                  

#define MAKE_STATIC_SINGLETON(CLASS)                       \
    friend class StaticSingleton<CLASS>;                  

//--------------------------------------------------------------------------------
template<class T> class Singleton
{
public:

	static T* GetInstance()
	{
		//AssertMsg(Singleton<T>::ms_pInstance, L("Instance not created"));
		return ms_pInstance;
	}

	static void DestroyInstance()
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}

	static void CreateInstance()
	{
		AssertMsg(!Singleton<T>::ms_pInstance, L("Instance already created"));
		if (!Singleton<T>::ms_pInstance)
		{
			Singleton<T>::ms_pInstance = new T;
			Singleton<T>::ms_pInstance->Initialize();
		}
	}

protected:
	// Only derived classes can access ctor/dtor
	Singleton() {}
	virtual ~Singleton() {}

	// No copy
	Singleton(const Singleton &);
	void operator =(const Singleton &);

	virtual void Initialize() {};

	//Unique instance
	static T *ms_pInstance;
};

template<class T> T *Singleton<T>::ms_pInstance = 0;



//--------------------------------------------------------------------------------
template<class T> class StaticSingleton
{
public:

	static T* GetInstance()
	{
		static T ms_Instance;
		ms_pInstance = &ms_Instance;
		return &ms_Instance;
	}

protected:
	// Only derived classes can access ctor/dtor
	StaticSingleton() {}
	virtual ~StaticSingleton() {}

	// No copy
	StaticSingleton(const StaticSingleton &);
	void operator=(const StaticSingleton &);

	static T *ms_pInstance;
};

template<class T> T *StaticSingleton<T>::ms_pInstance = 0;

