#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "input.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "cmd.h"
#include "buffer_manager.h"
#include "protocol.h"
#include "pvp.h"
#include "start_position.h"
#include "messenger_manager.h"
#include "guild_manager.h"
#include "party.h"
#include "dungeon.h"
#include "war_map.h"
#include "questmanager.h"
#include "building.h"
#include "wedding.h"
#include "affect.h"
#include "arena.h"
#include "OXEvent.h"
#include "priv_manager.h"
#include "log.h"
#include "horsename_manager.h"
#include "MarkManager.h"
#include "../../common/service.h"
#ifdef ENABLE_BUFFI_SYSTEM
#include "buffi_system.h"
#endif
#ifdef ENABLE_MOUNT_SYSTEM
	#include "MountSystem.h"
#endif

#ifdef ENABLE_PET_SYSTEM
	#include "PetSystem.h"
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
	#include "switchbot_manager.h"
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	#include "offlineshop_manager.h"
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
	#include "maintenance.h"
#endif

#ifdef ENABLE_DUNGEON_INFO
	#include "dungeon_info.h"
#endif

#ifdef ENABLE_RESP_SYSTEM
	#include "resp_manager.h"
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	#include "CollectionsSystem.hpp"
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
	#include "minigame.h"
#endif

#ifdef ENABLE_STONE_EVENT_SYSTEM
	#include "stone_event.h"
#endif
#ifdef ENABLE_EVENT_SYSTEM
	#include "auto_event_list.h"
#endif

static void _send_bonus_info(LPCHARACTER ch)
{
	int	item_drop_bonus = 0;
	int gold_drop_bonus = 0;
	int gold10_drop_bonus = 0;
	int exp_bonus = 0;

	item_drop_bonus = CPrivManager::instance().GetPriv(ch, PRIV_ITEM_DROP);
	gold_drop_bonus = CPrivManager::instance().GetPriv(ch, PRIV_GOLD_DROP);
	gold10_drop_bonus = CPrivManager::instance().GetPriv(ch, PRIV_GOLD10_DROP);
	exp_bonus = CPrivManager::instance().GetPriv(ch, PRIV_EXP_PCT);

	if (item_drop_bonus)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("The quota of the Item Drop is at the moment plus %d%%"), item_drop_bonus);
	}
	if (gold_drop_bonus)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("The quota of the Yang Drop is at the moment plus %d%%"), gold_drop_bonus);
	}
	if (gold10_drop_bonus)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("The drop rate for Yang rain is currently %d%% higher."), gold10_drop_bonus);
	}
	if (exp_bonus)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("The experience bonus is currently an additional %d%%."), exp_bonus);
	}

#ifdef __LEADERSHIP__BONUS__
	float kLeaderShip = (float) ch->GetSkillPowerByLevel( MIN(SKILL_MAX_LEVEL, ch->GetLeadershipSkillLevel() ) )/ 100.0f;
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SkillPowerLeadership %f", kLeaderShip);
#endif
}

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
static void send_event_halloween(LPCHARACTER ch)
{	
	if (ch->GetHalounPoints() > 0)
	{
		ch->OpenHalloween();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "eveniment_haloun %d", 1);
	}
	if (ch->GetHalounPoints() == 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "eveniment_haloun %d", 0);
	}
}
#endif

static bool FN_is_battle_zone(LPCHARACTER ch)
{
	switch (ch->GetMapIndex())
	{
		case 90:
		case 2:
		case 21:
		case 23:
		case 91:
		case 43:
		case 113:
			return false;
	}

	return true;
}

void CInputLogin::LoginByKey(LPDESC d, const char * data)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	TPacketCGLogin2 * pinfo = (TPacketCGLogin2 *) data;

	char login[LOGIN_MAX_LEN + 1];
	trim_and_lower(pinfo->login, login, sizeof(login));

	if (g_bNoMoreClient)
	{
		TPacketGCLoginFailure failurePacket;

		failurePacket.header = HEADER_GC_LOGIN_FAILURE;
		strlcpy(failurePacket.szStatus, "SHUTDOWN", sizeof(failurePacket.szStatus));
		d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
		return;
	}

	if (g_iUserLimit > 0)
	{
		int iTotal;
		int * paiEmpireUserCount;
		int iLocal;

		DESC_MANAGER::instance().GetUserCount(iTotal, &paiEmpireUserCount, iLocal);

		if (g_iUserLimit <= iTotal)
		{
			TPacketGCLoginFailure failurePacket;

			failurePacket.header = HEADER_GC_LOGIN_FAILURE;
			strlcpy(failurePacket.szStatus, "FULL", sizeof(failurePacket.szStatus));

			d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
			return;
		}
	}

	sys_log(0, "LOGIN_BY_KEY: %s key %u", login, pinfo->dwLoginKey);

	d->SetLoginKey(pinfo->dwLoginKey);
	d->SetSecurityKey(pinfo->adwClientKey);

	TPacketGDLoginByKey ptod;

	strlcpy(ptod.szLogin, login, sizeof(ptod.szLogin));
	ptod.dwLoginKey = pinfo->dwLoginKey;
	thecore_memcpy(ptod.adwClientKey, pinfo->adwClientKey, sizeof(DWORD) * 4);
	strlcpy(ptod.szIP, d->GetHostName(), sizeof(ptod.szIP));

	db_clientdesc->DBPacket(HEADER_GD_LOGIN_BY_KEY, d->GetHandle(), &ptod, sizeof(TPacketGDLoginByKey));
}

