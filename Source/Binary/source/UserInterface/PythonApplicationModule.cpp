#include "StdAfx.h"
#include "Resource.h"
#include <Shellapi.h>
#include "PythonApplication.h"

#include "../EterLib/Camera.h"

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	#include "PythonDynamicModuleNames.h"
#endif

extern D3DXCOLOR g_fSpecularColor;
extern BOOL bVisibleNotice = true;
extern BOOL bTestServerFlag = FALSE;
extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE = 0;

PyObject *appShowWebPage(PyObject *poSelf, PyObject *poArgs)
{
	char *szWebPage;
	if (!PyTuple_GetString(poArgs, 0, &szWebPage))
		return Py_BuildException();

	PyObject *poRect=PyTuple_GetItem(poArgs, 1);
	if (!PyTuple_Check(poRect))
		return Py_BuildException();

	RECT rcWebPage;
	rcWebPage.left=PyInt_AsLong(PyTuple_GetItem(poRect, 0));
	rcWebPage.top=PyInt_AsLong(PyTuple_GetItem(poRect, 1));
	rcWebPage.right=PyInt_AsLong(PyTuple_GetItem(poRect, 2));
	rcWebPage.bottom=PyInt_AsLong(PyTuple_GetItem(poRect, 3));

	CPythonApplication::Instance().ShowWebPage(szWebPage, rcWebPage);
	return Py_BuildNone();
}

PyObject *appMoveWebPage(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *poRect=PyTuple_GetItem(poArgs, 0);
	if (!PyTuple_Check(poRect))
		return Py_BuildException();

	RECT rcWebPage;
	rcWebPage.left=PyInt_AsLong(PyTuple_GetItem(poRect, 0));
	rcWebPage.top=PyInt_AsLong(PyTuple_GetItem(poRect, 1));
	rcWebPage.right=PyInt_AsLong(PyTuple_GetItem(poRect, 2));
	rcWebPage.bottom=PyInt_AsLong(PyTuple_GetItem(poRect, 3));

	CPythonApplication::Instance().MoveWebPage(rcWebPage);
	return Py_BuildNone();
}

PyObject *appHideWebPage(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().HideWebPage();
	return Py_BuildNone();
}

PyObject *appIsWebPageMode(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().IsWebPageMode());
}

extern BOOL HAIR_COLOR_ENABLE;
extern BOOL USE_ARMOR_SPECULAR;
extern BOOL USE_WEAPON_SPECULAR;
extern BOOL SKILL_EFFECT_UPGRADE_ENABLE;
extern BOOL RIDE_HORSE_ENABLE;
extern double g_specularSpd;

extern void TextTail_SetLivingTime(long livingTime);

PyObject *appSetTextTailLivingTime(PyObject *poSelf, PyObject *poArgs)
{
	float livingTime;
	if (!PyTuple_GetFloat(poArgs, 0, &livingTime))
		return Py_BuildException();

	TextTail_SetLivingTime(livingTime*1000);

	return Py_BuildNone();
}

PyObject *appSetHairColorEnable(PyObject *poSelf, PyObject *poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	HAIR_COLOR_ENABLE=nEnable;

	return Py_BuildNone();
}

PyObject *appSetArmorSpecularEnable(PyObject *poSelf, PyObject *poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	USE_ARMOR_SPECULAR=nEnable;

	return Py_BuildNone();
}

PyObject *appSetWeaponSpecularEnable(PyObject *poSelf, PyObject *poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	USE_WEAPON_SPECULAR=nEnable;

	return Py_BuildNone();
}

PyObject *appSetSkillEffectUpgradeEnable(PyObject *poSelf, PyObject *poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	SKILL_EFFECT_UPGRADE_ENABLE=nEnable;

	return Py_BuildNone();
}

PyObject *SetTwoHandedWeaponAttSpeedDecreaseValue(PyObject *poSelf, PyObject *poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE = iValue;

	return Py_BuildNone();
}

PyObject *appSetRideHorseEnable(PyObject *poSelf, PyObject *poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	RIDE_HORSE_ENABLE=nEnable;

	return Py_BuildNone();
}

PyObject *appSetCameraMaxDistance(PyObject *poSelf, PyObject *poArgs)
{
	float fMax;
	if (!PyTuple_GetFloat(poArgs, 0, &fMax))
		return Py_BuildException();

	CCamera::SetCameraMaxDistance(fMax);
	return Py_BuildNone();
}

PyObject *appSetControlFP(PyObject *poSelf, PyObject *poArgs)
{
	_controlfp( _PC_24, _MCW_PC );
	return Py_BuildNone();
}

PyObject *appSetSpecularSpeed(PyObject *poSelf, PyObject *poArgs)
{
	float fSpeed;
	if (!PyTuple_GetFloat(poArgs, 0, &fSpeed))
		return Py_BuildException();

	g_specularSpd = fSpeed;

	return Py_BuildNone();
}

PyObject *appSetMinFog(PyObject *poSelf, PyObject *poArgs)
{
	float fMinFog;
	if (!PyTuple_GetFloat(poArgs, 0, &fMinFog))
		return Py_BuildException();

	CPythonApplication::Instance().SetMinFog(fMinFog);
	return Py_BuildNone();
}

PyObject *appSetFrameSkip(PyObject *poSelf, PyObject *poArgs)
{
	int nFrameSkip;
	if (!PyTuple_GetInteger(poArgs, 0, &nFrameSkip))
		return Py_BuildException();

	CPythonApplication::Instance().SetFrameSkip(nFrameSkip ? true : false);
	return Py_BuildNone();
}

bool LoadLocaleData(const char *localePath);

#include "../eterBase/tea.h"

PyObject *appLoadLocaleAddr(PyObject *poSelf, PyObject *poArgs)
{
	char *addrPath;
	if (!PyTuple_GetString(poArgs, 0, &addrPath))
		return Py_BuildException();

	FILE* fp = fopen(addrPath, "rb");
	if (!fp)
		return Py_BuildException();

	fseek(fp, 0, SEEK_END);

	int size = ftell(fp);
	char *enc = (char *)_alloca(size);
	fseek(fp, 0, SEEK_SET);
	fread(enc, size, 1, fp);
	fclose(fp);

	static const unsigned char key[16] = {
		0x82, 0x1b, 0x34, 0xae,
		0x12, 0x3b, 0xfb, 0x17,
		0xd7, 0x2c, 0x39, 0xae,
		0x41, 0x98, 0xf1, 0x63
	};

	char *buf = (char *)_alloca(size);

	tea_decrypt((unsigned long*)buf, (const unsigned long*)enc, (const unsigned long*)key, size);
	unsigned int retSize = *(unsigned int*)buf;
	char *ret = buf + sizeof(unsigned int);
	return Py_BuildValue("s#", ret, retSize);
}

PyObject *appGetLocaleName(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("s", LocaleService_GetLocaleName());
}

PyObject *appLoadLocaleData(PyObject *poSelf, PyObject *poArgs)
{
	char *localePath;
	if (!PyTuple_GetString(poArgs, 0, &localePath))
		return Py_BuildException();

	return Py_BuildValue("i", LoadLocaleData(localePath));
}

PyObject *appGetLocalePath(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("s", LocaleService_GetLocalePath());
}

PyObject *appGetDefaultCodePage(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", GetCodePage());
}

#include <il/il.h>

PyObject *appGetImageInfo(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	BOOL canLoad=FALSE;
	ILuint uWidth=0;
	ILuint uHeight=0;

	ILuint uImg;
	ilGenImages(1, &uImg);
	ilBindImage(uImg);
	if (ilLoad(IL_TYPE_UNKNOWN, szFileName))
	{
		canLoad=TRUE;
		uWidth=ilGetInteger(IL_IMAGE_WIDTH);
		uHeight=ilGetInteger(IL_IMAGE_HEIGHT);
	}

	ilDeleteImages(1, &uImg);

	return Py_BuildValue("iii", canLoad, uWidth, uHeight);
}

#include "../EterPack/EterPackManager.h"

PyObject *appIsExistFile(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	bool isExist=CEterPackManager::Instance().isExist(szFileName);

	return Py_BuildValue("i", isExist);
}

PyObject *appGetFileList(PyObject *poSelf, PyObject *poArgs)
{
	char *szFilter;
	if (!PyTuple_GetString(poArgs, 0, &szFilter))
		return Py_BuildException();

	PyObject *poList=PyList_New(0);

	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(wfd));

	HANDLE hFind = FindFirstFile(szFilter, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			PyObject *poFileName=PyString_FromString(wfd.cFileName) ;
			PyList_Append(poList, poFileName);
		}
		while (FindNextFile(hFind, &wfd));

		FindClose(hFind);
	}

	return poList;
}

PyObject *appUpdateGame(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().UpdateGame();
	return Py_BuildNone();
}

PyObject *appRenderGame(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().RenderGame();
	return Py_BuildNone();
}



PyObject *appSetMouseHandler(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonApplication::Instance().SetMouseHandler(poHandler);
	return Py_BuildNone();
}

