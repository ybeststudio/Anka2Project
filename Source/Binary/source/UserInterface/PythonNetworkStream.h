#pragma once

#include "../eterLib/FuncObject.h"
#include "../eterlib/NetStream.h"
#include "../eterLib/NetPacketHeaderMap.h"

#include "StdAfx.h"
#include "InsultChecker.h"
#include "Packet.h"

#ifdef ENABLE_RENEWAL_SWITCHBOT
	#include "PythonSwitchbot.h"
#endif

class CInstanceBase;
class CNetworkActorManager;
struct SNetworkActorData;
struct SNetworkUpdateActorData;

class CPythonNetworkStream : public CNetworkStream, public CSingleton<CPythonNetworkStream>
{
	public:
		enum
		{
			SERVER_COMMAND_LOG_OUT = 0,
			SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER = 1,
			SERVER_COMMAND_QUIT = 2,

			MAX_ACCOUNT_PLAYER
		};
		
		enum
		{
			ERROR_NONE,
			ERROR_UNKNOWN,
			ERROR_CONNECT_MARK_SERVER,
			ERROR_LOAD_MARK,
			ERROR_MARK_WIDTH,
			ERROR_MARK_HEIGHT,
			ERROR_MARK_UPLOAD_NEED_RECONNECT,
			ERROR_MARK_CHECK_NEED_RECONNECT,
		};

		enum
		{
			ACCOUNT_CHARACTER_SLOT_ID,
			ACCOUNT_CHARACTER_SLOT_NAME,
			ACCOUNT_CHARACTER_SLOT_RACE,
			ACCOUNT_CHARACTER_SLOT_LEVEL,
			ACCOUNT_CHARACTER_SLOT_STR,
			ACCOUNT_CHARACTER_SLOT_DEX,
			ACCOUNT_CHARACTER_SLOT_HTH,
			ACCOUNT_CHARACTER_SLOT_INT,
			ACCOUNT_CHARACTER_SLOT_PLAYTIME,
			ACCOUNT_CHARACTER_SLOT_FORM,
			ACCOUNT_CHARACTER_SLOT_ADDR,
			ACCOUNT_CHARACTER_SLOT_PORT,
			ACCOUNT_CHARACTER_SLOT_GUILD_ID,
			ACCOUNT_CHARACTER_SLOT_GUILD_NAME,
			ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG,
			ACCOUNT_CHARACTER_SLOT_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			ACCOUNT_CHARACTER_SLOT_ACCE,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			ACCOUNT_CHARACTER_SLOT_AURA,
#endif
			ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME,
			ACCOUNT_CHARACTER_SLOT_MAPINDEX,
		};

		enum
		{
			PHASE_WINDOW_LOGO,
			PHASE_WINDOW_LOGIN,
			PHASE_WINDOW_SELECT,
			PHASE_WINDOW_CREATE,
			PHASE_WINDOW_LOAD,
			PHASE_WINDOW_GAME,
			PHASE_WINDOW_EMPIRE,
			PHASE_WINDOW_NUM,
		};

	public:
		CPythonNetworkStream();
		virtual ~CPythonNetworkStream();

		bool SendSpecial(int nLen, void * pvBuf);

		void StartGame();
		void Warp(LONG lGlobalX, LONG lGlobalY);

		void NotifyHack(const char *c_szMsg);
		void SetWaitFlag();

#ifdef ENABLE_DISCORD_RPC
		void Discord_Start();
		void Discord_Close();
		void Discord_Update(const bool bInGame);
#endif

		void SendEmoticon(UINT eEmoticon);

		void ExitApplication();
		void ExitGame();
		void LogOutGame();
		void AbsoluteExitGame();
		void AbsoluteExitApplication();

		void EnableChatInsultFilter(bool isEnable);
		bool IsChatInsultIn(const char *c_szMsg);
		bool IsInsultIn(const char *c_szMsg);

		DWORD GetGuildID();

		UINT UploadMark(const char *c_szImageFileName);
		UINT UploadSymbol(const char *c_szImageFileName);

		bool LoadInsultList(const char *c_szInsultListFileName);
		bool LoadConvertTable(DWORD dwEmpireID, const char *c_szFileName);

		UINT GetAccountCharacterSlotDatau(UINT iSlot, UINT eType);
		const char *GetAccountCharacterSlotDataz(UINT iSlot, UINT eType);

		// SUPPORT_BGM
		const char *GetFieldMusicFileName();
		float GetFieldMusicVolume();
		// END_OF_SUPPORT_BGM

		bool IsSelectedEmpire();

		void ToggleGameDebugInfo();

