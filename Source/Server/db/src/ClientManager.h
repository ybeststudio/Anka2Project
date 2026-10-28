#ifndef __INC_CLIENTMANAGER_H__
#define __INC_CLIENTMANAGER_H__

#include <unordered_map>
#include <unordered_set>
#include <chrono>

#include "../../common/stl.h"
#include "../../common/tables.h"
#include "../../common/service.h"
#include "../../common/building.h"

#include "Peer.h"
#include "DBManager.h"
#include "LoginData.h"

class CPlayerTableCache;
class CItemCache;
class CItemPriceListTableCache;
#ifdef ENABLE_SKILL_COLOR_SYSTEM
class CSKillColorCache;
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
class CGrowthPetCache;
#endif

class CPacketInfo
{
	public:
	void Add(int header);
	void Reset();

	std::map<int, int> m_map_info;
};

size_t CreatePlayerSaveQuery(char * pszQuery, size_t querySize, TPlayerTable * pkTab);

class CClientManager : public CNetBase, public singleton<CClientManager>
{
	public:
		typedef std::list<CPeer *> TPeerList;
		typedef std::unordered_map<DWORD, CPlayerTableCache *> TPlayerTableCacheMap;
		typedef std::unordered_map<DWORD, CItemCache *> TItemCacheMap;
		typedef std::unordered_set<CItemCache *, std::hash<CItemCache*> > TItemCacheSet;
		typedef std::unordered_map<DWORD, TItemCacheSet *> TItemCacheSetPtrMap;
		typedef std::unordered_map<DWORD, CItemPriceListTableCache*> TItemPriceListCacheMap;
		typedef std::unordered_map<int, BYTE> TChannelStatusMap;
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		typedef std::unordered_map<DWORD, CSKillColorCache*> TSkillColorCacheMap;
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		typedef std::unordered_map<DWORD, CGrowthPetCache*> TGrowthPetCacheMap;
		typedef std::unordered_set<CGrowthPetCache*, std::hash<CGrowthPetCache*> > TGrowthPetCacheSet;
		typedef std::unordered_map<DWORD, TGrowthPetCacheSet*> TGrowthPetCacheSetPtrMap;
#endif
		typedef std::pair< DWORD, DWORD > TItemPricelistReqInfo;

		class ClientHandleInfo
		{
			public:
			DWORD dwHandle;
			DWORD account_id;
			DWORD player_id;
			BYTE account_index;
			char login[LOGIN_MAX_LEN + 1];
			char safebox_password[SAFEBOX_PASSWORD_MAX_LEN + 1];
			char ip[MAX_HOST_LENGTH + 1];

			TAccountTable* pAccountTable;
			TSafeboxTable* pSafebox;

			ClientHandleInfo(DWORD argHandle, DWORD dwPID = 0)
			{
				dwHandle = argHandle;
				pSafebox = NULL;
				pAccountTable = NULL;
				player_id = dwPID;
			};

			ClientHandleInfo(DWORD argHandle, DWORD dwPID, DWORD accountId)
			{
				dwHandle = argHandle;
				pSafebox = NULL;
				pAccountTable = NULL;
				player_id = dwPID;
				account_id = accountId;
			};

			~ClientHandleInfo()
			{
				if (pSafebox)
				{
					delete pSafebox;
					pSafebox = NULL;
				}
			}
		};

	public:
		CClientManager();
		~CClientManager();

		bool	Initialize();
		time_t	GetCurrentTime();

		void MainLoop();
		void Quit();

		void SetTablePostfix(const char* c_pszTablePostfix);
		void SetPlayerIDStart(int iIDStart);
		int	GetPlayerIDStart() { return m_iPlayerIDStart; }

		int	GetPlayerDeleteLevelLimit() { return m_iPlayerDeleteLevelLimit; }

		void SetChinaEventServer(bool flag) { m_bChinaEventServer = flag; }
		bool IsChinaEventServer() { return m_bChinaEventServer; }

		DWORD	GetUserCount();

		void SendAllGuildSkillRechargePacket();
		void SendTime();

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		CSKillColorCache* GetSkillColorCache(DWORD dwID);
		void PutSkillColorCache(const TSkillColor* c_pNew);
		void UpdateSkillColorCache();
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
		void CreateGrowthPetCacheSet(DWORD dwID);
		TGrowthPetCacheSet* GetGrowthPetCacheSet(DWORD dwID);
		void FlushGrowthPetCacheSet(DWORD dwID);

