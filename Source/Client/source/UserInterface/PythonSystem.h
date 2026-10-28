#pragma once

#include "StdAfx.h"
#include <unordered_map>

class CPythonSystem : public CSingleton<CPythonSystem>
{
	public:
		enum EWindow
		{
			WINDOW_STATUS,
			WINDOW_INVENTORY,
			WINDOW_ABILITY,
			WINDOW_SOCIETY,
			WINDOW_JOURNAL,
			WINDOW_COMMAND,

			WINDOW_QUICK,
			WINDOW_GAUGE,
			WINDOW_MINIMAP,
			WINDOW_CHAT,

			WINDOW_MAX_NUM,
		};

		enum
		{
			FREQUENCY_MAX_NUM  = 30,
			RESOLUTION_MAX_NUM = 100
		};

		typedef struct SResolution
		{
			DWORD width;
			DWORD height;
			DWORD bpp;

			DWORD frequency[20];
			BYTE frequency_count;
		} TResolution;

		typedef struct SWindowStatus
		{
			int isVisible;
			int isMinimized;

			int ixPosition;
			int iyPosition;
			int iHeight;
		} TWindowStatus;

		typedef struct SConfig
		{
			DWORD width;
			DWORD height;
			DWORD bpp;
			DWORD frequency;

			bool is_software_cursor;
			bool is_object_culling;
			int iDistance;
			int iShadowLevel;

			int iShadowQualityLevel;
			int iGrassType;
			int iTerrainShadow;
			int iTreeShadow;
			int iRainLevel;
			int iWindLevel;
			int iSpecularLevel;
			int iBloodLevel;
			int iGraphicsLevel;
			float fSharpnessPower;
			int iShadowTargetLevel;
			float fLightSpecularPower;

			FLOAT music_volume;
			BYTE voice_volume;

			int gamma;

			int isSaveID;
			char SaveID[20];

			bool bWindowed;
			bool bDecompressDDS;
			bool bNoSoundCard;
			bool bUseDefaultIME;
			BYTE bSoftwareTiling;
			bool bViewChat;
			bool bAlwaysShowName;
			bool bShowDamage;
			bool bShowSalesText;
#ifdef ENABLE_FOG_FIX
			bool bFogMode;
#endif
#ifdef ENABLE_ENB_MODE
			bool bENBModeStatus;
#endif
#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
			bool bNightMode;
			bool bSnowMode;
#endif
#ifdef ENABLE_GRAPHIC_ON_OFF
			int iEffectLevel;
			int iPrivateShopLevel;
			int iDropItemLevel;

			bool bPetStatus;
			bool bNpcNameStatus;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
			bool bShowMobLevel;
			bool bShowMobAIFlag;
#endif
#ifdef ENABLE_RENEWAL_TEXT_SHADOW
			bool bShowOutline;
#endif
#ifdef ENABLE_FOV_OPTION
			float fFOV;
#endif
#ifdef ENABLE_STONE_SCALE_OPTION
			float m_fStoneScale;
#endif
#ifdef ENABLE_TRACK_WINDOW
			bool bDungeonTrack;
			bool bBossTrack;
#endif
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
			BYTE bSlotCharEnlargeMode;
#endif
		} TConfig;

	public:
		CPythonSystem();
		virtual ~CPythonSystem();

		void Clear();
		void SetInterfaceHandler(PyObject *poHandler);
		void DestroyInterfaceHandler();

		void SetDefaultConfig();
		bool LoadConfig();
		bool SaveConfig();
		void ApplyConfig();
		void SetConfig(TConfig * set_config);
		TConfig* GetConfig();
		void ChangeSystem();

		bool LoadInterfaceStatus();
		void SaveInterfaceStatus();
		bool isInterfaceConfig();
		const TWindowStatus& GetWindowStatusReference(int iIndex);

		DWORD GetWidth();
		DWORD GetHeight();
		DWORD GetBPP();
		DWORD GetFrequency();

		bool IsSoftwareCursor();
		bool IsWindowed();
		bool IsViewChat();
		bool IsAlwaysShowName();
		bool IsShowDamage();
		bool IsShowSalesText();
		bool IsUseDefaultIME();
		bool IsNoSoundCard();
		void SetViewChatFlag(int iFlag);
		void SetAlwaysShowNameFlag(int iFlag);
		void SetShowDamageFlag(int iFlag);
		void SetShowSalesTextFlag(int iFlag);
#ifdef ENABLE_FOG_FIX
		void SetFogMode(int iFlag);
		bool IsFogMode();
#endif

#ifdef ENABLE_ENB_MODE
		bool IsENBModeStatus();
		void SetENBModeStatusFlag(int iFlag);
#endif

#ifdef ENABLE_ENVIRONMENT_EFFECT_OPTION
		void SetNightModeOption(bool bEnable);
		bool GetNightModeOption() const;