		void SetMarkServer(const char *c_szAddr, UINT uPort);
		void ConnectLoginServer(const char *c_szAddr, UINT uPort);
		void ConnectGameServer(UINT iChrSlot);

		void SetLoginInfo(const char *c_szID, const char *c_szPassword);
		void SetLoginKey(DWORD dwLoginKey);
		void ClearLoginInfo( void );

		void SetHandler(PyObject *poHandler);
		void SetPhaseWindow(UINT ePhaseWnd, PyObject *poPhaseWnd);
		void ClearPhaseWindow(UINT ePhaseWnd, PyObject *poPhaseWnd);
		void SetServerCommandParserWindow(PyObject *poPhaseWnd);

		bool SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY);

		bool SendAttackPacket(UINT uMotAttack, DWORD dwVIDVictim);
		bool SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
		bool SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID=0);
		bool SendTargetPacket(DWORD dwVID);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		bool SendSkillColorPacket(BYTE bSkillSlot, DWORD dwCol1, DWORD dwCol2, DWORD dwCol3, DWORD dwCol4, DWORD dwCol5);
#endif

		// OLDCODE:
		bool SendCharacterStartWalkingPacket(float fRotation, long lx, long ly);
		bool SendCharacterEndWalkingPacket(float fRotation, long lx, long ly);
		bool SendCharacterCheckWalkingPacket(float fRotation, long lx, long ly);

		bool SendCharacterPositionPacket(BYTE iPosition);

		bool SendItemUsePacket(TItemPos pos);
		bool SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos);
		bool SendItemDropPacket(TItemPos pos, DWORD elk);
		bool SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD count);
#if defined(ENABLE_DESTROY_DIALOG) && defined(ENABLE_STACK_LIMIT)
		bool SendItemDestroyPacket(TItemPos pos, DWORD elk, WORD count);
#elif defined(ENABLE_DESTROY_DIALOG)
		bool SendItemDestroyPacket(TItemPos pos, DWORD elk, BYTE count);
#endif
#ifdef ENABLE_STACK_LIMIT
		bool SendItemMovePacket(TItemPos pos, TItemPos change_pos, WORD num);
#else
		bool SendItemMovePacket(TItemPos pos, TItemPos change_pos, BYTE num);
#endif
		bool SendItemPickUpPacket(DWORD vid);

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
		bool SendItemsPickUpPacket(std::vector<DWORD> itemVector);
#endif

#ifdef ENABLE_QUICK_SELL_ITEM
		bool SendItemSellPacket(TItemPos pos, DWORD elk);
#endif

		bool SendQuickSlotAddPacket(BYTE wpos, BYTE type, BYTE pos);
		bool SendQuickSlotDelPacket(BYTE wpos);
		bool SendQuickSlotMovePacket(BYTE wpos, BYTE change_pos);

		bool SendPointResetPacket();

		// Shop
		bool SendShopEndPacket();
		bool SendShopBuyPacket(BYTE byCount);
		bool SendShopSellPacket(BYTE bySlot);
#ifdef ENABLE_STACK_LIMIT
		bool SendShopSellPacketNew(BYTE bySlot, WORD byCount);
#else
		bool SendShopSellPacketNew(BYTE bySlot, BYTE byCount);
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
		// Offline Shop
		bool SendOfflineShopEndPacket();
		bool SendOfflineShopBuyPacket(DWORD vid, BYTE pos);
		bool SendAddOfflineShopItem(TItemPos bDisplayPos, BYTE bPos, long long lPrice);
		bool SendRemoveOfflineShopItem(BYTE bPos);
		bool SendOpenShopSlot(BYTE bPos);
		bool SendGetBackItems();
		bool SendAddTime();
		bool SendDestroyOfflineShop();
		bool SendTakeOfflineMoney();
		bool SendOfflineShopChangeTitle(const char *title);
		bool SendOfflineShopCheck();
		bool SendOfflineShopButton();
		bool SendOfflineShopRemoveLogs();
		bool SendOfflineOpenWithVID(DWORD vid);
		bool SendOfflineShopChangeDecoration(const char *sign, DWORD vnum, DWORD type);
#endif

		// Exchange
		bool SendExchangeStartPacket(DWORD vid);
		bool SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos);
#ifdef ENABLE_GOLD_LIMIT
		bool SendExchangeElkAddPacket(long long elk);
#else
		bool SendExchangeElkAddPacket(DWORD elk);
#endif
		bool SendExchangeItemDelPacket(BYTE pos);
		bool SendExchangeAcceptPacket();
		bool SendExchangeExitPacket();

		// Quest
		bool SendScriptAnswerPacket(int iAnswer);
		bool SendScriptButtonPacket(unsigned int iIndex);
		bool SendQuestInputStringPacket(const char *c_szString);
