#ifndef __INC_METIN_II_GAME_CHARACTER_MANAGER_H__
#define __INC_METIN_II_GAME_CHARACTER_MANAGER_H__

#include "../../common/stl.h"
#include "../../common/tables.h"
#include "../../common/length.h"
#include "../../common/service.h"

#include "vid.h"

class CDungeon;
class CHARACTER;
class CharacterVectorInteractor;

class CHARACTER_MANAGER : public singleton<CHARACTER_MANAGER>
{
	public:
		typedef std::unordered_map<std::string, LPCHARACTER> NAME_MAP;

		CHARACTER_MANAGER();
		virtual ~CHARACTER_MANAGER();

		void Destroy();

		void GracefulShutdown();

		DWORD AllocVID();

		LPCHARACTER CreateCharacter(const char * name, DWORD dwPID = 0);

		void DestroyCharacter(LPCHARACTER ch);
		void DestroyCharacterInMap(long lMapIndex);

		void Update(int iPulse);

#ifdef ENABLE_SHOW_MOB_INFO
		LPCHARACTER SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion = false, int iRot = -1, bool bShow = true, bool bAggressive = false);
#else
		LPCHARACTER SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion = false, int iRot = -1, bool bShow = true);
#endif
		LPCHARACTER SpawnMobRange(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, bool bIsException=false, bool bSpawnMotion = false , bool bAggressive = false);
		LPCHARACTER SpawnGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen = NULL, bool bAggressive_ = false, LPDUNGEON pDungeon = NULL);
		bool SpawnGroupGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen = NULL, bool bAggressive_ = false, LPDUNGEON pDungeon = NULL);
		bool SpawnMoveGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, int tx, int ty, LPREGEN pkRegen = NULL, bool bAggressive_ = false);
		LPCHARACTER SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex);

		void SelectStone(LPCHARACTER pkChrStone);

		NAME_MAP & GetPCMap() { return m_map_pkPCChr; }

		LPCHARACTER Find(DWORD dwVID);
		LPCHARACTER Find(const VID & vid);
		LPCHARACTER FindPC(const char * name);
		LPCHARACTER FindByPID(DWORD dwPID);

		bool AddToStateList(LPCHARACTER ch);
		void RemoveFromStateList(LPCHARACTER ch);

		void DelayedSave(LPCHARACTER ch);
		bool FlushDelayedSave(LPCHARACTER ch);
		void ProcessDelayedSave();

		template<class Func>	Func for_each_pc(Func f);

		void RegisterForMonsterLog(LPCHARACTER ch);
		void UnregisterForMonsterLog(LPCHARACTER ch);
		void PacketMonsterLog(LPCHARACTER ch, const void* buf, int size);

		void KillLog(DWORD dwVnum);

		void RegisterRaceNum(DWORD dwVnum);
		void RegisterRaceNumMap(LPCHARACTER ch);
		void UnregisterRaceNumMap(LPCHARACTER ch);
		bool GetCharactersByRaceNum(DWORD dwRaceNum, CharacterVectorInteractor & i);

		LPCHARACTER FindSpecifyPC(unsigned int uiJobFlag, long lMapIndex, LPCHARACTER except=NULL, int iMinLevel = 1, int iMaxLevel = PLAYER_MAX_LEVEL_CONST);

		void SetMobItemRate(int value) { m_iMobItemRate = value; }
		void SetMobDamageRate(int value) { m_iMobDamageRate = value; }
		void SetMobGoldAmountRate(int value) { m_iMobGoldAmountRate = value; }
		void SetMobGoldDropRate(int value) { m_iMobGoldDropRate = value; }
		void SetMobExpRate(int value) { m_iMobExpRate = value; }

		void SetMobItemRatePremium(int value) { m_iMobItemRatePremium = value; }
		void SetMobGoldAmountRatePremium(int value) { m_iMobGoldAmountRatePremium = value; }
		void SetMobGoldDropRatePremium(int value) { m_iMobGoldDropRatePremium = value; }
		void SetMobExpRatePremium(int value) { m_iMobExpRatePremium = value; }

		void SetUserDamageRatePremium(int value) { m_iUserDamageRatePremium = value; }
		void SetUserDamageRate(int value ) { m_iUserDamageRate = value; }
		int GetMobItemRate(LPCHARACTER ch);
		int GetMobDamageRate(LPCHARACTER ch);
		int GetMobGoldAmountRate(LPCHARACTER ch);
		int GetMobGoldDropRate(LPCHARACTER ch);
		int GetMobExpRate(LPCHARACTER ch);

		int GetUserDamageRate(LPCHARACTER ch);
		void SendScriptToMap(long lMapIndex, const std::string & s); 

		bool BeginPendingDestroy();
		void FlushPendingDestroy();

	private:
		int m_iMobItemRate;
		int m_iMobDamageRate;
		int m_iMobGoldAmountRate;
		int m_iMobGoldDropRate;
		int m_iMobExpRate;

		int m_iMobItemRatePremium;
		int m_iMobGoldAmountRatePremium;
		int m_iMobGoldDropRatePremium;
		int m_iMobExpRatePremium;

		int m_iUserDamageRate;
		int m_iUserDamageRatePremium;
		int m_iVIDCount;

		std::unordered_map<DWORD, LPCHARACTER> m_map_pkChrByVID;
		std::unordered_map<DWORD, LPCHARACTER> m_map_pkChrByPID;
		NAME_MAP m_map_pkPCChr;

		char dummy1[1024];
		CHARACTER_SET m_set_pkChrState;
		CHARACTER_SET m_set_pkChrForDelayedSave;
		CHARACTER_SET m_set_pkChrMonsterLog;

		LPCHARACTER m_pkChrSelectedStone;

		std::map<DWORD, DWORD> m_map_dwMobKillCount;

		std::set<DWORD> m_set_dwRegisteredRaceNum;
		std::map<DWORD, CHARACTER_SET> m_map_pkChrByRaceNum;

		bool m_bUsePendingDestroy;
		CHARACTER_SET m_set_pkChrPendingDestroy;

