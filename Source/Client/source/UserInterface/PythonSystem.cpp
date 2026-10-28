#include "StdAfx.h"
#include "PythonSystem.h"
#include "PythonApplication.h"
#include "PythonBackground.h"
#include "../SphereLib/frustum.h"

#define DEFAULT_VALUE_ALWAYS_SHOW_NAME true

void CPythonSystem::SetInterfaceHandler(PyObject *poHandler)
{
	m_poInterfaceHandler = poHandler;
}

void CPythonSystem::DestroyInterfaceHandler()
{
	m_poInterfaceHandler = nullptr;
}

void CPythonSystem::SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight)
{
	m_WindowStatus[iIndex].isVisible = iVisible;
	m_WindowStatus[iIndex].isMinimized = iMinimized;
	m_WindowStatus[iIndex].ixPosition = ix;
	m_WindowStatus[iIndex].iyPosition = iy;
	m_WindowStatus[iIndex].iHeight = iHeight;
}

void CPythonSystem::GetDisplaySettings()
{
#ifdef ENABLE_DIRECTX9_UPDATE
	memset(m_ResolutionList, 0, sizeof(TResolution) * RESOLUTION_MAX_NUM);
	m_ResolutionCount = 0;

	LPDIRECT3D9 lpD3D = CPythonGraphic::Instance().GetD3D();

	D3DADAPTER_IDENTIFIER9 d3dAdapterIdentifier;
	D3DDISPLAYMODE d3ddmDesktop;

	lpD3D->GetAdapterIdentifier(0, 0, &d3dAdapterIdentifier);
	lpD3D->GetAdapterDisplayMode(0, &d3ddmDesktop);

	DWORD dwNumAdapterModes = lpD3D->GetAdapterModeCount(0, d3ddmDesktop.Format);

	for (UINT iMode = 0; iMode < dwNumAdapterModes; iMode++)
	{
		D3DDISPLAYMODE DisplayMode;
		lpD3D->EnumAdapterModes(0, d3ddmDesktop.Format, iMode, &DisplayMode);
#else
	memset(m_ResolutionList, 0, sizeof(TResolution) * RESOLUTION_MAX_NUM);
	m_ResolutionCount = 0;

	LPDIRECT3D8 lpD3D = CPythonGraphic::Instance().GetD3D();

	D3DADAPTER_IDENTIFIER8 d3dAdapterIdentifier;
	D3DDISPLAYMODE d3ddmDesktop;

	lpD3D->GetAdapterIdentifier(0, D3DENUM_NO_WHQL_LEVEL, &d3dAdapterIdentifier);
	lpD3D->GetAdapterDisplayMode(0, &d3ddmDesktop);

	DWORD dwNumAdapterModes = lpD3D->GetAdapterModeCount(0);

	for (UINT iMode = 0; iMode < dwNumAdapterModes; iMode++)
	{
		D3DDISPLAYMODE DisplayMode;
		lpD3D->EnumAdapterModes(0, iMode, &DisplayMode);
#endif
		DWORD bpp = 0;

		if (DisplayMode.Width < 800 || DisplayMode.Height < 600)
			continue;

		if (DisplayMode.Format == D3DFMT_R5G6B5)
			bpp = 16;
		else if (DisplayMode.Format == D3DFMT_X8R8G8B8)
			bpp = 32;
		else
			continue;

		int check_res = false;

		for (int i = 0; !check_res && i < m_ResolutionCount; ++i)
		{
			if (m_ResolutionList[i].bpp != bpp ||
				m_ResolutionList[i].width != DisplayMode.Width ||
				m_ResolutionList[i].height != DisplayMode.Height)
				continue;

			int check_fre = false;

			for (int j = 0; j < m_ResolutionList[i].frequency_count; ++j)
			{
				if (m_ResolutionList[i].frequency[j] == DisplayMode.RefreshRate)
				{
					check_fre = true;
					break;
				}
			}

			if (!check_fre)
				if (m_ResolutionList[i].frequency_count < FREQUENCY_MAX_NUM)
					m_ResolutionList[i].frequency[m_ResolutionList[i].frequency_count++] = DisplayMode.RefreshRate;

			check_res = true;
		}

		if (!check_res)
		{
			if (m_ResolutionCount < RESOLUTION_MAX_NUM)
			{
				m_ResolutionList[m_ResolutionCount].width			= DisplayMode.Width;
				m_ResolutionList[m_ResolutionCount].height			= DisplayMode.Height;
				m_ResolutionList[m_ResolutionCount].bpp				= bpp;
				m_ResolutionList[m_ResolutionCount].frequency[0]	= DisplayMode.RefreshRate;
				m_ResolutionList[m_ResolutionCount].frequency_count	= 1;

				++m_ResolutionCount;
			}
		}
	}
}

int	CPythonSystem::GetResolutionCount()
{
	return m_ResolutionCount;
}

int CPythonSystem::GetFrequencyCount(int index)
{
	if (index >= m_ResolutionCount)
		return 0;

    return m_ResolutionList[index].frequency_count;
}

bool CPythonSystem::GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp)
{
	if (index >= m_ResolutionCount)
		return false;

	*width = m_ResolutionList[index].width;
	*height = m_ResolutionList[index].height;
	*bpp = m_ResolutionList[index].bpp;
	return true;
}

