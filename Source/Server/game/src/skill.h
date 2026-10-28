#ifndef __INC_METIN_II_GAME_CSkillManager_H__
#define __INC_METIN_II_GAME_CSkillManager_H__

#include "../../library/libpoly/Poly.h"

enum ESkillFlags
{
	SKILL_FLAG_ATTACK					= (1 << 0),
	SKILL_FLAG_USE_MELEE_DAMAGE			= (1 << 1),
	SKILL_FLAG_COMPUTE_ATTGRADE			= (1 << 2),
	SKILL_FLAG_SELFONLY					= (1 << 3),
	SKILL_FLAG_USE_MAGIC_DAMAGE			= (1 << 4),
	SKILL_FLAG_USE_HP_AS_COST			= (1 << 5),
	SKILL_FLAG_COMPUTE_MAGIC_DAMAGE		= (1 << 6),
	SKILL_FLAG_SPLASH					= (1 << 7),
	SKILL_FLAG_GIVE_PENALTY				= (1 << 8),
	SKILL_FLAG_USE_ARROW_DAMAGE			= (1 << 9),
	SKILL_FLAG_PENETRATE				= (1 << 10),
	SKILL_FLAG_IGNORE_TARGET_RATING		= (1 << 11),
	SKILL_FLAG_SLOW						= (1 << 12),
	SKILL_FLAG_STUN						= (1 << 13),
	SKILL_FLAG_HP_ABSORB				= (1 << 14),
	SKILL_FLAG_SP_ABSORB				= (1 << 15),
	SKILL_FLAG_FIRE_CONT				= (1 << 16),
	SKILL_FLAG_REMOVE_BAD_AFFECT		= (1 << 17),
	SKILL_FLAG_REMOVE_GOOD_AFFECT		= (1 << 18),
	SKILL_FLAG_CRUSH					= (1 << 19),
	SKILL_FLAG_POISON					= (1 << 20),
	SKILL_FLAG_TOGGLE					= (1 << 21),
	SKILL_FLAG_DISABLE_BY_POINT_UP		= (1 << 22),
	SKILL_FLAG_CRUSH_LONG				= (1 << 23),
	SKILL_FLAG_WIND						= (1 << 24),
	SKILL_FLAG_ELEC						= (1 << 25),
	SKILL_FLAG_FIRE						= (1 << 26),
#ifdef ENABLE_OCHAO_TEMPLE_SYSTEM
	SKILL_FLAG_PARTY					= (1 << 27),
#endif
#ifdef ENABLE_PVP_BALANCE
	SKILL_FLAG_KNOCKBACK 				= (1 << 28),
#endif
};

enum
{
	SKILL_PENALTY_DURATION = 3,
	SKILL_TYPE_HORSE = 5,
};

enum ESkillIndexes
{
	SKILL_RESERVED = 0,

	// Bedensel Savaþçý Skill Grubu
	SKILL_SAMYEON		= 1,	// Dreiwege-Schnitt
	SKILL_PALBANG		= 2,	// Schwertwirbel
	SKILL_JEONGWI		= 3,	// Kampfrausch
	SKILL_GEOMKYUNG		= 4,	// Aura des Schwertes
	SKILL_TANHWAN		= 5,	// Sausen
	// Lebenswille

	// Zihinsel Savaþçý Skill Grubu
	SKILL_GIGONGCHAM	= 16,	// Durchschlag
	SKILL_GYOKSAN		= 17,	// Heftiges Schlagen
	SKILL_DAEJINGAK		= 18,	// Stampfer
	SKILL_CHUNKEON		= 19,	// Starker Körper
	SKILL_GEOMPUNG		= 20,	// Schwertschlag
	// yaþam isteði

	// Yakýn Dövüþ Ninja Skill Grubu
	SKILL_AMSEOP		= 31,	// Hinterhalt
	SKILL_GUNGSIN		= 32,	// Blitzangriff
	SKILL_CHARYUN		= 33,	// Degenwirbel
	SKILL_EUNHYUNG		= 34,	// Tarnung
	SKILL_SANGONG		= 35,	// Giftwolke
	// Sinsice zehir

	// Uzak Dövüþ Ninja Skill Grubu
	SKILL_YEONSA		= 46,	// Wiederholter Schuss
	SKILL_KWANKYEOK		= 47,	// Pfeilregen
	SKILL_HWAJO			= 48,	// Feuerpfeil
	SKILL_GYEONGGONG	= 49,	// Federschreiten
	SKILL_GIGUNG		= 50,	// Giftpfeil
	// Funkenschlag

