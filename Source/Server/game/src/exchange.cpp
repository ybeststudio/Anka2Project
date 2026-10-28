#include "stdafx.h"
#include "../../common/service.h"
#include "../../library/libgame/include/grid.h"
#include "utils.h"
#include "desc.h"
#include "desc_client.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "locale_service.h"
#include "../../common/length.h"
#include "exchange.h"
#include "DragonSoul.h"
#include "questmanager.h" // @fixme150
#include "questmanager.h"

#ifdef ENABLE_MESSENGER_BLOCK
	#include "messenger_manager.h"
#endif

#ifdef ENABLE_GOLD_LIMIT
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, long long arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL);

void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, long long arg1, TItemPos arg2, DWORD arg3, void * pvData)
#else
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL);

void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData)
#endif
{
	if (!ch->GetDesc())
		return;

	struct packet_exchange pack_exchg;

	pack_exchg.header = HEADER_GC_EXCHANGE;
	pack_exchg.sub_header = sub_header;
	pack_exchg.is_me = is_me;
	pack_exchg.arg1 = arg1;
	pack_exchg.arg2 = arg2;
	pack_exchg.arg3 = arg3;

	if (sub_header == EXCHANGE_SUBHEADER_GC_ITEM_ADD && pvData)
	{
#ifdef ENABLE_SLOT_MARKING_SYSTEM
		pack_exchg.arg4 = TItemPos(((LPITEM) pvData)->GetWindow(), ((LPITEM) pvData)->GetCell());
#endif
		thecore_memcpy(&pack_exchg.alSockets, ((LPITEM) pvData)->GetSockets(), sizeof(pack_exchg.alSockets));
		thecore_memcpy(&pack_exchg.aAttr, ((LPITEM) pvData)->GetAttributes(), sizeof(pack_exchg.aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		pack_exchg.dwTransmutationVnum = static_cast<LPITEM>(pvData)->GetTransmutationVnum();
#endif
#ifdef ENABLE_SET_ITEM
		pack_exchg.dwSetValue = static_cast<LPITEM>(pvData)->GetSetValue();
#endif
	}
	else
	{
#ifdef ENABLE_SLOT_MARKING_SYSTEM
		pack_exchg.arg4 = TItemPos(RESERVED_WINDOW, 0);
#endif
		memset(&pack_exchg.alSockets, 0, sizeof(pack_exchg.alSockets));
		memset(&pack_exchg.aAttr, 0, sizeof(pack_exchg.aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		pack_exchg.dwTransmutationVnum = 0;
#endif
#ifdef ENABLE_SET_ITEM
		pack_exchg.dwSetValue = 0;
#endif
	}

	ch->GetDesc()->Packet(&pack_exchg, sizeof(pack_exchg));
}

bool CHARACTER::ExchangeStart(LPCHARACTER victim)
{

#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::Instance().IsBlocked(GetName(), victim->GetName()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unblock %s to continue."), victim->GetName());
		return false;
	}
	else if (MessengerManager::Instance().IsBlocked(victim->GetName(), GetName()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s has blocked you."), victim->GetName());
		return false;
	}
#endif

	if (this == victim)
		return false;

	if (IsObserverMode())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade while observing."));
		return false;
	}

	if (victim->IsNPC() || !victim->IsPC())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true) // Fix
		return false;

	if (quest::CQuestManager::instance().GetPCForce(victim->GetPlayerID())->IsRunning() == true) // Fix
		return false;

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (GetPetWindowType() == PET_WINDOW_ATTR_CHANGE || GetPetWindowType() == PET_WINDOW_PRIMIUM_FEEDSTUFF)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("While modifying your pet's stats, you cannot trade, sell items, use the storeroom etc."));
		return false;
	}

	if (victim->GetPetWindowType() == PET_WINDOW_ATTR_CHANGE || victim->GetPetWindowType() == PET_WINDOW_PRIMIUM_FEEDSTUFF)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade while your trading partner is modifying their pet's stats."));
		return false;
	}

	if (GetActiveGrowthPet())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 737, "");
		return false;
	}

	if (victim->GetActiveGrowthPet())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 740, "");
		return false;
	}