#ifdef ENABLE_RENEWAL_OX_EVENT
		bool SendQuestInputStringLongPacket(const char *c_szString);
#endif
		bool SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID);

		// Event
		bool SendOnClickPacket(DWORD vid);

#ifdef ENABLE_EVENT_MANAGER
		bool SendRequestEventQuest(const char *c_szName);
		bool SendRequestEventData(int iMonth);
		bool RecvEventInformation();
		bool RecvEventReload();
#endif

		// Fly
		bool SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget);
		bool SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget);
		bool SendShootPacket(UINT uSkill);

		// Command
		bool ClientCommand(const char *c_szCommand);
		void ServerCommand(char *c_szCommand);

		// Emoticon
		void RegisterEmoticonString(const char *pcEmoticonString);

		// Party
		bool SendPartyInvitePacket(DWORD dwVID);
		bool SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAccept);
		bool SendPartyRemovePacket(DWORD dwPID);
		bool SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag);
		bool SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID);
		bool SendPartyParameterPacket(BYTE byDistributeMode);

		// SafeBox
		bool SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney);

#ifdef ENABLE_EXTENDED_SAFEBOX
		bool SendSafeBoxCheckinPacket(TItemPos InventoryPos, DWORD bySafeBoxPos);
		bool SendSafeBoxCheckoutPacket(DWORD bySafeBoxPos, TItemPos InventoryPos);
#ifdef ENABLE_STACK_LIMIT
		bool SendSafeBoxItemMovePacket(DWORD bySourcePos, DWORD byTargetPos, WORD byCount);
#else
		bool SendSafeBoxItemMovePacket(DWORD bySourcePos, DWORD byTargetPos, BYTE byCount);
#endif
		bool SendMallCheckoutPacket(DWORD byMallPos, TItemPos InventoryPos);
#else
		bool SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos);
		bool SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos);
#ifdef ENABLE_STACK_LIMIT
		bool SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, WORD byCount);
#else
		bool SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, BYTE byCount);
#endif
		bool SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos);
#endif

		// Guild
		bool SendGuildAddMemberPacket(DWORD dwVID);
		bool SendGuildRemoveMemberPacket(DWORD dwPID);
		bool SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char *c_szName);
		bool SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority);
		bool SendGuildOfferPacket(DWORD dwExperience);
		bool SendGuildPostCommentPacket(const char *c_szMessage);
		bool SendGuildDeleteCommentPacket(DWORD dwIndex);
		bool SendGuildRefreshCommentsPacket(DWORD dwHighestIndex);
		bool SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade);
		bool SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID);
		bool SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag);
		bool SendGuildInvitePacket(DWORD dwVID);
		bool SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer);
		bool SendGuildChargeGSPPacket(DWORD dwMoney);
		bool SendGuildDepositMoneyPacket(DWORD dwMoney);
		bool SendGuildWithdrawMoneyPacket(DWORD dwMoney);

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		bool SendChangeLookCheckInPacket(const TItemPos& pos, const BYTE bSlotType);
		bool SendChangeLookCheckOutPacket(const BYTE bSlotType);
		bool SendChangeLookCheckInFreeYangItemPacket(const TItemPos& pos);
		bool SendChangeLookCheckOutFreeYangItemPacket();
		bool SendChangeLookAcceptPacket();
		bool SendChangeLookCancelPacket();

		bool RecvChangeLookItemSetPacket();
		bool RecvChangeLookItemDelPacket();
		bool RecvChangeLookFreeItemSetPacket();
		bool RecvChangeLookFreeItemDelPacket();
#endif

		// Mall
		bool RecvMallOpenPacket();
		bool RecvMallItemSetPacket();
		bool RecvMallItemDelPacket();

		// Lover
		bool RecvLoverInfoPacket();
		bool RecvLovePointUpdatePacket();

		// Dig
		bool RecvDigMotionPacket();

#ifdef ENABLE_GUILD_RANK_SYSTEM
		bool RecvGuildRanking();
#endif

#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
		bool RecvPickupItemPacket();
#endif

		// Fishing
		bool SendFishingPacket(int iRotation);
		bool SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount);

#ifdef ENABLE_RENEWAL_OFFLINESHOP
		// Offline Shop
		bool SendBuildOfflineShopPacket(const char *c_szName, const std::vector<TOfflineShopItemTable>& c_rSellingItemStock, DWORD shopVnum, BYTE shopTitle);
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
		bool SendWhisperDetails(const char *name);
#endif

