#include "StdAfx.h"
#include "Packet.h"
#include "GameType.h"

std::string g_strResourcePath = "d:/ymir work/";
std::string g_strImagePath = "d:/ymir work/ui/";

std::string g_strGuildSymbolPathName = "mark/10/";

// DEFAULT_FONT
static std::string gs_strDefaultFontName = "����ü:12.fnt";
static std::string gs_strDefaultItalicFontName = "����ü:12i.fnt";

#ifdef ENABLE_UI_DEBUG_WINDOW
CResource* gs_pkDefaultFont = nullptr;
#else
static CResource* gs_pkDefaultFont = nullptr;
#endif
static CResource* gs_pkDefaultItalicFont = nullptr;

static bool gs_isReloadDefaultFont = false;

void DefaultFont_Startup()
{
	gs_pkDefaultFont = nullptr;
}

void DefaultFont_Cleanup()
{
	if (gs_pkDefaultFont)
	{
		gs_pkDefaultFont->Release();
	}
}

void DefaultFont_SetName(const char* c_szFontName)
{
	gs_strDefaultFontName = c_szFontName;
	gs_strDefaultFontName += ".fnt";

	gs_strDefaultItalicFontName = c_szFontName;
	if (strchr (c_szFontName, ':'))
	{
		gs_strDefaultItalicFontName += "i";
	}
	gs_strDefaultItalicFontName += ".fnt";

	gs_isReloadDefaultFont = true;
}

bool ReloadDefaultFonts()
{
	CResourceManager& rkResMgr = CResourceManager::Instance();

	gs_isReloadDefaultFont = false;

	CResource* pkNewFont = rkResMgr.GetResourcePointer(gs_strDefaultFontName.c_str());
	pkNewFont->AddReference();
	if (gs_pkDefaultFont)
	{
		gs_pkDefaultFont->Release();
	}
	gs_pkDefaultFont = pkNewFont;

	CResource* pkNewItalicFont = rkResMgr.GetResourcePointer(gs_strDefaultItalicFontName.c_str());
	pkNewItalicFont->AddReference();
	if (gs_pkDefaultItalicFont)
	{
		gs_pkDefaultItalicFont->Release();
	}
	gs_pkDefaultItalicFont = pkNewItalicFont;

	return true;
}

CResource* DefaultFont_GetResource()
{
	if (!gs_pkDefaultFont || gs_isReloadDefaultFont)
	{
		ReloadDefaultFonts();
	}
	return gs_pkDefaultFont;
}

CResource* DefaultItalicFont_GetResource()
{
	if (!gs_pkDefaultItalicFont || gs_isReloadDefaultFont)
	{
		ReloadDefaultFonts();
	}
	return gs_pkDefaultItalicFont;
}

// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char* c_szPathName)
{
	g_strGuildSymbolPathName = "mark/";
	g_strGuildSymbolPathName += c_szPathName;
	g_strGuildSymbolPathName += "/";
}

const char* GetGuildSymbolFileName(DWORD dwGuildID)
{
	return _getf("%s%03d.jpg", g_strGuildSymbolPathName.c_str(), dwGuildID);
}

BYTE c_aSlotTypeToInvenType[SLOT_TYPE_MAX] =
{
	RESERVED_WINDOW,		// SLOT_TYPE_NONE
	INVENTORY,				// SLOT_TYPE_INVENTORY
	RESERVED_WINDOW,		// SLOT_TYPE_SKILL
	RESERVED_WINDOW,		// SLOT_TYPE_EMOTION
	RESERVED_WINDOW,		// SLOT_TYPE_SHOP
	RESERVED_WINDOW,		// SLOT_TYPE_EXCHANGE_OWNER
	RESERVED_WINDOW,		// SLOT_TYPE_EXCHANGE_TARGET
	RESERVED_WINDOW,		// SLOT_TYPE_QUICK_SLOT
	RESERVED_WINDOW,		// SLOT_TYPE_SAFEBOX
	RESERVED_WINDOW,		// SLOT_TYPE_PRIVATE_SHOP
	RESERVED_WINDOW,		// SLOT_TYPE_MALL
	DRAGON_SOUL_INVENTORY,	// SLOT_TYPE_DRAGON_SOUL_INVENTORY
	BELT_INVENTORY,			// SLOT_TYPE_BELT_INVENTORY
#ifdef ENABLE_RENEWAL_SWITCHBOT
	SWITCHBOT,				// SLOT_TYPE_SWITCHBOT
#endif
#ifdef ENABLE_SPECIAL_INVENTORY
	INVENTORY,				// SLOT_TYPE_SKILL_BOOK_INVENTORY
	INVENTORY,				// SLOT_TYPE_MATERIAL_INVENTORY
	INVENTORY,				// SLOT_TYPE_STONE_INVENTORY
	INVENTORY,				// SLOT_TYPE_GIFT_BOX_INVENTORY
	INVENTORY,				// SLOT_TYPE_CHANGERS_INVENTORY
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	AURA_REFINE,			// SLOT_TYPE_AURA
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	RESERVED_WINDOW,		// SLOT_TYPE_CHANGE_LOOK
#endif
};

