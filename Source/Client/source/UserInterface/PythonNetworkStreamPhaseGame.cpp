#include "StdAfx.h"
#include "PythonSystem.h"
#include "PythonNetworkStream.h"
#include "Packet.h"

#include "PythonGuild.h"
#include "PythonCharacterManager.h"
#include "PythonPlayer.h"
#include "PythonBackground.h"
#include "PythonMiniMap.h"
#include "PythonTextTail.h"
#include "PythonItem.h"
#include "PythonChat.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonQuest.h"
#include "PythonEventManager.h"
#include "PythonMessenger.h"
#include "PythonApplication.h"
#include "PythonStatusEffectManager.h"
#include "PythonYinAndYangAttrManager.h"
#ifdef ENABLE_SHOP_REPURCHASE
#include "PythonRepurchaseShop.h"
#endif
#include "../EterPack/EterPackManager.h"
#include "../gamelib/ItemManager.h"
#include "AbstractApplication.h"
#include "AbstractCharacterManager.h"
#include "InstanceBase.h"
#include "ProcessCRC.h"
#ifdef ENABLE_DUNGEON_INFO
	#include "PythonDungeonInfo.h"
#endif

#ifdef ENABLE_RENEWAL_CUBE
	#include "PythonCubeRenewal.h"
#endif

#ifdef ENABLE_DISCORD_RPC
	#include "discord_rpc.h"
	#include "Discord.h"

	#ifdef _DEBUG
		#pragma comment(lib, "Discord_d.lib")
	#else
		#pragma comment(lib, "Discord_r.lib")
	#endif
#endif

#ifdef ENABLE_RIDING_EXTENDED
	#include "PythonMountUpGrade.h"
#endif

#ifdef ENABLE_EVENT_SYSTEM
	#include "PythonGameEvents.h"
#endif

BOOL gs_bEmpireLanuageEnable = TRUE;

void CPythonNetworkStream::__RefreshAlignmentWindow()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshAlignment", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshTargetBoardByVID(DWORD dwVID)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
}

void CPythonNetworkStream::__RefreshTargetBoardByName(const char *c_szName)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByName", Py_BuildValue("(s)", c_szName));
}