void CInputLogin::ChangeName(LPDESC d, const char * data)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	TPacketCGChangeName * p = (TPacketCGChangeName *) data;
	const TAccountTable & c_r = d->GetAccountTable();

	if (!c_r.id)
	{
		sys_err("no account table");
		return;
	}

	if (p->index >= PLAYER_PER_ACCOUNT) // @fixme190
	{
		sys_err("index overflow %d, login: %s", p->index, c_r.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (!c_r.players[p->index].dwID) // @fixme190
	{
		sys_err("player index not found, login: %s", c_r.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (!c_r.players[p->index].bChangeName)
		return;

	if (!check_name(p->name))
	{
		TPacketGCCreateFailure pack;
		pack.header = HEADER_GC_CHARACTER_CREATE_FAILURE;
		pack.bType = 0;
		d->Packet(&pack, sizeof(pack));
		return;
	}

	TPacketGDChangeName pdb;

	pdb.pid = c_r.players[p->index].dwID;
	strlcpy(pdb.name, p->name, sizeof(pdb.name));
	db_clientdesc->DBPacket(HEADER_GD_CHANGE_NAME, d->GetHandle(), &pdb, sizeof(TPacketGDChangeName));
}

void CInputLogin::CharacterSelect(LPDESC d, const char * data)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	struct command_player_select * pinfo = (struct command_player_select *) data;
	const TAccountTable & c_r = d->GetAccountTable();

	sys_log(0, "player_select: login: %s index: %d", c_r.login, pinfo->index);

	if (!c_r.id)
	{
		sys_err("no account table");
		d->SetPhase(PHASE_CLOSE); // Fix
		return;
	}

	if (pinfo->index >= PLAYER_PER_ACCOUNT)
	{
		sys_err("index overflow %d, login: %s", pinfo->index, c_r.login);
		d->SetPhase(PHASE_CLOSE); // Fix
		return;
	}

	if (!c_r.players[pinfo->index].dwID) // fixme190
	{
		sys_err("player index not found, login: %s", c_r.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (c_r.players[pinfo->index].bChangeName)
	{
		sys_err("name must be changed idx %d, login %s, name %s",
			pinfo->index, c_r.login, c_r.players[pinfo->index].szName);
		return;
	}

	TPlayerLoadPacket player_load_packet;

	player_load_packet.account_id = c_r.id;
	player_load_packet.player_id = c_r.players[pinfo->index].dwID;

	if (player_load_packet.player_id == 0)
	{
		sys_err("invalid player_id from account %d\n", c_r.id);
		d->DelayedDisconnect(0);
		return;
	}

	player_load_packet.account_index = pinfo->index;

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_LOAD, d->GetHandle(), &player_load_packet, sizeof(TPlayerLoadPacket));
}

bool RaceToJob(unsigned race, unsigned* ret_job)
{
	*ret_job = 0;

	if (race >= MAIN_RACE_MAX_NUM)
		return false;

	switch (race)
	{
		case MAIN_RACE_WARRIOR_M:
			*ret_job = JOB_WARRIOR;
			break;

		case MAIN_RACE_WARRIOR_W:
			*ret_job = JOB_WARRIOR;
			break;

		case MAIN_RACE_ASSASSIN_M:
			*ret_job = JOB_ASSASSIN;
			break;

		case MAIN_RACE_ASSASSIN_W:
			*ret_job = JOB_ASSASSIN;
			break;

		case MAIN_RACE_SURA_M:
			*ret_job = JOB_SURA;
			break;

		case MAIN_RACE_SURA_W:
			*ret_job = JOB_SURA;
			break;

		case MAIN_RACE_SHAMAN_M:
			*ret_job = JOB_SHAMAN;
			break;

		case MAIN_RACE_SHAMAN_W:
			*ret_job = JOB_SHAMAN;
			break;

		default:
			return false;
			break;
	}
	return true;
}

// new character support
bool NewPlayerTable(TPlayerTable * table, const char * name, BYTE race, BYTE shape, BYTE bEmpire)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err ("NewPlayerTable.OUT_OF_RACE_RANGE(%d >= max(%d))\n", race, MAIN_RACE_MAX_NUM);
		return false;
	}

	unsigned job;

	if (!RaceToJob (race, &job))
	{
		sys_err ("NewPlayerTable.RACE_TO_JOB_ERROR(%d)\n", race);
		return false;
	}

	sys_log (0, "NewPlayerTable(name=%s, race=%d, job=%d)", name, race, job);

	memset(table, 0, sizeof(TPlayerTable));

	strlcpy(table->name, name, sizeof(table->name));

	table->level = 1;
	table->job = race;
	table->voice = 0;
	table->part_base = shape;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	table->parts[PART_ACCE] = 0;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	table->parts[PART_AURA] = 0;
#endif

	table->st = JobInitialPoints[job].st;
	table->dx = JobInitialPoints[job].dx;
	table->ht = JobInitialPoints[job].ht;
	table->iq = JobInitialPoints[job].iq;

	table->hp = JobInitialPoints[job].max_hp + table->ht * JobInitialPoints[job].hp_per_ht;
	table->sp = JobInitialPoints[job].max_sp + table->iq * JobInitialPoints[job].sp_per_iq;
	table->stamina = JobInitialPoints[job].max_stamina;

	table->x = CREATE_START_X(bEmpire) + number(-300, 300);
	table->y = CREATE_START_Y(bEmpire) + number(-300, 300);
	table->z = 0;
	table->dir = 0;
	table->playtime = 0;
	table->gold = 0;

	table->skill_group = 0;

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	table->dwPickUPMode = 0;
#endif

	return true;
}

void CInputLogin::CharacterCreate(LPDESC d, const char * data)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	struct command_player_create * pinfo = (struct command_player_create *) data;

	const TAccountTable& c_rAccountTable = d->GetAccountTable();

	if (!c_rAccountTable.id)
	{
		sys_err("no account table");
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	TPlayerCreatePacket player_create_packet;

	sys_log(0, "PlayerCreate: name %s pos %d job %d shape %d",
		pinfo->name,
		pinfo->index,
		pinfo->job,
		pinfo->shape);

	TPacketGCLoginFailure packFailure;
	memset(&packFailure, 0, sizeof(packFailure));
	packFailure.header = HEADER_GC_CHARACTER_CREATE_FAILURE;

	if (true == g_BlockCharCreation)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	// �sim kullan�lamazsa veya yanl�� g�ndelik giyim ise, yarat�m ba�ar�s�z olur.
	if (!check_name(pinfo->name) || pinfo->shape > 1)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	// Fix
	if (pinfo->index >= PLAYER_PER_ACCOUNT)
	{
		sys_err("index overflow %d, login: %s", pinfo->index, c_rAccountTable.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	// Fix
	if (c_rAccountTable.players[pinfo->index].dwID != 0)
	{
		sys_err("login(%s) no empty character slot(%u)", c_rAccountTable.login, pinfo->index);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

#ifdef ENABLE_CHARACTER_NAME_HACK_FIX
	if (strlen(pinfo->name) > 24 || strlen(pinfo->name) < 3)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}
#endif

	if (0 == strcmp(c_rAccountTable.login, pinfo->name))
	{
		TPacketGCCreateFailure pack;
		pack.header = HEADER_GC_CHARACTER_CREATE_FAILURE;
		pack.bType = 1;

		d->Packet(&pack, sizeof(pack));
		return;
	}

	memset(&player_create_packet, 0, sizeof(TPlayerCreatePacket));

	if (!NewPlayerTable(&player_create_packet.player_table, pinfo->name, pinfo->job, pinfo->shape, d->GetEmpire()))
	{
		sys_err("player_prototype error: job %d face %d ", pinfo->job, pinfo->shape);
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	trim_and_lower(c_rAccountTable.login, player_create_packet.login, sizeof(player_create_packet.login));
	strlcpy(player_create_packet.passwd, c_rAccountTable.passwd, sizeof(player_create_packet.passwd));

	player_create_packet.account_id	= c_rAccountTable.id;
	player_create_packet.account_index	= pinfo->index;

	sys_log(0, "PlayerCreate: name %s account_id %d, TPlayerCreatePacketSize(%d), Packet->Gold %d",
			pinfo->name,
			pinfo->index,
			sizeof(TPlayerCreatePacket),
			player_create_packet.player_table.gold);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_CREATE, d->GetHandle(), &player_create_packet, sizeof(TPlayerCreatePacket));
}

void CInputLogin::CharacterDelete(LPDESC d, const char * data)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	struct command_player_delete * pinfo = (struct command_player_delete *) data;
	const TAccountTable & c_rAccountTable = d->GetAccountTable();

	if (!c_rAccountTable.id)
	{
		sys_err("PlayerDelete: no login data");
		d->SetPhase(PHASE_CLOSE); // Fix
		return;
	}

	sys_log(0, "PlayerDelete: login: %s index: %d, social_id %s", c_rAccountTable.login, pinfo->index, pinfo->private_code);

	if (pinfo->index >= PLAYER_PER_ACCOUNT)
	{
		sys_err("PlayerDelete: index overflow %d, login: %s", pinfo->index, c_rAccountTable.login);
		d->SetPhase(PHASE_CLOSE); // Fix
		return;
	}

	if (!c_rAccountTable.players[pinfo->index].dwID)
	{
		sys_err("PlayerDelete: Wrong Social ID index %d, login: %s", pinfo->index, c_rAccountTable.login);
		d->Packet(encode_byte(HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID), 1);
		return;
	}

	TPlayerDeletePacket	player_delete_packet;

	trim_and_lower(c_rAccountTable.login, player_delete_packet.login, sizeof(player_delete_packet.login));
	player_delete_packet.player_id	= c_rAccountTable.players[pinfo->index].dwID;
	player_delete_packet.account_index	= pinfo->index;
	strlcpy(player_delete_packet.private_code, pinfo->private_code, sizeof(player_delete_packet.private_code));

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_DELETE, d->GetHandle(), &player_delete_packet, sizeof(TPlayerDeletePacket));
}

void CInputLogin::Entergame(LPDESC d, const char * data)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
	{
		sys_err("EnterGame: No Desc!!");
		d->SetPhase(PHASE_CLOSE);
		return;
	}
#endif

	// Fix
	const TAccountTable& c_rAccountTable = d->GetAccountTable();

	if (!c_rAccountTable.id)
	{
		sys_err("no login data");
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	PIXEL_POSITION pos = ch->GetXYZ();

	if (!SECTREE_MANAGER::instance().GetMovablePosition(ch->GetMapIndex(), pos.x, pos.y, pos))
	{
		PIXEL_POSITION pos2;
		SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos2);

		sys_err("!GetMovablePosition (name %s %dx%d map %d changed to %dx%d)",
			ch->GetName(),
			pos.x, pos.y,
			ch->GetMapIndex(),
			pos2.x, pos2.y);
		pos = pos2;
	}

	CGuildManager::instance().LoginMember(ch);

	ch->Show(ch->GetMapIndex(), pos.x, pos.y, pos.z);
	SECTREE_MANAGER::instance().SendNPCPosition(ch);

	ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
	ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

	ch->ReviveInvisible(5);
	d->SetPhase(PHASE_GAME);

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	if (ch->GetQuestFlag("costume_option.hide_body") != 0)
		ch->SetBodyCostumeHidden(true);
	else
		ch->SetBodyCostumeHidden(false);

	if (ch->GetQuestFlag("costume_option.hide_hair") != 0)
		ch->SetHairCostumeHidden(true);
	else
		ch->SetHairCostumeHidden(false);

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (ch->GetQuestFlag("costume_option.hide_acce") != 0)
		ch->SetAcceCostumeHidden(true);
	else
		ch->SetAcceCostumeHidden(false);
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (ch->GetQuestFlag("costume_option.hide_weapon") != 0)
		ch->SetWeaponCostumeHidden(true);
	else
		ch->SetWeaponCostumeHidden(false);
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	if (ch->GetQuestFlag("costume_option.hide_aura") != 0)
		ch->SetAuraCostumeHidden(true);
	else
		ch->SetAuraCostumeHidden(false);
#endif
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (int i = 0; i < UNLOCK_INVENTORY_MAX; ++i)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerInventoryUnlock Hide|%d|%d", ch->GetUnlockSlotsW(i), i);
	}