BYTE SlotTypeToInvenType(BYTE bSlotType)
{
#ifdef ENABLE_SHOP_REPURCHASE
	if (bSlotType == SLOT_TYPE_SHOP_REPURCHASE)
		return SHOP_REPURCHASE;
#endif
	if (bSlotType >= SLOT_TYPE_MAX)
	{
		return RESERVED_WINDOW;
	}
	else
	{
		return c_aSlotTypeToInvenType[bSlotType];
	}
}

#ifdef ENABLE_AURA_COSTUME_SYSTEM
static int s_aiAuraRefineInfo[CItemData::AURA_GRADE_MAX_NUM][AURA_REFINE_INFO_MAX] = {
	{1,   1,  49,  1000, 30617, 10,  5000000, 100},
	{2,  50,  99,  2000, 31136, 10,  5000000, 100},
	{3, 100, 149,  4000, 31137, 10,  5000000, 100},
	{4, 150, 199,  8000, 31138, 10,  8000000, 100},
	{5, 200, 249, 16000, 31138, 20, 10000000, 100},
	{6, 250, 250,     0,     0,  0,        0,   0},
	{0,   0,   0,     0,     0,  0,        0,   0}
};

int* GetAuraRefineInfo(BYTE bLevel)
{
	if (bLevel > 250)
		return NULL;

	for (int i = 0; i < CItemData::AURA_GRADE_MAX_NUM + 1; ++i)
	{
		if (bLevel >= s_aiAuraRefineInfo[i][AURA_REFINE_INFO_LEVEL_MIN] && bLevel <= s_aiAuraRefineInfo[i][AURA_REFINE_INFO_LEVEL_MAX])
			return s_aiAuraRefineInfo[i];
	}

	return NULL;
}
#endif

#ifndef ENABLE_CONQUEROR_LEVEL
typedef struct SApplyInfo
{
	BYTE	bPointType;
} TApplyInfo;