	// Büyülü Silah Sura Skill Grubu
	SKILL_SWAERYUNG		= 61,	// Fingerschlag
	SKILL_YONGKWON		= 62,	// Drachenwirbel
	SKILL_GWIGEOM		= 63,	// Verzauberte Klinge
	SKILL_TERROR		= 64,	// Furcht
	SKILL_JUMAGAP		= 65,	// Verzauberte Rüstung
	SKILL_PABEOB		= 66,	// Zauber aufheben

	// Kara Büyü Sura Skill Grubu
	SKILL_MARYUNG		= 76,	// Dunkler Schlag
	SKILL_HWAYEOMPOK	= 77,	// Flammenschlag
	SKILL_MUYEONG		= 78,	// Geist der Flamme
	SKILL_MANASHILED	= 79,	// Dunkler Schutz
	SKILL_TUSOK			= 80,	// Geisterschlag
	SKILL_MAHWAN		= 81,	// Dunkler Stein

	// Ejderha Gücü Þaman Skill Grubu
	SKILL_BIPABU		= 91,	// Fliegender Talisman
	SKILL_YONGBI		= 92,	// Drachenschießen
	SKILL_PAERYONG		= 93,	// Drachengebrüll
	//SKILL_BUDONG,
	SKILL_HOSIN			= 94,	// Segen
	SKILL_REFLECT		= 95,	// Reflektieren
	SKILL_GICHEON		= 96,	// Hilfe des Drachen

	// Ýyileþtirmeci Þaman Skill Grubu
	SKILL_NOEJEON		= 106,	// Blitzwurf
	SKILL_BYEURAK		= 107,	// Blitz heraufbeschwören
	SKILL_CHAIN			= 108,	// Blitzkralle
	SKILL_JEONGEOP		= 109,	// Kurieren
	SKILL_KWAESOK		= 110,	// Schnelligkeit
	SKILL_JEUNGRYEOK	= 111,	// Angriff+

#ifdef ENABLE_WOLFMAN_CHARACTER
	SKILL_CHAYEOL		= 170,	// Zerreißen
	SKILL_SALPOONG		= 171,	// Atem des Wolfes
	SKILL_GONGDAB		= 172,	// Wolfssprung
	SKILL_PASWAE		= 173,	// Wolfsklaue
	SKILL_JEOKRANG		= 174,	// Purpurwolfseele
	SKILL_CHEONGRANG	= 175,	// Indigowolfseele
#endif

#ifdef ENABLE_NINETH_SKILL
	SKILL_FINISH		= 176,	// Erdbeben
	SKILL_ILGWANGPYO	= 177,	// Lichtsterne
	SKILL_PUNGLOEPO		= 178,	// Sturmschuss
	SKILL_GEOMAGGWI		= 179,	// Höllenstoß
	SKILL_MABEOBAGGWI	= 180,	// Todeswelle
	SKILL_METEO			= 181,	// Meteor
	SKILL_CHEONUN		= 182,	// Ätherschild
#ifdef ENABLE_WOLFMAN_CHARACTER
	SKILL_ILIPUNGU		= 183,	// Klauensturm
#endif
#endif

	// common skill
	// 7
	SKILL_7_A_ANTI_TANHWAN = 112,	// 
	SKILL_7_B_ANTI_AMSEOP,			// 
	SKILL_7_C_ANTI_SWAERYUNG,		// 
	SKILL_7_D_ANTI_YONGBI,			// 

	// 8
	SKILL_8_A_ANTI_GIGONGCHAM,		// 
	SKILL_8_B_ANTI_YEONSA,			// 
	SKILL_8_C_ANTI_MAHWAN,			// 
	SKILL_8_D_ANTI_BYEURAK,			// 

	// secondary skill
	SKILL_LEADERSHIP		= 121,	// Führung
	SKILL_COMBO				= 122,	// Combo
	SKILL_CREATE			= 123,	// 
	SKILL_MINING			= 124,	// Bergbau

	SKILL_LANGUAGE1			= 126,	// Shinsoo
	SKILL_LANGUAGE2			= 127,	// Chunjo
	SKILL_LANGUAGE3			= 128,	// Jinno
	SKILL_POLYMORPH			= 129,	// Verwandlung

	SKILL_HORSE				= 130,	// Reiten
	SKILL_HORSE_SUMMON		= 131,	// Pferd rufen
	SKILL_HORSE_WILDATTACK	= 137,	// Kampf vom Pferderücken
	SKILL_HORSE_CHARGE		= 138,	// Pferdestampfer
	SKILL_HORSE_ESCAPE		= 139,	// Kraftwelle
	SKILL_HORSE_WILDATTACK_RANGE = 140,	// Pfeilhagel

	SKILL_ADD_HP			= 141,	// Regenerationswille
	SKILL_RESIST_PENETRATE	= 142,	// Durchbruchzerstörung