#endif

	if(ch->GetItemAward_cmd())
		quest::CQuestManager::instance().ItemInformer(ch->GetPlayerID(),ch->GetItemAward_vnum());

	sys_log(0, "ENTERGAME: %s %dx%dx%d %s map_index %d", ch->GetName(), ch->GetX(), ch->GetY(), ch->GetZ(), d->GetHostName(), ch->GetMapIndex());

	if (ch->GetHorseLevel() > 0)
	{
		ch->EnterHorse();
	}

	// Start of playtime recording
	ch->ResetPlayTime();

	// Add auto save event
	ch->StartSaveEvent();
	ch->StartRecoveryEvent();
	ch->StartCheckSpeedHackEvent();
#ifdef __DUNGEON_INFO__
	ch->SendDungeonCooldown(0);
#endif
#ifdef ENABLE_KILL_STATISTICS
	ch->SendKillStatisticsPacket();
#endif
#ifdef ENABLE_DUNGEON_INFO
	if (ch->GetMapIndex() < 10000)
	{
		CDungeonInfoExtern::instance().CheckBossKill(ch);
		CDungeonInfoExtern::instance().SetTimeRespawn(ch);
	}
#endif
	ch->SetQuestFlag("item.last_time", get_global_time()); // Fix

	CPVPManager::instance().Connect(ch);
	CPVPManager::instance().SendList(d);