void CPythonNetworkStream::__RefreshTargetBoard()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoard", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshGuildWindowGradePage()
{
	m_isRefreshGuildWndGradePage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowSkillPage()
{
	m_isRefreshGuildWndSkillPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPageGradeComboBox()
{
	m_isRefreshGuildWndMemberPageGradeComboBox=true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPage()
{
	m_isRefreshGuildWndMemberPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowBoardPage()
{
	m_isRefreshGuildWndBoardPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowInfoPage()
{
	m_isRefreshGuildWndInfoPage=true;
}

void CPythonNetworkStream::__RefreshMessengerWindow()
{
	m_isRefreshMessengerWnd=true;
}

void CPythonNetworkStream::__RefreshSafeboxWindow()
{
	m_isRefreshSafeboxWnd=true;
}

void CPythonNetworkStream::__RefreshMallWindow()
{
	m_isRefreshMallWnd=true;
}

void CPythonNetworkStream::__RefreshSkillWindow()
{
	m_isRefreshSkillWnd=true;
}

void CPythonNetworkStream::__RefreshExchangeWindow()
{
	m_isRefreshExchangeWnd=true;
}

void CPythonNetworkStream::__RefreshStatus()
{
	m_isRefreshStatus=true;
}

void CPythonNetworkStream::__RefreshCharacterWindow()
{
	m_isRefreshCharacterWnd=true;
}

void CPythonNetworkStream::__RefreshInventoryWindow()
{
	m_isRefreshInventoryWnd=true;
}

void CPythonNetworkStream::__RefreshEquipmentWindow()
{
	m_isRefreshEquipmentWnd=true;
}

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
void CPythonNetworkStream::__RefreshShopSearchWindow()
{
	m_isRefreshShopSearchWnd = true;
}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CPythonNetworkStream::__RefreshAcceWindow()
{
	m_isRefreshAcceWnd = true;
}
#endif

#ifdef ENABLE_GUILDRENEWAL_SYSTEM
void CPythonNetworkStream::__RefreshGuildBank()
{
	m_isRefreshGuildBank = true;
}

void CPythonNetworkStream::__RefreshGuildBankInfo()
{
	m_isRefreshGuildBankInfo = true;
}
#endif

void CPythonNetworkStream::__RefreshGuildBaseInfoPage()
{
	m_isRefreshGuildBaseInfoPage = true;
}

void CPythonNetworkStream::__RefreshGuildBaseInfoPageBankGold()
{
	m_isRefreshGuildBaseInfoPageBankGold = true;
}

#ifdef ENABLE_GUILDRENEWAL_SYSTEM
void CPythonNetworkStream::__RefreshGuildWarInfoPage()
{
	m_isRefreshGuildWarInfoPage = true;
}
#endif

void CPythonNetworkStream::__SetGuildID(DWORD id)
{
	if (m_dwGuildID != id)
	{
		m_dwGuildID = id;
		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

		for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
			if (!strncmp(m_akSimplePlayerInfo[i].szName, rkPlayer.GetName(), CHARACTER_NAME_MAX_LEN))
			{
				m_adwGuildID[i] = id;

				std::string  guildName;
				if (CPythonGuild::Instance().GetGuildName(id, &guildName))
				{
					m_astrGuildName[i] = guildName;
				}
				else
				{
					m_astrGuildName[i] = "";
				}
			}
	}
}

struct PERF_PacketInfo
{
	DWORD dwCount;
	DWORD dwTime;

	PERF_PacketInfo()
	{
		dwCount=0;
		dwTime=0;
	}
};

// Game Phase ---------------------------------------------------------------------------
void CPythonNetworkStream::GamePhase()
{
	if (!m_kQue_stHack.empty())
	{
		__SendHack(m_kQue_stHack.front().c_str());
		m_kQue_stHack.pop_front();
	}

	TPacketHeader header = 0;
	bool ret = true;

	const DWORD MAX_RECV_COUNT = 4;
	const DWORD SAFE_RECV_BUFSIZE = 8192;
	DWORD dwRecvCount = 0;

	while (ret)
	{
		if(dwRecvCount++ >= MAX_RECV_COUNT-1 && GetRecvBufferSize() < SAFE_RECV_BUFSIZE
			&& m_strPhase == "Game")
			break;

		if (!CheckPacket(&header))
			break;

		switch (header)
		{
			case HEADER_GC_OBSERVER_ADD:
				ret = RecvObserverAddPacket();
				break;

			case HEADER_GC_OBSERVER_REMOVE:
				ret = RecvObserverRemovePacket();
				break;

			case HEADER_GC_OBSERVER_MOVE:
				ret = RecvObserverMovePacket();
				break;

			case HEADER_GC_WARP:
				ret = RecvWarpPacket();
				break;

			case HEADER_GC_PHASE:
				ret = RecvPhasePacket();
				return;
				break;

			case HEADER_GC_PVP:
				ret = RecvPVPPacket();
				break;

			case HEADER_GC_DUEL_START:
				ret = RecvDuelStartPacket();
				break;

			case HEADER_GC_CHARACTER_ADD:
 				ret = RecvCharacterAppendPacket();
				break;

			case HEADER_GC_CHAR_ADDITIONAL_INFO:
				ret = RecvCharacterAdditionalInfo();
				break;

			case HEADER_GC_CHARACTER_ADD2:
				ret = RecvCharacterAppendPacketNew();
				break;

			case HEADER_GC_CHARACTER_UPDATE:
			case HEADER_GC_PARTS:
				ret = RecvCharacterUpdatePacket();
				break;

			case HEADER_GC_CHARACTER_UPDATE2:
				ret = RecvCharacterUpdatePacketNew();
				break;

			case HEADER_GC_CHARACTER_DEL:
				ret = RecvCharacterDeletePacket();
				break;

			case HEADER_GC_CHAT:
				ret = RecvChatPacket();
				break;

			case HEADER_GC_SYNC_POSITION:
				ret = RecvSyncPositionPacket();
				break;

			case HEADER_GC_OWNERSHIP:
				ret = RecvOwnerShipPacket();
				break;

			case HEADER_GC_WHISPER:
				ret = RecvWhisperPacket();
				break;

			case HEADER_GC_CHARACTER_MOVE:
				ret = RecvCharacterMovePacket();
				break;

			// Position
			case HEADER_GC_CHARACTER_POSITION:
				ret = RecvCharacterPositionPacket();
				break;

			// Battle Packet
			case HEADER_GC_STUN:
				ret = RecvStunPacket();
				break;

			case HEADER_GC_DEAD:
				ret = RecvDeadPacket();
				break;

			case HEADER_GC_PLAYER_POINT_CHANGE:
			case 170: /* point change (alternate header from server) */
				ret = RecvPointChange();
				break;

			case 144: /* unknown from server; skip 1 byte */
			case 195: /* unknown from server; skip 1 byte */
			case 33: /* unknown from server; skip 1 byte */
			{
				TPacketGCBlank skip;
				ret = Recv(sizeof(skip), &skip);
			}
				break;

			// item packet.
			case HEADER_GC_ITEM_SET:
				ret = RecvItemSetPacket();
				break;

			case HEADER_GC_ITEM_SET2:
				ret = RecvItemSetPacket2();
				break;

			case HEADER_GC_ITEM_USE:
				ret = RecvItemUsePacket();
				break;

			case HEADER_GC_ITEM_UPDATE:
				ret = RecvItemUpdatePacket();
				break;

			case HEADER_GC_ITEM_GROUND_ADD:
				ret = RecvItemGroundAddPacket();
				break;

			case HEADER_GC_ITEM_GROUND_DEL:
				ret = RecvItemGroundDelPacket();
				break;

			case HEADER_GC_ITEM_OWNERSHIP:
				ret = RecvItemOwnership();
				break;

			case HEADER_GC_QUICKSLOT_ADD:
				ret = RecvQuickSlotAddPacket();
				break;

			case HEADER_GC_QUICKSLOT_DEL:
				ret = RecvQuickSlotDelPacket();
				break;

			case HEADER_GC_QUICKSLOT_SWAP:
				ret = RecvQuickSlotMovePacket();
				break;

			case HEADER_GC_MOTION:
				ret = RecvMotionPacket();
				break;

			case HEADER_GC_SHOP:
				ret = RecvShopPacket();
				break;

			case HEADER_GC_SHOP_SIGN:
				ret = RecvShopSignPacket();
				break;

			case HEADER_GC_EXCHANGE:
				ret = RecvExchangePacket();
				break;

#ifdef ENABLE_GUILD_RANK_SYSTEM
			case HEADER_GC_GUILD_RANKING:
				ret = RecvGuildRanking();
				break;
#endif
			case HEADER_GC_QUEST_INFO:
				ret = RecvQuestInfoPacket();
				break;

#ifdef ENABLE_PARTY_POSITION
			case HEADER_GC_PARTY_POSITION_INFO:
				ret = RecvPartyPositionInfo();
				break;
#endif
			case HEADER_GC_REQUEST_MAKE_GUILD:
				ret = RecvRequestMakeGuild();
				break;

			case HEADER_GC_PING:
				ret = RecvPingPacket();
				break;

			case HEADER_GC_SCRIPT:
				ret = RecvScriptPacket();
				break;

			case HEADER_GC_QUEST_CONFIRM:
				ret = RecvQuestConfirmPacket();
				break;

			case HEADER_GC_TARGET:
				ret = RecvTargetPacket();
				break;

			case HEADER_GC_DAMAGE_INFO:
				ret = RecvDamageInfoPacket();
				break;

			case HEADER_GC_MOUNT:
				ret = RecvMountPacket();
				break;

			case HEADER_GC_CHANGE_SPEED:
				ret = RecvChangeSpeedPacket();
				break;

			case HEADER_GC_PLAYER_POINTS:
				ret = __RecvPlayerPoints();
				break;

			case HEADER_GC_CREATE_FLY:
				ret = RecvCreateFlyPacket();
				break;

			case HEADER_GC_FLY_TARGETING:
				ret = RecvFlyTargetingPacket();
				break;

			case HEADER_GC_ADD_FLY_TARGETING:
				ret = RecvAddFlyTargetingPacket();
				break;

			case HEADER_GC_SKILL_LEVEL:
				ret = RecvSkillLevel();
				break;

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
			case HEADER_GC_SOUL_ROULETTE:
				ret = RecvSoulRoulette();
				break;
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
			case HEADER_GC_MINI_GAME_CATCH_KING:
				ret = RecvMiniGameCatchKingPacket();
				break;
#endif

#ifdef ENABLE_STONE_EVENT
			case HEADER_GC_STONE_EVENT:
				ret = RecvStoneEvent();
				break;
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
			case HEADER_GC_HIT_COUNT_INFO:
				ret = RecvHitCountInfo();
				break;
				
			case HEADER_GC_ATTENDANCE_EVENT:
				ret = RecvAttendanceEvent();
				break;
				
			case HEADER_GC_ATTENDANCE_EVENT_INFO:
				ret = RecvAttendanceEventInfo();
				break;
#endif

			case HEADER_GC_SKILL_LEVEL_NEW:
				ret = RecvSkillLevelNew();
				break;

			case HEADER_GC_MESSENGER:
				ret = RecvMessenger();
				break;

			case HEADER_GC_GUILD:
				ret = RecvGuild();
				break;

			case HEADER_GC_PARTY_INVITE:
				ret = RecvPartyInvite();
				break;

			case HEADER_GC_PARTY_ADD:
				ret = RecvPartyAdd();
				break;

			case HEADER_GC_PARTY_UPDATE:
				ret = RecvPartyUpdate();
				break;

			case HEADER_GC_PARTY_REMOVE:
				ret = RecvPartyRemove();
				break;

			case HEADER_GC_PARTY_LINK:
				ret = RecvPartyLink();
				break;

			case HEADER_GC_PARTY_UNLINK:
				ret = RecvPartyUnlink();
				break;

			case HEADER_GC_PARTY_PARAMETER:
				ret = RecvPartyParameter();
				break;

			case HEADER_GC_SAFEBOX_SET:
				ret = RecvSafeBoxSetPacket();
				break;

			case HEADER_GC_SAFEBOX_DEL:
				ret = RecvSafeBoxDelPacket();
				break;

			case HEADER_GC_SAFEBOX_WRONG_PASSWORD:
				ret = RecvSafeBoxWrongPasswordPacket();
				break;

			case HEADER_GC_SAFEBOX_SIZE:
				ret = RecvSafeBoxSizePacket();
				break;

			case HEADER_GC_SAFEBOX_MONEY_CHANGE:
				ret = RecvSafeBoxMoneyChangePacket();
				break;

#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
			case HEADER_GC_SAFEBOX_PASSWORD_SUCCESS:
				ret = RecvSafeBoxPasswordSuccessPacket();
				break;
#endif

			case HEADER_GC_FISHING:
				ret = RecvFishing();
				break;

			case HEADER_GC_DUNGEON:
				ret = RecvDungeon();
				break;

			case HEADER_GC_TIME:
				ret = RecvTimePacket();
				break;

			case HEADER_GC_WALK_MODE:
				ret = RecvWalkModePacket();
				break;

			case HEADER_GC_CHANGE_SKILL_GROUP:
				ret = RecvChangeSkillGroupPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION:
				ret = RecvRefineInformationPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION_NEW:
				ret = RecvRefineInformationPacketNew();
				break;

			case HEADER_GC_SEPCIAL_EFFECT:
				ret = RecvSpecialEffect();
				break;

			case HEADER_GC_NPC_POSITION:
				ret = RecvNPCList();
				break;

			case HEADER_GC_CHANNEL:
				ret = RecvChannelPacket();
				break;

			case HEADER_GC_VIEW_EQUIP:
				ret = RecvViewEquipPacket();
				break;

			case HEADER_GC_LAND_LIST:
				ret = RecvLandPacket();
				break;

			case HEADER_GC_TARGET_CREATE_NEW:
				ret = RecvTargetCreatePacketNew();
				break;

			case HEADER_GC_TARGET_UPDATE:
				ret = RecvTargetUpdatePacket();
				break;

			case HEADER_GC_TARGET_DELETE:
				ret = RecvTargetDeletePacket();
				break;

			case HEADER_GC_AFFECT_ADD:
				ret = RecvAffectAddPacket();
				break;

			case HEADER_GC_AFFECT_REMOVE:
				ret = RecvAffectRemovePacket();
				break;

			case HEADER_GC_MALL_OPEN:
				ret = RecvMallOpenPacket();
				break;

			case HEADER_GC_MALL_SET:
				ret = RecvMallItemSetPacket();
				break;

			case HEADER_GC_MALL_DEL:
				ret = RecvMallItemDelPacket();
				break;

#ifdef ENABLE_CONQUEROR_LEVEL
			case HEADER_GC_SUNGMA_ATTR_UPDATE:
				ret = RecvSungmaAttrUpdatePacket();
				break;
#endif

#ifdef ENABLE_STATUS_EFFECT_SHOWER
			case HEADER_GC_STATUS_EFFECT:
				ret = RecvStatusEffectPacket();
				break;
#endif

#ifdef ENABLE_YIN_AND_YANG_ATTR
			case HEADER_GC_YIN_YANG_ATTR:
				ret = RecvYinAndYangAttrPacket();
				break;
#endif

			case HEADER_GC_LOVER_INFO:
				ret = RecvLoverInfoPacket();
				break;

			case HEADER_GC_LOVE_POINT_UPDATE:
				ret = RecvLovePointUpdatePacket();
				break;

			case HEADER_GC_DIG_MOTION:
				ret = RecvDigMotionPacket();
				break;

			case HEADER_GC_HANDSHAKE:
				RecvHandshakePacket();
				return;
				break;

			case HEADER_GC_HANDSHAKE_OK:
				RecvHandshakeOKPacket();
				return;
				break;

#ifdef ENABLE_ITEMSHOP
			case HEADER_GC_ITEMSHOP:
			{
				ret = RecvItemShop();
				break;
			}
#endif

			case HEADER_GC_SPECIFIC_EFFECT:
				ret = RecvSpecificEffect();
				break;

#ifdef ENABLE_BUFFI_SYSTEM
			case HEADER_GC_BUFFI_USE_SKILL:
				ret = RecvBuffiUseSkill();
				break;
#endif

			case HEADER_GC_DRAGON_SOUL_REFINE:
				ret = RecvDragonSoulRefine();
				break;

#ifdef ENABLE_FISH_EVENT_SYSTEM
			case HEADER_GC_FISH_EVENT_INFO:
				ret = RecvFishEventInfo();
				break;
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			case HEADER_GC_REQUEST_CHANGE_LANGUAGE:
				ret = RecvRequestChangeLanguage();
				break;
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
			case HEADER_GC_WHISPER_DETAILS:
				ret = RecvWhisperDetails();
				break;
#endif

#ifdef ENABLE_RESP_SYSTEM
			case HEADER_GC_RESP:
				ret = RecvRespPacket();
				break;
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
			case HEADER_GC_SWITCHBOT:
				ret = RecvSwitchbotPacket();
				break;
#endif

#ifdef ENABLE_RENEWAL_CUBE
			case HEADER_GC_CUBE_RENEWAL:
				ret = RecvCubeRenewalPacket();
				break;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			case HEADER_GC_ACCE:
				ret = RecvAccePacket();
				break;
#endif

#ifdef ENABLE_MOB_DROP_INFO
			case HEADER_GC_TARGET_INFO:
				ret = RecvTargetInfoPacket();
				break;
#endif

#ifdef ENABLE_COLLECT_WINDOW
			case HEADER_GC_COLLECT:
				ret = RecvCollectPacket();
				break;
#endif

#ifdef ENABLE_VIEW_CHEST_DROP
			case HEADER_GC_CHEST_DROP_INFO:
				ret = RecvChestDropInfo();
				break;
#endif

#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
			case HEADER_GC_PICKUP_ITEM_SC:
				ret = RecvPickupItemPacket();
				break;
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
			case HEADER_GC_OFFLINE_SHOP:
				ret = RecvOfflineShopPacket();
				break;

			case HEADER_GC_OFFLINE_SHOP_SIGN:
				ret = RecvOfflineShopSignPacket();
				break;
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
			case HEADER_GC_SHOPSEARCH_SET:
				ret = RecvShopSearchSet();
				break;
#endif

#ifdef ENABLE_CLIENT_LOCALE_STRING
			case HEADER_GC_LOCALE_CHAT:
				ret = RecvLocaleChatPacket();
				break;

			case HEADER_GC_LOCALE_WHISPER:
				ret = RecvLocaleWhishperPacket();
				break;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
			case HEADER_GC_AURA:
				ret = RecvAuraPacket();
				break;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
			case HEADER_GC_PET:
				ret = RecvPet();
				break;

			case HEADER_GC_PET_SET:
				ret = RecvPetSet();
				break;

			case HEADER_GC_PET_SET_EXCHANGE:
				ret = RecvPetSetExchange();
				break;

			case HEADER_GC_PET_DEL:
				ret = RecvPetDelete();
				break;

			case HEADER_GC_PET_SUMMON:
				ret = RecvPetSummon();
				break;

			case HEADER_GC_PET_POINT_CHANGE:
				ret = RecvPetPointChange();
				break;

			case HEADER_GC_PET_NAME_CHANGE_RESULT:
				ret = RecvPetNameChangeResult();
				break;

			case HEADER_GC_PET_SKILL_UPDATE:
				ret = RecvPetSkillUpdate();
				break;

			case HEADER_GC_PET_SKILL_COOLTIME:
				ret = RecvPetSkillCooltime();
				break;

			case HEADER_GC_PET_DETERMINE_RESULT:
				ret = RecvPetDetermineResult();
				break;

			case HEADER_GC_PET_ATTR_CHANGE_RESULT:
				ret = RecvPetAttrChangeResult();
				break;
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			case HEADER_GC_CHANGE_LOOK_SET:
				ret = RecvChangeLookItemSetPacket();
				break;

			case HEADER_GC_CHANGE_LOOK_DEL:
				ret = RecvChangeLookItemDelPacket();
				break;

			case HEADER_GC_CHANGE_LOOK_FREE_SET:
				ret = RecvChangeLookFreeItemSetPacket();
				break;

			case HEADER_GC_CHANGE_LOOK_FREE_DEL:
				ret = RecvChangeLookFreeItemDelPacket();
				break;
#endif

#ifdef ENABLE_HUNTING_SYSTEM
			case HEADER_GC_HUNTING_OPEN_MAIN:
				ret = RecvHuntingOpenWindowMain();
				break;

			case HEADER_GC_HUNTING_OPEN_SELECT:
				ret = RecvHuntingOpenWindowSelect();
				break;

			case HEADER_GC_HUNTING_OPEN_REWARD:
				ret = RecvHuntingOpenWindowReward();
				break;

			case HEADER_GC_HUNTING_UPDATE:
				ret = RecvHuntingUpdate();
				break;

			case HEADER_GC_HUNTING_RECIVE_RAND_ITEMS:
				ret = RecvHuntingRandomItems();
				break;
#endif

#ifdef ENABLE_RIDING_EXTENDED
			case HEADER_GC_MOUNT_UP_GRADE:
				ret = RecvMountUpGrade();
				break;

			case HEADER_GC_MOUNT_UP_GRADE_CHAT:
				ret = RecvMountUpGradeChat();
				break;
#endif

#ifdef ENABLE_DUNGEON_INFO
			case HEADER_GC_DUNGEON_INFO_SYSTEM:
				ret = RecvDungeonInfoPacket();
				break;
#endif

#ifdef ENABLE_EVENT_SYSTEM
			case HEADER_GC_EVENT_INFO:
				ret = RecvEventInfo();
				break;
#endif

#ifdef ENABLE_RANKING
			case HEADER_GC_RANKING_SEND:
				ret = RecvRankingTable();
				break;
#endif

#ifdef ENABLE_KILL_STATISTICS
			case HEADER_GC_KILL_STATISTICS:
				ret = RecvKillStatistics();
				break;
#endif

			default:
				ret = RecvDefaultPacket(header);
				break;
		}
	}

	if (!ret)
		RecvErrorPacket(header);

	static DWORD s_nextRefreshTime = ELTimer_GetMSec();

	DWORD curTime = ELTimer_GetMSec();
	if (s_nextRefreshTime > curTime)
		return;

	if (m_isRefreshCharacterWnd)
	{
		m_isRefreshCharacterWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshEquipmentWnd)
	{
		m_isRefreshEquipmentWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEquipment", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshInventoryWnd)
	{
		m_isRefreshInventoryWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshInventory", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshExchangeWnd)
	{
		m_isRefreshExchangeWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshExchange", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSkillWnd)
	{
		m_isRefreshSkillWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSkill", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSafeboxWnd)
	{
		m_isRefreshSafeboxWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafebox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMallWnd)
	{
		m_isRefreshMallWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMall", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshStatus)
	{
		m_isRefreshStatus=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMessengerWnd)
	{
		m_isRefreshMessengerWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMessenger", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndInfoPage)
	{
		m_isRefreshGuildWndInfoPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndBoardPage)
	{
		m_isRefreshGuildWndBoardPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBoardPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPage)
	{
		m_isRefreshGuildWndMemberPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPageGradeComboBox)
	{
		m_isRefreshGuildWndMemberPageGradeComboBox=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPageGradeComboBox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndSkillPage)
	{
		m_isRefreshGuildWndSkillPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildSkillPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndGradePage)
	{
		m_isRefreshGuildWndGradePage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGradePage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (m_isRefreshAcceWnd)
	{
		m_isRefreshAcceWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshAcceWindow", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
#endif

#ifdef ENABLE_GUILDRENEWAL_SYSTEM
	if (m_isRefreshGuildBank)
	{
		m_isRefreshGuildBank = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBank", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildBankInfo)
	{
		m_isRefreshGuildBankInfo = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBankInfo", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
#endif

	if (m_isRefreshGuildBaseInfoPage)
	{
		m_isRefreshGuildBaseInfoPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBaseInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildBaseInfoPageBankGold)
	{
		m_isRefreshGuildBaseInfoPageBankGold = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBaseInfoPageBankGold", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

#ifdef ENABLE_GUILDRENEWAL_SYSTEM
	if (m_isRefreshGuildWarInfoPage)
	{
		m_isRefreshGuildWarInfoPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildWarInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	if (m_isRefreshShopSearchWnd)
	{
		m_isRefreshShopSearchWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshShopSearch", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 500;
	}
#endif
}

void CPythonNetworkStream::__InitializeGamePhase()
{
	__ServerTimeSync_Initialize();

	m_isRefreshStatus=false;
	m_isRefreshCharacterWnd=false;
	m_isRefreshEquipmentWnd=false;
	m_isRefreshInventoryWnd=false;
	m_isRefreshExchangeWnd=false;
	m_isRefreshSkillWnd=false;
	m_isRefreshSafeboxWnd=false;
	m_isRefreshMallWnd=false;
	m_isRefreshMessengerWnd=false;
	m_isRefreshGuildWndInfoPage=false;
	m_isRefreshGuildWndBoardPage=false;
	m_isRefreshGuildWndMemberPage=false;
	m_isRefreshGuildWndMemberPageGradeComboBox=false;
	m_isRefreshGuildWndSkillPage=false;
	m_isRefreshGuildWndGradePage=false;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_isRefreshAcceWnd = false;
#endif
#ifdef ENABLE_GUILDRENEWAL_SYSTEM
	m_isRefreshGuildBank = false;
	m_isRefreshGuildBankInfo = false;
#endif
	m_isRefreshGuildBaseInfoPage = false;
	m_isRefreshGuildBaseInfoPageBankGold = false;
#ifdef ENABLE_GUILDRENEWAL_SYSTEM
	m_isRefreshGuildWarInfoPage = false;
#endif
	m_bIsLeftSeatLogOutState = false;

	m_EmoticonStringVector.clear();

	m_pInstTarget = nullptr;
}

void CPythonNetworkStream::Warp(LONG lGlobalX, LONG lGlobalY)
{
	CPythonBackground& rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.Destroy();
	rkBgMgr.Create();
	rkBgMgr.Warp(lGlobalX, lGlobalY);
	rkBgMgr.RefreshShadowLevel();
	rkBgMgr.ReleaseSelectScreenMapPreload();

	LONG lLocalX = lGlobalX;
	LONG lLocalY = lGlobalY;
	__GlobalPositionToLocalPosition(lLocalX, lLocalY);
	float fHeight = CPythonBackground::Instance().GetHeight(float(lLocalX), float(lLocalY));

	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();
	rkApp.SetCenterPosition(float(lLocalX), float(lLocalY), fHeight);

	__ShowMapName(lLocalX, lLocalY);
}

void CPythonNetworkStream::__ShowMapName(LONG lLocalX, LONG lLocalY)
{
	const std::string & c_rstrMapFileName = CPythonBackground::Instance().GetWarpMapName();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ShowMapName", Py_BuildValue("(sii)", c_rstrMapFileName.c_str(), lLocalX, lLocalY));
}

void CPythonNetworkStream::__LeaveGamePhase()
{
#ifdef ENABLE_SHOP_REPURCHASE
	if (CPythonRepurchaseShop::InstancePtr())
		CPythonRepurchaseShop::Instance().Clear();
#endif
	CInstanceBase::ClearPVPKeySystem();

	__ClearNetworkActorManager();

	m_bComboSkillFlag = FALSE;

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	rkChrMgr.Destroy();

	CPythonItem& rkItemMgr=CPythonItem::Instance();
	rkItemMgr.Destroy();
}

void CPythonNetworkStream::SetGamePhase()
{
	if ("Game"!=m_strPhase)
		m_phaseLeaveFunc.Run();

	Tracen("");
	Tracen("## Network - Game Phase ##");
	Tracen("");

	m_strPhase = "Game";

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set(this, &CPythonNetworkStream::GamePhase);
	m_phaseLeaveFunc.Set(this, &CPythonNetworkStream::__LeaveGamePhase);

	IAbstractPlayer & rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	// Game phase'e geçildi?inde dil bilgisini server'a gönder
	if (m_bLanguageSet == FALSE && m_bLanguage != 0)
	{
		SendChangeLanguagePacket(m_bLanguage);
	}
#endif

#ifdef ENABLE_TRACK_WINDOW
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "TrackWindowUpdate", Py_BuildValue("()"));
#endif

	__RefreshStatus();
}

bool CPythonNetworkStream::RecvObserverAddPacket()
{
	TPacketGCObserverAdd kObserverAddPacket;
	if (!Recv(sizeof(kObserverAddPacket), &kObserverAddPacket))
		return false;

	CPythonMiniMap::Instance().AddObserver(
		kObserverAddPacket.vid, 
		kObserverAddPacket.x*100.0f, 
		kObserverAddPacket.y*100.0f);

	return true;
}

#ifdef ENABLE_MOB_DROP_INFO
bool CPythonNetworkStream::SendTargetInfoLoadPacket(DWORD dwVID)
{
	TPacketCGTargetInfoLoad TargetInfoLoadPacket;
	TargetInfoLoadPacket.header = HEADER_CG_TARGET_INFO_LOAD;
	TargetInfoLoadPacket.dwVID = dwVID;

	if (!Send(sizeof(TargetInfoLoadPacket), &TargetInfoLoadPacket))
		return false;

	return SendSequence();
}
#endif

bool CPythonNetworkStream::RecvObserverRemovePacket()
{
	TPacketGCObserverAdd kObserverRemovePacket;
	if (!Recv(sizeof(kObserverRemovePacket), &kObserverRemovePacket))
		return false;

	CPythonMiniMap::Instance().RemoveObserver(
		kObserverRemovePacket.vid
	);

	return true;
}

bool CPythonNetworkStream::RecvObserverMovePacket()
{
	TPacketGCObserverMove kObserverMovePacket;
	if (!Recv(sizeof(kObserverMovePacket), &kObserverMovePacket))
		return false;

	CPythonMiniMap::Instance().MoveObserver(kObserverMovePacket.vid, kObserverMovePacket.x*100.0f, kObserverMovePacket.y*100.0f);

	return true;
}

bool CPythonNetworkStream::RecvWarpPacket()
{
	TPacketGCWarp kWarpPacket;

	if (!Recv(sizeof(kWarpPacket), &kWarpPacket))
		return false;

#ifdef ENABLE_CLIENT_PERFORMANCE
	__DirectEnterMode_Set(m_dwSelectedCharacterIndex, true);
#else
	__DirectEnterMode_Set(m_dwSelectedCharacterIndex);
#endif

	CNetworkStream::Connect((DWORD)kWarpPacket.lAddr, kWarpPacket.wPort);

	return true;
}

bool CPythonNetworkStream::RecvDuelStartPacket()
{
	TPacketGCDuelStart kDuelStartPacket;
	if (!Recv(sizeof(kDuelStartPacket), &kDuelStartPacket))
		return false;
	
	DWORD count = (kDuelStartPacket.wSize - sizeof(kDuelStartPacket))/sizeof(DWORD);

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
	{
		TraceError("CPythonNetworkStream::RecvDuelStartPacket - MainCharacter is NULL");
		return false;
	}
	DWORD dwVIDSrc = pkInstMain->GetVirtualID();
	DWORD dwVIDDest;

	for ( DWORD i = 0; i < count; i++)
	{
		Recv(sizeof(dwVIDDest),&dwVIDDest);
		CInstanceBase::InsertDUELKey(dwVIDSrc,dwVIDDest);
	}

	if(count == 0)
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_CANNOTATTACK);
	else
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_START);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));

	rkChrMgr.RefreshAllPCTextTail();

	return true;
}

bool CPythonNetworkStream::RecvPVPPacket()
{
	TPacketGCPVP kPVPPacket;
	if (!Recv(sizeof(kPVPPacket), &kPVPPacket))
		return false;

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();
	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();

	switch (kPVPPacket.bMode)
	{
		case PVP_MODE_AGREE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDDst))
				rkPlayer.RememberChallengeInstance(kPVPPacket.dwVIDSrc);

			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDSrc))
				rkPlayer.RememberCantFightInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_REVENGE:
		{
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			DWORD dwKiller = kPVPPacket.dwVIDSrc;
			DWORD dwVictim = kPVPPacket.dwVIDDst;

			if (rkPlayer.IsMainCharacterIndex(dwVictim))
				rkPlayer.RememberRevengeInstance(dwKiller);

			if (rkPlayer.IsMainCharacterIndex(dwKiller))
				rkPlayer.RememberCantFightInstance(dwVictim);
			break;
		}

		case PVP_MODE_FIGHT:
			rkChrMgr.InsertPVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_NONE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
	}

	__RefreshTargetBoardByVID(kPVPPacket.dwVIDSrc);
	__RefreshTargetBoardByVID(kPVPPacket.dwVIDDst);

	return true;
}

void CPythonNetworkStream::NotifyHack(const char *c_szMsg)
{
	if (!m_kQue_stHack.empty())
		if (c_szMsg==m_kQue_stHack.back())
			return;

	m_kQue_stHack.push_back(c_szMsg);
}

bool CPythonNetworkStream::__SendHack(const char *c_szMsg)
{
	Tracen(c_szMsg);
	
	TPacketCGHack kPacketHack;
	kPacketHack.bHeader=HEADER_CG_HACK;
	strncpy(kPacketHack.szBuf, c_szMsg, sizeof(kPacketHack.szBuf)-1);

	if (!Send(sizeof(kPacketHack), &kPacketHack))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_VID;
	if (!Send(sizeof(packet), &packet))
		return false;
	if (!Send(sizeof(vid), &vid))
		return false;
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByNamePacket(const char *c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_NAME;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN-1);
	szName[CHARACTER_NAME_MAX_LEN-1] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;
	Tracef(" SendMessengerAddByNamePacket : %s\n", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerRemovePacket(const char *c_szKey, const char *c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_REMOVE;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szKey[CHARACTER_NAME_MAX_LEN];
	memset(szKey, 0, sizeof(szKey));
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN-1);
	if (!Send(sizeof(szKey), &szKey))
		return false;
	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}

#ifdef ENABLE_MESSENGER_BLOCK
bool CPythonNetworkStream::SendMessengerBlockAddByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_VID;

	if (!Send(sizeof(packet), &packet))
		return false;

	if (!Send(sizeof(vid), &vid))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockAddByNamePacket(const char *c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_NAME;

	if (!Send(sizeof(packet), &packet))
		return false;

	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN - 1);
	szName[CHARACTER_NAME_MAX_LEN - 1] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;

	Tracef(" SendMessengerAddBlockByNamePacket : %s\n", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockRemovePacket(const char *c_szKey, const char *c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_BLOCK_REMOVE_BLOCK;

	if (!Send(sizeof(packet), &packet))
		return false;

	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN - 1);

	if (!Send(sizeof(szKey), &szKey))
		return false;

	CPythonMessenger::Instance().RemoveBlock(szKey);
	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockRemoveByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_BLOCK_REMOVE_BLOCK;

	if (!Send(sizeof(packet), &packet))
		return false;

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(vid);

	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, pInstance->GetNameString(), CHARACTER_NAME_MAX_LEN - 1);
	if (!Send(sizeof(szKey), &szKey))
		return false;

	CPythonMessenger::Instance().RemoveBlock(szKey);
	__RefreshTargetBoardByName(pInstance->GetNameString());
	return SendSequence();
}
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
bool CPythonNetworkStream::SendMessengerRequestWarpByNamePacket(const char *c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_REQUEST_WARP_BY_NAME;

	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}

	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN - 1);
	szName[CHARACTER_NAME_MAX_LEN - 1] = '\0';

	if (!Send(sizeof(szName), &szName))
	{
		return false;
	}

	return true;
}

bool CPythonNetworkStream::SendMessengerSummonByNamePacket(const char *c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_SUMMON_BY_NAME;

	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}

	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN - 1);
	szName[CHARACTER_NAME_MAX_LEN - 1] = '\0';

	if (!Send(sizeof(szName), &szName))
	{
		return false;
	}

	return true;
}
#endif

bool CPythonNetworkStream::SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
	NANOBEGIN
	if (!__CanActMainInstance())
		return true;

	if (fDstRot < 0.0f)
		fDstRot = 360 + fDstRot;
	else if (fDstRot > 360.0f)
		fDstRot = fmodf(fDstRot, 360.0f);

	TPacketCGMove kStatePacket;
	kStatePacket.bHeader = HEADER_CG_CHARACTER_MOVE;
	kStatePacket.bFunc = eFunc;
	kStatePacket.bArg = uArg;
	kStatePacket.bRot = fDstRot/5.0f;
	kStatePacket.lX = long(c_rkPPosDst.x);
	kStatePacket.lY = long(c_rkPPosDst.y);
	kStatePacket.dwTime = ELTimer_GetServerMSec();
	
	assert(kStatePacket.lX >= 0 && kStatePacket.lX < 204800);

	__LocalPositionToGlobalPosition(kStatePacket.lX, kStatePacket.lY);

	if (!Send(sizeof(kStatePacket), &kStatePacket))
	{
		Tracenf("CPythonNetworkStream::SendCharacterStatePacket(dwCmdTime=%u, fDstPos=(%f, %f), fDstRot=%f, eFunc=%d uArg=%d) - PACKET SEND ERROR",
			kStatePacket.dwTime,
			float(kStatePacket.lX),
			float(kStatePacket.lY),
			fDstRot,
			kStatePacket.bFunc,
			kStatePacket.bArg);
		return false;
	}
	NANOEND
	return SendSequence();
}

bool CPythonNetworkStream::SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID)
{
	TPacketCGUseSkill UseSkillPacket;
	UseSkillPacket.bHeader = HEADER_CG_USE_SKILL;
	UseSkillPacket.dwVnum = dwSkillIndex;
	UseSkillPacket.dwTargetVID = dwTargetVID;
	if (!Send(sizeof(TPacketCGUseSkill), &UseSkillPacket))
	{
		Tracen("CPythonNetworkStream::SendUseSkillPacket - SEND PACKET ERROR");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
bool CPythonNetworkStream::SendChatPacket(const char *c_szChat, BYTE byType, DWORD dwLangID)
#else
bool CPythonNetworkStream::SendChatPacket(const char *c_szChat, BYTE byType)
#endif
{
	if (strlen(c_szChat) == 0)
		return true;

#ifdef ENABLE_EMOTICONS_SYSTEM
	if (strlen (c_szChat) >= 1024)
#else
	if (strlen (c_szChat) >= 512)
#endif
		return true;

	if (c_szChat[0] == '/')
	{
		if (1 == strlen(c_szChat))
		{
			if (!m_strLastCommand.empty())
				c_szChat = m_strLastCommand.c_str();
		}
		else
		{
			m_strLastCommand = c_szChat;
		}
	}

	if (ClientCommand(c_szChat))
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGChat ChatPacket;
	ChatPacket.header = HEADER_CG_CHAT;
	ChatPacket.length = sizeof(ChatPacket) + iTextLen;
	ChatPacket.type = byType;

	if (!Send(sizeof(ChatPacket), &ChatPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

//////////////////////////////////////////////////////////////////////////
// Emoticon
void CPythonNetworkStream::RegisterEmoticonString(const char *pcEmoticonString)
{
	if (m_EmoticonStringVector.size() >= CInstanceBase::EMOTICON_NUM)
	{
		TraceError("Can't register emoticon string... vector is full (size:%d)", m_EmoticonStringVector.size() );
		return;
	}
	m_EmoticonStringVector.push_back(pcEmoticonString);
}

bool CPythonNetworkStream::ParseEmoticon(const char *pChatMsg, DWORD * pdwEmoticon)
{
	for (DWORD dwEmoticonIndex = 0; dwEmoticonIndex < m_EmoticonStringVector.size() ; ++dwEmoticonIndex)
	{
		if (strlen(pChatMsg) > m_EmoticonStringVector[dwEmoticonIndex].size())
			continue;

		const char *pcFind = strstr(pChatMsg, m_EmoticonStringVector[dwEmoticonIndex].c_str());

		if (pcFind != pChatMsg)
			continue;

		*pdwEmoticon = dwEmoticonIndex;

		return true;
	}

	return false;
}
// Emoticon
//////////////////////////////////////////////////////////////////////////

void CPythonNetworkStream::__ConvertEmpireText(DWORD dwEmpireID, char *szText)
{
	if (dwEmpireID<1 || dwEmpireID>3)
		return;

	UINT uHanPos;

	STextConvertTable& rkTextConvTable=m_aTextConvTable[dwEmpireID-1];

	BYTE* pbText=(BYTE*)szText;
	while (*pbText)
	{
		if (*pbText & 0x80)
		{
			if (pbText[0]>=0xb0 && pbText[0]<=0xc8 && pbText[1]>=0xa1 && pbText[1]<=0xfe)
			{
				uHanPos=(pbText[0]-0xb0)*(0xfe-0xa1+1)+(pbText[1]-0xa1);
				pbText[0]=rkTextConvTable.aacHan[uHanPos][0];
				pbText[1]=rkTextConvTable.aacHan[uHanPos][1];
			}
			pbText+=2;
		}
		else
		{
			if (*pbText>='a' && *pbText<='z')
			{
				*pbText=rkTextConvTable.acLower[*pbText-'a'];
			}
			else if (*pbText>='A' && *pbText<='Z')
			{
				*pbText=rkTextConvTable.acUpper[*pbText-'A'];
			}
			pbText++;
		}
	}
}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
static std::string SBS(const std::string& c_str, const std::string& c_strStartDelim, const std::string& c_strEndDelim)
{
	unsigned u_strFirstDelimPos = c_str.find(c_strStartDelim);
	unsigned u_strEndPosFirstDelim = u_strFirstDelimPos + c_strStartDelim.length();
	unsigned u_strLastDelimPos = c_str.find_first_of(c_strEndDelim, u_strEndPosFirstDelim);
	return c_str.substr(u_strEndPosFirstDelim, u_strLastDelimPos - u_strEndPosFirstDelim);
}

static const char *FilterChat(const char *c_pszBuffer)
{
	std::string strChatBuf = c_pszBuffer;

	std::string strHyperItemLink = "|cffffc700|H";
	std::string strNHyperItemLink = "|cfff1e6c0|H";

	std::string::size_type zPos = 0;
	while ((zPos = strChatBuf.find(strHyperItemLink, zPos)) != std::string::npos)
	{
		zPos += strHyperItemLink.length();

		std::string strElement = strChatBuf.substr(zPos, strChatBuf.length());

		if (strElement.find("[") != std::string::npos && strElement.find("]") != std::string::npos)
		{
			std::string strItemLink = SBS(strElement, "", "|h");
			std::string strItemVnum = SBS(strElement, "item:", ":");
			DWORD dwItemVnum = htoi(strItemVnum.c_str());
			std::string strItemName = SBS(strElement, "[", "]").c_str();

			CItemManager::Instance().SelectItemData(dwItemVnum);
			CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

			const char *c_pszItemName = pItemData->GetName();

			{
				char szSearchBuf[1024 + 1];
				snprintf(szSearchBuf, sizeof(szSearchBuf), "[%s]", strItemName.c_str());
				std::string strReplace(szSearchBuf);

				char szReplaceBuf[1024 + 1];
				snprintf(szReplaceBuf, sizeof(szReplaceBuf), "[%s]", c_pszItemName);

				std::string::size_type pos = strChatBuf.find(strReplace);
				strChatBuf.replace(pos, strReplace.length(), szReplaceBuf);
			}
		}
	}

	std::string::size_type xPos = 0;
	while ((xPos = strChatBuf.find(strNHyperItemLink, xPos)) != std::string::npos)
	{
		xPos += strNHyperItemLink.length();

		std::string strElement = strChatBuf.substr(xPos, strChatBuf.length());

		if (strElement.find("[") != std::string::npos && strElement.find("]") != std::string::npos)
		{
			std::string strItemLink = SBS(strElement, "", "|h");
			std::string strItemVnum = SBS(strElement, "item:", ":");
			DWORD dwItemVnum = htoi(strItemVnum.c_str());
			std::string strItemName = SBS(strElement, "[", "]").c_str();

			CItemManager::Instance().SelectItemData(dwItemVnum);
			CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

			const char *c_pszItemName = pItemData->GetName();

			{
				char szSearchBuf[1024 + 1];
				snprintf(szSearchBuf, sizeof(szSearchBuf), "[%s]", strItemName.c_str());
				std::string strReplace(szSearchBuf);

				char szReplaceBuf[1024 + 1];
				snprintf(szReplaceBuf, sizeof(szReplaceBuf), "[%s]", c_pszItemName);

				std::string::size_type pos = strChatBuf.find(strReplace);
				strChatBuf.replace(pos, strReplace.length(), szReplaceBuf);
			}
		}
	}

	static char szFilteredChat[1024 + 1] = {};
	const char *c_szpFilterBuf = strChatBuf.c_str();
	snprintf(szFilteredChat, sizeof(szFilteredChat), "%s", c_szpFilterBuf);
	return szFilteredChat;
}
#endif

bool CPythonNetworkStream::RecvChatPacket()
{
	TPacketGCChat kChat;

#ifdef ENABLE_EMOTICONS_SYSTEM
	char buf[2048 + 1];
	char line[2048 + 1];
#else
	char buf[1024 + 1];
	char line[1024 + 1];
#endif

	if (!Recv(sizeof(kChat), &kChat))
		return false;

	UINT uChatSize=kChat.size - sizeof(kChat);

	if (!Recv(uChatSize, buf))
		return false;

	buf[uChatSize]='\0';

	if (kChat.type >= CHAT_TYPE_MAX_NUM)
		return true;

	if (CHAT_TYPE_COMMAND == kChat.type)
	{
		ServerCommand(buf);
		return true;
	}

	if (kChat.dwVID != 0)
	{
		CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
		CInstanceBase * pkInstChatter = rkChrMgr.GetInstancePtr(kChat.dwVID);
		if (NULL == pkInstChatter)
			return true;

		switch (kChat.type)
		{
			case CHAT_TYPE_TALKING:
			case CHAT_TYPE_PARTY:
			case CHAT_TYPE_GUILD:
			case CHAT_TYPE_SHOUT:
			case CHAT_TYPE_WHISPER:
				{
					char *p = strchr(buf, ':');

					// fix PM on Player Name
					if (p != NULL)
					{
						p = strchr(p + 1, ':');
					}

					if (p)
						p += 2;
					else
						p = buf;

					DWORD dwEmoticon;

					if (ParseEmoticon(p, &dwEmoticon))
					{
						pkInstChatter->SetEmoticon(dwEmoticon);
						return true;
					}
					else
					{
						if (gs_bEmpireLanuageEnable)
						{
							CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
							if (pkInstMain)
								if (!pkInstMain->IsSameEmpire(*pkInstChatter))
									__ConvertEmpireText(pkInstChatter->GetEmpireID(), p);
						}

						if (m_isEnableChatInsultFilter)
						{
							if (false == pkInstChatter->IsNPC() && false == pkInstChatter->IsEnemy())
							{
								__FilterInsult(p, strlen(p));
							}
						}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
						if (pkInstChatter->IsNPC() || pkInstChatter->IsEnemy())
						{
							std::string strChat(buf, sizeof(buf));

							unsigned uFirstDelim = strChat.find("[");
							unsigned uLastDelim = strChat.find("]");

							std::string strFilter = strChat.substr(uFirstDelim + 1, uLastDelim - uFirstDelim);
							DWORD id = atoi(strFilter.c_str());

							const char *c_szMobChat = GetLocaleChatVnum(id);

							_snprintf(line, sizeof(line), "%s", c_szMobChat);
							break; return true;
						}
#endif

						_snprintf(line, sizeof(line), "%s", p);
					}
				}
				break;

			case CHAT_TYPE_COMMAND:
			case CHAT_TYPE_INFO:
			case CHAT_TYPE_NOTICE:
			case CHAT_TYPE_BIG_NOTICE:
#ifdef ENABLE_DICE_SYSTEM
			case CHAT_TYPE_DICE_INFO:
#endif
#ifdef ENABLE_RENEWAL_OX_EVENT
			case CHAT_TYPE_CONTROL_NOTICE:
#endif
			case CHAT_TYPE_MAX_NUM:
			default:
				_snprintf(line, sizeof(line), "%s", buf);
				break;
		}

		if (CHAT_TYPE_SHOUT != kChat.type)
		{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, FilterChat(line));
#else
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, line);
#endif
		}

#ifdef ENABLE_MESSENGER_BLOCK
		if (CPythonMessenger::Instance().IsBlockFriendByName(pkInstChatter->GetNameString()))
			return true;
#endif

        if (kChat.type == CHAT_TYPE_GUILD && CPythonSystem::Instance().GetAdvancedOption("guild_chat_to_whisper"))
        {
            PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdvancedGuildChat", Py_BuildValue("(s)", buf));
            return true;
        }
		if (pkInstChatter->IsPC())
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			CPythonChat::Instance().AppendChat(kChat.type, FilterChat(buf));
#else
			CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif
	}
	else
	{
		if (CHAT_TYPE_NOTICE == kChat.type)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", buf));
		}
		else if (CHAT_TYPE_BIG_NOTICE == kChat.type)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", buf));
		}
#ifdef ENABLE_RENEWAL_OX_EVENT
		else if (CHAT_TYPE_CONTROL_NOTICE == kChat.type)
		{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			std::string strChat(buf, sizeof(buf));

			unsigned uFirstDelim = strChat.find("[");
			unsigned uLastDelim = strChat.find("]");

			std::string strFilter = strChat.substr(uFirstDelim + 1, uLastDelim - uFirstDelim);
			DWORD id = atoi(strFilter.c_str());

			if (strstr(strChat.c_str(), "#start") || strstr(strChat.c_str(), "#send") || strstr(strChat.c_str(), "#end"))
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", buf));
			}
			else
			{
				if (id > 900)
				{
					std::string c_szStringChat = GetLocaleStringVnum(id);
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", c_szStringChat));
					CPythonChat::Instance().AppendChat(kChat.type, c_szStringChat.c_str());
				}
				else
				{
					const char *c_szQuizChat = GetLocaleQuizVnum(id);
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", c_szQuizChat));
					CPythonChat::Instance().AppendChat(kChat.type, c_szQuizChat);
				}
			}
			return true;
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", buf));
#endif
		}
#endif
		else if (CHAT_TYPE_SHOUT == kChat.type)
		{
			char *p = strchr(buf, ':');

			if (p)
			{
				if (m_isEnableChatInsultFilter)
					__FilterInsult(p, strlen(p));
			}
		}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		CPythonChat::Instance().AppendChat(kChat.type, FilterChat(buf));
#else
		CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif

	}
	return true;
}

bool CPythonNetworkStream::RecvWhisperPacket()
{
	TPacketGCWhisper whisperPacket;
	char buf[512 + 1];

	if (!Recv(sizeof(whisperPacket), &whisperPacket))
		return false;

	assert(whisperPacket.wSize - sizeof(whisperPacket) < 512);

	if (!Recv(whisperPacket.wSize - sizeof(whisperPacket), &buf))
		return false;

	buf[whisperPacket.wSize - sizeof(whisperPacket)] = '\0';

	static char line[256];
	if (CPythonChat::WHISPER_TYPE_CHAT == whisperPacket.bType || CPythonChat::WHISPER_TYPE_GM == whisperPacket.bType
#ifdef ENABLE_OFFLINE_MESSAGE
		|| CPythonChat::WHISPER_TYPE_OFFLINE == whisperPacket.bType
#endif
	)
	{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, FilterChat(buf));
#else
		_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, buf);
#endif
#ifdef ENABLE_MULTILANGUAGE_CHATTING
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(issi)", (int) whisperPacket.bType, whisperPacket.szNameFrom, line, 0));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, line));
#endif
	}
	else if (CPythonChat::WHISPER_TYPE_SYSTEM == whisperPacket.bType || CPythonChat::WHISPER_TYPE_ERROR == whisperPacket.bType)
	{
		PyCallClassMemberFunc (m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperSystemMessage", Py_BuildValue ("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}
	else
	{
		PyCallClassMemberFunc (m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperError", Py_BuildValue ("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}

	return true;
}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
bool CPythonNetworkStream::SendWhisperPacket(const char* name, const char* c_szChat, DWORD dwLangID)
#else
bool CPythonNetworkStream::SendWhisperPacket (const char* name, const char* c_szChat)
#endif
{
	if (strlen (c_szChat) >= 255)
	{
		return true;
	}

	int iTextLen = strlen (c_szChat) + 1;
	TPacketCGWhisper WhisperPacket;
	WhisperPacket.bHeader = HEADER_CG_WHISPER;
	WhisperPacket.wSize = sizeof (WhisperPacket) + iTextLen;

	strncpy (WhisperPacket.szNameTo, name, sizeof (WhisperPacket.szNameTo) - 1);

	if (!Send (sizeof (WhisperPacket), &WhisperPacket))
	{
		return false;
	}

	if (!Send (iTextLen, c_szChat))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvPointChange()
{
	TPacketGCPointChange PointChange;

	if (!Recv (sizeof (TPacketGCPointChange), &PointChange))
	{
		Tracen ("Recv Point Change Packet Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	rkChrMgr.ShowPointEffect (PointChange.Type, PointChange.dwVID);

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pInstance)
	if (PointChange.dwVID == pInstance->GetVirtualID())
	{
		CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
		rkPlayer.SetStatus(PointChange.Type, PointChange.value);

		switch (PointChange.Type)
		{
			case POINT_STAT_RESET_COUNT:
				__RefreshStatus();
				break;
			case POINT_PLAYTIME:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].dwPlayMinutes = PointChange.value;
				break;
			case POINT_LEVEL:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byLevel = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_ST:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byST = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_DX:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byDX = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_HT:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byHT = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_IQ:
#ifdef ENABLE_CONQUEROR_LEVEL
			case POINT_CONQUEROR_LEVEL:
			case POINT_SUNGMA_STR:
			case POINT_SUNGMA_HP:
			case POINT_SUNGMA_MOVE:
			case POINT_SUNGMA_IMMUNE:
#endif
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byIQ = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_SKILL:
			case POINT_SUB_SKILL:
			case POINT_HORSE_SKILL:
				__RefreshSkillWindow();
				break;
			case POINT_ENERGY:
				if (PointChange.value == 0)
				{
					rkPlayer.SetStatus(POINT_ENERGY_END_TIME, 0);
				}
				__RefreshStatus();
				break;
			default:
				__RefreshStatus();
				break;
		}

		if (POINT_GOLD == PointChange.Type)
		{
			if (PointChange.amount > 0)
			{
#ifdef ENABLE_GOLD_LIMIT
				PyObject *args = PyTuple_New(1);
				PyTuple_SetItem(args, 0, PyLong_FromLongLong(PointChange.amount));
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickMoney", args);
#else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickMoney", Py_BuildValue("(i)", PointChange.amount));
#endif
			}
		}
#ifdef ENABLE_GAYA_SYSTEM
		else if (POINT_GEM == PointChange.Type)
		{
			if (PointChange.amount > 0)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickGem", Py_BuildValue("(i)", PointChange.amount));
			}
		}
#endif
	}
	else if (POINT_LEVEL == PointChange.Type) // Fix
	{
		CInstanceBase* pOtherInstance = CPythonCharacterManager::Instance().GetInstancePtr(PointChange.dwVID);
		if (pOtherInstance)
			pOtherInstance->UpdateTextTailLevel(PointChange.value);
	}
	else
	{
		pInstance = rkChrMgr.GetInstancePtr(PointChange.dwVID);
		if (pInstance && PointChange.Type == POINT_LEVEL)
		{
#ifdef ENABLE_GROWTH_PET_SYSTEM
			if (pInstance->IsGrowthPet())
				pInstance->SetPetLevel(PointChange.value);
			else
#endif
			{
				pInstance->SetLevel(PointChange.value);
				pInstance->UpdateTextTailLevel(PointChange.value);
			}
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvStunPacket()
{
	TPacketGCStun StunPacket;

	if (!Recv (sizeof (StunPacket), &StunPacket))
	{
		Tracen ("CPythonNetworkStream::RecvStunPacket Error");
		return false;
	}

	//Tracef("RecvStunPacket %d\n", StunPacket.vid);

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstSel = rkChrMgr.GetInstancePtr (StunPacket.vid);

	if (pkInstSel)
	{
		if (CPythonCharacterManager::Instance().GetMainInstancePtr()==pkInstSel)
		{
			pkInstSel->Die();
		}
		else
		{
			pkInstSel->Stun();
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvDeadPacket()
{
	TPacketGCDead DeadPacket;
	if (!Recv (sizeof (DeadPacket), &DeadPacket))
	{
		Tracen ("CPythonNetworkStream::RecvDeadPacket Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkChrInstSel = rkChrMgr.GetInstancePtr (DeadPacket.vid);
	if (pkChrInstSel)
	{
		CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
		if (pkInstMain==pkChrInstSel)
		{
			Tracenf ("ÁÖÀÎ°ø »ç¸Á");
			if (false == pkInstMain->GetDuelMode())
			{
#ifndef ENABLE_RENEWAL_DEAD_PACKET
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("()"));
#else
				PyObject *times = PyTuple_New(REVIVE_TYPE_MAX);
				for (int i = REVIVE_TYPE_HERE; i < REVIVE_TYPE_MAX; i++)
					PyTuple_SetItem(times, i, PyInt_FromLong(DeadPacket.t_d[i]));
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("(O)", times));
#endif
			}
			CPythonPlayer::Instance().NotifyDeadMainCharacter();
			CPythonPlayer::Instance().ResetSkillCoolTimes();
		}

		pkChrInstSel->Die();
	}

	return true;
}

bool CPythonNetworkStream::SendCharacterPositionPacket (BYTE iPosition)
{
	TPacketCGPosition PositionPacket;

	PositionPacket.header = HEADER_CG_CHARACTER_POSITION;
	PositionPacket.position = iPosition;

	if (!Send (sizeof (TPacketCGPosition), &PositionPacket))
	{
		Tracen ("Send Character Position Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOnClickPacket (DWORD vid)
{
	TPacketCGOnClick OnClickPacket;
	OnClickPacket.header	= HEADER_CG_ON_CLICK;
	OnClickPacket.vid		= vid;

	if (!Send (sizeof (OnClickPacket), &OnClickPacket))
	{
		Tracen ("Send On_Click Packet Error");
		return false;
	}

	Tracef("SendOnClickPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::RecvCharacterPositionPacket()
{
	TPacketGCPosition PositionPacket;

	if (!Recv (sizeof (TPacketGCPosition), &PositionPacket))
	{
		return false;
	}

	CInstanceBase* pChrInstance = CPythonCharacterManager::Instance().GetInstancePtr (PositionPacket.vid);

	if (!pChrInstance)
	{
		return true;
	}

	//pChrInstance->UpdatePosition(PositionPacket.position);

	return true;
}

bool CPythonNetworkStream::RecvMotionPacket()
{
	TPacketGCMotion MotionPacket;

	if (!Recv (sizeof (TPacketGCMotion), &MotionPacket))
	{
		return false;
	}

	CInstanceBase* pMainInstance = CPythonCharacterManager::Instance().GetInstancePtr (MotionPacket.vid);
	CInstanceBase* pVictimInstance = nullptr;

	if (0 != MotionPacket.victim_vid)
	{
		pVictimInstance = CPythonCharacterManager::Instance().GetInstancePtr (MotionPacket.victim_vid);
	}

	/* Kanalda henüz spawn olmam?? vid için motion gelirse hata verme, yoksay (syserr spam önleme) */
	if (!pMainInstance)
	{
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvShopPacket()
{
	std::vector<char> vecBuffer;
	vecBuffer.clear();

	TPacketGCShop  packet_shop;
	if (!Recv (sizeof (packet_shop), &packet_shop))
	{
		return false;
	}

#ifdef ENABLE_SHOP_REPURCHASE
	if (packet_shop.subheader >= SHOP_SUBHEADER_GC_REPURCHASE_OPEN &&
		packet_shop.subheader <= SHOP_SUBHEADER_GC_REPURCHASE_ITEM_LIST)
	{
		return CPythonRepurchaseShop::Instance().ProcessPacket(packet_shop.subheader);
	}
#endif

	int iSize = packet_shop.size - sizeof (packet_shop);
	if (iSize > 0)
	{
		vecBuffer.resize (iSize);
		if (!Recv (iSize, &vecBuffer[0]))
		{
			return false;
		}
	}

	switch (packet_shop.subheader)
	{
		case SHOP_SUBHEADER_GC_START:
			{
				CPythonShop::Instance().Clear();

				DWORD dwVID = *(DWORD *)&vecBuffer[0];

				TPacketGCShopStart * pShopStartPacket = (TPacketGCShopStart *)&vecBuffer[4];
				for (BYTE iItemIndex = 0; iItemIndex < SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
				{
					CPythonShop::Instance().SetItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
			}
			break;

		case SHOP_SUBHEADER_GC_START_EX:
			{
				CPythonShop::Instance().Clear();

				TPacketGCShopStartEx * pShopStartPacket = (TPacketGCShopStartEx *)&vecBuffer[0];
				size_t read_point = sizeof(TPacketGCShopStartEx);

				DWORD dwVID = pShopStartPacket->owner_vid;
				BYTE shop_tab_count = pShopStartPacket->shop_tab_count;

				CPythonShop::instance().SetTabCount(shop_tab_count);
				
				for (unsigned char i = 0; i < shop_tab_count; i++)
				{
					TPacketGCShopStartEx::TSubPacketShopTab* pPackTab = (TPacketGCShopStartEx::TSubPacketShopTab*)&vecBuffer[read_point];
					read_point += sizeof(TPacketGCShopStartEx::TSubPacketShopTab);
					
					CPythonShop::instance().SetTabCoinType(i, pPackTab->coin_type);
					CPythonShop::instance().SetTabName(i, pPackTab->name);

					struct packet_shop_item* item = &pPackTab->items[0];
					
					for (BYTE j = 0; j < SHOP_HOST_ITEM_MAX_NUM; j++)
					{
						TShopItemData* itemData = (item + j);
						CPythonShop::Instance().SetItemData(i, j, *itemData);
					}
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
			}
			break;


		case SHOP_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndShop", Py_BuildValue("()"));
			break;

		case SHOP_SUBHEADER_GC_UPDATE_ITEM:
			{
				TPacketGCShopUpdateItem * pShopUpdateItemPacket = (TPacketGCShopUpdateItem *)&vecBuffer[0];
				CPythonShop::Instance().SetItemData(pShopUpdateItemPacket->pos, pShopUpdateItemPacket->item);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshShop", Py_BuildValue("()"));
			}
			break;

		case SHOP_SUBHEADER_GC_UPDATE_PRICE:
#ifdef ENABLE_GOLD_LIMIT
			{
				PyObject *args = PyTuple_New(1);
				PyTuple_SetItem(args, 0, PyLong_FromLongLong(*(long long *)&vecBuffer[0]));
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", args);
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", Py_BuildValue("(i)", *(int *)&vecBuffer[0]));
#endif
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_COINS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_COINS"));
			break;

		case SHOP_SUBHEADER_GC_SOLDOUT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "SOLDOUT"));
			break;

		case SHOP_SUBHEADER_GC_INVENTORY_FULL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVENTORY_FULL"));
			break;

		case SHOP_SUBHEADER_GC_INVALID_POS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVALID_POS"));
			break;

#ifdef ENABLE_RENEWAL_SHOPEX
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_ITEM"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_EXP:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_EXP"));
			break;

		case SHOP_SUBHEADER_GC_YOU_ALREADY_HAVE_ONE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "ALREADY_HAVE_ONE_ITEM"));
			break;
#endif

		default:
			TraceError("CPythonNetworkStream::RecvShopPacket: Unknown subheader\n");
			break;
	}

	return true;
}

bool CPythonNetworkStream::RecvExchangePacket()
{
	TPacketGCExchange exchange_packet;

	if (!Recv(sizeof(exchange_packet), &exchange_packet))
		return false;

	switch (exchange_packet.subheader)
	{
		case EXCHANGE_SUBHEADER_GC_START:
			CPythonExchange::Instance().Clear();
			CPythonExchange::Instance().Start();
			CPythonExchange::Instance().SetSelfName(CPythonPlayer::Instance().GetName());
			CPythonExchange::Instance().SetSelfRace(CPythonPlayer::Instance().GetRace());
			CPythonExchange::Instance().SetSelfLevel(CPythonPlayer::Instance().GetStatus(POINT_LEVEL));

			{
				CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(exchange_packet.arg1);

				if (pCharacterInstance)
				{
					CPythonExchange::Instance().SetTargetName(pCharacterInstance->GetNameString());
					CPythonExchange::Instance().SetTargetRace(pCharacterInstance->GetRace());
					CPythonExchange::Instance().SetTargetLevel(pCharacterInstance->GetLevel());
				}
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartExchange", Py_BuildValue("()"));
			break;

		case EXCHANGE_SUBHEADER_GC_ITEM_ADD:
			if (exchange_packet.is_me)
			{
				int iSlotIndex = exchange_packet.arg2.cell;
#ifdef ENABLE_STACK_LIMIT
				CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, (WORD) exchange_packet.arg3);
#else
				CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, (BYTE) exchange_packet.arg3);
#endif
				for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
					CPythonExchange::Instance().SetItemMetinSocketToSelf(iSlotIndex, i, exchange_packet.alValues[i]);
				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					CPythonExchange::Instance().SetItemAttributeToSelf(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);

#ifdef ENABLE_SLOT_MARKING_SYSTEM
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddExchangeItemSlotIndex", Py_BuildValue("(i)", exchange_packet.arg4.cell));
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
				CPythonExchange::Instance().SetItemChangeLookVnumToSelf(iSlotIndex, exchange_packet.dwTransmutationVnum);
#endif
			}
			else
			{
				int iSlotIndex = exchange_packet.arg2.cell;
#ifdef ENABLE_STACK_LIMIT
				CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, (WORD) exchange_packet.arg3);
#else
				CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, (BYTE) exchange_packet.arg3);
#endif
				for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
					CPythonExchange::Instance().SetItemMetinSocketToTarget(iSlotIndex, i, exchange_packet.alValues[i]);
				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					CPythonExchange::Instance().SetItemAttributeToTarget(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
				CPythonExchange::Instance().SetItemChangeLookVnumToTarget(iSlotIndex, exchange_packet.dwTransmutationVnum);
#endif
			}

			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ITEM_DEL:
			if (exchange_packet.is_me)
			{
#ifdef ENABLE_STACK_LIMIT
				CPythonExchange::Instance().DelItemOfSelf((WORD)exchange_packet.arg1);
#else
				CPythonExchange::Instance().DelItemOfSelf((BYTE)exchange_packet.arg1);
#endif
			}
			else
			{
#ifdef ENABLE_STACK_LIMIT
				CPythonExchange::Instance().DelItemOfTarget((WORD)exchange_packet.arg1);
#else
				CPythonExchange::Instance().DelItemOfTarget((BYTE)exchange_packet.arg1);
#endif
			}
			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ELK_ADD:
			if (exchange_packet.is_me)
				CPythonExchange::Instance().SetElkToSelf(exchange_packet.arg1);
			else
				CPythonExchange::Instance().SetElkToTarget(exchange_packet.arg1);

			__RefreshExchangeWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ACCEPT:
			if (exchange_packet.is_me)
			{
				CPythonExchange::Instance().SetAcceptToSelf((BYTE) exchange_packet.arg1);
			}
			else
			{
				CPythonExchange::Instance().SetAcceptToTarget((BYTE) exchange_packet.arg1);
			}
			__RefreshExchangeWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndExchange", Py_BuildValue("()"));
			__RefreshInventoryWindow();
			CPythonExchange::Instance().End();
#ifdef ENABLE_GROWTH_PET_SYSTEM
			CPythonPlayer::Instance().ClearPetExchangeData();
#endif
			break;

		case EXCHANGE_SUBHEADER_GC_ALREADY:
			Tracef("trade_already");
			break;

		case EXCHANGE_SUBHEADER_GC_LESS_ELK:
			Tracef("trade_less_elk");
			break;
	};

	return true;
}

bool CPythonNetworkStream::RecvQuestInfoPacket()
{
	TPacketGCQuestInfo QuestInfo;

	if (!Peek(sizeof(TPacketGCQuestInfo), &QuestInfo))
	{
		Tracen("Recv Quest Info Packet Error #1");
		return false;
	}

	if (!Peek(QuestInfo.size))
	{
		Tracen("Recv Quest Info Packet Error #2");
		return false;
	}

	Recv(sizeof(TPacketGCQuestInfo));

	const BYTE & c_rFlag = QuestInfo.flag;

	enum
	{
		QUEST_PACKET_TYPE_NONE,
		QUEST_PACKET_TYPE_BEGIN,
		QUEST_PACKET_TYPE_UPDATE,
		QUEST_PACKET_TYPE_END,
	};

	BYTE byQuestPacketType = QUEST_PACKET_TYPE_NONE;

	if (0 != (c_rFlag & QUEST_SEND_IS_BEGIN))
	{
		BYTE isBegin;
		if (!Recv(sizeof(isBegin), &isBegin))
			return false;

		if (isBegin)
			byQuestPacketType = QUEST_PACKET_TYPE_BEGIN;
		else
			byQuestPacketType = QUEST_PACKET_TYPE_END;
	}
	else
	{
		byQuestPacketType = QUEST_PACKET_TYPE_UPDATE;
	}

	// Recv Data Start
	char szTitle[30 + 1] = "";
	char szClockName[16 + 1] = "";
	int iClockValue = 0;
	char szCounterName[16 + 1] = "";
	int iCounterValue = 0;
	char szIconFileName[24 + 1] = "";

	if (0 != (c_rFlag & QUEST_SEND_TITLE))
	{
		if (!Recv(sizeof(szTitle), &szTitle))
			return false;

		szTitle[30] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_NAME))
	{
		if (!Recv(sizeof(szClockName), &szClockName))
			return false;

		szClockName[16] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_VALUE))
	{
		if (!Recv(sizeof(iClockValue), &iClockValue))
			return false;
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_NAME))
	{
		if (!Recv(sizeof(szCounterName), &szCounterName))
			return false;

		szCounterName[16] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_VALUE))
	{
		if (!Recv(sizeof(iCounterValue), &iCounterValue))
			return false;
	}
	if (0 != (c_rFlag & QUEST_SEND_ICON_FILE))
	{
		if (!Recv(sizeof(szIconFileName), &szIconFileName))
			return false;

		szIconFileName[24] = '\0';
	}
	// Recv Data End

	CPythonQuest& rkQuest = CPythonQuest::Instance();

	// Process Start
	if (QUEST_PACKET_TYPE_END == byQuestPacketType)
	{
		rkQuest.DeleteQuestInstance(QuestInfo.index);
	}
	else if (QUEST_PACKET_TYPE_UPDATE == byQuestPacketType)
	{
		if (!rkQuest.IsQuest(QuestInfo.index))
		{
#ifdef ENABLE_RENEWAL_QUEST
			rkQuest.MakeQuest(QuestInfo.index, QuestInfo.c_index);
#else
			rkQuest.MakeQuest(QuestInfo.index);
#endif
		}

		if (strlen(szTitle) > 0)
			rkQuest.SetQuestTitle(QuestInfo.index, szTitle);
		if (strlen(szClockName) > 0)
			rkQuest.SetQuestClockName(QuestInfo.index, szClockName);
		if (strlen(szCounterName) > 0)
			rkQuest.SetQuestCounterName(QuestInfo.index, szCounterName);
		if (strlen(szIconFileName) > 0)
			rkQuest.SetQuestIconFileName(QuestInfo.index, szIconFileName);

		if (c_rFlag & QUEST_SEND_CLOCK_VALUE)
			rkQuest.SetQuestClockValue(QuestInfo.index, iClockValue);
		if (c_rFlag & QUEST_SEND_COUNTER_VALUE)
			rkQuest.SetQuestCounterValue(QuestInfo.index, iCounterValue);
	}
	else if (QUEST_PACKET_TYPE_BEGIN == byQuestPacketType)
	{
		CPythonQuest::SQuestInstance QuestInstance;
		QuestInstance.dwIndex = QuestInfo.index;
		QuestInstance.strTitle = szTitle;
		QuestInstance.strClockName = szClockName;
		QuestInstance.iClockValue = iClockValue;
		QuestInstance.strCounterName = szCounterName;
		QuestInstance.iCounterValue = iCounterValue;
		QuestInstance.strIconFileName = szIconFileName;
		CPythonQuest::Instance().RegisterQuestInstance(QuestInstance);
	}
	// Process Start End

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshQuest", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvQuestConfirmPacket()
{
	TPacketGCQuestConfirm kQuestConfirmPacket;
	if (!Recv(sizeof(kQuestConfirmPacket), &kQuestConfirmPacket))
	{
		Tracen("RecvQuestConfirmPacket Error");
		return false;
	}

	PyObject *poArg = Py_BuildValue("(sii)", kQuestConfirmPacket.msg, kQuestConfirmPacket.timeout, kQuestConfirmPacket.requestPID);
 	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OnQuestConfirm", poArg);
	return true;
}

bool CPythonNetworkStream::RecvRequestMakeGuild()
{
	TPacketGCBlank blank;
	if (!Recv(sizeof(blank), &blank))
	{
		Tracen("RecvRequestMakeGuild Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AskGuildName", Py_BuildValue("()"));

	return true;
}

void CPythonNetworkStream::ToggleGameDebugInfo()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ToggleDebugInfo", Py_BuildValue("()"));
}

#ifdef ENABLE_PARTY_POSITION
bool CPythonNetworkStream::RecvPartyPositionInfo()
{
	TPacketGCPartyPosition Packet;
	if (!Recv(sizeof(Packet), &Packet))
	{
		Tracen("RecvPartyPositionInfo Error");
		return false;
	}

	for (auto iPacketSize = Packet.wSize - sizeof(Packet); iPacketSize > 0; iPacketSize -= sizeof(TPartyPosition))
	{
		TPartyPosition PositionInfo;
		if (Recv(sizeof(PositionInfo), &PositionInfo))
			CPythonMiniMap::Instance().AddPartyPositionInfo(PositionInfo);
	}
	
	return true;
}
#endif

#ifdef ENABLE_GUILD_RANK_SYSTEM
bool CPythonNetworkStream::RecvGuildRanking()
{
	TPacketGCGuildRankingSend Packet;
	if (!Recv(sizeof(Packet), &Packet))
	{
		Tracen("RecvGuildRanking Error");
		return false;
	}

	switch (Packet.subheader)
	{
		case GUILD_RANK::SUBHEADER_GUILD_RANKING_SEND:
			CPythonGuildRanking::Instance().RegisterRankingData(Packet.id, Packet.szGuildName, Packet.szOwnerName, Packet.level, Packet.point, Packet.win, Packet.draw, Packet.loss);
			break;
		case GUILD_RANK::SUBHEADER_GUILD_RANKING_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_GUILD_RANK_OPEN", Py_BuildValue("()"));
			break;
	}

	return true;
}
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
bool CPythonNetworkStream::RecvSoulRoulette()
{
	TPacketGCSoulRoulette Packet;
	if (!Recv(sizeof(Packet), &Packet)) {
		Tracen("RecvSoulRoulette Error");
		return false;
	}

	enum { OPEN, CLOSE, TURN };
	switch (Packet.option) {
	case OPEN:
		for (int i = 0; i < ROULETTE_ITEM_MAX; i++)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ROULETTE_ICON", Py_BuildValue("(iii)", i, Packet.ItemInfo[i].vnum, Packet.ItemInfo[i].count));
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ROULETTE_OPEN", Py_BuildValue("(ii)", Packet.yang, Packet.soul));
		break;
	case CLOSE:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ROULETTE_CLOSE", Py_BuildValue("()"));
		break;
	case TURN:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ROULETTE_TURN", Py_BuildValue("(ii)", Packet.ItemInfo[0].vnum, Packet.ItemInfo[0].count));
		break;
	}

	return true;
}

bool CPythonNetworkStream::SoulRoulette(const BYTE option)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGSoulRoulette p;
	p.header = HEADER_CG_SOUL_ROULETTE;
	p.option = option;

	if (!Send(sizeof(p), &p)) {
		Tracen("Error SoulRoulette");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendExchangeStartPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_START;
	packet.arg1			= vid;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_start_packet Error\n");
		return false;
	}

	Tracef("send_trade_start_packet   vid %d \n", vid);
	return SendSequence();
}

#ifdef ENABLE_GOLD_LIMIT
bool CPythonNetworkStream::SendExchangeElkAddPacket(long long elk)
#else
bool CPythonNetworkStream::SendExchangeElkAddPacket(DWORD elk)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ELK_ADD;
	packet.arg1			= elk;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_elk_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_ADD;
	packet.Pos			= ItemPos;
	packet.arg2			= byDisplayPos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemDelPacket(BYTE pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_DEL;
	packet.arg1			= pos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_del_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeAcceptPacket()
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ACCEPT;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_accept_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeExitPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_CANCEL;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_exit_packet Error\n");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_DUNGEON_INFO

bool CPythonNetworkStream::SendDungeonInfoOpen()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGDungeonInfoSend	packet;

	packet.header = HEADER_CG_DUNGEON_INFO_SYSTEM;
	packet.subheader = DUNGEON_INFO_SUB_HEADER_OPEN;

	if (!Send(sizeof(TPacketCGDungeonInfoSend), &packet))
	{
		Tracef("SendDungeonInfoOpen Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendDungeonInfoTeleport(int index_teleport)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGDungeonInfoSend	packet;

	packet.header = HEADER_CG_DUNGEON_INFO_SYSTEM;
	packet.subheader = DUNGEON_INFO_SUB_HEADER_TELEPORT;
	packet.index = index_teleport;

	if (!Send(sizeof(TPacketCGDungeonInfoSend), &packet))
	{
		Tracef("SendDungeonInfoTeleport Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendDungeonInfoMision(int index)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGDungeonInfoSend	packet;

	packet.header = HEADER_CG_DUNGEON_INFO_SYSTEM;
	packet.subheader = DUNGEON_INFO_SUB_HEADER_MISION;
	packet.index = index;

	if (!Send(sizeof(TPacketCGDungeonInfoSend), &packet))
	{
		Tracef("SendDungeonInfoMision Error\n");
		return false;
	}

	return SendSequence();
}


bool CPythonNetworkStream::SendDungeonInfoRanking(int index_dungeon)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGDungeonInfoSend	packet;

	packet.header = HEADER_CG_DUNGEON_INFO_SYSTEM;
	packet.subheader = DUNGEON_INFO_SUB_HEADER_RANKING;
	packet.index = index_dungeon;

	if (!Send(sizeof(TPacketCGDungeonInfoSend), &packet))
	{
		Tracef("SendDungeonInfoRanking Error\n");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendPointResetPacket()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartPointReset", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__IsPlayerAttacking()
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	if (!pkInstMain->IsAttacking())
		return false;

	return true;
}

bool CPythonNetworkStream::RecvScriptPacket()
{
	TPacketGCScript ScriptPacket;

	if (!Recv(sizeof(TPacketGCScript), &ScriptPacket))
	{
		TraceError("RecvScriptPacket_RecvError");
		return false;
	}

	if (ScriptPacket.size < sizeof(TPacketGCScript))
	{
		TraceError("RecvScriptPacket_SizeError");
		return false;
	}

	ScriptPacket.size -= sizeof(TPacketGCScript);

	static std::string str;
	str = "";
	str.resize(ScriptPacket.size + 1);

	if (!Recv(ScriptPacket.size, &str[0]))
		return false;

	str[str.size() - 1] = '\0';

	int iIndex = CPythonEventManager::Instance().RegisterEventSetFromString(str);

	if (-1 != iIndex)
	{
		CPythonEventManager::Instance().SetVisibleLineCount(iIndex, 30);
		CPythonNetworkStream::Instance().OnScriptEventStart(ScriptPacket.skin,iIndex);
	}

	return true;
}

bool CPythonNetworkStream::SendScriptAnswerPacket(int iAnswer)
{
	TPacketCGScriptAnswer ScriptAnswer;

	ScriptAnswer.header = HEADER_CG_SCRIPT_ANSWER;
	ScriptAnswer.answer = (BYTE) iAnswer;
	if (!Send(sizeof(TPacketCGScriptAnswer), &ScriptAnswer))
	{
		Tracen("Send Script Answer Packet Error");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_COLLECT_WINDOW
bool CPythonNetworkStream::SendScriptButtonPacket(unsigned int iIndex, BYTE bButtonIndex)
#else
bool CPythonNetworkStream::SendScriptButtonPacket(unsigned int iIndex)
#endif
{
	TPacketCGScriptButton ScriptButton;

	ScriptButton.header = HEADER_CG_SCRIPT_BUTTON;
	ScriptButton.idx = iIndex;
#ifdef ENABLE_COLLECT_WINDOW
	ScriptButton.button = bButtonIndex;
#endif
	if (!Send(sizeof(TPacketCGScriptButton), &ScriptButton))
	{
		Tracen("Send Script Button Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAnswerMakeGuildPacket(const char* c_szName)
{
	TPacketCGAnswerMakeGuild Packet;

	Packet.header = HEADER_CG_ANSWER_MAKE_GUILD;
	strncpy(Packet.guild_name, c_szName, GUILD_NAME_MAX_LEN);
	Packet.guild_name[GUILD_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendAnswerMakeGuild Packet Error");
		return false;
	}

	// 	Tracef(" SendAnswerMakeGuildPacket : %s", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendQuestInputStringPacket(const char *c_szString)
{
	TPacketCGQuestInputString Packet;
	Packet.bHeader = HEADER_CG_QUEST_INPUT_STRING;
	strncpy(Packet.szString, c_szString, QUEST_INPUT_STRING_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendQuestInputStringPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID)
{
	TPacketCGQuestConfirm kPacket;
	kPacket.header = HEADER_CG_QUEST_CONFIRM;
	kPacket.answer = byAnswer;
	kPacket.requestPID = dwPID;

	if (!Send(sizeof(kPacket), &kPacket))
	{
		Tracen("SendQuestConfirmPacket Error");
		return false;
	}

	Tracenf(" SendQuestConfirmPacket : %d, %d", byAnswer, dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::RecvSkillCoolTimeEnd()
{
	TPacketGCSkillCoolTimeEnd kPacketSkillCoolTimeEnd;
	if (!Recv(sizeof(kPacketSkillCoolTimeEnd), &kPacketSkillCoolTimeEnd))
	{
		Tracen("CPythonNetworkStream::RecvSkillCoolTimeEnd - RecvError");
		return false;
	}

	CPythonPlayer::Instance().EndSkillCoolTime(kPacketSkillCoolTimeEnd.bSkill);

	return true;
}

bool CPythonNetworkStream::RecvSkillLevel()
{
	assert(!"CPythonNetworkStream::RecvSkillLevel - Don't use this function");
	TPacketGCSkillLevel packet;
	if (!Recv(sizeof(TPacketGCSkillLevel), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevel - RecvError");
		return false;
	}

	DWORD dwSlotIndex;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		if (rkPlayer.GetSkillSlotIndex(i, &dwSlotIndex))
			rkPlayer.SetSkillLevel(dwSlotIndex, packet.abSkillLevels[i]);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	Tracef(" >> RecvSkillLevel\n");
	return true;
}

bool CPythonNetworkStream::RecvSkillLevelNew()
{
	TPacketGCSkillLevelNew packet;

	if (!Recv(sizeof(TPacketGCSkillLevelNew), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevelNew - RecvError");
		return false;
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();

	rkPlayer.SetSkill(7, 0);
	rkPlayer.SetSkill(8, 0);

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		TPlayerSkill & rPlayerSkill = packet.skills[i];

		if (i >= 112 && i <= 115 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(7, i);

		if (i >= 116 && i <= 119 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(8, i);

		rkPlayer.SetSkillLevel_(i, rPlayerSkill.bMasterType, rPlayerSkill.bLevel);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	return true;
}


bool CPythonNetworkStream::RecvDamageInfoPacket()
{
	TPacketGCDamageInfo DamageInfoPacket;

	if (!Recv(sizeof(TPacketGCDamageInfo), &DamageInfoPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(DamageInfoPacket.dwVictimVID);
	if (!pInstTarget)
		return true;

	bool bSelf = (pInstTarget == CPythonCharacterManager::Instance().GetMainInstancePtr());
	bool bTarget = (pInstTarget == m_pInstTarget);
#ifdef MULTIPLE_DAMAGE_DISPLAY_SYSTEM
	// Moblar için de çoklu damage gösterimi için: tüm karakterler için damage göster
	if (!bTarget)
		bTarget = true; // pInstTarget->IsPC(); // Eski: sadece PC'ler için
#endif

	if(DamageInfoPacket.damage >= 0)
		pInstTarget->AddDamageEffect(DamageInfoPacket.damage, DamageInfoPacket.flag, bSelf, bTarget, DamageInfoPacket.dwVictimVID, DamageInfoPacket.dwAttackerVID);
	else
		TraceError("Damage is equal or below 0.");

	return true;
}

bool CPythonNetworkStream::RecvTargetPacket()
{
	TPacketGCTarget TargetPacket;

	if (!Recv(sizeof(TPacketGCTarget), &TargetPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstPlayer = CPythonCharacterManager::Instance().GetMainInstancePtr();
	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(TargetPacket.dwVID);
	if (pInstPlayer && pInstTarget)
	{
		if (!pInstTarget->IsDead())
		{
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
			if (pInstTarget->IsBuilding())
#else
			if (pInstTarget->IsPC() || pInstTarget->IsBuilding())
#endif
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoardIfDifferent", Py_BuildValue("(i)", TargetPacket.dwVID));
			else if (pInstPlayer->CanViewTargetHP(*pInstTarget))
#ifdef ENABLE_VIEW_TARGET_DECIMAL_HP
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue("(iiii)", TargetPacket.dwVID, TargetPacket.bHPPercent, TargetPacket.iMinHP, TargetPacket.iMaxHP));
#else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue("(ii)", TargetPacket.dwVID, TargetPacket.bHPPercent));
#endif
			else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
			
			m_pInstTarget = pInstTarget;
		}
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
	}

	return true;
}

#ifdef ENABLE_MOB_DROP_INFO
bool CPythonNetworkStream::RecvTargetInfoPacket()
{
	TPacketGCTargetInfo pInfoTargetPacket;

	if (!Recv(sizeof(TPacketGCTargetInfo), &pInfoTargetPacket))
	{
		Tracen("Recv Info Target Packet Error");
		return false;
	}

	CInstanceBase* pInstPlayer = CPythonCharacterManager::Instance().GetMainInstancePtr();
	CInstanceBase* pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(pInfoTargetPacket.dwVID);
	if (pInstPlayer && pInstTarget)
	{
		if (!pInstTarget->IsDead())
		{
			if (pInstTarget->IsEnemy() || pInstTarget->IsStone())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AddTargetMonsterDropInfo",
					Py_BuildValue("(iii)", pInfoTargetPacket.race, pInfoTargetPacket.dwVnum, pInfoTargetPacket.count));
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshTargetMonsterDropInfo", Py_BuildValue("(i)", pInfoTargetPacket.race));
			}
			else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
		}
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
	}

	return true;
}
#endif

bool CPythonNetworkStream::RecvMountPacket()
{
	TPacketGCMount MountPacket;

	if (!Recv(sizeof(TPacketGCMount), &MountPacket))
	{
		Tracen("Recv Mount Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(MountPacket.vid);

	if (pInstance)
	{
		// Mount
		if (0 != MountPacket.mount_vid)
		{
//			pInstance->Ride(MountPacket.pos, MountPacket.mount_vid);
		}
		// Unmount
		else
		{
//			pInstance->Unride(MountPacket.pos, MountPacket.x, MountPacket.y);
		}
	}

	if (CPythonPlayer::Instance().IsMainCharacterIndex(MountPacket.vid))
	{
//		CPythonPlayer::Instance().SetRidingVehicleIndex(MountPacket.mount_vid);
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSpeedPacket()
{
	TPacketGCChangeSpeed SpeedPacket;

	if (!Recv(sizeof(TPacketGCChangeSpeed), &SpeedPacket))
	{
		Tracen("Recv Speed Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(SpeedPacket.vid);

	if (!pInstance)
		return true;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Recv

bool CPythonNetworkStream::SendAttackPacket(UINT uMotAttack, DWORD dwVIDVictim)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGAttack kPacketAtk;

	kPacketAtk.header = HEADER_CG_ATTACK;
	kPacketAtk.bType = uMotAttack;
	kPacketAtk.dwVictimVID = dwVIDVictim;

	if (!SendSpecial(sizeof(kPacketAtk), &kPacketAtk))
	{
		Tracen("Send Battle Attack Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSpecial(int nLen, void * pvBuf)
{
	BYTE bHeader = *(BYTE *) pvBuf;

	switch (bHeader)
	{
		case HEADER_CG_ATTACK:
			{
				TPacketCGAttack * pkPacketAtk = (TPacketCGAttack *) pvBuf;
				pkPacketAtk->bCRCMagicCubeProcPiece = GetProcessCRCMagicCubePiece();
				pkPacketAtk->bCRCMagicCubeFilePiece = GetProcessCRCMagicCubePiece();
				return Send(nLen, pvBuf);
			}
			break;
	}

	return Send(nLen, pvBuf);
}

bool CPythonNetworkStream::RecvAddFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	Tracef("VID [%d]°¡ Å¸°ÙÀ» Ãß°¡ ¼³Á¤\n",kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifndef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX,kPacket.lY) + 60.0f;
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(D3DXVECTOR3(kPacket.lX,kPacket.lY,h));
	}

	return true;
}

bool CPythonNetworkStream::RecvFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifdef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX, kPacket.lY) + 60.0f;
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(D3DXVECTOR3(kPacket.lX,kPacket.lY,h));
	}

	return true;
}

bool CPythonNetworkStream::SendShootPacket(UINT uSkill)
{
	TPacketCGShoot kPacketShoot;
	kPacketShoot.bHeader=HEADER_CG_SHOOT;
	kPacketShoot.bType=uSkill;

	if (!Send(sizeof(kPacketShoot), &kPacketShoot))
	{
		Tracen("SendShootPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	packet.bHeader	= HEADER_CG_ADD_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);
	
	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}


bool CPythonNetworkStream::SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	packet.bHeader	= HEADER_CG_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);
	
	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvCreateFlyPacket()
{
	TPacketGCCreateFly kPacket;
	if (!Recv(sizeof(TPacketGCCreateFly), &kPacket))
		return false;

	CFlyingManager& rkFlyMgr = CFlyingManager::Instance();
	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase * pkStartInst = rkChrMgr.GetInstancePtr(kPacket.dwStartVID);
	CInstanceBase * pkEndInst = rkChrMgr.GetInstancePtr(kPacket.dwEndVID);
	if (!pkStartInst || !pkEndInst)
		return true;

	rkFlyMgr.CreateIndexedFly(kPacket.bType, pkStartInst->GetGraphicThingInstancePtr(), pkEndInst->GetGraphicThingInstancePtr());

	return true;
}

bool CPythonNetworkStream::SendTargetPacket(DWORD dwVID)
{
	TPacketCGTarget packet;
	packet.header = HEADER_CG_TARGET;
	packet.dwVID = dwVID;

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send Target Packet Error");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
bool CPythonNetworkStream::SendSkillColorPacket(BYTE bSkillSlot, DWORD dwCol1, DWORD dwCol2, DWORD dwCol3, DWORD dwCol4, DWORD dwCol5)
{
	TPacketCGSkillColor CGPacket;
	CGPacket.bHeader = HEADER_CG_SKILL_COLOR;
	CGPacket.bSkillSlot = bSkillSlot;
	CGPacket.dwCol1 = dwCol1;
	CGPacket.dwCol2 = dwCol2;
	CGPacket.dwCol3 = dwCol3;
	CGPacket.dwCol4 = dwCol4;
	CGPacket.dwCol5 = dwCol5;

	if (!Send(sizeof(CGPacket), &CGPacket))
	{
		Tracen("Send Skill Color Packet Error");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY)
{
	TPacketCGSyncPositionElement kSyncPos;
	kSyncPos.dwVID=dwVictimVID;
	kSyncPos.lX=dwVictimX;
	kSyncPos.lY=dwVictimY;

	__LocalPositionToGlobalPosition(kSyncPos.lX, kSyncPos.lY);

	if (!Send(sizeof(kSyncPos), &kSyncPos))
	{
		Tracen("CPythonNetworkStream::SendSyncPositionElementPacket - ERROR");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvMessenger()
{
	TPacketGCMessenger p;
	if (!Recv(sizeof(p), &p))
		return false;

	int iSize = p.size - sizeof(p);
	char char_name[24+1];

	switch (p.subheader)
	{
		case MESSENGER_SUBHEADER_GC_LIST:
		{
			TPacketGCMessengerListOnline on;
			while(iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerListOffline),&on))
					return false;

				if (!Recv(on.length, char_name))
					return false;

				char_name[on.length] = 0;

				if (on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnFriendLogin(char_name);
				else
					CPythonMessenger::Instance().OnFriendLogout(char_name);

				iSize -= sizeof(TPacketGCMessengerListOffline);
				iSize -= on.length;
			}
			break;
		}

		case MESSENGER_SUBHEADER_GC_LOGIN:
		{
			TPacketGCMessengerLogin p;
			if (!Recv(sizeof(p),&p))
				return false;
			if (!Recv(p.length, char_name))
				return false;
			char_name[p.length] = 0;
			CPythonMessenger::Instance().OnFriendLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_LOGOUT:
		{
			TPacketGCMessengerLogout logout;
			if (!Recv(sizeof(logout),&logout))
				return false;
			if (!Recv(logout.length, char_name))
				return false;
			char_name[logout.length] = 0;
			CPythonMessenger::Instance().OnFriendLogout(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_REMOVE_FRIEND:
		{
			BYTE bLength;
			if (!Recv(sizeof(bLength), &bLength))
				return false;

			if (!Recv(bLength, char_name))
				return false;

			char_name[bLength] = 0;
			CPythonMessenger::Instance().RemoveFriend(char_name);
			break;
		}

#ifdef ENABLE_MESSENGER_TEAM
		case MESSENGER_SUBHEADER_GC_TEAM_LIST:
		{
			TPacketGCMessengerTeamListOnline on;
			while(iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerTeamListOffline),&on))
					return false;

				if (!Recv(on.length, char_name))
					return false;

				char_name[on.length] = 0;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				if (on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnTeamLoginWithLang(char_name, on.lang, on.lang2nd);
				else
					CPythonMessenger::Instance().OnTeamLogoutWithLang(char_name, on.lang, on.lang2nd);
#else
				if (on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnTeamLogin(char_name);
				else
					CPythonMessenger::Instance().OnTeamLogout(char_name);
#endif

				iSize -= sizeof(TPacketGCMessengerTeamListOffline);
				iSize -= on.length;
			}
			break;
		}
		
		case MESSENGER_SUBHEADER_GC_TEAM_LOGIN:
		{
			TPacketGCMessengerLogin p;
			if (!Recv(sizeof(p),&p))
				return false;
			if (!Recv(p.length, char_name))
				return false;
			char_name[p.length] = 0;
			CPythonMessenger::Instance().OnTeamLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_TEAM_LOGOUT:
		{
			TPacketGCMessengerLogout logout;
			if (!Recv(sizeof(logout),&logout))
				return false;
			if (!Recv(logout.length, char_name))
				return false;
			char_name[logout.length] = 0;
			CPythonMessenger::Instance().OnTeamLogout(char_name);
			break;
		}
#endif

#ifdef ENABLE_MESSENGER_BLOCK
		case MESSENGER_SUBHEADER_GC_BLOCK_LIST:
		{
			TPacketGCMessengerBlockListOnline block_on;
			while (iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerBlockListOffline), &block_on))
					return false;

				if (!Recv(block_on.length, char_name))
					return false;

				char_name[block_on.length] = 0;

				if (block_on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnBlockLogin(char_name);
				else
					CPythonMessenger::Instance().OnBlockLogout(char_name);

				iSize -= sizeof(TPacketGCMessengerBlockListOffline);
				iSize -= block_on.length;
			}
			break;
		}

		case MESSENGER_SUBHEADER_GC_BLOCK_LOGIN:
		{
			TPacketGCMessengerLogin block_p;
			if (!Recv(sizeof(block_p), &block_p))
				return false;
			if (!Recv(block_p.length, char_name))
				return false;
			char_name[block_p.length] = 0;
			CPythonMessenger::Instance().OnBlockLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT:
		{
			TPacketGCMessengerLogout block_logout;
			if (!Recv(sizeof(block_logout), &block_logout))
				return false;
			if (!Recv(block_logout.length, char_name))
				return false;
			char_name[block_logout.length] = 0;
			CPythonMessenger::Instance().OnBlockLogout(char_name);
			break;
		}
#endif
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Party

bool CPythonNetworkStream::SendPartyInvitePacket(DWORD dwVID)
{
	TPacketCGPartyInvite kPartyInvitePacket;
	kPartyInvitePacket.header = HEADER_CG_PARTY_INVITE;
	kPartyInvitePacket.vid = dwVID;

	if (!Send(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInvitePacket [%ud] - PACKET SEND ERROR", dwVID);
		return false;
	}

	Tracef(" << SendPartyInvitePacket : %d\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAnswer)
{
	TPacketCGPartyInviteAnswer kPartyInviteAnswerPacket;
	kPartyInviteAnswerPacket.header = HEADER_CG_PARTY_INVITE_ANSWER;
	kPartyInviteAnswerPacket.leader_pid = dwLeaderVID;
	kPartyInviteAnswerPacket.accept = byAnswer;

	if (!Send(sizeof(kPartyInviteAnswerPacket), &kPartyInviteAnswerPacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInviteAnswerPacket [%ud %ud] - PACKET SEND ERROR", dwLeaderVID, byAnswer);
		return false;
	}

	Tracef(" << SendPartyInviteAnswerPacket : %d, %d\n", dwLeaderVID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyRemovePacket(DWORD dwPID)
{
	TPacketCGPartyRemove kPartyInviteRemove;
	kPartyInviteRemove.header = HEADER_CG_PARTY_REMOVE;
	kPartyInviteRemove.pid = dwPID;

	if (!Send(sizeof(kPartyInviteRemove), &kPartyInviteRemove))
	{
		Tracenf("CPythonNetworkStream::SendPartyRemovePacket [%ud] - PACKET SEND ERROR", dwPID);
		return false;
	}

	Tracef(" << SendPartyRemovePacket : %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag)
{
	TPacketCGPartySetState kPartySetState;
	kPartySetState.byHeader = HEADER_CG_PARTY_SET_STATE;
	kPartySetState.dwVID = dwVID;
	kPartySetState.byState = byState;
	kPartySetState.byFlag = byFlag;

	if (!Send(sizeof(kPartySetState), &kPartySetState))
	{
		Tracenf("CPythonNetworkStream::SendPartySetStatePacket(%ud, %ud) - PACKET SEND ERROR", dwVID, byState);
		return false;
	}

	Tracef(" << SendPartySetStatePacket : %d, %d, %d\n", dwVID, byState, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID)
{
	TPacketCGPartyUseSkill kPartyUseSkill;
	kPartyUseSkill.byHeader = HEADER_CG_PARTY_USE_SKILL;
	kPartyUseSkill.bySkillIndex = bySkillIndex;
	kPartyUseSkill.dwTargetVID = dwVID;

	if (!Send(sizeof(kPartyUseSkill), &kPartyUseSkill))
	{
		Tracenf("CPythonNetworkStream::SendPartyUseSkillPacket(%ud, %ud) - PACKET SEND ERROR", bySkillIndex, dwVID);
		return false;
	}

	Tracef(" << SendPartyUseSkillPacket : %d, %d\n", bySkillIndex, dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyParameterPacket(BYTE byDistributeMode)
{
	TPacketCGPartyParameter kPartyParameter;
	kPartyParameter.bHeader = HEADER_CG_PARTY_PARAMETER;
	kPartyParameter.bDistributeMode = byDistributeMode;

	if (!Send(sizeof(kPartyParameter), &kPartyParameter))
	{
		Tracenf("CPythonNetworkStream::SendPartyParameterPacket(%d) - PACKET SEND ERROR", byDistributeMode);
		return false;
	}

	Tracef(" << SendPartyParameterPacket : %d\n", byDistributeMode);
	return SendSequence();
}

bool CPythonNetworkStream::RecvPartyInvite()
{
	TPacketGCPartyInvite kPartyInvitePacket;
	if (!Recv(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kPartyInvitePacket.leader_pid);
	if (!pInstance)
	{
		TraceError(" CPythonNetworkStream::RecvPartyInvite - Failed to find leader instance [%d]\n", kPartyInvitePacket.leader_pid);
		return true;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvPartyInviteQuestion", Py_BuildValue("(is)", kPartyInvitePacket.leader_pid, pInstance->GetNameString()));
	Tracef(" >> RecvPartyInvite : %d, %s\n", kPartyInvitePacket.leader_pid, pInstance->GetNameString());

	return true;
}

bool CPythonNetworkStream::RecvPartyAdd()
{
	TPacketGCPartyAdd kPartyAddPacket;
	if (!Recv(sizeof(kPartyAddPacket), &kPartyAddPacket))
		return false;

	CPythonPlayer::Instance().AppendPartyMember(kPartyAddPacket.pid, kPartyAddPacket.name);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddPartyMember", Py_BuildValue("(is)", kPartyAddPacket.pid, kPartyAddPacket.name));
	Tracef(" >> RecvPartyAdd : %d, %s\n", kPartyAddPacket.pid, kPartyAddPacket.name);

	return true;
}

bool CPythonNetworkStream::RecvPartyUpdate()
{
	TPacketGCPartyUpdate kPartyUpdatePacket;
	if (!Recv(sizeof(kPartyUpdatePacket), &kPartyUpdatePacket))
		return false;

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(kPartyUpdatePacket.pid, &pPartyMemberInfo))
		return true;

	BYTE byOldState = pPartyMemberInfo->byState;

	CPythonPlayer::Instance().UpdatePartyMemberInfo(kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);
	for (int i = 0; i < PARTY_AFFECT_SLOT_MAX_NUM; ++i)
	{
		CPythonPlayer::Instance().UpdatePartyMemberAffect(kPartyUpdatePacket.pid, i, kPartyUpdatePacket.affects[i]);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UpdatePartyMemberInfo", Py_BuildValue("(i)", kPartyUpdatePacket.pid));

	// ¸¸¾à ¸®´õ°¡ ¹Ù²î¾ú´Ù¸é, TargetBoard ÀÇ ¹öÆ°À» ¾÷µ¥ÀÌÆ® ÇÑ´Ù.
	DWORD dwVID;
	if (CPythonPlayer::Instance().PartyMemberPIDToVID(kPartyUpdatePacket.pid, &dwVID))
	if (byOldState != kPartyUpdatePacket.state)
	{
		__RefreshTargetBoardByVID(dwVID);
	}

// 	Tracef(" >> RecvPartyUpdate : %d, %d, %d\n", kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);

	return true;
}

bool CPythonNetworkStream::RecvPartyRemove()
{
	TPacketGCPartyRemove kPartyRemovePacket;
	if (!Recv(sizeof(kPartyRemovePacket), &kPartyRemovePacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RemovePartyMember", Py_BuildValue("(i)", kPartyRemovePacket.pid));
	Tracef(" >> RecvPartyRemove : %d\n", kPartyRemovePacket.pid);

	return true;
}

bool CPythonNetworkStream::RecvPartyLink()
{
	TPacketGCPartyLink kPartyLinkPacket;
	if (!Recv(sizeof(kPartyLinkPacket), &kPartyLinkPacket))
		return false;

	CPythonPlayer::Instance().LinkPartyMember(kPartyLinkPacket.pid, kPartyLinkPacket.vid);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LinkPartyMember", Py_BuildValue("(ii)", kPartyLinkPacket.pid, kPartyLinkPacket.vid));
	Tracef(" >> RecvPartyLink : %d, %d\n", kPartyLinkPacket.pid, kPartyLinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyUnlink()
{
	TPacketGCPartyUnlink kPartyUnlinkPacket;
	if (!Recv(sizeof(kPartyUnlinkPacket), &kPartyUnlinkPacket))
		return false;

	CPythonPlayer::Instance().UnlinkPartyMember(kPartyUnlinkPacket.pid);

	if (CPythonPlayer::Instance().IsMainCharacterIndex(kPartyUnlinkPacket.vid))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkAllPartyMember", Py_BuildValue("()"));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkPartyMember", Py_BuildValue("(i)", kPartyUnlinkPacket.pid));
	}

	Tracef(" >> RecvPartyUnlink : %d, %d\n", kPartyUnlinkPacket.pid, kPartyUnlinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyParameter()
{
	TPacketGCPartyParameter kPartyParameterPacket;
	if (!Recv(sizeof(kPartyParameterPacket), &kPartyParameterPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ChangePartyParameter", Py_BuildValue("(i)", kPartyParameterPacket.bDistributeMode));
	Tracef(" >> RecvPartyParameter : %d\n", kPartyParameterPacket.bDistributeMode);

	return true;
}

// Party
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Guild

bool CPythonNetworkStream::SendGuildAddMemberPacket(DWORD dwVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_ADD_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwVID), &dwVID))
		return false;

	Tracef(" SendGuildAddMemberPacket\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRemoveMemberPacket(DWORD dwPID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REMOVE_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwPID), &dwPID))
		return false;

	Tracef(" SendGuildRemoveMemberPacket %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char *c_szName)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;

	char szName[GUILD_GRADE_NAME_MAX_LEN+1];
	strncpy(szName, c_szName, GUILD_GRADE_NAME_MAX_LEN);
	szName[GUILD_GRADE_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;

	Tracef(" SendGuildChangeGradeNamePacket %d, %s\n", byGradeNumber, c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;
	if (!Send(sizeof(byAuthority), &byAuthority))
		return false;

	Tracef(" SendGuildChangeGradeAuthorityPacket %d, %d\n", byGradeNumber, byAuthority);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildOfferPacket(DWORD dwExperience)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_OFFER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwExperience), &dwExperience))
		return false;

	Tracef(" SendGuildOfferPacket %d\n", dwExperience);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildPostCommentPacket(const char *c_szMessage)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_POST_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	BYTE bySize = BYTE(strlen(c_szMessage)) + 1;
	if (!Send(sizeof(bySize), &bySize))
		return false;
	if (!Send(bySize, c_szMessage))
		return false;

	Tracef(" SendGuildPostCommentPacket %d, %s\n", bySize, c_szMessage);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDeleteCommentPacket(DWORD dwIndex)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DELETE_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwIndex), &dwIndex))
		return false;

	Tracef(" SendGuildDeleteCommentPacket %d\n", dwIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRefreshCommentsPacket(DWORD dwHighestIndex)
{
	static DWORD s_LastTime = timeGetTime() - 1001;

	if (timeGetTime() - s_LastTime < 1000)
		return true;
	s_LastTime = timeGetTime();

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	Tracef(" SendGuildRefreshCommentPacket %d\n", dwHighestIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byGrade), &byGrade))
		return false;

	Tracef(" SendGuildChangeMemberGradePacket %d, %d\n", dwPID, byGrade);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_USE_SKILL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwSkillID), &dwSkillID))
		return false;
	if (!Send(sizeof(dwTargetVID), &dwTargetVID))
		return false;

	Tracef(" SendGuildUseSkillPacket %d, %d\n", dwSkillID, dwTargetVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byFlag), &byFlag))
		return false;

	Tracef(" SendGuildChangeMemberGeneralFlagPacket %d, %d\n", dwPID, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwGuildID), &dwGuildID))
		return false;
	if (!Send(sizeof(byAnswer), &byAnswer))
		return false;

	Tracef(" SendGuildInviteAnswerPacket %d, %d\n", dwGuildID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChargeGSPPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHARGE_GSP;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildChargeGSPPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDepositMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DEPOSIT_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildDepositMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildWithdrawMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_WITHDRAW_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildWithdrawMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::RecvGuild()
{
	TPacketGCGuild GuildPacket;
	if (!Recv(sizeof(GuildPacket), &GuildPacket))
		return false;

	switch(GuildPacket.subheader)
	{
		case GUILD_SUBHEADER_GC_LOGIN:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			CPythonGuild::TGuildMemberData * pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LoginGuildMember(pGuildMemberData->strName.c_str());

			break;
		}
		case GUILD_SUBHEADER_GC_LOGOUT:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			CPythonGuild::TGuildMemberData * pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LogoutGuildMember(pGuildMemberData->strName.c_str());

			break;
		}
		case GUILD_SUBHEADER_GC_REMOVE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;

			if (CPythonGuild::Instance().IsMainPlayer(dwPID))
			{
				CPythonGuild::Instance().Destroy();
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "DeleteGuild", Py_BuildValue("()"));
				CPythonMessenger::Instance().RemoveAllGuildMember();
				__SetGuildID(0);
				__RefreshMessengerWindow();
				__RefreshTargetBoard();
				__RefreshCharacterWindow();
			}
			else
			{
				std::string strMemberName = "";
				CPythonGuild::TGuildMemberData * pData;
				if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pData))
				{
					strMemberName = pData->strName;
					CPythonMessenger::Instance().RemoveGuildMember(pData->strName.c_str());
				}

				CPythonGuild::Instance().RemoveMember(dwPID);

				__RefreshTargetBoardByName(strMemberName.c_str());
				__RefreshGuildWindowMemberPage();
			}

			Tracef(" <Remove> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LIST:
		{
			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			for (; iPacketSize > 0;)
			{
				TPacketGCGuildSubMember memberPacket;
				if (!Recv(sizeof(memberPacket), &memberPacket))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN+1] = "";
				if (memberPacket.byNameFlag)
				{
					if (!Recv(sizeof(szName), &szName))
						return false;

					iPacketSize -= CHARACTER_NAME_MAX_LEN+1;
				}
				else
				{
					CPythonGuild::TGuildMemberData * pMemberData;
					if (CPythonGuild::Instance().GetMemberDataPtrByPID(memberPacket.pid, &pMemberData))
					{
						strncpy(szName, pMemberData->strName.c_str(), CHARACTER_NAME_MAX_LEN);
					}
				}

				CPythonGuild::SGuildMemberData GuildMemberData;
				GuildMemberData.dwPID = memberPacket.pid;
				GuildMemberData.byGrade = memberPacket.byGrade;
				GuildMemberData.strName = szName;
				GuildMemberData.byJob = memberPacket.byJob;
				GuildMemberData.byLevel = memberPacket.byLevel;
				GuildMemberData.dwOffer = memberPacket.dwOffer;
				GuildMemberData.byGeneralFlag = memberPacket.byIsGeneral;
				CPythonGuild::Instance().RegisterMember(GuildMemberData);

				if (strcmp(szName, CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().AppendGuildMember(szName);

				__RefreshTargetBoardByName(szName);

				iPacketSize -= sizeof(memberPacket);
			}

			__RefreshGuildWindowInfoPage();
			__RefreshGuildWindowMemberPage();
			__RefreshMessengerWindow();
			__RefreshCharacterWindow();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			for (BYTE i = 0; i < byCount; ++ i)
			{
				BYTE byIndex;
				if (!Recv(sizeof(byCount), &byIndex))
					return false;
				TPacketGCGuildSubGrade GradePacket;
				if (!Recv(sizeof(GradePacket), &GradePacket))
					return false;

				auto data = CPythonGuild::SGuildGradeData(GradePacket.auth_flag, GradePacket.grade_name);
				CPythonGuild::Instance().SetGradeData(byIndex, data);
			}
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_NAME:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;

			char szGradeName[GUILD_GRADE_NAME_MAX_LEN+1] = "";
			if (!Recv(sizeof(szGradeName), &szGradeName))
				return false;

			CPythonGuild::Instance().SetGradeName(byGradeNumber, szGradeName);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Name> %d, %s\n", byGradeNumber, szGradeName);
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_AUTH:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;
			BYTE byAuthorityFlag;
			if (!Recv(sizeof(byAuthorityFlag), &byAuthorityFlag))
				return false;

			CPythonGuild::Instance().SetGradeAuthority(byGradeNumber, byAuthorityFlag);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Authority> %d, %d\n", byGradeNumber, byAuthorityFlag);
			__RefreshGuildWindowGradePage();
			break;
		}
		case GUILD_SUBHEADER_GC_INFO:
		{
			TPacketGCGuildInfo GuildInfo;
			if (!Recv(sizeof(GuildInfo), &GuildInfo))
				return false;

			CPythonGuild::Instance().EnableGuild();
			CPythonGuild::TGuildInfo & rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();
			strncpy(rGuildInfo.szGuildName, GuildInfo.name, GUILD_NAME_MAX_LEN);
			rGuildInfo.szGuildName[GUILD_NAME_MAX_LEN] = '\0';

			rGuildInfo.dwGuildID = GuildInfo.guild_id;
			rGuildInfo.dwMasterPID = GuildInfo.master_pid;
			rGuildInfo.dwGuildLevel = GuildInfo.level;
			rGuildInfo.dwCurrentExperience = GuildInfo.exp;
			rGuildInfo.dwCurrentMemberCount = GuildInfo.member_count;
			rGuildInfo.dwMaxMemberCount = GuildInfo.max_member_count;
			rGuildInfo.dwGuildMoney = GuildInfo.gold;
			rGuildInfo.bHasLand = GuildInfo.hasLand;

			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_COMMENTS:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			CPythonGuild::Instance().ClearComment();

			for (BYTE i = 0; i < byCount; ++i)
			{
				DWORD dwCommentID;
				if (!Recv(sizeof(dwCommentID), &dwCommentID))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN+1] = "";
				if (!Recv(sizeof(szName), &szName))
					return false;

				char szComment[GULID_COMMENT_MAX_LEN+1] = "";
				if (!Recv(sizeof(szComment), &szComment))
					return false;

				CPythonGuild::Instance().RegisterComment(dwCommentID, szName, szComment);
			}

			__RefreshGuildWindowBoardPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_EXP:
		{
			BYTE byLevel;
			if (!Recv(sizeof(byLevel), &byLevel))
				return false;
			DWORD dwEXP;
			if (!Recv(sizeof(dwEXP), &dwEXP))
				return false;
			CPythonGuild::Instance().SetGuildEXP(byLevel, dwEXP);
			Tracef(" <ChangeEXP> %d, %d\n", byLevel, dwEXP);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byGrade;
			if (!Recv(sizeof(byGrade), &byGrade))
				return false;
			CPythonGuild::Instance().ChangeGuildMemberGrade(dwPID, byGrade);
			Tracef(" <ChangeMemberGrade> %d, %d\n", dwPID, byGrade);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_SKILL_INFO:
		{
			CPythonGuild::TGuildSkillData & rSkillData = CPythonGuild::Instance().GetGuildSkillDataRef();
			if (!Recv(sizeof(rSkillData.bySkillPoint), &rSkillData.bySkillPoint))
				return false;
			if (!Recv(sizeof(rSkillData.bySkillLevel), rSkillData.bySkillLevel))
				return false;
			if (!Recv(sizeof(rSkillData.wGuildPoint), &rSkillData.wGuildPoint))
				return false;
			if (!Recv(sizeof(rSkillData.wMaxGuildPoint), &rSkillData.wMaxGuildPoint))
				return false;

			Tracef(" <SkillInfo> %d / %d, %d\n", rSkillData.bySkillPoint, rSkillData.wGuildPoint, rSkillData.wMaxGuildPoint);
			__RefreshGuildWindowSkillPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byFlag;
			if (!Recv(sizeof(byFlag), &byFlag))
				return false;

			CPythonGuild::Instance().ChangeGuildMemberGeneralFlag(dwPID, byFlag);
			Tracef(" <ChangeMemberGeneralFlag> %d, %d\n", dwPID, byFlag);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_INVITE:
		{
			DWORD dwGuildID;
			if (!Recv(sizeof(dwGuildID), &dwGuildID))
				return false;
			char szGuildName[GUILD_NAME_MAX_LEN+1];
			if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
				return false;

			szGuildName[GUILD_NAME_MAX_LEN] = 0;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvGuildInviteQuestion", Py_BuildValue("(is)", dwGuildID, szGuildName));
			Tracef(" <Guild Invite> %d, %s\n", dwGuildID, szGuildName);
			break;
		}
		case GUILD_SUBHEADER_GC_WAR:
		{
			TPacketGCGuildWar kGuildWar;
			if (!Recv(sizeof(kGuildWar), &kGuildWar))
				return false;

			switch (kGuildWar.bWarState)
			{
				case GUILD_WAR_SEND_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_SEND_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnSendDeclare",
						Py_BuildValue("(i)", kGuildWar.dwGuildOpp)
					);
					break;
				case GUILD_WAR_RECV_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_RECV_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnRecvDeclare",
						Py_BuildValue("(ii)", kGuildWar.dwGuildOpp, kGuildWar.bType)
					);
					break;
				case GUILD_WAR_ON_WAR:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_ON_WAR : %d, %d\n", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp);
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnStart",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().StartGuildWar(kGuildWar.dwGuildOpp);
					break;
				case GUILD_WAR_END:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_END\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnEnd",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().EndGuildWar(kGuildWar.dwGuildOpp);
					break;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_NAME:
		{
			DWORD dwID;
			char szGuildName[GUILD_NAME_MAX_LEN+1];

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			int nItemSize = sizeof(dwID) + GUILD_NAME_MAX_LEN;

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_NAME");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwID), &dwID))
					return false;

				if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
					return false;

				szGuildName[GUILD_NAME_MAX_LEN] = 0;

				CPythonGuild::Instance().RegisterGuildName(dwID, szGuildName);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_WAR_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::InsertGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarEndList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::RemoveGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_WAR_POINT:
		{
			TPacketGuildWarPoint GuildWarPoint;
			if (!Recv(sizeof(GuildWarPoint), &GuildWarPoint))
				return false;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"BINARY_GuildWar_OnRecvPoint",
				Py_BuildValue("(iii)", GuildWarPoint.dwGainGuildID, GuildWarPoint.dwOpponentGuildID, GuildWarPoint.lPoint)
			);
			break;
		}
		case GUILD_SUBHEADER_GC_MONEY_CHANGE:
		{
			DWORD dwMoney;
			if (!Recv(sizeof(dwMoney), &dwMoney))
				return false;

			CPythonGuild::Instance().SetGuildMoney(dwMoney);

			__RefreshGuildWindowInfoPage();
			Tracef(" >> Guild Money Change : %d\n", dwMoney);
			break;
		}
	}

	return true;
}

// Guild
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// Fishing

bool CPythonNetworkStream::SendFishingPacket(int iRotation)
{
	BYTE byHeader = HEADER_CG_FISHING;
	if (!Send(sizeof(byHeader), &byHeader))
		return false;
	BYTE byPacketRotation = iRotation / 5;
	if (!Send(sizeof(BYTE), &byPacketRotation))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount)
{
	TPacketCGGiveItem GiveItemPacket;
	GiveItemPacket.byHeader = HEADER_CG_GIVE_ITEM;
	GiveItemPacket.dwTargetVID = dwTargetVID;
	GiveItemPacket.ItemPos = ItemPos;
	GiveItemPacket.byItemCount = iItemCount;

	if (!Send(sizeof(GiveItemPacket), &GiveItemPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvFishing()
{
	TPacketGCFishing FishingPacket;
	if (!Recv(sizeof(FishingPacket), &FishingPacket))
		return false;

	CInstanceBase * pFishingInstance = nullptr;
	if (FISHING_SUBHEADER_GC_FISH != FishingPacket.subheader)
	{
		pFishingInstance = CPythonCharacterManager::Instance().GetInstancePtr(FishingPacket.info);
		if (!pFishingInstance)
			return true;
	}

	switch (FishingPacket.subheader)
	{
		case FISHING_SUBHEADER_GC_START:
			pFishingInstance->StartFishing(float(FishingPacket.dir) * 5.0f);
			break;
		case FISHING_SUBHEADER_GC_STOP:
			if (pFishingInstance->IsFishing())
				pFishingInstance->StopFishing();
			break;
		case FISHING_SUBHEADER_GC_REACT:
			if (pFishingInstance->IsFishing())
			{
				pFishingInstance->SetFishEmoticon(); // Fish Emoticon
				pFishingInstance->ReactFishing();
			}
			break;
		case FISHING_SUBHEADER_GC_SUCCESS:
			pFishingInstance->CatchSuccess();
			break;
		case FISHING_SUBHEADER_GC_FAIL:
			pFishingInstance->CatchFail();
			if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingFailure", Py_BuildValue("()"));
			}
			break;
		case FISHING_SUBHEADER_GC_FISH:
		{
			DWORD dwFishID = FishingPacket.info;

			if (0 == FishingPacket.info)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotifyUnknown", Py_BuildValue("()"));
				return true;
			}

			CItemData * pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(dwFishID, &pItemData))
				return true;

			CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
			if (!pMainInstance)
				return true;

			if (pMainInstance->IsFishing())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotify", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingSuccess", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			}
			break;
		}
	}

	return true;
}
// Fishing
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Dungeon
bool CPythonNetworkStream::RecvDungeon()
{
	TPacketGCDungeon DungeonPacket;
	if (!Recv(sizeof(DungeonPacket), &DungeonPacket))
		return false;

	switch (DungeonPacket.subheader)
	{
		case DUNGEON_SUBHEADER_GC_TIME_ATTACK_START:
		{
			break;
		}
		case DUNGEON_SUBHEADER_GC_DESTINATION_POSITION:
		{
			unsigned long ulx, uly;
			if (!Recv(sizeof(ulx), &ulx))
				return false;
			if (!Recv(sizeof(uly), &uly))
				return false;

			CPythonPlayer::Instance().SetDungeonDestinationPosition(ulx, uly);
			break;
		}
	}

	return true;
}
// Dungeon
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// MyShop
bool CPythonNetworkStream::SendBuildPrivateShopPacket(const char *c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock)
{
	TPacketCGMyShop packet;
	packet.bHeader = HEADER_CG_MYSHOP;
	strncpy(packet.szSign, c_szName, SHOP_SIGN_MAX_LEN);
	packet.bCount = static_cast<unsigned char>(c_rSellingItemStock.size());
	if (!Send(sizeof(packet), &packet))
		return false;

	for (std::vector<TShopItemTable>::const_iterator itor = c_rSellingItemStock.begin(); itor < c_rSellingItemStock.end(); ++itor)
	{
		const TShopItemTable & c_rItem = *itor;
		if (!Send(sizeof(c_rItem), &c_rItem))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvShopSignPacket()
{
	TPacketGCShopSign p;
	if (!Recv(sizeof(TPacketGCShopSign), &p))
		return false;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	
	if (0 == strlen(p.szSign))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], 
			"BINARY_PrivateShop_Disappear", 
			Py_BuildValue("(i)", p.dwVID)
		);

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.ClosePrivateShop();
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], 
			"BINARY_PrivateShop_Appear", 
			Py_BuildValue("(is)", p.dwVID, p.szSign)
		);

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.OpenPrivateShop();
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////

bool CPythonNetworkStream::RecvTimePacket()
{
	TPacketGCTime TimePacket;
	if (!Recv(sizeof(TimePacket), &TimePacket))
		return false;

	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();
	rkApp.SetServerTime(TimePacket.time);

	return true;
}

bool CPythonNetworkStream::RecvWalkModePacket()
{
	TPacketGCWalkMode WalkModePacket;
	if (!Recv(sizeof(WalkModePacket), &WalkModePacket))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(WalkModePacket.vid);
	if (pInstance)
	{
		if (WALKMODE_RUN == WalkModePacket.mode)
		{
			pInstance->SetRunMode();
		}
		else
		{
			pInstance->SetWalkMode();
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSkillGroupPacket()
{
	TPacketGCChangeSkillGroup ChangeSkillGroup;
	if (!Recv(sizeof(ChangeSkillGroup), &ChangeSkillGroup))
		return false;

	m_dwMainActorSkillGroup = ChangeSkillGroup.skill_group;

	CPythonPlayer::Instance().NEW_ClearSkillData(true);
	__RefreshCharacterWindow();
	return true;
}

void CPythonNetworkStream::__TEST_SetSkillGroupFake(int iIndex)
{
	m_dwMainActorSkillGroup = DWORD(iIndex);

	CPythonPlayer::Instance().NEW_ClearSkillData();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendRefinePacket(BYTE byPos, BYTE byType
#ifdef ENABLE_PITTY_REFINE
, bool bUseSealOfGod
#endif
)
{
	TPacketCGRefine kRefinePacket;
	kRefinePacket.header = HEADER_CG_REFINE;
	kRefinePacket.pos = byPos;
	kRefinePacket.type = byType;
#ifdef ENABLE_PITTY_REFINE
	kRefinePacket.seal_of_god = bUseSealOfGod;
#endif

	if (!Send(sizeof(kRefinePacket), &kRefinePacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSelectItemPacket(DWORD dwItemPos)
{
	TPacketCGScriptSelectItem kScriptSelectItem;
	kScriptSelectItem.header = HEADER_CG_SCRIPT_SELECT_ITEM;
	kScriptSelectItem.selection = dwItemPos;

	if (!Send(sizeof(kScriptSelectItem), &kScriptSelectItem))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRefineInformationPacket()
{
	TPacketGCRefineInformation kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
		return false;

	TRefineTable & rkRefineTable = kRefineInfoPacket.refine_table;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
		Py_BuildValue("(iiii)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.prob));

	for (int i = 0; i < rkRefineTable.material_count; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacket(pos=%d, result_vnum=%d, cost=%d, prob=%d)\n",
														kRefineInfoPacket.pos,
														kRefineInfoPacket.refine_table.result_vnum,
														rkRefineTable.cost,
														rkRefineTable.prob);
#endif

	return true;
}

bool CPythonNetworkStream::RecvRefineInformationPacketNew()
{
	TPacketGCRefineInformationNew kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
		return false;

	TRefineTable & rkRefineTable = kRefineInfoPacket.refine_table;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
		Py_BuildValue("(iiiii)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.prob,
			kRefineInfoPacket.type)
		);

	for (int i = 0; i < rkRefineTable.material_count; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacketNew(pos=%d, result_vnum=%d, cost=%d, prob=%d, type=%d)\n",
														kRefineInfoPacket.pos,
														kRefineInfoPacket.refine_table.result_vnum,
														rkRefineTable.cost,
														rkRefineTable.prob,
														kRefineInfoPacket.type);
#endif

	return true;
}

bool CPythonNetworkStream::RecvNPCList()
{
	TPacketGCNPCPosition kNPCPosition;
	if (!Recv(sizeof(kNPCPosition), &kNPCPosition))
		return false;

	assert(int(kNPCPosition.size)-sizeof(kNPCPosition) == kNPCPosition.count*sizeof(TNPCPosition) && "HEADER_GC_NPC_POSITION");

	CPythonMiniMap::Instance().ClearAtlasMarkInfo();

	for (int i = 0; i < kNPCPosition.count; ++i)
	{
		TNPCPosition NPCPosition;
		if (!Recv(sizeof(TNPCPosition), &NPCPosition))
			return false;

#ifdef ENABLE_ULTIMATE_REGEN
		const char* c_szName;
		CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
		rkNonPlayer.GetName(NPCPosition.mobVnum, &c_szName);

		CPythonMiniMap::Instance().RegisterAtlasMark(NPCPosition.bType, NPCPosition.mobVnum, c_szName, NPCPosition.x, NPCPosition.y, NPCPosition.regenTime);
#else
		CPythonMiniMap::Instance().RegisterAtlasMark(NPCPosition.bType, NPCPosition.name, NPCPosition.x, NPCPosition.y);
#endif
	}

	return true;
}

bool CPythonNetworkStream::__SendCRCReportPacket()
{
	return true;
}

bool CPythonNetworkStream::RecvAffectAddPacket()
{
	TPacketGCAffectAdd kAffectAdd;
	if (!Recv(sizeof(kAffectAdd), &kAffectAdd))
		return false;

	TPacketAffectElement & rkElement = kAffectAdd.elem;
	if (rkElement.bPointIdxApplyOn == POINT_ENERGY)
	{
		CPythonPlayer::instance().SetStatus (POINT_ENERGY_END_TIME, CPythonApplication::Instance().GetServerTimeStamp() + rkElement.lDuration);
		__RefreshStatus();
	}
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_AddAffect", Py_BuildValue("(iiii)", rkElement.dwType, rkElement.bPointIdxApplyOn, rkElement.lApplyValue, rkElement.lDuration));

	return true;
}

bool CPythonNetworkStream::RecvAffectRemovePacket()
{
	TPacketGCAffectRemove kAffectRemove;
	if (!Recv(sizeof(kAffectRemove), &kAffectRemove))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_RemoveAffect", Py_BuildValue("(ii)", kAffectRemove.dwType, kAffectRemove.bApplyOn));

	return true;
}

bool CPythonNetworkStream::RecvChannelPacket()
{
	TPacketGCChannel kChannelPacket;
	if (!Recv(sizeof(kChannelPacket), &kChannelPacket))
		return false;

#ifdef ENABLE_CHANGE_CHANNEL
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshChannel", Py_BuildValue("(i)", kChannelPacket.channel));
#endif

#ifdef ENABLE_ANTI_EXP
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetAntiExp", Py_BuildValue("(i)", kChannelPacket.anti_exp));
#endif

	return true;
}

bool CPythonNetworkStream::RecvViewEquipPacket()
{
	TPacketGCViewEquip kViewEquipPacket;
	if (!Recv(sizeof(kViewEquipPacket), &kViewEquipPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenEquipmentDialog", Py_BuildValue("(i)", kViewEquipPacket.dwVID));

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		TEquipmentItemSet & rItemSet = kViewEquipPacket.equips[i];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogItem", Py_BuildValue("(iiiii)", kViewEquipPacket.dwVID, i, rItemSet.vnum, rItemSet.count, rItemSet.dwTransmutationVnum));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogItem", Py_BuildValue("(iiii)", kViewEquipPacket.dwVID, i, rItemSet.vnum, rItemSet.count));
#endif

		for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; ++j)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogSocket", Py_BuildValue("(iiii)", kViewEquipPacket.dwVID, i, j, rItemSet.alSockets[j]));

		for (int k = 0; k < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++k)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogAttr", Py_BuildValue("(iiiii)", kViewEquipPacket.dwVID, i, k, rItemSet.aAttr[k].bType, rItemSet.aAttr[k].sValue));
#ifdef ENABLE_SUNGMA_SCORE
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogSungmaScore", Py_BuildValue("(iii)", kViewEquipPacket.dwVID, i, rItemSet.dwSungmaScore));
#endif
	}

	return true;
}

bool CPythonNetworkStream::RecvLandPacket()
{
	TPacketGCLandList kLandList;
	if (!Recv(sizeof(kLandList), &kLandList))
		return false;

	std::vector<DWORD> kVec_dwGuildID;

	CPythonMiniMap & rkMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkBG = CPythonBackground::Instance();
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();

	rkMiniMap.ClearGuildArea();
	rkBG.ClearGuildArea();

	int iPacketSize = (kLandList.size - sizeof(TPacketGCLandList));
	for (; iPacketSize > 0; iPacketSize-=sizeof(TLandPacketElement))
	{
		TLandPacketElement kElement;
		if (!Recv(sizeof(TLandPacketElement), &kElement))
			return false;

		rkMiniMap.RegisterGuildArea(kElement.dwID,
									kElement.dwGuildID,
									kElement.x,
									kElement.y,
									kElement.width,
									kElement.height);

		if (pMainInstance)
		if (kElement.dwGuildID == pMainInstance->GetGuildID())
		{
			rkBG.RegisterGuildArea(kElement.x,
								   kElement.y,
								   kElement.x+kElement.width,
								   kElement.y+kElement.height);
		}

		if (0 != kElement.dwGuildID)
			kVec_dwGuildID.push_back(kElement.dwGuildID);
	}

	__DownloadSymbol(kVec_dwGuildID);

	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacket()
{
	TPacketGCTargetCreate kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacketNew()
{
	TPacketGCTargetCreateNew kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	if (CREATE_TARGET_TYPE_LOCATION == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);
	}
	else
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect(kTargetCreate.lID, kTargetCreate.dwVID);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetUpdatePacket()
{
	TPacketGCTargetUpdate kTargetUpdate;
	if (!Recv(sizeof(kTargetUpdate), &kTargetUpdate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.UpdateTarget(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.CreateTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

	return true;
}

bool CPythonNetworkStream::RecvTargetDeletePacket()
{
	TPacketGCTargetDelete kTargetDelete;
	if (!Recv(sizeof(kTargetDelete), &kTargetDelete))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.DeleteTarget(kTargetDelete.lID);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.DeleteTargetEffect(kTargetDelete.lID);

	return true;
}

#ifdef ENABLE_CONQUEROR_LEVEL
bool CPythonNetworkStream::RecvSungmaAttrUpdatePacket()
{
	TPacketGCSungmaAttrUpdate kSungmaAttrUpdate;
	if (!Recv(sizeof(kSungmaAttrUpdate), &kSungmaAttrUpdate))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SungMaAttr", Py_BuildValue("(iiii)", kSungmaAttrUpdate.bSTR, kSungmaAttrUpdate.bHP, kSungmaAttrUpdate.bMOVE, kSungmaAttrUpdate.bIMMUNE));
#ifdef _DEBUG
	Tracef("RECV SUNGMA ATTR UPDATE : %d\n", kSungmaAttrUpdate.bSTR, kSungmaAttrUpdate.bHP, kSungmaAttrUpdate.bMOVE, kSungmaAttrUpdate.bIMMUNE);
#endif
	return true;
}
#endif

bool CPythonNetworkStream::RecvLoverInfoPacket()
{
	TPacketGCLoverInfo kLoverInfo;
	if (!Recv (sizeof (kLoverInfo), &kLoverInfo))
	{
		return false;
	}

	PyCallClassMemberFunc (m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_LoverInfo", Py_BuildValue ("(si)", kLoverInfo.szName, kLoverInfo.byLovePoint));
	#ifdef _DEBUG
	Tracef ("RECV LOVER INFO : %s, %d\n", kLoverInfo.szName, kLoverInfo.byLovePoint);
	#endif
	return true;
}

bool CPythonNetworkStream::RecvLovePointUpdatePacket()
{
	TPacketGCLovePointUpdate kLovePointUpdate;
	if (!Recv (sizeof (kLovePointUpdate), &kLovePointUpdate))
	{
		return false;
	}

	PyCallClassMemberFunc (m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateLovePoint", Py_BuildValue ("(i)", kLovePointUpdate.byLovePoint));
	#ifdef _DEBUG
	Tracef ("RECV LOVE POINT UPDATE : %d\n", kLovePointUpdate.byLovePoint);
	#endif
	return true;
}

bool CPythonNetworkStream::RecvDigMotionPacket()
{
	TPacketGCDigMotion kDigMotion;
	if (!Recv (sizeof (kDigMotion), &kDigMotion))
	{
		return false;
	}

	#ifdef _DEBUG
	Tracef (" Dig Motion [%d/%d]\n", kDigMotion.vid, kDigMotion.count);
	#endif

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	CInstanceBase* pkInstMain = rkChrMgr.GetInstancePtr (kDigMotion.vid);
	CInstanceBase* pkInstTarget = rkChrMgr.GetInstancePtr (kDigMotion.target_vid);
	if (NULL == pkInstMain)
	{
		return true;
	}

	if (pkInstTarget)
	{
		pkInstMain->NEW_LookAtDestInstance (*pkInstTarget);
	}

	for (int i = 0; i < kDigMotion.count; ++i)
	{
		pkInstMain->PushOnceMotion (CRaceMotionData::NAME_DIG);
	}

	return true;
}

bool CPythonNetworkStream::SendDragonSoulRefinePacket (BYTE bRefineType, TItemPos* pos)
{
	TPacketCGDragonSoulRefine pk;
	pk.header = HEADER_CG_DRAGON_SOUL_REFINE;
	pk.bSubType = bRefineType;
	memcpy (pk.ItemGrid, pos, sizeof (TItemPos) * DS_REFINE_WINDOW_MAX_NUM);
	if (!Send (sizeof (pk), &pk))
	{
		return false;
	}
	return true;
}

#ifdef ENABLE_DUNGEON_INFO
bool CPythonNetworkStream::RecvDungeonInfoPacket()
{

	TPacketGCDungeonInfoReceive KDungeonInfo;

	if (!Recv(sizeof(KDungeonInfo), &KDungeonInfo))
		return false;

	switch (KDungeonInfo.bSubType)
	{

	case DUNGEON_INFO_SUB_HEADER_OPEN_SEND:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_TABLE_DUNGEON_INFO_OPEN", Py_BuildValue("()"));
	}
	break;

	case DUNGEON_INFO_SUB_HEADER_DATES_CLEAR:
	{
		CPythonDungeonInfo::Instance().Clear();
	}
	break;

	case DUNGEON_INFO_SUB_HEADER_DATES_MISION_CLEAR:
	{
		CPythonDungeonInfo::Instance().ClearMision();

	}
	break;

	case DUNGEON_INFO_SUB_HEADER_DATES_SEND:
	{

		CPythonDungeonInfo::Instance().DateInfoList(KDungeonInfo.dateinfo);
	}
	break;

	case DUNGEON_INFO_SUB_HEADER_DATES_MISION_SEND:
	{
		CPythonDungeonInfo::Instance().DateInfoMisionList(KDungeonInfo.dateinfom);
	}

	case DUNGEON_INFO_SUB_HEADER_DATES_RANKING_CLEAR:
	{
		CPythonDungeonInfo::Instance().ClearRanking();
	}
	break;

	case DUNGEON_INFO_SUB_HEADER_DATES_RANKING_SEND:
	{

		CPythonDungeonInfo::Instance().DateInfoRankingList(KDungeonInfo.dateinfo_ranking);
	}
	break;
	case DUNGEON_INFO_SUB_HEADER_DATES_RANKING_LOAD:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_TABLE_DUNGEON_RANKING_LOAD", Py_BuildValue("()"));
	}
	break;
	case DUNGEON_INFO_SUB_HEADER_DATES_MISION_LOAD:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_TABLE_DUNGEON_MISION_LOAD", Py_BuildValue("()"));
	}
	break;
	}

	return true;
}
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
bool CPythonNetworkStream::SendChangeLanguagePacket(BYTE bLanguage)
{
	// E?er dil bilgisi zaten gönderildiyse tekrar gönderme
	if (m_bLanguageSet != FALSE)
		return true;

	// Game phase'e geçilmediyse gönderme (login s?ras?nda gönderilecek)
	if (m_strPhase != "Game")
		return true;

	TPacketChangeLanguage packet;
	packet.bHeader = HEADER_CG_CHANGE_LANGUAGE;
	packet.bLanguage = bLanguage;

	m_bLanguageSet = TRUE;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRequestChangeLanguage()
{
	TPacketChangeLanguage packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();

	if (!pkInstMain)
	{
		TraceError("CPythonNetworkStream::RecvRequestChangeLanguage - MainCharacter is NULL");
		return false;
	}

	pkInstMain->SetLanguage(packet.bLanguage);
	pkInstMain->Update();

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RequestChangeLanguage", Py_BuildValue("(i)", packet.bLanguage));

	return true;
}
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
bool CPythonNetworkStream::SendWhisperDetails(const char *name)
{
	TPacketCGWhisperDetails packet;
	packet.header = HEADER_CG_WHISPER_DETAILS;
	strncpy(packet.name, name, sizeof(packet.name) - 1);
	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvWhisperDetails()
{
	TPacketGCWhisperDetails packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecieveWhisperDetails", Py_BuildValue("(si)", packet.name, packet.bLanguage));
#else
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecieveWhisperDetails", Py_BuildValue("(s)", packet.name));
#endif

	return true;
}
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
bool CPythonNetworkStream::RecvSwitchbotPacket()
{
	TPacketGCSwitchbot pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	size_t packet_size = int(pack.size) - sizeof(TPacketGCSwitchbot);
	if (pack.subheader == SUBHEADER_GC_SWITCHBOT_UPDATE)
	{
		if (packet_size != sizeof(CPythonSwitchbot::TSwitchbotTable))
		{
			return false;
		}

		CPythonSwitchbot::TSwitchbotTable table;
		if (!Recv(sizeof(table), &table))
		{
			return false;
		}

		CPythonSwitchbot::Instance().Update(table);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotWindow", Py_BuildValue("()"));
	}
	else if (pack.subheader == SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM)
	{
		if (packet_size != sizeof(TSwitchbotUpdateItem))
		{
			return false;
		}

		TSwitchbotUpdateItem update;
		if (!Recv(sizeof(update), &update))
		{
			return false;
		}

		TItemPos pos(SWITCHBOT, update.slot);

		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
		rkPlayer.SetItemCount(pos, update.count);

		for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		{
			rkPlayer.SetItemMetinSocket(pos, i, update.alSockets[i]);

		}

		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		{
			rkPlayer.SetItemAttribute(pos, j, update.aAttr[j].bType, update.aAttr[j].sValue);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotItem", Py_BuildValue("(i)", update.slot));
		return true;
	}
	else if (pack.subheader == SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION)
	{
		CPythonSwitchbot::Instance().ClearAttributeMap();

		size_t table_size = sizeof(CPythonSwitchbot::TSwitchbottAttributeTable);
		while (packet_size >= table_size)
		{
			const int test = sizeof(CPythonSwitchbot::TSwitchbottAttributeTable);

			CPythonSwitchbot::TSwitchbottAttributeTable table;
			if (!Recv(table_size, &table))
			{
				return false;
			}

			CPythonSwitchbot::Instance().AddAttributeToMap(table);
			packet_size -= table_size;
		}
	}

	return true;
}

bool CPythonNetworkStream::SendSwitchbotStartPacket(BYTE slot, std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> alternatives)
{
	TPacketCGSwitchbot pack;
	pack.header = HEADER_CG_SWITCHBOT;
	pack.subheader = SUBHEADER_CG_SWITCHBOT_START;
	pack.size = sizeof(TPacketCGSwitchbot) + sizeof(CPythonSwitchbot::TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
	pack.slot = slot;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	for (const auto& it : alternatives)
	{
		if (!Send(sizeof(it), &it))
		{
			return false;
		}
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSwitchbotStopPacket(BYTE slot)
{
	TPacketCGSwitchbot pack;
	pack.header = HEADER_CG_SWITCHBOT;
	pack.subheader = SUBHEADER_CG_SWITCHBOT_STOP;
	pack.size = sizeof(TPacketCGSwitchbot);
	pack.slot = slot;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_RESP_SYSTEM
bool CPythonNetworkStream::SendRespFetchDropPacket(uint32_t mobVnum)
{
	if (!__CanActMainInstance())
		return false;

	TPacketCGRespHeader header{};
	header.header = HEADER_CG_RESP;
	header.subheader = RESP_CG_SUBHEADER_FETCH_DROP;

	if (!Send(sizeof(TPacketCGRespHeader), &header) ||
		!Send(sizeof(uint32_t), &mobVnum))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendRespFetchRespPacket(uint32_t mobVnum)
{
	if (!__CanActMainInstance())
		return false;

	TPacketCGRespHeader header{};
	header.header = HEADER_CG_RESP;
	header.subheader = RESP_CG_SUBHEADER_FETCH_RESP;

	if (!Send(sizeof(TPacketCGRespHeader), &header) ||
		!Send(sizeof(uint32_t), &mobVnum))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendRespTeleportPacket(size_t id)
{
	if (!__CanActMainInstance())
		return false;

	TPacketCGRespHeader header{};
	header.header = HEADER_CG_RESP;
	header.subheader = RESP_CG_SUBHEADER_TELEPORT;

	if (!Send(sizeof(TPacketCGRespHeader), &header) ||
		!Send(sizeof(size_t), &id))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRespPacket()
{
	TPacketGCRespHeader header{};
	if (!Recv(sizeof(TPacketGCRespHeader), &header))
		return false;

	switch (header.subheader)
	{
		case RESP_GC_SUBHEADER_DATA_RESP:
		case RESP_GC_SUBHEADER_DATA_DROP:
		{
			uint16_t itemCount = 0;
			if (!Recv(sizeof(uint16_t), &itemCount))
				return false;

			uint32_t mobVnum = 0;
			if (!Recv(sizeof(uint32_t), &mobVnum))
				return false;

			switch (header.subheader)
			{
				case RESP_GC_SUBHEADER_DATA_RESP:
				{
					PyObject* respDataList = PyList_New(itemCount);

					for (int i=0; i < itemCount; i++)
					{
						TPacketGCRespData pack{};
						if (!Recv(sizeof(TPacketGCRespData), &pack))
							return false;

						PyObject* positionTuple = PyTuple_New(2);
						PyTuple_SetItem(positionTuple, 0, Py_BuildValue("i", pack.x));
						PyTuple_SetItem(positionTuple, 1, Py_BuildValue("i", pack.y));

						PyObject* respDict = PyDict_New();

						PyDict_SetItem(respDict, Py_BuildValue("s", "resp"), Py_BuildValue("i", pack.nextRespTime));
						PyDict_SetItem(respDict, Py_BuildValue("s", "cord"), Py_BuildValue("O", positionTuple));
						PyDict_SetItem(respDict, Py_BuildValue("s", "time"), Py_BuildValue("i", pack.time));
						PyDict_SetItem(respDict, Py_BuildValue("s", "id"), Py_BuildValue("k", pack.id));

						PyList_SetItem(respDataList, i, respDict);
					}

					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMobRespData", Py_BuildValue("(lO)", mobVnum, respDataList));

					break;
				}

				case RESP_GC_SUBHEADER_DATA_DROP:
				{
					TPacketGCRespGold gold{};
					if (!Recv(sizeof(TPacketGCRespGold), &gold))
						return false;

					PyObject* dropDataList = PyList_New(itemCount + 1);

					PyObject* goldDropDict = PyDict_New();

					PyDict_SetItem(goldDropDict, Py_BuildValue("s", "vnum"), Py_BuildValue("i", 1));
					PyDict_SetItem(goldDropDict, Py_BuildValue("s", "count"), Py_BuildValue("i", gold.minGold));

					PyList_SetItem(dropDataList, 0, goldDropDict);

					for (int i = 0; i < itemCount; i++)
					{
						TPacketGCRespItem data{};
						if (!Recv(sizeof(TPacketGCRespItem), &data))
							return false;

						PyObject* dropDict = PyDict_New();

						PyDict_SetItem(dropDict, Py_BuildValue("s", "vnum"), Py_BuildValue("i", data.vnum));
						PyDict_SetItem(dropDict, Py_BuildValue("s", "count"), Py_BuildValue("i", data.minCount));

						PyList_SetItem(dropDataList, i + 1, dropDict);
					}

					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMobDropData", Py_BuildValue("(lO)", mobVnum, dropDataList));

					break;
				}
			}
			break;
		}
		case RESP_GC_SUBHEADER_DATA_MOB:
		{
			TPacketGCMapData pack{};
			if (!Recv(sizeof(TPacketGCMapData), &pack))
				return false;

			PyObject* mobDataList = PyList_New(pack.mobCount);

			for (int i = 0; i < pack.mobCount; i++)
			{
				uint32_t mobVnum = 0;
				if (!Recv(sizeof(uint32_t), &mobVnum))
					return false;

				PyList_SetItem(mobDataList, i, Py_BuildValue("l", mobVnum));
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMapData", Py_BuildValue("(Oiiii)", 
				mobDataList, pack.currentBossCount, pack.maxBossCount, pack.currentMetinCount, pack.maxMetinCount));
			break;
		}
		case RESP_GC_SUBHEADER_REFRESH_RESP:
		{
			TPacketGCRespRefresh pack{};
			if (!Recv(sizeof(TPacketGCRespRefresh), &pack))
				return false;

			PyObject* positionTuple = PyTuple_New(2);
			PyTuple_SetItem(positionTuple, 0, Py_BuildValue("i", pack.x));
			PyTuple_SetItem(positionTuple, 1, Py_BuildValue("i", pack.y));

PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshResp", Py_BuildValue("(kllO)", pack.id, pack.vnum, pack.nextRespTime, positionTuple));
			break;
		}
	}

	return true;
}
#endif

#ifdef ENABLE_RENEWAL_CUBE
bool CPythonNetworkStream::CubeRenewalMakeItem(int index_item, int count_item, int index_item_improve)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGCubeRenewalSend packet;

	packet.header = HEADER_CG_CUBE_RENEWAL;
	packet.subheader = CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM;
	packet.index_item = index_item;
	packet.count_item = count_item;
	packet.index_item_improve = index_item_improve;

	if (!Send(sizeof(TPacketCGCubeRenewalSend), &packet))
	{
		Tracef("CPythonNetworkStream::CubeRenewalMakeItem Error\n");
		return false;
	}

	return SendSequence();
}
bool CPythonNetworkStream::CubeRenewalClose()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGCubeRenewalSend packet;

	packet.header = HEADER_CG_CUBE_RENEWAL;
	packet.subheader = CUBE_RENEWAL_SUB_HEADER_CLOSE;

	if (!Send(sizeof(TPacketCGCubeRenewalSend), &packet))
	{
		Tracef("CPythonNetworkStream::CubeRenewalClose Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvCubeRenewalPacket()
{

	TPacketGCCubeRenewalReceive CubeRenewalPacket;

	if (!Recv(sizeof(CubeRenewalPacket), &CubeRenewalPacket))
		return false;

	switch (CubeRenewalPacket.subheader)
	{

		case CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_CUBE_RENEWAL_OPEN", Py_BuildValue("()"));
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE:
		{
			CPythonCubeRenewal::Instance().ReceiveList(CubeRenewalPacket.date_cube_renewal);
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_DATES_LOADING:
		{
			CPythonCubeRenewal::Instance().LoadingList();
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE:
		{
			CPythonCubeRenewal::Instance().ClearList();
		}
		break;
	}

	return true;
}
#endif

#ifdef ENABLE_DISCORD_RPC
static int64_t StartTime = 0;
void CPythonNetworkStream::Discord_Start()
{
	StartTime = time(0);
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize(Discord::DiscordClientID, &handlers, 1, nullptr);
	Discord_Update(false);
}

void CPythonNetworkStream::Discord_Update(const bool bInGame)
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.startTimestamp = StartTime;

	if (!bInGame)
	{
		Discord_UpdatePresence(&discordPresence);
		return;
	}

	auto NameData = Discord::GetNameData();
	discordPresence.state = std::get<0>(NameData).c_str();
	discordPresence.details = std::get<1>(NameData).c_str();

	auto RaceData = Discord::GetRaceData();
	discordPresence.largeImageKey = std::get<0>(RaceData).c_str();
	discordPresence.largeImageText = std::get<1>(RaceData).c_str();

	auto EmpireData = Discord::GetEmpireData();
	discordPresence.smallImageKey = std::get<0>(EmpireData).c_str();
	discordPresence.smallImageText = std::get<1>(EmpireData).c_str();

	Discord_UpdatePresence(&discordPresence);
}

void CPythonNetworkStream::Discord_Close()
{
	Discord_Shutdown();
}
#endif

#ifdef ENABLE_ITEMSHOP
bool CPythonNetworkStream::RecvItemShop()
{
	TPacketGCItemShop p;
	if (!Recv(sizeof(p), &p))
	{
#ifdef _DEBUG
		Tracenf("RecvItemShop -> cannot recv: TPacketGCItemShop.");
#endif
		return false;
	}

	BYTE subHeader;
	if (!Recv(sizeof(subHeader), &subHeader))
	{
#ifdef _DEBUG
		Tracenf("RecvItemShop -> cannot recv: subHeader.");
#endif
		return false;
	}

	switch (subHeader)
	{
		case ITEMSHOP_DRAGONCOIN:
		{
			long long act_lldCoins;
			if (!Recv(sizeof(act_lldCoins), &act_lldCoins))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: act_lldCoins (subHeader: %u).", subHeader);
#endif
				return false;
			}

#ifdef USE_ITEMSHOP_RENEWED
			long long act_lldJCoins;
			if (!Recv(sizeof(act_lldJCoins), &act_lldJCoins))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: act_lldJCoins (subHeader: %u).", subHeader);
#endif
				return false;
			}
#endif

			bool isLogOpen;
			if (!Recv(sizeof(isLogOpen), &isLogOpen))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: isLogOpen (subHeader: %u).", subHeader);
#endif
				return false;
			}

			if (isLogOpen)
			{
				TIShopLogData logData;
				if (!Recv(sizeof(logData), &logData))
				{
#ifdef _DEBUG
					Tracenf("RecvItemShop -> cannot recv: logData (subHeader: %u).", subHeader);
#endif
					return false;
				}

#ifdef USE_ITEMSHOP_RENEWED
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog",
										Py_BuildValue("(sissiiLL)",
										logData.buyDate,
										logData.buyTime,
										logData.playerName,
										logData.ipAdress,
										logData.itemVnum,
										logData.itemCount,
										logData.itemPrice,
										logData.itemPriceJD));
#else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog",
										Py_BuildValue("(sissiiL)",
										logData.buyDate,
										logData.buyTime,
										logData.playerName,
										logData.ipAdress,
										logData.itemVnum,
										logData.itemCount,
										logData.itemPrice));
#endif
			}

#ifdef USE_ITEMSHOP_RENEWED
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin",
									Py_BuildValue("(LL)",
									act_lldCoins,
									act_lldJCoins));
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin",
									Py_BuildValue("(L)",
									act_lldCoins));
#endif

			return true;
		}
		case ITEMSHOP_LOG:
		{
			int logCount;
			if (!Recv(sizeof(int), &logCount))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: logCount (subHeader: %u).", subHeader);
#endif
				return false;
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopHideLoading",
									Py_BuildValue("()"));

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopLogClear",
									Py_BuildValue("()"));

			if (logCount > 0)
			{
				std::vector<TIShopLogData> m_vec;
				m_vec.resize(logCount);

				if (!Recv(sizeof(TIShopLogData) * logCount, &m_vec[0]))
				{
#ifdef _DEBUG
					Tracenf("RecvItemShop -> cannot recv: m_vec (subHeader: %u).", subHeader);
#endif
					return false;
				}

				for (auto j = 0; j < m_vec.size(); ++j)
				{
#ifdef USE_ITEMSHOP_RENEWED
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog",
											Py_BuildValue("(sissiiLL)",
											m_vec[j].buyDate,
											m_vec[j].buyTime,
											m_vec[j].playerName,
											m_vec[j].ipAdress,
											m_vec[j].itemVnum,
											m_vec[j].itemCount,
											m_vec[j].itemPrice,
											m_vec[j].itemPriceJD));
#else
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog",
											Py_BuildValue("(sissiiL)",
											m_vec[j].buyDate,
											m_vec[j].buyTime,
											m_vec[j].playerName,
											m_vec[j].ipAdress,
											m_vec[j].itemVnum,
											m_vec[j].itemCount,
											m_vec[j].itemPrice));
#endif
				}
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopPurchasesWindow",
									Py_BuildValue("()"));

			return true;
		}
		case ITEMSHOP_UPDATE_ITEM:
		{
			TIShopData updateItem;
			if (!Recv(sizeof(updateItem), &updateItem))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: updateItem (subHeader: %u).", subHeader);
#endif
				return false;
			}

#ifdef USE_ITEMSHOP_RENEWED
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopUpdateItem",
									Py_BuildValue("(iiLiiiiLiL)",
									updateItem.id,
									updateItem.itemVnum,
									updateItem.itemPrice,
									updateItem.discount,
									updateItem.offerTime,
									updateItem.topSellingIndex,
									updateItem.addedTime,
									updateItem.sellCount,
									updateItem.maxSellCount,
									updateItem.itemPriceJD));
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopUpdateItem",
									Py_BuildValue("(iiLiiiiLi)",
									updateItem.id,
									updateItem.itemVnum,
									updateItem.itemPrice,
									updateItem.discount,
									updateItem.offerTime,
									updateItem.topSellingIndex,
									updateItem.addedTime,
									updateItem.sellCount,
									updateItem.maxSellCount));
#endif

			return true;
		}
		case ITEMSHOP_LOAD:
		{
			long long act_lldCoins;
			if (!Recv(sizeof(act_lldCoins), &act_lldCoins))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: act_lldCoins (subHeader: %u).", subHeader);
#endif
				return false;
			}

#ifdef USE_ITEMSHOP_RENEWED
			long long act_lldJCoins;
			if (!Recv(sizeof(act_lldJCoins), &act_lldJCoins))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: act_lldJCoins (subHeader: %u).", subHeader);
#endif
				return false;
			}
#endif

			int updateTime;
			if (!Recv(sizeof(updateTime), &updateTime))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: updateTime (subHeader: %u).", subHeader);
#endif
				return false;
			}

#ifdef USE_ITEMSHOP_RENEWED
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin",
									Py_BuildValue("(LL)",
									act_lldCoins,
									act_lldJCoins));
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin",
									Py_BuildValue("(L)",
									act_lldCoins));
#endif

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopHideLoading",
									Py_BuildValue("()"));

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopClear",
									Py_BuildValue("(i)", updateTime));

			int categoryTotalSize;
			if (!Recv(sizeof(categoryTotalSize), &categoryTotalSize))
			{
#ifdef _DEBUG
				Tracenf("RecvItemShop -> cannot recv: categoryTotalSize (subHeader: %u).", subHeader);
#endif
				return false;
			}

			if (categoryTotalSize == 9999)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopOpenMainPage",
										Py_BuildValue("()"));

				return true;
			}


			for (auto j = 0; j < categoryTotalSize; ++j)
			{
				BYTE categoryIndex;
				if (!Recv(sizeof(categoryIndex), &categoryIndex))
				{
#ifdef _DEBUG
					Tracenf("RecvItemShop -> cannot recv: categoryIndex (subHeader: %u).", subHeader);
#endif
					return false;
				}

				BYTE categorySize;
				if (!Recv(sizeof(categorySize), &categorySize))
				{
#ifdef _DEBUG
					Tracenf("RecvItemShop -> cannot recv: categorySize (subHeader: %u).", subHeader);
#endif
					return false;
				}

				for (auto x = 0; x < categorySize; ++x)
				{
					BYTE categorySubHeader;
					if (!Recv(sizeof(categorySubHeader), &categorySubHeader))
					{
#ifdef _DEBUG
						Tracenf("RecvItemShop -> cannot recv: categorySubHeader (subHeader: %u).", subHeader);
#endif
						return false;
					}

					BYTE categorySubSize;
					if (!Recv(sizeof(categorySubSize), &categorySubSize))
					{
#ifdef _DEBUG
						Tracenf("RecvItemShop -> cannot recv: categorySubSize (subHeader: %u).", subHeader);
#endif
						return false;
					}

					if (categorySubSize > 0)
					{
						for (auto k = 0; k < categorySubSize; ++k)
						{
							TIShopData shopData;
							if (!Recv(sizeof(shopData), &shopData))
							{
#ifdef _DEBUG
								Tracenf("RecvItemShop -> cannot recv: shopData (subHeader: %u).", subHeader);
#endif
								return false;
							}

#ifdef USE_ITEMSHOP_RENEWED
							PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendItem",
													Py_BuildValue("(iiiiLiiiiLiL)",
													categoryIndex,
													categorySubHeader,
													shopData.id,
													shopData.itemVnum,
													shopData.itemPrice,
													shopData.discount,
													shopData.offerTime,
													shopData.topSellingIndex,
													shopData.addedTime,
													shopData.sellCount,
													shopData.maxSellCount,
													shopData.itemPriceJD));
#else
							PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendItem",
													Py_BuildValue("(iiiiLiiiiLi)",
													categoryIndex,
													categorySubHeader,
													shopData.id,
													shopData.itemVnum,
													shopData.itemPrice,
													shopData.discount,
													shopData.offerTime,
													shopData.topSellingIndex,
													shopData.addedTime,
													shopData.sellCount,
													shopData.maxSellCount));
#endif
						}
					}
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopOpenMainPage",
										Py_BuildValue("()"));
			}

			return true;
		}
		default:
		{
#ifdef _DEBUG
			TraceError("RecvItemShop -> invalid subHeader: %u.", subHeader);
#endif
			return false;
		}
	}
}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
bool CPythonNetworkStream::RecvAccePacket(bool bReturn)
{
	TPacketAcce sPacket;
	if (!Recv(sizeof(sPacket), &sPacket))
		return bReturn;

	bReturn = true;
	switch (sPacket.subheader)
	{
		case ACCE_SUBHEADER_GC_OPEN:
			CPythonAcce::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 1, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_CLOSE:
			CPythonAcce::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 2, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_ADDED:
			CPythonAcce::Instance().AddMaterial(sPacket.dwPrice, sPacket.bPos, sPacket.tPos);
			if (sPacket.bPos == 1)
			{
				CPythonAcce::Instance().AddResult(sPacket.dwItemVnum, sPacket.dwMinAbs, sPacket.dwMaxAbs);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AlertAcce", Py_BuildValue("(b)", sPacket.bWindow));
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 3, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_REMOVED:
			CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.bPos);
			if (sPacket.bPos == 0)
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 4, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_CG_REFINED:
			if (sPacket.dwMaxAbs == 0)
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);
			else
			{
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 0);
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 4, sPacket.bWindow));
			break;

		default:
			TraceError("CPythonNetworkStream::RecvAccePacket: unknown subheader %d\n.", sPacket.subheader);
			break;
	}

	return bReturn;
}

bool CPythonNetworkStream::SendAcceClosePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_CG_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_CG_CLOSE;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceAddPacket(TItemPos tPos, BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketAcce sPacket;
	sPacket.header = HEADER_CG_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_CG_ADD;
	sPacket.dwPrice = 0;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRemovePacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_CG_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_CG_REMOVE;
	sPacket.dwPrice = 0;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefinePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_CG_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_CG_REFINE;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_VIEW_CHEST_DROP
bool CPythonNetworkStream::SendChestDropInfo(WORD wInventoryCell)
{
	TPacketCGChestDropInfo packet;
	packet.header = HEADER_CG_CHEST_DROP_INFO;
	packet.wInventoryCell = wInventoryCell;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvChestDropInfo()
{
	TPacketGCChestDropInfo packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	packet.wSize -= sizeof(packet);
	while (packet.wSize > 0)
	{
		TChestDropInfoTable kTab;
		if (!Recv(sizeof(kTab), &kTab))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AddChestDropInfo", Py_BuildValue("(iiiii)", packet.dwChestVnum, kTab.bPageIndex, kTab.bSlotIndex, kTab.dwItemVnum, kTab.bItemCount));

		packet.wSize -= sizeof(kTab);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshChestDropInfo", Py_BuildValue("(i)", packet.dwChestVnum));

	return true;
}
#endif

#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
bool CPythonNetworkStream::RecvPickupItemPacket()
{
	TPacketGCPickupItemSC PickupItemGC;

	if (!Recv(sizeof(TPacketGCPickupItemSC), &PickupItemGC))
	{
		Tracen("Recv PickupItemGC Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickItem", Py_BuildValue("(i)", PickupItemGC.item_vnum));

	return true;
}
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
#pragma warning (disable:4267)
bool CPythonNetworkStream::SendBuildOfflineShopPacket(const char *c_szName, const std::vector<TOfflineShopItemTable>& c_rSellingItemStock, DWORD shopVnum, BYTE shopTitle)
{
	TPacketCGMyOfflineShop packet;
	packet.bHeader = HEADER_CG_MY_OFFLINE_SHOP;
	strncpy(packet.szSign, c_szName, SHOP_SIGN_MAX_LEN);
	packet.bCount = c_rSellingItemStock.size();
	packet.shopVnum = shopVnum;
	packet.shopTitle = shopTitle;
	if (!Send(sizeof(packet), &packet))
		return false;

	for (std::vector<TOfflineShopItemTable>::const_iterator itor = c_rSellingItemStock.begin(); itor < c_rSellingItemStock.end(); ++itor)
	{
		const TOfflineShopItemTable& c_rItem = *itor;
		if (!Send(sizeof(c_rItem), &c_rItem))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvOfflineShopSignPacket()
{
	TPacketGCShopSign p;
	if (!Recv(sizeof(TPacketGCShopSign), &p))
		return false;
	CInstanceBase* pInstance = CPythonCharacterManager::instance().GetInstancePtr(p.dwVID);
	if (pInstance)
	{
		if (pInstance->IsShop())
		{
			if (0 == strlen(p.szSign))
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OfflineShop_Disappear", Py_BuildValue("(i)", p.dwVID));
			else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OfflineShop_Appear", Py_BuildValue("(is)", p.dwVID, p.szSign));
		}
	}
	return true;
}

bool CPythonNetworkStream::RecvOfflineShopPacket()
{
	TPacketGCShop  packet_shop;
	if (!Recv(sizeof(packet_shop), &packet_shop))
		return false;

	std::vector<char> vecBuffer;
	vecBuffer.clear();
	int iSize = packet_shop.size - sizeof(packet_shop);

	switch (packet_shop.subheader)
	{
		case SHOP_SUBHEADER_GC_REFRESH_MONEY:
		case SHOP_SUBHEADER_GC_REFRESH_DISPLAY_COUNT:
		case SHOP_SUBHEADER_GC_CHECK_RESULT:
		case SHOP_SUBHEADER_GC_REFRESH_COUNT:
		case SHOP_SUBHEADER_GC_REALWATCHER_COUNT:
		case SHOP_SUBHEADER_GC_CHANGE_TITLE:
			break;
		default:
			if (iSize > 0)
			{
				vecBuffer.resize(iSize);
				if (!Recv(iSize, &vecBuffer[0]))
					return false;
			}
	}

	switch (packet_shop.subheader)
	{
		case SHOP_SUBHEADER_GC_START:
		{
			CPythonShop::Instance().Clear();
			TPacketGCOfflineShopStart* pShopStartPacket = (TPacketGCOfflineShopStart*)vecBuffer.data();
			for (BYTE iItemIndex = 0; iItemIndex < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
				CPythonShop::Instance().SetOfflineShopItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);
			CPythonShop::Instance().SetShopDisplayedCount(pShopStartPacket->m_dwDisplayedCount);
			CPythonShop::Instance().SetRealWatcherCount(pShopStartPacket->m_dwRealWatcherCount);
			CPythonShop::Instance().SetCurrentOfflineShopMoney(pShopStartPacket->price);
			CPythonShop::Instance().SetShopSign(pShopStartPacket->title);
			CPythonShop::Instance().SetShopFlag(pShopStartPacket->flag);
			CPythonShop::Instance().SetShopType(pShopStartPacket->type);
			CPythonShop::Instance().SetShopTime(pShopStartPacket->time);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClearOfflineShopLog", Py_BuildValue("()"));
			for (int logIndex = OFFLINE_SHOP_HOST_ITEM_MAX_NUM-1; logIndex >= 0; --logIndex)
				if (pShopStartPacket->log[logIndex].price != 0) {
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendShopLog", Py_BuildValue("(ssiiL)", pShopStartPacket->log[logIndex].name, pShopStartPacket->log[logIndex].date, pShopStartPacket->log[logIndex].itemVnum, pShopStartPacket->log[logIndex].itemCount, pShopStartPacket->log[logIndex].price));
				}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartOfflineShop", Py_BuildValue("(ii)", pShopStartPacket->owner_vid, pShopStartPacket->IsOwner == true ? 1 : 0));
		}
		break;

		case SHOP_SUBHEADER_GC_END:
		{
			CPythonShop::instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClearOfflineShopLog", Py_BuildValue("()"));
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndOfflineShop", Py_BuildValue("()"));
		}
		break;

		case SHOP_SUBHEADER_GC_UPDATE_ITEM:
		{
			TPacketGCOfflineShopUpdateItem* pShopUpdateItemPacket = (TPacketGCOfflineShopUpdateItem*)vecBuffer.data();
			CPythonShop::Instance().SetShopDisplayedCount(pShopUpdateItemPacket->m_dwDisplayedCount);
			CPythonShop::Instance().SetRealWatcherCount(pShopUpdateItemPacket->m_dwRealWatcherCount);
			CPythonShop::Instance().SetShopFlag(pShopUpdateItemPacket->flag);
			CPythonShop::Instance().SetShopTime(pShopUpdateItemPacket->time);
			CPythonShop::Instance().SetShopSign(pShopUpdateItemPacket->title);
			CPythonShop::Instance().SetShopType(pShopUpdateItemPacket->type);
			CPythonShop::Instance().SetOfflineShopItemData(pShopUpdateItemPacket->pos, pShopUpdateItemPacket->item);
			CPythonShop::Instance().SetCurrentOfflineShopMoney(pShopUpdateItemPacket->price);
			if (pShopUpdateItemPacket->log.price != 0) {
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendShopLogFirst", Py_BuildValue("(ssiiL)", pShopUpdateItemPacket->log.name, pShopUpdateItemPacket->log.date, pShopUpdateItemPacket->log.itemVnum, pShopUpdateItemPacket->log.itemCount, pShopUpdateItemPacket->log.price));
			}
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshOfflineShop", Py_BuildValue("()"));
		}
		break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY"));
			break;

		case SHOP_SUBHEADER_GC_SOLD_OUT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "SOLDOUT"));
			break;

		case SHOP_SUBHEADER_GC_OK:
			break;

		case SHOP_SUBHEADER_GC_INVENTORY_FULL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVENTORY_FULL"));
			break;

		case SHOP_SUBHEADER_GC_INVALID_POS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVALID_POS"));
			break;

		case SHOP_SUBHEADER_GC_REFRESH_MONEY:
		{
			long long llMoney = 0;
			if (!Recv(sizeof(long long), &llMoney))
				return false;
			CPythonShop::Instance().SetCurrentOfflineShopMoney(llMoney);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshOfflineShop", Py_BuildValue("()"));
		}
		break;

		case SHOP_SUBHEADER_GC_CHECK_RESULT:
		{
			BYTE bHasOfflineShop = 0;
			if (!Recv(sizeof(BYTE), &bHasOfflineShop))
				return false;
			CPythonShop::instance().SetHasOfflineShop(bHasOfflineShop == 1 ? true : false);
		}
		break;

		case SHOP_SUBHEADER_GC_REFRESH_DISPLAY_COUNT:
		{
			DWORD displayCount = 0;
			if (!Recv(sizeof(DWORD), &displayCount))
				return false;
			CPythonShop::Instance().SetShopDisplayedCount(displayCount);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshOfflineShop", Py_BuildValue("()"));
		}
		break;
		case SHOP_SUBHEADER_GC_REALWATCHER_COUNT:
		{
			DWORD realWatcher = 0;
			if (!Recv(sizeof(DWORD), &realWatcher))
				return false;
			CPythonShop::Instance().SetRealWatcherCount(realWatcher);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshOfflineShop", Py_BuildValue("()"));
		}
		break;

		case SHOP_SUBHEADER_GC_REFRESH_COUNT:
		{
			DWORD count[2] = { 0,0 };
			if (!Recv(sizeof(count), &count))
				return false;
			CPythonShop::Instance().SetShopDisplayedCount(count[0]);
			CPythonShop::Instance().SetRealWatcherCount(count[1]);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshOfflineShop", Py_BuildValue("()"));
		}
		break;

		case SHOP_SUBHEADER_GC_CHANGE_TITLE:
		{
			char sign[SHOP_SIGN_MAX_LEN + 1];
			if (!Recv(sizeof(sign), &sign))
				return false;
			CPythonShop::Instance().SetShopSign(sign);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshOfflineShop", Py_BuildValue("()"));
		}
		break;
		default:
			TraceError("CPythonNetworkStream::RecvOfflineShopPacket: Unknown subheader %d\n", packet_shop.subheader);
			break;
	}

	return true;
}
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
bool CPythonNetworkStream::SendPrivateShopSearchInfo(TPacketCGShopSearch* p)
{
	if (!__CanActMainInstance())
		return true;

	if (!Send(sizeof(TPacketCGShopSearch), p))
		return false;

	return true;
}

bool CPythonNetworkStream::RecvShopSearchSet()
{
	TPacketGCShopSearchItemSet packet_item_set;
	if (!Recv(sizeof(TPacketGCShopSearchItemSet), &packet_item_set))
		return false;

	WORD itemCount;
	if (!Recv(sizeof(itemCount), &itemCount))
		return false;

	std::vector<TOfflineShopItemData> m_vec;
	m_vec.resize(itemCount);
	if (!Recv(itemCount * sizeof(TOfflineShopItemData), m_vec.data()))
		return false;

	for (WORD j = 0; j < itemCount; j++)
		CPythonPrivateShopSearch::Instance().AddItemData(m_vec[j]);

	__RefreshShopSearchWindow();

	return true;
}
#endif

#ifdef ENABLE_CLIENT_LOCALE_STRING
bool CPythonNetworkStream::RecvLocaleChatPacket()
{
	TPacketGCLocaleChat kLocaleChatPacket;
	if (!Recv(sizeof(kLocaleChatPacket), &kLocaleChatPacket))
	{
		TraceError("cannot recv TPacketGCLocaleChat");
		return false;
	}

	WORD size = kLocaleChatPacket.size - sizeof(kLocaleChatPacket);
	char buf[256];

	if (size > 0)
	{
		if (!Recv(size, buf))
		{
			TraceError("cannot recv TPacketGCLocaleChat size %d", size);
			return false;
		}
	}
	else
		strncpy_s(buf, sizeof(buf), "", _TRUNCATE);

	buf[size] = '\0';

	if (kLocaleChatPacket.type != CHAT_TYPE_INFO && kLocaleChatPacket.type != CHAT_TYPE_NOTICE && kLocaleChatPacket.type != CHAT_TYPE_BIG_NOTICE
#ifdef ENABLE_DICE_SYSTEM
		 && kLocaleChatPacket.type != CHAT_TYPE_DICE_INFO
#endif
#ifdef ENABLE_RENEWAL_OX_EVENT
		 && kLocaleChatPacket.type != CHAT_TYPE_CONTROL_NOTICE
#endif
	)
		return true;

	std::string tLocaleChatPacket = CPythonNetworkStream::Instance().GetLocaleStringVnum(kLocaleChatPacket.id);
	if (!tLocaleChatPacket.empty())
	{
		if (size > 0)
		{
			char *token = strtok(buf, "#");
			while (token != nullptr)
			{
				if (tLocaleChatPacket.find("%s") != std::string::npos)
				{
					tLocaleChatPacket.replace(tLocaleChatPacket.find("%s"), 2, token);
					token = strtok(nullptr, "#");
				}
				else
				{
					TraceError("CPythonNetworkStream::RecvLocaleChatPacket not all the arguments converted for id (%u).", kLocaleChatPacket.id);
					break;
				}
			}
		}

		if (CHAT_TYPE_NOTICE == kLocaleChatPacket.type)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", tLocaleChatPacket.c_str()));
		}
		else if (CHAT_TYPE_BIG_NOTICE == kLocaleChatPacket.type)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", tLocaleChatPacket.c_str()));
		}
#ifdef ENABLE_RENEWAL_OX_EVENT
		else if (CHAT_TYPE_CONTROL_NOTICE == kLocaleChatPacket.type)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", tLocaleChatPacket.c_str()));
		}
#endif

		CPythonChat::Instance().AppendChat(kLocaleChatPacket.type, tLocaleChatPacket.c_str());
	}

#ifdef _DEBUG
	Tracef("RecvLocaleChatPacket: type (%d), id (%u), size (%u), args (%s)\n", kLocaleChatPacket.type, kLocaleChatPacket.id, kLocaleChatPacket.size, buf);
#endif
	return true;
}

bool CPythonNetworkStream::RecvLocaleWhishperPacket()
{
	TPacketGCLocaleWhisper kLocaleWhisperPacket;
	if (!Recv(sizeof(kLocaleWhisperPacket), &kLocaleWhisperPacket))
	{
		TraceError("cannot recv TPacketGCLocaleWhisper");
		return false;
	}

	WORD size = kLocaleWhisperPacket.size - sizeof(kLocaleWhisperPacket);
	char buf[256];

	if (size > 0)
	{
		if (!Recv(size, buf))
		{
			TraceError("cannot recv TPacketGCLocaleWhisper size %d", size);
			return false;
		}
	}
	else
		strncpy_s(buf, sizeof(buf), "", _TRUNCATE);

	buf[size] = '\0';

	std::string tLocaleWhisperPacket = CPythonNetworkStream::Instance().GetLocaleStringVnum(kLocaleWhisperPacket.id);
	if (!tLocaleWhisperPacket.empty())
	{
		if (size > 0)
		{
			char* token = strtok(buf, "#");
			while (token != nullptr)
			{
				if (tLocaleWhisperPacket.find("%s") != std::string::npos)
				{
					tLocaleWhisperPacket.replace(tLocaleWhisperPacket.find("%s"), 2, token);
					token = strtok(nullptr, "#");
				}
				else
				{
					TraceError("CPythonNetworkStream::RecvLocaleWhishperPacket not all the arguments converted for id (%u).", kLocaleWhisperPacket.id);
					break;
				}
			}
		}
	
		static char line[256];
		if (CPythonChat::WHISPER_TYPE_CHAT == kLocaleWhisperPacket.type || CPythonChat::WHISPER_TYPE_GM == kLocaleWhisperPacket.type
#ifdef ENABLE_OFFLINE_MESSAGE
			|| CPythonChat::WHISPER_TYPE_OFFLINE == kLocaleWhisperPacket.type
#endif
			)
		{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			_snprintf(line, sizeof(line), "%s : %s", kLocaleWhisperPacket.namefrom, FilterChat(tLocaleWhisperPacket.c_str()));
#else
			_snprintf(line, sizeof(line), "%s : %s", kLocaleWhisperPacket.namefrom, tLocaleWhisperPacket.c_str());
#endif
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(iss)", (int)kLocaleWhisperPacket.type, kLocaleWhisperPacket.namefrom, line));
		}
		else if (CPythonChat::WHISPER_TYPE_SYSTEM == kLocaleWhisperPacket.type || CPythonChat::WHISPER_TYPE_ERROR == kLocaleWhisperPacket.type)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperSystemMessage", Py_BuildValue("(iss)", (int)kLocaleWhisperPacket.type, kLocaleWhisperPacket.namefrom, tLocaleWhisperPacket.c_str()));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperError", Py_BuildValue("(iss)", (int)kLocaleWhisperPacket.type, kLocaleWhisperPacket.namefrom, tLocaleWhisperPacket.c_str()));
		}
	}

#ifdef _DEBUG
	Tracef("RecvLocaleWhishperPacket: type (%d), id (%u), size (%u), args (%s)\n", kLocaleWhisperPacket.type, kLocaleWhisperPacket.id, kLocaleWhisperPacket.size, buf);
#endif
	return true;
}
#endif

#ifdef ENABLE_RENEWAL_OX_EVENT
bool CPythonNetworkStream::SendQuestInputStringLongPacket(const char *c_szString)
{
	TPacketCGQuestInputLongString Packet;
	Packet.bHeader = HEADER_CG_QUEST_INPUT_LONG_STRING;
	strncpy(Packet.szString, c_szString, QUEST_INPUT_LONG_STRING_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendQuestInputLongStringPacket Error");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_COLLECT_WINDOW
bool CPythonNetworkStream::RecvCollectPacket()
{
	TPacketGCCollectWindow kCollectWindow;

	if (!Recv(sizeof(kCollectWindow), &kCollectWindow))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateCollectWindow", Py_BuildValue("(iiiiiiiii)", kCollectWindow.bWindowType, kCollectWindow.dwTime, kCollectWindow.bCount, kCollectWindow.dwItemVnum, kCollectWindow.bCountTotal, kCollectWindow.bChance, kCollectWindow.bRenderTargetID, kCollectWindow.bQuestIndex, kCollectWindow.RequiredLevel));

	return true;
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
bool CPythonNetworkStream::RecvAuraPacket()
{
	TPacketGCAura kAuraPacket;
	if (!Recv(sizeof(TPacketGCAura), &kAuraPacket))
	{
		Tracef("Aura Packet Error SubHeader %u\n", kAuraPacket.bSubHeader);
		return false;
	}

	int iPacketSize = int(kAuraPacket.wSize) - sizeof(TPacketGCAura);

	switch (kAuraPacket.bSubHeader)
	{
		case AURA_SUBHEADER_GC_OPEN:
		case AURA_SUBHEADER_GC_CLOSE:
		{
			if (iPacketSize > 0)
			{
				TSubPacketGCAuraOpenClose kSubPacket;
				assert(iPacketSize % sizeof(TSubPacketGCAuraOpenClose) == 0 && "AURA_SUBHEADER_GC_OPENCLOSE");
				if (!Recv(sizeof(TSubPacketGCAuraOpenClose), &kSubPacket))
					return false;

				if (kAuraPacket.bSubHeader == AURA_SUBHEADER_GC_OPEN)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AuraWindowOpen", Py_BuildValue("(i)", kSubPacket.bAuraWindowType));

				else if (kAuraPacket.bSubHeader == AURA_SUBHEADER_GC_CLOSE)
				{
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AuraWindowClose", Py_BuildValue("()"));
					CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_MAIN);
					CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_SUB);
					CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_RESULT);
				}

				CPythonPlayer::instance().SetAuraRefineWindowOpen(kSubPacket.bAuraWindowType);
			}
			else
				TraceError(" RecvAuraPacket Error 0x04100%u0F", kAuraPacket.bSubHeader);

			break;
		}
		case AURA_SUBHEADER_GC_SET_ITEM:
		{
			if (iPacketSize > 0)
			{
				TSubPacketGCAuraSetItem kSubPacket;
				assert(iPacketSize % sizeof(TSubPacketGCAuraSetItem) == 0 && "AURA_SUBHEADER_GC_SET_ITEM");
				if (!Recv(sizeof(TSubPacketGCAuraSetItem), &kSubPacket))
					return false;

				TItemData kItemData;
				kItemData.vnum = kSubPacket.pItem.vnum;
				kItemData.count = kSubPacket.pItem.count;
				for (int iSocket = 0; iSocket < ITEM_SOCKET_SLOT_MAX_NUM; ++iSocket)
					kItemData.alSockets[iSocket] = kSubPacket.pItem.alSockets[iSocket];
				for (int iAttr = 0; iAttr < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iAttr)
					kItemData.aAttr[iAttr] = kSubPacket.pItem.aAttr[iAttr];

				if (kSubPacket.Cell.IsValidCell() && !kSubPacket.Cell.IsEquipCell())
					CPythonPlayer::instance().SetActivatedAuraSlot(BYTE(kSubPacket.AuraCell.cell), kSubPacket.Cell);

				CPythonPlayer::instance().SetAuraItemData(BYTE(kSubPacket.AuraCell.cell), kItemData);
			}
			else
				TraceError(" RecvAuraPacket Error 0x040%uBABE", kAuraPacket.bSubHeader);

			break;
		}
		case AURA_SUBHEADER_GC_CLEAR_SLOT:
		{
			if (iPacketSize > 0)
			{
				TSubPacketGCAuraClearSlot kSubPacket;
				assert(iPacketSize % sizeof(TSubPacketGCAuraClearSlot) == 0 && "AURA_SUBHEADER_GC_CLEAR_SLOT");
				if (!Recv(sizeof(TSubPacketGCAuraClearSlot), &kSubPacket))
					return false;

				CPythonPlayer::instance().DelAuraItemData(kSubPacket.bAuraSlotPos);
			}
			else
				TraceError(" RecvAuraPacket Error 0x04FF0%uAA", kAuraPacket.bSubHeader);

			break;
		}
		case AURA_SUBHEADER_GC_CLEAR_ALL:
		{
			CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_MAIN);
			CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_SUB);
			CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_RESULT);
			break;
		}
		case AURA_SUBHEADER_GC_CLEAR_RIGHT:
		{
			if (iPacketSize == 0)
				CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_SUB);
			else
				TraceError("invalid packet size %d", iPacketSize);
			break;
		}
		case AURA_SUBHEADER_GC_REFINE_INFO:
		{
			if (iPacketSize > 0)
			{
				for (size_t i = 0; iPacketSize > 0; ++i)
				{
					assert(iPacketSize % sizeof(TSubPacketGCAuraRefineInfo) == 0 && "AURA_SUBHEADER_GC_REFINE_INFO");
					TSubPacketGCAuraRefineInfo kSubPacket;
					if (!Recv(sizeof(TSubPacketGCAuraRefineInfo), &kSubPacket))
						return false;

					CPythonPlayer::instance().SetAuraRefineInfo(kSubPacket.bAuraRefineInfoType, kSubPacket.bAuraRefineInfoLevel, kSubPacket.bAuraRefineInfoExpPercent);
					iPacketSize -= sizeof(TSubPacketGCAuraRefineInfo);
				}

			}
			else
				TraceError(" RecvAuraPacket Error 0x04000%FF", kAuraPacket.bSubHeader);

			break;
		}
	}

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::SendAuraRefineCheckIn(TItemPos InventoryCell, TItemPos AuraCell, BYTE byAuraRefineWindowType)
{
	__PlayInventoryItemDropSound(InventoryCell);

	TPacketCGAura kAuraPacket;
	kAuraPacket.wSize = sizeof(TPacketCGAura) + sizeof(TSubPacketCGAuraRefineCheckIn);
	kAuraPacket.bSubHeader = AURA_SUBHEADER_CG_REFINE_CHECKIN;

	TSubPacketCGAuraRefineCheckIn kAuraSubPacket;
	kAuraSubPacket.ItemCell = InventoryCell;
	kAuraSubPacket.AuraCell = AuraCell;
	kAuraSubPacket.byAuraRefineWindowType = byAuraRefineWindowType;

	if (!Send(sizeof(TPacketCGAura), &kAuraPacket))
		return false;

	if (!Send(sizeof(TSubPacketCGAuraRefineCheckIn), &kAuraSubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineCheckOut(TItemPos AuraCell, BYTE byAuraRefineWindowType)
{
	TPacketCGAura kAuraPacket;
	kAuraPacket.wSize = sizeof(TPacketCGAura) + sizeof(TSubPacketCGAuraRefineCheckOut);
	kAuraPacket.bSubHeader = AURA_SUBHEADER_CG_REFINE_CHECKOUT;

	TSubPacketCGAuraRefineCheckOut kAuraSubPacket;
	kAuraSubPacket.AuraCell = AuraCell;
	kAuraSubPacket.byAuraRefineWindowType = byAuraRefineWindowType;

	if (!Send(sizeof(TPacketCGAura), &kAuraPacket))
		return false;

	if (!Send(sizeof(TSubPacketCGAuraRefineCheckOut), &kAuraSubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineAccept(BYTE byAuraRefineWindowType)
{
	TPacketCGAura kAuraPacket;
	kAuraPacket.wSize = sizeof(TPacketCGAura) + sizeof(TSubPacketCGAuraRefineAccept);
	kAuraPacket.bSubHeader = AURA_SUBHEADER_CG_REFINE_ACCEPT;

	TSubPacketCGAuraRefineAccept kAuraSubPacket;
	kAuraSubPacket.byAuraRefineWindowType = byAuraRefineWindowType;

	if (!Send(sizeof(TPacketCGAura), &kAuraPacket))
		return false;

	if (!Send(sizeof(TSubPacketCGAuraRefineAccept), &kAuraSubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineCancel()
{
	TPacketCGAura kAuraPacket;
	kAuraPacket.wSize = sizeof(TPacketCGAura);
	kAuraPacket.bSubHeader = AURA_SUBHEADER_CG_REFINE_CANCEL;

	if (!Send(sizeof(TPacketCGAura), &kAuraPacket))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
bool CPythonNetworkStream::SendPetHatchingPacket(const char *c_szName, TItemPos eggPos)
{
	TPacketCGPetHatch packet;
	packet.header = HEADER_CG_PET_HATCH;
	strncpy(packet.name, c_szName, sizeof(packet.name));
	packet.eggPos = eggPos;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetWindowType(BYTE bType)
{
	TPacketCGPetWindowType packet;
	packet.header = HEADER_CG_PET_WINDOW_TYPE;
	packet.type = bType;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetHatchingWindowPacket(bool bState)
{
	TPacketCGPetWindow packet;
	packet.header = HEADER_CG_PET_WINDOW;
	packet.window = PET_WINDOW_HATCH;
	packet.state = bState;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetNameChangeWindowPacket(bool bState)
{
	TPacketCGPetWindow packet;
	packet.header = HEADER_CG_PET_WINDOW;
	packet.window = PET_WINDOW_NAME_CHANGE;
	packet.state = bState;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetNameChangePacket(const char *c_szName, TItemPos changeNamePos, TItemPos upBringingPos)
{
	TPacketCGPetNameChange packet;
	packet.header = HEADER_CG_PET_NAME_CHANGE;
	strncpy(packet.name, c_szName, sizeof(packet.name));
	packet.changeNamePos = changeNamePos;
	packet.upBringingPos = upBringingPos;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetFeedPacket(int iFeedIndex, const std::vector<std::pair<WORD, WORD>>& itemVec)
{
	TPacketCGPetFeed packet;
	packet.header = HEADER_CG_PET_FEED;
	packet.index = iFeedIndex;
	memset(packet.pos, 0, sizeof(packet.pos));
	memset(packet.count, 0, sizeof(packet.count));

	for (int i = 0; i < itemVec.size(); i++)
	{
		packet.pos[i] = itemVec.at(i).first;
		packet.count[i] = itemVec.at(i).second;
	}

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetAttrDetermine(TItemPos determinePos)
{
	TPacketCGPetDetermine packet;
	packet.header = HEADER_CG_PET_DETERMINE;
	packet.determinePos = determinePos;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetAttrChange(TItemPos upBringingPos, TItemPos attrChangePos)
{
	TPacketCGPetAttrChange packet;
	packet.header = HEADER_CG_PET_ATTR_CHANGE;
	packet.upBringingPos = upBringingPos;
	packet.attrChangePos = attrChangePos;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetRevive(TItemPos upBringingPos, const std::vector<std::pair<WORD, WORD>>& itemVec)
{
	TPacketCGPetRevive packet;
	packet.header = HEADER_CG_PET_REVIVE;
	packet.upBringingPos = upBringingPos;
	memset(packet.pos, 0, sizeof(packet.pos));
	memset(packet.count, 0, sizeof(packet.count));

	for (int i = 0; i < itemVec.size(); i++)
	{
		packet.pos[i] = itemVec.at(i).first;
		packet.count[i] = itemVec.at(i).second;
	}

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetLearnSkill(BYTE bSlot, TItemPos learnSkillPos)
{
	TPacketCGPetLearnSkill packet;
	packet.header = HEADER_CG_PET_LEARN_SKILL;
	packet.slotIndex = bSlot;
	packet.learnSkillPos = learnSkillPos;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetSkillUpgradeRequest(BYTE bSlot)
{
	TPacketCGPetSkillUpgrade packet;
	packet.header = HEADER_CG_PET_SKILL_UPGRADE;
	packet.slotIndex = bSlot;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetDeleteSkill(BYTE bSlot, TItemPos deleteSkillPos)
{
	TPacketCGPetDeleteSkill packet;
	packet.header = HEADER_CG_PET_DELETE_SKILL;
	packet.slotIndex = bSlot;
	packet.deleteSkillPos = deleteSkillPos;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPetDeleteAllSkill(TItemPos deleteAllSkillPos)
{
	TPacketCGPetDeleteAllSkill packet;
	packet.header = HEADER_CG_PET_DELETE_ALL_SKILL;
	packet.deleteAllSkillPos = deleteAllSkillPos;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvPet()
{
	TPacketGCPet packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPet Error");
		return false;
	}

	switch (packet.subheader)
	{
		case SUBHEADER_PET_EGG_USE_SUCCESS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetHatchingWindowCommand", Py_BuildValue("(i)", CItemData::EGG_USE_SUCCESS));
			break;

		case SUBHEADER_PET_EGG_USE_FAILED_BECAUSE_NAME:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetHatchingWindowCommand", Py_BuildValue("(i)", CItemData::EGG_USE_FAILED_BECAUSE_NAME));
			break;

		case SUBHEADER_PET_EGG_USE_FAILED_TIMEOVER:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetHatchingWindowCommand", Py_BuildValue("(i)", CItemData::EGG_USE_FAILED_TIMEOVER));
			break;

		case SUBHEADER_PET_UNSUMMON:
			CPythonPlayer::Instance().SetActivePet(NULL);
			break;

		case SUBHEADER_PET_FEED_FAILED:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetFeedReuslt", Py_BuildValue("(i)", FALSE));
			break;

		case SUBHEADER_PET_FEED_SUCCESS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetFeedReuslt", Py_BuildValue("(i)", TRUE));
			break;

		case SUBHEADER_PET_REVIVE_FAILED:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetReviveResult", Py_BuildValue("(i)", FALSE));
			break;

		case SUBHEADER_PET_REVIVE_SUCCESS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetReviveResult", Py_BuildValue("(i)", TRUE));
			break;

		case SUBHEADER_PET_WINDOW_TYPE_INFO:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetWindowTypeResult", Py_BuildValue("(i)", PET_WINDOW_INFO));
			break;

		case SUBHEADER_PET_WINDOW_TYPE_ATTR_CHANGE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetWindowTypeResult", Py_BuildValue("(i)", PET_WINDOW_ATTR_CHANGE));
			break;

		case SUBHEADER_PET_WINDOW_TYPE_PREMIUM_FEED:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetWindowTypeResult", Py_BuildValue("(i)", PET_WINDOW_PRIMIUM_FEEDSTUFF));
			break;
	}

	return true;
}

bool CPythonNetworkStream::RecvPetSet()
{
	TPacketGCPetSet packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetSet Error");
		return false;
	}

	CPythonPlayer::TPetData pet;
	pet.dwID = packet.dwID;
	pet.strName = packet.szName;
	pet.dwSummonItemVnum = packet.dwSummonItemVnum;

	for (DWORD i = 0; i < POINT_UPBRINGING_MAX_NUM; ++i)
		pet.dwPoints[i] = packet.dwPoints[i];

	for (int i = 0; i < PET_SKILL_COUNT_MAX; ++i)
	{
		pet.skillData.aSkillInfo[i].bSkill = packet.aSkill[i].bSkill;
		pet.skillData.aSkillInfo[i].bLevel = packet.aSkill[i].bLevel;
		pet.skillData.aSkillInfo[i].dwCooltime = packet.aSkill[i].dwCooltime;

		if (!packet.aSkill[i].bLocked)
			++pet.skillData.bSkillCount;
	}

	CPythonPlayer::Instance().SetPetData(&pet);

	return true;
}

bool CPythonNetworkStream::RecvPetSetExchange()
{
	TPacketGCPetSet packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetSet-Exchange Error");
		return false;
	}

	CPythonPlayer::TPetData pet;
	pet.dwID = packet.dwID;
	pet.strName = packet.szName;
	pet.dwSummonItemVnum = packet.dwSummonItemVnum;

	for (DWORD i = 0; i < POINT_UPBRINGING_MAX_NUM; ++i)
		pet.dwPoints[i] = packet.dwPoints[i];

	for (int i = 0; i < PET_SKILL_COUNT_MAX; ++i)
	{
		pet.skillData.aSkillInfo[i].bSkill = packet.aSkill[i].bSkill;
		pet.skillData.aSkillInfo[i].bLevel = packet.aSkill[i].bLevel;
		pet.skillData.aSkillInfo[i].dwCooltime = packet.aSkill[i].dwCooltime;

		if (!packet.aSkill[i].bLocked)
			++pet.skillData.bSkillCount;
	}

	pet.bIsExchange = true;

	CPythonPlayer::Instance().SetPetData(&pet);

	return true;
}

bool CPythonNetworkStream::RecvPetDelete()
{
	TPacketGCPetDelete packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetDelete Error");
		return false;
	}

	CPythonPlayer::Instance().DeletePetData(packet.dwID);

	return true;
}

bool CPythonNetworkStream::RecvPetSummon()
{
	TPacketGCPetSummon packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetSummon Error");
		return false;
	}

	CPythonPlayer::Instance().SetActivePet(packet.dwID);

	return true;
}

bool CPythonNetworkStream::RecvPetPointChange()
{
	TPacketGCPetPointUpdate packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetPointUpdate Error");
		return false;
	}

	CPythonPlayer::Instance().SetPetStatus(packet.dwID, packet.bPoint, packet.dwValue);

	return true;
}

bool CPythonNetworkStream::RecvPetNameChangeResult()
{
	TPacketGCPetNameChangeResult packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetNameChangeResult Error");
		return false;
	}

	switch (packet.subheader)
	{
		case SUBHEADER_PET_NAME_CHANGE_FAILED:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetNameChangeWindowCommand", Py_BuildValue("(i)", CItemData::NAME_CHANGE_USE_FAILED_BECAUSE_NAME));
		}	break;

		case SUBHEADER_PET_NAME_CHANGE_SUCCESS:
		{
			CPythonPlayer::TPetData* pPet = nullptr;
			CPythonPlayer::Instance().GetPetInfo(packet.dwID, &pPet);

			if (pPet)
				pPet->strName = packet.szName;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetNameChangeWindowCommand", Py_BuildValue("(i)", CItemData::NAME_CHANGE_USE_SUCCESS));
		}	break;
	}

	return true;
}

bool CPythonNetworkStream::RecvPetSkillUpdate()
{
	TPacketGCPetSkillUpdate packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetSkillUpdate Error");
		return false;
	}

	CPythonPlayer::Instance().SetPetSkillData(packet.dwID, packet.aSkill);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetAffectShowerRefresh", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvPetSkillCooltime()
{
	TPacketGCPetSkillCooltime packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetSkillCooltime Error");
		return false;
	}

	CPythonPlayer::Instance().SetPetSkillCooltime(packet.dwID, packet.bSlot, packet.dwCooltime);

	return true;
}

bool CPythonNetworkStream::RecvPetDetermineResult()
{
	TPacketGCPetDetermineResult packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetDetermineResult Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetAttrDetermineResult", Py_BuildValue("(i)", packet.type));
	return true;
}

bool CPythonNetworkStream::RecvPetAttrChangeResult()
{
	TPacketGCPetAttrChangeResult packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCPetAttrChangeResult Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PetAttrChangeResult", Py_BuildValue("(ii)", packet.pos.cell, packet.type));

	return true;
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
#define TransmutationPacket(x) TPacketCGTransmutation p(static_cast<std::underlying_type_t<ECG_TRANSMUTATION_SHEADER>>(ECG_TRANSMUTATION_SHEADER::x))

bool CPythonNetworkStream::SendChangeLookCheckInPacket(const TItemPos& pos, const BYTE bSlotType)
{
	TransmutationPacket(ITEM_CHECK_IN);
	p.slot_type = bSlotType;
	p.pos = pos;

	if (!Send(sizeof(p), &p))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCheckInPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookCheckOutPacket(const BYTE bSlotType)
{
	TransmutationPacket(ITEM_CHECK_OUT);
	p.slot_type = bSlotType;

	if (!Send(sizeof(p), &p))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCheckOutPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookCheckInFreeYangItemPacket(const TItemPos& pos)
{
	TransmutationPacket(FREE_ITEM_CHECK_IN);
	p.pos = pos;

	if (!Send(sizeof(p), &p))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCheckInFreeYangItemPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookCheckOutFreeYangItemPacket()
{
	TransmutationPacket(FREE_ITEM_CHECK_OUT);

	if (!Send(sizeof(p), &p))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCheckOutFreeYangItemPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookAcceptPacket()
{
	TransmutationPacket(ACCEPT);

	if (!Send(sizeof(p), &p))
	{
		Tracef("CPythonNetworkStream::SendChangeLookAcceptPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookCancelPacket()
{
	TransmutationPacket(CANCEL);

	if (!Send(sizeof(p), &p))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCancelPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvChangeLookItemSetPacket()
{
	TPacketGCTransmutationItemSet kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	if (kItemSet.bSlotType >= static_cast<BYTE>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_MAX))
		return true;

	CPythonPlayer::Instance().SetChangeLookItemData(kItemSet);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvChangeLookItemDelPacket()
{
	TPacketGCTransmutationDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonPlayer::Instance().DelChangeLookItemData(kItemDel.bSlotType, true);

	__RefreshInventoryWindow();
	Tracef(" >> CPythonNetworkStream::RecvChangeLookItemDelPacket\n");

	return true;
}

bool CPythonNetworkStream::RecvChangeLookFreeItemSetPacket()
{
	TPacketGCTransmutationItemSet kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	CPythonPlayer::Instance().SetChangeLookFreeItemData(kItemSet);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvChangeLookFreeItemDelPacket()
{
	TPacketGCTransmutationDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonPlayer::Instance().DelChangeLookFreeItemData(true);

	__RefreshInventoryWindow();
	Tracef(" >> CPythonNetworkStream::RecvChangeLookFreeItemDelPacket\n");

	return true;
}
#endif

#ifdef ENABLE_HUNTING_SYSTEM
bool CPythonNetworkStream::SendHuntingAction(BYTE bAction, DWORD dValue)
{
	if (!__CanActMainInstance())
		return true;

	TPacketGCHuntingAction packet;
	packet.bHeader = HEADER_CG_SEND_HUNTING_ACTION;
	packet.bAction = bAction;
	packet.dValue = dValue;

	if (!Send(sizeof(TPacketGCHuntingAction), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvHuntingOpenWindowMain()
{
	TPacketGCOpenWindowHuntingMain packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenHuntingWindowMain", Py_BuildValue("(iiiiiiiiii)", 
		packet.dLevel, packet.dMonster, packet.dCurCount, packet.dDestCount, packet.dMoneyMin, packet.dMoneyMax, packet.dExpMin, packet.dExpMax, packet.dRaceItem, packet.dRaceItemCount
	));

	return true;
}

bool CPythonNetworkStream::RecvHuntingOpenWindowSelect()
{
	TPacketGCOpenWindowHuntingSelect packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenHuntingWindowSelect", Py_BuildValue("(iiiiiiiiii)",
		packet.dLevel, packet.bType, packet.dMonster, packet.dCount, packet.dMoneyMin, packet.dMoneyMax, packet.dExpMin, packet.dExpMax, packet.dRaceItem, packet.dRaceItemCount
	));

	return true;
}

bool CPythonNetworkStream::RecvHuntingOpenWindowReward()
{
	TPacketGCOpenWindowReward packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenHuntingWindowReward", Py_BuildValue("(iiiiiii)", packet.dLevel, packet.dReward, packet.dRewardCount, packet.dRandomReward, packet.dRandomRewardCount, packet.dMoney, packet.bExp ));

	return true;
}

bool CPythonNetworkStream::RecvHuntingUpdate()
{
	TPacketGCUpdateHunting packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateHuntingMission", Py_BuildValue("(i)", packet.dCount));

	return true;
}

bool CPythonNetworkStream::RecvHuntingRandomItems()
{
	TPacketGCReciveRandomItems packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_HuntingReciveRandomItem", Py_BuildValue("(iii)", packet.bWindow, packet.dItemVnum, packet.dItemCount));

	return true;
}
#endif

#ifdef ENABLE_RIDING_EXTENDED
bool CPythonNetworkStream::MountUpGrade(const uint8_t iSubHeader)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGMountUpGrade p =
	{
		static_cast<uint8_t>(HEADER_CG_MOUNT_UP_GRADE),
		static_cast<uint8_t>(iSubHeader),
	};

	if (!Send(sizeof(p), &p))
	{
		Tracen("CPythonNetworkStream::MountUpGrade Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvMountUpGrade()
{
	TPacketGCMountUpGrade p;
	if (!Recv(sizeof(p), &p))
	{
		Tracen("RecvOpenMountUpGrade Error");
		return false;
	}

	CPythonMountUpGrade::Instance().SetHorseLevel(p.horse_level);
	CPythonMountUpGrade::Instance().SetMountUpGradeFail(p.is_fail);
	CPythonMountUpGrade::Instance().SetMountUpGradeExistingExp(p.existing_exp);

	switch (p.subheader)
	{
		case CPythonMountUpGrade::EMountUpGradeGCSubheaderType::SUBHEADER_GC_MOUNT_UP_GRADE_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenMountUpGrade", Py_BuildValue("()"));
			break;

		case CPythonMountUpGrade::EMountUpGradeGCSubheaderType::SUBHEADER_GC_MOUNT_UP_GRADE_REFRESH:
			CPythonMountUpGrade::Instance().Refresh();
			break;

		default:
		{
			TraceError("CPythonNetworkStream::RecvMountUpGrade invalid subHeader: %d", p.subheader);
			break;
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvMountUpGradeChat()
{
	TPacketGCMountUpGradeChat p;
	if (!Recv(sizeof(p), &p))
	{
		Tracen("RecvMountUpGradeChat Error");
		return false;
	}

	CPythonMountUpGrade::Instance().Chat(p.type, p.value);

	return true;
}
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
bool CPythonNetworkStream::RecvHitCountInfo()
{
	TPacketGCHitCountInfo hitCountPacket;
	if (!Recv(sizeof(hitCountPacket), &hitCountPacket))
		return false;

	CPythonPlayer::Instance().SetHitCountInfo(hitCountPacket.dwVid, hitCountPacket.dwCount);
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshHitCount", Py_BuildValue("(i)", hitCountPacket.dwVid));

	return true;
}

bool CPythonNetworkStream::SendAttendanceGetReward()
{
	BYTE header = HEADER_CG_ATTENDANCE_REWARD;
	if (!Send(sizeof(BYTE), &header))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvAttendanceEvent()
{
	TPacketGCAttendanceEvent packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameAttendanceSetData", Py_BuildValue("(ii)", packet.bType, packet.bValue));

	return true;
}

bool CPythonNetworkStream::RecvAttendanceEventInfo()
{
	TPacketGCAttendanceEventInfo packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	packet.wSize -= sizeof(packet);
	while (packet.wSize > 0)
	{
		TRewardItem kReward;
		if (!Recv(sizeof(kReward), &kReward))
			return false;

		CPythonPlayer::Instance().SetRewardItem(kReward.bDay, kReward.dwVnum, kReward.dwCount);

		packet.wSize -= sizeof(kReward);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameAttendanceEvent", Py_BuildValue("(i)", packet.bIsEnable));
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameAttendanceSetData", Py_BuildValue("(ii)", 1, packet.bDay));
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameAttendanceSetData", Py_BuildValue("(ii)", 2, packet.bClear));
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameAttendanceSetData", Py_BuildValue("(ii)", 3, packet.bReward));

	return true;
}
#endif

#ifdef ENABLE_EVENT_SYSTEM
bool CPythonNetworkStream::RecvEventInfo()
{
	TPacketGCEventInfo	infoPacket;
	if (!Recv(sizeof(infoPacket), &infoPacket))
		return false;
	CPythonGameEvents::instance().SetActivateEvent(infoPacket.isActivate, infoPacket.event_id);
	CPythonGameEvents::instance().SetEventTime(infoPacket.event_id, infoPacket.event_time);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvEventInformation", Py_BuildValue("()"));
	return true;
}
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
bool CPythonNetworkStream::SendMiniGameCatchKing(BYTE bSubHeader, BYTE bSubArgument)
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGMiniGameCatchKing packet;
	packet.bHeader = HEADER_CG_MINI_GAME_CATCH_KING;
	packet.bSubheader = bSubHeader;
	packet.bSubArgument = bSubArgument;

	if (!Send(sizeof(TPacketCGMiniGameCatchKing), &packet))
	{
		Tracef("SendMiniGameCatchKing Send Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvMiniGameCatchKingPacket()
{
	std::vector<char> vecBuffer;
	vecBuffer.clear();

	TPacketGCMiniGameCatchKing packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	int iSize = packet.wSize - sizeof(packet);
	if (iSize > 0)
	{
		vecBuffer.resize(iSize);
		if (!Recv(iSize, &vecBuffer[0]))
			return false;
	}

	switch (packet.bSubheader)
	{
		case SUBHEADER_GC_CATCH_KING_EVENT_INFO:
			{
				bool bIsEnable = *(bool *)&vecBuffer[0];
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingEvent", Py_BuildValue("(b)", bIsEnable));
			}
			break;
			
		case SUBHEADER_GC_CATCH_KING_START:
			{
				DWORD dwBigScore = *(DWORD *)&vecBuffer[0];
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingEventStart", Py_BuildValue("(i)", dwBigScore));
			}
			break;
			
		case SUBHEADER_GC_CATCH_KING_SET_CARD:
			{
				BYTE bCardNumber = *(BYTE *)&vecBuffer[0];
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingSetHandCard", Py_BuildValue("(i)", bCardNumber));
			}
			break;
			
		case SUBHEADER_GC_CATCH_KING_RESULT_FIELD:
			{
				TPacketGCMiniGameCatchKingResult * packSecond = (TPacketGCMiniGameCatchKingResult *)&vecBuffer[0];
				
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingResultField", Py_BuildValue("(iiiibbbb)", 
					packSecond->dwPoints, packSecond->bRowType, packSecond->bCardPos, packSecond->bCardValue, 
					packSecond->bKeepFieldCard, packSecond->bDestroyHandCard, packSecond->bGetReward, packSecond->bIsFiveNearBy));
			}
			break;
			
		case SUBHEADER_GC_CATCH_KING_SET_END_CARD:
			{
				TPacketGCMiniGameCatchKingSetEndCard * packSecond = (TPacketGCMiniGameCatchKingSetEndCard *)&vecBuffer[0];
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingSetEndCard", Py_BuildValue("(ii)", packSecond->bCardPos, packSecond->bCardValue));
			}
			break;
			
		case SUBHEADER_GC_CATCH_KING_REWARD:
			{
				BYTE bReturnCode = *(BYTE *)&vecBuffer[0];
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingReward", Py_BuildValue("(i)", bReturnCode));
			}
			break;
			
		default:
			TraceError("CPythonNetworkStream::RecvMiniGameCatchKingPacket: Unknown subheader\n");
			break;
	}

	return true;
}
#endif

#ifdef ENABLE_STONE_EVENT
bool CPythonNetworkStream::RecvStoneEvent()
{
	TPacketGCStoneEvent p;
	if (!Recv(sizeof(TPacketGCStoneEvent), &p))
	{
		Tracenf("Recv Stone Event Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_STONE_EVENT", Py_BuildValue("(i)", p.stone_point));
	return true;
}
#endif

#ifdef ENABLE_RANKING
bool CPythonNetworkStream::RecvRankingTable()
{
	TPacketGCRankingTable p;
	if (!Recv(sizeof(p), &p))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RankingClearData", Py_BuildValue("()"));
	for (int i = 0; i < MAX_RANKING_LIST; i++)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RankingAddRank", Py_BuildValue("(iiLsi)", p.list[i].iPosition, p.list[i].iLevel, p.list[i].iPoints, p.list[i].szName, p.list[i].iRealPosition));
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RankingRefresh", Py_BuildValue("()"));
	return true;
}
#endif

#ifdef ENABLE_KILL_STATISTICS
bool CPythonNetworkStream::RecvKillStatistics()
{
	TPacketGCKillStatistics KStatisticsGC;

	if (!Recv(sizeof(TPacketGCKillStatistics), &KStatisticsGC))
	{
		Tracen("Recv KStatisticsGC Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ReceiveKillStatisticsPacket", Py_BuildValue("(iiiiiiiiiii)",
		KStatisticsGC.iJinnoKills, KStatisticsGC.iShinsooKills, KStatisticsGC.iChunjoKills, KStatisticsGC.iTotalKills, KStatisticsGC.iTotalDeaths,
		KStatisticsGC.iDuelsWon, KStatisticsGC.iDuelsLost, KStatisticsGC.iBossesKills, KStatisticsGC.iStonesKills, KStatisticsGC.iMobsKills
		, KStatisticsGC.top_damage));
	return true;
}
#endif

bool CPythonNetworkStream::SendLeftSeatOffPacket()
{
	if (!__CanActMainInstance())
		return false;

	if (CPythonPlayer::Instance().IsObserverMode())
		return true;

	TPacketCGLeftSeat kPacket;
	kPacket.bHeader = HEADER_CG_LEFT_SEAT;
	kPacket.wSize = sizeof(TPacketCGLeftSeat);
	kPacket.bSub = LEFT_SEAT_SUB_OFF;

	if (!Send(sizeof(kPacket), &kPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendLeftSeatWaitTimeIndexPacket(BYTE bWaitTimeIndex, BYTE bSubMode)
{
	if (!__CanActMainInstance())
		return false;

	if (CPythonPlayer::Instance().IsObserverMode())
		return true;

	TPacketCGLeftSeatSetTime kPacket;
	kPacket.bHeader = HEADER_CG_LEFT_SEAT;
	kPacket.wSize = sizeof(TPacketCGLeftSeatSetTime);
	kPacket.bSub = LEFT_SEAT_SUB_SET_TIME;
	kPacket.bWaitTimeIndex = bWaitTimeIndex;
	kPacket.bSubMode = bSubMode;

	if (!Send(sizeof(kPacket), &kPacket))
		return false;

	return SendSequence();
}

#ifdef ENABLE_STATUS_EFFECT_SHOWER
bool CPythonNetworkStream::RecvStatusEffectPacket()
{
	return CPythonStatusEffectManager::Instance().RecvStatusEffectPacket();
}
#endif

#ifdef ENABLE_YIN_AND_YANG_ATTR
bool CPythonNetworkStream::RecvYinAndYangAttrPacket()
{
	return CPythonYinAndYangAttrManager::Instance().RecvYinAndYangAttrPacket();
}
#endif