	GUILD_SKILL_START		= 151,	// 
	GUILD_SKILL_EYE			= 151,	// Drachenaugen
	GUILD_SKILL_BLOOD		= 152,	// Blut des Drachengotts
	GUILD_SKILL_BLESS		= 153,	// Segnung des Drachengotts
	GUILD_SKILL_SEONGHWI	= 154,	// Heilige Rüstung
	GUILD_SKILL_ACCEL		= 155,	// Beschleunigung
	GUILD_SKILL_BUNNO		= 156,	// Wut des Drachengottes
	GUILD_SKILL_JUMUN		= 157,	// Zauberhilfe
	GUILD_SKILL_TELEPORT	= 158,	// 
	GUILD_SKILL_DOOR		= 159,	// 
	GUILD_SKILL_END			= 162,	// 
	GUILD_SKILL_COUNT = GUILD_SKILL_END - GUILD_SKILL_START + 1,

#ifdef ENABLE_78TH_SKILL
	// Konter-Fertigkeiten
	SKILL_ANTI_PALBANG		= 221,	// Schwertwirbel-Konter
	SKILL_ANTI_AMSEOP		= 222,	// Hinterhalt-Konter
	SKILL_ANTI_SWAERYUNG	= 223,	// Fingerschlag-Konter
	SKILL_ANTI_YONGBI		= 224,	// Drachenschießen-Konter
	SKILL_ANTI_GIGONGCHAM	= 225,	// Durchschlag-Konter
	SKILL_ANTI_HWAJO		= 226,	// Feuerpfeil-Konter
	SKILL_ANTI_MARYUNG		= 227,	// Dunkler Schlag-Konter
	SKILL_ANTI_BYEURAK		= 228,	// Blitz heraufbeschwören-Konter
#ifdef ENABLE_WOLFMAN_CHARACTER
	SKILL_ANTI_SALPOONG		= 229,	// Atem des Wolfes-Konter
#endif

	// Boost-Fertigkeiten
	SKILL_HELP_PALBANG		= 236,	// Schwertwirbel-Boost
	SKILL_HELP_AMSEOP		= 237,	// Hinterhalt-Boost
	SKILL_HELP_SWAERYUNG	= 238,	// Fingerschlag-Boost
	SKILL_HELP_YONGBI		= 239,	// Drachenschießen-Boost
	SKILL_HELP_GIGONGCHAM	= 240,	// Durchschlag-Boost
	SKILL_HELP_HWAJO		= 241,	// Feuerpfeil-Boost
	SKILL_HELP_MARYUNG		= 242,	// Dunkler Schlag-Boost
	SKILL_HELP_BYEURAK		= 243,	// Blitz heraufbeschwören-Boost
#ifdef ENABLE_WOLFMAN_CHARACTER
	SKILL_HELP_SALPOONG		= 244,	// Atem des Wolfes-Boost
#endif
#endif

};

class CSkillProto
{
	public:
		char szName[64];
		DWORD dwVnum;

		DWORD dwType;
		BYTE bMaxLevel;
		BYTE bLevelLimit;
		int iSplashRange;

		BYTE bPointOn;
		CPoly kPointPoly;

		CPoly kSPCostPoly;
		CPoly kDurationPoly;
		CPoly kDurationSPCostPoly;
		CPoly kCooldownPoly;
		CPoly kMasterBonusPoly;
		CPoly kSplashAroundDamageAdjustPoly;

		DWORD dwFlag;
		DWORD dwAffectFlag;

		BYTE bLevelStep;
		DWORD preSkillVnum;
		BYTE preSkillLevel;

		long lMaxHit;

		BYTE bSkillAttrType;

		BYTE bPointOn2;
		CPoly kPointPoly2;
		CPoly kDurationPoly2;
		DWORD dwFlag2;
		DWORD dwAffectFlag2;

		DWORD dwTargetRange;

		bool IsChargeSkill()
		{
			return dwVnum == SKILL_TANHWAN || dwVnum == SKILL_HORSE_CHARGE; 
		}

		BYTE bPointOn3;
		CPoly kPointPoly3;
		CPoly kDurationPoly3;

		CPoly kGrandMasterAddSPCostPoly;

		void SetPointVar(const std::string& strName, double dVar);
		void SetDurationVar(const std::string& strName, double dVar);
		void SetSPCostVar(const std::string& strName, double dVar);
};

class CSkillManager : public singleton<CSkillManager>
{
	public:
		CSkillManager();
		virtual ~CSkillManager();

		bool Initialize(TSkillTable* pTab, int iSize);
		CSkillProto* Get(DWORD dwVnum);
		CSkillProto* Get(const char* c_pszSkillName);

	protected:
		std::map<DWORD, CSkillProto*> m_map_pkSkillProto;
};

#endif