PyObject *appCreate(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	int width;
	if (!PyTuple_GetInteger(poArgs, 1, &width))
		return Py_BuildException();

	int height;
	if (!PyTuple_GetInteger(poArgs, 2, &height))
		return Py_BuildException();

	int Windowed;
	if (!PyTuple_GetInteger(poArgs, 3, &Windowed))
		return Py_BuildException();

	CPythonApplication& rkApp = CPythonApplication::Instance();

	if (!rkApp.Create(poSelf, APP_TITLE, width, height, Windowed))
		return NULL;

	return Py_BuildNone();
}

PyObject *appLoop(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().Loop();
		
	return Py_BuildNone();
}

PyObject *appGetInfo(PyObject *poSelf, PyObject *poArgs)
{
	int nInfo;
	if (!PyTuple_GetInteger(poArgs, 0, &nInfo))
		return Py_BuildException();

	std::string stInfo;
	CPythonApplication::Instance().GetInfo(nInfo, &stInfo);
	return Py_BuildValue("s", stInfo.c_str());
}

PyObject *appProcess(PyObject *poSelf, PyObject *poArgs)
{
	if (CPythonApplication::Instance().Process())
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}

PyObject *appAbort(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().Abort();
	return Py_BuildNone();
}

PyObject *appExit(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().Exit();
	return Py_BuildNone();
}

PyObject *appSetCamera(PyObject *poSelf, PyObject *poArgs)
{
	float Distance;
	if (!PyTuple_GetFloat(poArgs, 0, &Distance))
		return Py_BuildException();

	float Pitch;
	if (!PyTuple_GetFloat(poArgs, 1, &Pitch))
		return Py_BuildException();

	float Rotation;
	if (!PyTuple_GetFloat(poArgs, 2, &Rotation))
		return Py_BuildException();

	float fDestinationHeight;
	if (!PyTuple_GetFloat(poArgs, 3, &fDestinationHeight))
		return Py_BuildException();

	CPythonApplication::Instance().SetCamera(Distance, Pitch, Rotation, fDestinationHeight);
	return Py_BuildNone();
}

PyObject *appGetCamera(PyObject *poSelf, PyObject *poArgs)
{
	float Distance, Pitch, Rotation, DestinationHeight;
	CPythonApplication::Instance().GetCamera(&Distance, &Pitch, &Rotation, &DestinationHeight);

	return Py_BuildValue("ffff", Distance, Pitch, Rotation, DestinationHeight);
}

PyObject *appGetCameraPitch(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetPitch());
}

PyObject *appGetCameraRotation(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetRotation());
}

PyObject *appGetTime(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetGlobalTime());
}

PyObject *appGetGlobalTime(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetServerTime());
}

PyObject *appGetGlobalTimeStamp(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetServerTimeStamp());
}

PyObject *appGetUpdateFPS(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetUpdateFPS());
}

PyObject *appGetRenderFPS(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetRenderFPS());
}

PyObject *appRotateCamera(PyObject *poSelf, PyObject *poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().RotateCamera(iDirection);
	return Py_BuildNone();
}

PyObject *appPitchCamera(PyObject *poSelf, PyObject *poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().PitchCamera(iDirection);
	return Py_BuildNone();
}

PyObject *appZoomCamera(PyObject *poSelf, PyObject *poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().ZoomCamera(iDirection);
	return Py_BuildNone();
}

PyObject *appMovieRotateCamera(PyObject *poSelf, PyObject *poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MovieRotateCamera(iDirection);
	return Py_BuildNone();
}

PyObject *appMoviePitchCamera(PyObject *poSelf, PyObject *poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MoviePitchCamera(iDirection);
	return Py_BuildNone();
}

PyObject *appMovieZoomCamera(PyObject *poSelf, PyObject *poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MovieZoomCamera(iDirection);
	return Py_BuildNone();
}

PyObject *appMovieResetCamera(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().MovieResetCamera();
	return Py_BuildNone();
}

PyObject *appGetFaceSpeed(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetFaceSpeed());
}

PyObject *appGetRenderTime(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("fi", 
		CPythonApplication::Instance().GetAveRenderTime(),
		CPythonApplication::Instance().GetCurRenderTime());
}

PyObject *appGetUpdateTime(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetCurUpdateTime());
}

PyObject *appGetLoad(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetLoad());
}
PyObject *appGetFaceCount(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetFaceCount());
}

PyObject *appGetAvaiableTextureMememory(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CGraphicBase::GetAvailableTextureMemory());
}

PyObject *appSetFPS(PyObject *poSelf, PyObject *poArgs)
{
	int	iFPS;
	if (!PyTuple_GetInteger(poArgs, 0, &iFPS))
		return Py_BuildException();

	CPythonApplication::Instance().SetFPS(iFPS);

	return Py_BuildNone();
}

PyObject *appSetGlobalCenterPosition(PyObject *poSelf, PyObject *poArgs)
{
	int x;
	if (!PyTuple_GetInteger(poArgs, 0, &x))
		return Py_BuildException();

	int y;
	if (!PyTuple_GetInteger(poArgs, 1, &y))
		return Py_BuildException();

	CPythonApplication::Instance().SetGlobalCenterPosition(x, y);
	return Py_BuildNone();
}


PyObject *appSetCenterPosition(PyObject *poSelf, PyObject *poArgs)
{
	float fx;
	if (!PyTuple_GetFloat(poArgs, 0, &fx))
		return Py_BuildException();

	float fy;
	if (!PyTuple_GetFloat(poArgs, 1, &fy))
		return Py_BuildException();

	float fz;
	if (!PyTuple_GetFloat(poArgs, 2, &fz))
		return Py_BuildException();

	CPythonApplication::Instance().SetCenterPosition(fx, -fy, fz);
	return Py_BuildNone();
}

PyObject *appGetCursorPosition(PyObject *poSelf, PyObject *poArgs)
{
	long lx, ly;
	UI::CWindowManager& rkWndMgr=UI::CWindowManager::Instance();
	rkWndMgr.GetMousePosition(lx, ly);

	return Py_BuildValue("ii", lx, ly);
}

PyObject *appRunPythonFile(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	bool ret = CPythonLauncher::Instance().RunFile(szFileName);
	return Py_BuildValue("i", ret);
}

PyObject *appIsPressed(PyObject *poSelf, PyObject *poArgs)
{
	int iKey;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonApplication::Instance().IsPressed(iKey));
}

PyObject *appSetCursor(PyObject *poSelf, PyObject *poArgs)
{
	int iCursorNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iCursorNum))
		return Py_BuildException();

	if (!CPythonApplication::Instance().SetCursorNum(iCursorNum))
		return Py_BuildException("Wrong Cursor Name [%d]", iCursorNum);

	return Py_BuildNone();
}

PyObject *appGetCursor(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetCursorNum());
}

PyObject *appShowCursor(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().SetCursorVisible(TRUE);

	return Py_BuildNone();
}

PyObject *appHideCursor(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().SetCursorVisible(FALSE);

	return Py_BuildNone();
}

PyObject *appIsShowCursor(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", TRUE == CPythonApplication::Instance().GetCursorVisible());
}

PyObject *appIsLiarCursorOn(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", TRUE == CPythonApplication::Instance().GetLiarCursorOn());
}

PyObject *appSetSoftwareCursor(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_SOFTWARE);
	return Py_BuildNone();
}

PyObject *appSetHardwareCursor(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_HARDWARE);
	return Py_BuildNone();
}

PyObject *appSetConnectData(PyObject *poSelf, PyObject *poArgs)
{
	char *szIP;
	if (!PyTuple_GetString(poArgs, 0, &szIP))
		return Py_BuildException();

	int	iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
		return Py_BuildException();

	CPythonApplication::Instance().SetConnectData(szIP, iPort);

	return Py_BuildNone();
}

PyObject *appGetConnectData(PyObject *poSelf, PyObject *poArgs)
{
	std::string strIP;
	int iPort;

	CPythonApplication::Instance().GetConnectData(strIP, iPort);

	return Py_BuildValue("si", strIP.c_str(), iPort);
}

PyObject *appGetRandom(PyObject *poSelf, PyObject *poArgs)
{
	int from;
	if (!PyTuple_GetInteger(poArgs, 0, &from))
		return Py_BuildException();

	int	to;
	if (!PyTuple_GetInteger(poArgs, 1, &to))
		return Py_BuildException();

	if (from > to)
	{
		int tmp = from;
		from = to;
		to = tmp;
	}

	return Py_BuildValue("i", random_range(from, to));
}

PyObject *appGetRotatingDirection(PyObject *poSelf, PyObject *poArgs)
{
	float fSource;
	if (!PyTuple_GetFloat(poArgs, 0, &fSource))
		return Py_BuildException();
	float fTarget;
	if (!PyTuple_GetFloat(poArgs, 1, &fTarget))
		return Py_BuildException();

	return Py_BuildValue("i", GetRotatingDirection(fSource, fTarget));
}

