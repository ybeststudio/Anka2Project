#include "StdAfx.h"
#include "../AudioLib/SoundEngine.h"
#include "PythonSystem.h"
#include "PythonTextTail.h"
#include "PythonBackground.h"
#include "PythonNonPlayer.h"
#include "PythonCharacterManager.h"

PyObject *systemGetWidth(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetWidth());
}

PyObject *systemGetHeight(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetHeight());
}

PyObject *systemSetInterfaceHandler(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonSystem::Instance().SetInterfaceHandler(poHandler);
	return Py_BuildNone();
}

PyObject *systemDestroyInterfaceHandler(PyObject *poSelf, PyObject *poArgs)
{
	CPythonSystem::Instance().DestroyInterfaceHandler();
	return Py_BuildNone();
}

PyObject *systemReserveResource(PyObject *poSelf, PyObject *poArgs)
{
	std::set<std::string> ResourceSet;
	CResourceManager::Instance().PushBackgroundLoadingSet(ResourceSet);
	return Py_BuildNone();
}

PyObject *systemisInterfaceConfig(PyObject *poSelf, PyObject *poArgs)
{
	int isInterfaceConfig = CPythonSystem::Instance().isInterfaceConfig() && CPythonSystem::Instance().GetAdvancedOption("save_status_window",1);
	return Py_BuildValue("i", isInterfaceConfig);
}

PyObject *systemSaveWindowStatus(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iVisible;
	if (!PyTuple_GetInteger(poArgs, 1, &iVisible))
		return Py_BuildException();

	int iMinimized;
	if (!PyTuple_GetInteger(poArgs, 2, &iMinimized))
		return Py_BuildException();

	int ix;
	if (!PyTuple_GetInteger(poArgs, 3, &ix))
		return Py_BuildException();

	int iy;
	if (!PyTuple_GetInteger(poArgs, 4, &iy))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 5, &iHeight))
		return Py_BuildException();

	if (!CPythonSystem::Instance().GetAdvancedOption("save_status_window",1)) return Py_BuildNone();
	CPythonSystem::Instance().SaveWindowStatus(iIndex, iVisible, iMinimized, ix, iy, iHeight);
	return Py_BuildNone();
}

PyObject *systemGetWindowStatus(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const CPythonSystem::TWindowStatus & c_rWindowStatus = CPythonSystem::Instance().GetWindowStatusReference(iIndex);
	return Py_BuildValue("iiiii", c_rWindowStatus.isVisible,
								  c_rWindowStatus.isMinimized,
								  c_rWindowStatus.ixPosition,
								  c_rWindowStatus.iyPosition,
								  c_rWindowStatus.iHeight);
}

PyObject *systemGetConfig(PyObject *poSelf, PyObject *poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();

	int iRes = CPythonSystem::Instance().GetResolutionIndex(tmp->width, tmp->height, tmp->bpp);
	int iFrequency = CPythonSystem::Instance().GetFrequencyIndex(iRes, tmp->frequency);

	return Py_BuildValue("iiiiiiii",  iRes,
									  iFrequency,
									  tmp->is_software_cursor,
									  tmp->is_object_culling,
									  tmp->music_volume,
									  tmp->voice_volume,
									  tmp->gamma,
									  tmp->iDistance);
}

PyObject *systemSetSaveID(PyObject *poSelf, PyObject *poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	char *szSaveID;
	if (!PyTuple_GetString(poArgs, 1, &szSaveID))
		return Py_BuildException();

	CPythonSystem::Instance().SetSaveID(iValue, szSaveID);
	return Py_BuildNone();
}

PyObject *systemisSaveID(PyObject *poSelf, PyObject *poArgs)
{
	int value = CPythonSystem::Instance().IsSaveID();
	return Py_BuildValue("i", value);
}

PyObject *systemGetSaveID(PyObject *poSelf, PyObject *poArgs)
{
	const char *c_szSaveID = CPythonSystem::Instance().GetSaveID();
	return Py_BuildValue("s", c_szSaveID);
}

