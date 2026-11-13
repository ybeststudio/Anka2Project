#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_WARDROBE_SYSTEM
#include "utils.h"
#include "config.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "db.h"
#include "log.h"
#include "locale_service.h"
#include <fstream>
#include "wardrobe.h"
#if defined(ENABLE_WARDROBE_AUTO_STORE) && defined(ENABLE_RENEWAL_SWITCHBOT)
#include "switchbot_manager.h"
#endif

#include <sstream>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>

namespace
{
	struct SWardrobeOutfit
	{
		DWORD		revision = 0;
		bool		used = false;
		std::string	nameHex;
		DWORD		looks[WARDROBE_LOOK_COUNT] = {};
	};

	struct SWardrobeData
	{
		DWORD			looks[WARDROBE_LOOK_COUNT] = {};
		BYTE			remembered = 0;	// slot + 1, 0 = none
		BYTE			applied = 0;	// slot + 1, 0 = none
		SWardrobeOutfit	outfits[WARDROBE_OUTFIT_MAX];
		DWORD			lastAction = 0;
		DWORD			lastRead = 0;
	};

	std::unordered_map<DWORD, SWardrobeData> s_mapData;	// pid -> data, loaded on first use

	enum
	{
		WARDROBE_ACTION_DELAY	= 250,	// ms between two changes (Baerim client throttle)
		WARDROBE_READ_DELAY		= 500,	// ms between two catalog / outfit reads
		WARDROBE_CATALOG_CHUNK	= 380,
	};

	// ---------------------------------------------------------------------------------------------------- items
	int LookSlotOf(const TItemTable* p)
	{
		if (!p)
			return -1;
		if (p->bType == ITEM_COSTUME)
		{
			switch (p->bSubType)
			{
				case COSTUME_BODY:			return WARDROBE_LOOK_BODY;
				case COSTUME_HAIR:			return WARDROBE_LOOK_HAIR;
#ifdef ENABLE_MOUNT_PET_SKIN
				case COSTUME_MOUNT:			return WARDROBE_LOOK_MOUNT;
				case COSTUME_PET:			return WARDROBE_LOOK_PET;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
				case COSTUME_ACCE:			return WARDROBE_LOOK_ACCE;
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				case COSTUME_WEAPON:		return WARDROBE_LOOK_WEAPON;
#endif
#ifdef ENABLE_ACCE_SKIN_SYSTEM
				case COSTUME_ACCE_SKIN:		return WARDROBE_LOOK_ACCE_SKIN;
#endif
				default:					return -1;
			}
		}
#ifdef ENABLE_PET_SYSTEM
		// the proto reader has no sub type list for ITEM_PET (PET_PAY is read as 0): every pet item with a mob is a pet look
		if (p->bType == ITEM_PET && (p->alValues[0] || p->alValues[1]))
			return WARDROBE_LOOK_PET;
#endif
#ifdef ENABLE_SHINING_SYSTEM
		if (p->bType == ITEM_SHINING)
		{
			if (p->bSubType == SHINING_WEAPON)
				return WARDROBE_LOOK_SHINING_WEAPON;
			if (p->bSubType == SHINING_ARMOR)
				return WARDROBE_LOOK_SHINING_ARMOR;
		}
#endif
		return -1;
	}

	const TItemTable* LookTable(DWORD dwVnum)
	{
		return dwVnum ? ITEM_MANAGER::instance().GetTable(dwVnum) : NULL;
	}

	// sex / class anti flags of the item against the character
	bool IsCompatible(const CHARACTER* ch, const TItemTable* p)
	{
		if (!p)
			return false;
		const DWORD dwAnti = p->dwAntiFlags;
		const ESex eSex = GET_SEX(const_cast<LPCHARACTER>(ch));
		if (eSex == SEX_MALE && (dwAnti & ITEM_ANTIFLAG_MALE))
			return false;
		if (eSex == SEX_FEMALE && (dwAnti & ITEM_ANTIFLAG_FEMALE))
			return false;
		switch (ch->GetJob())
		{
			case JOB_WARRIOR:	return !(dwAnti & ITEM_ANTIFLAG_WARRIOR);
			case JOB_ASSASSIN:	return !(dwAnti & ITEM_ANTIFLAG_ASSASSIN);
			case JOB_SURA:		return !(dwAnti & ITEM_ANTIFLAG_SURA);
			case JOB_SHAMAN:	return !(dwAnti & ITEM_ANTIFLAG_SHAMAN);
			default:			return true;
		}
	}

	DWORD MountMobOf(const TItemTable* p)
	{
		if (!p)
			return 0;
		for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		{
			if (p->aApplies[i].bType == APPLY_MOUNT && p->aApplies[i].lValue > 0)
				return p->aApplies[i].lValue;
		}
		return p->alValues[1];
	}