bool CPythonSystem::GetFrequency(int index, int freq_index, OUT DWORD *frequncy)
{
	if (index >= m_ResolutionCount)
		return false;

	if (freq_index >= m_ResolutionList[index].frequency_count)
		return false;

	*frequncy = m_ResolutionList[index].frequency[freq_index];
	return true;
}

int	CPythonSystem::GetResolutionIndex(DWORD width, DWORD height, DWORD bit)
{
	DWORD re_width, re_height, re_bit;
	int i = 0;

	while (GetResolution(i, &re_width, &re_height, &re_bit))
	{
		if (re_width == width)
			if (re_height == height)
				if (re_bit == bit)
					return i;
		i++;
	}

	return 0;
}

int	CPythonSystem::GetFrequencyIndex(int res_index, DWORD frequency)
{
	DWORD re_frequency;
	int i = 0;

	while (GetFrequency(res_index, i, &re_frequency))
	{
		if (re_frequency == frequency)
			return i;

		i++;
	}

	return 0;
}

DWORD CPythonSystem::GetWidth()
{
	return m_Config.width;
}

DWORD CPythonSystem::GetHeight()
{
	return m_Config.height;
}

DWORD CPythonSystem::GetBPP()
{
	return m_Config.bpp;
}

DWORD CPythonSystem::GetFrequency()
{
	return m_Config.frequency;
}

bool CPythonSystem::IsNoSoundCard()
{
	return m_Config.bNoSoundCard;
}

bool CPythonSystem::IsSoftwareCursor()
{
	return m_Config.is_software_cursor;
}

float CPythonSystem::GetMusicVolume()
{
	return m_Config.music_volume;
}

int CPythonSystem::GetSoundVolume()
{
	return m_Config.voice_volume;
}

void CPythonSystem::SetMusicVolume(float fVolume)
{
	m_Config.music_volume = fVolume;
}

void CPythonSystem::SetSoundVolumef(float fVolume)
{
	m_Config.voice_volume = int(5 * fVolume);
}

int CPythonSystem::GetDistance()
{
	return m_Config.iDistance;
}

int CPythonSystem::GetShadowLevel()
{
	return m_Config.iShadowLevel;
}

void CPythonSystem::SetShadowLevel(unsigned int level)
{
	m_Config.iShadowLevel = MIN(level, 5);
	CPythonBackground::instance().RefreshShadowLevel();
}

int CPythonSystem::GetGraphicsLevel()
{
	return m_Config.iGraphicsLevel;
}

void CPythonSystem::SetGraphicsLevel(unsigned int level)
{
	m_Config.iGraphicsLevel = MIN(level, 2);
	CPythonBackground::instance().RefreshGraphicsLevel();
}

int CPythonSystem::GetRainLevel()
{
	return MIN(MAX(m_Config.iRainLevel,
		CPythonBackground::RAIN_0), /* min */
		CPythonBackground::RAIN_3 /* max */
	);
}

void CPythonSystem::SetRainLevel(unsigned int level)
{
	m_Config.iRainLevel = MIN(MAX(level,
		CPythonBackground::RAIN_0), /* min */
		CPythonBackground::RAIN_3 /* max */
	);
}

int CPythonSystem::GetWindLevel()
{
	return MIN(MAX(m_Config.iWindLevel,
		CPythonBackground::WIND_0), /* min */
		CPythonBackground::WIND_3 /* max */
	);
}

void CPythonSystem::SetWindLevel(unsigned int level)
{
	m_Config.iWindLevel = MIN(MAX(level,
		CPythonBackground::WIND_0), /* min */
		CPythonBackground::WIND_3 /* max */
	);
}

int CPythonSystem::GetSpecularLevel()
{
	return MIN(MAX(m_Config.iSpecularLevel,
		CPythonBackground::SPECULAR_on), /* min */
		CPythonBackground::SPECULAR_off /* max */
	);
}

void CPythonSystem::SetSpecularLevel(unsigned int level)
{
	m_Config.iSpecularLevel = MIN(MAX(level,
		CPythonBackground::SPECULAR_on), /* min */
		CPythonBackground::SPECULAR_off /* max */
	);
}

int CPythonSystem::GetGrassType()
{
	return m_Config.iGrassType;
}

void CPythonSystem::SetGrassType(unsigned int level)
{
	m_Config.iGrassType = level;
}