#ifdef ENABLE_STONE_EVENT_SYSTEM
	CStoneEvent::instance().StoneInformation(ch);
#endif
	MessengerManager::instance().Login(ch->GetName());

	CPartyManager::instance().SetParty(ch);
	CGuildManager::instance().SendGuildWar(ch);

	building::CManager::instance().SendLandList(d, ch->GetMapIndex());

	marriage::CManager::instance().Login(ch);

#ifdef ENABLE_MULTI_FARM_BLOCK
	CHARACTER_MANAGER::instance().CheckMultiFarmAccount(d->GetHostName(), ch->GetPlayerID(), ch->GetName(), true, false);
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	CSystemCollections::instance().SendCollections(ch);
#endif

#ifdef ENABLE_EVENT_SYSTEM
	CGameEventsManager::instance().SendEventCharacter(ch);
#endif

	TPacketGCTime p;
	p.bHeader = HEADER_GC_TIME;
	p.time = get_global_time();
	d->Packet(&p, sizeof(p));

	TPacketGCChannel p2;
	p2.header = HEADER_GC_CHANNEL;
	p2.channel = g_bChannel;
#ifdef ENABLE_ANTI_EXP
	p2.anti_exp = ch->GetAntiExp();
#endif
	d->Packet(&p2, sizeof(p2));