PyObject *systemGetMusicVolume(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetMusicVolume());
}

PyObject *systemGetSoundVolume(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSoundVolume());
}

PyObject *systemSetMusicVolume(PyObject *poSelf, PyObject *poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetMusicVolume(fVolume);
	return Py_BuildNone();
}

PyObject *systemSetSoundVolumef(PyObject *poSelf, PyObject *poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetSoundVolumef(fVolume);
	return Py_BuildNone();
}

PyObject *systemIsSoftwareCursor(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsSoftwareCursor());
}

PyObject *systemSetViewChatFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetViewChatFlag(iFlag);

	return Py_BuildNone();
}

PyObject *systemIsViewChat(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsViewChat());
}

PyObject *systemSetAlwaysShowNameFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetAlwaysShowNameFlag(iFlag);

	return Py_BuildNone();
}

PyObject *systemSetShowDamageFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowDamageFlag(iFlag);

	return Py_BuildNone();
}

PyObject *systemSetShowSalesTextFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowSalesTextFlag(iFlag);

	return Py_BuildNone();
}

PyObject *systemIsAlwaysShowName(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsAlwaysShowName());
}

PyObject *systemIsShowDamage(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowDamage());
}

PyObject *systemIsShowSalesText(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowSalesText());
}

PyObject *systemSetConfig(PyObject *poSelf, PyObject *poArgs)
{
	int res_index;
	int width;
	int height;
	int bpp;
	int frequency_index;
	int frequency;
	int software_cursor;
	int shadow;
	int object_culling;
	int music_volume;
	int voice_volume;
	int gamma;
	int distance;

	if (!PyTuple_GetInteger(poArgs, 0, &res_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &software_cursor))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 3, &shadow))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 4, &object_culling))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 5, &music_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 6, &voice_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 7, &gamma))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 8, &distance))
		return Py_BuildException();

	if (!CPythonSystem::Instance().GetResolution(res_index, (DWORD *) &width, (DWORD *) &height, (DWORD *) &bpp))
		return Py_BuildNone();

	if (!CPythonSystem::Instance().GetFrequency(res_index,frequency_index, (DWORD *) &frequency))
		return Py_BuildNone();

	CPythonSystem::TConfig tmp;

	memcpy(&tmp, CPythonSystem::Instance().GetConfig(), sizeof(tmp));

	tmp.width				= width;
	tmp.height				= height;
	tmp.bpp					= bpp;
	tmp.frequency			= frequency;
	tmp.is_software_cursor	= software_cursor ? true : false;
	tmp.is_object_culling	= object_culling ? true : false;
	tmp.music_volume		= (char) music_volume;
	tmp.voice_volume		= (char) voice_volume;
	tmp.gamma				= gamma;
	tmp.iDistance			= distance;

	CPythonSystem::Instance().SetConfig(&tmp);
	return Py_BuildNone();
}

PyObject *systemApplyConfig(PyObject *poSelf, PyObject *poArgs)
{
	CPythonSystem::Instance().ApplyConfig();
	return Py_BuildNone();
}

PyObject *systemSaveConfig(PyObject *poSelf, PyObject *poArgs)
{
	int ret = CPythonSystem::Instance().SaveConfig();
	return Py_BuildValue("i", ret);
}

PyObject *systemGetResolutionCount(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetResolutionCount());
}

PyObject *systemGetFrequencyCount(PyObject *poSelf, PyObject *poArgs)
{
	int	index;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonSystem::Instance().GetFrequencyCount(index));
}

PyObject *systemGetResolution(PyObject *poSelf, PyObject *poArgs)
{
	int	index;
	DWORD width = 0, height = 0, bpp = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonSystem::Instance().GetResolution(index, &width, &height, &bpp);
	return Py_BuildValue("iii", width, height, bpp);
}