int CPythonSystem::GetBloodLevel()
{
	return MIN(MAX(m_Config.iBloodLevel,
		CPythonBackground::BLOOD_on), /* min */
		CPythonBackground::BLOOD_off /* max */
	);
}

void CPythonSystem::SetBloodLevel(unsigned int level)
{
	m_Config.iBloodLevel = MIN(MAX(level,
		CPythonBackground::BLOOD_on), /* min */
		CPythonBackground::BLOOD_off /* max */
	);
}

void CPythonSystem::SetSoftwareTiling(bool isEnable)
{
	if (isEnable)
		m_Config.bSoftwareTiling = 1;
	else
		m_Config.bSoftwareTiling = 2;
}

bool CPythonSystem::IsSoftwareTiling()
{
	if (m_Config.bSoftwareTiling == 1)
		return true;
	return false;
}

int CPythonSystem::GetShadowQualityLevel()
{
	return MIN(MAX(m_Config.iShadowQualityLevel,
		CPythonBackground::SHADOW_QUALITY_BAD), /* min */
		CPythonBackground::SHADOW_QUALITY_ULTRA /* max */
	);
}

void CPythonSystem::SetShadowQualityLevel(unsigned int level)
{
	m_Config.iShadowQualityLevel = MIN(MAX(level,
		CPythonBackground::SHADOW_QUALITY_BAD), /* min */
		CPythonBackground::SHADOW_QUALITY_ULTRA /* max */
	);
	CPythonBackground::instance().SetShadowQualityLevel(m_Config.iShadowQualityLevel);
}

int CPythonSystem::GetTerrainShadow()
{
	return MIN(MAX(m_Config.iTerrainShadow,
		CPythonBackground::TERRAIN_SHADOW_off), /* min */
		CPythonBackground::TERRAIN_SHADOW_on /* max */
	);
}

void CPythonSystem::SetTerrainShadow(unsigned int level)
{
	m_Config.iTerrainShadow = MIN(MAX(level,
		CPythonBackground::TERRAIN_SHADOW_off), /* min */
		CPythonBackground::TERRAIN_SHADOW_on /* max */
	);
	CPythonBackground::instance().SetTerrainShadow(m_Config.iTerrainShadow);
}

int CPythonSystem::GetTreeShadow()
{
	return MIN(MAX(m_Config.iTreeShadow,
		CPythonBackground::TREE_SHADOW_on), /* min */
		CPythonBackground::TREE_SHADOW_all /* max */
	);
}

void CPythonSystem::SetTreeShadow(unsigned int level)
{
	m_Config.iTreeShadow = MIN(MAX(level,
		CPythonBackground::TREE_SHADOW_on), /* min */
		CPythonBackground::TREE_SHADOW_all /* max */
	);
	CPythonBackground::instance().SetTreeShadow(m_Config.iTreeShadow);
}

void CPythonSystem::SetSpecularPower(float fspecular)
{
	m_Config.fLightSpecularPower = fMINMAX(c_fMinSpecularPower, fspecular, c_fMaxSpecularPower);
}

float CPythonSystem::GetSpecularPower()
{
	return m_Config.fLightSpecularPower;
}

void CPythonSystem::SetSharpnessPower(float fsharpness)
{
	m_Config.fSharpnessPower = fMINMAX(c_fMinSharpnessPower, fsharpness, c_fMaxSharpnessPower);
}

float CPythonSystem::GetSharpnessPower()
{
	Frustum::Instance().fSharpnessPower = m_Config.fSharpnessPower;
	return m_Config.fSharpnessPower;
}

int CPythonSystem::IsSaveID()
{
	return m_Config.isSaveID;
}

const char *CPythonSystem::GetSaveID()
{
	return m_Config.SaveID;
}

bool CPythonSystem::isViewCulling()
{
	return m_Config.is_object_culling;
}

void CPythonSystem::SetSaveID(int iValue, const char *c_szSaveID)
{
	if (iValue != 1)
		return;

	m_Config.isSaveID = iValue;
	strncpy(m_Config.SaveID, c_szSaveID, sizeof(m_Config.SaveID) - 1);
}

CPythonSystem::TConfig * CPythonSystem::GetConfig()
{
	return &m_Config;
}

void CPythonSystem::SetConfig(TConfig * pNewConfig)
{
	m_Config = *pNewConfig;
}