#endif

	if (IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		|| IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		|| GetTransmutation()
#endif
		)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If the window is open, you cannot trade with others."));
		return false;
	}

	if (victim->IsOpenSafebox() || victim->GetShopOwner() || victim->GetMyShop() || victim->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		|| victim->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		|| victim->GetTransmutation()
#endif
		)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person is currently busy so you cannot trade right now."));
		return false;
	}

	int iDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());

	if (iDist >= EXCHANGE_MAX_DISTANCE)
		return false;

	if (GetExchange())
		return false;

	if (victim->GetExchange())
	{
		exchange_packet(this, EXCHANGE_SUBHEADER_GC_ALREADY, 0, 0, NPOS, 0);
		return false;
	}

	if (victim->IsBlockMode(BLOCK_EXCHANGE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person has cancelled the trade."));
		return false;
	}

#ifdef ENABLE_BOT_PLAYER
	if (victim->IsBotCharacter())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 교환 거부 상태입니다."));
		return false;
	}
#endif

	SetExchange(M2_NEW CExchange(this));
	victim->SetExchange(M2_NEW CExchange(victim));

	victim->GetExchange()->SetCompany(GetExchange());
	GetExchange()->SetCompany(victim->GetExchange());

	//
	SetExchangeTime();
	victim->SetExchangeTime();

	exchange_packet(victim, EXCHANGE_SUBHEADER_GC_START, 0, GetVID(), NPOS, 0);
	exchange_packet(this, EXCHANGE_SUBHEADER_GC_START, 0, victim->GetVID(), NPOS, 0);

	return true;
}

CExchange::CExchange(LPCHARACTER pOwner)
{
	m_pCompany = NULL;

	m_bAccept = false;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		m_apItems[i] = NULL;
		m_aItemPos[i] = NPOS;
		m_abItemDisplayPos[i] = 0;
	}

	m_lGold = 0;

	m_pOwner = pOwner;
	pOwner->SetExchange(this);

	m_pGrid = M2_NEW CGrid(6, 4);
}

CExchange::~CExchange()
{
	M2_DELETE(m_pGrid);
}

bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos)
{
	assert(m_pOwner != NULL && GetCompany());

	if (!item_pos.IsValidItemPosition())
		return false;

	if (item_pos.IsEquipPosition())
		return false;

	LPITEM item;

	if (!(item = m_pOwner->GetItem(item_pos)))
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
	{
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade this item."));
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

#ifdef ENABLE_SEALBIND_SYSTEM
	if (item->IsSealed())
	{
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ruh bagi ile bagli esyayi ticarete koyamazsin."));
		return false;
	}
#endif

	if (item->IsExchanging())
	{
		sys_log(0, "EXCHANGE under exchanging");
		return false;
	}

	// Fix
	if (quest::CQuestManager::instance().GetPCForce(m_pOwner->GetPlayerID())->IsRunning() == true)
		return false;

	LPCHARACTER	victim = GetCompany()->GetOwner();
	if (quest::CQuestManager::instance().GetPCForce(victim->GetPlayerID())->IsRunning() == true)
		return false;

	if (item->IsEquipped())
		return false;

	if(display_pos == 69)
	{
		for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
		{
			if(m_pGrid->IsEmpty(i, 1, item->GetSize()))
			{
				display_pos = i;
				break;
			}
		}
	}

	if(display_pos == 69)
	{
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough space."));
		return false;
	}

	if (!m_pGrid->IsEmpty(display_pos, 1, item->GetSize()))
	{
		sys_log(0, "EXCHANGE not empty item_pos %d %d %d", display_pos, 1, item->GetSize());
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			continue;

		m_apItems[i] = item;
		m_aItemPos[i] = item_pos;
		m_abItemDisplayPos[i] = display_pos;
		m_pGrid->Put(display_pos, 1, item->GetSize());

		item->SetExchanging(true);

#ifdef ENABLE_GROWTH_PET_SYSTEM
		if (item->GetType() == ITEM_GROWTH_PET && item->GetSubType() == PET_BAG)
		{
			LPGROWTH_PET pkPet = item->GetOwner()->GetGrowthPet(item->GetSocket(2));

			if (pkPet)
				pkPet->PetSetExchangePacket(GetCompany()->GetOwner());
		}
#endif

		exchange_packet(m_pOwner, 
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				true,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		exchange_packet(GetCompany()->GetOwner(),
				EXCHANGE_SUBHEADER_GC_ITEM_ADD, 
				false, 
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		sys_log(0, "EXCHANGE AddItem success %s pos(%d, %d) %d", item->GetName(), item_pos.window_type, item_pos.cell, display_pos);

		return true;
	}

	return false;
}

bool CExchange::RemoveItem(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return false;

	if (!m_apItems[pos])
		return false;

	TItemPos PosOfInventory = m_aItemPos[pos];
	m_apItems[pos]->SetExchanging(false);

	m_pGrid->Get(m_abItemDisplayPos[pos], 1, m_apItems[pos]->GetSize());

	exchange_packet(GetOwner(),	EXCHANGE_SUBHEADER_GC_ITEM_DEL, true, pos, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ITEM_DEL, false, pos, PosOfInventory, 0);

	Accept(false);
	GetCompany()->Accept(false);

	m_apItems[pos] = NULL;
	m_aItemPos[pos] = NPOS;
	m_abItemDisplayPos[pos] = 0;
	return true;
}

#ifdef ENABLE_GOLD_LIMIT
bool CExchange::AddGold(long long gold)
#else
bool CExchange::AddGold(long gold)
#endif
{
	if (gold <= 0)
		return false;

	if (GetOwner()->GetGold() < gold)
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
		return false;
	}

	if ( m_lGold > 0 )
	{
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	m_lGold = gold;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, true, m_lGold, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, false, m_lGold, NPOS, 0);
	return true;
}

bool CExchange::Check(int * piItemCount)
{
	if (GetOwner()->GetGold() < m_lGold)
		return false;

	int item_count = 0;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
			continue;

		if (!m_aItemPos[i].IsValidItemPosition())
			return false;

		if (m_apItems[i] != GetOwner()->GetItem(m_aItemPos[i]))
			return false;

		++item_count;
	}

	*piItemCount = item_count;
	return true;
}

bool CExchange::CheckSpace()
{
	static CGrid s_grid1(5, INVENTORY_MAX_NUM/5/4);
	static CGrid s_grid2(5, INVENTORY_MAX_NUM/5/4);
	static CGrid s_grid3(5, INVENTORY_MAX_NUM/5/4);
	static CGrid s_grid4(5, INVENTORY_MAX_NUM/5/4);

	s_grid1.Clear();
	s_grid2.Clear();
	s_grid3.Clear();
	s_grid4.Clear();

#ifdef ENABLE_SPECIAL_INVENTORY
	static CGrid s_grid5(5, SKILL_BOOK_INVENTORY_MAX_NUM / 5 / SPECIAL_INVENTORY_PAGE_COUNT);
	static CGrid s_grid6(5, UPGRADE_ITEMS_INVENTORY_MAX_NUM / 5 / SPECIAL_INVENTORY_PAGE_COUNT);
	static CGrid s_grid7(5, STONE_INVENTORY_MAX_NUM / 5 / SPECIAL_INVENTORY_PAGE_COUNT);
	static CGrid s_grid8(5, GIFT_BOX_INVENTORY_MAX_NUM / 5 / SPECIAL_INVENTORY_PAGE_COUNT);
	static CGrid s_grid9(5, CHANGERS_INVENTORY_MAX_NUM / 5 / SPECIAL_INVENTORY_PAGE_COUNT);

	s_grid5.Clear();
	s_grid6.Clear();
	s_grid7.Clear();
	s_grid8.Clear();
	s_grid9.Clear();
#endif

	LPCHARACTER	victim = GetCompany()->GetOwner();
	LPITEM item;

	int i;

	const int perPageSlotCount = INVENTORY_MAX_NUM / 4;

	for (i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		BYTE itemSize = item->GetSize();

		if (i < perPageSlotCount)
			s_grid1.Put(i, 1, itemSize);
		else if (i < perPageSlotCount * 2)
			s_grid2.Put(i - perPageSlotCount, 1, itemSize);
		else if (i < perPageSlotCount * 3)
			s_grid3.Put(i - perPageSlotCount * 2, 1, itemSize);
		else
			s_grid4.Put(i - perPageSlotCount * 3, 1, itemSize);
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	int x;
	int y;

	const int perPageSkillBookSlotCount = SKILL_BOOK_INVENTORY_MAX_NUM / 1;
	const int perPageUpgradeItemsSlotCount = UPGRADE_ITEMS_INVENTORY_MAX_NUM / 1;
	const int perPageStoneSlotCount = STONE_INVENTORY_MAX_NUM / 1;
	const int perPageGiftBoxSlotCount = GIFT_BOX_INVENTORY_MAX_NUM / 1;
	const int perPageChangerSlotCount = CHANGERS_INVENTORY_MAX_NUM / 1;

	for (x = 0; x < SKILL_BOOK_INVENTORY_MAX_NUM; ++x)
	{
		if (!(item = victim->GetSkillBookInventoryItem(x)))
			continue;

		BYTE itemSize = item->GetSize();

		if (x < perPageSkillBookSlotCount)
			s_grid5.Put(x, 1, itemSize);
	}

	for (y = 0; y < UPGRADE_ITEMS_INVENTORY_MAX_NUM; ++y)
	{
		if (!(item = victim->GetUpgradeItemsInventoryItem(y)))
			continue;

		BYTE itemSize = item->GetSize();

		if (y < perPageUpgradeItemsSlotCount)
			s_grid6.Put(y, 1, itemSize);
	}

	for (y = 0; y < STONE_INVENTORY_MAX_NUM; ++y)
	{
		if (!(item = victim->GetStoneInventoryItem(y)))
			continue;

		BYTE itemSize = item->GetSize();

		if (y < perPageStoneSlotCount)
			s_grid7.Put(y, 1, itemSize);
	}

	for (y = 0; y < GIFT_BOX_INVENTORY_MAX_NUM; ++y)
	{
		if (!(item = victim->GetGiftBoxInventoryItem(y)))
			continue;

		BYTE itemSize = item->GetSize();

		if (y < perPageGiftBoxSlotCount)
			s_grid8.Put(y, 1, itemSize);
	}
	for (y = 0; y < CHANGERS_INVENTORY_MAX_NUM; ++y)
	{
		if (!(item = victim->GetChangersInventoryItem(y)))
			continue;

		BYTE itemSize = item->GetSize();

		if (y < perPageChangerSlotCount)
			s_grid9.Put(y, 1, itemSize);
	}
#endif

	static std::vector <WORD> s_vDSGrid(DRAGON_SOUL_INVENTORY_MAX_NUM);
	bool bDSInitialized = false;

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
		int slots;
		if (item->IsDragonSoul())
			slots = victim->GetEmptyDragonSoulInventory(item);
		else
			slots = victim->GetEmptyInventory(item->GetSize());

		if (slots < 0)
			return false;
#endif

		BYTE itemSize = item->GetSize();

		if (item->IsDragonSoul())
		{
			if (!victim->DragonSoul_IsQualified())
			{
				return false;
			}

			if (!bDSInitialized)
			{
				bDSInitialized = true;
				victim->CopyDragonSoulItemGrid(s_vDSGrid);
			}

			bool bExistEmptySpace = false;
			WORD wBasePos = DSManager::instance().GetBasePosition(item);
			if (wBasePos >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				return false;

			for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; i++)
			{
				WORD wPos = wBasePos + i;
				if (0 == s_vDSGrid[wPos])
				{
					bool bEmpty = true;
					for (int j = 1; j < item->GetSize(); j++)
					{
						if (s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM])
						{
							bEmpty = false;
							break;
						}
					}
					if (bEmpty)
					{
						for (int j = 0; j < item->GetSize(); j++)
						{
							s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM] = wPos + 1;
						}
						bExistEmptySpace = true;
						break;
					}
				}
				if (bExistEmptySpace)
					break;
			}
			if (!bExistEmptySpace)
				return false;
		}
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
		{
			BYTE itemSize = item->GetSize();

			int iPos = s_grid5.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid5.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid1.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid1.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid2.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid2.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid3.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid3.Put(iPos, 1, itemSize);
				continue;
			}

			return false;
		}
		else if (item->IsUpgradeItem())
		{
			BYTE itemSize = item->GetSize();

			int iPos = s_grid6.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid6.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid1.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid1.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid2.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid2.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid3.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid3.Put(iPos, 1, itemSize);
				continue;
			}

			return false;
		}
		else if (item->IsStone())
		{
			BYTE itemSize = item->GetSize();

			int iPos = s_grid7.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid7.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid1.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid1.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid2.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid2.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid3.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid3.Put(iPos, 1, itemSize);
				continue;
			}

			return false;
		}
		else if (item->IsGiftBox())
		{
			BYTE itemSize = item->GetSize();

			int iPos = s_grid8.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid8.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid1.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid1.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid2.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid2.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid3.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid3.Put(iPos, 1, itemSize);
				continue;
			}

			return false;
		}
		else if (item->IsChanger())
		{
			BYTE itemSize = item->GetSize();

			int iPos = s_grid9.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid9.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid1.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid1.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid2.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid2.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid3.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid3.Put(iPos, 1, itemSize);
				continue;
			}

			return false;
		}
