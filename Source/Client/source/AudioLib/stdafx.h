#pragma once

#include <windows.h>
#include <objbase.h>  // For CoInitializeEx

#pragma warning(disable:4786)
#pragma warning(disable:4100)

#include "../eterBase/CRC32.h"
#include "../eterBase/Utils.h"
#include "../eterBase/Debug.h"

#include <algorithm>
#include <cassert>
#ifndef MD_ASSERT
	#define MD_ASSERT(x) ((x) ? true : (assert(false), false))
#endif
