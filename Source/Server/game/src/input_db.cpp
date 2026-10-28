#include "stdafx.h"
#include "../../common/service.h"
#include "../../common/tables.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "protocol.h"
#include "mob_manager.h"
#include "shop_manager.h"
#include "sectree_manager.h"
#include "skill.h"
#include "questmanager.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "start_position.h"
#include "party.h"
#include "refine.h"
#include "banword.h"
#include "priv_manager.h"
#include "db.h"
#include "building.h"
#include "login_sim.h"
#include "wedding.h"
#include "login_data.h"
#include "unique_item.h"
#include "affect.h"
#include "motion.h"

#include "log.h"
#include "buffer_manager.h"
#include "horsename_manager.h"
#include "gm.h"
#include "map_location.h"
#include "safebox.h"

#include "DragonSoul.h"

#ifdef ENABLE_REAL_TIME_REGEN
	#include "RealTimeRegen.hpp"
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	#include "offlineshop_manager.h"
#endif

#ifdef ENABLE_ULTIMATE_REGEN
	#include "new_mob_timer.h"
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
	#include "minigame.h"
#endif

extern BYTE g_bAuthServer;

extern void gm_insert(const char * name, BYTE level);
extern BYTE	gm_get_level(const char * name, const char * host, const char* account );
extern void gm_host_insert(const char * host);

#define MAPNAME_DEFAULT "none"

bool GetServerLocation(TAccountTable & rTab, BYTE bEmpire)
{
	bool bFound = false;

	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
	{
		if (0 == rTab.players[i].dwID)
			continue;

		bFound = true;
		long lIndex = 0;

		if (!CMapLocation::instance().Get(rTab.players[i].x,
					rTab.players[i].y,
					lIndex,
					rTab.players[i].lAddr,
					rTab.players[i].wPort))
		{
			sys_err("location error name %s mapindex %d %d x %d empire %d", rTab.players[i].szName, lIndex, rTab.players[i].x, rTab.players[i].y, rTab.bEmpire);

			rTab.players[i].x = EMPIRE_START_X(rTab.bEmpire);
			rTab.players[i].y = EMPIRE_START_Y(rTab.bEmpire);

			lIndex = 0;

			if (!CMapLocation::instance().Get(rTab.players[i].x, rTab.players[i].y, lIndex, rTab.players[i].lAddr, rTab.players[i].wPort))
			{
				sys_err("cannot find server for mapindex %d %d x %d (name %s)", lIndex, rTab.players[i].x, rTab.players[i].y, rTab.players[i].szName);

#ifdef ENABLE_PROXY_IP
				if (!g_stProxyIP.empty())
					rTab.players[i].lAddr=inet_addr(g_stProxyIP.c_str());
#endif

				continue;
			}
		}

		struct in_addr in;

#ifdef ENABLE_PROXY_IP
		if (!g_stProxyIP.empty())
			rTab.players[i].lAddr=inet_addr(g_stProxyIP.c_str());
#endif

		in.s_addr = rTab.players[i].lAddr;
		sys_log(0, "success to %s:%d", inet_ntoa(in), rTab.players[i].wPort);
	}

	return bFound;
}

extern std::map<DWORD, CLoginSim *> g_sim;
extern std::map<DWORD, CLoginSim *> g_simByPID;

void CInputDB::LoginSuccess(DWORD dwHandle, const char *data)
{
	sys_log(0, "LoginSuccess");

	TAccountTable * pTab = (TAccountTable *) data;

	itertype(g_sim) it = g_sim.find(pTab->id);
	if (g_sim.end() != it)
	{
		sys_log(0, "CInputDB::LoginSuccess - already exist sim [%s]", pTab->login);
		it->second->SendLoad();
		return;
	}

	LPDESC d = DESC_MANAGER::instance().FindByHandle(dwHandle);

	if (!d)
	{
		sys_log(0, "CInputDB::LoginSuccess - cannot find handle [%s]", pTab->login);

		TLogoutPacket pack;

		strlcpy(pack.login, pTab->login, sizeof(pack.login));
		db_clientdesc->DBPacket(HEADER_GD_LOGOUT, dwHandle, &pack, sizeof(pack));
		return;
	}

	if (strcmp(pTab->status, "OK"))
	{
		sys_log(0, "CInputDB::LoginSuccess - status[%s] is not OK [%s]", pTab->status, pTab->login);

		TLogoutPacket pack;

		strlcpy(pack.login, pTab->login, sizeof(pack.login));
		db_clientdesc->DBPacket(HEADER_GD_LOGOUT, dwHandle, &pack, sizeof(pack));

		LoginFailure(d, pTab->status);
		return;
	}

	for (int i = 0; i != PLAYER_PER_ACCOUNT; ++i)
	{
		TSimplePlayer& player = pTab->players[i];
		sys_log(0, "\tplayer(%s).job(%d)", player.szName, player.byJob);
	}

	bool bFound = GetServerLocation(*pTab, pTab->bEmpire);

	d->BindAccountTable(pTab);

	TPacketGCEmpire pe;
	pe.bHeader = HEADER_GC_EMPIRE;
	BYTE realEmpire = 0;
	for (int i = 0; i != PLAYER_PER_ACCOUNT; ++i)
	{
		TSimplePlayer& player = pTab->players[i];
		if (player.dwID)
		{
			realEmpire = d->GetEmpire();
			break;
		}
	}
	pe.bEmpire = realEmpire;
	d->Packet(&pe, sizeof(pe));

	d->SendLoginSuccessPacket();
	d->SetPhase(PHASE_SELECT);

	char bufPids[48 + 1];
	int bufPidsSize = snprintf(bufPids, sizeof(bufPids), " ");
	for (int i = 0; i != PLAYER_PER_ACCOUNT; ++i)
	{
		TSimplePlayer& player = pTab->players[i];
		DWORD pid = player.dwID;
		bufPidsSize += snprintf(bufPids + bufPidsSize, sizeof(bufPids) - bufPidsSize, "%d, ", pid);
	}

	sys_log(0, "InputDB::login_success: username(%s), pids(%s)", pTab->login, bufPids);
}

void CInputDB::PlayerCreateFailure(LPDESC d, BYTE bType)
{
	if (!d)
		return;

	TPacketGCCreateFailure pack;

	pack.header	= HEADER_GC_CHARACTER_CREATE_FAILURE;
	pack.bType	= bType;

	d->Packet(&pack, sizeof(pack));
}

void CInputDB::PlayerCreateSuccess(LPDESC d, const char * data)
{
	if (!d)
		return;

	TPacketDGCreateSuccess * pPacketDB = (TPacketDGCreateSuccess *) data;

	if (pPacketDB->bAccountCharacterIndex >= PLAYER_PER_ACCOUNT)
	{
		d->Packet(encode_byte(HEADER_GC_CHARACTER_CREATE_FAILURE), 1);
		return;
	}

	long lIndex = 0;

	if (!CMapLocation::instance().Get(pPacketDB->player.x, pPacketDB->player.y, lIndex, pPacketDB->player.lAddr, pPacketDB->player.wPort))
	{
		sys_err("InputDB::PlayerCreateSuccess: cannot find server for mapindex %d %d x %d (name %s)", lIndex, pPacketDB->player.x, pPacketDB->player.y, pPacketDB->player.szName);
	}

	TAccountTable & r_Tab = d->GetAccountTable();
	r_Tab.players[pPacketDB->bAccountCharacterIndex] = pPacketDB->player;

	TPacketGCPlayerCreateSuccess pack;

	pack.header = HEADER_GC_CHARACTER_CREATE_SUCCESS;
	pack.bAccountCharacterIndex = pPacketDB->bAccountCharacterIndex;
	pack.player = pPacketDB->player;

#ifdef ENABLE_PROXY_IP
	if (!g_stProxyIP.empty())
		pack.player.lAddr=inet_addr(g_stProxyIP.c_str());
#endif

	d->Packet(&pack, sizeof(TPacketGCPlayerCreateSuccess));

	TPlayerItem t;
	memset(&t, 0, sizeof(t));

	LogManager::instance().CharLog(pack.player.dwID, 0, 0, 0, "CREATE PLAYER", "", d->GetHostName());
}

void CInputDB::PlayerDeleteSuccess(LPDESC d, const char * data)
{
	if (!d)
		return;

	BYTE account_index;
	account_index = decode_byte(data);
	d->BufferedPacket(encode_byte(HEADER_GC_CHARACTER_DELETE_SUCCESS),	1);
	d->Packet(encode_byte(account_index),			1);

	d->GetAccountTable().players[account_index].dwID = 0;
}

void CInputDB::PlayerDeleteFail(LPDESC d)
{
	if (!d)
		return;

	d->Packet(encode_byte(HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID),	1);
}