PyObject *appGetDegreeDifference(PyObject *poSelf, PyObject *poArgs)
{
	float fSource;
	if (!PyTuple_GetFloat(poArgs, 0, &fSource))
		return Py_BuildException();
	float fTarget;
	if (!PyTuple_GetFloat(poArgs, 1, &fTarget))
		return Py_BuildException();

	return Py_BuildValue("f", GetDegreeDifference(fSource, fTarget));
}

PyObject *appSleep(PyObject *poSelf, PyObject *poArgs)
{
	int	iTime;
	if (!PyTuple_GetInteger(poArgs, 0, &iTime))
		return Py_BuildException();

	Sleep(iTime);

	return Py_BuildNone();
}

PyObject *appSetDefaultFontName(PyObject *poSelf, PyObject *poArgs)
{
	char *szFontName;
	if (!PyTuple_GetString(poArgs, 0, &szFontName))
		return Py_BuildException();

	DefaultFont_SetName(szFontName);

	return Py_BuildNone();
}

PyObject *appSetGuildSymbolPath(PyObject *poSelf, PyObject *poArgs)
{
	char *szPathName;
	if (!PyTuple_GetString(poArgs, 0, &szPathName))
		return Py_BuildException();

	SetGuildSymbolPath(szPathName);

	return Py_BuildNone();
}

PyObject *appEnableSpecialCameraMode(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().EnableSpecialCameraMode();
	return Py_BuildNone();
}

PyObject *appSetCameraSpeed(PyObject *poSelf, PyObject *poArgs)
{
	int iPercentage;
	if (!PyTuple_GetInteger(poArgs, 0, &iPercentage))
		return Py_BuildException();

	CPythonApplication::Instance().SetCameraSpeed(iPercentage);

	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();
	if (pCamera)
		pCamera->SetResistance(float(iPercentage) / 100.0f);
	return Py_BuildNone();
}

PyObject *appIsFileExist(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	return Py_BuildValue("i", -1 != _access(szFileName, 0));
}

PyObject *appSetCameraSetting(PyObject *poSelf, PyObject *poArgs)
{
	int ix;
	if (!PyTuple_GetInteger(poArgs, 0, &ix))
		return Py_BuildException();
	int iy;
	if (!PyTuple_GetInteger(poArgs, 1, &iy))
		return Py_BuildException();
	int iz;
	if (!PyTuple_GetInteger(poArgs, 2, &iz))
		return Py_BuildException();

	int iZoom;
	if (!PyTuple_GetInteger(poArgs, 3, &iZoom))
		return Py_BuildException();
	int iRotation;
	if (!PyTuple_GetInteger(poArgs, 4, &iRotation))
		return Py_BuildException();
	int iPitch;
	if (!PyTuple_GetInteger(poArgs, 5, &iPitch))
		return Py_BuildException();

	CPythonApplication::SCameraSetting CameraSetting;
	ZeroMemory(&CameraSetting, sizeof(CameraSetting));
	CameraSetting.v3CenterPosition.x = float(ix);
	CameraSetting.v3CenterPosition.y = float(iy);
	CameraSetting.v3CenterPosition.z = float(iz);
	CameraSetting.fZoom = float(iZoom);
	CameraSetting.fRotation = float(iRotation);
	CameraSetting.fPitch = float(iPitch);
	CPythonApplication::Instance().SetEventCamera(CameraSetting);
	return Py_BuildNone();
}

PyObject *appSaveCameraSetting(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonApplication::Instance().SaveCameraSetting(szFileName);
	return Py_BuildNone();
}

PyObject *appLoadCameraSetting(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	bool bResult = CPythonApplication::Instance().LoadCameraSetting(szFileName);
	return Py_BuildValue("i", bResult);
}

PyObject *appSetDefaultCamera(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().SetDefaultCamera();
	return Py_BuildNone();
}

PyObject *appSetSightRange(PyObject *poSelf, PyObject *poArgs)
{
	int iRange;
	if (!PyTuple_GetInteger(poArgs, 0, &iRange))
		return Py_BuildException();

	CPythonApplication::Instance().SetForceSightRange(iRange);
	return Py_BuildNone();
}

extern int g_iAccumulationTime;

PyObject *apptestGetAccumulationTime(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", g_iAccumulationTime);
}

PyObject *apptestResetAccumulationTime(PyObject *poSelf, PyObject *poArgs)
{
	g_iAccumulationTime = 0;
	return Py_BuildNone();
}

PyObject *apptestSetSpecularColor(PyObject *poSelf, PyObject *poArgs)
{
	float fr;
	if (!PyTuple_GetFloat(poArgs, 0, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 1, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 2, &fb))
		return Py_BuildException();
	g_fSpecularColor = D3DXCOLOR(fr, fg, fb, 1.0f);
	return Py_BuildNone();
}

PyObject *appSetVisibleNotice(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	bVisibleNotice = iFlag;
	return Py_BuildNone();
}

PyObject *appIsVisibleNotice(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", bVisibleNotice);
}

PyObject *appEnableTestServerFlag(PyObject *poSelf, PyObject *poArgs)
{
	bTestServerFlag = TRUE;
	return Py_BuildNone();
}

PyObject *appIsEnableTestServerFlag(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", bTestServerFlag);
}

class CTextLineLoader
{
	public:
		CTextLineLoader(const char *c_szFileName)
		{
			const VOID* pvData;
			CMappedFile kFile;
			if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
				return;

			m_kTextFileLoader.Bind(kFile.Size(), pvData);
		}

		DWORD GetLineCount()
		{
			return m_kTextFileLoader.GetLineCount();
		}

		const char *GetLine(DWORD dwIndex)
		{
			if (dwIndex >= GetLineCount())
				return "";

			return m_kTextFileLoader.GetLineString(dwIndex).c_str();
		}

	protected:
		CMemoryTextFileLoader m_kTextFileLoader;
};

PyObject *appOpenTextFile(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CTextLineLoader * pTextLineLoader = new CTextLineLoader(szFileName);

	return Py_BuildValue("i", (int)pTextLineLoader);
}

PyObject *appCloseTextFile(PyObject *poSelf, PyObject *poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();

	CTextLineLoader * pTextFileLoader = (CTextLineLoader *)iHandle;
	delete pTextFileLoader;

	return Py_BuildNone();
}

PyObject *appGetTextFileLineCount(PyObject *poSelf, PyObject *poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();

	CTextLineLoader * pTextFileLoader = (CTextLineLoader *)iHandle;
	return Py_BuildValue("i", pTextFileLoader->GetLineCount());
}

PyObject *appGetTextFileLine(PyObject *poSelf, PyObject *poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();
	int iLineIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iLineIndex))
		return Py_BuildException();

	CTextLineLoader * pTextFileLoader = (CTextLineLoader *)iHandle;
	return Py_BuildValue("s", pTextFileLoader->GetLine(iLineIndex));
}

PyObject *appSetGuildMarkPath(PyObject *poSelf, PyObject *poArgs)
{
	char *path;
	if (!PyTuple_GetString(poArgs, 0, &path))
		return Py_BuildException();

	char newPath[256];
	char *ext = strstr(path, ".tga");

	if (ext)
	{
		int extPos = ext - path;
		strncpy(newPath, path, extPos);
		newPath[extPos] = '\0';
	}
	else
		strncpy(newPath, path, sizeof(newPath)-1);

	CGuildMarkManager::Instance().SetMarkPathPrefix(newPath);
	return Py_BuildNone();
}

PyObject *appLogoOpen(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	int nIsSuccess = 1;

	return Py_BuildValue("i", nIsSuccess);
}

PyObject *appLogoUpdate(PyObject *poSelf, PyObject *poArgs)
{
	int nIsRun = 0;
	return Py_BuildValue("i", nIsRun);
}

PyObject *appLogoRender(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildNone();
}

PyObject *appLogoClose(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildNone();
}

PyObject *appShellExecute(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();
	ShellExecuteA(0, "open", szName, 0, 0, SW_SHOWNORMAL);
	CPythonApplication::Instance().Exit();
	return Py_BuildNone();
}

PyObject *appOpenWebBrowser(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();
	ShellExecuteA(0, "open", szName, 0, 0, SW_SHOWNORMAL);
	return Py_BuildNone();
}

PyObject *appGetName(PyObject *poSelf, PyObject *poArgs)
{
	TCHAR szAppName[MAX_PATH];
	GetModuleFileName(nullptr, szAppName, MAX_PATH);
	return Py_BuildValue("s", szAppName);
}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
PyObject *appSetLocale(PyObject *poSelf, PyObject *poArgs)
{
	int iCodePage;
	if (!PyTuple_GetInteger(poArgs, 0, &iCodePage))
		return Py_BuildException();

	char *szLocale;
	if (!PyTuple_GetString(poArgs, 1, &szLocale))
		return Py_BuildException();

	return Py_BuildValue("i", LocaleService_SaveLocale(iCodePage, szLocale));
}

PyObject *appGetLocale(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("s", LocaleService_GetLocale());
}
#endif