#endif
		else
		{
			int iPos = s_grid1.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid1.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid2.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid2.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid3.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid3.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid4.FindBlank(1, itemSize);
			if (iPos >= 0)
			{
				s_grid4.Put(iPos, 1, itemSize);
				continue;
			}

			return false;
		}
	}

	return true;
}

bool CExchange::Done()
{
	int empty_pos, i;
	LPITEM item;

	LPCHARACTER	victim = GetCompany()->GetOwner();

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

		if (item->IsDragonSoul())
			empty_pos = victim->GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
			empty_pos = victim->GetEmptySkillBookInventory(item->GetSize());
		else if (item->IsUpgradeItem())
			empty_pos = victim->GetEmptyUpgradeItemsInventory(item->GetSize());
		else if (item->IsStone())
			empty_pos = victim->GetEmptyStoneInventory(item->GetSize());
		else if (item->IsGiftBox())
			empty_pos = victim->GetEmptyGiftBoxInventory(item->GetSize());
		else if (item->IsChanger())
			empty_pos = victim->GetEmptyChangersInventory(item->GetSize());
#endif
		else
			empty_pos = victim->GetEmptyInventory(item->GetSize());

		if (empty_pos < 0)
		{
			sys_err("Exchange::Done : Cannot find blank position in inventory %s <-> %s item %s", m_pOwner->GetName(), victim->GetName(), item->GetName());
			continue;
		}

		assert(empty_pos >= 0);

		m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 255);