void CPythonSystem::SetDefaultConfig()
{
	memset(&m_Config, 0, sizeof(m_Config));

	m_Config.width = 1024;
	m_Config.height = 768;
	m_Config.bpp = 32;

	m_Config.bWindowed = false;
	m_Config.is_software_cursor = false;
	m_Config.is_object_culling = true;
	m_Config.iDistance = 3;

	m_Config.gamma = 3;
	m_Config.music_volume = 1.0f;
	m_Config.voice_volume = 5;

	m_Config.bDecompressDDS = 0;
	m_Config.iShadowLevel = 3;
	m_Config.iGraphicsLevel = 2;
	m_Config.iShadowQualityLevel = CPythonBackground::SHADOW_QUALITY_AVERAGE;
	m_Config.iGrassType = 1;
	m_Config.iBloodLevel = CPythonBackground::BLOOD_on;
	m_Config.iTerrainShadow = CPythonBackground::TERRAIN_SHADOW_on;
	m_Config.iTreeShadow = CPythonBackground::TREE_SHADOW_all;
	m_Config.iRainLevel = CPythonBackground::RAIN_0;
	m_Config.iWindLevel = CPythonBackground::WIND_1;
	m_Config.iSpecularLevel = CPythonBackground::SPECULAR_on;
	m_Config.fSharpnessPower = c_fDefaultSharpnessPower;
	m_Config.fLightSpecularPower = c_fDefaultSpecularPower;
	m_Config.bViewChat = true;
	m_Config.bAlwaysShowName = DEFAULT_VALUE_ALWAYS_SHOW_NAME;
	m_Config.bShowDamage = true;
	m_Config.bShowSalesText = true;
#ifdef ENABLE_FOG_FIX
	m_Config.bFogMode = true;
#endif
#ifdef ENABLE_ENB_MODE
	m_Config.bENBModeStatus = false;
#endif
#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
	m_Config.bNightMode = true;
	m_Config.bSnowMode = true;
#endif
#ifdef ENABLE_GRAPHIC_ON_OFF
	m_Config.iEffectLevel = 0;
	m_Config.iPrivateShopLevel = 0;
	m_Config.iDropItemLevel = 0;

	m_Config.bPetStatus = false;
	m_Config.bNpcNameStatus = false;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
	m_Config.bShowMobLevel = true;
	m_Config.bShowMobAIFlag = true;
#endif
#ifdef ENABLE_RENEWAL_TEXT_SHADOW
	m_Config.bShowOutline = true;
#endif
#ifdef ENABLE_FOV_OPTION
	m_Config.fFOV = c_fDefaultCameraPerspective;
#endif
#ifdef ENABLE_STONE_SCALE_OPTION
	m_Config.m_fStoneScale = 1.0f;
#endif
#ifdef ENABLE_TRACK_WINDOW
	m_Config.bDungeonTrack = false;
	m_Config.bBossTrack = false;
#endif
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
	m_Config.bSlotCharEnlargeMode = SLOT_CHAR_ENLARGE_MODE_KEY;
#endif
}

bool CPythonSystem::IsWindowed()
{
	return m_Config.bWindowed;
}

bool CPythonSystem::IsViewChat()
{
	return m_Config.bViewChat;
}