#ifdef ENABLE_ZODIAC_MISSION
	ch->BeadCount(ch);
#endif
	ch->SendGreetMessage();

	_send_bonus_info(ch);
#ifdef ENABLE_BATTLE_PASS
	ch->ExternBattlePass();
	ch->Load_BattlePass();
	ch->ExternBattlePassPremium();
	ch->Load_BattlePassPremium();
#endif
#ifdef ENABLE_DUNGEON_INFO
	CDungeonInfoExtern::instance().LoadDateDungeonRanking();
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	CSoulRoulette::StateError(CSoulRoulette::Error::GIVE, ch);
#endif

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
	send_event_halloween(ch);
#endif

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE player%s SET `last_play` = NOW() WHERE `name` = '%s'", get_table_postfix(), ch->GetName()));

	for (int i = 0; i <= PREMIUM_MAX_NUM; ++i)
	{
		int remain = ch->GetPremiumRemainSeconds(i);

		if (remain <= 0)
			continue;

		ch->AddAffect(AFFECT_PREMIUM_START + i, POINT_NONE, 0, 0, remain, 0, true);
		sys_log(0, "PREMIUM: %s type %d %dmin", ch->GetName(), i, remain);
	}

	if (ch->IsGM() == true)
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");

	if (ch->GetMapIndex() >= 10000)
	{
		if (CWarMapManager::instance().IsWarMap(ch->GetMapIndex()))
			ch->SetWarMap(CWarMapManager::instance().Find(ch->GetMapIndex()));
		else if (marriage::WeddingManager::instance().IsWeddingMap(ch->GetMapIndex()))
			ch->SetWeddingMap(marriage::WeddingManager::instance().Find(ch->GetMapIndex()));
		else {
			ch->SetDungeon(CDungeonManager::instance().FindByMapIndex(ch->GetMapIndex()));
		}
	}
	else if (CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		int memberFlag = CArenaManager::instance().IsMember(ch->GetMapIndex(), ch->GetPlayerID());
		if (memberFlag == MEMBER_OBSERVER)
		{
			ch->SetObserverMode(true);
			ch->SetArenaObserverMode(true);
			if (CArenaManager::instance().RegisterObserverPtr(ch, ch->GetMapIndex(), ch->GetX()/100, ch->GetY()/100))
			{
				sys_log(0, "ARENA : Observer add failed");
			}

			if (ch->IsHorseRiding() == true)
			{
				ch->StopRiding();
				ch->HorseSummon(false);
			}
		}
		else if (memberFlag == MEMBER_DUELIST)
		{
			TPacketGCDuelStart duelStart;
			duelStart.header = HEADER_GC_DUEL_START;
			duelStart.wSize = sizeof(TPacketGCDuelStart);

			ch->GetDesc()->Packet(&duelStart, sizeof(TPacketGCDuelStart));

			if (ch->IsHorseRiding() == true)
			{
				ch->StopRiding();
				ch->HorseSummon(false);
			}

			LPPARTY pParty = ch->GetParty();
			if (pParty != NULL)
			{
				if (pParty->GetMemberCount() == 2)
				{
					CPartyManager::instance().DeleteParty(pParty);
				}
				else
				{
					pParty->Quit(ch->GetPlayerID());
				}
			}
		}
		else if (memberFlag == MEMBER_NO)
		{
			if (ch->GetGMLevel() == GM_PLAYER)
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		} else {}
	}
	else if (ch->GetMapIndex() == 113)
	{
#ifdef ENABLE_MOUNT_SYSTEM
		if (ch->IsHorseRiding() == true)
		{
			ch->StopRiding();
			ch->HorseSummon(false);
		}

		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsRiding())
			ch->StopRiding();
#endif

		if (COXEventManager::instance().Enter(ch) == false)
		{
			// No permission to enter the ox map. If you're a player, let's send it to the village
			if (ch->GetGMLevel() == GM_PLAYER)
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}
	}
	else
	{
		if (CWarMapManager::instance().IsWarMap(ch->GetMapIndex()) ||
				marriage::WeddingManager::instance().IsWeddingMap(ch->GetMapIndex()))
		{
			if (!test_server)
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}
	}

#ifdef ENABLE_CONQUEROR_LEVEL
	if (ch->IsConquerorMap(ch->GetMapIndex()) && ch->IsPC())
	{
		const BYTE bSTR = SECTREE_MANAGER::instance().GetSungmaValueAffectByRegion(ch->GetMapIndex(), AFFECT_SUNGMA_STR);
		const BYTE bHP = SECTREE_MANAGER::instance().GetSungmaValueAffectByRegion(ch->GetMapIndex(), AFFECT_SUNGMA_HP);
		const BYTE bMOVE = SECTREE_MANAGER::instance().GetSungmaValueAffectByRegion(ch->GetMapIndex(), AFFECT_SUNGMA_MOVE);
		const BYTE bIMMUNE = SECTREE_MANAGER::instance().GetSungmaValueAffectByRegion(ch->GetMapIndex(), AFFECT_SUNGMA_IMMUNE);
		TPacketGCSungmaAttrUpdate p;

		p.bHeader = HEADER_GC_SUNGMA_ATTR_UPDATE;
		p.bSTR = bSTR;
		p.bHP = bHP;
		p.bMOVE = bMOVE;
		p.bIMMUNE = bIMMUNE;
		ch->GetDesc()->Packet(&p, sizeof(p));	
	}