#ifdef ENABLE_GROWTH_PET_SYSTEM
		if (item->GetType() == ITEM_GROWTH_PET && item->GetSubType() == PET_BAG)
		{
			LPGROWTH_PET pkPet = item->GetOwner()->GetGrowthPet(item->GetSocket(2));
			if (pkPet)
			{
				item->GetOwner()->DeleteGrowthPet(item->GetSocket(2));
				victim->SetGrowthPet(pkPet);
				pkPet->Save();
			}
		}
#endif

		item->RemoveFromCharacter();
		if (item->IsDragonSoul())
			item->AddToCharacter(victim, TItemPos(DRAGON_SOUL_INVENTORY, empty_pos));
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsStone())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsGiftBox())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsChanger())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
#endif
		else
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		item->SetExchanging(false);
		{
			char exchange_buf[51];

			snprintf(exchange_buf, sizeof(exchange_buf), "%s %u %u", item->GetName(), GetOwner()->GetPlayerID(), item->GetCount());
			LogManager::instance().ItemLog(victim, item, "EXCHANGE_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%s %u %u", item->GetName(), victim->GetPlayerID(), item->GetCount());
			LogManager::instance().ItemLog(GetOwner(), item, "EXCHANGE_GIVE", exchange_buf);

			if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
			{
				LogManager::instance().GoldBarLog(victim->GetPlayerID(), item->GetID(), EXCHANGE_TAKE, "");
				LogManager::instance().GoldBarLog(GetOwner()->GetPlayerID(), item->GetID(), EXCHANGE_GIVE, "");
			}
		}

		m_apItems[i] = NULL;
	}

	if (m_lGold)
	{
		GetOwner()->PointChange(POINT_GOLD, -m_lGold, true);
		victim->PointChange(POINT_GOLD, m_lGold, true);

		if (m_lGold > 1000)
		{
			char exchange_buf[51];
			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", GetOwner()->GetPlayerID(), GetOwner()->GetName());
			LogManager::instance().CharLog(victim, m_lGold, "EXCHANGE_GOLD_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", victim->GetPlayerID(), victim->GetName());
			LogManager::instance().CharLog(GetOwner(), m_lGold, "EXCHANGE_GOLD_GIVE", exchange_buf);
		}
	}

	m_pGrid->Clear();
	return true;
}