		CGrowthPetCache* GetGrowthPetCache(DWORD id);
		void PutGrowthPetCache(TGrowthPet* pNew, bool bSkipQuery = false);
		bool DeleteGrowthPetCache(DWORD dwID);

		void UpdateGrowthPetCache();
		void UpdateGrowthPetCacheSet(DWORD pid);

		void QUERY_GROWTH_PET_SAVE(CPeer* pkPeer, const char* c_pData);
		void QUERY_GROWTH_PET_DELETE(CPeer* pkPeer, const char* c_pData);
		bool CreateGrowthPetTableFromRes(MYSQL_RES* res, std::vector<TGrowthPet>* pVec, DWORD dwPID);
		void RESULT_GROWTH_PET_LOAD(CPeer* peer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwPID);
#endif

		CPlayerTableCache *	GetPlayerCache(DWORD id);
		void PutPlayerCache(TPlayerTable * pNew);

		void CreateItemCacheSet(DWORD dwID);
		TItemCacheSet * GetItemCacheSet(DWORD dwID);
		void FlushItemCacheSet(DWORD dwID);

		CItemCache * GetItemCache(DWORD id);
		void PutItemCache(TPlayerItem * pNew, bool bSkipQuery = false);
		bool DeleteItemCache(DWORD id);

		void UpdatePlayerCache();
		void UpdateItemCache();

		CItemPriceListTableCache* GetItemPriceListCache(DWORD dwID);
		void PutItemPriceListCache(const TItemPriceListTable* pItemPriceList);

		void UpdateItemPriceListCache(void);

		void SendGuildSkillUsable(DWORD guild_id, DWORD dwSkillVnum, bool bUsable);

		void SetCacheFlushCountLimit(int iLimit);

		template <class Func>
		Func for_each_peer(Func f);

		CPeer * GetAnyPeer();

		void ForwardPacket(BYTE header, const void* data, int size, BYTE bChannel = 0, CPeer * except = NULL);

		void SendNotice(const char * c_pszFormat, ...);

	// @fixme203 directly GetCommand instead of strcpy
		char* GetCommand(char* str, char* command); //Function to get command from German gift function
		void ItemAward(CPeer * peer, char* login); //German gift function

	protected:
		void Destroy();

	private:
		bool InitializeTables();
		bool InitializeShopTable();
#ifdef ENABLE_RENEWAL_SHOPEX
		bool InitializeShopEXTable();
#endif
		bool InitializeMobTable();
		bool InitializeItemTable();
		bool InitializeQuestItemTable();
		bool InitializeSkillTable();
		bool InitializeRefineTable();
		bool InitializeBanwordTable();
		bool InitializeItemAttrTable();
		bool InitializeItemRareTable();
		bool InitializeLandTable();
		bool InitializeObjectProto();
		bool InitializeObjectTable();
#ifdef ENABLE_GROWTH_PET_SYSTEM
		bool InitializeGrowthPetSkillTable();
#endif

		// Reflect mob_proto and item_proto read from mob_proto.txt and item_proto.txt to real db.
		// Even if item_proto and mob_proto are not reflected in the db, there is no problem with the game running,
		// A problem occurs because the operation tool reads and writes item_proto and mob_proto of db.

		void AddPeer(socket_t fd);
		void RemovePeer(CPeer * pPeer);
		CPeer * GetPeer(IDENT ident);

		int AnalyzeQueryResult(SQLMsg * msg);
		int AnalyzeErrorMsg(CPeer * peer, SQLMsg * msg);

		int Process();

		void ProcessPackets(CPeer * peer);

		CLoginData * GetLoginData(DWORD dwKey);
		CLoginData * GetLoginDataByLogin(const char * c_pszLogin);
		CLoginData * GetLoginDataByAID(DWORD dwAID);

		void InsertLoginData(CLoginData * pkLD);
		void DeleteLoginData(CLoginData * pkLD);