void CPythonSystem::SetViewChatFlag(int iFlag)
{
	m_Config.bViewChat = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsAlwaysShowName()
{
	return m_Config.bAlwaysShowName;
}

void CPythonSystem::SetAlwaysShowNameFlag(int iFlag)
{
	m_Config.bAlwaysShowName = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsShowDamage()
{
	return m_Config.bShowDamage;
}

void CPythonSystem::SetShowDamageFlag(int iFlag)
{
	m_Config.bShowDamage = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsShowSalesText()
{
	return m_Config.bShowSalesText;
}

void CPythonSystem::SetShowSalesTextFlag(int iFlag)
{
	m_Config.bShowSalesText = iFlag == 1 ? true : false;
}

#ifdef ENABLE_FOG_FIX
void CPythonSystem::SetFogMode(int iFlag)
{
	m_Config.bFogMode = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsFogMode()
{
	return m_Config.bFogMode;
}
#endif

#ifdef ENABLE_ENB_MODE
bool CPythonSystem::IsENBModeStatus()
{
	return m_Config.bENBModeStatus;
}
void CPythonSystem::SetENBModeStatusFlag(int iFlag)
{
	m_Config.bENBModeStatus = iFlag == 1 ? true : false;
}
#endif

#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
void CPythonSystem::SetNightModeOption(bool bEnable)
{
	m_Config.bNightMode = bEnable;
}

bool CPythonSystem::GetNightModeOption() const
{
	return m_Config.bNightMode;
}

void CPythonSystem::SetSnowModeOption(int iOpt)
{
	m_Config.bSnowMode = iOpt == 1 ? true : false;

	if (m_Config.bSnowMode)
		CPythonBackground::Instance().EnableSnowEnvironment();
	else
		CPythonBackground::Instance().DisableSnowEnvironment();
}

bool CPythonSystem::GetSnowModeOption()
{
	return m_Config.bSnowMode;
}
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
int CPythonSystem::GetEffectLevel()
{
	return m_Config.iEffectLevel;
}

void CPythonSystem::SetEffectLevel(unsigned int level)
{
	m_Config.iEffectLevel = MIN(level, 5);
}

int CPythonSystem::GetPrivateShopLevel()
{
	return m_Config.iPrivateShopLevel;
}

void CPythonSystem::SetPrivateShopLevel(unsigned int level)
{
	m_Config.iPrivateShopLevel = MIN(level, 5);
}

int CPythonSystem::GetDropItemLevel()
{
	return m_Config.iDropItemLevel;
}

void CPythonSystem::SetDropItemLevel(unsigned int level)
{
	m_Config.iDropItemLevel = MIN(level, 5);
}

bool CPythonSystem::IsPetStatus()
{
	return m_Config.bPetStatus;
}
void CPythonSystem::SetPetStatusFlag(int iFlag)
{
	m_Config.bPetStatus = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsNpcNameStatus()
{
	return m_Config.bNpcNameStatus;
}

void CPythonSystem::SetNpcNameStatusFlag(int iFlag)
{
	m_Config.bNpcNameStatus = iFlag == 1 ? true : false;
}
#endif

#ifdef ENABLE_SHOW_MOB_INFO
void CPythonSystem::SetShowMobLevel(int iOpt)
{
	m_Config.bShowMobLevel = iOpt == 1 ? true : false;
}

bool CPythonSystem::IsShowMobLevel()
{
	return m_Config.bShowMobLevel;
}

void CPythonSystem::SetShowMobAIFlag(int iOpt)
{
	m_Config.bShowMobAIFlag = iOpt == 1 ? true : false;
}

bool CPythonSystem::IsShowMobAIFlag()
{
	return m_Config.bShowMobAIFlag;
}
#endif

#ifdef ENABLE_RENEWAL_TEXT_SHADOW
bool CPythonSystem::IsShowOutline() const
{
	return m_Config.bShowOutline;
}

void CPythonSystem::SetShowOutlineFlag(const bool bShow)
{
	m_Config.bShowOutline = bShow == 1 ? true : false;
}
#endif

#ifdef ENABLE_FOV_OPTION
float CPythonSystem::GetFOV()
{
	return m_Config.fFOV;
}

void CPythonSystem::SetFOV(float fFlag)
{
	m_Config.fFOV = fMINMAX(c_fMinCameraPerspective, fFlag, c_fMaxCameraPerspective);
}
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
float CPythonSystem::GetStoneScale()
{
	return m_Config.m_fStoneScale;
}

void CPythonSystem::SetStoneScale(float fScale)
{
	m_Config.m_fStoneScale = fScale;
}
#endif

bool CPythonSystem::IsUseDefaultIME()
{
	return m_Config.bUseDefaultIME;
}

bool CPythonSystem::LoadConfig()
{
	FILE * fp = nullptr;

	if (NULL == (fp = fopen("metin2.cfg", "rt")))
		return false;

	char buf[256];
	char command[256];
	char value[256];

	while (fgets(buf, 256, fp))
	{
		if (sscanf(buf, " %s %s\n", command, value) == EOF)
			break;

		if (!stricmp(command, "WIDTH"))
			m_Config.width = atoi(value);
		else if (!stricmp(command, "HEIGHT"))
			m_Config.height	= atoi(value);
		else if (!stricmp(command, "BPP"))
			m_Config.bpp = atoi(value);
		else if (!stricmp(command, "FREQUENCY"))
			m_Config.frequency = atoi(value);
		else if (!stricmp(command, "SOFTWARE_CURSOR"))
			m_Config.is_software_cursor = atoi(value) ? true : false;
		else if (!stricmp(command, "OBJECT_CULLING"))
			m_Config.is_object_culling = atoi(value) ? true : false;
		else if (!stricmp(command, "VISIBILITY"))
			m_Config.iDistance = atoi(value);
		else if (!stricmp(command, "MUSIC_VOLUME"))
		{
			if(strchr(value, '.') == 0)
			{
				m_Config.music_volume = pow(10.0f, (-1.0f + (((float) atoi(value)) / 5.0f)));
				if(atoi(value) == 0)
					m_Config.music_volume = 0.0f;
			}
			else
				m_Config.music_volume = atof(value);
		}
		else if (!stricmp(command, "VOICE_VOLUME"))
			m_Config.voice_volume = (char) atoi(value);
		else if (!stricmp(command, "GAMMA"))
			m_Config.gamma = atoi(value);
		else if (!stricmp(command, "IS_SAVE_ID"))
			m_Config.isSaveID = atoi(value);
		else if (!stricmp(command, "SAVE_ID"))
			strncpy(m_Config.SaveID, value, 20);
		else if (!stricmp(command, "PRE_LOADING_DELAY_TIME"))
			g_iLoadingDelayTime = atoi(value);
		else if (!stricmp(command, "WINDOWED"))
		{
			m_Config.bWindowed = atoi(value) == 1 ? true : false;
		}
		else if (!stricmp(command, "USE_DEFAULT_IME"))
			m_Config.bUseDefaultIME = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHADOW_LEVEL"))
			m_Config.iShadowLevel = atoi(value);
		else if (!stricmp(command, "SHARPNESS"))
			m_Config.fSharpnessPower = atof(value);
		else if (!stricmp(command, "SPECULAR_POWER"))
			m_Config.fLightSpecularPower = atof(value);
		else if (!stricmp(command, "SHADOW_QUALITY_LEVEL"))
			m_Config.iShadowQualityLevel = atoi(value);
		else if (!stricmp(command, "TERRAIN_SHADOW"))
			m_Config.iTerrainShadow = atoi(value);
		else if (!stricmp(command, "SHADOW_DETAILS"))
			m_Config.iTreeShadow = atoi(value);
		else if (!stricmp(command, "RAIN_LEVEL"))
			m_Config.iRainLevel = atoi(value);
		else if (!stricmp(command, "GRASS_TYPE"))
			m_Config.iGrassType = atoi(value);
		else if (!stricmp(command, "GRAPHICS_LEVEL"))
			m_Config.iGraphicsLevel = atoi(value);
		else if (!stricmp(command, "SPECULAR_LEVEL"))
			m_Config.iSpecularLevel = atoi(value);
		else if (!stricmp(command, "WIND_LEVEL"))
			m_Config.iWindLevel = atoi(value);
		else if (!stricmp(command, "DECOMPRESSED_TEXTURE"))
			m_Config.bDecompressDDS = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "NO_SOUND_CARD"))
			m_Config.bNoSoundCard = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "VIEW_CHAT"))
			m_Config.bViewChat = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "ALWAYS_VIEW_NAME"))
			m_Config.bAlwaysShowName = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHOW_DAMAGE"))
			m_Config.bShowDamage = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHOW_SALESTEXT"))
			m_Config.bShowSalesText = atoi(value) == 1 ? true : false;
#ifdef ENABLE_FOG_FIX
		else if (!stricmp(command, "FOG_MODE_ON"))
			m_Config.bFogMode = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_ENB_MODE
		else if (!stricmp(command, "ENB_MODE_STATUS"))
			m_Config.bENBModeStatus = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
		else if (!stricmp(command, "NIGHT_MODE_ON"))
			m_Config.bNightMode = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SNOW_MODE_ON"))
			m_Config.bSnowMode = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_GRAPHIC_ON_OFF
		else if (!stricmp(command, "EFFECT_LEVEL"))
			m_Config.iEffectLevel = atoi(value);
		else if (!stricmp(command, "PRIVATE_SHOP_LEVEL"))
			m_Config.iPrivateShopLevel = atoi(value);
		else if (!stricmp(command, "DROP_ITEM_LEVEL"))
			m_Config.iDropItemLevel = atoi(value);

		else if (!stricmp(command, "PET_STATUS"))
			m_Config.bPetStatus = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "NPC_NAME_STATUS"))
			m_Config.bNpcNameStatus = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
		else if (!stricmp(command, "SHOW_MOBLEVEL"))
			m_Config.bShowMobLevel = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHOW_MOBAIFLAG"))
			m_Config.bShowMobAIFlag = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_RENEWAL_TEXT_SHADOW
		else if (!stricmp(command, "SHADOW_TEXT"))
			m_Config.bShowOutline = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_FOV_OPTION
		else if (!stricmp(command, "FOV"))
			m_Config.fFOV = atof(value);