	DWORD PetMobOf(const TItemTable* p)
	{
		if (!p)
			return 0;
		return p->alValues[1] ? p->alValues[1] : p->alValues[0];
	}

	// every look vnum the character owns: inventory, equipment, special inventories and the wardrobe storage
	void CollectOwned(const CHARACTER* ch, std::set<DWORD>& owned)
	{
		for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		{
			LPITEM item = ch->GetInventoryItem(i);
			if (item)
				owned.insert(item->GetVnum());
		}
		for (WORD i = 0; i < WARDROBE_MAX_NUM; ++i)
		{
			LPITEM item = ch->GetWardrobeItem(i);
			if (item)
				owned.insert(item->GetVnum());
		}
	}

	// ---------------------------------------------------------------------------------------------------- data
	bool IsValidNameHex(const std::string& s)
	{
		if (s.empty() || s.size() % 2 || s.size() > WARDROBE_OUTFIT_NAME_MAX * 2)
			return false;
		bool bText = false;
		for (size_t i = 0; i < s.size(); i += 2)
		{
			const int hi = isxdigit((unsigned char) s[i]) ? (isdigit((unsigned char) s[i]) ? s[i] - '0' : (tolower(s[i]) - 'a' + 10)) : -1;
			const int lo = isxdigit((unsigned char) s[i + 1]) ? (isdigit((unsigned char) s[i + 1]) ? s[i + 1] - '0' : (tolower(s[i + 1]) - 'a' + 10)) : -1;
			if (hi < 0 || lo < 0)
				return false;
			const int c = hi * 16 + lo;
			if (c < 0x20 || c == 0x7f || c == '|')
				return false;
			if (c != ' ')
				bText = true;
		}
		return bText;
	}