		bool InsertLogonAccount(const char * c_pszLogin, DWORD dwHandle, const char * c_pszIP);
		bool DeleteLogonAccount(const char * c_pszLogin, DWORD dwHandle);
		bool FindLogonAccount(const char * c_pszLogin);

		void GuildCreate(CPeer * peer, DWORD dwGuildID);
		void GuildSkillUpdate(CPeer * peer, TPacketGuildSkillUpdate* p);
		void GuildExpUpdate(CPeer * peer, TPacketGuildExpUpdate* p);
		void GuildAddMember(CPeer * peer, TPacketGDGuildAddMember* p);
		void GuildChangeGrade(CPeer * peer, TPacketGuild* p);
		void GuildRemoveMember(CPeer * peer, TPacketGuild* p);
		void GuildChangeMemberData(CPeer * peer, TPacketGuildChangeMemberData* p);
		void GuildDisband(CPeer * peer, TPacketGuild * p);
		void GuildWar(CPeer * peer, TPacketGuildWar * p);
		void GuildWarScore(CPeer * peer, TPacketGuildWarScore * p);
		void GuildChangeLadderPoint(TPacketGuildLadderPoint* p);
		void GuildUseSkill(TPacketGuildUseSkill* p);
		void GuildDepositMoney(TPacketGDGuildMoney* p);
		void GuildWithdrawMoney(CPeer* peer, TPacketGDGuildMoney* p);
		void GuildWithdrawMoneyGiveReply(TPacketGDGuildMoneyWithdrawGiveReply* p);
		void GuildWarBet(TPacketGDGuildWarBet * p);
		void GuildChangeMaster(TPacketChangeGuildMaster* p);

		void SetGuildWarEndTime(DWORD guild_id1, DWORD guild_id2, time_t tEndTime);

		void QUERY_BOOT(CPeer * peer, TPacketGDBoot * p);

		void QUERY_LOGIN(CPeer * peer, DWORD dwHandle, SLoginPacket* data);
		void QUERY_LOGOUT(CPeer * peer, DWORD dwHandle, const char *);

		void RESULT_LOGIN(CPeer * peer, SQLMsg *msg);

		void QUERY_PLAYER_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket*);
		void RESULT_COMPOSITE_PLAYER(CPeer * peer, SQLMsg * pMsg, DWORD dwQID);
		void RESULT_PLAYER_LOAD(CPeer * peer, MYSQL_RES * pRes, ClientHandleInfo * pkInfo);
		void RESULT_ITEM_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID);
		void RESULT_QUEST_LOAD(CPeer * pkPeer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID);
	// @fixme402 (RESULT_AFFECT_LOAD +dwRealPID)
		void RESULT_AFFECT_LOAD(CPeer * pkPeer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwRealPID);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		void QUERY_SKILL_COLOR_LOAD(CPeer* peer, DWORD dwHandle, TPlayerLoadPacket* packet);
		void RESULT_SKILL_COLOR_LOAD(CPeer* peer, MYSQL_RES* pRes, DWORD dwHandle);
#endif

		void RESULT_PLAYER_INDEX_CREATE(CPeer *pkPeer, SQLMsg *msg);
		void RESULT_PRICELIST_LOAD(CPeer* peer, SQLMsg* pMsg);
		void RESULT_PRICELIST_LOAD_FOR_UPDATE(SQLMsg* pMsg);

		void QUERY_PLAYER_SAVE(CPeer * peer, DWORD dwHandle, TPlayerTable*);

		void __QUERY_PLAYER_CREATE(CPeer * peer, DWORD dwHandle, TPlayerCreatePacket *);
		void __QUERY_PLAYER_DELETE(CPeer * peer, DWORD dwHandle, TPlayerDeletePacket *);
		void __RESULT_PLAYER_DELETE(CPeer * peer, SQLMsg* msg);

		void QUERY_PLAYER_COUNT(CPeer * pkPeer, TPlayerCountPacket *);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		void QUERY_SKILL_COLOR_SAVE(const char* c_pData);