#ifdef ENABLE_RENEWAL_CUBE
		bool CubeRenewalMakeItem(int index_item, int count_item, int index_item_improve);
		bool CubeRenewalClose();
		bool RecvCubeRenewalPacket();
#endif

#ifdef ENABLE_VIEW_CHEST_DROP
		bool SendChestDropInfo(WORD wInventoryCell);
		bool RecvChestDropInfo();
#endif

		// Private Shop
		bool SendBuildPrivateShopPacket(const char *c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock);

		// Refine
		bool SendRefinePacket(BYTE byPos, BYTE byType);
		bool SendSelectItemPacket(DWORD dwItemPos);

		// CRC Report
		bool __SendCRCReportPacket();

		bool SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos);

		// Handshake
		bool RecvHandshakePacket();
		bool RecvHandshakeOKPacket();

		// ETC
		DWORD GetMainActorVID();
		DWORD GetMainActorRace();
		DWORD GetMainActorEmpire();
		DWORD GetMainActorSkillGroup();
		void SetEmpireID(DWORD dwEmpireID);
		DWORD GetEmpireID();
		void __TEST_SetSkillGroupFake(int iIndex);

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool SendAcceClosePacket();
		bool SendAcceAddPacket(TItemPos tPos, BYTE bPos);
		bool SendAcceRemovePacket(BYTE bPos);
		bool SendAcceRefinePacket();
#endif

#ifdef ENABLE_CLIENT_PERFORMANCE
	public:
		void SendPythonData(PyObject *obj, const char *funcname);
#endif

	public:
#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
		bool SendItemNewAttributePacket(TItemPos source_pos, TItemPos target_pos, BYTE* bValues);
#endif
		void SetOffLinePhase();
		void SetHandShakePhase();
		void SetLoginPhase();
		void SetSelectPhase();
		void SetLoadingPhase();
		void SetGamePhase();
		void ClosePhase();

		// Login Phase
		bool SendLoginPacket(const char *c_szName, const char *c_szPassword);
		bool SendLoginPacketNew(const char *c_szName, const char *c_szPassword);
		bool SendDirectEnterPacket(const char *c_szName, const char *c_szPassword, UINT uChrSlot);

		bool SendEnterGame();

		// Select Phase
		bool SendSelectEmpirePacket(DWORD dwEmpireID);
		bool SendSelectCharacterPacket(BYTE account_Index);
		bool SendChangeNamePacket(BYTE index, const char *name);
		bool SendCreateCharacterPacket(BYTE index, const char *name, BYTE job, BYTE shape, BYTE byStat1, BYTE byStat2, BYTE byStat3, BYTE byStat4);
		bool SendDestroyCharacterPacket(BYTE index, const char *szPrivateCode);

		// Main Game Phase
		bool SendC2CPacket(DWORD dwSize, void * pData);
		bool SendChatPacket(const char *c_szChat, BYTE byType = CHAT_TYPE_TALKING);
		bool SendWhisperPacket(const char *name, const char *c_szChat);
		bool SendMessengerAddByVIDPacket(DWORD vid);
		bool SendMessengerAddByNamePacket(const char *c_szName);
		bool SendMessengerRemovePacket(const char *c_szKey, const char *c_szName);

#ifdef ENABLE_MESSENGER_BLOCK
		bool SendMessengerBlockAddByVIDPacket(DWORD vid);
		bool SendMessengerBlockAddByNamePacket(const char *c_szName);
		bool SendMessengerBlockRemovePacket(const char *c_szKey, const char *c_szName);
		bool SendMessengerBlockRemoveByVIDPacket(DWORD vid);
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
		bool SendMessengerRequestWarpByNamePacket(const char *c_szName);
		bool SendMessengerSummonByNamePacket(const char *c_szName);
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
		bool RecvOfflineShopPacket();
		bool RecvOfflineShopSignPacket();
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		bool SendPrivateShopSearchInfo(TPacketCGShopSearch* p);
		bool RecvShopSearchSet();