	SWardrobeData& Data(const CHARACTER* ch)
	{
		const DWORD dwPID = ch->GetPlayerID();
		auto it = s_mapData.find(dwPID);
		if (it != s_mapData.end())
			return it->second;

		SWardrobeData& d = s_mapData[dwPID];
		if (!dwPID)
			return d;

		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
				"SELECT look0, look1, look2, look3, look4, look5, look6, look7, look8, remembered, applied FROM wardrobe_state WHERE pid = %u", dwPID));
			if (pMsg->Get() && pMsg->Get()->uiNumRows)
			{
				MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
				for (int i = 0; i < WARDROBE_LOOK_COUNT; ++i)
					str_to_number(d.looks[i], row[i]);
				str_to_number(d.remembered, row[WARDROBE_LOOK_COUNT]);
				str_to_number(d.applied, row[WARDROBE_LOOK_COUNT + 1]);
				if (d.remembered > WARDROBE_OUTFIT_MAX)
					d.remembered = 0;
				if (d.applied > WARDROBE_OUTFIT_MAX)
					d.applied = 0;
			}
		}
		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
				"SELECT slot, revision, used, name, look0, look1, look2, look3, look4, look5, look6, look7, look8 FROM wardrobe_outfit WHERE pid = %u", dwPID));
			if (pMsg->Get())
			{
				for (unsigned int n = 0; n < pMsg->Get()->uiNumRows; ++n)
				{
					MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
					int iSlot = -1;
					str_to_number(iSlot, row[0]);
					if (iSlot < 0 || iSlot >= WARDROBE_OUTFIT_MAX)
						continue;
					SWardrobeOutfit& o = d.outfits[iSlot];
					int iUsed = 0;
					str_to_number(o.revision, row[1]);
					str_to_number(iUsed, row[2]);
					o.used = iUsed != 0;
					o.nameHex = row[3] ? row[3] : "";
					for (int i = 0; i < WARDROBE_LOOK_COUNT; ++i)
						str_to_number(o.looks[i], row[4 + i]);
					if (o.used && !IsValidNameHex(o.nameHex))
						o.used = false;
				}
			}
		}
		return d;
	}

	void SaveState(const CHARACTER* ch, const SWardrobeData& d)
	{
		const DWORD* l = d.looks;
		DBManager::instance().Query(
			"REPLACE INTO wardrobe_state (pid, look0, look1, look2, look3, look4, look5, look6, look7, look8, remembered, applied) "
			"VALUES (%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
			ch->GetPlayerID(), l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7], l[8], d.remembered, d.applied);
	}

	void SaveOutfit(const CHARACTER* ch, int iSlot, const SWardrobeOutfit& o)
	{
		const DWORD* l = o.looks;
		// nameHex is validated hex only, safe in the query
		DBManager::instance().Query(
			"REPLACE INTO wardrobe_outfit (pid, slot, revision, used, name, look0, look1, look2, look3, look4, look5, look6, look7, look8) "
			"VALUES (%u, %d, %u, %d, '%s', %u, %u, %u, %u, %u, %u, %u, %u, %u)",
			ch->GetPlayerID(), iSlot, o.revision, o.used ? 1 : 0, o.nameHex.c_str(), l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7], l[8]);
	}

	// ---------------------------------------------------------------------------------------------------- packets
	void SendResult(LPCHARACTER ch, DWORD dwSerial, int iCode)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeResult %u %d", dwSerial, iCode);
	}

	void SendAppearance(LPCHARACTER ch, const SWardrobeData& d)
	{
		const DWORD* l = d.looks;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeAppearance %u %u %u %u %u %u %u %u %u", l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7], l[8]);
	}

	// collection: one entry per different look item in the storage
	void SendProgress(LPCHARACTER ch)
	{
		std::set<DWORD> setVnum;
		for (WORD i = 0; i < WARDROBE_MAX_NUM; ++i)
		{
			LPITEM item = ch->GetWardrobeItem(i);
			if (item && LookSlotOf(item->GetProto()) >= 0)
				setVnum.insert(item->GetVnum());
		}
		const DWORD dwCostumes = setVnum.size();
		const DWORD dwExp = dwCostumes * WARDROBE_EXP_PER_COSTUME;
		const DWORD dwLevel = dwExp / WARDROBE_EXP_PER_LEVEL + 1;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeProgress %u %u %u %u %u", dwCostumes, dwLevel, dwExp, dwExp % WARDROBE_EXP_PER_LEVEL, (DWORD) WARDROBE_EXP_PER_LEVEL);
	}

	// locale/<locale>/wardrobe_catalog.txt (Baerim catalog in Baerim order):
	//	C <id> <name>	category (the first one is shown without a header)
	//	<vnum> <id>		look
	struct SCatalog
	{
		std::vector<std::pair<int, std::string> >	categories;	// id, name as hex
		std::vector<std::string>					rows;		// "vnum,id,slot"
	};

	const SCatalog& Catalog()
	{
		static SCatalog s_catalog;
		static bool s_bLoaded = false;
		if (s_bLoaded)
			return s_catalog;
		s_bLoaded = true;

		char szPath[256];
		snprintf(szPath, sizeof(szPath), "%s/wardrobe_catalog.txt", LocaleService_GetBasePath().c_str());
		std::ifstream in(szPath);
		if (!in.is_open())
		{
			sys_err("wardrobe: cannot open %s", szPath);
			return s_catalog;
		}

		std::set<DWORD> setSeen;
		std::set<int> setCategory;
		std::string line;
		while (std::getline(in, line))
		{
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			if (line.empty() || line[0] == '#')
				continue;
			if (line[0] == 'C' && line.size() > 2 && line[1] == '\t')
			{
				const size_t tab = line.find('\t', 2);
				if (tab == std::string::npos)
					continue;
				const int id = atoi(line.substr(2, tab - 2).c_str());
				const std::string name = line.substr(tab + 1);
				std::string hex;
				char sz[3];
				for (size_t i = 0; i < name.size() && i < 64; ++i)
				{
					snprintf(sz, sizeof(sz), "%02x", (unsigned char) name[i]);
					hex += sz;
				}
				if (id > 0 && !hex.empty() && setCategory.insert(id).second)
					s_catalog.categories.push_back(std::make_pair(id, hex));
				continue;
			}
			std::istringstream ss(line);
			DWORD dwVnum = 0;
			int id = 0;
			if (!(ss >> dwVnum >> id) || setCategory.find(id) == setCategory.end() || !setSeen.insert(dwVnum).second)
				continue;
			const int iSlot = LookSlotOf(LookTable(dwVnum));
			if (iSlot < 0)
				continue;	// not imported (yet) / not a look item
			char sz[48];
			snprintf(sz, sizeof(sz), "%u,%d,%d", dwVnum, id, iSlot);
			s_catalog.rows.push_back(sz);
		}
		sys_log(0, "wardrobe: catalog %u looks, %u categories", (unsigned) s_catalog.rows.size(), (unsigned) s_catalog.categories.size());
		return s_catalog;
	}

	void SendCatalog(LPCHARACTER ch, DWORD dwSerial)
	{
		const SCatalog& c = Catalog();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeCatalogBegin %u %u", dwSerial, (unsigned) c.rows.size());
		for (const auto& cat : c.categories)
			ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeCatalogCat %u %d %s", dwSerial, cat.first, cat.second.c_str());
		std::string chunk;
		for (const std::string& s : c.rows)
		{
			if (!chunk.empty() && chunk.size() + s.size() + 1 > WARDROBE_CATALOG_CHUNK)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeCatalogRows %u %s", dwSerial, chunk.c_str());
				chunk.clear();
			}
			if (!chunk.empty())
				chunk += ';';
			chunk += s;
		}
		if (!chunk.empty())
			ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeCatalogRows %u %s", dwSerial, chunk.c_str());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeCatalogEnd %u", dwSerial);
	}

	void SendOutfits(LPCHARACTER ch, DWORD dwSerial)
	{
		const SWardrobeData& d = Data(ch);
		int iCount = 0;
		for (int i = 0; i < WARDROBE_OUTFIT_MAX; ++i)
			if (d.outfits[i].revision)
				++iCount;

		ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeOutfitsBegin %u %d %u %u", dwSerial, iCount, d.remembered, d.applied);
		for (int i = 0; i < WARDROBE_OUTFIT_MAX; ++i)
		{
			const SWardrobeOutfit& o = d.outfits[i];
			if (!o.revision)
				continue;	// never used
			const DWORD* l = o.looks;
			ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeOutfitRow %u %d %u %s %u %u %u %u %u %u %u %u %u", dwSerial, i, o.revision,
				o.used ? o.nameHex.c_str() : "-", l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7], l[8]);
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "WardrobeOutfitsEnd %u", dwSerial);
	}

	// ---------------------------------------------------------------------------------------------------- actions
	bool IsBlocked(LPCHARACTER ch)
	{
		return ch->IsDead() || ch->GetExchange() || ch->GetShop() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen();
	}

	// commands that move an item: also no refine / acce / change look / dragon soul window, no warp (CanHandleItem)
	bool IsItemBlocked(LPCHARACTER ch)
	{
		return IsBlocked(ch) || !ch->CanHandleItem();
	}

	bool ReadLooks(std::istringstream& ss, DWORD* looks)
	{
		for (int i = 0; i < WARDROBE_LOOK_COUNT; ++i)
		{
			long long v = -1;
			if (!(ss >> v) || v < 0 || v > 0x7fffffffLL)
				return false;
			looks[i] = (DWORD) v;
		}
		return true;
	}

	// every vnum is 0 or an item of that look slot
	bool IsValidLooks(const DWORD* looks)
	{
		for (int i = 0; i < WARDROBE_LOOK_COUNT; ++i)
			if (looks[i] && LookSlotOf(LookTable(looks[i])) != i)
				return false;
		return true;
	}

	void RefreshLooks(LPCHARACTER ch, SWardrobeData& d, const DWORD* looks)
	{
#ifdef ENABLE_PET_SYSTEM
		LPITEM pPet = (looks[WARDROBE_LOOK_PET] != d.looks[WARDROBE_LOOK_PET]) ? ch->GetWear(WEAR_PET) : NULL;
		if (pPet)
			ch->PetUnsummon(pPet);	// with the old look (the summoned model)
#endif
		memcpy(d.looks, looks, sizeof(d.looks));
#ifdef ENABLE_PET_SYSTEM
		if (pPet)
			ch->PetSummon(pPet);
#endif
		ch->UpdatePacket();
	}

	int Deposit(LPCHARACTER ch, WORD wCell, DWORD dwVnum)
	{
		if (wCell >= INVENTORY_MAX_NUM)
			return WARDROBE_RESULT_INVALID;
		LPITEM item = ch->GetInventoryItem(wCell);
		if (!item || item->GetVnum() != dwVnum || item->IsExchanging() || item->isLocked() || item->IsEquipped() || LookSlotOf(item->GetProto()) < 0)
			return WARDROBE_RESULT_INVALID;

		int iCell = -1;
		for (WORD i = 0; i < WARDROBE_MAX_NUM; ++i)
		{
			if (!ch->GetWardrobeItem(i))
			{
				iCell = i;
				break;
			}
		}
		if (iCell < 0)
			return WARDROBE_RESULT_FULL;

		item->RemoveFromCharacter();
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(ch, TItemPos(WARDROBE, iCell), false);
#else
		item->AddToCharacter(ch, TItemPos(WARDROBE, iCell));
#endif
		LogManager::instance().ItemLog(ch, item, "WARDROBE_IN", "");
		return WARDROBE_RESULT_OK;
	}

	int Withdraw(LPCHARACTER ch, WORD wCell, DWORD dwVnum)
	{
#ifdef ENABLE_WARDROBE_AUTO_STORE
		return WARDROBE_RESULT_INVALID;	// Baerim: the costumes stay in the wardrobe inventory
#endif
		LPITEM item = ch->GetWardrobeItem(wCell);
		if (!item || item->GetVnum() != dwVnum)
			return WARDROBE_RESULT_INVALID;

		const int iPos = ch->GetEmptyInventory(item->GetSize());
		if (iPos < 0)
			return WARDROBE_RESULT_FULL;

		item->RemoveFromCharacter();
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(ch, TItemPos(INVENTORY, iPos), false);
#else
		item->AddToCharacter(ch, TItemPos(INVENTORY, iPos));
#endif
		LogManager::instance().ItemLog(ch, item, "WARDROBE_OUT", "");
		return WARDROBE_RESULT_OK;
	}