#endif
#ifdef ENABLE_STONE_SCALE_OPTION
		else if (!stricmp(command, "STONE_SCALE"))
			if (strchr(value, '.') == 0)
			{
				m_Config.m_fStoneScale = pow(10.0f, (-1.0f + (((float)atoi(value)) / 5.0f)));
				if (atoi(value) == 0)
					m_Config.m_fStoneScale = 0.0f;
			}
			else
				m_Config.m_fStoneScale = atof(value);
#endif
#ifdef ENABLE_TRACK_WINDOW
		else if (!stricmp(command, "DUNGEON_TRACK"))
			m_Config.bDungeonTrack = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "BOSS_TRACK"))
			m_Config.bBossTrack = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		else if (!stricmp(command, "SLOT_CHAR_ENLARGE_MODE"))
			m_Config.bSlotCharEnlargeMode = atoi(value);
#endif
	}

	if (m_Config.bWindowed)
	{
		unsigned screen_width = GetSystemMetrics(SM_CXFULLSCREEN);
		unsigned screen_height = GetSystemMetrics(SM_CYFULLSCREEN);

		if (m_Config.width >= screen_width)
		{
			m_Config.width = screen_width;
		}
		if (m_Config.height >= screen_height) //Fix
		{
			int config_height = m_Config.height;
			int difference = (config_height-screen_height)+7;
			m_Config.height = config_height - difference;
		}
	}

	m_OldConfig = m_Config;

	fclose(fp);

	return true;
}