PyObject *systemGetCurrentResolution(PyObject *poSelf, PyObject *poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();
	return Py_BuildValue("iii", tmp->width, tmp->height, tmp->bpp);
}

PyObject *systemGetFrequency(PyObject *poSelf, PyObject *poArgs)
{
	int	index, frequency_index;
	DWORD frequency = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	CPythonSystem::Instance().GetFrequency(index, frequency_index, &frequency);
	return Py_BuildValue("i", frequency);
}

PyObject *systemGetShadowLevel(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowLevel());
}

PyObject *systemSetShadowLevel(PyObject *poSelf, PyObject *poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level > 0)
		CPythonSystem::Instance().SetShadowLevel(level);

	return Py_BuildNone();
}

PyObject* systemGetGraphicsLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetGraphicsLevel());
}

PyObject* systemSetGraphicsLevel(PyObject* poSelf, PyObject* poArgs)
{
	int level;
	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	CPythonSystem::Instance().SetGraphicsLevel(level);
	return Py_BuildNone();
}

PyObject* systemGetShadowQualityLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowQualityLevel());
}

PyObject* systemSetShadowQualityLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetShadowQualityLevel(iLevel);
	return Py_BuildNone();
}

PyObject* systemGetTerrainShadow(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetTerrainShadow());
}

// The only Python caller is the advanced-options "shadow_caster_terrain" toggle (1/0). It used to write
// that 1/0 into the shadow *level*, which dropped every preset to "player only" so trees and
// buildings never cast. The level now comes from the graphics preset; this is just the switch.
PyObject* systemSetTerrainShadow(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetAdvancedOption("shadow_caster_terrain", iLevel ? 1.0f : 0.0f);
	return Py_BuildNone();
}

PyObject* systemGetTreeShadow(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetTreeShadow());
}

// The only Python caller is the advanced-options "shadow_caster_trees" toggle (1/0). It used to write
// that 1/0 into the shadow *level*, which dropped every preset to "player only" so trees and
// buildings never cast. The level now comes from the graphics preset; this is just the switch.
PyObject* systemSetTreeShadow(PyObject* poSelf, PyObject* poArgs)
{
	int iLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BuildException();

	CPythonSystem::Instance().SetAdvancedOption("shadow_caster_trees", iLevel ? 1.0f : 0.0f);
	return Py_BuildNone();
}

PyObject* systemSetSharpnessPower(PyObject* poSelf, PyObject* poArgs)
{
	float fValue;
	if (!PyTuple_GetFloat(poArgs, 0, &fValue))
		return Py_BuildException();

	CPythonSystem::Instance().SetSharpnessPower(fValue);
	return Py_BuildNone();
}

PyObject* systemGetSharpnessPower(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetSharpnessPower());
}

#ifdef ENABLE_FOG_FIX
PyObject *systemSetFogMode(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetFogMode(iFlag);

	return Py_BuildNone();
}

PyObject *systemIsFogMode(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsFogMode());
}
#endif

#ifdef ENABLE_ENB_MODE
PyObject *systemSetENBModeStatusFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	CPythonSystem::Instance().SetENBModeStatusFlag(iFlag);
	return Py_BuildNone();
}

PyObject *systemIsENBModeStatus(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsENBModeStatus());
}
#endif

#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
PyObject *systemSetNightModeOption(PyObject *poSelf, PyObject *poArgs)
{
	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BuildException();

	CPythonSystem::Instance().SetNightModeOption(bEnable);
	return Py_BuildNone();
}

PyObject *systemGetNightModeOption(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetNightModeOption());
}

PyObject *systemSetSnowModeOption(PyObject *poSelf, PyObject *poArgs)
{
	int iOpt;
	if (!PyTuple_GetInteger(poArgs, 0, &iOpt))
		return Py_BuildException();

	CPythonSystem::Instance().SetSnowModeOption(iOpt);

	return Py_BuildNone();
}

PyObject *systemGetSnowModeOption(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSnowModeOption());
}
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
PyObject *systemGetEffectLevel(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetEffectLevel());
}

