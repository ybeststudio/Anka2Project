#pragma once

#pragma warning(disable:4091)

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../EterLib/StdAfx.h"
#include "../ScriptLib/StdAfx.h"
#include "../UserInterface/Locale_inc.h"
#include "PythonGraphic.h"
#include "PythonWindowManager.h"

void initgrp();
void initgrpImage();
void initgrpText();
void initgrpThing();
void initscriptWindow();
void initwndMgr();