#ifdef ENABLE_WARDROBE_AUTO_STORE
	int FreeStoreCell(LPCHARACTER ch)
	{
		for (WORD i = 0; i < WARDROBE_MAX_NUM; ++i)
			if (!ch->GetWardrobeItem(i))
				return i;
		return -1;
	}

	void AddToStorage(LPCHARACTER ch, LPITEM item, int iCell)
	{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(ch, TItemPos(WARDROBE, iCell), false);
#else
		item->AddToCharacter(ch, TItemPos(WARDROBE, iCell));
#endif
	}

	// the stored costume goes on through the normal equip (all its checks); the costume it replaces comes back into the storage
	bool s_bNoRedirect = false;	// the equip below needs the costume in the inventory for a moment

	bool s_bQuietUnequip = false;	// the costume replaced by an equip goes back without opening the inventory tab again

	// straight from the wardrobe (no inventory cell, the client never sees it in the inventory): the worn costume of that slot
	// goes back into the wardrobe, then the normal equip with all its checks
	int EquipStored(LPCHARACTER ch, WORD wCell, DWORD dwVnum)
	{
		LPITEM item = ch->GetWardrobeItem(wCell);
		if (!item || item->GetVnum() != dwVnum || !item->IsEquipable() || item->IsExchanging() || item->isLocked())
			return WARDROBE_RESULT_INVALID;

		const int iWear = item->FindEquipCell(ch);
		if (iWear < 0 || !ch->CanEquipNow(item))
			return WARDROBE_RESULT_ERROR;

		LPITEM pOld = ch->GetWear(iWear);
		if (pOld)
		{
			if (IS_SET(pOld->GetFlag(), ITEM_FLAG_IRREMOVABLE) || !ch->CanUnequipNow(pOld))
				return WARDROBE_RESULT_ERROR;
			s_bQuietUnequip = true;
			const bool bUnequipped = ch->UnequipItem(pOld);
			s_bQuietUnequip = false;
			if (!bUnequipped || pOld->IsEquipped())
				return WARDROBE_RESULT_ERROR;
		}

		if (!ch->EquipItem(item) || !item->IsEquipped())
		{
			if (pOld && pOld->GetOwner() == ch && !pOld->IsEquipped())
				ch->EquipItem(pOld, iWear);	// put the old costume back on
			return WARDROBE_RESULT_ERROR;	// EquipItem told the reason
		}
		LogManager::instance().ItemLog(ch, item, "WARDROBE_EQUIP", "");
		return WARDROBE_RESULT_OK;
	}