#ifdef __AUTO_HUNT__
	public:
		// Metin/slot rezervasyon sistemi - bir oyuncu bir metine/slota saldýrdýðýnda rezerve edilir
		bool ReserveTarget(DWORD dwTargetVID, DWORD dwPlayerPID);
		void ReleaseTarget(DWORD dwTargetVID);
		void ReleaseAllTargetsByPlayer(DWORD dwPlayerPID);
		bool IsTargetReserved(DWORD dwTargetVID, DWORD dwPlayerPID = 0); // dwPlayerPID = 0 ise herhangi biri tarafýndan rezerve edilmiþ mi kontrol eder
		DWORD GetReserverPID(DWORD dwTargetVID); // Hedefi rezerve eden oyuncunun PID'sini döndürür, yoksa 0

	protected:
		std::map<DWORD, DWORD> m_mapTargetReservation; // VID -> PID (rezerve eden oyuncu)
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
	public:
		int GetMultiFarmCount(const char* playerIP, std::map<DWORD, std::pair<std::string, bool>>& m_mapNames);
		void CheckMultiFarmAccount(const char* szIP, const DWORD playerID, const char* playerName, const bool bStatus, BYTE affectType = 0, int affectDuration = 0, bool isP2P = false);
		void SetMultiFarm(const char* szIP, const DWORD playerID, const char* playerName, const bool bStatus, const BYTE affectType, const int affectTime);
		void RemoveMultiFarm(const char* szIP, const DWORD playerID, const bool isP2P);
		void CheckMultiFarmAccounts(const char* szIP);

	protected:
		std::map<std::string, std::vector<TMultiFarm>> m_mapmultiFarm;
#endif

#ifdef ENABLE_RENEWAL_INGAME_ITEMSHOP
	public:
		void LoadItemShopData(const char* c_pData);
		void LoadItemShopData(LPCHARACTER ch, bool isAll = true);
		void LoadItemShopLog(LPCHARACTER ch);
		void LoadItemShopLogReal(LPCHARACTER ch, const char* c_pData);
		void LoadItemShopBuy(LPCHARACTER ch, int itemID, int itemCount);
		void LoadItemShopBuyReal(LPCHARACTER ch, const char* c_pData);
		int GetItemShopUpdateTime() { return itemshopUpdateTime; }
		void UpdateItemShopItem(const char* c_pData);

	protected:
		int itemshopUpdateTime;
		std::map<BYTE, std::map<BYTE, std::vector<TIShopData>>> m_IShopManager;
#endif
};

	template<class Func>
Func CHARACTER_MANAGER::for_each_pc(Func f)
{
	std::unordered_map<DWORD, LPCHARACTER>::iterator it;

	for (it = m_map_pkChrByPID.begin(); it != m_map_pkChrByPID.end(); ++it)
		f(it->second);

	return f;
}

class CharacterVectorInteractor : public CHARACTER_VECTOR
{
	public:
		CharacterVectorInteractor() : m_bMyBegin(false) {}

		CharacterVectorInteractor(const CHARACTER_SET & r);
		virtual ~CharacterVectorInteractor();

	private:
		bool m_bMyBegin;
};

#define M2_DESTROY_CHARACTER(ptr) CHARACTER_MANAGER::instance().DestroyCharacter(ptr)

#endif