PyObject *systemSetEffectLevel(PyObject *poSelf, PyObject *poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level >= 0)
		CPythonSystem::Instance().SetEffectLevel(level);

	return Py_BuildNone();
}

PyObject *systemGetPrivateShopLevel(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetPrivateShopLevel());
}

PyObject *systemSetPrivateShopLevel(PyObject *poSelf, PyObject *poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level >= 0)
		CPythonSystem::Instance().SetPrivateShopLevel(level);

	return Py_BuildNone();
}

PyObject *systemGetDropItemLevel(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetDropItemLevel());
}

PyObject *systemSetDropItemLevel(PyObject *poSelf, PyObject *poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level >= 0)
		CPythonSystem::Instance().SetDropItemLevel(level);

	return Py_BuildNone();
}

PyObject *systemSetPetStatusFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	CPythonSystem::Instance().SetPetStatusFlag(iFlag);
	return Py_BuildNone();
}

PyObject *systemIsPetStatus(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsPetStatus());
}

PyObject *systemSetNpcNameStatusFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	CPythonSystem::Instance().SetNpcNameStatusFlag(iFlag);
	return Py_BuildNone();
}

PyObject *systemIsNpcNameStatus(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsNpcNameStatus());
}
#endif

#ifdef ENABLE_SHOW_MOB_INFO
PyObject *systemSetShowMobLevel(PyObject *poSelf, PyObject *poArgs)
{
	int iOpt;
	if (!PyTuple_GetInteger(poArgs, 0, &iOpt))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobLevel(iOpt);
	return Py_BuildNone();
}

PyObject *systemIsShowMobLevel(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobLevel());
}

PyObject *systemSetShowMobAIFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iOpt;
	if (!PyTuple_GetInteger(poArgs, 0, &iOpt))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobAIFlag(iOpt);
	return Py_BuildNone();
}

PyObject *systemIsShowMobAIFlag(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobAIFlag());
}
#endif

#ifdef ENABLE_RENEWAL_TEXT_SHADOW
PyObject *systemIsShowOutline(PyObject *poSelf, PyObject *poArgs)
{
	static auto& system = CPythonSystem::Instance();
	return Py_BuildValue("b", system.IsShowOutline());
}

PyObject *systemSetShowOutlineFlag(PyObject *poSelf, PyObject *poArgs)
{
	bool isShow;
	PyTuple_GetBoolean(poArgs, 0, &isShow);

	static auto& system = CPythonSystem::Instance();
	system.SetShowOutlineFlag(isShow);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_FOV_OPTION
PyObject *systemSetFOV(PyObject *poSelf, PyObject *poArgs)
{
	float fValue;
	if (!PyTuple_GetFloat(poArgs, 0, &fValue))
		return Py_BuildException();

	CPythonSystem::Instance().SetFOV(fValue);
	return Py_BuildNone();
}

PyObject *systemGetFOV(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetFOV());
}
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
PyObject *systemSetStoneScale(PyObject *poSelf, PyObject *poArgs)
{
	float fScale;
	if (!PyTuple_GetFloat(poArgs, 0, &fScale))
		return Py_BuildException();

	CPythonSystem::Instance().SetStoneScale(fScale);
	return Py_BuildNone();
}

PyObject *systemGetStoneScale(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetStoneScale());
}
#endif

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
PyObject * systemSetSlotCharEnlargeMode(PyObject * poSelf, PyObject * poArgs)
{
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 0, &iMode))
		return Py_BuildException();

	CPythonSystem::Instance().SetSlotCharEnlargeMode(iMode);
	return Py_BuildNone();
}

PyObject * systemGetSlotCharEnlargeMode(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSlotCharEnlargeMode());
}

PyObject * systemNeedEnlargeSlotChar(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("b", CPythonSystem::Instance().NeedEnlargeSlotChar());
}
#endif