bool CPythonSystem::SaveConfig()
{
	FILE *fp;

	if (NULL == (fp = fopen("metin2.cfg", "wt")))
		return false;

	fprintf(fp, "WIDTH						%d\n"
				"HEIGHT						%d\n"
				"BPP						%d\n"
				"FREQUENCY					%d\n"
				"SOFTWARE_CURSOR			%d\n"
				"OBJECT_CULLING				%d\n"
				"VISIBILITY					%d\n"
				"MUSIC_VOLUME				%.3f\n"
				"VOICE_VOLUME				%d\n"
				"GAMMA						%d\n"
				"IS_SAVE_ID					%d\n"
				"SAVE_ID					%s\n"
				"PRE_LOADING_DELAY_TIME		%d\n"
				"DECOMPRESSED_TEXTURE		%d\n",
				m_Config.width,
				m_Config.height,
				m_Config.bpp,
				m_Config.frequency,
				m_Config.is_software_cursor,
				m_Config.is_object_culling,
				m_Config.iDistance,
				m_Config.music_volume,
				m_Config.voice_volume,
				m_Config.gamma,
				m_Config.isSaveID,
				m_Config.SaveID,
				g_iLoadingDelayTime,
				m_Config.bDecompressDDS);

	if (m_Config.bWindowed == 1)
		fprintf(fp, "WINDOWED				%d\n", m_Config.bWindowed);
	if (m_Config.bViewChat == 0)
		fprintf(fp, "VIEW_CHAT				%d\n", m_Config.bViewChat);
	if (m_Config.bAlwaysShowName != DEFAULT_VALUE_ALWAYS_SHOW_NAME)
		fprintf(fp, "ALWAYS_VIEW_NAME		%d\n", m_Config.bAlwaysShowName);
	if (m_Config.bShowDamage == 0)
		fprintf(fp, "SHOW_DAMAGE			%d\n", m_Config.bShowDamage);
	if (m_Config.bShowSalesText == 0)
		fprintf(fp, "SHOW_SALESTEXT			%d\n", m_Config.bShowSalesText);
#ifdef ENABLE_FOG_FIX
	if (m_Config.bFogMode == 0)
		fprintf(fp, "FOG_MODE_ON			%d\n", m_Config.bFogMode);
#endif
#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
	fprintf(fp, "NIGHT_MODE_ON				%d\n", m_Config.bNightMode);
	fprintf(fp, "SNOW_MODE_ON				%d\n", m_Config.bSnowMode);
#endif
#ifdef ENABLE_ENB_MODE
	fprintf(fp, "ENB_MODE_STATUS			%d\n", m_Config.bENBModeStatus);
#endif
#ifdef ENABLE_GRAPHIC_ON_OFF
	fprintf(fp, "EFFECT_LEVEL				%d\n", m_Config.iEffectLevel);
	fprintf(fp, "PRIVATE_SHOP_LEVEL			%d\n", m_Config.iPrivateShopLevel);
	fprintf(fp, "DROP_ITEM_LEVEL			%d\n", m_Config.iDropItemLevel);
	fprintf(fp, "PET_STATUS					%d\n", m_Config.bPetStatus);
	fprintf(fp, "NPC_NAME_STATUS			%d\n", m_Config.bNpcNameStatus);
#endif
#ifdef ENABLE_SHOW_MOB_INFO
	fprintf(fp, "SHOW_MOBLEVEL				%d\n", m_Config.bShowMobLevel);
	fprintf(fp, "SHOW_MOBAIFLAG				%d\n", m_Config.bShowMobAIFlag);
#endif
#ifdef ENABLE_RENEWAL_TEXT_SHADOW
	fprintf(fp, "SHADOW_TEXT				%d\n", m_Config.bShowOutline);
#endif
#ifdef ENABLE_FOV_OPTION
	fprintf(fp, "FOV						%.2f\n", m_Config.fFOV);
#endif
#ifdef ENABLE_STONE_SCALE_OPTION
	fprintf(fp, "STONE_SCALE				%.3f\n", m_Config.m_fStoneScale);
#endif
#ifdef ENABLE_TRACK_WINDOW
	fprintf(fp, "DUNGEON_TRACK				%d\n", m_Config.bDungeonTrack);
	fprintf(fp, "BOSS_TRACK					%d\n", m_Config.bBossTrack);
#endif
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
	fprintf(fp, "SLOT_CHAR_ENLARGE_MODE	%d\n", m_Config.bSlotCharEnlargeMode);
#endif
	fprintf(fp, "USE_DEFAULT_IME			%d\n", m_Config.bUseDefaultIME);
	fprintf(fp, "SHADOW_LEVEL				%d\n", m_Config.iShadowLevel);
	fprintf(fp, "GRAPHICS_LEVEL				%d\n", m_Config.iGraphicsLevel);
	fprintf(fp, "SHADOW_QUALITY_LEVEL		%d\n", m_Config.iShadowQualityLevel);
	fprintf(fp, "TERRAIN_SHADOW				%d\n", m_Config.iTerrainShadow);
	fprintf(fp, "SHADOW_DETAILS				%d\n", m_Config.iTreeShadow);
	fprintf(fp, "RAIN_LEVEL					%d\n", m_Config.iRainLevel);
	fprintf(fp, "GRASS_TYPE					%d\n", m_Config.iGrassType);
	fprintf(fp, "SHARPNESS					%.2f\n", m_Config.fSharpnessPower);
	fprintf(fp, "SPECULAR_POWER				%.2f\n", m_Config.fLightSpecularPower);
	fprintf(fp, "WIND_LEVEL					%d\n", m_Config.iWindLevel);
	fprintf(fp, "SPECULAR_LEVEL				%d\n", m_Config.iSpecularLevel);
	fprintf(fp, "\n");

	fclose(fp);
	return true;
}