const TApplyInfo aApplyInfo[CItemData::MAX_APPLY_NUM] =
{
	{ POINT_NONE                          }, // APPLY_NONE (0)
	{ POINT_MAX_HP                        }, // APPLY_MAX_HP (1)
	{ POINT_MAX_SP                        }, // APPLY_MAX_SP (2)
	{ POINT_HT                            }, // APPLY_CON (3)
	{ POINT_IQ                            }, // APPLY_INT (4)
	{ POINT_ST                            }, // APPLY_STR (5)
	{ POINT_DX                            }, // APPLY_DEX (6)
	{ POINT_ATT_SPEED                     }, // APPLY_ATT_SPEED (7)
	{ POINT_MOV_SPEED                     }, // APPLY_MOV_SPEED (8)
	{ POINT_CASTING_SPEED                 }, // APPLY_CAST_SPEED (9)
	{ POINT_HP_REGEN                      }, // APPLY_HP_REGEN (10)
	{ POINT_SP_REGEN                      }, // APPLY_SP_REGEN (11)
	{ POINT_POISON_PCT                    }, // APPLY_POISON_PCT (12)
	{ POINT_STUN_PCT                      }, // APPLY_STUN_PCT (13)
	{ POINT_SLOW_PCT                      }, // APPLY_SLOW_PCT (14)
	{ POINT_CRITICAL_PCT                  }, // APPLY_CRITICAL_PCT (15)
	{ POINT_PENETRATE_PCT                 }, // APPLY_PENETRATE_PCT (16)
	{ POINT_ATTBONUS_HUMAN                }, // APPLY_ATTBONUS_HUMAN (17)
	{ POINT_ATTBONUS_ANIMAL               }, // APPLY_ATTBONUS_ANIMAL (18)
	{ POINT_ATTBONUS_ORC                  }, // APPLY_ATTBONUS_ORC (19)
	{ POINT_ATTBONUS_MILGYO               }, // APPLY_ATTBONUS_MILGYO (20)
	{ POINT_ATTBONUS_UNDEAD               }, // APPLY_ATTBONUS_UNDEAD (21)
	{ POINT_ATTBONUS_DEVIL                }, // APPLY_ATTBONUS_DEVIL (22)
	{ POINT_STEAL_HP                      }, // APPLY_STEAL_HP (23)
	{ POINT_STEAL_SP                      }, // APPLY_STEAL_SP (24)
	{ POINT_MANA_BURN_PCT                 }, // APPLY_MANA_BURN_PCT (25)
	{ POINT_DAMAGE_SP_RECOVER             }, // APPLY_DAMAGE_SP_RECOVER (26)
	{ POINT_BLOCK                         }, // APPLY_BLOCK (27)
	{ POINT_DODGE                         }, // APPLY_DODGE (28)
	{ POINT_RESIST_SWORD                  }, // APPLY_RESIST_SWORD (29)
	{ POINT_RESIST_TWOHAND                }, // APPLY_RESIST_TWOHAND (30)
	{ POINT_RESIST_DAGGER                 }, // APPLY_RESIST_DAGGER (31)
	{ POINT_RESIST_BELL                   }, // APPLY_RESIST_BELL (32)
	{ POINT_RESIST_FAN                    }, // APPLY_RESIST_FAN (33)
	{ POINT_RESIST_BOW                    }, // APPLY_RESIST_BOW (34)
	{ POINT_RESIST_FIRE                   }, // APPLY_RESIST_FIRE (35)
	{ POINT_RESIST_ELEC                   }, // APPLY_RESIST_ELEC (36)
	{ POINT_RESIST_MAGIC                  }, // APPLY_RESIST_MAGIC (37)
	{ POINT_RESIST_WIND                   }, // APPLY_RESIST_WIND (38)
	{ POINT_REFLECT_MELEE                 }, // APPLY_REFLECT_MELEE (39)
	{ POINT_REFLECT_CURSE                 }, // APPLY_REFLECT_CURSE (40)
	{ POINT_POISON_REDUCE                 }, // APPLY_POISON_REDUCE (41)
	{ POINT_KILL_SP_RECOVER               }, // APPLY_KILL_SP_RECOVER (42)
	{ POINT_EXP_DOUBLE_BONUS              }, // APPLY_EXP_DOUBLE_BONUS (43)
	{ POINT_GOLD_DOUBLE_BONUS             }, // APPLY_GOLD_DOUBLE_BONUS (44)
	{ POINT_ITEM_DROP_BONUS               }, // APPLY_ITEM_DROP_BONUS (45)
	{ POINT_POTION_BONUS                  }, // APPLY_POTION_BONUS (46)
	{ POINT_KILL_HP_RECOVER               }, // APPLY_KILL_HP_RECOVER (47)
	{ POINT_IMMUNE_STUN                   }, // APPLY_IMMUNE_STUN (48)
	{ POINT_IMMUNE_SLOW                   }, // APPLY_IMMUNE_SLOW (49)
	{ POINT_IMMUNE_FALL                   }, // APPLY_IMMUNE_FALL (50)
	{ POINT_NONE                          }, // APPLY_SKILL (51)
	{ POINT_BOW_DISTANCE                  }, // APPLY_BOW_DISTANCE (52)
	{ POINT_ATT_GRADE_BONUS               }, // APPLY_ATT_GRADE_BONUS (53)
	{ POINT_DEF_GRADE_BONUS               }, // APPLY_DEF_GRADE_BONUS (54)
	{ POINT_MAGIC_ATT_GRADE_BONUS         }, // APPLY_MAGIC_ATT_GRADE (55)
	{ POINT_MAGIC_DEF_GRADE_BONUS         }, // APPLY_MAGIC_DEF_GRADE (56)
	{ POINT_CURSE_PCT                     }, // APPLY_CURSE_PCT (57)
	{ POINT_MAX_STAMINA                   }, // APPLY_MAX_STAMINA (58)
	{ POINT_ATTBONUS_WARRIOR              }, // APPLY_ATTBONUS_WARRIOR (59)
	{ POINT_ATTBONUS_ASSASSIN             }, // APPLY_ATTBONUS_ASSASSIN (60)
	{ POINT_ATTBONUS_SURA                 }, // APPLY_ATTBONUS_SURA (61)
	{ POINT_ATTBONUS_SHAMAN               }, // APPLY_ATTBONUS_SHAMAN (62)
	{ POINT_ATTBONUS_MONSTER              }, // APPLY_ATTBONUS_MONSTER (63)
	{ POINT_ATT_BONUS                     }, // APPLY_MALL_ATTBONUS (64)
	{ POINT_MALL_DEFBONUS                 }, // APPLY_MALL_DEFBONUS (65)
	{ POINT_MALL_EXPBONUS                 }, // APPLY_MALL_EXPBONUS (66)
	{ POINT_MALL_ITEMBONUS                }, // APPLY_MALL_ITEMBONUS (67)
	{ POINT_MALL_GOLDBONUS                }, // APPLY_MALL_GOLDBONUS (68)
	{ POINT_MAX_HP_PCT                    }, // APPLY_MAX_HP_PCT (69)
	{ POINT_MAX_SP_PCT                    }, // APPLY_MAX_SP_PCT (70)
	{ POINT_SKILL_DAMAGE_BONUS            }, // APPLY_SKILL_DAMAGE_BONUS (71)
	{ POINT_NORMAL_HIT_DAMAGE_BONUS       }, // APPLY_NORMAL_HIT_DAMAGE_BONUS (72)
	{ POINT_SKILL_DEFEND_BONUS            }, // APPLY_SKILL_DEFEND_BONUS (73)
	{ POINT_NORMAL_HIT_DEFEND_BONUS       }, // APPLY_NORMAL_HIT_DEFEND_BONUS (74)
	{ POINT_NONE                          }, // APPLY_EXTRACT_HP_PCT (75)
	{ POINT_PC_BANG_EXP_BONUS             }, // APPLY_PC_BANG_EXP_BONUS (76)
	{ POINT_PC_BANG_DROP_BONUS            }, // APPLY_PC_BANG_DROP_BONUS (77)
	{ POINT_RESIST_WARRIOR                }, // APPLY_RESIST_WARRIOR (78)
	{ POINT_RESIST_ASSASSIN               }, // APPLY_RESIST_ASSASSIN (79)
	{ POINT_RESIST_SURA                   }, // APPLY_RESIST_SURA (80)
	{ POINT_RESIST_SHAMAN                 }, // APPLY_RESIST_SHAMAN (81)
	{ POINT_ENERGY                        }, // APPLY_ENERGY (82)
	{ POINT_DEF_GRADE                     }, // APPLY_DEF_GRADE (83)
	{ POINT_COSTUME_ATTR_BONUS            }, // APPLY_COSTUME_ATTR_BONUS (84)
	{ POINT_MAGIC_ATT_BONUS_PER           }, // APPLY_MAGIC_ATTBONUS_PER (85)
	{ POINT_MELEE_MAGIC_ATT_BONUS_PER     }, // APPLY_MELEE_MAGIC_ATTBONUS_PER (86)
	{ POINT_RESIST_ICE                    }, // APPLY_RESIST_ICE (87)
	{ POINT_RESIST_EARTH                  }, // APPLY_RESIST_EARTH (88)
	{ POINT_RESIST_DARK                   }, // APPLY_RESIST_DARK (89)
	{ POINT_RESIST_CRITICAL               }, // APPLY_ANTI_CRITICAL_PCT (90)
	{ POINT_RESIST_PENETRATE              }, // APPLY_ANTI_PENETRATE_PCT (91)
	{ POINT_NONE                          }, // APPLY_BLEEDING_REDUCE (92)
	{ POINT_NONE                          }, // APPLY_BLEEDING_PCT (93)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_WOLFMAN (94)
	{ POINT_NONE                          }, // APPLY_RESIST_WOLFMAN (95)
	{ POINT_NONE                          }, // APPLY_RESIST_CLAW (96)
	{ POINT_ACCEDRAIN_RATE                }, // APPLY_ACCEDRAIN_RATE (97)
	{ POINT_RESIST_MAGIC_REDUCTION        }, // APPLY_RESIST_MAGIC_REDUCTION (98)
	{ POINT_ENCHANT_ELECT                 }, // APPLY_ENCHANT_ELECT (99)
	{ POINT_ENCHANT_FIRE                  }, // APPLY_ENCHANT_FIRE (100)
	{ POINT_ENCHANT_ICE                   }, // APPLY_ENCHANT_ICE (101)
	{ POINT_ENCHANT_WIND                  }, // APPLY_ENCHANT_WIND (102)
	{ POINT_ENCHANT_EARTH                 }, // APPLY_ENCHANT_EARTH (103)
	{ POINT_ENCHANT_DARK                  }, // APPLY_ENCHANT_DARK (104)
	{ POINT_ATTBONUS_CZ                   }, // APPLY_ATTBONUS_CZ (105)
	{ POINT_ATTBONUS_INSECT               }, // APPLY_ATTBONUS_INSECT (106)
	{ POINT_ATTBONUS_DESERT               }, // APPLY_ATTBONUS_DESERT (107)
	{ POINT_ATTBONUS_SWORD                }, // APPLY_ATTBONUS_SWORD (108)
	{ POINT_ATTBONUS_TWOHAND              }, // APPLY_ATTBONUS_TWOHAND (109)
	{ POINT_ATTBONUS_DAGGER               }, // APPLY_ATTBONUS_DAGGER (110)
	{ POINT_ATTBONUS_BELL                 }, // APPLY_ATTBONUS_BELL (111)
	{ POINT_ATTBONUS_FAN                  }, // APPLY_ATTBONUS_FAN (112)
	{ POINT_ATTBONUS_BOW                  }, // APPLY_ATTBONUS_BOW (113)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_CLAW (114)
	{ POINT_RESIST_HUMAN                  }, // APPLY_RESIST_HUMAN (115)
	{ POINT_RESIST_MOUNT_FALL             }, // APPLY_RESIST_MOUNT_FALL (116)
	{ POINT_NONE                          }, // UNK17 (117)
	{ POINT_NONE                          }, // APPLY_MOUNT (118)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_SAMYEON (119)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_TANHWAN (120)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_PALBANG (121)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_GIGONGCHAM (122)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_GYOKSAN (123)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_GEOMPUNG (124)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_AMSEOP (125)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_GUNGSIN (126)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_CHARYUN (127)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_SANGONG (128)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_YEONSA (129)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_KWANKYEOK (130)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_GIGUNG (131)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_HWAJO (132)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_SWAERYUNG (133)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_YONGKWON (134)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_PABEOB (135)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_MARYUNG (136)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_HWAYEOMPOK (137)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_MAHWAN (138)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_BIPABU (139)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_YONGBI (140)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_PAERYONG (141)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_NOEJEON (142)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_BYEURAK (143)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_CHAIN (144)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_CHAYEOL (145)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_SALPOONG (146)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_GONGDAB (147)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_PASWAE (148)
	{ POINT_NONE                          }, // APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE (149)
	{ POINT_NONE                          }, // APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE (150)
	{ POINT_NONE                          }, // APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE (151)
	{ POINT_NONE                          }, // APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE (152)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_ENCHANT_FIRE (153)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_ENCHANT_ICE (154)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_ENCHANT_ELEC (155)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_ENCHANT_WIND (156)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_ENCHANT_DARK (157)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_ENCHANT_EARTH (158)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_RESIST_FIRE (159)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_RESIST_ICE (160)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_RESIST_ELEC (161)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_RESIST_WIND (162)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_RESIST_DARK (163)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_RESIST_EARTH (164)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED (165)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED (166)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT (167)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS (168)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS (169)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_JEOKRANG_DEF_BONUS (170)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GWIGEOM_DEF_BONUS (171)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS (172)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS (173)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_MANASHILED_CASTING_SPEED (174)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_HOSIN_DEF_BONUS (175)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS (176)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS (177)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS (178)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS (179)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED (180)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS (181)
	{ POINT_NONE                          }, // APPLY_SKILL_DURATION_INCREASE_EUNHYUNG (182)
	{ POINT_NONE                          }, // APPLY_SKILL_DURATION_INCREASE_GYEONGGONG (183)
	{ POINT_NONE                          }, // APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG (184)
	{ POINT_NONE                          }, // APPLY_SKILL_DURATION_INCREASE_JEOKRANG (185)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_PALBANG_HP_ABSORB (186)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_AMSEOP_HP_ABSORB (187)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_YEONSA_HP_ABSORB (188)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_YONGBI_HP_ABSORB (189)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_CHAIN_HP_ABSORB (190)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_PASWAE_SP_ABSORB (191)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GIGONGCHAM_STUN (192)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_CHARYUN_STUN (193)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_PABEOB_STUN (194)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_MAHWAN_STUN (195)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GONGDAB_STUN (196)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_SAMYEON_STUN (197)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GYOKSAN_KNOCKBACK (198)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_SEOMJEON_KNOCKBACK (199)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK (200)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK (201)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GONGDAB_KNOCKBACK (202)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK (203)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER (204)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER (205)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER (206)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER (207)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER (208)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER (209)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER (210)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER (211)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER (212)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER (213)
	{ POINT_ATTBONUS_STONE                }, // APPLY_ATTBONUS_STONE (214)
	{ POINT_NONE                          }, // APPLY_DAMAGE_HP_RECOVERY (215)
	{ POINT_NONE                          }, // APPLY_DAMAGE_SP_RECOVERY (216)
	{ POINT_NONE                          }, // APPLY_ALIGNMENT_DAMAGE_BONUS (217)
	{ POINT_NONE                          }, // APPLY_NORMAL_DAMAGE_GUARD (218)
	{ POINT_NONE                          }, // APPLY_MORE_THEN_HP90_DAMAGE_REDUCE (219)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_TUSOK_HP_ABSORB (220)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_PAERYONG_HP_ABSORB (221)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_BYEURAK_HP_ABSORB (222)
	{ POINT_NONE                          }, // APPLY_FIRST_ATTRIBUTE_BONUS (223)
	{ POINT_NONE                          }, // APPLY_SECOND_ATTRIBUTE_BONUS (224)
	{ POINT_NONE                          }, // APPLY_THIRD_ATTRIBUTE_BONUS (225)
	{ POINT_NONE                          }, // APPLY_FOURTH_ATTRIBUTE_BONUS (226)
	{ POINT_NONE                          }, // APPLY_FIFTH_ATTRIBUTE_BONUS (227)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER (228)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER (229)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER (230)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER (231)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER (232)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER (233)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER (234)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER (235)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER (236)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER (237)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_CHAYEOL_HP_ABSORB (238)
	{ POINT_SUNGMA_STR                    }, // APPLY_SUNGMA_STR (239)
	{ POINT_SUNGMA_HP                     }, // APPLY_SUNGMA_HP (240)
	{ POINT_SUNGMA_MOVE                   }, // APPLY_SUNGMA_MOVE (241)
	{ POINT_SUNGMA_IMMUNE                 }, // APPLY_SUNGMA_IMMUNE (242)
	{ POINT_NONE                          }, // APPLY_HIT_PCT (243)
	{ POINT_NONE                          }, // APPLY_RANDOM (244)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_HUMAN (245)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_ANIMAL (246)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_ORC (247)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_MILGYO (248)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_UNDEAD (249)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_DEVIL (250)
	{ POINT_NONE                          }, // APPLY_ENCHANT_PER_ELECT (251)
	{ POINT_NONE                          }, // APPLY_ENCHANT_PER_FIRE (252)
	{ POINT_NONE                          }, // APPLY_ENCHANT_PER_ICE (253)
	{ POINT_NONE                          }, // APPLY_ENCHANT_PER_WIND (254)
	{ POINT_NONE                          }, // APPLY_ENCHANT_PER_EARTH (255)
	{ POINT_NONE                          }, // APPLY_ENCHANT_PER_DARK (256)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_CZ (257)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_INSECT (258)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_DESERT (259)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_STONE (260)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_PER_MONSTER (261)
	{ POINT_NONE                          }, // APPLY_RESIST_PER_HUMAN (262)
	{ POINT_NONE                          }, // APPLY_RESIST_PER_ICE (263)
	{ POINT_NONE                          }, // APPLY_RESIST_PER_DARK (264)
	{ POINT_NONE                          }, // APPLY_RESIST_PER_EARTH (265)
	{ POINT_NONE                          }, // APPLY_RESIST_PER_FIRE (266)
	{ POINT_NONE                          }, // APPLY_RESIST_PER_ELEC (267)
	{ POINT_NONE                          }, // APPLY_RESIST_PER_MAGIC (268)
	{ POINT_NONE                          }, // APPLY_RESIST_PER_WIND (269)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_SUNGMA_STR (270)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_SUNGMA_MOVE (271)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_SUNGMA_HP (272)
	{ POINT_NONE                          }, // APPLY_HIT_BUFF_SUNGMA_IMMUNE (273)
	{ POINT_NONE                          }, // APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER (274)
	{ POINT_NONE                          }, // APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER (275)
	{ POINT_NONE                          }, // APPLY_HIT_AUTO_HP_RECOVERY (276)
	{ POINT_NONE                          }, // APPLY_HIT_AUTO_SP_RECOVERY (277)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_COOLTIME_DECREASE_ALL (278)
	{ POINT_NONE                          }, // APPLY_HIT_STONE_ATTBONUS_STONE (279)
	{ POINT_NONE                          }, // APPLY_HIT_STONE_DEF_GRADE_BONUS (280)
	{ POINT_NONE                          }, // APPLY_KILL_BOSS_ITEM_BONUS (281)
	{ POINT_NONE                          }, // APPLY_MOB_HIT_MOB_AGGRESSIVE (282)
	{ POINT_NONE                          }, // APPLY_NO_DEATH_AND_HP_RECOVERY30 (283)
	{ POINT_NONE                          }, // APPLY_AUTO_PICKUP (284)
	{ POINT_NONE                          }, // APPLY_MOUNT_NO_KNOCKBACK (285)
	{ POINT_NONE                          }, // APPLY_SUNGMA_PER_STR (286)
	{ POINT_NONE                          }, // APPLY_SUNGMA_PER_HP (287)
	{ POINT_NONE                          }, // APPLY_SUNGMA_PER_MOVE (288)
	{ POINT_NONE                          }, // APPLY_SUNGMA_PER_IMMUNE (289)
	{ POINT_NONE                          }, // APPLY_IMMUNE_POISON100 (290)
	{ POINT_NONE                          }, // APPLY_IMMUNE_BLEEDING100 (291)
	{ POINT_NONE                          }, // APPLY_MONSTER_DEFEND_BONUS (292)
	{ POINT_NONE                          }, // APPLY_RESERVED_293 (293)
	{ POINT_NONE                          }, // APPLY_RESERVED_294 (294)
	{ POINT_NONE                          }, // APPLY_RESERVED_295 (295)
	{ POINT_NONE                          }, // APPLY_RESERVED_296 (296)
	{ POINT_NONE                          }, // APPLY_RESERVED_297 (297)
	{ POINT_NONE                          }, // APPLY_RESERVED_298 (298)
	{ POINT_NONE                          }, // APPLY_RESERVED_299 (299)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_FINISH_NEXT_COOLTIME_DECREASE_PCT (300)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_ILGWANGPYO_NEXT_COOLTIME_DECREASE_PCT (301)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_PUNGLOEPO_NEXT_COOLTIME_DECREASE_PCT (302)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_GEOMAGGWI_NEXT_COOLTIME_DECREASE_PCT (303)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_MABEOBAGGWI_NEXT_COOLTIME_DECREASE_PCT (304)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_METEO_NEXT_COOLTIME_DECREASE_PCT (305)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_CHEONUN_NEXT_COOLTIME_DECREASE_PCT (306)
	{ POINT_NONE                          }, // APPLY_USE_SKILL_ILIPUNGU_NEXT_COOLTIME_DECREASE_PCT (307)
	{ POINT_NONE                          }, // APPLY_GLOVE_BASE_EFFECT_INCREASE (308)
	{ POINT_NONE                          }, // APPLY_NO_FALLEN (309)
	{ POINT_NONE                          }, // APPLY_HIT_AREA_ELEC_ATTACK (310)
	{ POINT_NONE                          }, // APPLY_BLOCK_PENETRATE_PCT (311)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_MYSTERY_DUNGEON (312)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_DRAGON (313)
	{ POINT_NONE                          }, // APPLY_RESERVED_314 (314)
	{ POINT_NONE                          }, // APPLY_RESERVED_315 (315)
	{ POINT_NONE                          }, // APPLY_RESERVED_316 (316)
	{ POINT_NONE                          }, // APPLY_RESERVED_317 (317)
	{ POINT_NONE                          }, // APPLY_RESERVED_318 (318)
	{ POINT_NONE                          }, // APPLY_RESERVED_319 (319)
	{ POINT_NONE                          }, // APPLY_RESERVED_320 (320)
	{ POINT_NONE                          }, // APPLY_RESERVED_321 (321)
	{ POINT_NONE                          }, // APPLY_ATTBONUS_MOON (322)
	{ POINT_NONE                          }, // APPLY_RESERVED_323 (323)
	{ POINT_NONE                          }, // APPLY_BONUS_BLOCK_PENETRATE_PCT (324)
	{ POINT_NONE                          }, // APPLY_BONUS_HIT_PCT (325)
	{ POINT_NONE                          }, // APPLY_REFUSE_EXP (326)
	{ POINT_NONE                          }, // APPLY_RESERVED_327 (327)
	{ POINT_NONE                          }, // APPLY_RESERVED_328 (328)
	{ POINT_NONE                          }, // APPLY_RESERVED_329 (329)
	{ POINT_ATTBONUS_BOSS                 }, // APPLY_ATTBONUS_BOSS (330)
	{ POINT_ATTBONUS_ELEMENTS             }, // APPLY_ATTBONUS_ELEMENTS (331)
	{ POINT_ENCHANT_ELEMENTS              }, // APPLY_ENCHANT_ELEMENTS (332)
	{ POINT_ATTBONUS_CHARACTERS           }, // APPLY_ATTBONUS_CHARACTERS (333)
	{ POINT_ENCHANT_CHARACTERS            }, // APPLY_ENCHANT_CHARACTERS (334)
	{ POINT_RESIST_MONSTER                }, // APPLY_RESIST_MONSTER (335)
	{ POINT_ATTBONUS_MEDI_PVM             }, // APPLY_ATTBONUS_MEDI_PVM (336)
	{ POINT_ATTBONUS_PVM_STR              }, // APPLY_ATTBONUS_PVM_STR (337)
	{ POINT_ATTBONUS_PVM_BERSERKER        }, // APPLY_ATTBONUS_PVM_BERSERKER (338)
	{ POINT_NONE                          }, // UNUSED (339)
	{ POINT_NONE                          }, // UNUSED (340)
	{ POINT_NONE                          }, // UNUSED (341)
	{ POINT_NONE                          }, // UNUSED (342)
	{ POINT_NONE                          }, // UNUSED (343)
	{ POINT_NONE                          }, // UNUSED (344)
	{ POINT_NONE                          }, // UNUSED (345)
	{ POINT_NONE                          }, // UNUSED (346)
	{ POINT_NONE                          }, // UNUSED (347)
	{ POINT_NONE                          }, // UNUSED (348)
	{ POINT_NONE                          }, // UNUSED (349)
};

BYTE ApplyTypeToPointType(WORD bApplyType)
{
	if (bApplyType >= CItemData::MAX_APPLY_NUM)
		return POINT_NONE;
	else
		return aApplyInfo[bApplyType].bPointType;
}
#endif