#ifdef ENABLE_TRACK_WINDOW
PyObject* systemSetDungeonTrack(PyObject* poSelf, PyObject* poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();
	CPythonSystem::Instance().SetDungeonTrack(iValue);
	return Py_BuildNone();
}

PyObject* systemGetDungeonTrack(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetDungeonTrack());
}

PyObject* systemSetBossTrack(PyObject* poSelf, PyObject* poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();
	CPythonSystem::Instance().SetBossTrack(iValue);
	return Py_BuildNone();
}

PyObject* systemGetBossTrack(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetBossTrack());
}
#endif

PyObject* systemSetAdvancedOption(PyObject*, PyObject* args)
{
    char* key;
    float value;
    if (!PyTuple_GetString(args, 0, &key) || !PyTuple_GetFloat(args, 1, &value))
        return Py_BuildException();
    CPythonSystem::Instance().SetAdvancedOption(key, value);
    if (strcmp(key, "show_guild_grade") == 0) CPythonTextTail::Instance().RefreshGuildGrades();
    if (strcmp(key, "specular_reflection") == 0) CPythonBackground::Instance().SetSpecularLevel(value != 0 ? 1 : 0);
    if (strcmp(key, "speedtree_wind") == 0) CPythonBackground::Instance().SetRainLevel(CPythonSystem::Instance().GetAdvancedOption("storm") ? 3 : (CPythonSystem::Instance().GetAdvancedOption("rain") ? 2 : 0));
    if (strcmp(key, "env_volume_pos") == 0) SoundEngine::Instance().SetEnvironmentVolume(value);
    return Py_BuildNone();
}
PyObject* systemGetAdvancedOption(PyObject*, PyObject* args)
{
    char* key;
    float fallback = 0.0f;
    if (!PyTuple_GetString(args, 0, &key)) return Py_BuildException();
    if (PyTuple_Size(args) > 1 && !PyTuple_GetFloat(args, 1, &fallback)) return Py_BuildException();
    return Py_BuildValue("f", CPythonSystem::Instance().GetAdvancedOption(key, fallback));
}