#endif

		void QUERY_ITEM_SAVE(CPeer * pkPeer, const char * c_pData);
		void QUERY_ITEM_DESTROY(CPeer * pkPeer, const char * c_pData);
		void QUERY_ITEM_FLUSH(CPeer * pkPeer, const char * c_pData);


		void QUERY_QUEST_SAVE(CPeer * pkPeer, TQuestTable *, DWORD dwLen);
		void QUERY_ADD_AFFECT(CPeer * pkPeer, TPacketGDAddAffect * p);
		void QUERY_REMOVE_AFFECT(CPeer * pkPeer, TPacketGDRemoveAffect * p);

		void QUERY_SAFEBOX_LOAD(CPeer * pkPeer, DWORD dwHandle, TSafeboxLoadPacket *, bool bMall);
		void QUERY_SAFEBOX_SAVE(CPeer * pkPeer, TSafeboxTable * pTable);
		void QUERY_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangeSizePacket * p);
		void QUERY_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangePasswordPacket * p);

		void RESULT_SAFEBOX_LOAD(CPeer * pkPeer, SQLMsg * msg);
		void RESULT_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, SQLMsg * msg);
		void RESULT_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, SQLMsg * msg);
		void RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(CPeer * pkPeer, SQLMsg * msg);

		void QUERY_EMPIRE_SELECT(CPeer * pkPeer, DWORD dwHandle, TEmpireSelectPacket * p);
		void QUERY_SETUP(CPeer * pkPeer, DWORD dwHandle, const char * c_pData);

		void SendPartyOnSetup(CPeer * peer);

		void QUERY_HIGHSCORE_REGISTER(CPeer * peer, TPacketGDHighscore* data);
		void RESULT_HIGHSCORE_REGISTER(CPeer * pkPeer, SQLMsg * msg);

		void QUERY_FLUSH_CACHE(CPeer * pkPeer, const char * c_pData);

		void QUERY_PARTY_CREATE(CPeer * peer, TPacketPartyCreate* p);
		void QUERY_PARTY_DELETE(CPeer * peer, TPacketPartyDelete* p);
		void QUERY_PARTY_ADD(CPeer * peer, TPacketPartyAdd* p);
		void QUERY_PARTY_REMOVE(CPeer * peer, TPacketPartyRemove* p);
		void QUERY_PARTY_STATE_CHANGE(CPeer * peer, TPacketPartyStateChange* p);
		void QUERY_PARTY_SET_MEMBER_LEVEL(CPeer * peer, TPacketPartySetMemberLevel* p);

		void QUERY_RELOAD_PROTO();

		void QUERY_CHANGE_NAME(CPeer * peer, DWORD dwHandle, TPacketGDChangeName * p);
		void GetPlayerFromRes(TPlayerTable * player_table, MYSQL_RES* res);

		void QUERY_LOGIN_KEY(CPeer * pkPeer, TPacketGDLoginKey * p);

		void AddGuildPriv(TPacketGiveGuildPriv* p);
		void AddEmpirePriv(TPacketGiveEmpirePriv* p);
		void AddCharacterPriv(TPacketGiveCharacterPriv* p);

		void MoneyLog(TPacketMoneyLog* p);

		void QUERY_AUTH_LOGIN(CPeer * pkPeer, DWORD dwHandle, TPacketGDAuthLogin * p);

		void QUERY_LOGIN_BY_KEY(CPeer * pkPeer, DWORD dwHandle, TPacketGDLoginByKey * p);
		void RESULT_LOGIN_BY_KEY(CPeer * peer, SQLMsg * msg);

		void ChargeCash(const TRequestChargeCash * p);

		void LoadEventFlag();
		void SetEventFlag(TPacketSetEventFlag* p);
		void SendEventFlagsOnSetup(CPeer* peer);

		void MarriageAdd(TPacketMarriageAdd * p);
		void MarriageUpdate(TPacketMarriageUpdate * p);
		void MarriageRemove(TPacketMarriageRemove * p);

		void WeddingRequest(TPacketWeddingRequest * p);
		void WeddingReady(TPacketWeddingReady * p);
		void WeddingEnd(TPacketWeddingEnd * p);

		void MyshopPricelistUpdate(const TPacketMyshopPricelistHeader* pPacket);
		void MyshopPricelistRequest(CPeer* peer, DWORD dwHandle, DWORD dwPlayerID);

		void CreateObject(TPacketGDCreateObject * p);
		void DeleteObject(DWORD dwID);
		void UpdateLand(DWORD * pdw);

		void BlockChat(TPacketBlockChat * p);

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		void ChangeLanguage(const TRequestChangeLanguage* p);
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
		void RequestReadOfflineMessages(CPeer* pkPeer, DWORD dwHandle, TPacketGDReadOfflineMessage* p);
		void SendOfflineMessage(TPacketGDSendOfflineMessage* p);
		void OfflineMessageGarbage();