#ifdef ENABLE_BLINK_ALERT
PyObject *appFlashApplication(PyObject *poSelf, PyObject *poArgs)
{
	CPythonApplication::Instance().FlashApplication();
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
PyObject *appSetMultiFarmExeIcon(PyObject *poSelf, PyObject *poArgs)
{
	int iStatus;
	if (!PyTuple_GetInteger(poArgs, 0, &iStatus))
		return Py_BuildException();
	CPythonApplication::Instance().MultiFarmBlockIcon(iStatus);
	return Py_BuildNone();
}
#endif

void initapp()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetTextTailLivingTime",		appSetTextTailLivingTime,		METH_VARARGS },

		{ "SetHairColorEnable",			appSetHairColorEnable,			METH_VARARGS },

		{ "SetArmorSpecularEnable",		appSetArmorSpecularEnable,		METH_VARARGS },
		{ "SetWeaponSpecularEnable",	appSetWeaponSpecularEnable,		METH_VARARGS },
		{ "SetSkillEffectUpgradeEnable",appSetSkillEffectUpgradeEnable,	METH_VARARGS },
		{ "SetTwoHandedWeaponAttSpeedDecreaseValue", SetTwoHandedWeaponAttSpeedDecreaseValue, METH_VARARGS },
		{ "SetRideHorseEnable",			appSetRideHorseEnable,			METH_VARARGS },

		{ "SetCameraMaxDistance",		appSetCameraMaxDistance,		METH_VARARGS },
		{ "SetMinFog",					appSetMinFog,					METH_VARARGS },
		{ "SetFrameSkip",				appSetFrameSkip,				METH_VARARGS },
		{ "GetImageInfo",				appGetImageInfo,				METH_VARARGS },
		{ "GetInfo",					appGetInfo,						METH_VARARGS },
		{ "UpdateGame",					appUpdateGame,					METH_VARARGS },
		{ "RenderGame",					appRenderGame,					METH_VARARGS },
		{ "Loop",						appLoop,						METH_VARARGS },
		{ "Create",						appCreate,						METH_VARARGS },
		{ "Process",					appProcess,						METH_VARARGS },
		{ "Exit",						appExit,						METH_VARARGS },
		{ "Abort",						appAbort,						METH_VARARGS },
		{ "SetMouseHandler",			appSetMouseHandler,				METH_VARARGS },
		{ "IsExistFile",				appIsExistFile,					METH_VARARGS },
		{ "GetFileList",				appGetFileList,					METH_VARARGS },

		{ "SetCamera",					appSetCamera,					METH_VARARGS },
		{ "GetCamera",					appGetCamera,					METH_VARARGS },
		{ "GetCameraPitch",				appGetCameraPitch,				METH_VARARGS },
		{ "GetCameraRotation",			appGetCameraRotation,			METH_VARARGS },
		{ "GetTime",					appGetTime,						METH_VARARGS },
		{ "GetGlobalTime",				appGetGlobalTime,				METH_VARARGS },
		{ "GetGlobalTimeStamp",			appGetGlobalTimeStamp,			METH_VARARGS },
		{ "GetUpdateFPS",				appGetUpdateFPS,				METH_VARARGS },
		{ "GetRenderFPS",				appGetRenderFPS,				METH_VARARGS },
		{ "RotateCamera",				appRotateCamera,				METH_VARARGS },
		{ "PitchCamera",				appPitchCamera,					METH_VARARGS },
		{ "ZoomCamera",					appZoomCamera,					METH_VARARGS },
		{ "MovieRotateCamera",			appMovieRotateCamera,			METH_VARARGS },
		{ "MoviePitchCamera",			appMoviePitchCamera,			METH_VARARGS },
		{ "MovieZoomCamera",			appMovieZoomCamera,				METH_VARARGS },
		{ "MovieResetCamera",			appMovieResetCamera,			METH_VARARGS },

		{ "GetAvailableTextureMemory",	appGetAvaiableTextureMememory,	METH_VARARGS },
		{ "GetRenderTime",				appGetRenderTime,				METH_VARARGS },
		{ "GetUpdateTime",				appGetUpdateTime,				METH_VARARGS },
		{ "GetLoad",					appGetLoad,						METH_VARARGS },
		{ "GetFaceSpeed",				appGetFaceSpeed,				METH_VARARGS },
		{ "GetFaceCount",				appGetFaceCount,				METH_VARARGS },
		{ "SetFPS",						appSetFPS,						METH_VARARGS },
		{ "SetGlobalCenterPosition",	appSetGlobalCenterPosition,		METH_VARARGS },
		{ "SetCenterPosition",			appSetCenterPosition,			METH_VARARGS },
		{ "GetCursorPosition",			appGetCursorPosition,			METH_VARARGS },

		{ "GetRandom",					appGetRandom,					METH_VARARGS },
		{ "RunPythonFile",				appRunPythonFile,				METH_VARARGS },
		{ "IsWebPageMode",				appIsWebPageMode,				METH_VARARGS },
		{ "ShowWebPage",				appShowWebPage,					METH_VARARGS },
		{ "MoveWebPage",				appMoveWebPage,					METH_VARARGS },
		{ "HideWebPage",				appHideWebPage,					METH_VARARGS },
		{ "IsPressed",					appIsPressed,					METH_VARARGS },
		{ "SetCursor",					appSetCursor,					METH_VARARGS },
		{ "GetCursor",					appGetCursor,					METH_VARARGS },
		{ "ShowCursor",					appShowCursor,					METH_VARARGS },
		{ "HideCursor",					appHideCursor,					METH_VARARGS },
		{ "IsShowCursor",				appIsShowCursor,				METH_VARARGS },
		{ "IsLiarCursorOn",				appIsLiarCursorOn,				METH_VARARGS },
		{ "SetSoftwareCursor",			appSetSoftwareCursor,			METH_VARARGS },
		{ "SetHardwareCursor",			appSetHardwareCursor,			METH_VARARGS },

		{ "SetConnectData",				appSetConnectData,				METH_VARARGS },
		{ "GetConnectData",				appGetConnectData,				METH_VARARGS },

		{ "GetRotatingDirection",		appGetRotatingDirection,		METH_VARARGS },
		{ "GetDegreeDifference",		appGetDegreeDifference,			METH_VARARGS },
		{ "Sleep",						appSleep,						METH_VARARGS },
		{ "SetDefaultFontName",			appSetDefaultFontName,			METH_VARARGS },
		{ "SetGuildSymbolPath",			appSetGuildSymbolPath,			METH_VARARGS },

		{ "EnableSpecialCameraMode",	appEnableSpecialCameraMode,		METH_VARARGS },
		{ "SetCameraSpeed",				appSetCameraSpeed,				METH_VARARGS },

		{ "SaveCameraSetting",			appSaveCameraSetting,			METH_VARARGS },
		{ "LoadCameraSetting",			appLoadCameraSetting,			METH_VARARGS },
		{ "SetDefaultCamera",			appSetDefaultCamera,			METH_VARARGS },
		{ "SetCameraSetting",			appSetCameraSetting,			METH_VARARGS },

		{ "SetSightRange",				appSetSightRange,				METH_VARARGS },

		{ "IsFileExist",				appIsFileExist,					METH_VARARGS },
		{ "OpenTextFile",				appOpenTextFile,				METH_VARARGS },
		{ "CloseTextFile",				appCloseTextFile,				METH_VARARGS },
		{ "GetTextFileLineCount",		appGetTextFileLineCount,		METH_VARARGS },
		{ "GetTextFileLine",			appGetTextFileLine,				METH_VARARGS },

		{ "GetLocaleName",				appGetLocaleName,				METH_VARARGS },
		{ "GetLocalePath",				appGetLocalePath,				METH_VARARGS },
		{ "LoadLocaleAddr",				appLoadLocaleAddr,				METH_VARARGS },
		{ "LoadLocaleData",				appLoadLocaleData,				METH_VARARGS },

		{ "GetDefaultCodePage",			appGetDefaultCodePage,			METH_VARARGS },
		{ "SetControlFP",				appSetControlFP,				METH_VARARGS },
		{ "SetSpecularSpeed",			appSetSpecularSpeed,			METH_VARARGS },

		{ "testGetAccumulationTime",	apptestGetAccumulationTime,		METH_VARARGS },
		{ "testResetAccumulationTime",	apptestResetAccumulationTime,	METH_VARARGS },
		{ "testSetSpecularColor",		apptestSetSpecularColor,		METH_VARARGS },

		{ "SetVisibleNotice",			appSetVisibleNotice,			METH_VARARGS },
		{ "IsVisibleNotice",			appIsVisibleNotice,				METH_VARARGS },
		{ "EnableTestServerFlag",		appEnableTestServerFlag,		METH_VARARGS },
		{ "IsEnableTestServerFlag",		appIsEnableTestServerFlag,		METH_VARARGS },

		{ "SetGuildMarkPath",			appSetGuildMarkPath,			METH_VARARGS },

		{ "OnLogoUpdate",				appLogoUpdate,					METH_VARARGS },
		{ "OnLogoRender",				appLogoRender,					METH_VARARGS },
		{ "OnLogoOpen",					appLogoOpen,					METH_VARARGS },
		{ "OnLogoClose",				appLogoClose,					METH_VARARGS },

		{ "ShellExecute", 				appShellExecute,				METH_VARARGS },
		{ "OpenWebBrowser", 			appOpenWebBrowser,				METH_VARARGS },

		{ "GetName",					appGetName,						METH_VARARGS },

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		{ "SetLocale",					appSetLocale,					METH_VARARGS },
		{ "GetLocale",					appGetLocale,					METH_VARARGS },
#endif

#ifdef ENABLE_BLINK_ALERT
		{ "FlashApplication",			appFlashApplication,			METH_VARARGS },
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
		{ "SetMultiFarmExeIcon", 		appSetMultiFarmExeIcon,			METH_VARARGS },
#endif

		{ nullptr, nullptr },
	};

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	PyObject *poModule = Py_InitModule(GetModuleName(APP_MODULE).c_str(), s_methods);
#else
	PyObject *poModule = Py_InitModule("app", s_methods);