#endif

#if defined(ENABLE_WARDROBE_AUTO_STORE) && defined(ENABLE_RENEWAL_SWITCHBOT)
	// Shift + left click on a stored costume (Baerim inventory shortcut): into the first free switchbot slot
	int MoveToSwitchbot(LPCHARACTER ch, WORD wCell, DWORD dwVnum)
	{
		LPITEM item = ch->GetWardrobeItem(wCell);
		if (!item || item->GetVnum() != dwVnum || item->IsExchanging() || item->isLocked() || !SwitchbotHelper::IsValidItem(item))
			return WARDROBE_RESULT_INVALID;
		for (int i = 0; i < SWITCHBOT_SLOT_COUNT; ++i)
		{
			if (ch->GetItem(TItemPos(SWITCHBOT, i)))
				continue;
			item->RemoveFromCharacter();
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(ch, TItemPos(SWITCHBOT, i), false);
#else
			item->AddToCharacter(ch, TItemPos(SWITCHBOT, i));
#endif
			LogManager::instance().ItemLog(ch, item, "WARDROBE_OUT", "switchbot");
			return WARDROBE_RESULT_OK;
		}
		return WARDROBE_RESULT_FULL;
	}
#endif

	int Apply(LPCHARACTER ch, int iOutfit, const DWORD* looks)
	{
		SWardrobeData& d = Data(ch);
		if (iOutfit < 0 || iOutfit > WARDROBE_OUTFIT_MAX)
			return WARDROBE_RESULT_INVALID;
		if (iOutfit && !d.outfits[iOutfit - 1].used)
			return WARDROBE_RESULT_INVALID;
		if (!IsValidLooks(looks))
			return WARDROBE_RESULT_INVALID;

		std::set<DWORD> owned;
		CollectOwned(ch, owned);
		for (int i = 0; i < WARDROBE_LOOK_COUNT; ++i)
		{
			if (!looks[i] || looks[i] == d.looks[i])
				continue;	// an active look stays even if it was moved meanwhile
			if (owned.find(looks[i]) == owned.end())
				return WARDROBE_RESULT_UNOWNED;
			if (!IsCompatible(ch, LookTable(looks[i])))
				return WARDROBE_RESULT_INVALID;
		}

		RefreshLooks(ch, d, looks);
		d.applied = (BYTE) iOutfit;
		SaveState(ch, d);
		SendAppearance(ch, d);
		return WARDROBE_RESULT_OK;
	}

	int Save(LPCHARACTER ch, int iSlot, DWORD dwRevision, const std::string& nameHex, const DWORD* looks)
	{
		SWardrobeData& d = Data(ch);
		if (iSlot < 0 || iSlot >= WARDROBE_OUTFIT_MAX || !IsValidNameHex(nameHex) || !IsValidLooks(looks))
			return WARDROBE_RESULT_INVALID;
		SWardrobeOutfit& o = d.outfits[iSlot];
		if (o.revision != dwRevision)
			return WARDROBE_RESULT_INVALID;	// changed meanwhile: the client refreshes

		o.revision = dwRevision + 1;
		o.used = true;
		o.nameHex = nameHex;
		memcpy(o.looks, looks, sizeof(o.looks));
		SaveOutfit(ch, iSlot, o);
		d.remembered = (BYTE) (iSlot + 1);
		SaveState(ch, d);
		return WARDROBE_RESULT_OK;
	}

	int Rename(LPCHARACTER ch, int iSlot, DWORD dwRevision, const std::string& nameHex)
	{
		SWardrobeData& d = Data(ch);
		if (iSlot < 0 || iSlot >= WARDROBE_OUTFIT_MAX || !IsValidNameHex(nameHex))
			return WARDROBE_RESULT_INVALID;
		SWardrobeOutfit& o = d.outfits[iSlot];
		if (!o.used || o.revision != dwRevision)
			return WARDROBE_RESULT_INVALID;
		o.revision = dwRevision + 1;
		o.nameHex = nameHex;
		SaveOutfit(ch, iSlot, o);
		return WARDROBE_RESULT_OK;
	}

	int Delete(LPCHARACTER ch, int iSlot, DWORD dwRevision)
	{
		SWardrobeData& d = Data(ch);
		if (iSlot < 0 || iSlot >= WARDROBE_OUTFIT_MAX)
			return WARDROBE_RESULT_INVALID;
		SWardrobeOutfit& o = d.outfits[iSlot];
		if (!o.used || o.revision != dwRevision)
			return WARDROBE_RESULT_INVALID;
		o.revision = dwRevision + 1;	// the slot keeps its revision (Baerim: deleted rows stay with "-")
		o.used = false;
		o.nameHex.clear();
		memset(o.looks, 0, sizeof(o.looks));
		SaveOutfit(ch, iSlot, o);
		if (d.remembered == iSlot + 1)
			d.remembered = 0;
		if (d.applied == iSlot + 1)
			d.applied = 0;
		SaveState(ch, d);
		return WARDROBE_RESULT_OK;
	}
}