#endif

	private:
		int m_looping;
		socket_t m_fdAccept;
		TPeerList m_peerList;

		CPeer * m_pkAuthPeer;

		typedef std::unordered_map<DWORD, CLoginData *> TLoginDataByLoginKey;
		TLoginDataByLoginKey m_map_pkLoginData;

		typedef std::unordered_map<std::string, CLoginData *> TLoginDataByLogin;
		TLoginDataByLogin m_map_pkLoginDataByLogin;

		typedef std::unordered_map<DWORD, CLoginData *> TLoginDataByAID;
		TLoginDataByAID m_map_pkLoginDataByAID;

		typedef std::unordered_map<std::string, CLoginData *> TLogonAccountMap;
		TLogonAccountMap m_map_kLogonAccount;

		int m_iPlayerIDStart;
		int m_iPlayerDeleteLevelLimit;
		int m_iPlayerDeleteLevelLimitLower;
		bool m_bChinaEventServer;

		std::vector<TMobTable> m_vec_mobTable;
		std::vector<TItemTable> m_vec_itemTable;
		std::map<DWORD, TItemTable *> m_map_itemTableByVnum;

		int m_iShopTableSize;
		TShopTable * m_pShopTable;

#ifdef ENABLE_RENEWAL_SHOPEX
		TShopTable* m_pShopEXTable;
		int m_iShopEXTableSize;
#endif

		int m_iRefineTableSize;
		TRefineTable* m_pRefineTable;

		std::vector<TSkillTable> m_vec_skillTable;
		std::vector<TBanwordTable> m_vec_banwordTable;
		std::vector<TItemAttrTable> m_vec_itemAttrTable;
		std::vector<TItemAttrTable> m_vec_itemRareTable;

		std::vector<building::TLand> m_vec_kLandTable;
		std::vector<building::TObjectProto> m_vec_kObjectProto;
		std::map<DWORD, building::TObject*> m_map_pkObjectTable;

#ifdef ENABLE_GROWTH_PET_SYSTEM
		std::vector<TGrowthPetSkillTable> m_vec_growthPetSkillTable;
#endif

		bool m_bShutdowned;

		TPlayerTableCacheMap m_map_playerCache;

		TItemCacheMap m_map_itemCache;
		TItemCacheSetPtrMap m_map_pkItemCacheSetPtr;
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		TSkillColorCacheMap m_map_SkillColorCache;
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		TGrowthPetCacheMap m_map_growthPetCache;
		TGrowthPetCacheSetPtrMap m_map_pkGrowthPetCacheSetPtr;
#endif
		TItemPriceListCacheMap m_mapItemPriceListCache;

		TChannelStatusMap m_mChannelStatus;

		struct TPartyInfo
		{
			BYTE bRole;
#ifdef ENABLE_LEVEL_INT
			int bLevel;
#else
			BYTE bLevel;
#endif

			TPartyInfo() :bRole(0), bLevel(0) {}
		};

		typedef std::map<DWORD, TPartyInfo> TPartyMember;
		typedef std::map<DWORD, TPartyMember> TPartyMap;
		typedef std::map<BYTE, TPartyMap> TPartyChannelMap;
		TPartyChannelMap m_map_pkChannelParty;

		typedef std::map<std::string, long> TEventFlagMap;
		TEventFlagMap m_map_lEventFlag;

		BYTE m_bLastHeader;
		int m_iCacheFlushCount;
		int m_iCacheFlushCountLimit;

#ifdef ENABLE_OFFLINE_MESSAGE
		struct SOfflineMessage
		{
			std::string From;
			std::string Message;
			std::chrono::system_clock::time_point t;
			SOfflineMessage(const char* szFrom, const char* szMessage)
				: From(szFrom), Message(szMessage), t(std::chrono::system_clock::now()) {}
		};
		std::unordered_map<std::string, std::vector<std::shared_ptr<SOfflineMessage>>> m_OfflineMessage;
