#pragma once

#pragma warning(disable:4786)

#include <granny.h>

#include "../eterBase/Utils.h"
#include "../eterBase/Debug.h"
#include "../eterBase/Stl.h"

#include "Util.h"

#include "../UserInterface/Locale_inc.h"

#if defined(_M_X64) || defined(_WIN64)
	#ifndef NANOBEGIN
		#define NANOBEGIN
		#define NANOEND
	#endif
#else
	#ifndef NANOBEGIN
		#ifdef __BORLANDC__
			#define NANOBEGIN     __emit__ (0xEB,0x03,0xD6,0xD7,0x01)
			#define NANOEND       __emit__ (0xEB,0x03,0xD6,0xD7,0x00)
		#else
			#define NANOBEGIN     __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD7 __asm _emit 0x01
			#define NANOEND       __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD7 __asm _emit 0x00
		#endif
	#endif
#endif
