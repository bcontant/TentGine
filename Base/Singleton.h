#pragma once

#include "StringUtils.h"

#define MAKE_SINGLETON(CLASS)                       \
    friend class Singleton<CLASS>;                  \
    static const StringChar* GET_NAME() { return L(#CLASS); }

template<class T> class Singleton
{
public:

	static T* GetInstance()
	{
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