void CInputDB::ChangeName(LPDESC d, const char * data)
{
	if (!d)
		return;

	TPacketDGChangeName * p = (TPacketDGChangeName *) data;

	TAccountTable & r = d->GetAccountTable();

	if (!r.id)
		return;

	for (size_t i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		if (r.players[i].dwID == p->pid)
		{
			strlcpy(r.players[i].szName, p->name, sizeof(r.players[i].szName));
			r.players[i].bChangeName = 0;

			TPacketGCChangeName pgc;

			pgc.header = HEADER_GC_CHANGE_NAME;
			pgc.pid = p->pid;
			strlcpy(pgc.name, p->name, sizeof(pgc.name));

			d->Packet(&pgc, sizeof(TPacketGCChangeName));
			break;
		}
}

void CInputDB::PlayerLoad(LPDESC d, const char * data)
{
	TPlayerTable * pTab = (TPlayerTable *) data;

	if (!d)
		return;

	long lMapIndex = pTab->lMapIndex;
	PIXEL_POSITION pos;

	if (lMapIndex == 0)
	{
		lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(pTab->x, pTab->y);

		if (lMapIndex == 0)
		{
			lMapIndex = EMPIRE_START_MAP(d->GetAccountTable().bEmpire);
			pos.x = EMPIRE_START_X(d->GetAccountTable().bEmpire);
			pos.y = EMPIRE_START_Y(d->GetAccountTable().bEmpire);
		}
		else
		{
			pos.x = pTab->x;
			pos.y = pTab->y;
		}
	}
	pTab->lMapIndex = lMapIndex;

	if (!SECTREE_MANAGER::instance().GetValidLocation(pTab->lMapIndex, pTab->x, pTab->y, lMapIndex, pos, d->GetEmpire()))
	{
		sys_err("InputDB::PlayerLoad : cannot find valid location %d x %d (name: %s)", pTab->x, pTab->y, pTab->name);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	pTab->x = pos.x;
	pTab->y = pos.y;
	pTab->lMapIndex = lMapIndex;

	if (d->GetCharacter() || d->IsPhase(PHASE_GAME))
	{
		LPCHARACTER p = d->GetCharacter();
		sys_err("login state already has main state (character %s %p)", p->GetName(), get_pointer(p));
		return;
	}

	if (NULL != CHARACTER_MANAGER::Instance().FindPC(pTab->name))
	{
		sys_err("InputDB: PlayerLoad : %s already exist in game", pTab->name);
		return;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pTab->name, pTab->id);

	ch->BindDesc(d);
	ch->SetPlayerProto(pTab);
	ch->SetEmpire(d->GetEmpire());

#ifdef ENABLE_COINS_INVENTORY
	long long coins = ch->GetCoins();
	ch->SetCoins(coins);
#endif

	d->BindCharacter(ch);

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, ch->GetName(), sizeof(p.szName));
		p.dwPID = ch->GetPlayerID();
		p.bEmpire = ch->GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(ch->GetX(), ch->GetY());
		p.bChannel = g_bChannel;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		p.bLanguage = ch->GetLanguage();
#endif

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));

		char buf[51];
		snprintf(buf, sizeof(buf), "%s %lld %d %ld %d", inet_ntoa(ch->GetDesc()->GetAddr().sin_addr), (long long) ch->GetGold(), (int) g_bChannel, (long) ch->GetMapIndex(), (int) ch->GetAlignment());

		LogManager::instance().CharLog(ch, 0, "LOGIN", buf);
	}

	d->SetPhase(PHASE_LOADING);
	ch->MainCharacterPacket();

	long lPublicMapIndex = lMapIndex >= 10000 ? lMapIndex / 10000 : lMapIndex;

	//if (!map_allow_find(lMapIndex >= 10000 ? lMapIndex / 10000 : lMapIndex) || !CheckEmpire(ch, lMapIndex))
	if (!map_allow_find(lPublicMapIndex))
	{
		sys_err("InputDB::PlayerLoad : entering %d map is not allowed here (name: %s, empire %u)",
				lMapIndex, pTab->name, d->GetEmpire());

		ch->SetWarpLocation(EMPIRE_START_MAP(d->GetEmpire()),
				EMPIRE_START_X(d->GetEmpire()) / 100,
				EMPIRE_START_Y(d->GetEmpire()) / 100);

		d->SetPhase(PHASE_CLOSE);
		return;
	}

	quest::CQuestManager::instance().BroadcastEventFlagOnLogin(ch);

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		ch->SetQuickslot(i, pTab->quickslot[i]);

	ch->PointsPacket();
	ch->SkillLevelPacket();

	sys_log(0, "InputDB: player_load %s %dx%dx%d LEVEL %d MOV_SPEED %d JOB %d ATG %d DFG %d GMLv %d",
			pTab->name,
			ch->GetX(), ch->GetY(), ch->GetZ(),
			ch->GetLevel(),
			ch->GetPoint(POINT_MOV_SPEED),
			ch->GetJob(),
			ch->GetPoint(POINT_ATT_GRADE),
			ch->GetPoint(POINT_DEF_GRADE),
			ch->GetGMLevel());

	ch->QuerySafeboxSize();
}

