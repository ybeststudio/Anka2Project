#pragma once

enum
{
	PROGRAM_PATH_LENGTH = 256,
};

enum
{
	LIGHT_TYPE_TERRAIN = 0,

	LIGHT_TYPE_UNKNOWN1 = 1,
	LIGHT_TYPE_UNKNOWN2 = 2,
	LIGHT_TYPE_UNKNOWN3 = 3,
	LIGHT_TYPE_UNKNOWN4 = 4,

	LIGHT_TYPE_CHARACTER = 5,
	LIGHT_TYPE_CHARACTER1 = 5,
	LIGHT_TYPE_CHARACTER2 = 6,
	LIGHT_TYPE_CHARACTER3 = 7,
};

const float c_fModel_Script_Version = 1.0f;
const float c_fMotion_Script_Version = 1.0f;
const float c_fLight_Script_Version = 1.0f;
const float c_fLight_Element_Script_Version = 1.0f;

enum
{
	WINDOW_TIMER_ID_EFFECT_TIME_BAR,
	WINDOW_TIMER_ID_OBJECT_PROPERTY_PREVIEW,
	WINDOW_TIMER_ID_TEXTURE_PREVIEW,
	WINDOW_TIMER_ID_EMITTER_GRAPH,
	WINDOW_TIMER_ID_PARTICLE_GRAPH,
	WINDOW_TIMER_ID_LIGHT_GRAPH,
	WINDOW_TIMER_ID_MESH_ALPHA_GRAPH,
};

const std::string c_strYmirWorkPath			= "E:\\Metin2";
const std::string c_strModelDataPath		= "E:\\Metin2";
const std::string c_strAnimationDataPath	= "E:\\Metin2";
const std::string c_strEffectDataPath		= "D:\\Ymir Work\\effect";
const std::string c_strSoundDataPath		= "D:\\Ymir Work\\sound";
const std::string c_strUIDataPath			= "D:\\Ymir Work\\ui";