#endif

	PyModule_AddIntConstant(poModule, "INFO_ITEM", CPythonApplication::INFO_ITEM);
	PyModule_AddIntConstant(poModule, "INFO_ACTOR", CPythonApplication::INFO_ACTOR);
	PyModule_AddIntConstant(poModule, "INFO_EFFECT", CPythonApplication::INFO_EFFECT);
	PyModule_AddIntConstant(poModule, "INFO_TEXTTAIL", CPythonApplication::INFO_TEXTTAIL);

	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_SAME", DEGREE_DIRECTION_SAME);
	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_RIGHT", DEGREE_DIRECTION_RIGHT);
	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_LEFT", DEGREE_DIRECTION_LEFT);

	PyModule_AddIntConstant(poModule, "VK_LEFT", VK_LEFT);
	PyModule_AddIntConstant(poModule, "VK_RIGHT", VK_RIGHT);
	PyModule_AddIntConstant(poModule, "VK_UP", VK_UP);
	PyModule_AddIntConstant(poModule, "VK_DOWN", VK_DOWN);
	PyModule_AddIntConstant(poModule, "VK_HOME", VK_HOME);
	PyModule_AddIntConstant(poModule, "VK_END", VK_END);
	PyModule_AddIntConstant(poModule, "VK_DELETE", VK_DELETE);

	PyModule_AddIntConstant(poModule, "DIK_ESCAPE", DIK_ESCAPE);
	PyModule_AddIntConstant(poModule, "DIK_ESC", DIK_ESCAPE);
	PyModule_AddIntConstant(poModule, "DIK_1", DIK_1);
	PyModule_AddIntConstant(poModule, "DIK_2", DIK_2);
	PyModule_AddIntConstant(poModule, "DIK_3", DIK_3);
	PyModule_AddIntConstant(poModule, "DIK_4", DIK_4);
	PyModule_AddIntConstant(poModule, "DIK_5", DIK_5);
	PyModule_AddIntConstant(poModule, "DIK_6", DIK_6);
	PyModule_AddIntConstant(poModule, "DIK_7", DIK_7);
	PyModule_AddIntConstant(poModule, "DIK_8", DIK_8);
	PyModule_AddIntConstant(poModule, "DIK_9", DIK_9);
	PyModule_AddIntConstant(poModule, "DIK_0", DIK_0);
	PyModule_AddIntConstant(poModule, "DIK_MINUS", DIK_MINUS);
	PyModule_AddIntConstant(poModule, "DIK_EQUALS", DIK_EQUALS);
	PyModule_AddIntConstant(poModule, "DIK_BACK", DIK_BACK);
	PyModule_AddIntConstant(poModule, "DIK_TAB", DIK_TAB);
	PyModule_AddIntConstant(poModule, "DIK_Q", DIK_Q);
	PyModule_AddIntConstant(poModule, "DIK_W", DIK_W);
	PyModule_AddIntConstant(poModule, "DIK_E", DIK_E);
	PyModule_AddIntConstant(poModule, "DIK_R", DIK_R);
	PyModule_AddIntConstant(poModule, "DIK_T", DIK_T);
	PyModule_AddIntConstant(poModule, "DIK_Y", DIK_Y);
	PyModule_AddIntConstant(poModule, "DIK_U", DIK_U);
	PyModule_AddIntConstant(poModule, "DIK_I", DIK_I);
	PyModule_AddIntConstant(poModule, "DIK_O", DIK_O);
	PyModule_AddIntConstant(poModule, "DIK_P", DIK_P);
	PyModule_AddIntConstant(poModule, "DIK_LBRACKET", DIK_LBRACKET);
	PyModule_AddIntConstant(poModule, "DIK_RBRACKET", DIK_RBRACKET);
	PyModule_AddIntConstant(poModule, "DIK_RETURN", DIK_RETURN);
	PyModule_AddIntConstant(poModule, "DIK_LCONTROL", DIK_LCONTROL);
	PyModule_AddIntConstant(poModule, "DIK_A", DIK_A);
	PyModule_AddIntConstant(poModule, "DIK_S", DIK_S);
	PyModule_AddIntConstant(poModule, "DIK_D", DIK_D);
	PyModule_AddIntConstant(poModule, "DIK_F", DIK_F);
	PyModule_AddIntConstant(poModule, "DIK_G", DIK_G);
	PyModule_AddIntConstant(poModule, "DIK_H", DIK_H);
	PyModule_AddIntConstant(poModule, "DIK_J", DIK_J);
	PyModule_AddIntConstant(poModule, "DIK_K", DIK_K);
	PyModule_AddIntConstant(poModule, "DIK_L", DIK_L);
	PyModule_AddIntConstant(poModule, "DIK_SEMICOLON", DIK_SEMICOLON);
	PyModule_AddIntConstant(poModule, "DIK_APOSTROPHE", DIK_APOSTROPHE);
	PyModule_AddIntConstant(poModule, "DIK_GRAVE", DIK_GRAVE);
	PyModule_AddIntConstant(poModule, "DIK_LSHIFT", DIK_LSHIFT);
	PyModule_AddIntConstant(poModule, "DIK_BACKSLASH", DIK_BACKSLASH);
	PyModule_AddIntConstant(poModule, "DIK_Z", DIK_Z);
	PyModule_AddIntConstant(poModule, "DIK_X", DIK_X);
	PyModule_AddIntConstant(poModule, "DIK_C", DIK_C);
	PyModule_AddIntConstant(poModule, "DIK_V", DIK_V);
	PyModule_AddIntConstant(poModule, "DIK_B", DIK_B);
	PyModule_AddIntConstant(poModule, "DIK_N", DIK_N);
	PyModule_AddIntConstant(poModule, "DIK_M", DIK_M);
	PyModule_AddIntConstant(poModule, "DIK_COMMA", DIK_COMMA);
	PyModule_AddIntConstant(poModule, "DIK_PERIOD", DIK_PERIOD);
	PyModule_AddIntConstant(poModule, "DIK_SLASH", DIK_SLASH);
	PyModule_AddIntConstant(poModule, "DIK_RSHIFT", DIK_RSHIFT);
	PyModule_AddIntConstant(poModule, "DIK_MULTIPLY", DIK_MULTIPLY);
	PyModule_AddIntConstant(poModule, "DIK_LALT", DIK_LMENU);
	PyModule_AddIntConstant(poModule, "DIK_SPACE", DIK_SPACE);
	PyModule_AddIntConstant(poModule, "DIK_CAPITAL", DIK_CAPITAL);
	PyModule_AddIntConstant(poModule, "DIK_F1", DIK_F1);
	PyModule_AddIntConstant(poModule, "DIK_F2", DIK_F2);
	PyModule_AddIntConstant(poModule, "DIK_F3", DIK_F3);
	PyModule_AddIntConstant(poModule, "DIK_F4", DIK_F4);
	PyModule_AddIntConstant(poModule, "DIK_F5", DIK_F5);
	PyModule_AddIntConstant(poModule, "DIK_F6", DIK_F6);
	PyModule_AddIntConstant(poModule, "DIK_F7", DIK_F7);
	PyModule_AddIntConstant(poModule, "DIK_F8", DIK_F8);
	PyModule_AddIntConstant(poModule, "DIK_F9", DIK_F9);
	PyModule_AddIntConstant(poModule, "DIK_F10", DIK_F10);
	PyModule_AddIntConstant(poModule, "DIK_NUMLOCK", DIK_NUMLOCK);
	PyModule_AddIntConstant(poModule, "DIK_SCROLL", DIK_SCROLL);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD7", DIK_NUMPAD7);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD8", DIK_NUMPAD8);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD9", DIK_NUMPAD9);
	PyModule_AddIntConstant(poModule, "DIK_SUBTRACT", DIK_SUBTRACT);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD4", DIK_NUMPAD4);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD5", DIK_NUMPAD5);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD6", DIK_NUMPAD6);
	PyModule_AddIntConstant(poModule, "DIK_ADD", DIK_ADD);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD1", DIK_NUMPAD1);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD2", DIK_NUMPAD2);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD3", DIK_NUMPAD3);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD0", DIK_NUMPAD0);
	PyModule_AddIntConstant(poModule, "DIK_DECIMAL", DIK_DECIMAL);
	PyModule_AddIntConstant(poModule, "DIK_F11", DIK_F11);
	PyModule_AddIntConstant(poModule, "DIK_F12", DIK_F12);
	PyModule_AddIntConstant(poModule, "DIK_NEXTTRACK", DIK_NEXTTRACK);
	PyModule_AddIntConstant(poModule, "DIK_NUMPADENTER", DIK_NUMPADENTER);
	PyModule_AddIntConstant(poModule, "DIK_RCONTROL", DIK_RCONTROL);
	PyModule_AddIntConstant(poModule, "DIK_MUTE", DIK_MUTE);
	PyModule_AddIntConstant(poModule, "DIK_CALCULATOR", DIK_CALCULATOR);
	PyModule_AddIntConstant(poModule, "DIK_PLAYPAUSE", DIK_PLAYPAUSE);
	PyModule_AddIntConstant(poModule, "DIK_MEDIASTOP", DIK_MEDIASTOP);
	PyModule_AddIntConstant(poModule, "DIK_VOLUMEDOWN", DIK_VOLUMEDOWN);
	PyModule_AddIntConstant(poModule, "DIK_VOLUMEUP", DIK_VOLUMEUP);
	PyModule_AddIntConstant(poModule, "DIK_WEBHOME", DIK_WEBHOME);
	PyModule_AddIntConstant(poModule, "DIK_NUMPADCOMMA", DIK_NUMPADCOMMA);
	PyModule_AddIntConstant(poModule, "DIK_DIVIDE", DIK_DIVIDE);
	PyModule_AddIntConstant(poModule, "DIK_SYSRQ", DIK_SYSRQ);
	PyModule_AddIntConstant(poModule, "DIK_RALT", DIK_RMENU);
	PyModule_AddIntConstant(poModule, "DIK_PAUSE", DIK_PAUSE);
	PyModule_AddIntConstant(poModule, "DIK_HOME", DIK_HOME);
	PyModule_AddIntConstant(poModule, "DIK_UP", DIK_UP);
	PyModule_AddIntConstant(poModule, "DIK_PGUP", DIK_PRIOR);
	PyModule_AddIntConstant(poModule, "DIK_LEFT", DIK_LEFT);
	PyModule_AddIntConstant(poModule, "DIK_RIGHT", DIK_RIGHT);
	PyModule_AddIntConstant(poModule, "DIK_END", DIK_END);
	PyModule_AddIntConstant(poModule, "DIK_DOWN", DIK_DOWN);
	PyModule_AddIntConstant(poModule, "DIK_PGDN", DIK_NEXT);
	PyModule_AddIntConstant(poModule, "DIK_INSERT", DIK_INSERT);
	PyModule_AddIntConstant(poModule, "DIK_DELETE", DIK_DELETE);
	PyModule_AddIntConstant(poModule, "DIK_LWIN", DIK_LWIN);
	PyModule_AddIntConstant(poModule, "DIK_RWIN", DIK_RWIN);
	PyModule_AddIntConstant(poModule, "DIK_APPS", DIK_APPS);

	PyModule_AddIntConstant(poModule, "NORMAL", CPythonApplication::CURSOR_SHAPE_NORMAL);
	PyModule_AddIntConstant(poModule, "ATTACK", CPythonApplication::CURSOR_SHAPE_ATTACK);
	PyModule_AddIntConstant(poModule, "TARGET", CPythonApplication::CURSOR_SHAPE_TARGET);
	PyModule_AddIntConstant(poModule, "TALK", CPythonApplication::CURSOR_SHAPE_TALK);
	PyModule_AddIntConstant(poModule, "CANT_GO", CPythonApplication::CURSOR_SHAPE_CANT_GO);
	PyModule_AddIntConstant(poModule, "PICK", CPythonApplication::CURSOR_SHAPE_PICK);

	PyModule_AddIntConstant(poModule, "DOOR", CPythonApplication::CURSOR_SHAPE_DOOR);
	PyModule_AddIntConstant(poModule, "CHAIR", CPythonApplication::CURSOR_SHAPE_CHAIR);
	PyModule_AddIntConstant(poModule, "MAGIC", CPythonApplication::CURSOR_SHAPE_MAGIC);
	PyModule_AddIntConstant(poModule, "BUY", CPythonApplication::CURSOR_SHAPE_BUY);
	PyModule_AddIntConstant(poModule, "SELL", CPythonApplication::CURSOR_SHAPE_SELL);

	PyModule_AddIntConstant(poModule, "CAMERA_ROTATE", CPythonApplication::CURSOR_SHAPE_CAMERA_ROTATE);
	PyModule_AddIntConstant(poModule, "HSIZE", CPythonApplication::CURSOR_SHAPE_HSIZE);
	PyModule_AddIntConstant(poModule, "VSIZE", CPythonApplication::CURSOR_SHAPE_VSIZE);
	PyModule_AddIntConstant(poModule, "HVSIZE", CPythonApplication::CURSOR_SHAPE_HVSIZE);

