// NonPlayerCharacterInfo.h: interface for the NonPlayerCharacterInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NONPLAYERCHARACTERINFO_H__356E26F9_0EDD_4A2F_9E4C_CFC032B88C3A__INCLUDED_)
#define AFX_NONPLAYERCHARACTERINFO_H__356E26F9_0EDD_4A2F_9E4C_CFC032B88C3A__INCLUDED_

#pragma once

class CNonPlayerCharacterInfo : public CSingleton<CNonPlayerCharacterInfo>
{
	public:
		enum
		{
			CHARACTER_NAME_MAX_LEN = 24,
		};

		enum EInstanceType
		{
			INSTANCE_TYPE_ENEMY,
			INSTANCE_TYPE_NPC,
			INSTANCE_TYPE_PLAYER,

			INSTANCE_TYPE_MAX_NUM,
		};

		enum EMobEnchants
		{
			MOB_ENCHANT_CURSE,
			MOB_ENCHANT_SLOW,
			MOB_ENCHANT_POISON,
			MOB_ENCHANT_STUN,
			MOB_ENCHANT_CRITICAL,
			MOB_ENCHANT_PENETRATE,
			MOB_ENCHANTS_MAX_NUM
		};
		enum EMobResists
		{
			MOB_RESIST_SWORD,
			MOB_RESIST_TWOHAND,
			MOB_RESIST_DAGGER,
			MOB_RESIST_BELL,
			MOB_RESIST_FAN,
			MOB_RESIST_BOW,
			MOB_RESIST_FIRE,
			MOB_RESIST_ELECT,
			MOB_RESIST_MAGIC,
			MOB_RESIST_WIND,
			MOB_RESIST_POISON,
			MOB_RESISTS_MAX_NUM
		};

#pragma pack(push)
#pragma pack(1)
		typedef struct SMobTable
		{
			DWORD       dwVnum;
			char        szName[CHARACTER_NAME_MAX_LEN + 1];
			char        szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

			BYTE        bType;                  // Monster, NPC
			BYTE        bRank;                  // PAWN, KNIGHT, KING
			BYTE        bBattleType;            // MELEE, etc..
			BYTE        abLevelRange[2];        // Level 결정 범위: [0] ~ [1]
			BYTE        bSize;

			DWORD       dwGold;
			DWORD       dwExp;

			DWORD       dwAIFlag;
			DWORD       dwRaceFlag;
			DWORD       dwImmuneFlag;

			BYTE        bStr, bDex, bCon, bInt;
			DWORD       dwDamageRange[2];

			short       sAttackSpeed;
			short       sMovingSpeed;
			BYTE        bAggresiveHPPct;
			BYTE        wAggressiveSight;

			char        cEnchants[MOB_ENCHANTS_MAX_NUM];
			char        cResists[MOB_RESISTS_MAX_NUM];

			BYTE        bMountCapacity;

			BYTE        bOnClickType;

			BYTE        bEmpire;
			char        szFolder[64 + 1];
		} TMobTable;
#pragma pack(pop)

		typedef struct SNPCGroup
		{
			char				m_szName[CHARACTER_NAME_MAX_LEN + 1];
			DWORD				m_dwLeaderID;
			std::vector<DWORD>	m_FollowerIDVector;
		} TNPCGroup;


	public:
		CNonPlayerCharacterInfo();
		~CNonPlayerCharacterInfo();

		void Clear();
		void Destroy();

		bool				LoadNonPlayerData (const char* c_szFileName);
		bool				LoadNPCGroupData (const char* c_szFileName);

		const char* 		GetNameByVID (DWORD dwVID);
		BYTE				GetRankByVID (DWORD dwVID);
		void				GetLevelRangeByVID (DWORD dwVID, BYTE * pbyLowLevelLimit, BYTE * pbyHighLevelLimit);

		DWORD				GetVIDByName (std::string strName);
		const TMobTable* 	GetTable (DWORD dwVnum);
		BYTE				GetInstanceType (DWORD dwVnum);

		DWORD				GetNonPlayerCount();
		const char* 		GetNameByMapIndex (DWORD dwMapIndex);
		BYTE				GetRankByMapIndex (DWORD dwMapIndex);
		DWORD				GetVIDByMapIndex (DWORD dwMapIndex);
		void				GetLevelRangeByMapIndex (DWORD dwMapIndex, BYTE * pbyLowLevelLimit, BYTE * pbyHighLevelLimit);

		// Group
		const TNPCGroup* 	GetGroup (DWORD dwGroupID);
		DWORD				GetNPCGroupCount();
		const char* 		GetNPCGroupNameByMapIndex (DWORD dwMapIndex);
		DWORD				GetNPCGroupIDByMapIndex (DWORD dwMapIndex);
		DWORD				GetNPCGroupLeaderVIDByMapIndex (DWORD dwMapIndex);
		const char* 		GetNPCGroupLeaderNameByMapIndex (DWORD dwMapIndex);
		std::vector<DWORD>	GetNPCGroupFollowerVIDsByMapIndex (DWORD dwMapIndex);
		DWORD				GetNPCGroupFollowerCountByMapIndex (DWORD dwMapIndex);

		DWORD				GetGroupIDByGroupName (std::string strGroupName);

		const char* 		GetNPCGroupNameByGroupID (DWORD dwGroupID);
		DWORD				GetNPCGroupLeaderVIDByGroupID (DWORD dwGroupID);
		const char* 		GetNPCGroupLeaderNameByGroupID (DWORD dwGroupID);
		std::vector<DWORD>	GetNPCGroupFollowerVIDsByGroupID (DWORD dwGroupID);
		DWORD				GetNPCGroupFollowerCountByGroupID (DWORD dwGroupID);

	private:

		typedef std::map<DWORD, TMobTable*> TNonPlayerDataMap;
		TNonPlayerDataMap					m_NonPlayerDataMap;
		TNonPlayerDataMap::iterator			m_NonPlayerDataMapIterator;

		typedef std::map<DWORD, TNPCGroup*> TNPCGroupDataMap;
		TNPCGroupDataMap					m_NPCGroupDataMap;
		TNPCGroupDataMap::iterator			m_NPCGroupDataMapIterator;

};

#endif // !defined(AFX_NONPLAYERCHARACTERINFO_H__356E26F9_0EDD_4A2F_9E4C_CFC032B88C3A__INCLUDED_)