#endif

	protected:
		bool OnProcess();
		void OffLinePhase();
		void HandShakePhase();
		void LoginPhase();
		void SelectPhase();
		void LoadingPhase();
		void GamePhase();

		bool __IsNotPing();

		void __DownloadMark();
		void __DownloadSymbol(const std::vector<DWORD> & c_rkVec_dwGuildID);

		void __PlayInventoryItemUseSound(TItemPos uSlotPos);
		void __PlayInventoryItemDropSound(TItemPos uSlotPos);
		//void __PlayShopItemDropSound(UINT uSlotPos);
		void __PlaySafeBoxItemDropSound(UINT uSlotPos);
		void __PlayMallItemDropSound(UINT uSlotPos);

		bool __CanActMainInstance();

		enum REFRESH_WINDOW_TYPE
		{
			RefreshStatus = (1 << 0),
			RefreshAlignmentWindow = (1 << 1),
			RefreshCharacterWindow = (1 << 2),
			RefreshEquipmentWindow = (1 << 3), 
			RefreshInventoryWindow = (1 << 4),
			RefreshExchangeWindow = (1 << 5),
			RefreshSkillWindow = (1 << 6),
			RefreshSafeboxWindow  = (1 << 7),
			RefreshMessengerWindow = (1 << 8),
			RefreshGuildWindowInfoPage = (1 << 9),
			RefreshGuildWindowBoardPage = (1 << 10),
			RefreshGuildWindowMemberPage = (1 << 11), 
			RefreshGuildWindowMemberPageGradeComboBox = (1 << 12),
			RefreshGuildWindowSkillPage = (1 << 13),
			RefreshGuildWindowGradePage = (1 << 14),
			RefreshTargetBoard = (1 << 15),
			RefreshMallWindow = (1 << 16),
		};

		void __RefreshStatus();
		void __RefreshAlignmentWindow();
		void __RefreshCharacterWindow();
		void __RefreshEquipmentWindow();
		void __RefreshInventoryWindow();
		void __RefreshExchangeWindow();
		void __RefreshSkillWindow();
		void __RefreshSafeboxWindow();
		void __RefreshMessengerWindow();
		void __RefreshGuildWindowInfoPage();
		void __RefreshGuildWindowBoardPage();
		void __RefreshGuildWindowMemberPage();
		void __RefreshGuildWindowMemberPageGradeComboBox();
		void __RefreshGuildWindowSkillPage();
		void __RefreshGuildWindowGradePage();
		void __RefreshTargetBoardByVID(DWORD dwVID);
		void __RefreshTargetBoardByName(const char *c_szName);
		void __RefreshTargetBoard();
		void __RefreshMallWindow();
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		void __RefreshShopSearchWindow();
#endif

		bool __SendHack(const char *c_szMsg);

	protected:
		bool RecvObserverAddPacket();
		bool RecvObserverRemovePacket();
		bool RecvObserverMovePacket();

		// Common
		bool RecvErrorPacket(int header);
		bool RecvPingPacket();
		bool RecvDefaultPacket(int header);
		bool RecvPhasePacket();

		// Login Phase
		bool __RecvLoginSuccessPacket3();
		bool __RecvLoginSuccessPacket4();
		bool __RecvLoginFailurePacket();
		bool __RecvEmpirePacket();
		bool __RecvLoginKeyPacket();

		// Select Phase
		bool __RecvPlayerCreateSuccessPacket();
		bool __RecvPlayerCreateFailurePacket();
		bool __RecvPlayerDestroySuccessPacket();
		bool __RecvPlayerDestroyFailurePacket();
		bool __RecvPreserveItemPacket();
		bool __RecvPlayerPoints();
		bool __RecvChangeName();

		// Loading Phase
		bool RecvMainCharacter();
		bool RecvMainCharacter2_EMPIRE();
		bool RecvMainCharacter3_BGM();
		bool RecvMainCharacter4_BGM_VOL();

		void __SetFieldMusicFileName(const char *musicName);
		void __SetFieldMusicFileInfo(const char *musicName, float vol);
		// END_OF_SUPPORT_BGM

		// Main Game Phase
		bool RecvWarpPacket();
		bool RecvPVPPacket();
		bool RecvDuelStartPacket();
        bool RecvGlobalTimePacket();
		bool RecvCharacterAppendPacket();
		bool RecvCharacterAdditionalInfo();
		bool RecvCharacterAppendPacketNew();
		bool RecvCharacterUpdatePacket();
		bool RecvCharacterUpdatePacketNew();
		bool RecvCharacterDeletePacket();
		bool RecvChatPacket();
		bool RecvOwnerShipPacket();
		bool RecvSyncPositionPacket();
		bool RecvWhisperPacket();
		bool RecvPointChange();					// Alarm to python
		bool RecvChangeSpeedPacket();

		bool RecvStunPacket();
		bool RecvDeadPacket();
		bool RecvCharacterMovePacket();

		bool RecvItemSetPacket();					// Alarm to python
		bool RecvItemSetPacket2();					// Alarm to python
		bool RecvItemUsePacket();					// Alarm to python
		bool RecvItemUpdatePacket();				// Alarm to python
		bool RecvItemGroundAddPacket();
		bool RecvItemGroundDelPacket();
		bool RecvItemOwnership();

		bool RecvQuickSlotAddPacket();				// Alarm to python
		bool RecvQuickSlotDelPacket();				// Alarm to python
		bool RecvQuickSlotMovePacket();				// Alarm to python

		bool RecvCharacterPositionPacket();
		bool RecvMotionPacket();

		bool RecvShopPacket();
		bool RecvShopSignPacket();
		bool RecvExchangePacket();

		// Quest
		bool RecvScriptPacket();
		bool RecvQuestInfoPacket();
		bool RecvQuestConfirmPacket();
		bool RecvRequestMakeGuild();
		bool RecvMarkPass();

		// Skill
		bool RecvSkillLevel();
		bool RecvSkillLevelNew();
		bool RecvSkillCoolTimeEnd();

		// Target
		bool RecvTargetPacket();
		bool RecvViewEquipPacket();
		bool RecvDamageInfoPacket();