#ifdef ENABLE_FISH_GAME
	PyModule_AddIntConstant(poModule, "FISH", CPythonApplication::CURSOR_SHAPE_FISH);
#endif

	PyModule_AddIntConstant(poModule, "CAMERA_TO_POSITIVE", CPythonApplication::CAMERA_TO_POSITIVE);
	PyModule_AddIntConstant(poModule, "CAMERA_TO_NEGATIVE", CPythonApplication::CAMERA_TO_NEGITIVE);
	PyModule_AddIntConstant(poModule, "CAMERA_STOP", CPythonApplication::CAMERA_STOP);

#ifdef ENABLE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_COSTUME_SYSTEM", 0);
#endif

#ifdef ENABLE_ENERGY_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_ENERGY_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ENERGY_SYSTEM", 0);
#endif

#ifdef ENABLE_DRAGON_SOUL_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_DRAGON_SOUL_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DRAGON_SOUL_SYSTEM", 0);
#endif

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EQUIPMENT_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EQUIPMENT_SYSTEM", 0);
#endif

#ifdef ENABLE_LOAD_INDEX_BINARY
	PyModule_AddIntConstant(poModule, "ENABLE_LOAD_INDEX_BINARY", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LOAD_INDEX_BINARY", 0);
#endif

#ifdef ENABLE_CHANGE_CHANNEL
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGE_CHANNEL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGE_CHANNEL", 0);
#endif

#ifdef ENABLE_GUILD_RANK_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_RANK_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_RANK_SYSTEM", 0);
#endif

#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_PLAYER_HP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_PLAYER_HP", 0);
#endif

#ifdef ENABLE_VIEW_TARGET_DECIMAL_HP
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_DECIMAL_HP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_DECIMAL_HP", 0);
#endif

#ifdef ENABLE_MESSENGER_TEAM
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_TEAM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_TEAM", 0);
#endif

#ifdef ENABLE_RENEWAL_DEAD_PACKET
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_DEAD_PACKET", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_DEAD_PACKET", 0);
#endif

#ifdef ENABLE_MAP_TELEPORT
	PyModule_AddIntConstant(poModule, "ENABLE_MAP_TELEPORT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MAP_TELEPORT", 0);
#endif

#ifdef ENABLE_RENEWAL_SHOPEX
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SHOPEX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SHOPEX", 0);
#endif

