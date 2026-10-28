#ifndef __INC_YMIR_GAMELIB__
#define __INC_YMIR_GAMELIB__

//#ifndef WORLD_EDITOR
//#define WORLD_EDITOR
//#endif

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma warning(disable:4710)	// not inlined
#pragma warning(disable:4786)
#pragma warning(disable:4244)	// type conversion possible lose of data

#pragma warning(disable:4018)
#pragma warning(disable:4245)
#pragma warning(disable:4512)
#pragma warning(disable:4201)

#if _MSC_VER >= 1400
	#pragma warning(disable:4201 4512 4238 4239)
#endif

#include "../eterBase/Utils.h"
#include "../eterBase/CRC32.h"
#include "../eterBase/Random.h"

#include "../eterLib/StdAfx.h"
#include "../milesLib/StdAfx.h"
#include "../effectLib/StdAfx.h"

#include "GameType.h"
#include "GameUtil.h"
#include "MapType.h"
#include "MapUtil.h"
#include "Interface.h"

#include "../UserInterface/Locale_inc.h"

#endif