#ifdef ENABLE_MOB_DROP_INFO
		bool RecvTargetInfoPacket();

	public:
		bool SendTargetInfoLoadPacket(DWORD dwVID);

	protected:
#endif

		// Mount
		bool RecvMountPacket();

		// Fly
		bool RecvCreateFlyPacket();
		bool RecvFlyTargetingPacket();
		bool RecvAddFlyTargetingPacket();

		// Messenger
		bool RecvMessenger();

		// Guild
		bool RecvGuild();

		// Party
		bool RecvPartyInvite();
		bool RecvPartyAdd();
		bool RecvPartyUpdate();
		bool RecvPartyRemove();
		bool RecvPartyLink();
		bool RecvPartyUnlink();
		bool RecvPartyParameter();
#ifdef ENABLE_PARTY_POSITION
		bool RecvPartyPositionInfo();
#endif

		// SafeBox
		bool RecvSafeBoxSetPacket();
		bool RecvSafeBoxDelPacket();
		bool RecvSafeBoxWrongPasswordPacket();
		bool RecvSafeBoxSizePacket();
		bool RecvSafeBoxMoneyChangePacket();

		// Fishing
		bool RecvFishing();

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
		// Whisper Details
		bool RecvWhisperDetails();
#endif

		// Dungeon
		bool RecvDungeon();

		// Time
		bool RecvTimePacket();

		// WalkMode
		bool RecvWalkModePacket();

		// ChangeSkillGroup
		bool RecvChangeSkillGroupPacket();

		// Refine
		bool RecvRefineInformationPacket();
		bool RecvRefineInformationPacketNew();

		bool RecvSpecialEffect();

		bool RecvSpecificEffect();

		bool RecvDragonSoulRefine();

		// MiniMap Info
		bool RecvNPCList();
		bool RecvLandPacket();
		bool RecvTargetCreatePacket();
		bool RecvTargetCreatePacketNew();
		bool RecvTargetUpdatePacket();
		bool RecvTargetDeletePacket();

		// Affect
		bool RecvAffectAddPacket();
		bool RecvAffectRemovePacket();

		// Channel
		bool RecvChannelPacket();

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool RecvAccePacket(bool bReturn = false);
#endif

#ifdef ENABLE_RENEWAL_INGAME_ITEMSHOP
		bool RecvItemShop();
#endif

#ifdef ENABLE_GUILD_TOKEN_AUTH
		bool RecvGuildTokenPacket();
#endif

	protected:
		bool ParseEmoticon(const char *pChatMsg, DWORD * pdwEmoticon);

		void OnConnectFailure();
		void OnScriptEventStart(int iSkin, int iIndex);

		void OnRemoteDisconnect();
		void OnDisconnect();

		void SetGameOnline();
		void SetGameOffline();
		BOOL IsGameOnline();

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	public:
		bool SendExtBattlePassAction(BYTE bAction);
		bool SendExtBattlePassPremiumItem(int slotindex);

	protected:
		bool RecvExtBattlePassOpenPacket();
		bool RecvExtBattlePassGeneralInfoPacket();
		bool RecvExtBattlePassMissionInfoPacket();
		bool RecvExtBattlePassMissionUpdatePacket();
		bool RecvExtBattlePassRankingPacket();
#endif

#ifdef ENABLE_HUNTING_SYSTEM
	public:
		bool SendHuntingAction(BYTE bAction, DWORD dValue);

	protected:
		bool RecvHuntingOpenWindowMain();
		bool RecvHuntingOpenWindowSelect();
		bool RecvHuntingOpenWindowReward();
		bool RecvHuntingUpdate();
		bool RecvHuntingRandomItems();
