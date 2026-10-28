#ifndef __INC_ETERLIB_SINGLETON_H__
#define __INC_ETERLIB_SINGLETON_H__

#include <assert.h>
#include <stdio.h>
#include <typeinfo>
#include <cstdint>
#ifdef _WIN32
#include <windows.h>
#endif

template <typename T> class CSingleton
{
	static T * ms_singleton;

public:
	CSingleton()
	{
		assert(!ms_singleton);
		intptr_t offset = (intptr_t) (T*) 1 - (intptr_t) (CSingleton <T>*) (T*) 1;
		ms_singleton = (T*) ((intptr_t) this + offset);
	}

	virtual ~CSingleton()
	{
		assert(ms_singleton);
		ms_singleton = 0;
	}

	__forceinline static T & Instance()
	{
		if (!ms_singleton)
		{
#ifdef _WIN32
			char szMsg[512];
			const char* pTypeName = typeid(T).name();
#ifdef _MSC_VER
			sprintf_s(szMsg, sizeof(szMsg), "CSingleton<%s>::Instance() called before initialization!\nFile: %s\nLine: %d\n", pTypeName, __FILE__, __LINE__);
#else
			snprintf(szMsg, sizeof(szMsg), "CSingleton<%s>::Instance() called before initialization!\nFile: %s\nLine: %d\n", pTypeName, __FILE__, __LINE__);
#endif
			OutputDebugStringA(szMsg);
#endif
			assert(ms_singleton);
		}
		return (*ms_singleton);
	}

	__forceinline static T * InstancePtr()
	{
		return (ms_singleton);
	}

	__forceinline static T & instance()
	{
		return Instance();
	}
};

template <typename T> T * CSingleton <T>::ms_singleton = 0;

template <typename T> class singleton
{
	static T * ms_singleton;

public:
	singleton()
	{
		assert(!ms_singleton);
		intptr_t offset = (intptr_t) (T*) 1 - (intptr_t) (CSingleton <T>*) (T*) 1;
		ms_singleton = (T*) ((intptr_t) this + offset);
	}

	virtual ~singleton()
	{
		assert(ms_singleton);
		ms_singleton = 0;
	}

	__forceinline static T & Instance()
	{
		if (!ms_singleton)
		{
#ifdef _WIN32
			char szMsg[512];
			const char* pTypeName = typeid(T).name();
#ifdef _MSC_VER
			sprintf_s(szMsg, sizeof(szMsg), "singleton<%s>::Instance() called before initialization!\nFile: %s\nLine: %d\n", pTypeName, __FILE__, __LINE__);
#else
			snprintf(szMsg, sizeof(szMsg), "singleton<%s>::Instance() called before initialization!\nFile: %s\nLine: %d\n", pTypeName, __FILE__, __LINE__);
#endif
			OutputDebugStringA(szMsg);
#endif
			assert(ms_singleton);
		}
		return (*ms_singleton);
	}

	__forceinline static T * InstancePtr()
	{
		return (ms_singleton);
	}

	__forceinline static T & instance()
	{
		return Instance();
	}
};

template <typename T> T * singleton <T>::ms_singleton = 0;

#endif