// -------------------------------------------------------------------------------------------------------- public
void Wardrobe_Command(LPCHARACTER ch, const char* argument)
{
	if (!ch || !ch->GetDesc() || !ch->IsPC() || !ch->GetPlayerID())
		return;

	std::istringstream ss(argument ? argument : "");
	std::string cmd;
	DWORD dwSerial = 0;
	if (!(ss >> cmd >> dwSerial))
		return;

	SWardrobeData& d = Data(ch);
	const DWORD dwNow = get_dword_time();

	if (cmd == "catalog" || cmd == "outfits")
	{
		if (dwNow - d.lastRead < WARDROBE_READ_DELAY)
		{
			SendResult(ch, dwSerial, WARDROBE_RESULT_BUSY);
			return;
		}
		d.lastRead = dwNow;
		if (cmd == "catalog")
			SendCatalog(ch, dwSerial);
		else
			SendOutfits(ch, dwSerial);
		return;
	}

	if (dwNow - d.lastAction < WARDROBE_ACTION_DELAY)
	{
		SendResult(ch, dwSerial, WARDROBE_RESULT_BUSY);
		return;
	}
	d.lastAction = dwNow;

	int iResult = WARDROBE_RESULT_INVALID;
	if (cmd == "deposit" || cmd == "withdraw")
	{
		long long llCell = -1, llVnum = -1;
		if (!(ss >> llCell >> llVnum) || llCell < 0 || llCell >= WARDROBE_MAX_NUM || llVnum <= 0 || llVnum > 0xffffffffLL)
			iResult = WARDROBE_RESULT_INVALID;
		else if (IsItemBlocked(ch))
			iResult = WARDROBE_RESULT_BLOCKED;
		else if (cmd == "deposit")
			iResult = Deposit(ch, (WORD) llCell, (DWORD) llVnum);
		else
			iResult = Withdraw(ch, (WORD) llCell, (DWORD) llVnum);
		SendResult(ch, dwSerial, iResult);
		if (iResult == WARDROBE_RESULT_OK)
			SendProgress(ch);
		return;
	}

#if defined(ENABLE_WARDROBE_AUTO_STORE) && defined(ENABLE_RENEWAL_SWITCHBOT)
	if (cmd == "switchbot")
	{
		long long llCell = -1, llVnum = -1;
		if (!(ss >> llCell >> llVnum) || llCell < 0 || llCell >= WARDROBE_MAX_NUM || llVnum <= 0 || llVnum > 0xffffffffLL)
			iResult = WARDROBE_RESULT_INVALID;
		else if (IsItemBlocked(ch))
			iResult = WARDROBE_RESULT_BLOCKED;
		else
			iResult = MoveToSwitchbot(ch, (WORD) llCell, (DWORD) llVnum);
		SendResult(ch, dwSerial, iResult);
		if (iResult == WARDROBE_RESULT_OK)
			SendProgress(ch);
		return;
	}
#endif

#ifdef ENABLE_WARDROBE_AUTO_STORE
	if (cmd == "equip")
	{
		long long llCell = -1, llVnum = -1;
		if (!(ss >> llCell >> llVnum) || llCell < 0 || llCell >= WARDROBE_MAX_NUM || llVnum <= 0 || llVnum > 0xffffffffLL)
			iResult = WARDROBE_RESULT_INVALID;
		else if (IsItemBlocked(ch))
			iResult = WARDROBE_RESULT_BLOCKED;
		else
			iResult = EquipStored(ch, (WORD) llCell, (DWORD) llVnum);
		SendResult(ch, dwSerial, iResult);
		if (iResult == WARDROBE_RESULT_OK)
			SendProgress(ch);
		return;
	}
#endif

	if (cmd == "apply")
	{
		int iOutfit = -1;
		DWORD looks[WARDROBE_LOOK_COUNT];
		if (!(ss >> iOutfit) || !ReadLooks(ss, looks))
			iResult = WARDROBE_RESULT_INVALID;
		else if (IsBlocked(ch))
			iResult = WARDROBE_RESULT_BLOCKED;
		else
			iResult = Apply(ch, iOutfit, looks);
	}
	else if (cmd == "save")
	{
		int iSlot = -1;
		DWORD dwRevision = 0;
		std::string nameHex;
		DWORD looks[WARDROBE_LOOK_COUNT];
		if (ss >> iSlot >> dwRevision >> nameHex && ReadLooks(ss, looks))
			iResult = Save(ch, iSlot, dwRevision, nameHex, looks);
	}
	else if (cmd == "rename")
	{
		int iSlot = -1;
		DWORD dwRevision = 0;
		std::string nameHex;
		if (ss >> iSlot >> dwRevision >> nameHex)
			iResult = Rename(ch, iSlot, dwRevision, nameHex);
	}
	else if (cmd == "delete")
	{
		int iSlot = -1;
		DWORD dwRevision = 0;
		if (ss >> iSlot >> dwRevision)
			iResult = Delete(ch, iSlot, dwRevision);
	}
	else
		return;

	SendResult(ch, dwSerial, iResult);
}