#endif

	protected:
		bool CheckPacket(TPacketHeader * pRetHeader);

		void __InitializeGamePhase();
		void __InitializeMarkAuth();
		void __GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY);
		void __LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY);

		bool __IsPlayerAttacking();
		bool __IsEquipItemInSlot(TItemPos Cell);

		void __ShowMapName(LONG lLocalX, LONG lLocalY);

		void __LeaveOfflinePhase() {}
		void __LeaveHandshakePhase() {}
		void __LeaveLoginPhase() {}
		void __LeaveSelectPhase() {}
		void __LeaveLoadingPhase() {}
		void __LeaveGamePhase();

		void __ClearNetworkActorManager();

		void __ClearSelectCharacterData();

		void __ConvertEmpireText(DWORD dwEmpireID, char *szText);

		void __RecvCharacterAppendPacket(SNetworkActorData * pkNetActorData);
		void __RecvCharacterUpdatePacket(SNetworkUpdateActorData * pkNetUpdateActorData);

		void __FilterInsult(char *szLine, UINT uLineLen);

		void __SetGuildID(DWORD id);

#ifdef ENABLE_GUILD_TOKEN_AUTH
		void __SetGuildToken(uint64_t token);
#endif

	protected:
		TPacketGCHandshake m_HandshakeData;
		DWORD m_dwChangingPhaseTime;
		DWORD m_dwBindupRetryCount;
		DWORD m_dwMainActorVID;
		DWORD m_dwMainActorRace;
		DWORD m_dwMainActorEmpire;
		DWORD m_dwMainActorSkillGroup;
		BOOL m_isGameOnline;
		BOOL m_isStartGame;

		DWORD m_dwGuildID;
#ifdef ENABLE_GUILD_TOKEN_AUTH
		uint64_t m_dwGuildToken{};
#endif
		DWORD m_dwGuildMarkPass;
		DWORD m_dwEmpireID;

		struct SServerTimeSync
		{
			DWORD m_dwChangeServerTime;
			DWORD m_dwChangeClientTime;
		} m_kServerTimeSync;

		void __ServerTimeSync_Initialize();

		DWORD m_dwLastGamePingTime;

		std::string	m_stID;
		std::string	m_stPassword;
		std::string	m_strLastCommand;
		std::string	m_strPhase;
		DWORD m_dwLoginKey;
		BOOL m_isWaitLoginKey;

		std::string m_stMarkIP;

		CFuncObject<CPythonNetworkStream> m_phaseProcessFunc;
		CFuncObject<CPythonNetworkStream> m_phaseLeaveFunc;

		PyObject *m_poHandler;
		PyObject *m_apoPhaseWnd[PHASE_WINDOW_NUM];
		PyObject *m_poSerCommandParserWnd;

		TSimplePlayerInformation m_akSimplePlayerInfo[PLAYER_PER_ACCOUNT4];
		DWORD m_adwGuildID[PLAYER_PER_ACCOUNT4];
		std::string m_astrGuildName[PLAYER_PER_ACCOUNT4];
		bool m_bSimplePlayerInfo;

		CRef<CNetworkActorManager> m_rokNetActorMgr;

		bool m_isRefreshStatus;
		bool m_isRefreshCharacterWnd;
		bool m_isRefreshEquipmentWnd;
		bool m_isRefreshInventoryWnd;
		bool m_isRefreshExchangeWnd;
		bool m_isRefreshSkillWnd;
		bool m_isRefreshSafeboxWnd;
		bool m_isRefreshMallWnd;
		bool m_isRefreshMessengerWnd;
		bool m_isRefreshGuildWndInfoPage;
		bool m_isRefreshGuildWndBoardPage;
		bool m_isRefreshGuildWndMemberPage;
		bool m_isRefreshGuildWndMemberPageGradeComboBox;
		bool m_isRefreshGuildWndSkillPage;
		bool m_isRefreshGuildWndGradePage;
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		bool m_isRefreshShopSearchWnd;
#endif

		// Emoticon
		std::vector<std::string> m_EmoticonStringVector;

		struct STextConvertTable 
		{
			char acUpper[26];
			char acLower[26];
			BYTE aacHan[5000][2];
		} m_aTextConvTable[3];



		struct SMarkAuth
		{
			CNetworkAddress m_kNetAddr;
			DWORD m_dwHandle;
			DWORD m_dwRandomKey;
		} m_kMarkAuth;



		DWORD m_dwSelectedCharacterIndex;

		CInsultChecker m_kInsultChecker;

		bool m_isEnableChatInsultFilter;
		bool m_bComboSkillFlag;

		std::deque<std::string> m_kQue_stHack;

	private:
		struct SDirectEnterMode
		{
			bool m_isSet;
#ifdef ENABLE_CLIENT_PERFORMANCE
			bool m_isWarp;
#endif
			DWORD m_dwChrSlotIndex;
		} m_kDirectEnterMode;

		void __DirectEnterMode_Initialize();