		void SetSnowModeOption(int iFlag);
		bool GetSnowModeOption();
#endif

#ifdef ENABLE_GRAPHIC_ON_OFF
		int GetEffectLevel();
		void SetEffectLevel(unsigned int level);

		int GetPrivateShopLevel();
		void SetPrivateShopLevel(unsigned int level);

		int GetDropItemLevel();
		void SetDropItemLevel(unsigned int level);

		bool IsPetStatus();
		void SetPetStatusFlag(int iFlag);

		bool IsNpcNameStatus();
		void SetNpcNameStatusFlag(int iFlag);
#endif

#ifdef ENABLE_SHOW_MOB_INFO
		void SetShowMobAIFlag(int iFlag);
		bool IsShowMobAIFlag();

		void SetShowMobLevel(int iFlag);
		bool IsShowMobLevel();
#endif

#ifdef ENABLE_RENEWAL_TEXT_SHADOW
		bool IsShowOutline() const;
		void SetShowOutlineFlag(const bool bShow);
#endif

#ifdef ENABLE_FOV_OPTION
		float GetFOV();
		void SetFOV(const float c_fValue);
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
		float GetStoneScale();
		void SetStoneScale(float fScale);
#endif

#ifdef ENABLE_TRACK_WINDOW
		bool GetDungeonTrack() { return m_Config.bDungeonTrack; }
		void SetDungeonTrack(bool flag) { m_Config.bDungeonTrack = flag; }

		bool GetBossTrack() { return m_Config.bBossTrack; }
		void SetBossTrack(bool flag) { m_Config.bBossTrack = flag; }
#endif

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		enum ESlotCharEnlargeMode
		{
			SLOT_CHAR_ENLARGE_MODE_ALWAYS = 0,
			SLOT_CHAR_ENLARGE_MODE_NEVER = 1,
			SLOT_CHAR_ENLARGE_MODE_KEY = 2,
			SLOT_CHAR_ENLARGE_MODE_AUTO = 2,
		};

		bool NeedEnlargeSlotChar();
		void SetSlotCharEnlargeMode(BYTE bMode);
		BYTE GetSlotCharEnlargeMode() const;
#endif

		void SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight);

		int IsSaveID();
		const char *GetSaveID();
		void SetSaveID(int iValue, const char *c_szSaveID);

		void GetDisplaySettings();

		int GetResolutionCount();
		int GetFrequencyCount(int index);
		bool GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp);
		bool GetFrequency(int index, int freq_index, OUT DWORD *frequncy);
		int GetResolutionIndex(DWORD width, DWORD height, DWORD bpp);
		int GetFrequencyIndex(int res_index, DWORD frequency);
		bool isViewCulling();

		FLOAT GetMusicVolume();
		int GetSoundVolume();
		void SetMusicVolume(float fVolume);
		void SetSoundVolumef(float fVolume);

		int GetDistance();
		int GetShadowLevel();
		void SetShadowLevel(unsigned int level);

		float GetSpecularPower();
		void SetSpecularPower(float fSpecular);

		float GetSharpnessPower();
		void SetSharpnessPower(float fsharpness);

		int GetGraphicsLevel();
		void SetGraphicsLevel(unsigned int level);

		int GetGrassType();
		void SetGrassType(unsigned int level);

		int GetShadowQualityLevel();
		void SetShadowQualityLevel(unsigned int level);

		int GetTerrainShadow();
		void SetTerrainShadow(unsigned int level);

		int GetTreeShadow();
		void SetTreeShadow(unsigned int level);

		int GetRainLevel();
		void SetRainLevel(unsigned int level);

		int GetSpecularLevel();
		void SetSpecularLevel(unsigned int level);

		int GetBloodLevel();
		void SetBloodLevel(unsigned int level);

		int GetWindLevel();
		void SetWindLevel(unsigned int level);

		bool IsSoftwareTiling();
		void SetSoftwareTiling(bool isEnable);

	public:
		float GetAdvancedOption(const char* key, float fallback = 0.0f) const
		{
			auto found = m_advancedOptions.find(key);
			return found == m_advancedOptions.end() ? fallback : found->second;
		}
		void SetAdvancedOption(const char* key, float value) { m_advancedOptions[key] = value; }

	protected:
		// unordered_map: this is read via GetAdvancedOption() dozens of times per actor
		// per frame (InstanceBase::Update/Render, InstanceBaseEffect, PythonTextTail,
		// PythonMiniMap) -- an ordered std::map's O(log n) multi-comparison string lookup
		// showed up as the stutter when many players/mobs are on screen at once.
		std::unordered_map<std::string, float> m_advancedOptions;
	protected:
		TResolution m_ResolutionList[RESOLUTION_MAX_NUM];
		int m_ResolutionCount;

		TConfig m_Config;
		TConfig m_OldConfig;

		bool m_isInterfaceConfig;
		PyObject *m_poInterfaceHandler;
		TWindowStatus m_WindowStatus[WINDOW_MAX_NUM];
};