void initsystem()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetAdvancedOption", systemSetAdvancedOption, METH_VARARGS },
		{ "GetAdvancedOption", systemGetAdvancedOption, METH_VARARGS },
		{ "GetWidth",					systemGetWidth,					METH_VARARGS },
		{ "GetHeight",					systemGetHeight,				METH_VARARGS },

		{ "SetInterfaceHandler",		systemSetInterfaceHandler,		METH_VARARGS },
		{ "DestroyInterfaceHandler",	systemDestroyInterfaceHandler,	METH_VARARGS },
		{ "ReserveResource",			systemReserveResource,			METH_VARARGS },

		{ "isInterfaceConfig",			systemisInterfaceConfig,		METH_VARARGS },
		{ "SaveWindowStatus",			systemSaveWindowStatus,			METH_VARARGS },
		{ "GetWindowStatus",			systemGetWindowStatus,			METH_VARARGS },

		{ "GetResolutionCount",			systemGetResolutionCount,		METH_VARARGS },
		{ "GetFrequencyCount",			systemGetFrequencyCount,		METH_VARARGS },

		{ "GetCurrentResolution",		systemGetCurrentResolution,		METH_VARARGS },

		{ "GetResolution",				systemGetResolution,			METH_VARARGS },
		{ "GetFrequency",				systemGetFrequency,				METH_VARARGS },

		{ "ApplyConfig",				systemApplyConfig,				METH_VARARGS },
		{ "SetConfig",					systemSetConfig,				METH_VARARGS },
		{ "SaveConfig",					systemSaveConfig,				METH_VARARGS },
		{ "GetConfig",					systemGetConfig,				METH_VARARGS },

		{ "SetSaveID",					systemSetSaveID,				METH_VARARGS },
		{ "isSaveID",					systemisSaveID,					METH_VARARGS },
		{ "GetSaveID",					systemGetSaveID,				METH_VARARGS },

		{ "GetMusicVolume",				systemGetMusicVolume,			METH_VARARGS },
		{ "GetSoundVolume",				systemGetSoundVolume,			METH_VARARGS },

		{ "SetMusicVolume",				systemSetMusicVolume,			METH_VARARGS },
		{ "SetSoundVolumef",			systemSetSoundVolumef,			METH_VARARGS },
		{ "IsSoftwareCursor",			systemIsSoftwareCursor,			METH_VARARGS },

		{ "SetViewChatFlag",			systemSetViewChatFlag,			METH_VARARGS },
		{ "IsViewChat",					systemIsViewChat,				METH_VARARGS },

		{ "SetAlwaysShowNameFlag",		systemSetAlwaysShowNameFlag,	METH_VARARGS },
		{ "IsAlwaysShowName",			systemIsAlwaysShowName,			METH_VARARGS },

		{ "SetShowDamageFlag",			systemSetShowDamageFlag,		METH_VARARGS },
		{ "IsShowDamage",				systemIsShowDamage,				METH_VARARGS },

		{ "SetShowSalesTextFlag",		systemSetShowSalesTextFlag,		METH_VARARGS },
		{ "IsShowSalesText",			systemIsShowSalesText,			METH_VARARGS },

		{ "GetShadowLevel",				systemGetShadowLevel,			METH_VARARGS },
		{ "SetShadowLevel",				systemSetShadowLevel,			METH_VARARGS },

		{ "GetGraphicsLevel",			systemGetGraphicsLevel,			METH_VARARGS },
		{ "SetGraphicsLevel",			systemSetGraphicsLevel,			METH_VARARGS },

		{ "GetShadowQualityLevel",		systemGetShadowQualityLevel,	METH_VARARGS },
		{ "SetShadowQualityLevel",		systemSetShadowQualityLevel,	METH_VARARGS },

		{ "GetTerrainShadow",			systemGetTerrainShadow,			METH_VARARGS },
		{ "SetTerrainShadow",			systemSetTerrainShadow,			METH_VARARGS },

		{ "GetTreeShadow",				systemGetTreeShadow,			METH_VARARGS },
		{ "SetTreeShadow",				systemSetTreeShadow,			METH_VARARGS },

		{ "GetSharpnessPower",			systemGetSharpnessPower,		METH_VARARGS },
		{ "SetSharpnessPower",			systemSetSharpnessPower,		METH_VARARGS },

#ifdef ENABLE_FOG_FIX
		{ "SetFogMode",					systemSetFogMode, METH_VARARGS },
		{ "IsFogMode",					systemIsFogMode, METH_VARARGS },
#endif

#ifdef ENABLE_ENB_MODE
		{ "SetENBModeStatusFlag",		systemSetENBModeStatusFlag,			METH_VARARGS },
		{ "IsENBModeStatus",			systemIsENBModeStatus,			METH_VARARGS },
#endif

#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
		{ "SetNightModeOption",			systemSetNightModeOption,			METH_VARARGS },
		{ "GetNightModeOption",			systemGetNightModeOption,			METH_VARARGS },

		{ "SetSnowModeOption",			systemSetSnowModeOption,			METH_VARARGS },
		{ "GetSnowModeOption",			systemGetSnowModeOption,			METH_VARARGS },
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
		{ "GetEffectLevel", systemGetEffectLevel,			METH_VARARGS },
		{ "SetEffectLevel", systemSetEffectLevel,			METH_VARARGS },

		{ "GetPrivateShopLevel", systemGetPrivateShopLevel,			METH_VARARGS },
		{ "SetPrivateShopLevel", systemSetPrivateShopLevel,			METH_VARARGS },

		{ "GetDropItemLevel", systemGetDropItemLevel,			METH_VARARGS },
		{ "SetDropItemLevel", systemSetDropItemLevel,			METH_VARARGS },

		{ "SetPetStatusFlag", systemSetPetStatusFlag,			METH_VARARGS },
		{ "IsPetStatus", systemIsPetStatus,			METH_VARARGS },

		{ "SetNpcNameStatusFlag", systemSetNpcNameStatusFlag, METH_VARARGS },
		{ "IsNpcNameStatus", systemIsNpcNameStatus, METH_VARARGS },
