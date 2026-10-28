#pragma once

#ifdef ENABLE_WIKI_SYSTEM
#include "../gamelib/ItemManager.h"
#include "../gamelib/ItemData.h"
#include "PythonNonPlayer.h"

enum
{
	MAX_REFINE_ITEM = 5,
};

typedef struct r_table
{
	DWORD id;
	DWORD item_vnums[MAX_REFINE_ITEM];
	DWORD item_count[MAX_REFINE_ITEM];
	DWORD cost;
	DWORD prob;
	DWORD refine_count;
} refineTable;

typedef struct refine_map
{
	DWORD itemVnum;
	DWORD level;
	refine_map(DWORD _vnum, DWORD _level) : itemVnum(_vnum), level(_level) {}
} character_data;

typedef struct special_map
{
	DWORD itemVnum;
	DWORD count;
	special_map(DWORD _vnum, DWORD _count): itemVnum(_vnum),count(_count){}
} special_data;

class CPythonWiki : public CSingleton <CPythonWiki>
{
	public:

		CPythonWiki();
		virtual ~CPythonWiki();

		void Destroy();

		bool LoadItemTable(const char *c_szFileName);
		bool LoadRefineTable(const char *c_szFileName);

		void ClearData();

		void LoadItem(CItemData::TItemTable* item);
		void LoadMonster(CPythonNonPlayer::TMobTable* monster);

		void ReadData(const char *localeFile);
		BYTE GetRefineLevel(DWORD vnum, DWORD type, DWORD subtype);

		refineTable* GetRefineItem(DWORD index);
		bool BlackList(DWORD vnum, DWORD type, DWORD subtype);

		bool ReadSpecialDropItemFile(const char *c_pszFileName);
		bool ReadMobDropItemFile(const char *c_pszFileName);

		void ListReverse();

		std::unordered_map<DWORD, std::vector<special_data>> GetSpecialDrop() {return m_vecSpecialDrop;}
		std::unordered_map<DWORD, std::vector<special_data>> GetMobDrop() {return m_vecMobDrop;}

		std::vector<character_data> m_vecBossCategory[3];
		std::vector<character_data> m_vecMonsterCategory[3];
		std::vector<character_data> m_vecStoneCategory[3];

		std::vector<character_data> m_vecWeapon[4];
		std::vector<character_data> m_vecArmor[4];
		std::vector<character_data> m_vecHelmets[4];
		std::vector<character_data> m_vecShields[4];
		std::vector<character_data> m_vecEarrings[4];
		std::vector<character_data> m_vecBracelet[4];
		std::vector<character_data> m_vecNecklace[4];
		std::vector<character_data> m_vecShoes[4];
		std::vector<character_data> m_vecBelt[4];
		std::vector<character_data> m_vecTalisman[4];

		std::unordered_map<DWORD, std::vector<special_data>> m_vecSpecialDrop;
		std::unordered_map<DWORD, std::vector<special_data>> m_vecMobDrop;

	protected:
		std::unordered_map<DWORD, refineTable*> m_vecRefineTable;
};
#endif