#endif

#ifdef ENABLE_MOUNT_LEVEL_BUG_FIX
	if (ch->GetHorseLevel() > 0)
	{
		DWORD pid = ch->GetPlayerID();

		if (pid != 0 && CHorseNameManager::instance().GetHorseName(pid) == NULL)
			db_clientdesc->DBPacket(HEADER_GD_REQ_HORSE_NAME, 0, &pid, sizeof(DWORD));

		ch->SetHorseLevel(ch->GetHorseLevel());
		ch->SkillLevelPacket();
	}
#else
	if (ch->GetHorseLevel() > 0)
	{
		DWORD pid = ch->GetPlayerID();

		if (pid != 0 && CHorseNameManager::instance().GetHorseName(pid) == NULL)
			db_clientdesc->DBPacket(HEADER_GD_REQ_HORSE_NAME, 0, &pid, sizeof(DWORD));
	}
#endif

	if (g_noticeBattleZone)
	{
		if (FN_is_battle_zone(ch))
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 564, "");
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 565, "");
		}
	}

#ifdef ENABLE_MOUNT_SYSTEM
	if (ch->GetMapIndex() != 113 && CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == false)
		ch->CheckMount();
#endif

#ifdef ENABLE_PET_SYSTEM
	if (ch->GetMapIndex() != 113 && CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == false)
		ch->CheckPet();
#endif

#ifdef ENABLE_BUFFI_SYSTEM
	if (ch->GetMapIndex() != 113 && CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == false)
		CBuffiManager::instance().OnLogin(ch);
#endif

#ifdef ENABLE_FISH_EVENT
	ch->FishEventGeneralInfo();
#endif

#ifdef ENABLE_RESP_SYSTEM
	CRespManager::instance().LoginToMap(ch);
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
	CSwitchbotManager::Instance().EnterGame(ch);
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	COfflineShopManager::Instance().HasOfflineShop(ch);
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	ch->ChangePickUPMode(ch->GetPickUPMode());
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
	MaintenanceManager::instance().Send_CheckTable(ch);
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
	ch->ReadOfflineMessages();
#endif

#ifdef ENABLE_RENEWAL_SKILL_SELECT
	ch->RenewalSkillSelect();
#endif

#ifdef ENABLE_HUNTING_SYSTEM
	ch->CheckHunting();
#endif

#ifdef __DOJANG_SRC_FUNCTIONS__
	if (ch->GetMapIndex() == DOJANG_MAPINDEX)
	{
		if (ch->GetParty())
		{
			if (ch->GetParty()->GetMemberCount() == 2)
				CPartyManager::instance().DeleteParty(ch->GetParty());
			else
				ch->GetParty()->Quit(ch->GetPlayerID());
		}
	}
#endif

#ifdef ENABLE_PITTY_REFINE
	ch->LoadRefineFlags();
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
	CMiniGame::instance().MiniGameCatchKingEventInfo(ch);
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
	CMiniGame::instance().AttendanceEventInfo(ch);
#endif

#ifdef ENABLE_BIOLOGIST_SYSTEM
	{
		int biodurum = ch->GetQuestFlag("bio.durum");
		if (biodurum == 0 && ch->GetLevel() >= 30)
		{
			ch->SetQuestFlag("bio.durum", 1);
			ch->SetQuestFlag("bio.verilen", 0);
			ch->SetQuestFlag("bio.ruhtasi", 0);
			ch->SetQuestFlag("bio.kalan", 0);

			int bioverilen = ch->GetQuestFlag("bio.verilen");
			int biokalan = ch->GetQuestFlag("bio.kalan");
			int biostate = ch->GetQuestFlag("bio.ruhtasi");
			biodurum = ch->GetQuestFlag("bio.durum");
			ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", biodurum, biostate, bioverilen, BiyologSistemi[biodurum][1], biokalan);
		}
		else
		{
			int bioverilen = ch->GetQuestFlag("bio.verilen");
			int biokalan = ch->GetQuestFlag("bio.kalan")-get_global_time();
			int biostate = ch->GetQuestFlag("bio.ruhtasi");
			biodurum = ch->GetQuestFlag("bio.durum");
			ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", biodurum, biostate, bioverilen, BiyologSistemi[biodurum][1], biokalan);
		}
	}
#endif
}

void CInputLogin::Empire(LPDESC d, const char * c_pData)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	const TPacketCGEmpire* p = reinterpret_cast<const TPacketCGEmpire*>(c_pData);

	// Fix
	if (p->bEmpire == 0 || p->bEmpire >= EMPIRE_MAX_NUM)
	{
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	const TAccountTable& r = d->GetAccountTable();

	if (r.bEmpire != 0)
	{
		for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		{
			if (0 != r.players[i].dwID)
			{
				sys_err("EmpireSelectFailed %d", r.players[i].dwID);
				d->SetPhase(PHASE_CLOSE);
				return;
			}
		}
	}

	TEmpireSelectPacket pd;

	pd.dwAccountID = r.id;
	pd.bEmpire = p->bEmpire;

	db_clientdesc->DBPacket(HEADER_GD_EMPIRE_SELECT, d->GetHandle(), &pd, sizeof(pd));
}