bool CPythonSystem::LoadInterfaceStatus()
{
	FILE * File;
	File = fopen("interface.cfg", "rb");

	if (!File)
		return false;

	fread(m_WindowStatus, 1, sizeof(TWindowStatus) * WINDOW_MAX_NUM, File);
	fclose(File);
	return true;
}

void CPythonSystem::SaveInterfaceStatus()
{
	if (!m_poInterfaceHandler)
		return;

	PyCallClassMemberFunc(m_poInterfaceHandler, "OnSaveInterfaceStatus", Py_BuildValue("()"));

	FILE * File;

	File = fopen("interface.cfg", "wb");

	if (!File)
	{
		TraceError("Cannot open interface.cfg");
		return;
	}

	fwrite(m_WindowStatus, 1, sizeof(TWindowStatus) * WINDOW_MAX_NUM, File);
	fclose(File);
}

bool CPythonSystem::isInterfaceConfig()
{
	return m_isInterfaceConfig;
}

const CPythonSystem::TWindowStatus & CPythonSystem::GetWindowStatusReference(int iIndex)
{
	return m_WindowStatus[iIndex];
}

void CPythonSystem::ApplyConfig()
{
	if (m_OldConfig.gamma != m_Config.gamma)
	{
		float val = 1.0f;

		switch (m_Config.gamma)
		{
			case 0: val = 0.4f;	break;
			case 1: val = 0.7f; break;
			case 2: val = 1.0f; break;
			case 3: val = 1.2f; break;
			case 4: val = 1.4f; break;
		}
		CPythonGraphic::Instance().SetGamma(val);
	}

	if (m_OldConfig.is_software_cursor != m_Config.is_software_cursor)
	{
		if (m_Config.is_software_cursor)
			CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_SOFTWARE);
		else
			CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_HARDWARE);
	}

	m_OldConfig = m_Config;

	ChangeSystem();
}

void CPythonSystem::ChangeSystem()
{
	CSoundManager& rkSndMgr = CSoundManager::Instance();
	rkSndMgr.SetMusicVolume(m_Config.music_volume);
	rkSndMgr.SetSoundVolumeGrade(m_Config.voice_volume);

#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
	CPythonBackground& rkBG = CPythonBackground::Instance();
	rkBG.SetNightModeOption(m_Config.bNightMode);
#endif
}

void CPythonSystem::Clear()
{
	SetInterfaceHandler(nullptr);
}

CPythonSystem::CPythonSystem()
{
	memset(&m_Config, 0, sizeof(TConfig));

	m_poInterfaceHandler = nullptr;

	SetDefaultConfig();

	LoadConfig();

	ChangeSystem();

	if (LoadInterfaceStatus())
		m_isInterfaceConfig = true;
	else
		m_isInterfaceConfig = false;
}

CPythonSystem::~CPythonSystem()
{
	assert(m_poInterfaceHandler==NULL && "CPythonSystem MUST CLEAR!");
}

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
bool CPythonSystem::NeedEnlargeSlotChar()
{
	switch (m_Config.bSlotCharEnlargeMode)
	{
		case SLOT_CHAR_ENLARGE_MODE_ALWAYS:
			return true;
		case SLOT_CHAR_ENLARGE_MODE_NEVER:
			return false;
		case SLOT_CHAR_ENLARGE_MODE_AUTO:
			return CPythonApplication::Instance().IsPressed(DIK_LCONTROL);
		default:
			return false;
	}
}

void CPythonSystem::SetSlotCharEnlargeMode(BYTE bMode)
{
	m_Config.bSlotCharEnlargeMode = bMode;
}

BYTE CPythonSystem::GetSlotCharEnlargeMode() const
{
	return m_Config.bSlotCharEnlargeMode;
}
#endif