void CInputDB::Boot(const char* data)
{
	signal_timer_disable();

	DWORD dwPacketSize = decode_4bytes(data);
	data += 4;

	BYTE bVersion = decode_byte(data);
	data += 1;

	sys_log(0, "BOOT: PACKET: %d", dwPacketSize);
	sys_log(0, "BOOT: VERSION: %d", bVersion);
	if (bVersion != 6)
	{
		sys_err("boot version error");
		thecore_shutdown();
	}

	sys_log(0, "sizeof(TMobTable) = %d", sizeof(TMobTable));
	sys_log(0, "sizeof(TItemTable) = %d", sizeof(TItemTable));
	sys_log(0, "sizeof(TShopTable) = %d", sizeof(TShopTable));
	sys_log(0, "sizeof(TSkillTable) = %d", sizeof(TSkillTable));
	sys_log(0, "sizeof(TRefineTable) = %d", sizeof(TRefineTable));

	sys_log(0, "sizeof(TItemAttrTable) = %d", sizeof(TItemAttrTable));
	sys_log(0, "sizeof(TItemRareTable) = %d", sizeof(TItemAttrTable));
	sys_log(0, "sizeof(TBanwordTable) = %d", sizeof(TBanwordTable));
	sys_log(0, "sizeof(TLand) = %d", sizeof(building::TLand));
	sys_log(0, "sizeof(TObjectProto) = %d", sizeof(building::TObjectProto));
	sys_log(0, "sizeof(TObject) = %d", sizeof(building::TObject));
#ifdef ENABLE_GROWTH_PET_SYSTEM
	sys_log(0, "sizeof(TGrowthPetSkillTable) = %d", sizeof(TGrowthPetSkillTable));
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	sys_log(0, "sizeof(TOfflineShop) = %d", sizeof(TOfflineShop));
#endif
	//ADMIN_MANAGER
	sys_log(0, "sizeof(TAdminManager) = %d", sizeof (TAdminInfo) );
	//END_ADMIN_MANAGER

	WORD size;

	/*
	 * MOB
	 */

	if (decode_2bytes(data)!=sizeof(TMobTable))
	{
		sys_err("mob table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: MOB: %d", size);

	if (size)
	{
		CMobManager::instance().Initialize((TMobTable *) data, size);
		data += size * sizeof(TMobTable);
	}

	/*
	 * ITEM
	 */

	if (decode_2bytes(data) != sizeof(TItemTable))
	{
		sys_err("item table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: ITEM: %d", size);


	if (size)
	{
		ITEM_MANAGER::instance().Initialize((TItemTable *) data, size);
		data += size * sizeof(TItemTable);
	}

	/*
	 * SHOP
	 */

	if (decode_2bytes(data) != sizeof(TShopTable))
	{
		sys_err("shop table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: SHOP: %d", size);

	if (size)
	{
		if (!CShopManager::instance().Initialize((TShopTable *) data, size))
		{
			sys_err("shop table Initialize error");
			thecore_shutdown();
			return;
		}
		data += size * sizeof(TShopTable);
	}

#ifdef ENABLE_RENEWAL_SHOPEX
	if (decode_2bytes(data) != sizeof(TShopTable))
	{
		sys_err("shopex table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: SHOPEX: %d", size);

	if (size)
	{
		if (!CShopManager::instance().InitializeShopEX((TShopTable*)data, size))
		{
			sys_err("shopex table Initialize error");
			thecore_shutdown();
			return;
		}
		data += size * sizeof(TShopTable);
	}
#endif

	/*
	 * SKILL
	 */

	if (decode_2bytes(data) != sizeof(TSkillTable))
	{
		sys_err("skill table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: SKILL: %d", size);

	if (size)
	{
		if (!CSkillManager::instance().Initialize((TSkillTable *) data, size))
		{
			sys_err("cannot initialize skill table");
			thecore_shutdown();
			return;
		}

		data += size * sizeof(TSkillTable);
	}
	/*
	 * REFINE RECIPE
	 */
	if (decode_2bytes(data) != sizeof(TRefineTable))
	{
		sys_err("refine table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: REFINE: %d", size);

	if (size)
	{
		CRefineManager::instance().Initialize((TRefineTable*) data, size);
		data += size * sizeof(TRefineTable);
	}

	/*
	 * ITEM ATTR
	 */
	if (decode_2bytes(data) != sizeof(TItemAttrTable))
	{
		sys_err("item attr table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: ITEM_ATTR: %d", size);

	if (size)
	{
		TItemAttrTable * p = (TItemAttrTable *) data;

		for (int i = 0; i < size; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_itemAttr[p->dwApplyIndex] = *p;
			sys_log(0, "ITEM_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
	}

	data += size * sizeof(TItemAttrTable);


	/*
     * ITEM RARE
     */
	if (decode_2bytes(data) != sizeof(TItemAttrTable))
	{
		sys_err("item rare table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: ITEM_RARE: %d", size);

	if (size)
	{
		TItemAttrTable * p = (TItemAttrTable *) data;

		for (int i = 0; i < size; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_itemRare[p->dwApplyIndex] = *p;
			sys_log(0, "ITEM_RARE[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
	}

	data += size * sizeof(TItemAttrTable);

	if (decode_2bytes(data) != sizeof(TBanwordTable))
	{
		sys_err("ban word table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;

	CBanwordManager::instance().Initialize((TBanwordTable *) data, size);
	data += size * sizeof(TBanwordTable);

	{
		using namespace building;

		/*
		 * LANDS
		 */

		if (decode_2bytes(data) != sizeof(TLand))
		{
			sys_err("land table size error");
			thecore_shutdown();
			return;
		}
		data += 2;

		size = decode_2bytes(data);
		data += 2;

		TLand * kLand = (TLand *) data;
		data += size * sizeof(TLand);

		for (WORD i = 0; i < size; ++i, ++kLand)
			CManager::instance().LoadLand(kLand);

		/*
		 * OBJECT PROTO
		 */

		if (decode_2bytes(data) != sizeof(TObjectProto))
		{
			sys_err("object proto table size error");
			thecore_shutdown();
			return;
		}
		data += 2;

		size = decode_2bytes(data);
		data += 2;

		CManager::instance().LoadObjectProto((TObjectProto *) data, size);
		data += size * sizeof(TObjectProto);

		/*
		 * OBJECT
		 */
		if (decode_2bytes(data) != sizeof(TObject))
		{
			sys_err("object table size error");
			thecore_shutdown();
			return;
		}
		data += 2;

		size = decode_2bytes(data);
		data += 2;

		TObject * kObj = (TObject *) data;
		data += size * sizeof(TObject);

		for (WORD i = 0; i < size; ++i, ++kObj)
			CManager::instance().LoadObject(kObj, true);
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (decode_2bytes(data) != sizeof(TGrowthPetSkillTable))
	{
		sys_err("growth pet table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;
	sys_log(0, "BOOT: GROWTH PET SKILL: %d", size);

	if (size)
	{
		CGrowthPetManager::instance().InitializeSkill((TGrowthPetSkillTable*)data, size);
		data += size * sizeof(TGrowthPetSkillTable);
	}
#endif

	set_global_time(*(time_t *) data);
	data += sizeof(time_t);

	if (decode_2bytes(data) != sizeof(TItemIDRangeTable) )
	{
		sys_err("ITEM ID RANGE size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;

	TItemIDRangeTable* range = (TItemIDRangeTable*) data;
	data += size * sizeof(TItemIDRangeTable);

	TItemIDRangeTable* rangespare = (TItemIDRangeTable*) data;
	data += size * sizeof(TItemIDRangeTable);

	//ADMIN_MANAGER
	int ChunkSize = decode_2bytes(data );
	data += 2;
	int HostSize = decode_2bytes(data );
	data += 2;
	sys_log(0, "GM Value Count %d %d", HostSize, ChunkSize  );
	for (int n = 0; n < HostSize; ++n )
	{
		gm_new_host_inert(data );
		//sys_log(0, "GM HOST : IP[%s] ", data );
		data += ChunkSize;
	}

	data += 2;
	int adminsize = decode_2bytes(data );
	data += 2;

	for (int n = 0; n < adminsize; ++n )
	{
		tAdminInfo& rAdminInfo = *(tAdminInfo*)data;

		gm_new_insert(rAdminInfo );

		data += sizeof(rAdminInfo );
	}

	//END_ADMIN_MANAGER

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if (decode_2bytes(data) != sizeof(TOfflineShop))
	{
		sys_err("offlineshop table size error");
		thecore_shutdown();
		return;
	}
	data += 2;

	size = decode_2bytes(data);
	data += 2;

	std::vector<TOfflineShop> cache_data;
	cache_data.clear();
	cache_data.resize(size);

	if(size > 0)
		thecore_memcpy(&cache_data[0], (TOfflineShop*)data, sizeof(TOfflineShop)* size);

	data += size * sizeof(TOfflineShop);
#endif

	WORD endCheck=decode_2bytes(data);
	if (endCheck != 0xffff)
	{
		sys_err("boot packet end check error [%x]!=0xffff", endCheck);
		thecore_shutdown();
		return;
	}
	else
		sys_log(0, "boot packet end check ok [%x]==0xffff", endCheck );
	data +=2;

	if (!ITEM_MANAGER::instance().SetMaxItemID(*range))
	{
		sys_err("not enough item id contact your administrator!");
		thecore_shutdown();
		return;
	}

	if (!ITEM_MANAGER::instance().SetMaxSpareItemID(*rangespare))
	{
		sys_err("not enough item id for spare contact your administrator!");
		thecore_shutdown();
		return;
	}

	// LOCALE_SERVICE
	const int FILE_NAME_LEN = 256;
	char szCommonDropItemFileName[FILE_NAME_LEN];
	char szETCDropItemFileName[FILE_NAME_LEN];
	char szMOBDropItemFileName[FILE_NAME_LEN];
	char szDropItemGroupFileName[FILE_NAME_LEN];
	char szSpecialItemGroupFileName[FILE_NAME_LEN];
	char szMapIndexFileName[FILE_NAME_LEN];
	char szItemVnumMaskTableFileName[FILE_NAME_LEN];
	char szDragonSoulTableFileName[FILE_NAME_LEN];

#ifdef ENABLE_ATTENDANCE_EVENT
	char szAttendanceRewardFileName[FILE_NAME_LEN];
#endif

	snprintf(szCommonDropItemFileName, sizeof(szCommonDropItemFileName), "%s/common_drop_item.txt", LocaleService_GetBasePath().c_str());
	snprintf(szETCDropItemFileName, sizeof(szETCDropItemFileName), "%s/etc_drop_item.txt", LocaleService_GetBasePath().c_str());
	snprintf(szMOBDropItemFileName, sizeof(szMOBDropItemFileName), "%s/mob_drop_item.txt", LocaleService_GetBasePath().c_str());
	snprintf(szSpecialItemGroupFileName, sizeof(szSpecialItemGroupFileName), "%s/special_item_group.txt", LocaleService_GetBasePath().c_str());
	snprintf(szDropItemGroupFileName, sizeof(szDropItemGroupFileName), "%s/drop_item_group.txt", LocaleService_GetBasePath().c_str());
	snprintf(szMapIndexFileName, sizeof(szMapIndexFileName), "%s/index", LocaleService_GetMapPath().c_str());
	snprintf(szItemVnumMaskTableFileName, sizeof(szItemVnumMaskTableFileName), "%s/ori_to_new_table.txt", LocaleService_GetBasePath().c_str());
	snprintf(szDragonSoulTableFileName, sizeof(szDragonSoulTableFileName), "%s/dragon_soul_table.txt", LocaleService_GetBasePath().c_str());
#ifdef ENABLE_ATTENDANCE_EVENT
	snprintf(szAttendanceRewardFileName, sizeof(szAttendanceRewardFileName),
			"%s/attendance_reward.txt", LocaleService_GetBasePath().c_str());
#endif

	sys_log(0, "Initializing Informations of Cube System");
	if (!Cube_InformationInitialize())
	{
		sys_err("cannot init cube infomation.");
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: CommonDropItem: %s", szCommonDropItemFileName);
	if (!ITEM_MANAGER::instance().ReadCommonDropItemFile(szCommonDropItemFileName))
	{
		sys_err("cannot load CommonDropItem: %s", szCommonDropItemFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: ETCDropItem: %s", szETCDropItemFileName);
	if (!ITEM_MANAGER::instance().ReadEtcDropItemFile(szETCDropItemFileName))
	{
		sys_err("cannot load ETCDropItem: %s", szETCDropItemFileName);
		thecore_shutdown();
		return;
	}

#ifdef ENABLE_ATTENDANCE_EVENT
	sys_log(0, "LoadLocaleFile: AddendanceRewardList: %s", szAttendanceRewardFileName);
	if (!CMiniGame::instance().ReadRewardItemFile(szAttendanceRewardFileName))
	{
		sys_err("Cannot load AddendanceRewardList: %s", szAttendanceRewardFileName);
		//thecore_shutdown();
		//return;
	}
#endif

	sys_log(0, "LoadLocaleFile: DropItemGroup: %s", szDropItemGroupFileName);
	if (!ITEM_MANAGER::instance().ReadDropItemGroup(szDropItemGroupFileName))
	{
		sys_err("cannot load DropItemGroup: %s", szDropItemGroupFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: SpecialItemGroup: %s", szSpecialItemGroupFileName);
	if (!ITEM_MANAGER::instance().ReadSpecialDropItemFile(szSpecialItemGroupFileName))
	{
		sys_err("cannot load SpecialItemGroup: %s", szSpecialItemGroupFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: ItemVnumMaskTable : %s", szItemVnumMaskTableFileName);
	if (!ITEM_MANAGER::instance().ReadItemVnumMaskTable(szItemVnumMaskTableFileName))
	{
		sys_log(0, "Could not open MaskItemTable");
	}

	sys_log(0, "LoadLocaleFile: MOBDropItemFile: %s", szMOBDropItemFileName);
	if (!ITEM_MANAGER::instance().ReadMonsterDropItemGroup(szMOBDropItemFileName))
	{
		sys_err("cannot load MOBDropItemFile: %s", szMOBDropItemFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: MapIndex: %s", szMapIndexFileName);
	if (!SECTREE_MANAGER::instance().Build(szMapIndexFileName, LocaleService_GetMapPath().c_str()))
	{
		sys_err("cannot load MapIndex: %s", szMapIndexFileName);
		thecore_shutdown();
		return;
	}

	sys_log(0, "LoadLocaleFile: DragonSoulTable: %s", szDragonSoulTableFileName);
	if (!DSManager::instance().ReadDragonSoulTableFile(szDragonSoulTableFileName))
	{
		sys_err("cannot load DragonSoulTable: %s", szDragonSoulTableFileName);
		//thecore_shutdown();
		//return;
	}
	// END_OF_LOCALE_SERVICE

	building::CManager::instance().FinalizeBoot();

	CMotionManager::instance().Build();


#ifdef ENABLE_RENEWAL_OFFLINESHOP
	for (DWORD i = 0; i < cache_data.size(); i++)
		COfflineShopManager::Instance().CreateOfflineShop(&cache_data[i]);
#endif

	signal_timer_enable(30);

#ifdef ENABLE_ULTIMATE_REGEN
	char buf[250];
	snprintf(buf, sizeof(buf), "%s/newregen.txt", LocaleService_GetBasePath().c_str());
	CNewMobTimer::Instance().LoadFile(buf);
#endif

	if (test_server)
	{
		CMobManager::instance().DumpRegenCount("mob_count");
	}
#ifdef ENABLE_REAL_TIME_REGEN
	CRealTimeRegen::Instance().Initialize(); 
#endif
}

EVENTINFO(quest_login_event_info)
{
	DWORD dwPID;

	quest_login_event_info()
	: dwPID( 0 )
	{
	}
};

EVENTFUNC(quest_login_event)
{
	quest_login_event_info* info = dynamic_cast<quest_login_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "quest_login_event> <Factor> Null pointer" );
		return 0;
	}

	DWORD dwPID = info->dwPID;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (!ch)
		return 0;

	LPDESC d = ch->GetDesc();

	if (!d)
		return 0;

	if (d->IsPhase(PHASE_HANDSHAKE) ||
		d->IsPhase(PHASE_LOGIN) ||
		d->IsPhase(PHASE_SELECT) ||
		d->IsPhase(PHASE_DEAD) ||
		d->IsPhase(PHASE_LOADING))
	{
		return PASSES_PER_SEC(1);
	}
	else if (d->IsPhase(PHASE_CLOSE))
	{
		return 0;
	}
	else if (d->IsPhase(PHASE_GAME))
	{
		sys_log(0, "QUEST_LOAD: Login pc %d by event", ch->GetPlayerID());
		quest::CQuestManager::instance().Login(ch->GetPlayerID());
		return 0;
	}
	else
	{
		sys_err("input_db.cpp:quest_login_event INVALID PHASE pid %d", ch->GetPlayerID());
		return 0;
	}
}

void CInputDB::QuestLoad(LPDESC d, const char * c_pData)
{
	if (NULL == d)
		return;

	LPCHARACTER ch = d->GetCharacter();

	if (NULL == ch)
		return;

	const DWORD dwCount = decode_4bytes(c_pData);

	const TQuestTable* pQuestTable = reinterpret_cast<const TQuestTable*>(c_pData+4);

	if (NULL != pQuestTable)
	{
		if (dwCount != 0)
		{
			if (ch->GetPlayerID() != pQuestTable[0].dwPID)
			{
				sys_err("PID differs %u %u", ch->GetPlayerID(), pQuestTable[0].dwPID);
				return;
			}
		}

		sys_log(0, "QUEST_LOAD: count %d", dwCount);

		quest::PC * pkPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());

		if (!pkPC)
		{
			sys_err("null quest::PC with id %u", pQuestTable[0].dwPID);
			return;
		}

		if (pkPC->IsLoaded())
			return;

		for (unsigned int i = 0; i < dwCount; ++i)
		{
			std::string st(pQuestTable[i].szName);

			st += ".";
			st += pQuestTable[i].szState;

			sys_log(0,  "            %s %d", st.c_str(), pQuestTable[i].lValue);
			pkPC->SetFlag(st.c_str(), pQuestTable[i].lValue, false);
		}

		pkPC->SetLoaded();
		pkPC->Build();

		if (ch->GetDesc()->IsPhase(PHASE_GAME))
		{
			sys_log(0, "QUEST_LOAD: Login pc %d", pQuestTable[0].dwPID);
			quest::CQuestManager::instance().Login(pQuestTable[0].dwPID);
		}
		else
		{
			quest_login_event_info* info = AllocEventInfo<quest_login_event_info>();
			info->dwPID = ch->GetPlayerID();

			event_create(quest_login_event, info, PASSES_PER_SEC(1));
		}
	}
}

void CInputDB::SafeboxLoad(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	TSafeboxTable * p = (TSafeboxTable *) c_pData;

	if (d->GetAccountTable().id != p->dwID)
	{
		sys_err("SafeboxLoad: safebox has different id %u != %u", d->GetAccountTable().id, p->dwID);
		return;
	}

	if (!d->GetCharacter())
		return;

	BYTE bSize = 1;

	LPCHARACTER ch = d->GetCharacter();

	if (ch->GetShopOwner() || ch->GetExchange() || ch->GetMyShop() || ch->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		|| ch->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		|| ch->GetTransmutation()
#endif
		)
	{
		d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a Storeroom while another window is open."));
		d->GetCharacter()->CancelSafeboxLoad();
		return;
	}

	if (d->GetCharacter()->GetPremiumRemainSeconds(PREMIUM_SAFEBOX) > 0 || d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_LARGE_SAFEBOX))

#ifdef ENABLE_EXTENDED_SAFEBOX
		bSize = SAFEBOX_MAX_PAGE_COUNT;
#else
		bSize = 3;
#endif

	d->GetCharacter()->LoadSafebox(bSize * SAFEBOX_PAGE_SIZE, p->dwGold, p->wItemCount, (TPlayerItem *) (c_pData + sizeof(TSafeboxTable)));

#ifdef ENABLE_GROWTH_PET_SYSTEM
	c_pData += sizeof(TSafeboxTable) + sizeof(TPlayerItem) * p->wItemCount;

	WORD dwPetCount =*(WORD*)c_pData;
	c_pData += sizeof(WORD);

	d->GetCharacter()->GetSafebox()->LoadPet(dwPetCount, (TGrowthPet*)c_pData);
#endif
}

void CInputDB::SafeboxChangeSize(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	BYTE bSize = *(BYTE *) c_pData;

	if (!d->GetCharacter())
		return;

	d->GetCharacter()->ChangeSafeboxSize(bSize);
}

void CInputDB::SafeboxWrongPassword(LPDESC d)
{
	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	TPacketCGSafeboxWrongPassword p;
	p.bHeader = HEADER_GC_SAFEBOX_WRONG_PASSWORD;
	d->Packet(&p, sizeof(p));

	d->GetCharacter()->CancelSafeboxLoad();
}

void CInputDB::SafeboxChangePasswordAnswer(LPDESC d, const char* c_pData)
{
	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	TSafeboxChangePasswordPacketAnswer* p = (TSafeboxChangePasswordPacketAnswer*) c_pData;
	if (p->flag)
	{
		d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] Storeroom password has been changed."));
	}
	else
	{
		d->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered the wrong password."));
	}
}

void CInputDB::MallLoad(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	TSafeboxTable * p = (TSafeboxTable *) c_pData;

	if (d->GetAccountTable().id != p->dwID)
	{
		sys_err("safebox has different id %u != %u", d->GetAccountTable().id, p->dwID);
		return;
	}

	if (!d->GetCharacter())
		return;

	d->GetCharacter()->LoadMall(p->wItemCount, (TPlayerItem *) (c_pData + sizeof(TSafeboxTable)));
}

void CInputDB::LoginAlready(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	// INTERNATIONAL_VERSION
	{
		TPacketDGLoginAlready * p = (TPacketDGLoginAlready *) c_pData;

		LPDESC d2 = DESC_MANAGER::instance().FindByLoginName(p->szLogin);

		if (d2)
			d2->DisconnectOfSameLogin();
		else
		{
			TPacketGGDisconnect pgg;

			pgg.bHeader = HEADER_GG_DISCONNECT;
			strlcpy(pgg.szLogin, p->szLogin, sizeof(pgg.szLogin));

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGDisconnect));
		}
	}
	// END_OF_INTERNATIONAL_VERSION

	LoginFailure(d, "ALREADY");
}

void CInputDB::EmpireSelect(LPDESC d, const char * c_pData)
{
	sys_log(0, "EmpireSelect %p", get_pointer(d));

	if (!d)
		return;

	TAccountTable & rTable = d->GetAccountTable();
	rTable.bEmpire = *(BYTE *) c_pData;

	TPacketGCEmpire pe;
	pe.bHeader = HEADER_GC_EMPIRE;
	pe.bEmpire = rTable.bEmpire;
	d->Packet(&pe, sizeof(pe));

	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		if (rTable.players[i].dwID)
		{
			rTable.players[i].x = EMPIRE_START_X(rTable.bEmpire);
			rTable.players[i].y = EMPIRE_START_Y(rTable.bEmpire);
		}

	GetServerLocation(d->GetAccountTable(), rTable.bEmpire);

	d->SendLoginSuccessPacket();
}

void CInputDB::MapLocations(const char * c_pData)
{
	BYTE bCount = *(BYTE *) (c_pData++);

	sys_log(0, "InputDB::MapLocations %d", bCount);

	TMapLocation * pLoc = (TMapLocation *) c_pData;

	while (bCount--)
	{
		for (int i = 0; i < 64; ++i)
		{
			if (0 == pLoc->alMaps[i])
				break;

			CMapLocation::instance().Insert(pLoc->alMaps[i], pLoc->szHost, pLoc->wPort);
		}

		pLoc++;
	}
}

void CInputDB::P2P(const char * c_pData)
{
	extern LPFDWATCH main_fdw;

	TPacketDGP2P * p = (TPacketDGP2P *) c_pData;

	P2P_MANAGER& mgr = P2P_MANAGER::instance();

	if (false == DESC_MANAGER::instance().IsP2PDescExist(p->szHost, p->wPort))
	{
	    LPCLIENT_DESC pkDesc = NULL;
		sys_log(0, "InputDB:P2P %s:%u", p->szHost, p->wPort);
	    pkDesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, p->szHost, p->wPort, PHASE_P2P, false);
		mgr.RegisterConnector(pkDesc);
		pkDesc->SetP2P(p->szHost, p->wPort, p->bChannel);
	}
}

void CInputDB::GuildLoad(const char * c_pData)
{
	CGuildManager::instance().LoadGuild(*(DWORD *) c_pData);
}

void CInputDB::GuildSkillUpdate(const char* c_pData)
{
	TPacketGuildSkillUpdate * p = (TPacketGuildSkillUpdate *) c_pData;

	CGuild * g = CGuildManager::instance().TouchGuild(p->guild_id);

	if (g)
	{
		g->UpdateSkill(p->skill_point, p->skill_levels);
		g->GuildPointChange(POINT_SP, p->amount, p->save?true:false);
	}
}

void CInputDB::GuildWar(const char* c_pData)
{
	TPacketGuildWar * p = (TPacketGuildWar*) c_pData;

	sys_log(0, "InputDB::GuildWar %u %u state %d", p->dwGuildFrom, p->dwGuildTo, p->bWar);

	switch (p->bWar)
	{
		case GUILD_WAR_SEND_DECLARE:
		case GUILD_WAR_RECV_DECLARE:
			CGuildManager::instance().DeclareWar(p->dwGuildFrom, p->dwGuildTo, p->bType);
			break;

		case GUILD_WAR_REFUSE:
			CGuildManager::instance().RefuseWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_WAIT_START:
			CGuildManager::instance().WaitStartWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_CANCEL:
			CGuildManager::instance().CancelWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_ON_WAR:
			CGuildManager::instance().StartWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_END:
			CGuildManager::instance().EndWar(p->dwGuildFrom, p->dwGuildTo);
			break;

		case GUILD_WAR_OVER:
			CGuildManager::instance().WarOver(p->dwGuildFrom, p->dwGuildTo, p->bType);
			break;

		case GUILD_WAR_RESERVE:
			CGuildManager::instance().ReserveWar(p->dwGuildFrom, p->dwGuildTo, p->bType);
			break;

		default:
			sys_err("Unknown guild war state");
			break;
	}
}

void CInputDB::GuildWarScore(const char* c_pData)
{
	TPacketGuildWarScore* p = (TPacketGuildWarScore*) c_pData;
	CGuild * g = CGuildManager::instance().TouchGuild(p->dwGuildGainPoint);
	g->SetWarScoreAgainstTo(p->dwGuildOpponent, p->lScore);
}

void CInputDB::GuildSkillRecharge()
{
	CGuildManager::instance().SkillRecharge();
}

void CInputDB::GuildExpUpdate(const char* c_pData)
{
	TPacketGuildSkillUpdate * p = (TPacketGuildSkillUpdate *) c_pData;
	sys_log(1, "GuildExpUpdate %d", p->amount);

	CGuild * g = CGuildManager::instance().TouchGuild(p->guild_id);

	if (g)
		g->GuildPointChange(POINT_EXP, p->amount);
}

void CInputDB::GuildAddMember(const char* c_pData)
{
	TPacketDGGuildMember * p = (TPacketDGGuildMember *) c_pData;
	CGuild * g = CGuildManager::instance().TouchGuild(p->dwGuild);

	if (g)
		g->AddMember(p);
}

void CInputDB::GuildRemoveMember(const char* c_pData)
{
	TPacketGuild* p=(TPacketGuild*)c_pData;
	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);

	if (g)
		g->RemoveMember(p->dwInfo);
}

void CInputDB::GuildChangeGrade(const char* c_pData)
{
	TPacketGuild* p=(TPacketGuild*)c_pData;
	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);

	if (g)
		g->P2PChangeGrade((BYTE)p->dwInfo);
}

void CInputDB::GuildChangeMemberData(const char* c_pData)
{
	sys_log(0, "Recv GuildChangeMemberData");
	TPacketGuildChangeMemberData * p = (TPacketGuildChangeMemberData *) c_pData;
	CGuild * g = CGuildManager::instance().TouchGuild(p->guild_id);

	if (g)
		g->ChangeMemberData(p->pid, p->offer, p->level, p->grade);
}

void CInputDB::GuildDisband(const char* c_pData)
{
	TPacketGuild * p = (TPacketGuild*) c_pData;
	CGuildManager::instance().DisbandGuild(p->dwGuild);
}

void CInputDB::GuildLadder(const char* c_pData)
{
	TPacketGuildLadder* p = (TPacketGuildLadder*) c_pData;
	sys_log(0, "Recv GuildLadder %u %d / w %d d %d l %d", p->dwGuild, p->lLadderPoint, p->lWin, p->lDraw, p->lLoss);
	CGuild * g = CGuildManager::instance().TouchGuild(p->dwGuild);

	g->SetLadderPoint(p->lLadderPoint);
	g->SetWarData(p->lWin, p->lDraw, p->lLoss);
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CInputDB::SkillColorLoad(LPDESC d, const char* c_pData)
{
	LPCHARACTER ch;

	if (!d || !(ch = d->GetCharacter()))
		return;

	ch->SetSkillColor((DWORD*)c_pData);
}
#endif

void CInputDB::ItemLoad(LPDESC d, const char * c_pData)
{
	LPCHARACTER ch;

	if (!d || !(ch = d->GetCharacter()))
		return;

	if (ch->IsItemLoaded())
		return;

	DWORD dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	sys_log(0, "ITEM_LOAD: COUNT %s %u", ch->GetName(), dwCount);

	std::vector<LPITEM> v;

	TPlayerItem * p = (TPlayerItem *) c_pData;

	for (DWORD i = 0; i < dwCount; ++i, ++p)
	{
		LPITEM item = ITEM_MANAGER::instance().CreateItem(p->vnum, p->count, p->id);

		if (!item)
		{
			sys_err("cannot create item by vnum %u (name %s id %u)", p->vnum, ch->GetName(), p->id);
			continue;
		}

		item->SetSkipSave(true);
		item->SetSockets(p->alSockets);
		item->SetAttributes(p->aAttr);
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		item->SetTransmutationVnum(p->dwTransmutationVnum);
#endif
#ifdef ENABLE_SET_ITEM
		item->SetSetValue(p->dwSetValue);
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
		item->SetSealDate(p->sealbind);
#endif

		if ((p->window == INVENTORY && ch->GetInventoryItem(p->pos)) ||
				(p->window == EQUIPMENT && ch->GetWear(p->pos)))
		{
			sys_log(0, "ITEM_RESTORE: %s %s", ch->GetName(), item->GetName());
			v.push_back(item);
		}
		else
		{
			switch (p->window)
			{
				case INVENTORY:
				case DRAGON_SOUL_INVENTORY:
#ifdef ENABLE_RENEWAL_SWITCHBOT
				case SWITCHBOT:
#endif
#ifdef ENABLE_SPECIAL_INVENTORY
				case SKILL_BOOK_INVENTORY:
				case UPGRADE_ITEMS_INVENTORY:
				case STONE_INVENTORY:
				case GIFT_BOX_INVENTORY:
				case CHANGERS_INVENTORY:
#endif
#ifdef ENABLE_PICKUP_ITEM_EFFECT
					item->AddToCharacter(ch, TItemPos(p->window, p->pos), false);
#else
					item->AddToCharacter(ch, TItemPos(p->window, p->pos));
#endif
					break;

				case EQUIPMENT:
					if (item->CheckItemUseLevel(ch->GetLevel()) == true)
					{
						if (item->EquipTo(ch, p->pos) == false )
						{
							v.push_back(item);
						}
					}
					else
					{
						v.push_back(item);
					}
					break;
			}
		}

		if (false == item->OnAfterCreatedItem())
			sys_err("Failed to call ITEM::OnAfterCreatedItem (vnum: %d, id: %d)", item->GetVnum(), item->GetID());

		item->SetSkipSave(false);
	}

	itertype(v) it = v.begin();

	while (it != v.end())
	{
		LPITEM item = *(it++);

		int pos = ch->GetEmptyInventory(item->GetSize());

#ifdef ENABLE_SPECIAL_INVENTORY
		if (item->IsSkillBook())
		{
			pos = ch->GetEmptySkillBookInventory(item->GetSize());
		}
		else if (item->IsUpgradeItem())
		{
			pos = ch->GetEmptyUpgradeItemsInventory(item->GetSize());
		}
		else if (item->IsStone())
		{
			pos = ch->GetEmptyStoneInventory(item->GetSize());
		}
		else if (item->IsGiftBox())
		{
			pos = ch->GetEmptyGiftBoxInventory(item->GetSize());
		}
		else if (item->IsChanger())
		{
			pos = ch->GetEmptyChangersInventory(item->GetSize());
		}
#endif

		if (pos < 0)
		{
			PIXEL_POSITION coord;
			coord.x = ch->GetX();
			coord.y = ch->GetY();

			item->AddToGround(ch->GetMapIndex(), coord);
			item->SetOwnership(ch, 180);
			item->StartDestroyEvent();
		}
		else
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(ch, TItemPos(INVENTORY, pos), false);
#else
			item->AddToCharacter(ch, TItemPos(INVENTORY, pos));
#endif
	}

	ch->CheckMaximumPoints();
	ch->PointsPacket();

	ch->SetItemLoaded();
}

void CInputDB::AffectLoad(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	LPCHARACTER ch = d->GetCharacter();

	DWORD dwPID = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	DWORD dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	if (ch->GetPlayerID() != dwPID)
		return;

	ch->LoadAffect(dwCount, (TPacketAffectElement *) c_pData);
}

void CInputDB::PartyCreate(const char* c_pData)
{
	TPacketPartyCreate* p = (TPacketPartyCreate*) c_pData;
	CPartyManager::instance().P2PCreateParty(p->dwLeaderPID);
}

void CInputDB::PartyDelete(const char* c_pData)
{
	TPacketPartyDelete* p = (TPacketPartyDelete*) c_pData;
	CPartyManager::instance().P2PDeleteParty(p->dwLeaderPID);
}

void CInputDB::PartyAdd(const char* c_pData)
{
	TPacketPartyAdd* p = (TPacketPartyAdd*) c_pData;
	CPartyManager::instance().P2PJoinParty(p->dwLeaderPID, p->dwPID, p->bState);
}

void CInputDB::PartyRemove(const char* c_pData)
{
	TPacketPartyRemove* p = (TPacketPartyRemove*) c_pData;
	CPartyManager::instance().P2PQuitParty(p->dwPID);
}

void CInputDB::PartyStateChange(const char* c_pData)
{
	TPacketPartyStateChange * p = (TPacketPartyStateChange *) c_pData;
	LPPARTY pParty = CPartyManager::instance().P2PCreateParty(p->dwLeaderPID);

	if (!pParty)
		return;

	pParty->SetRole(p->dwPID, p->bRole, p->bFlag);
}

void CInputDB::PartySetMemberLevel(const char* c_pData)
{
	TPacketPartySetMemberLevel* p = (TPacketPartySetMemberLevel*) c_pData;
	LPPARTY pParty = CPartyManager::instance().P2PCreateParty(p->dwLeaderPID);

	if (!pParty)
		return;

	pParty->P2PSetMemberLevel(p->dwPID, p->bLevel);
}

void CInputDB::Time(const char * c_pData)
{
	set_global_time(*(time_t *) c_pData);
}

void CInputDB::ReloadProto(const char * c_pData)
{
	WORD wSize;

	/*
	 * Skill
	 */
	wSize = decode_2bytes(c_pData);
	c_pData += sizeof(WORD);
	if (wSize) CSkillManager::instance().Initialize((TSkillTable *) c_pData, wSize);
	c_pData += sizeof(TSkillTable) * wSize;

	/*
	 * Banwords
	 */

	wSize = decode_2bytes(c_pData);
	c_pData += sizeof(WORD);
	CBanwordManager::instance().Initialize((TBanwordTable *) c_pData, wSize);
	c_pData += sizeof(TBanwordTable) * wSize;

	/*
	 * ITEM
	 */
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: ITEM: %d", wSize);

	if (wSize)
	{
		ITEM_MANAGER::instance().Initialize((TItemTable *) c_pData, wSize);
		c_pData += wSize * sizeof(TItemTable);
	}

	/*
	 * MONSTER
	 */
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: MOB: %d", wSize);

	if (wSize)
	{
		CMobManager::instance().Initialize((TMobTable *) c_pData, wSize);
		c_pData += wSize * sizeof(TMobTable);
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: Growth Pet Skill: %d", wSize);

	if (wSize)
	{
		CGrowthPetManager::instance().InitializeSkill((TGrowthPetSkillTable*)c_pData, wSize);
		c_pData += wSize * sizeof(TGrowthPetSkillTable);
	}
#endif

	/*
	* SHOP
	*/

	wSize = decode_2bytes(c_pData);
	c_pData += sizeof(WORD);
	sys_log(0, "RELOAD: SHOP: %d", wSize);

	if (wSize)
	{
		CShopManager::instance().Initialize((TShopTable *)c_pData, wSize);
		c_pData += wSize * sizeof(TShopTable);
	}

#ifdef ENABLE_RENEWAL_SHOPEX
	wSize = decode_2bytes(c_pData);
	c_pData += sizeof(WORD);
	sys_log(0, "RELOAD: SHOPEX: %d", wSize);

	if (wSize)
	{
		CShopManager::instance().InitializeShopEX((TShopTable *)c_pData, wSize);
		c_pData += wSize * sizeof(TShopTable);
	}
#endif

	/*
	* REFINE
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: REFINE: %d", wSize);

	if (wSize)
	{
		CRefineManager::instance().Initialize((TRefineTable *)c_pData, wSize);
		c_pData += wSize * sizeof(TRefineTable);
	}

	/*
	* ATTR
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: ItemAtt: %d", wSize);

	if (wSize)
	{
		TItemAttrTable * p = (TItemAttrTable *)c_pData;
		g_map_itemAttr.clear();
		for (int i = 0; i < wSize; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_itemAttr[p->dwApplyIndex] = *p;
			sys_log(0, "ITEM_ATTR[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
		c_pData += wSize*sizeof(TItemAttrTable);
	}

	/*
	* ATTR_RARE
	*/
	wSize = decode_2bytes(c_pData);
	c_pData += 2;
	sys_log(0, "RELOAD: ItemRareAtt: %d", wSize);

	if (wSize)
	{
		TItemAttrTable * p = (TItemAttrTable *)c_pData;
		g_map_itemRare.clear();
		for (int i = 0; i < wSize; ++i, ++p)
		{
			if (p->dwApplyIndex >= MAX_APPLY_NUM)
				continue;

			g_map_itemRare[p->dwApplyIndex] = *p;
			sys_log(0, "ITEM_RARE[%d]: %s %u", p->dwApplyIndex, p->szApply, p->dwProb);
		}
		c_pData += wSize*sizeof(TItemAttrTable);
	}

	CMotionManager::instance().Build();
	CHARACTER_MANAGER::instance().for_each_pc(std::mem_fn(&CHARACTER::ComputePoints));
}

void CInputDB::GuildSkillUsableChange(const char* c_pData)
{
	TPacketGuildSkillUsableChange* p = (TPacketGuildSkillUsableChange*) c_pData;

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);

	g->SkillUsableChange(p->dwSkillVnum, p->bUsable?true:false);
}

void CInputDB::AuthLogin(LPDESC d, const char * c_pData)
{
	if (!d)
		return;

	BYTE bResult = *(BYTE *) c_pData;

	TPacketGCAuthSuccess ptoc;

	ptoc.bHeader = HEADER_GC_AUTH_SUCCESS;

	if (bResult)
	{
		ptoc.dwLoginKey = d->GetLoginKey();
	}
	else
	{
		ptoc.dwLoginKey = 0;
	}

	ptoc.bResult = bResult;

	d->Packet(&ptoc, sizeof(TPacketGCAuthSuccess));
	sys_log(0, "AuthLogin result %u key %u", bResult, d->GetLoginKey());
}

void CInputDB::ChangeEmpirePriv(const char* c_pData)
{
	TPacketDGChangeEmpirePriv* p = (TPacketDGChangeEmpirePriv*) c_pData;

	// ADD_EMPIRE_PRIV_TIME
	CPrivManager::instance().GiveEmpirePriv(p->empire, p->type, p->value, p->bLog, p->end_time_sec);
	// END_OF_ADD_EMPIRE_PRIV_TIME
}

void CInputDB::ChangeGuildPriv(const char* c_pData)
{
	TPacketDGChangeGuildPriv* p = (TPacketDGChangeGuildPriv*) c_pData;

	// ADD_GUILD_PRIV_TIME
	CPrivManager::instance().GiveGuildPriv(p->guild_id, p->type, p->value, p->bLog, p->end_time_sec);
	// END_OF_ADD_GUILD_PRIV_TIME
}

void CInputDB::ChangeCharacterPriv(const char* c_pData)
{
	TPacketDGChangeCharacterPriv* p = (TPacketDGChangeCharacterPriv*) c_pData;
	CPrivManager::instance().GiveCharacterPriv(p->pid, p->type, p->value, p->bLog);
}

void CInputDB::MoneyLog(const char* c_pData)
{
	TPacketMoneyLog * p = (TPacketMoneyLog *) c_pData;

	if (p->type == 4) // QUEST_MONEY_LOG_SKIP
		return;

	if (g_bAuthServer ==true )
		return;

	LogManager::instance().MoneyLog(p->type, p->vnum, p->gold);
}

void CInputDB::GuildMoneyChange(const char* c_pData)
{
	TPacketDGGuildMoneyChange* p = (TPacketDGGuildMoneyChange*) c_pData;

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);
	if (g)
	{
		g->RecvMoneyChange(p->iTotalGold);
	}
}

void CInputDB::GuildWithdrawMoney(const char* c_pData)
{
	TPacketDGGuildMoneyWithdraw* p = (TPacketDGGuildMoneyWithdraw*) c_pData;

	CGuild* g = CGuildManager::instance().TouchGuild(p->dwGuild);
	if (g)
	{
		g->RecvWithdrawMoneyGive(p->iChangeGold);
	}
}

void CInputDB::SetEventFlag(const char* c_pData)
{
	TPacketSetEventFlag* p = (TPacketSetEventFlag*) c_pData;
	quest::CQuestManager::instance().SetEventFlag(p->szFlagName, p->lValue);
}

void CInputDB::CreateObject(const char * c_pData)
{
	using namespace building;
	CManager::instance().LoadObject((TObject *) c_pData);
}

void CInputDB::DeleteObject(const char * c_pData)
{
	using namespace building;
	CManager::instance().DeleteObject(*(DWORD *) c_pData);
}

void CInputDB::UpdateLand(const char * c_pData)
{
	using namespace building;
	CManager::instance().UpdateLand((TLand *) c_pData);
}

void CInputDB::Notice(const char * c_pData)
{
	extern void SendNotice(const char* c_pszBuf);

	char szBuf[256+1];
	strlcpy(szBuf, c_pData, sizeof(szBuf));

	sys_log(0, "InputDB:: Notice: %s", szBuf);

	SendNotice(szBuf);
}

void CInputDB::GuildWarReserveAdd(TGuildWarReserve * p)
{
	CGuildManager::instance().ReserveWarAdd(p);
}

void CInputDB::GuildWarReserveDelete(DWORD dwID)
{
	CGuildManager::instance().ReserveWarDelete(dwID);
}

void CInputDB::GuildWarBet(TPacketGDGuildWarBet * p)
{
	CGuildManager::instance().ReserveWarBet(p);
}

void CInputDB::MarriageAdd(TPacketMarriageAdd * p)
{
	sys_log(0, "MarriageAdd %u %u %u %s %s", p->dwPID1, p->dwPID2, (DWORD)p->tMarryTime, p->szName1, p->szName2);
	marriage::CManager::instance().Add(p->dwPID1, p->dwPID2, p->tMarryTime, p->szName1, p->szName2);
}

void CInputDB::MarriageUpdate(TPacketMarriageUpdate * p)
{
	sys_log(0, "MarriageUpdate %u %u %d %d", p->dwPID1, p->dwPID2, p->iLovePoint, p->byMarried);
	marriage::CManager::instance().Update(p->dwPID1, p->dwPID2, p->iLovePoint, p->byMarried);
}

void CInputDB::MarriageRemove(TPacketMarriageRemove * p)
{
	sys_log(0, "MarriageRemove %u %u", p->dwPID1, p->dwPID2);
	marriage::CManager::instance().Remove(p->dwPID1, p->dwPID2);
}

void CInputDB::WeddingRequest(TPacketWeddingRequest* p)
{
	marriage::WeddingManager::instance().Request(p->dwPID1, p->dwPID2);
}

void CInputDB::WeddingReady(TPacketWeddingReady* p)
{
	sys_log(0, "WeddingReady %u %u %u", p->dwPID1, p->dwPID2, p->dwMapIndex);
	marriage::CManager::instance().WeddingReady(p->dwPID1, p->dwPID2, p->dwMapIndex);
}

void CInputDB::WeddingStart(TPacketWeddingStart* p)
{
	sys_log(0, "WeddingStart %u %u", p->dwPID1, p->dwPID2);
	marriage::CManager::instance().WeddingStart(p->dwPID1, p->dwPID2);
}

void CInputDB::WeddingEnd(TPacketWeddingEnd* p)
{
	sys_log(0, "WeddingEnd %u %u", p->dwPID1, p->dwPID2);
	marriage::CManager::instance().WeddingEnd(p->dwPID1, p->dwPID2);
}
//RELOAD_ADMIN

void CInputDB::MyshopPricelistRes(LPDESC d, const TPacketMyshopPricelistHeader* p )
{
	LPCHARACTER ch;

	if (!d || !(ch = d->GetCharacter()) )
		return;

	sys_log(0, "RecvMyshopPricelistRes name[%s]", ch->GetName());
	ch->UseSilkBotaryReal(p );

}

void CInputDB::ReloadAdmin(const char * c_pData )
{
	gm_new_clear();
	int ChunkSize = decode_2bytes(c_pData );
	c_pData += 2;
	int HostSize = decode_2bytes(c_pData );
	c_pData += 2;

	for (int n = 0; n < HostSize; ++n )
	{
		gm_new_host_inert(c_pData );
		c_pData += ChunkSize;
	}


	c_pData += 2;
	int size = 	decode_2bytes(c_pData );
	c_pData += 2;

	for (int n = 0; n < size; ++n )
	{
		tAdminInfo& rAdminInfo = *(tAdminInfo*)c_pData;

		gm_new_insert(rAdminInfo );

		c_pData += sizeof (tAdminInfo );

		LPCHARACTER pChar = CHARACTER_MANAGER::instance().FindPC(rAdminInfo.m_szName );
		if (pChar )
		{
			pChar->SetGMLevel();
		}
	}
}

int CInputDB::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	switch (bHeader)
	{
		case HEADER_DG_BOOT:
			Boot(c_pData);
			break;

		case HEADER_DG_LOGIN_SUCCESS:
			LoginSuccess(m_dwHandle, c_pData);
			break;

		case HEADER_DG_LOGIN_NOT_EXIST:
			LoginFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), "NOID");
			break;

		case HEADER_DG_LOGIN_WRONG_PASSWD:
			LoginFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), "WRONGPWD");
			break;

		case HEADER_DG_LOGIN_ALREADY:
			LoginAlready(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_PLAYER_LOAD_SUCCESS:
			PlayerLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_PLAYER_CREATE_SUCCESS:
			PlayerCreateSuccess(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_PLAYER_CREATE_FAILED:
			PlayerCreateFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), 0);
			break;

		case HEADER_DG_PLAYER_CREATE_ALREADY:
			PlayerCreateFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), 1);
			break;

		case HEADER_DG_PLAYER_DELETE_SUCCESS:
			PlayerDeleteSuccess(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_PLAYER_LOAD_FAILED:
			break;

		case HEADER_DG_PLAYER_DELETE_FAILED:
			PlayerDeleteFail(DESC_MANAGER::instance().FindByHandle(m_dwHandle));
			break;

		case HEADER_DG_ITEM_LOAD:
			ItemLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_QUEST_LOAD:
			QuestLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_AFFECT_LOAD:
			AffectLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_SAFEBOX_LOAD:
			SafeboxLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_SAFEBOX_CHANGE_SIZE:
			SafeboxChangeSize(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_SAFEBOX_WRONG_PASSWORD:
			SafeboxWrongPassword(DESC_MANAGER::instance().FindByHandle(m_dwHandle));
			break;

		case HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER:
			SafeboxChangePasswordAnswer(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_MALL_LOAD:
			MallLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_EMPIRE_SELECT:
			EmpireSelect(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_MAP_LOCATIONS:
			MapLocations(c_pData);
			break;

		case HEADER_DG_P2P:
			P2P(c_pData);
			break;

		case HEADER_DG_GUILD_SKILL_UPDATE:
			GuildSkillUpdate(c_pData);
			break;

		case HEADER_DG_GUILD_LOAD:
			GuildLoad(c_pData);
			break;

		case HEADER_DG_GUILD_SKILL_RECHARGE:
			GuildSkillRecharge();
			break;

		case HEADER_DG_GUILD_EXP_UPDATE:
			GuildExpUpdate(c_pData);
			break;

		case HEADER_DG_PARTY_CREATE:
			PartyCreate(c_pData);
			break;

		case HEADER_DG_PARTY_DELETE:
			PartyDelete(c_pData);
			break;

		case HEADER_DG_PARTY_ADD:
			PartyAdd(c_pData);
			break;

		case HEADER_DG_PARTY_REMOVE:
			PartyRemove(c_pData);
			break;

		case HEADER_DG_PARTY_STATE_CHANGE:
			PartyStateChange(c_pData);
			break;

		case HEADER_DG_PARTY_SET_MEMBER_LEVEL:
			PartySetMemberLevel(c_pData);    
			break;

		case HEADER_DG_TIME:
			Time(c_pData);
			break;

		case HEADER_DG_GUILD_ADD_MEMBER:
			GuildAddMember(c_pData);
			break;

		case HEADER_DG_GUILD_REMOVE_MEMBER:
			GuildRemoveMember(c_pData);
			break;

		case HEADER_DG_GUILD_CHANGE_GRADE:
			GuildChangeGrade(c_pData);
			break;

		case HEADER_DG_GUILD_CHANGE_MEMBER_DATA:
			GuildChangeMemberData(c_pData);
			break;

		case HEADER_DG_GUILD_DISBAND:
			GuildDisband(c_pData);
			break;

		case HEADER_DG_RELOAD_PROTO:
			ReloadProto(c_pData);
			break;

		case HEADER_DG_GUILD_WAR:
			GuildWar(c_pData);
			break;

		case HEADER_DG_GUILD_WAR_SCORE:
			GuildWarScore(c_pData);
			break;

		case HEADER_DG_GUILD_LADDER:
			GuildLadder(c_pData);
			break;

		case HEADER_DG_GUILD_SKILL_USABLE_CHANGE:
			GuildSkillUsableChange(c_pData);
			break;

		case HEADER_DG_CHANGE_NAME:
			ChangeName(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_AUTH_LOGIN:
			AuthLogin(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

		case HEADER_DG_CHANGE_EMPIRE_PRIV:
			ChangeEmpirePriv(c_pData);
			break;

		case HEADER_DG_CHANGE_GUILD_PRIV:
			ChangeGuildPriv(c_pData);
			break;

		case HEADER_DG_CHANGE_CHARACTER_PRIV:
			ChangeCharacterPriv(c_pData);
			break;

		case HEADER_DG_MONEY_LOG:
			MoneyLog(c_pData);
			break;

		case HEADER_DG_GUILD_WITHDRAW_MONEY_GIVE:
			GuildWithdrawMoney(c_pData);
			break;

		case HEADER_DG_GUILD_MONEY_CHANGE:
			GuildMoneyChange(c_pData);
			break;

		case HEADER_DG_SET_EVENT_FLAG:
			SetEventFlag(c_pData);
			break;

		case HEADER_DG_CREATE_OBJECT:
			CreateObject(c_pData);
			break;

		case HEADER_DG_DELETE_OBJECT:
			DeleteObject(c_pData);
			break;

		case HEADER_DG_UPDATE_LAND:
			UpdateLand(c_pData);
			break;

		case HEADER_DG_NOTICE:
			Notice(c_pData);
			break;

		case HEADER_DG_GUILD_WAR_RESERVE_ADD:
			GuildWarReserveAdd((TGuildWarReserve *) c_pData);
			break;

		case HEADER_DG_GUILD_WAR_RESERVE_DEL:
			GuildWarReserveDelete(*(DWORD *) c_pData);
			break;

		case HEADER_DG_GUILD_WAR_BET:
			GuildWarBet((TPacketGDGuildWarBet *) c_pData);
			break;

		case HEADER_DG_MARRIAGE_ADD:
			MarriageAdd((TPacketMarriageAdd*) c_pData);
			break;

		case HEADER_DG_MARRIAGE_UPDATE:
			MarriageUpdate((TPacketMarriageUpdate*) c_pData);
			break;

		case HEADER_DG_MARRIAGE_REMOVE:
			MarriageRemove((TPacketMarriageRemove*) c_pData);
			break;

		case HEADER_DG_WEDDING_REQUEST:
			WeddingRequest((TPacketWeddingRequest*) c_pData);
			break;

		case HEADER_DG_WEDDING_READY:
			WeddingReady((TPacketWeddingReady*) c_pData);
			break;

		case HEADER_DG_WEDDING_START:
			WeddingStart((TPacketWeddingStart*) c_pData);
			break;

		case HEADER_DG_WEDDING_END:
			WeddingEnd((TPacketWeddingEnd*) c_pData);
			break;

		case HEADER_DG_MYSHOP_PRICELIST_RES:
			MyshopPricelistRes(DESC_MANAGER::instance().FindByHandle(m_dwHandle), (TPacketMyshopPricelistHeader*) c_pData );
			break;

		case HEADER_DG_RELOAD_ADMIN:
			ReloadAdmin(c_pData );
			break;

		case HEADER_DG_ACK_CHANGE_GUILD_MASTER :
			this->GuildChangeMaster((TPacketChangeGuildMaster*) c_pData);
			break;

		case HEADER_DG_ACK_SPARE_ITEM_ID_RANGE :
			ITEM_MANAGER::instance().SetMaxSpareItemID(*((TItemIDRangeTable*)c_pData) );
			break;

		case HEADER_DG_UPDATE_HORSE_NAME :
		case HEADER_DG_ACK_HORSE_NAME :
			CHorseNameManager::instance().UpdateHorseName(((TPacketUpdateHorseName*)c_pData)->dwPlayerID, ((TPacketUpdateHorseName*)c_pData)->szHorseName);
			break;

		case HEADER_DG_NEED_LOGIN_LOG:
			DetailLog( (TPacketNeedLoginLogInfo*) c_pData );
			break;

		case HEADER_DG_ITEMAWARD_INFORMER:
			ItemAwardInformer((TPacketItemAwardInfromer*) c_pData);
			break;

		case HEADER_DG_RESPOND_CHANNELSTATUS:
			RespondChannelStatus(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		case HEADER_DG_SKILL_COLOR_LOAD:
			SkillColorLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
		case HEADER_DG_OFFLINESHOP:
			COfflineShopManager::Instance().RecvPackets(c_pData);
			break;
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
		case HEADER_DG_RESPOND_OFFLINE_MESSAGES:
			ReadOfflineMessages(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;
#endif

#ifdef ENABLE_ITEMSHOP
		case HEADER_DG_ITEMSHOP:
		{
			ItemShop(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
			break;
		}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	case HEADER_DG_GROWTH_PET_LOAD:
		GrowthPetLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), c_pData);
		break;
#endif

		default:
			return (-1);
	}

	return 0;
}

bool CInputDB::Process(LPDESC d, const void * orig, int bytes, int & r_iBytesProceed)
{
	const char* c_pData = (const char*) orig;
	BYTE bHeader, bLastHeader = 0;
	int iSize;
	int iLastPacketLen = 0;

	for (m_iBufferLeft = bytes; m_iBufferLeft > 0;)
	{
		if (m_iBufferLeft < 9)
			return true;

		bHeader = *((BYTE *) (c_pData));
		m_dwHandle = *((DWORD *) (c_pData + 1));
		iSize = *((DWORD *) (c_pData + 5));

		sys_log(1, "DBCLIENT: header %d handle %d size %d bytes %d", bHeader, m_dwHandle, iSize, bytes); 

		if (m_iBufferLeft - 9 < iSize)
			return true;

		const char * pRealData = (c_pData + 9);

		if (Analyze(d, bHeader, pRealData) < 0)
		{
			sys_err("in InputDB: UNKNOWN HEADER: %d, LAST HEADER: %d(%d), REMAIN BYTES: %d, DESC: %d", bHeader, bLastHeader, iLastPacketLen, m_iBufferLeft, d->GetSocket());
		}

		c_pData += 9 + iSize;
		m_iBufferLeft -= 9 + iSize;
		r_iBytesProceed += 9 + iSize;

		iLastPacketLen = 9 + iSize;
		bLastHeader = bHeader;
	}

	return true;
}

void CInputDB::GuildChangeMaster(TPacketChangeGuildMaster* p)
{
	CGuildManager::instance().ChangeMaster(p->dwGuildID);
}

void CInputDB::DetailLog(const TPacketNeedLoginLogInfo* info)
{
	LPCHARACTER pChar = CHARACTER_MANAGER::instance().FindByPID( info->dwPlayerID );

	if (NULL != pChar)
	{
		LogManager::instance().DetailLoginLog(true, pChar);
	}
}

void CInputDB::ItemAwardInformer(TPacketItemAwardInfromer *data)
{
	LPDESC d = DESC_MANAGER::instance().FindByLoginName(data->login);

	if(d == NULL)
		return;
	else
	{
		if (d->GetCharacter())
		{
			LPCHARACTER ch = d->GetCharacter();
			ch->SetItemAward_vnum(data->vnum);
			ch->SetItemAward_cmd(data->command);

			if(d->IsPhase(PHASE_GAME))
			{
				quest::CQuestManager::instance().ItemInformer(ch->GetPlayerID(),ch->GetItemAward_vnum());
			}
		}
	}
}

void CInputDB::RespondChannelStatus(LPDESC desc, const char* pcData) 
{
	if (!desc)
	{
		return;
	}

	const int nSize = decode_4bytes(pcData);
	pcData += sizeof(nSize);

	BYTE bHeader = HEADER_GC_RESPOND_CHANNELSTATUS;
	desc->BufferedPacket(&bHeader, sizeof(BYTE));
	desc->BufferedPacket(&nSize, sizeof(nSize));
	if (0 < nSize) {
		desc->BufferedPacket(pcData, sizeof(TChannelStatus)*nSize);
	}
	BYTE bSuccess = 1;
	desc->Packet(&bSuccess, sizeof(bSuccess));
	desc->SetChannelStatusRequested(false);
}

#ifdef ENABLE_OFFLINE_MESSAGE
void CInputDB::ReadOfflineMessages(LPDESC desc, const char* pcData)
{
	if (!desc || !desc->GetCharacter())
		return;

	if (desc->GetCharacter()->IsBlockMode(BLOCK_WHISPER))
		return;

	auto p = reinterpret_cast<const TPacketDGReadOfflineMessage*>(pcData);

	TPacketGCWhisper pack;
	int len = MIN(CHAT_MAX_LEN, strlen(p->szMessage) + 1);
	pack.bHeader = HEADER_GC_WHISPER;
	pack.wSize = static_cast<WORD>(sizeof(TPacketGCWhisper) + len);
	pack.bType = WHISPER_TYPE_OFFLINE;
	strlcpy(pack.szNameFrom, p->szFrom, sizeof(pack.szNameFrom));

	TEMP_BUFFER buf;
	buf.write(&pack, sizeof(TPacketGCWhisper));
	buf.write(p->szMessage, len);
	desc->Packet(buf.read_peek(), buf.size());
}
#endif

#ifdef ENABLE_ITEMSHOP
void CInputDB::ItemShop(LPDESC d, const char* c_pData)
{
	const BYTE subIndex = *(BYTE*)c_pData;
	c_pData += sizeof(BYTE);

	switch (subIndex)
	{
		case ITEMSHOP_LOAD:
		{
			CHARACTER_MANAGER::Instance().LoadItemShopData(c_pData);
			break;
		}
		case ITEMSHOP_LOG:
		{
			if (!d)
			{
				return;
			}

			CHARACTER_MANAGER::Instance().LoadItemShopLogReal(d->GetCharacter(), c_pData);
			break;
		}
		case ITEMSHOP_BUY:
		{
			if (!d)
			{
				return;
			}

			CHARACTER_MANAGER::Instance().LoadItemShopBuyReal(d->GetCharacter(), c_pData);
			break;
		}
		case ITEMSHOP_UPDATE_ITEM:
		{
			CHARACTER_MANAGER::Instance().UpdateItemShopItem(c_pData);
			break;
		}
		default:
		{
			sys_err("unknown subIndex %u", subIndex);
			return;
		}
	}
}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
void CInputDB::GrowthPetLoad(LPDESC d, const char* c_pData)
{
	LPCHARACTER ch;

	if (!d || !(ch = d->GetCharacter()))
		return;

	if (ch->IsGrowthPetLoaded())
		return;

	DWORD dwCount = decode_4bytes(c_pData);
	c_pData += sizeof(DWORD);

	sys_log(0, "GROWTH_PET_LOAD: COUNT %s %u", ch->GetName(), dwCount);

	TGrowthPet* p = (TGrowthPet*)c_pData;

	for (DWORD i = 0; i < dwCount; ++i, ++p)
	{
		LPGROWTH_PET pPet = CGrowthPetManager::Instance().CreateGrowthPet(ch, p->dwID);
		if (pPet)
		{
			pPet->SetGrowthPetProto(p);
			ch->SetGrowthPet(pPet);
		}
	}

	ch->SetGrowthPetLoaded(true);
}
#endif