bool CExchange::Accept(bool bAccept)
{
	if (m_bAccept == bAccept)
		return true;

	m_bAccept = bAccept;
	if (m_bAccept && GetCompany()->m_bAccept)
	{
		int	iItemCount;

		LPCHARACTER victim = GetCompany()->GetOwner();

		GetOwner()->SetExchangeTime();
		victim->SetExchangeTime();

		if (!Check(&iItemCount))
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang or not enough space in the inventory."));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person does not have enough Yang or does not have any space left in their inventory."));
			goto EXCHANGE_END;
		}

		if (!CheckSpace())
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person has no space left in their inventory."));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in your inventory."));
			goto EXCHANGE_END;
		}

		if (!GetCompany()->Check(&iItemCount))
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang or not enough space in the inventory."));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person does not have enough Yang or does not have any space left in their inventory."));
			goto EXCHANGE_END;
		}

		if (!GetCompany()->CheckSpace())
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person has no space left in their inventory."));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in your inventory."));
			goto EXCHANGE_END;
		}

		if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		{
			sys_err("Cannot use exchange feature while DB cache connection is dead.");
			victim->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			goto EXCHANGE_END;
		}

		if (Done())
		{
			if (m_lGold)
				GetOwner()->Save();

			if (GetCompany()->Done())
			{
				if (GetCompany()->m_lGold)
					victim->Save();

				GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The trade with %s has been successful."), victim->GetName());
				victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The trade with %s has been successful."), GetOwner()->GetName());
			}
		}

EXCHANGE_END:
		Cancel();
		return false;
	}
	else
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, true, m_bAccept, NPOS, 0);
		exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, false, m_bAccept, NPOS, 0);
		return true;
	}
}

void CExchange::Cancel()
{
	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_END, 0, 0, NPOS, 0);
	GetOwner()->SetExchange(NULL);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			m_apItems[i]->SetExchanging(false);
	}

	if (GetCompany())
	{
		GetCompany()->SetCompany(NULL);
		GetCompany()->Cancel();
	}

	M2_DELETE(this);
}