#endif

#ifdef ENABLE_SHOW_MOB_INFO
		{ "SetShowMobAIFlag", systemSetShowMobAIFlag,			METH_VARARGS },
		{ "IsShowMobAIFlag", systemIsShowMobAIFlag,			METH_VARARGS },

		{ "SetShowMobLevel", systemSetShowMobLevel, METH_VARARGS },
		{ "IsShowMobLevel", systemIsShowMobLevel, METH_VARARGS },
#endif

#ifdef ENABLE_RENEWAL_TEXT_SHADOW
		{"SetShowOutlineFlag", systemSetShowOutlineFlag, METH_VARARGS},
		{"IsShowOutline", systemIsShowOutline, METH_VARARGS},
#endif

#ifdef ENABLE_FOV_OPTION
		{ "SetFOV", systemSetFOV, METH_VARARGS },
		{ "GetFOV", systemGetFOV, METH_VARARGS },
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
		{ "SetStoneScale", systemSetStoneScale, METH_VARARGS },
		{ "GetStoneScale", systemGetStoneScale, METH_VARARGS },
	#endif

#ifdef ENABLE_TRACK_WINDOW
		{ "SetDungeonTrack", systemSetDungeonTrack, METH_VARARGS },
		{ "GetDungeonTrack", systemGetDungeonTrack, METH_VARARGS },
		{ "SetBossTrack", systemSetBossTrack, METH_VARARGS },
		{ "GetBossTrack", systemGetBossTrack, METH_VARARGS },
#endif
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		{ "SetSlotCharEnlargeMode",		systemSetSlotCharEnlargeMode,		METH_VARARGS },
		{ "GetSlotCharEnlargeMode",		systemGetSlotCharEnlargeMode,		METH_VARARGS },
		{ "NeedEnlargeSlotChar",		systemNeedEnlargeSlotChar,			METH_VARARGS },
#endif


		{ nullptr, nullptr }
	};

	PyObject *poModule = Py_InitModule("systemSetting", s_methods);

	PyModule_AddIntConstant(poModule, "WINDOW_STATUS", CPythonSystem::WINDOW_STATUS);
	PyModule_AddIntConstant(poModule, "WINDOW_INVENTORY", CPythonSystem::WINDOW_INVENTORY);
	PyModule_AddIntConstant(poModule, "WINDOW_ABILITY", CPythonSystem::WINDOW_ABILITY);
	PyModule_AddIntConstant(poModule, "WINDOW_SOCIETY", CPythonSystem::WINDOW_SOCIETY);
	PyModule_AddIntConstant(poModule, "WINDOW_JOURNAL", CPythonSystem::WINDOW_JOURNAL);
	PyModule_AddIntConstant(poModule, "WINDOW_COMMAND", CPythonSystem::WINDOW_COMMAND);

	PyModule_AddIntConstant(poModule, "WINDOW_QUICK", CPythonSystem::WINDOW_QUICK);
	PyModule_AddIntConstant(poModule, "WINDOW_GAUGE", CPythonSystem::WINDOW_GAUGE);
	PyModule_AddIntConstant(poModule, "WINDOW_MINIMAP", CPythonSystem::WINDOW_MINIMAP);
	PyModule_AddIntConstant(poModule, "WINDOW_CHAT", CPythonSystem::WINDOW_CHAT);
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
	PyModule_AddIntConstant(poModule, "SLOT_CHAR_ENLARGE_MODE_ON", 0);
	PyModule_AddIntConstant(poModule, "SLOT_CHAR_ENLARGE_MODE_OFF", 1);
	PyModule_AddIntConstant(poModule, "SLOT_CHAR_ENLARGE_MODE_KEY", 2);
#endif
}