#endif

	private :
		TItemIDRangeTable m_itemRange;

	public :
		bool InitializeNowItemID();
		DWORD GetItemID();
		DWORD GainItemID();
		TItemIDRangeTable GetItemRange() { return m_itemRange; }

	public:
		bool InitializeLocalization(); 

	private:
		std::vector<tLocale> m_vec_Locale;

		bool __GetAdminInfo(const char *szIP, std::vector<tAdminInfo> & rAdminVec);
		bool __GetHostInfo(std::vector<std::string> & rIPVec);

		void ReloadAdmin(CPeer * peer, TPacketReloadAdmin * p);

		void BreakMarriage(CPeer * peer, const char * data);

		struct TLogoutPlayer
		{
			DWORD pid;
			time_t time;

			bool operator < (const TLogoutPlayer & r) 
			{
			return (pid < r.pid);
			}
		};

		typedef std::unordered_map<DWORD, TLogoutPlayer*> TLogoutPlayerMap;
		TLogoutPlayerMap m_map_logout;

		void InsertLogoutPlayer(DWORD pid);
		void DeleteLogoutPlayer(DWORD pid);
		void UpdateLogoutPlayer();
		void UpdateItemCacheSet(DWORD pid);

		void FlushPlayerCacheSet(DWORD pid);

		void SendSpareItemIDRange(CPeer* peer);

		void UpdateHorseName(TPacketUpdateHorseName* data, CPeer* peer);
		void AckHorseName(DWORD dwPID, CPeer* peer);
		void DeleteLoginKey(TPacketDC *data);
		void ResetLastPlayerID(const TPacketNeedLoginLogInfo* data);

		void DeleteAwardId(TPacketDeleteAwardID* data);
		void UpdateChannelStatus(TChannelStatus* pData);
		void RequestChannelStatus(CPeer* peer, DWORD dwHandle);

#ifdef ENABLE_RANKING
		void QUERY_ADD_RANKING(CPeer * peer, TPacketGDAddRanking * p, DWORD dwLen);
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
		std::map<DWORD, TOfflineShop*> m_Offlineshop;
		void OfflineShopAssert(TOfflineShop* p);
		void CreateOfflineShop(TOfflineShop* p);
		void DeleteOfflineShop(DWORD ownerid);
		void RemoveItem(OFFLINE_SHOP_ITEM* item);
		void AddItem(OFFLINE_SHOP_ITEM* item);
		void BuyItem(TOfflineShopBuy* item);
		void WithdrawMoney(DWORD ch);
		void ClearLog(DWORD ch);
		void ChangeDecoration(TShopDecoration* ch);
		void ChangeTitle(TOfflineShopChangeTitle* p);
		void OpenSlot(TOfflineShopOpenSlot* p);
		void TimeDone(DWORD ch);
		void GetBackItem(DWORD p);
		void AddTime(DWORD p);
		void RecvPackets(const char* data);
		bool InitializeOfflineShop();
#endif

#ifdef ENABLE_ITEMSHOP
	protected:
		int itemShopUpdateTime;
		std::map<BYTE, std::map<BYTE, std::vector<TIShopData>>> m_IShopManager;
		std::map<DWORD, std::vector<TIShopLogData>> m_IShopLogManager;
	
	public:
		bool InitializeItemShop();
		void SendItemShopData(CPeer* pkPeer = NULL, bool isPacket = false);
		void RecvItemShop(CPeer* pkPeer, DWORD dwHandle, const char* data);
	
#ifdef USE_ITEMSHOP_RENEWED
		void GetAccountMoney(DWORD id, long long& coins, long long& jcoins);
		void SetAccountMoney(DWORD id, long long coins, long long jcoins);
#else
		long long GetDragonCoin(DWORD id);
		void SetDragonCoin(DWORD id, long long amount);
#endif
	
		void ItemShopIncreaseSellCount(DWORD itemID, int itemCount);
		void SetEventFlag(const char* flag, int value);
		int GetEventFlag(const char* flag);
#endif
};

template<class Func>
Func CClientManager::for_each_peer(Func f)
{
	TPeerList::iterator it;
	for (it = m_peerList.begin(); it!=m_peerList.end();++it)
	{
		f(*it);
	}
	return f;
}
#endif