#ifdef ENABLE_DESTROY_DIALOG
	PyModule_AddIntConstant(poModule, "ENABLE_DESTROY_DIALOG", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DESTROY_DIALOG", 0);
#endif

#ifdef ENABLE_FOG_FIX
	PyModule_AddIntConstant(poModule, "ENABLE_FOG_FIX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_FOG_FIX", 0);
#endif

#ifdef ENABLE_ENB_MODE
	PyModule_AddIntConstant(poModule, "ENABLE_ENB_MODE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ENB_MODE", 0);
#endif

#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
	PyModule_AddIntConstant(poModule, "ENABLE_ENVIRONMENT_EFFECT_OPTION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ENVIRONMENT_EFFECT_OPTION", 0);
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
	PyModule_AddIntConstant(poModule, "ENABLE_GRAPHIC_ON_OFF", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GRAPHIC_ON_OFF", 0);
#endif

#ifdef ENABLE_MOUNT_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_MOUNT_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MOUNT_SYSTEM", 0);
#endif

#ifdef ENABLE_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_PET_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PET_SYSTEM", 0);
#endif

#ifdef ENABLE_COINS_INVENTORY
	PyModule_AddIntConstant(poModule, "ENABLE_COINS_INVENTORY", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_COINS_INVENTORY", 0);
#endif

#ifdef ENABLE_SORT_INVENTORY
	PyModule_AddIntConstant(poModule, "ENABLE_SORT_INVENTORY", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SORT_INVENTORY", 0);
#endif

#ifdef ENABLE_INSTANT_PICKUP
	PyModule_AddIntConstant(poModule, "ENABLE_INSTANT_PICKUP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_INSTANT_PICKUP", 0);
#endif

#ifdef ENABLE_EXTENDED_SAFEBOX
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_SAFEBOX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_SAFEBOX", 0);
#endif

#ifdef ENABLE_MESSENGER_BLOCK
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_BLOCK", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_BLOCK", 0);
#endif

#ifdef ENABLE_RENEWAL_QUEST
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_QUEST", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_QUEST", 0);
#endif

#ifdef ENABLE_QUICK_SELL_ITEM
	PyModule_AddIntConstant(poModule, "ENABLE_QUICK_SELL_ITEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_QUICK_SELL_ITEM", 0);
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_LANGUAGE_SYSTEM", 1);
	PyModule_AddIntConstant(poModule, "LOCALE_DEFAULT", CPythonApplication::LOCALE_DEFAULT);
	PyModule_AddIntConstant(poModule, "LOCALE_EN", CPythonApplication::LOCALE_EN);
	PyModule_AddIntConstant(poModule, "LOCALE_RO", CPythonApplication::LOCALE_RO);
	PyModule_AddIntConstant(poModule, "LOCALE_PT", CPythonApplication::LOCALE_PT);
	PyModule_AddIntConstant(poModule, "LOCALE_ES", CPythonApplication::LOCALE_ES);
	PyModule_AddIntConstant(poModule, "LOCALE_FR", CPythonApplication::LOCALE_FR);
	PyModule_AddIntConstant(poModule, "LOCALE_DE", CPythonApplication::LOCALE_DE);
	PyModule_AddIntConstant(poModule, "LOCALE_PL", CPythonApplication::LOCALE_PL);
	PyModule_AddIntConstant(poModule, "LOCALE_IT", CPythonApplication::LOCALE_IT);
	PyModule_AddIntConstant(poModule, "LOCALE_CZ", CPythonApplication::LOCALE_CZ);
	PyModule_AddIntConstant(poModule, "LOCALE_HU", CPythonApplication::LOCALE_HU);
	PyModule_AddIntConstant(poModule, "LOCALE_TR", CPythonApplication::LOCALE_TR);
	PyModule_AddIntConstant(poModule, "LOCALE_MAX_NUM", CPythonApplication::LOCALE_MAX_NUM);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_LANGUAGE_SYSTEM", 0);
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_WHISPER_DETAILS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_WHISPER_DETAILS", 0);
#endif

#ifdef ENABLE_RENEWAL_BONUS_BOARD
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_BONUS_BOARD", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_BONUS_BOARD", 0);
#endif

#ifdef ENABLE_GOLD_LIMIT
	PyModule_AddIntConstant(poModule, "ENABLE_GOLD_LIMIT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GOLD_LIMIT", 0);
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_WEAPON_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_WEAPON_COSTUME_SYSTEM", 0);
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SWITCHBOT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SWITCHBOT", 0);
#endif

#ifdef ENABLE_RENEWAL_CUBE
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_CUBE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_CUBE", 0);
#endif

#ifdef ENABLE_RENEWAL_CLIENT_VERSION
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_CLIENT_VERSION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_CLIENT_VERSION", 0);
#endif

#ifdef ENABLE_CLIENT_PERFORMANCE
	PyModule_AddIntConstant(poModule, "ENABLE_CLIENT_PERFORMANCE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CLIENT_PERFORMANCE", 0);
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
	PyModule_AddIntConstant(poModule, "ENABLE_TELEPORT_TO_A_FRIEND", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_TELEPORT_TO_A_FRIEND", 0);
#endif

#ifdef ENABLE_BLINK_ALERT
	PyModule_AddIntConstant (poModule, "ENABLE_BLINK_ALERT", 1);
#else
	PyModule_AddIntConstant (poModule, "ENABLE_BLINK_ALERT", 0);
#endif

#ifdef ENABLE_METIN_STONES_MINIMAP
	PyModule_AddIntConstant(poModule, "ENABLE_METIN_STONES_MINIMAP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_METIN_STONES_MINIMAP", 0);
#endif

#ifdef ENABLE_AUTO_REFINE
	PyModule_AddIntConstant(poModule, "ENABLE_AUTO_REFINE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AUTO_REFINE", 0);
#endif

#ifdef ENABLE_SHOW_MOB_INFO
	PyModule_AddIntConstant(poModule, "ENABLE_SHOW_MOB_INFO", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SHOW_MOB_INFO", 0);
#endif

#ifdef ENABLE_DICE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_DICE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DICE_SYSTEM", 0);
#endif

#ifdef ENABLE_MOUNT_PET_SKIN
	PyModule_AddIntConstant(poModule, "ENABLE_MOUNT_PET_SKIN", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MOUNT_PET_SKIN", 0);
#endif

#ifdef ENABLE_PENDANT_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_PENDANT_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PENDANT_SYSTEM", 0);
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_HIDE_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HIDE_COSTUME_SYSTEM", 0);
#endif

#ifdef ENABLE_RENEWAL_SERVER_LIST
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SERVER_LIST", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SERVER_LIST", 0);
#endif

#ifdef ENABLE_EXTEND_TIME_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_TIME_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_TIME_COSTUME_SYSTEM", 0);
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_COSTUME_SYSTEM", 0);
#endif

#ifdef ENABLE_ATTBONUS_METIN
	PyModule_AddIntConstant(poModule, "ENABLE_ATTBONUS_METIN", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ATTBONUS_METIN", 0);
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_BIOLOG_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_BIOLOG_SYSTEM", 0);
#endif

#ifdef ENABLE_MOB_DROP_INFO
	PyModule_AddIntConstant(poModule, "ENABLE_MOB_DROP_INFO", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MOB_DROP_INFO", 0);
#endif

#ifdef ENABLE_ANTI_EXP
	PyModule_AddIntConstant(poModule, "ENABLE_ANTI_EXP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ANTI_EXP", 0);
#endif

#ifdef ENABLE_VIEW_CHEST_DROP
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_CHEST_DROP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_CHEST_DROP", 0);
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	PyModule_AddIntConstant(poModule, "ENABLE_SPECIAL_INVENTORY", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SPECIAL_INVENTORY", 0);
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_OFFLINESHOP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_OFFLINESHOP", 0);
#endif

#ifdef ENABLE_TITLE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_TITLE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_TITLE_SYSTEM", 0);
#endif

#ifdef ENABLE_ATTBONUS_BOSS
	PyModule_AddIntConstant(poModule, "ENABLE_ATTBONUS_BOSS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ATTBONUS_BOSS", 0);
#endif

#ifdef ENABLE_RENDER_TARGET
	PyModule_AddIntConstant(poModule, "ENABLE_RENDER_TARGET", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENDER_TARGET", 0);
#endif

#ifdef ENABLE_EVENT_MANAGER
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_MANAGER", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_MANAGER", 0);
#endif

#ifdef ENABLE_RESTART_INSTANT
	PyModule_AddIntConstant(poModule, "ENABLE_RESTART_INSTANT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RESTART_INSTANT", 0);
#endif

#ifdef ENABLE_MINIGAME_OKEY_CARDS_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_MINIGAME_OKEY_CARDS_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MINIGAME_OKEY_CARDS_SYSTEM", 0);
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_BATTLE_PASS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_BATTLE_PASS", 0);
#endif

#ifdef ENABLE_MONSTER_TARGET_ELEMENT
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_TARGET_ELEMENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_TARGET_ELEMENT", 0);
#endif

#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SPECIAL_CHAT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SPECIAL_CHAT", 0);
#endif

#ifdef ENABLE_RENEWAL_AFFECT
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_AFFECT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_AFFECT", 0);
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_FARM_BLOCK", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_FARM_BLOCK", 0);
#endif

#ifdef ENABLE_RENEWAL_TEAM_AFFECT
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_TEAM_AFFECT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_TEAM_AFFECT", 0);
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_SKILL_COLOR_SYSTEM", 1);
#ifdef ENABLE_5LAYER_SKILL_COLOR
	PyModule_AddIntConstant(poModule, "ENABLE_5LAYER_SKILL_COLOR", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_5LAYER_SKILL_COLOR", 0);
#endif
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SKILL_COLOR_SYSTEM", 0);
#endif

#ifdef ENABLE_RENEWAL_PREMIUM_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_PREMIUM_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_PREMIUM_SYSTEM", 0);
#endif

#ifdef ENABLE_INGAME_WIKI_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_INGAME_WIKI_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_INGAME_WIKI_SYSTEM", 0);
#endif

#ifdef ENABLE_PICKUP_ITEM_EFFECT
	PyModule_AddIntConstant(poModule, "ENABLE_PICKUP_ITEM_EFFECT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PICKUP_ITEM_EFFECT", 0);
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", 0);
#endif

#ifdef ENABLE_RENEWAL_OX_EVENT
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_OX_EVENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_OX_EVENT", 0);
#endif

#ifdef ENABLE_RENEWAL_TEXT_SHADOW
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_TEXT_SHADOW", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_TEXT_SHADOW", 0);
#endif

#ifdef ENABLE_FOV_OPTION
	PyModule_AddIntConstant(poModule, "ENABLE_FOV_OPTION", 1);
	PyModule_AddIntConstant(poModule, "DEFAULT_CAMERA_PERSPECTIVE", c_fDefaultCameraPerspective);
	PyModule_AddIntConstant(poModule, "MIN_CAMERA_PERSPECTIVE", c_fMinCameraPerspective);
	PyModule_AddIntConstant(poModule, "MAX_CAMERA_PERSPECTIVE", c_fMaxCameraPerspective);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_FOV_OPTION", 0);
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_AUTOMATIC_PICK_UP_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AUTOMATIC_PICK_UP_SYSTEM", 0);
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_MAINTENANCE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MAINTENANCE_SYSTEM", 0);
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
	PyModule_AddIntConstant(poModule, "ENABLE_STONE_SCALE_OPTION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_STONE_SCALE_OPTION", 0);
#endif

#ifdef ENABLE_RENEWAL_SKILL_SELECT
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SKILL_SELECT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_SKILL_SELECT", 0);
#endif

#ifdef ENABLE_MULTI_TEXTLINE
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_TEXTLINE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_TEXTLINE", 0);
#endif

#ifdef ENABLE_BONUS_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_BONUS_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_BONUS_COSTUME_SYSTEM", 0);
#endif

#ifdef ENABLE_NEW_DUNGEON_LIB
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_DUNGEON_LIB", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_DUNGEON_LIB", 0);
#endif

#ifdef ENABLE_RENEWAL_INGAME_ITEMSHOP
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_INGAME_ITEMSHOP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_INGAME_ITEMSHOP", 0);
#endif

#ifdef ENABLE_SLOT_MARKING_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_MARKING_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_MARKING_SYSTEM", 0);
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_AURA_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AURA_COSTUME_SYSTEM", 0);
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
	PyModule_AddIntConstant (poModule, "ENABLE_EMOTICONS_SYSTEM", 1);
#else
	PyModule_AddIntConstant (poModule, "ENABLE_EMOTICONS_SYSTEM", 0);
#endif

#ifdef ENABLE_FISH_GAME
	PyModule_AddIntConstant(poModule, "ENABLE_FISH_GAME", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_FISH_GAME", 0);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_GROWTH_PET_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GROWTH_PET_SYSTEM", 0);
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_INVENTORY_EXPANSION_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_INVENTORY_EXPANSION_SYSTEM", 0);
#endif

#ifdef ENABLE_SHIP_DEFENCE_DUNGEON
	PyModule_AddIntConstant(poModule, "ENABLE_SHIP_DEFENCE_DUNGEON", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SHIP_DEFENCE_DUNGEON", 0);
#endif

#ifdef ENABLE_RENEWAL_TELEPORT_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_TELEPORT_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_TELEPORT_SYSTEM", 0);
#endif

#ifdef ENABLE_DUNGEON_TRACKING_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_DUNGEON_TRACKING_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DUNGEON_TRACKING_SYSTEM", 0);
#endif

#ifdef ENABLE_RENEWAL_REGEN
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_REGEN", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_REGEN", 0);
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGE_LOOK_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGE_LOOK_SYSTEM", 0);
#endif

#ifdef ENABLE_HUNTING_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_HUNTING_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HUNTING_SYSTEM", 0);
#endif

#ifdef ENABLE_CLIP_MASKING
	PyModule_AddIntConstant(poModule, "ENABLE_CLIP_MASKING", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CLIP_MASKING", 0);
#endif

#ifdef ENABLE_STANDING_MOUNT
	PyModule_AddIntConstant(poModule, "ENABLE_STANDING_MOUNT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_STANDING_MOUNT", 0);
#endif

#ifdef WJ_SHOW_NPC_QUEST_NAME
	PyModule_AddIntConstant(poModule, "WJ_SHOW_NPC_QUEST_NAME", 1);
#else
	PyModule_AddIntConstant(poModule, "WJ_SHOW_NPC_QUEST_NAME", 0);
#endif

#ifdef ENABLE_ATLAS_SCALE
	PyModule_AddIntConstant(poModule, "ENABLE_ATLAS_SCALE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ATLAS_SCALE", 0);
#endif

#ifdef ENABLE_RIDING_EXTENDED
	PyModule_AddIntConstant(poModule, "ENABLE_RIDING_EXTENDED", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RIDING_EXTENDED", 0);
#endif

#ifdef ENABLE_SKILL_BOOK_READING
	PyModule_AddIntConstant(poModule, "ENABLE_SKILL_BOOK_READING",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SKILL_BOOK_READING",	0);
#endif

#ifdef ENABLE_SPIRIT_STONE_READING
	PyModule_AddIntConstant(poModule, "ENABLE_SPIRIT_STONE_READING",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SPIRIT_STONE_READING",	0);
#endif

#ifdef __AUTO_HUNT__
	PyModule_AddIntConstant(poModule, "__AUTO_HUNT__", 1);
#else
	PyModule_AddIntConstant(poModule, "__AUTO_HUNT__", 0);
#endif

#ifdef ENABLE_AUTO_SELL_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_AUTO_SELL_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AUTO_SELL_SYSTEM", 0);
#endif

#ifdef ENABLE_GUILDRENEWAL_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_GUILDRENEWAL_SYSTEM", 1);
	#ifdef ENABLE_SECOND_GUILDRENEWAL_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_SECOND_GUILDRENEWAL_SYSTEM", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_SECOND_GUILDRENEWAL_SYSTEM", 0);
	#endif

	#ifdef ENABLE_GUILD_MARK_RENEWAL
		PyModule_AddIntConstant(poModule, "ENABLE_GUILD_MARK_RENEWAL", 1);
	#else
		PyModule_AddIntConstant(poModule, "ENABLE_GUILD_MARK_RENEWAL", 0);
	#endif

	#ifdef ENABLE_EXTENDED_GUILD_LEVEL
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_GUILD_LEVEL", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_GUILD_LEVEL", 0);
	#endif

	#ifdef ENABLE_MEDAL_OF_HONOR
	PyModule_AddIntConstant(poModule, "ENABLE_MEDAL_OF_HONOR", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_MEDAL_OF_HONOR", 0);
	#endif
	#ifdef ENABLE_GUILD_DONATE_ATTENDANCE
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_DONATE_ATTENDANCE", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_DONATE_ATTENDANCE", 0);
	#endif

	#ifdef ENABLE_GUILD_WAR_SCORE
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_WAR_SCORE", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_WAR_SCORE", 0);
	#endif
	#ifdef ENABLE_GUILD_LAND_INFO
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_LAND_INFO", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_LAND_INFO", 0);
	#endif
	#ifdef ENABLE_GUILDBANK_LOG
	PyModule_AddIntConstant(poModule, "ENABLE_GUILDBANK_LOG", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILDBANK_LOG", 0);
	#endif

	#ifdef ENABLE_NEW_WORLD_GUILDRENEWAL
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_WORLD_GUILDRENEWAL", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_WORLD_GUILDRENEWAL", 0);
	#endif

	#ifdef ENABLE_GUILD_REQUEST
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_REQUEST", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_REQUEST", 0);
	#endif
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILDRENEWAL_SYSTEM", 0);
	#ifndef ENABLE_SECOND_GUILDRENEWAL_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_SECOND_GUILDRENEWAL_SYSTEM", 0);
	#endif

	#ifndef ENABLE_EXTENDED_GUILD_LEVEL
	PyModule_AddIntConstant(poModule, "ENABLE_EXTENDED_GUILD_LEVEL", 0);
	#endif

	#ifndef ENABLE_MEDAL_OF_HONOR
	PyModule_AddIntConstant(poModule, "ENABLE_MEDAL_OF_HONOR", 0);
	#endif
	#ifndef ENABLE_GUILD_DONATE_ATTENDANCE
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_DONATE_ATTENDANCE", 0);
	#endif

	#ifndef ENABLE_GUILD_WAR_SCORE
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_WAR_SCORE", 0);
	#endif
	#ifndef ENABLE_GUILD_LAND_INFO
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_LAND_INFO", 0);
	#endif
	#ifndef ENABLE_GUILDBANK_LOG
	PyModule_AddIntConstant(poModule, "ENABLE_GUILDBANK_LOG", 0);
	#endif

	#ifndef ENABLE_NEW_WORLD_GUILDRENEWAL
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_WORLD_GUILDRENEWAL", 0);
	#endif

	#ifndef ENABLE_GUILD_REQUEST
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_REQUEST", 0);
	#endif

	#ifdef ENABLE_GUILDSTORAGE_SYSTEM
		PyModule_AddIntConstant(poModule, "ENABLE_GUILDSTORAGE_SYSTEM", 1);
	#else
		PyModule_AddIntConstant(poModule, "ENABLE_GUILDSTORAGE_SYSTEM", 0);
	#endif
#endif

#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_STYLE_ATTRIBUTE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_STYLE_ATTRIBUTE_SYSTEM", 0);
#endif
}