void Wardrobe_OnLogin(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc() || !ch->IsPC() || !ch->GetPlayerID())
		return;

	SWardrobeData& d = Data(ch);

#ifdef ENABLE_WARDROBE_AUTO_STORE
	for (WORD i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (item && !item->IsEquipped() && !item->IsExchanging())
			Wardrobe_StoreFromInventory(ch, item);
	}
#endif

	// looks of items the character does not have any more are dropped
	std::set<DWORD> owned;
	CollectOwned(ch, owned);
	DWORD looks[WARDROBE_LOOK_COUNT];
	memcpy(looks, d.looks, sizeof(looks));
	bool bChanged = false;
	for (int i = 0; i < WARDROBE_LOOK_COUNT; ++i)
	{
		if (looks[i] && (owned.find(looks[i]) == owned.end() || LookSlotOf(LookTable(looks[i])) != i || !IsCompatible(ch, LookTable(looks[i]))))
		{
			looks[i] = 0;
			bChanged = true;
		}
	}
	if (bChanged)
	{
		RefreshLooks(ch, d, looks);
		d.applied = 0;
		SaveState(ch, d);
	}

	SendAppearance(ch, d);
	SendProgress(ch);
}

void Wardrobe_OnDestroy(LPCHARACTER ch)
{
	if (ch)
		s_mapData.erase(ch->GetPlayerID());
}

DWORD Wardrobe_VisualMountVnum(const CHARACTER* ch, DWORD dwMountVnum)
{
	if (!dwMountVnum || !ch->IsPC() || !ch->GetPlayerID())
		return dwMountVnum;
	const DWORD dwLook = Data(ch).looks[WARDROBE_LOOK_MOUNT];
	const DWORD dwMob = dwLook ? MountMobOf(LookTable(dwLook)) : 0;
	return dwMob ? dwMob : dwMountVnum;
}

DWORD Wardrobe_VisualShining(const CHARACTER* ch, int iIndex, DWORD dwVnum)
{
	if (!ch->IsPC() || !ch->GetPlayerID() || iIndex < 0 || iIndex > 1)
		return dwVnum;
	const DWORD dwLook = Data(ch).looks[WARDROBE_LOOK_SHINING_WEAPON + iIndex];
	return dwLook ? dwLook : dwVnum;
}