int CInputLogin::GuildSymbolUpload(LPDESC d, const char* c_pData, size_t uiBytes)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return -1;
#endif
	if (uiBytes < sizeof(TPacketCGGuildSymbolUpload))
		return -1;

	sys_log(0, "GuildSymbolUpload uiBytes %u", uiBytes);

	TPacketCGGuildSymbolUpload* p = (TPacketCGGuildSymbolUpload*) c_pData;

	if (uiBytes < p->size)
		return -1;

	int iSymbolSize = p->size - sizeof(TPacketCGGuildSymbolUpload);

	if (iSymbolSize <= 0 || iSymbolSize > 64 * 1024)
	{
		// Guild symbols larger than 64k cannot be uploaded
		// disconnect and ignore
		d->SetPhase(PHASE_CLOSE);
		return 0;
	}

	// If the guild does not own the land.
	if (!test_server)
		if (!building::CManager::instance().FindLandByGuild(p->guild_id))
		{
			d->SetPhase(PHASE_CLOSE);
			return 0;
		}

	sys_log(0, "GuildSymbolUpload Do Upload %02X%02X%02X%02X %d", c_pData[7], c_pData[8], c_pData[9], c_pData[10], sizeof(*p));

	CGuildMarkManager::instance().UploadSymbol(p->guild_id, iSymbolSize, (const BYTE*)(c_pData + sizeof(*p)));
	CGuildMarkManager::instance().SaveSymbol(GUILD_SYMBOL_FILENAME);
	return iSymbolSize;
}

void CInputLogin::GuildSymbolCRC(LPDESC d, const char* c_pData)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	const TPacketCGSymbolCRC & CGPacket = *((TPacketCGSymbolCRC *) c_pData);

	sys_log(0, "GuildSymbolCRC %u %u %u", CGPacket.guild_id, CGPacket.crc, CGPacket.size);

	const CGuildMarkManager::TGuildSymbol * pkGS = CGuildMarkManager::instance().GetGuildSymbol(CGPacket.guild_id);

	if (!pkGS)
		return;

	sys_log(0, "  Server %u %u", pkGS->crc, pkGS->raw.size());

	if (pkGS->raw.size() != CGPacket.size || pkGS->crc != CGPacket.crc)
	{
		TPacketGCGuildSymbolData GCPacket;

		GCPacket.header = HEADER_GC_SYMBOL_DATA;
		GCPacket.size = sizeof(GCPacket) + pkGS->raw.size();
		GCPacket.guild_id = CGPacket.guild_id;

		d->BufferedPacket(&GCPacket, sizeof(GCPacket));
		d->Packet(&pkGS->raw[0], pkGS->raw.size());

		sys_log(0, "SendGuildSymbolHead %02X%02X%02X%02X Size %d",
				pkGS->raw[0], pkGS->raw[1], pkGS->raw[2], pkGS->raw[3], pkGS->raw.size());
	}
}

void CInputLogin::GuildMarkUpload(LPDESC d, const char* c_pData)
{
	TPacketCGMarkUpload * p = (TPacketCGMarkUpload *) c_pData;
	CGuildManager& rkGuildMgr = CGuildManager::instance();
	CGuild * pkGuild{};

	if (!(pkGuild = rkGuildMgr.FindGuild(p->gid)))
	{
		sys_err("MARK_SERVER: GuildMarkUpload: no guild. gid %u", p->gid);
		return;
	}

	if (pkGuild->GetLevel() < guild_mark_min_level)
	{
		sys_log(0, "MARK_SERVER: GuildMarkUpload: level < %u (%u)", guild_mark_min_level, pkGuild->GetLevel());
		return;
	}

	CGuildMarkManager & rkMarkMgr = CGuildMarkManager::instance();

	sys_log(0, "MARK_SERVER: GuildMarkUpload: gid %u", p->gid);

	bool isEmpty = true;

	for (DWORD iPixel = 0; iPixel < SGuildMark::SIZE; ++iPixel)
		if (*((DWORD *) p->image + iPixel) != 0x00000000)
			isEmpty = false;

	if (isEmpty)
		rkMarkMgr.DeleteMark(p->gid);
	else
		rkMarkMgr.SaveMark(p->gid, p->image);
}

void CInputLogin::GuildMarkIDXList(LPDESC d, const char* c_pData)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	CGuildMarkManager & rkMarkMgr = CGuildMarkManager::instance();

	DWORD bufSize = sizeof(WORD) * 2 * rkMarkMgr.GetMarkCount();

	char * buf = NULL;

	if (bufSize > 0)
	{
		buf = (char *) malloc(bufSize);
		rkMarkMgr.CopyMarkIdx(buf);
	}

	TPacketGCMarkIDXList p;
	p.header = HEADER_GC_MARK_IDXLIST;
	p.bufSize = sizeof(p) + bufSize;
	p.count = rkMarkMgr.GetMarkCount();

	if (buf)
	{
		d->BufferedPacket(&p, sizeof(p));
		d->LargePacket(buf, bufSize);
		free(buf);
	}
	else
		d->Packet(&p, sizeof(p));

	sys_log(0, "MARK_SERVER: GuildMarkIDXList %d bytes sent.", p.bufSize);
}