#ifdef ENABLE_CLIENT_PERFORMANCE
		void __DirectEnterMode_Set(UINT uChrSlotIndex, bool bisWarp = false);
		bool __DirectEnterMode_IsWarp();
#else
		void __DirectEnterMode_Set(UINT uChrSlotIndex);
#endif
		bool __DirectEnterMode_IsSet();

	public:
		DWORD EXPORT_GetBettingGuildWarValue(const char *c_szValueName);

	private:
		struct SBettingGuildWar
		{
			DWORD m_dwBettingMoney;
			DWORD m_dwObserverCount;
		} m_kBettingGuildWar;

		CInstanceBase * m_pInstTarget;

		void __BettingGuildWar_Initialize();
		void __BettingGuildWar_SetObserverCount(UINT uObserverCount);
		void __BettingGuildWar_SetBettingMoney(UINT uBettingMoney);

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	public:
		bool SendChangeLanguagePacket(BYTE bLanguage);
		bool SendChangeLanguage(const char *lang);

		void SetLanguage(BYTE bLanguage);
		int GetLanguage() const { return m_bLanguage; }

	protected:
		bool RecvRequestChangeLanguage();

		BYTE m_bLanguage;
		BOOL m_bLanguageSet;
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
	public:
		bool RecvSwitchbotPacket();

		bool SendSwitchbotStartPacket(BYTE slot, std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> alternatives);
		bool SendSwitchbotStopPacket(BYTE slot);
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
	public:
		bool SendBiologManagerAction(BYTE bSubHeader);

	protected:
		bool RecvBiologManager();
#endif

#ifdef ENABLE_CLIENT_LOCALE_STRING
	public:
		bool RecvLocaleChatPacket();
		bool RecvLocaleWhishperPacket();

		bool LoadLocaleQuizVnum(const char *c_szLocaleQuizFileName);
		const char *GetLocaleQuizVnum(DWORD id);

		bool LoadLocaleChatVnum(const char *c_szLocaleChatFileName);
		const char *GetLocaleChatVnum(DWORD id);

		bool LoadLocaleStringVnum(const char *c_szLocaleStringFileName);
		std::string GetLocaleStringVnum(DWORD id);

	protected:
		std::map<DWORD, std::string> m_Quiz, m_MonsterChat, m_String;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	protected:
		bool RecvAuraPacket();

	public:
		bool SendAuraRefineCheckIn(TItemPos InventoryCell, TItemPos AuraCell, BYTE byAuraRefineWindowType);
		bool SendAuraRefineCheckOut(TItemPos AuraCell, BYTE byAuraRefineWindowType);
		bool SendAuraRefineAccept(BYTE byAuraRefineWindowType);
		bool SendAuraRefineCancel();
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	public:
		bool SendPetHatchingPacket(const char *c_szName, TItemPos eggPos);
		bool SendPetWindowType(BYTE bType);
		bool SendPetHatchingWindowPacket(bool bState);
		bool SendPetNameChangeWindowPacket(bool bState);
		bool SendPetNameChangePacket(const char *c_szName, TItemPos changeNamePos, TItemPos upBringingPos);
		bool SendPetFeedPacket(int iFeedIndex, const std::vector<std::pair<WORD, WORD>>& itemVec);
		bool SendPetAttrDetermine(TItemPos determinePos);
		bool SendPetAttrChange(TItemPos upbringingPos, TItemPos attrChangePos);
		bool SendPetRevive(TItemPos upbringingPos, const std::vector<std::pair<WORD, WORD>>& itemVec);
		bool SendPetLearnSkill(BYTE bSlot, TItemPos learnSkillPos);
		bool SendPetSkillUpgradeRequest(BYTE bSlot);
		bool SendPetDeleteSkill(BYTE bSlot, TItemPos deleteSkillPos);
		bool SendPetDeleteAllSkill(TItemPos deleteAllSkillPos);

	protected:
		bool RecvPet();
		bool RecvPetSet();
		bool RecvPetSetExchange();
		bool RecvPetDelete();
		bool RecvPetSummon();
		bool RecvPetPointChange();
		bool RecvPetNameChangeResult();
		bool RecvPetSkillUpdate();
		bool RecvPetSkillCooltime();
		bool RecvPetDetermineResult();
		bool RecvPetAttrChangeResult();
#endif

#ifdef ENABLE_RIDING_EXTENDED
public:
	bool MountUpGrade(const uint8_t iSubHeader);
	bool RecvMountUpGrade();
	bool RecvMountUpGradeChat();
#endif
};