DWORD Wardrobe_VisualPetMob(const CHARACTER* ch, DWORD dwMobVnum)
{
	if (!ch->IsPC() || !ch->GetPlayerID())
		return dwMobVnum;
	const DWORD dwLook = Data(ch).looks[WARDROBE_LOOK_PET];
	const DWORD dwMob = dwLook ? PetMobOf(LookTable(dwLook)) : 0;
	return dwMob ? dwMob : dwMobVnum;
}

WORD Wardrobe_VisualPart(const CHARACTER* ch, BYTE bPartPos, WORD wPart)
{
	if (!ch->IsPC() || !ch->GetPlayerID() || ch->IsPolymorphed())
		return wPart;

	const SWardrobeData& d = Data(ch);
	switch (bPartPos)
	{
		case PART_MAIN:
			if (d.looks[WARDROBE_LOOK_BODY] && d.looks[WARDROBE_LOOK_BODY] <= 0xffff)
				return (WORD) d.looks[WARDROBE_LOOK_BODY];
			break;

		case PART_HAIR:
			if (const TItemTable* p = LookTable(d.looks[WARDROBE_LOOK_HAIR]))
				return (WORD) p->alValues[3];
			break;

		case PART_WEAPON:
			// only over a weapon of the same kind (like the weapon costume)
			if (const TItemTable* p = LookTable(d.looks[WARDROBE_LOOK_WEAPON]))
			{
				LPITEM pWeapon = ch->GetWear(WEAR_WEAPON);
				if (pWeapon && pWeapon->GetType() == ITEM_WEAPON && p->alValues[3] == pWeapon->GetSubType() && d.looks[WARDROBE_LOOK_WEAPON] <= 0xffff)
					return (WORD) d.looks[WARDROBE_LOOK_WEAPON];
			}
			break;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case PART_ACCE:
		{
			DWORD dwLook = d.looks[WARDROBE_LOOK_ACCE_SKIN] ? d.looks[WARDROBE_LOOK_ACCE_SKIN] : d.looks[WARDROBE_LOOK_ACCE];
			if (dwLook > ACCE_BASE_VNUM)
			{
				DWORD dwPart = dwLook - ACCE_BASE_VNUM;
				// the glow of the worn sash stays (like the sash skin)
				LPITEM pAcce = ch->GetWear(WEAR_COSTUME_ACCE);
				if (pAcce && pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
					dwPart += ACCE_EFFECT_VNUM;
				return (WORD) dwPart;
			}
			break;
		}
#endif
	}
	return wPart;
}

WORD CHARACTER::GetVisualPart(BYTE bPartPos) const
{
	return Wardrobe_VisualPart(this, bPartPos, GetPart(bPartPos));
}
#endif

#ifdef ENABLE_WARDROBE_AUTO_STORE
int Wardrobe_AutoStoreCell(LPCHARACTER ch, LPITEM item)
{
	if (!ch || !item || !ch->IsPC() || !ch->GetDesc() || !ch->GetPlayerID() || LookSlotOf(item->GetProto()) < 0)
		return -1;
	return FreeStoreCell(ch);
}

bool Wardrobe_StoreNewItem(LPCHARACTER ch, LPITEM item)
{
	if (!item || item->GetOwner())
		return false;
	const int iCell = Wardrobe_AutoStoreCell(ch, item);
	if (iCell < 0)
		return false;
	AddToStorage(ch, item, iCell);
	LogManager::instance().ItemLog(ch, item, "WARDROBE_IN", "auto");
	SendProgress(ch);
	return true;
}

bool Wardrobe_StoreFromInventory(LPCHARACTER ch, LPITEM item)
{
	if (!item || item->GetOwner() != ch || item->IsEquipped() || item->GetWindow() != INVENTORY || item->IsExchanging() || item->isLocked())
		return false;
	const int iCell = Wardrobe_AutoStoreCell(ch, item);
	if (iCell < 0)
		return false;
	item->RemoveFromCharacter();
	AddToStorage(ch, item, iCell);
	LogManager::instance().ItemLog(ch, item, "WARDROBE_IN", "auto");
	SendProgress(ch);
	return true;
}

void Wardrobe_OnUnequipStored(LPCHARACTER ch, LPITEM item)
{
	LogManager::instance().ItemLog(ch, item, "WARDROBE_IN", "unequip");
	SendProgress(ch);
	if (!s_bQuietUnequip)
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenWardrobeInventory %d", LookSlotOf(item->GetProto()));
}
#endif

#ifdef ENABLE_WARDROBE_AUTO_STORE
int Wardrobe_RedirectCell(LPCHARACTER ch, LPITEM item)
{
	if (s_bNoRedirect || !ch || !ch->IsItemLoaded())	// not while the items load (the storage is not complete yet)
		return -1;
	return Wardrobe_AutoStoreCell(ch, item);
}
#endif