void CInputLogin::GuildMarkCRCList(LPDESC d, const char* c_pData)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	TPacketCGMarkCRCList * pCG = (TPacketCGMarkCRCList *) c_pData;

	std::map<BYTE, const SGuildMarkBlock *> mapDiffBlocks;
	CGuildMarkManager::instance().GetDiffBlocks(pCG->imgIdx, pCG->crclist, mapDiffBlocks);

	DWORD blockCount = 0;
	TEMP_BUFFER buf(1024 * 1024);

	for (itertype(mapDiffBlocks) it = mapDiffBlocks.begin(); it != mapDiffBlocks.end(); ++it)
	{
		BYTE posBlock = it->first;
		const SGuildMarkBlock & rkBlock = *it->second;

		buf.write(&posBlock, sizeof(BYTE));
		DWORD compSize = (DWORD) rkBlock.m_sizeCompBuf;
		buf.write(&compSize, sizeof(DWORD));
		buf.write(rkBlock.m_abCompBuf, rkBlock.m_sizeCompBuf);

		++blockCount;
	}

	TPacketGCMarkBlock pGC;

	pGC.header = HEADER_GC_MARK_BLOCK;
	pGC.imgIdx = pCG->imgIdx;
	pGC.bufSize = buf.size() + sizeof(TPacketGCMarkBlock);
	pGC.count = blockCount;

	sys_log(0, "MARK_SERVER: Sending blocks. (imgIdx %u diff %u size %u)", pCG->imgIdx, mapDiffBlocks.size(), pGC.bufSize);

	if (buf.size() > 0)
	{
		d->BufferedPacket(&pGC, sizeof(TPacketGCMarkBlock));
		d->LargePacket(buf.read_peek(), buf.size());
	}
	else
		d->Packet(&pGC, sizeof(TPacketGCMarkBlock));
}

int CInputLogin::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_LOGIN2:
			LoginByKey(d, c_pData);
			break;

		case HEADER_CG_CHARACTER_SELECT:
			CharacterSelect(d, c_pData);
			break;

		case HEADER_CG_CHARACTER_CREATE:
			CharacterCreate(d, c_pData);
			break;

		case HEADER_CG_CHARACTER_DELETE:
			CharacterDelete(d, c_pData);
			break;

		case HEADER_CG_ENTERGAME:
			Entergame(d, c_pData);
			break;

		case HEADER_CG_EMPIRE:
			Empire(d, c_pData);
			break;

		case HEADER_CG_MOVE:
			break;

			///////////////////////////////////////
			// Guild Mark
			/////////////////////////////////////
		case HEADER_CG_MARK_CRCLIST:
			GuildMarkCRCList(d, c_pData);
			break;

		case HEADER_CG_MARK_IDXLIST:
			GuildMarkIDXList(d, c_pData);
			break;

		case HEADER_CG_MARK_UPLOAD:
			GuildMarkUpload(d, c_pData);
			break;

			//////////////////////////////////////
			// Guild Symbol
			/////////////////////////////////////
		case HEADER_CG_GUILD_SYMBOL_UPLOAD:
			if ((iExtraLen = GuildSymbolUpload(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_SYMBOL_CRC:
			GuildSymbolCRC(d, c_pData);
			break;
			/////////////////////////////////////
		case HEADER_CG_MARK_LOGIN:
			break;

		case HEADER_CG_HACK:
			break;

		case HEADER_CG_TARGET:
			break;

		case HEADER_CG_CHANGE_NAME:
			ChangeName(d, c_pData);
			break;

		// @fixme120
		case HEADER_CG_ITEM_USE:
		case HEADER_CG_SCRIPT_BUTTON:
			// ScriptButton paketi login phase'de ignore edilir (karakter hen�z y�klenmemi� olabilir)
			// Bu paket game phase'de handle edilir
			break;

		// Bir slash-komutu (/confirm_levelup, /auto_cape, ...) veya sohbet satiri karakter
		// daha tam olarak oyuna girmeden once gelebilir; CHARACTER nesnesi henuz hazir
		// degilken komutu calistirmaya calismak yerine, paketi (boyutunu dogru okuyarak,
		// arabellegi bozmadan) sessizce atla - yukaridaki ITEM_USE/SCRIPT_BUTTON ile ayni mantik.
		case HEADER_CG_CHAT:
		{
			const TPacketCGChat* pChatInfo = reinterpret_cast<const TPacketCGChat*>(c_pData);
			if (m_iBufferLeft < (int)pChatInfo->size)
				return -1;
			iExtraLen = pChatInfo->size - sizeof(TPacketCGChat);
			if (iExtraLen < 0)
				return -1;
			break;
		}

		default:
			sys_err("login phase does not handle this packet! header %d", bHeader);
			//d->SetPhase(PHASE_CLOSE);
#if defined(ENABLE_ANALYZE_CLOSE_FIX)
			return -1;
#else
			return (0);
#endif
	}

	return (iExtraLen);
}