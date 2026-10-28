#pragma once
#include "../UserInterface/Locale_inc.h"
#define WIN32_LEAN_AND_MEAN
#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#define _WIN32_DCOM

#pragma warning(disable:4710)	// not inlined
#pragma warning(disable:4786)
#pragma warning(disable:4244)	// type conversion possible lose of data

#pragma warning(disable:4018)
#pragma warning(disable:4245)
#pragma warning(disable:4512)
#pragma warning(disable:4201)
#pragma warning(disable:4996)

#if _MSC_VER >= 1400
	#pragma warning(disable:4201 4512 4238 4239)
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
#include <d3d9.h>
#include <d3dx9.h>
#else
#include <d3d8.h>
#include <d3dx8.h>
#endif

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#pragma warning ( disable : 4201 )
#include <mmsystem.h>
#pragma warning ( default : 4201 )
#include <process.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <direct.h>
#include <malloc.h>

#pragma comment(lib, "winmm.lib")

#ifdef ENABLE_DIRECTX9_UPDATE
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#else
#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")
#endif

#include "../eterBase/StdAfx.h"
#include "../eterBase/Debug.h"
#include "../eterLocale/CodePageId.h"
#include "../UserInterface/Locale_inc.h"

#ifndef VC_EXTRALEAN
	#include <winsock.h>
#endif

