#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include "../UserInterface/locale_inc.h"
#include <assert.h>
//#include <crtdbg.h>

//#include "Forest.h"
#include "SpeedTreeForestDirectX8.h"
#include <iterator>
#include "../gameLib/StdAfx.h"

// Armadillo nanomite protection
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
