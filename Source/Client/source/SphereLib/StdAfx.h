#pragma once

#include "../UserInterface/Locale_inc.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#if defined(DirectX9Ex) || defined(ENABLE_DIRECTX9_UPDATE)
#include <d3d9.h>
#include <d3dx9.h>
#else
#include <d3d8.h>
#include <d3dx8.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "../eterBase/StdAfx.h"
