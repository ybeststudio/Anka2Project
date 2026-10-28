#include "stdafx.h"
#include "buffi_system.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "desc.h"
#include "sectree_manager.h"
#include "packet.h"
#include "protocol.h"
#include "log.h"
#include "skill.h"
#include "unique_item.h"
#include "profiler.h"
#include "marriage.h"
#include "item_addon.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "affect.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/service.h"
#include "../../common/VnumHelper.h"

#ifdef ENABLE_PET_SYSTEM
	#include "PetSystem.h"
#endif

CItem::CItem(DWORD dwVnum)
	: m_dwVnum(dwVnum), m_bWindow(0), m_dwID(0), m_bEquipped(false), m_dwVID(0), m_wCell(0), m_dwCount(0), m_lFlag(0), m_dwLastOwnerPID(0),
	m_bExchanging(false), m_pkDestroyEvent(NULL), m_pkUniqueExpireEvent(NULL), m_pkTimerBasedOnWearExpireEvent(NULL), m_pkRealTimeExpireEvent(NULL),
	m_pkExpireEvent(NULL),
	m_pkAccessorySocketExpireEvent(NULL), m_pkOwnershipEvent(NULL), m_dwOwnershipPID(0), m_bSkipSave(false), m_isLocked(false),
	m_dwMaskVnum(0), m_dwSIGVnum (0), m_bIsChangingAttr(false)
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	, m_pkAuraBoostSocketExpireEvent(NULL)
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	, m_dwTransmutationVnum(0)
#endif
#ifdef ENABLE_SET_ITEM
	, m_dwSetValue(0)
#endif
{
	memset( &m_alSockets, 0, sizeof(m_alSockets) );
	memset( &m_aAttr, 0, sizeof(m_aAttr) );
}

CItem::~CItem()
{
	Destroy();
}

void CItem::Initialize()
{
	CEntity::Initialize(ENTITY_ITEM);

	m_bWindow = RESERVED_WINDOW;
	m_pOwner = NULL;
	m_dwID = 0;
	m_bEquipped = false;
	m_dwVID = m_wCell = m_dwCount = m_lFlag = 0;
	m_pProto = NULL;
	m_bExchanging = false;
	memset(&m_alSockets, 0, sizeof(m_alSockets));
	memset(&m_aAttr, 0, sizeof(m_aAttr));
	m_pkDestroyEvent = NULL;
	m_pkOwnershipEvent = NULL;
	m_dwOwnershipPID = 0;
	m_pkUniqueExpireEvent = NULL;
	m_pkTimerBasedOnWearExpireEvent = NULL;
	m_pkRealTimeExpireEvent = NULL;

	m_pkAccessorySocketExpireEvent = NULL;

	m_bSkipSave = false;
	m_bIsChangingAttr = false;
	m_dwLastOwnerPID = 0;
#ifdef ENABLE_SEALBIND_SYSTEM
	m_lSealDate = 0;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	m_pkAuraBoostSocketExpireEvent = NULL;
#endif
}

void CItem::Destroy()
{
	event_cancel(&m_pkDestroyEvent);
	event_cancel(&m_pkOwnershipEvent);
	event_cancel(&m_pkUniqueExpireEvent);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);
	event_cancel(&m_pkRealTimeExpireEvent);
	event_cancel(&m_pkAccessorySocketExpireEvent);
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	event_cancel(&m_pkAuraBoostSocketExpireEvent);
#endif

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);
}

EVENTFUNC(item_destroy_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "item_destroy_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetOwner())
		sys_err("item_destroy_event: Owner exist. (item %s owner %s)", pkItem->GetName(), pkItem->GetOwner()->GetName());

	pkItem->SetDestroyEvent(NULL);
	M2_DESTROY_ITEM(pkItem);
	return 0;
}

void CItem::SetDestroyEvent(LPEVENT pkEvent)
{
	m_pkDestroyEvent = pkEvent;
}

void CItem::StartDestroyEvent(int iSec)
{
	if (m_pkDestroyEvent)
		return;

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetDestroyEvent(event_create(item_destroy_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::EncodeInsertPacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
		return;

	const PIXEL_POSITION & c_pos = GetXYZ();

	struct packet_item_ground_add pack;

	pack.bHeader	= HEADER_GC_ITEM_GROUND_ADD;
	pack.x		= c_pos.x;
	pack.y		= c_pos.y;
	pack.z		= c_pos.z;
	pack.dwVnum		= GetVnum();
	pack.dwVID		= m_dwVID;
#ifdef ENABLE_RENEWAL_BOOK_NAME
	pack.lSocket0 = ((GetType() == ITEM_SKILLBOOK || GetType() == ITEM_POLYMORPH) ? GetSocket(0) : 0);
#endif

	d->Packet(&pack, sizeof(pack));

	if (m_pkOwnershipEvent != NULL)
	{
		item_event_info * info = dynamic_cast<item_event_info *>(m_pkOwnershipEvent->info);

		if ( info == NULL )
		{
			sys_err( "CItem::EncodeInsertPacket> <Factor> Null pointer" );
			return;
		}

		TPacketGCItemOwnership p;

		p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
		p.dwVID = m_dwVID;
		strlcpy(p.szName, info->szOwnerName, sizeof(p.szName));

		d->Packet(&p, sizeof(TPacketGCItemOwnership));
	}
}

void CItem::EncodeRemovePacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
		return;

	struct packet_item_ground_del pack;

	pack.bHeader = HEADER_GC_ITEM_GROUND_DEL;
	pack.dwVID = m_dwVID;

	d->Packet(&pack, sizeof(pack));
	sys_log(2, "Item::EncodeRemovePacket %s to %s", GetName(), ((LPCHARACTER) ent)->GetName());
}

void CItem::SetProto(const TItemTable * table)
{
	assert(table != NULL);
	m_pProto = table;
	SetFlag(m_pProto->dwFlags);
}

void CItem::UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use *packet)
{
	if (!GetVnum())
		return;

	packet->header 	= HEADER_GC_ITEM_USE;
	packet->ch_vid 	= ch->GetVID();
	packet->victim_vid 	= victim->GetVID();
	packet->Cell = TItemPos(GetWindow(), m_wCell);
	packet->vnum	= GetVnum();
}

void CItem::RemoveFlag(long bit)
{
	REMOVE_BIT(m_lFlag, bit);
}

void CItem::AddFlag(long bit)
{
	SET_BIT(m_lFlag, bit);
}

void CItem::UpdatePacket()
{
	if (!m_pOwner || !m_pOwner->GetDesc())
		return;

#ifdef ENABLE_RENEWAL_SWITCHBOT
	if (m_bWindow == SWITCHBOT)
		return;
#endif

	if (IsChangingAttr())
		return;

	TPacketGCItemUpdate pack;

	pack.header = HEADER_GC_ITEM_UPDATE;
	pack.Cell = TItemPos(GetWindow(), m_wCell);
	pack.count = m_dwCount;

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		pack.alSockets[i] = m_alSockets[i];

	thecore_memcpy(pack.aAttr, GetAttributes(), sizeof(pack.aAttr));

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	pack.dwTransmutationVnum = GetTransmutationVnum();
#endif
#ifdef ENABLE_SET_ITEM
	pack.dwSetValue = GetSetValue();
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
	pack.sealbind = GetSealDate();
#endif

	sys_log(2, "UpdatePacket %s -> %s", GetName(), m_pOwner->GetName());
	m_pOwner->GetDesc()->Packet(&pack, sizeof(pack));
}

DWORD CItem::GetCount()
{
	if (GetType() == ITEM_ELK) return MIN(m_dwCount, INT_MAX);
	else
	{
#ifdef ENABLE_STACK_LIMIT
		return MIN(m_dwCount, ITEM_MAX_COUNT);
#else
		return MIN(m_dwCount, 200);
#endif
	}
}

bool CItem::SetCount(DWORD count)
{
	if (GetType() == ITEM_ELK)
	{
		m_dwCount = MIN(count, INT_MAX);
	}
	else
	{
		m_dwCount = MIN(count, ITEM_MAX_COUNT);
	}

	if (count == 0 && m_pOwner)
	{
		if (GetSubType() == USE_ABILITY_UP || GetSubType() == USE_POTION || GetVnum() == 70020)
		{
			LPCHARACTER pOwner = GetOwner();
			WORD wCell = GetCell();

			RemoveFromCharacter();

			if (!IsDragonSoul())
			{
				LPITEM pItem = pOwner->FindSpecifyItem(GetVnum());

				if (NULL != pItem)
				{
					pOwner->ChainQuickslotItem(pItem, QUICKSLOT_TYPE_ITEM, wCell);
				}
				else
				{
					pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, wCell, 255);
				}
			}

			M2_DESTROY_ITEM(this);
		}
		else
		{
			if (!IsDragonSoul())
			{
				m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, m_wCell, 255);
			}
			M2_DESTROY_ITEM(RemoveFromCharacter());
		}

		return false;
	}

	UpdatePacket();

	Save();
	return true;
}

LPITEM CItem::RemoveFromCharacter()
{
	if (!m_pOwner)
	{
		sys_err("Item::RemoveFromCharacter owner null");
		return (this);
	}

	LPCHARACTER pOwner = m_pOwner;

	if (m_bEquipped)
	{
		Unequip();

		SetWindow(RESERVED_WINDOW);
		Save();
		return (this);
	}
	else
	{
		if (GetWindow() != SAFEBOX && GetWindow() != MALL)
		{
			if (IsDragonSoul())
			{
				if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					sys_err("CItem::RemoveFromCharacter: pos >= DRAGON_SOUL_INVENTORY_MAX_NUM");
				else
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), NULL);
			}
#ifdef ENABLE_RENEWAL_SWITCHBOT
			else if (m_bWindow == SWITCHBOT)
			{
				if (m_wCell >= SWITCHBOT_SLOT_COUNT)
				{
					sys_err("CItem::RemoveFromCharacter: pos >= SWITCHBOT_SLOT_COUNT");
				}
				else
				{
					pOwner->SetItem(TItemPos(SWITCHBOT, m_wCell), NULL);
				}
			}
#endif
			else
			{
				TItemPos cell(INVENTORY, m_wCell);

				if (false == cell.IsDefaultInventoryPosition() && false == cell.IsBeltInventoryPosition()
#ifdef ENABLE_SPECIAL_INVENTORY
					&& false == cell.IsSkillBookInventoryPosition()
					&& false == cell.IsUpgradeItemsInventoryPosition()
					&& false == cell.IsStoneInventoryPosition()
					&& false == cell.IsGiftBoxInventoryPosition()
					&& false == cell.IsChangersInventoryPosition()
#endif
					)
					sys_err("CItem::RemoveFromCharacter: Invalid Item Position");
				else
				{
					pOwner->SetItem(cell, NULL);
				}
			}
		}

		m_pOwner = NULL;
		m_wCell = 0;

		SetWindow(RESERVED_WINDOW);
		Save();
		return (this);
	}
}

#ifdef ENABLE_PICKUP_ITEM_EFFECT
bool CItem::AddToCharacter(LPCHARACTER ch, TItemPos Cell, bool bHighlight)
#else
bool CItem::AddToCharacter(LPCHARACTER ch, TItemPos Cell)
#endif
{
	assert(GetSectree() == NULL);
	assert(m_pOwner == NULL);
	WORD pos = Cell.cell;
	BYTE window_type = Cell.window_type;

	if (INVENTORY == window_type)
	{
		if (m_wCell >= INVENTORY_MAX_NUM && BELT_INVENTORY_SLOT_START > m_wCell)
		{
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}

	else if (DRAGON_SOUL_INVENTORY == window_type)
	{
		if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}

#ifdef ENABLE_RENEWAL_SWITCHBOT
	else if (SWITCHBOT == window_type)
	{
		if (m_wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CItem::AddToCharacter:switchbot cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
#endif

	if (ch->GetDesc())
		m_dwLastOwnerPID = ch->GetPlayerID();

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((GetType() == ITEM_COSTUME) && (GetSubType() == COSTUME_ACCE) && (GetSocket(ACCE_ABSORPTION_SOCKET) == 0))
	{
		long lVal = GetValue(ACCE_GRADE_VALUE_FIELD);
		switch (lVal)
		{
			case 2:
				lVal = ACCE_GRADE_2_ABS;
				break;
			case 3:
				lVal = ACCE_GRADE_3_ABS;
				break;
			case 4:
				lVal = number(ACCE_GRADE_4_ABS_MIN, ACCE_GRADE_4_ABS_MAX_COMB);
				break;

			default:
				lVal = ACCE_GRADE_1_ABS;
				break;
		}
		SetSocket(ACCE_ABSORPTION_SOCKET, lVal);
	}
#endif

	event_cancel(&m_pkDestroyEvent);

#ifdef ENABLE_PICKUP_ITEM_EFFECT
	ch->SetItem(TItemPos(window_type, pos), this, bHighlight);
#else
	ch->SetItem(TItemPos(window_type, pos), this);
#endif
	m_pOwner = ch;

	Save();
	return true;
}

LPITEM CItem::RemoveFromGround()
{
	if (GetSectree())
	{
		SetOwnership(NULL);
		
		GetSectree()->RemoveEntity(this);
		
		ViewCleanup();

		Save();
	}

	return (this);
}

bool CItem::AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck)
{
	if (0 == lMapIndex)
	{
		sys_err("wrong map index argument: %d", lMapIndex);
		return false;
	}

	if (GetSectree())
	{
		sys_err("sectree already assigned");
		return false;
	}

	if (!skipOwnerCheck && m_pOwner)
	{
		sys_err("owner pointer not null");
		return false;
	}

	LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, pos.x, pos.y);

	if (!tree)
	{
		sys_err("cannot find sectree by %dx%d", pos.x, pos.y);
		return false;
	}

	SetWindow(GROUND);
	SetXYZ(pos.x, pos.y, pos.z);
	tree->InsertEntity(this);
	UpdateSectree();
	Save();
	return true;
}

bool CItem::DistanceValid(LPCHARACTER ch)
{
	if (!GetSectree())
		return false;

	int iDist = DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY());

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	const WORD distance = ch->FindAffect(AFFECT_AUTO_PICK_UP) != NULL ? 600 : 300;
	if (iDist > distance)
#else
	if (iDist > 300)
#endif
		return false;

	return true;
}

bool CItem::CanUsedBy(LPCHARACTER ch)
{
	// Anti flag check
	switch (ch->GetJob())
	{
		case JOB_WARRIOR:
			if (GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;
	}

	return true;
}

int CItem::FindEquipCell(LPCHARACTER ch, int iCandidateCell)
{
	// Costume items (ITEM_COSTUME) do not need WearFlag. (Dividing the wear position by sub type. Do I need to give a wear flag again..)
	// Dragon Soul Stones (ITEM_DS, ITEM_SPECIAL_DS) are also classified by SUB_TYPE. New rings and belts are classified by ITEM_TYPE -_-
	if ((0 == GetWearFlag() || ITEM_TOTEM == GetType()) && ITEM_COSTUME != GetType() && ITEM_DS != GetType() && ITEM_SPECIAL_DS != GetType() && ITEM_RING != GetType()
#ifdef ENABLE_MOUNT_SYSTEM
		&& ITEM_MOUNT != GetType()
#endif
#ifdef ENABLE_PET_SYSTEM
		&& ITEM_PET != GetType()
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
		&& ITEM_PASSIVE != GetType()
#endif
		)
	{
		return -1;
	}

	/* Dragon Soul Stone slots cannot be processed as WEAR (up to 32 WEARs are possible, but if you add Dragon Soul Stones, it will exceed 32.)
	* From a specific location in the inventory ((INVENTORY_MAX_NUM + WEAR_MAX_NUM) to (INVENTORY_MAX_NUM + WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX - 1))
	* Determined by Dragon Soul Stone slot.
	* When returning, the reason for subtracting INVENTORY_MAX_NUM is,
	* Because WearCell originally returns INVENTORY_MAX_NUM.
	*/
	if (GetType() == ITEM_DS || GetType() == ITEM_SPECIAL_DS)
	{
		if (iCandidateCell < 0)
		{
			return WEAR_MAX_NUM + GetSubType();
		}
		else
		{
			for (int i = 0; i < DRAGON_SOUL_DECK_MAX_NUM; i++)
			{
				if (WEAR_MAX_NUM + i * DS_SLOT_MAX + GetSubType() == iCandidateCell)
				{
					return iCandidateCell;
				}
			}
			return -1;
		}
	}
	else if (GetType() == ITEM_COSTUME)
	{
		if (GetSubType() == COSTUME_BODY)
			return WEAR_COSTUME_BODY;
		else if (GetSubType() == COSTUME_HAIR)
			return WEAR_COSTUME_HAIR;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		else if (GetSubType() == COSTUME_ACCE)
			return WEAR_COSTUME_ACCE;
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		else if (GetSubType() == COSTUME_WEAPON)
			return WEAR_COSTUME_WEAPON;
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
		else if (GetSubType() == COSTUME_MOUNT)
			return WEAR_COSTUME_MOUNT;

		else if (GetSubType() == COSTUME_PET)
			return WEAR_COSTUME_PET;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		else if (GetSubType() == COSTUME_AURA)
			return WEAR_COSTUME_AURA;
#endif
	}
	else if (GetType() == ITEM_RING)
	{
		if (ch->GetWear(WEAR_RING1))
			return WEAR_RING2;
		else
			return WEAR_RING1;
	}
#ifdef ENABLE_MOUNT_SYSTEM
	else if (GetType() == ITEM_MOUNT)
		return WEAR_MOUNT;
#endif
#ifdef ENABLE_PET_SYSTEM
	else if (GetType() == ITEM_PET)
		return WEAR_PET;
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
	else if (GetType() == ITEM_PASSIVE)
		return WEAR_PASSIVE;
#endif

	else if (GetWearFlag() & WEARABLE_BODY)
		return WEAR_BODY;
	else if (GetWearFlag() & WEARABLE_HEAD)
		return WEAR_HEAD;
	else if (GetWearFlag() & WEARABLE_FOOTS)
		return WEAR_FOOTS;
	else if (GetWearFlag() & WEARABLE_WRIST)
		return WEAR_WRIST;
	else if (GetWearFlag() & WEARABLE_WEAPON)
		return WEAR_WEAPON;
	else if (GetWearFlag() & WEARABLE_SHIELD)
		return WEAR_SHIELD;
	else if (GetWearFlag() & WEARABLE_NECK)
		return WEAR_NECK;
	else if (GetWearFlag() & WEARABLE_EAR)
		return WEAR_EAR;
	else if (GetWearFlag() & WEARABLE_ARROW)
		return WEAR_ARROW;

	else if (GetWearFlag() & WEARABLE_UNIQUE)
	{
		if (ch->GetWear(WEAR_UNIQUE1))
			return WEAR_UNIQUE2;
		else
			return WEAR_UNIQUE1;
	}
	else if (GetWearFlag() == WEARABLE_BELT)
		return WEAR_BELT;
#ifdef ENABLE_PENDANT_SYSTEM
	else if (GetWearFlag() & WEARABLE_PENDANT)
	{
		return WEAR_PENDANT;
	}
#endif

	// Once the items for the collection quest are embedded, you can never -E.
	else if (GetWearFlag() & WEARABLE_ABILITY)
	{
		if (!ch->GetWear(WEAR_ABILITY1))
		{
			return WEAR_ABILITY1;
		}
		else if (!ch->GetWear(WEAR_ABILITY2))
		{
			return WEAR_ABILITY2;
		}
		else if (!ch->GetWear(WEAR_ABILITY3))
		{
			return WEAR_ABILITY3;
		}
		else if (!ch->GetWear(WEAR_ABILITY4))
		{
			return WEAR_ABILITY4;
		}
		else if (!ch->GetWear(WEAR_ABILITY5))
		{
			return WEAR_ABILITY5;
		}
		else if (!ch->GetWear(WEAR_ABILITY6))
		{
			return WEAR_ABILITY6;
		}
		else if (!ch->GetWear(WEAR_ABILITY7))
		{
			return WEAR_ABILITY7;
		}
		else if (!ch->GetWear(WEAR_ABILITY8))
		{
			return WEAR_ABILITY8;
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

void CItem::ModifyPoints(bool bAdd)
{
#ifdef ENABLE_BUFFI_SYSTEM
	// Buffi seal / costumes only change the Buffi, they give the owner nothing
	if (m_bEquipped && GetCell() >= INVENTORY_MAX_NUM && CBuffiManager::IsBuffiWearCell(GetCell() - INVENTORY_MAX_NUM))
		return;
#endif
	int accessoryGrade;

	if (false == IsAccessoryForSocket())
	{
		if (m_pProto->bType == ITEM_WEAPON || m_pProto->bType == ITEM_ARMOR)
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				DWORD dwVnum;

				if ((dwVnum = GetSocket(i)) <= 2)
					continue;

				TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

				if (!p)
				{
					sys_err("cannot find table by vnum %u", dwVnum);
					continue;
				}

				if (ITEM_METIN == p->bType)
				{
					for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
					{
						if (p->aApplies[i].bType == APPLY_NONE)
							continue;

						if (p->aApplies[i].bType == APPLY_SKILL)
							m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? p->aApplies[i].lValue : p->aApplies[i].lValue ^ 0x00800000);
						else
							m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? p->aApplies[i].lValue : -p->aApplies[i].lValue);
					}
				}
			}
		}

		accessoryGrade = 0;
	}
	else
	{
		accessoryGrade = MIN(GetAccessorySocketGrade(), ITEM_ACCESSORY_SOCKET_MAX_NUM);
	}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((GetType() == ITEM_COSTUME) && (GetSubType() == COSTUME_ACCE) && (GetSocket(ACCE_ABSORBED_SOCKET)))
	{
		TItemTable * pkItemAbsorbed = ITEM_MANAGER::instance().GetTable(GetSocket(ACCE_ABSORBED_SOCKET));
		if (pkItemAbsorbed)
		{
			if ((pkItemAbsorbed->bType == ITEM_ARMOR) && (pkItemAbsorbed->bSubType == ARMOR_BODY))
			{
				auto lDefGrade = CalcAcceBonus(pkItemAbsorbed->alValues[1] + (pkItemAbsorbed->alValues[5] * 2));
				m_pOwner->ApplyPoint(APPLY_DEF_GRADE_BONUS, bAdd ? lDefGrade : -lDefGrade);

				auto lDefMagicBonus = CalcAcceBonus(pkItemAbsorbed->alValues[0]);
				m_pOwner->ApplyPoint(APPLY_MAGIC_DEF_GRADE, bAdd ? lDefMagicBonus : -lDefMagicBonus);
			}
			else if (pkItemAbsorbed->bType == ITEM_WEAPON)
			{
				if (pkItemAbsorbed->alValues[3] + pkItemAbsorbed->alValues[4] > 0)
				{
					auto lAttGrade = CalcAcceBonus(pkItemAbsorbed->alValues[4] + pkItemAbsorbed->alValues[5]);
					if (pkItemAbsorbed->alValues[3] > pkItemAbsorbed->alValues[4])
						lAttGrade = CalcAcceBonus(pkItemAbsorbed->alValues[3] + pkItemAbsorbed->alValues[5]);
					m_pOwner->ApplyPoint(APPLY_ATT_GRADE_BONUS, bAdd ? lAttGrade : -lAttGrade);
				}

				if (pkItemAbsorbed->alValues[1] + pkItemAbsorbed->alValues[2] > 0)
				{
					long lAttMagicGrade = CalcAcceBonus(pkItemAbsorbed->alValues[2] + pkItemAbsorbed->alValues[5]);
					if (pkItemAbsorbed->alValues[1] > pkItemAbsorbed->alValues[2])
						lAttMagicGrade = CalcAcceBonus(pkItemAbsorbed->alValues[1] + pkItemAbsorbed->alValues[5]);
					m_pOwner->ApplyPoint(APPLY_MAGIC_ATT_GRADE, bAdd ? lAttMagicGrade : -lAttMagicGrade);
				}
			}
		}
	}
#endif

#ifdef ENABLE_PASSIVE_SYSTEM
	if (CItemVnumHelper::IsPassive(GetVnum()) && GetSubType() == PASSIVE_JOB && GetSocket(1) == 0)
		return;
#endif

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (m_pProto->aApplies[i].bType == APPLY_NONE)
			continue;

#ifdef ENABLE_MOUNT_SYSTEM
		if(IsMountItem())
			continue;
#endif

#ifdef ENABLE_PET_SYSTEM
		if(IsPetItem())
			continue;
#endif

#ifdef ENABLE_MOUNT_PET_SKIN
		if(IsCostumeMountSkin())
			continue;

		if(IsCostumePetSkin())
			continue;
#endif

		long value = m_pProto->aApplies[i].lValue;

		if (m_pProto->aApplies[i].bType == APPLY_SKILL)
		{
			m_pOwner->ApplyPoint(m_pProto->aApplies[i].bType, bAdd ? value : value ^ 0x00800000);
		}
		else
		{
			if (0 != accessoryGrade && i < ITEM_APPLY_MAX_NUM - 1) // Fix
				value += MAX(accessoryGrade, value * aiAccessorySocketEffectivePct[accessoryGrade] / 100);
			m_pOwner->ApplyPoint(m_pProto->aApplies[i].bType, bAdd ? value : -value);
		}
	}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_ACCE)
	{
		TItemTable * pkItemAbsorbed = ITEM_MANAGER::instance().GetTable(GetSocket(ACCE_ABSORBED_SOCKET));
		if (pkItemAbsorbed)
		{
			for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				if (pkItemAbsorbed->aApplies[i].bType == APPLY_NONE)
					continue;
				int value = CalcAcceBonus(pkItemAbsorbed->aApplies[i].lValue);
				if (pkItemAbsorbed->aApplies[i].bType == APPLY_SKILL)
					m_pOwner->ApplyPoint(pkItemAbsorbed->aApplies[i].bType, bAdd ? value : value ^ 0x00800000);
				else
				{
					if (0 != accessoryGrade && i < ITEM_APPLY_MAX_NUM - 1) // Fix
						value += MAX(accessoryGrade, value * aiAccessorySocketEffectivePct[accessoryGrade] / 100);
					m_pOwner->ApplyPoint(pkItemAbsorbed->aApplies[i].bType, bAdd ? value : -value);
				}
			}
		}
	}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	float fAuraDrainPer = 0.0f;
	if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA)
	{
		const long c_lLevelSocket = GetSocket(ITEM_SOCKET_AURA_CURRENT_LEVEL);
		const long c_lDrainSocket = GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);
		const long c_lBoostSocket = GetSocket(ITEM_SOCKET_AURA_BOOST);

		BYTE bCurLevel = (c_lLevelSocket / 100000) - 1000;
		BYTE bBoostIndex = c_lBoostSocket / 100000000;

		TItemTable* pBoosterProto = ITEM_MANAGER::instance().GetTable(ITEM_AURA_BOOST_ITEM_VNUM_BASE + bBoostIndex);
		fAuraDrainPer = (1.0f * bCurLevel / 10.0f) / 100.0f;
		if (pBoosterProto)
			fAuraDrainPer += 1.0f * pBoosterProto->alValues[ITEM_AURA_BOOST_PERCENT_VALUE] / 100.0f;

		TItemTable* p = NULL;
		if (c_lDrainSocket != 0)
			p = ITEM_MANAGER::instance().GetTable(c_lDrainSocket);

		if (p != NULL && (ITEM_ARMOR == p->bType && (ARMOR_SHIELD == p->bSubType || ARMOR_WRIST == p->bSubType || ARMOR_NECK == p->bSubType || ARMOR_EAR == p->bSubType)))
		{
			for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				if (p->aApplies[i].bType == APPLY_NONE || p->aApplies[i].bType == APPLY_MOV_SPEED || p->aApplies[i].lValue <= 0)
					continue;

				float fValue = p->aApplies[i].lValue * fAuraDrainPer;
				int iValue = static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));

				if (p->aApplies[i].bType == APPLY_SKILL)
					m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? iValue : iValue ^ 0x00800000);
				else
					m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? iValue : -iValue);
			}
		}
	}
#endif

	if (true == CItemVnumHelper::IsRamadanMoonRing(GetVnum()) || true == CItemVnumHelper::IsHalloweenCandy(GetVnum()) || true == CItemVnumHelper::IsHappinessRing(GetVnum()) || true == CItemVnumHelper::IsLovePendant(GetVnum())) {}
	else
	{
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (GetAttributeType(i))
			{
				const TPlayerItemAttribute& ia = GetAttribute(i);
				auto value = ia.sValue;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
				if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_ACCE)
					value = CalcAcceBonus(value);
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
				if (ia.bType == APPLY_SKILL)
				{
					if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA)
					{
						if (ia.sValue <= 0)
							continue;

						float fValue = ia.sValue * fAuraDrainPer;
						int iValue = static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));
						m_pOwner->ApplyPoint(ia.bType, bAdd ? iValue : iValue ^ 0x00800000);
					}
					else
						m_pOwner->ApplyPoint(ia.bType, bAdd ? ia.sValue : ia.sValue ^ 0x00800000);
				}
				else
				{
					if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA)
					{
						if (ia.sValue <= 0)
							continue;

						float fValue = ia.sValue * fAuraDrainPer;
						int iValue = static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));
						m_pOwner->ApplyPoint(ia.bType, bAdd ? iValue : -iValue);
					}
					else
						m_pOwner->ApplyPoint(ia.bType, bAdd ? ia.sValue : -ia.sValue);
				}
#else
				if (ia.bType == APPLY_SKILL)
					m_pOwner->ApplyPoint(ia.bType, bAdd ? value : value ^ 0x00800000);
				else
					m_pOwner->ApplyPoint(ia.bType, bAdd ? value : -value);
#endif
			}
		}
	}

	switch (m_pProto->bType)
	{
		case ITEM_PICK:
		case ITEM_ROD:
			{
				if (bAdd)
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
						m_pOwner->SetPart(PART_WEAPON, GetVnum());
				}
				else
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
						m_pOwner->SetPart(PART_WEAPON, 0);
#else
						m_pOwner->SetPart(PART_WEAPON, m_pOwner->GetOriginalPart(PART_WEAPON));
#endif
				}
			}
			break;

		case ITEM_WEAPON:
			{
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				if (0 != m_pOwner->GetWear(WEAR_COSTUME_WEAPON))
					break;
#endif
				if (bAdd)
				{
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
					{
						const WORD wVnum = static_cast<WORD>(GetVnum());
						const WORD wTransmutationVnum = static_cast<WORD>(GetTransmutationVnum());
						m_pOwner->SetPart(PART_WEAPON, wTransmutationVnum != 0 ? wTransmutationVnum : wVnum);
					}
#else
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
						m_pOwner->SetPart(PART_WEAPON, GetVnum());
#endif
				}
				else
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
						m_pOwner->SetPart(PART_WEAPON, 0);
#else
						m_pOwner->SetPart(PART_WEAPON, m_pOwner->GetOriginalPart(PART_WEAPON));
#endif
				}
			}
			break;

		case ITEM_ARMOR:
			{
				if (0 != m_pOwner->GetWear(WEAR_COSTUME_BODY))
					break;

				if (GetSubType() == ARMOR_BODY || GetSubType() == ARMOR_HEAD || GetSubType() == ARMOR_FOOTS || GetSubType() == ARMOR_SHIELD)
				{
					if (bAdd)
					{
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
						if (GetProto()->bSubType == ARMOR_BODY)
						{
							const WORD wVnum = static_cast<WORD>(GetVnum());
							const WORD wTransmutationVnum = static_cast<WORD>(GetTransmutationVnum());
							m_pOwner->SetPart(PART_MAIN, wTransmutationVnum != 0 ? wTransmutationVnum : wVnum);
						}
#else
						if (GetProto()->bSubType == ARMOR_BODY)
							m_pOwner->SetPart(PART_MAIN, GetVnum());
#endif
					}
					else
					{
						if (GetProto()->bSubType == ARMOR_BODY)
							m_pOwner->SetPart(PART_MAIN, m_pOwner->GetOriginalPart(PART_MAIN));
					}
				}
			}
			break;

		case ITEM_COSTUME:
			{
				DWORD toSetValue = this->GetVnum();
				EParts toSetPart = PART_MAX_NUM;

				if (GetSubType() == COSTUME_BODY)
				{
					toSetPart = PART_MAIN;
					if (false == bAdd)
					{
						const CItem* pArmor = m_pOwner->GetWear(WEAR_BODY);
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
						if (pArmor)
							toSetValue = pArmor->GetTransmutationVnum() != 0 ? pArmor->GetTransmutationVnum() : pArmor->GetVnum();
						else
							toSetValue = m_pOwner->GetOriginalPart(PART_MAIN);
#else
						toSetValue = (NULL != pArmor) ? pArmor->GetVnum() : m_pOwner->GetOriginalPart(PART_MAIN);
#endif
					}
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
					else
						toSetValue = GetTransmutationVnum() != 0 ? GetTransmutationVnum() : GetVnum();
#endif
				}

				else if (GetSubType() == COSTUME_HAIR)
				{
					toSetPart = PART_HAIR;

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
					const DWORD c_dwTransmutationVnum = GetTransmutationVnum();
					if (c_dwTransmutationVnum != 0)
					{
						TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(c_dwTransmutationVnum);
						toSetValue = (pItemTable != NULL) ? pItemTable->alValues[3] : GetValue(3);
					}
					else
						toSetValue = (true == bAdd) ? this->GetValue(3) : 0;
#else
					toSetValue = (true == bAdd) ? this->GetValue(3) : 0;
#endif
				}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
				else if (GetSubType() == COSTUME_ACCE)
				{
					toSetValue -= ACCE_BASE_VNUM;
					if (GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
						toSetValue += ACCE_EFFECT_VNUM;

					toSetValue = (true == bAdd) ? toSetValue : 0;
					toSetPart = PART_ACCE;
				}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				else if (GetSubType() == COSTUME_WEAPON)
				{
					toSetPart = PART_WEAPON;
					if (false == bAdd)
					{
						const CItem* pWeapon = m_pOwner->GetWear(WEAR_WEAPON);
						toSetValue = (NULL != pWeapon) ? pWeapon->GetVnum() : m_pOwner->GetOriginalPart(PART_WEAPON);
					}
				}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
				else if (GetSubType() == COSTUME_AURA)
				{
					toSetPart = PART_AURA;
					toSetValue = (true == bAdd) ? this->GetVnum() : 0;
				}
#endif
				if (PART_MAX_NUM != toSetPart)
				{
					m_pOwner->SetPart((BYTE)toSetPart, toSetValue);
					m_pOwner->UpdatePacket();
				}
			}
			break;
		case ITEM_UNIQUE:
			{
				if (0 != GetSIGVnum())
				{
					const CSpecialItemGroup* pItemGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(GetSIGVnum());
					if (NULL == pItemGroup)
						break;
					DWORD dwAttrVnum = pItemGroup->GetAttrVnum(GetVnum());
					const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(dwAttrVnum);
					if (NULL == pAttrGroup)
						break;
					for (itertype (pAttrGroup->m_vecAttrs) it = pAttrGroup->m_vecAttrs.begin(); it != pAttrGroup->m_vecAttrs.end(); it++)
					{
						m_pOwner->ApplyPoint(it->apply_type, bAdd ? it->apply_value : -it->apply_value);
					}
				}
			}
			break;
	}
}

bool CItem::IsEquipable() const
{
	switch (this->GetType())
	{
		case ITEM_COSTUME:
		case ITEM_ARMOR:
		case ITEM_WEAPON:
		case ITEM_ROD:
		case ITEM_PICK:
		case ITEM_UNIQUE:
		case ITEM_DS:
		case ITEM_SPECIAL_DS:
		case ITEM_RING:
#ifdef ENABLE_MOUNT_SYSTEM
		case ITEM_MOUNT:
#endif
#ifdef ENABLE_PET_SYSTEM
		case ITEM_PET:
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
		case ITEM_PASSIVE:
#endif
			return true;
	}

	return false;
}

bool CItem::EquipTo(LPCHARACTER ch, BYTE bWearCell)
{
	if (!ch)
	{
		sys_err("EquipTo: nil character");
		return false;
	}

	// Dragon Soul Slot index is greater than WEAR_MAX_NUM.
	if (IsDragonSoul())
	{
		if (bWearCell < WEAR_MAX_NUM || bWearCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
		{
			sys_err("EquipTo: invalid dragon soul cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetSubType(), bWearCell - WEAR_MAX_NUM);
			return false;
		}
	}
	else
	{
		if (bWearCell >= WEAR_MAX_NUM)
		{
			sys_err("EquipTo: invalid wear cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetWearFlag(), bWearCell);
			return false;
		}
	}

	if (ch->GetWear(bWearCell))
	{
		sys_err("EquipTo: item already exist (this: #%d %s cell: %d %s)", GetOriginalVnum(), GetName(), bWearCell, ch->GetWear(bWearCell)->GetName());
		return false;
	}

	if (GetOwner())
		RemoveFromCharacter();

	ch->SetWear(bWearCell, this);

	m_pOwner = ch;
	m_bEquipped = true;
	m_wCell	= INVENTORY_MAX_NUM + bWearCell;

	// Fix
	DWORD dwImmuneFlag = 0;
	LPITEM item = NULL;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (item=m_pOwner->GetWear(i))
		{
			if (item->GetImmuneFlag() != 0)
				SET_BIT(dwImmuneFlag, item->GetImmuneFlag());
			if (item->GetAttributeCount() > 0)
			{
				if (item->HasAttr(APPLY_IMMUNE_STUN))
					SET_BIT(dwImmuneFlag, IMMUNE_STUN);
				if (item->HasAttr(APPLY_IMMUNE_SLOW))
					SET_BIT(dwImmuneFlag, IMMUNE_SLOW);
				if (item->HasAttr(APPLY_IMMUNE_FALL))
					SET_BIT(dwImmuneFlag, IMMUNE_FALL);
			}
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);

	if (IsDragonSoul())
	{
		DSManager::instance().ActivateDragonSoul(this);
	}
	else
	{
		ModifyPoints(true);
		StartUniqueExpireEvent();
		if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
			StartTimerBasedOnWearExpireEvent();

		// ACCESSORY_REFINE
		StartAccessorySocketExpireEvent();
		// END_OF_ACCESSORY_REFINE
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		StartAuraBoosterSocketExpireEvent();
#endif
	}

#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
	if (GetVnum() == DBONE_VNUM_1 || GetVnum() == DBONE_VNUM_2 || GetVnum() == DBONE_VNUM_3 || GetVnum() == DBONE_VNUM_4 || GetVnum() == DBONE_VNUM_5)
	{
		if (!ch->IsAffectFlag(AFFECT_DBONE_BRONZE))
			ch->AddAffect(AFFECT_DBONE_BRONZE, POINT_NONE, 0, AFF_DBONE_BRONZE, INFINITE_AFFECT_DURATION, 0, false);
	}

	if (GetVnum() == DBONE_VNUM_6 || GetVnum() == DBONE_VNUM_7 || GetVnum() == DBONE_VNUM_8)
	{
		if (!ch->IsAffectFlag(AFFECT_DBONE_SILVER))
			ch->AddAffect(AFFECT_DBONE_SILVER, POINT_NONE, 0, AFF_DBONE_SILVER, INFINITE_AFFECT_DURATION, 0, false);
	}

	if (GetVnum() == DBONE_VNUM_9 || GetVnum() == DBONE_VNUM_10 || GetVnum() == DBONE_VNUM_11)
	{
		if (!ch->IsAffectFlag(AFFECT_DBONE_GOLD))
			ch->AddAffect(AFFECT_DBONE_GOLD, POINT_NONE, 0, AFF_DBONE_GOLD, INFINITE_AFFECT_DURATION, 0, false);
	}

	if (GetVnum() == DBONE_VNUM_12 || GetVnum() == DBONE_VNUM_13 || GetVnum() == DBONE_VNUM_14 || GetVnum() == DBONE_VNUM_15 || GetVnum() == DBONE_VNUM_16)
	{
		if (!ch->IsAffectFlag(AFFECT_DBONE_DIAMOND))
			ch->AddAffect(AFFECT_DBONE_DIAMOND, POINT_NONE, 0, AFF_DBONE_DIAMOND, INFINITE_AFFECT_DURATION, 0, false);
	}

	if (GetVnum() == DBONE_VNUM_17 || GetVnum() == DBONE_VNUM_18 || GetVnum() == DBONE_VNUM_19)
	{
		if (!ch->IsAffectFlag(AFFECT_DBONE_PLATIN))
			ch->AddAffect(AFFECT_DBONE_PLATIN, POINT_NONE, 0, AFF_DBONE_PLATIN, INFINITE_AFFECT_DURATION, 0, false);
	}

	if (GetVnum() == DBONE_VNUM_20 || GetVnum() == DBONE_VNUM_21 || GetVnum() == DBONE_VNUM_22)
	{
		if (!ch->IsAffectFlag(AFFECT_DBONE_EMERALD))
			ch->AddAffect(AFFECT_DBONE_EMERALD, POINT_NONE, 0, AFF_DBONE_EMERALD, INFINITE_AFFECT_DURATION, 0, false);
	}
#endif

	ch->BuffOnAttr_AddBuffsFromItem(this);

#ifdef ENABLE_MOUNT_SYSTEM
	if (IsMountItem())
		ch->MountSummon(this);
#endif

#ifdef ENABLE_PET_SYSTEM
	if (IsPetItem())
		ch->PetSummon(this);
#endif

#ifdef ENABLE_MOUNT_PET_SKIN
	if (IsCostumeMountSkin())
		m_pOwner->EquipCostumeMountSkin();

	if (IsCostumePetSkin())
		m_pOwner->EquipCostumePetSkin();
#endif

	m_pOwner->ComputeBattlePoints();

	m_pOwner->UpdatePacket();

	Save();

#ifdef ENABLE_BUFFI_SYSTEM
	if (CBuffiManager::IsBuffiWearCell(bWearCell))
		CBuffiManager::instance().OnWearChanged(m_pOwner, bWearCell, true);
#endif

	return (true);
}

bool CItem::Unequip()
{
	if (!m_pOwner || GetCell() < INVENTORY_MAX_NUM)
	{
		// ITEM_OWNER_INVALID_PTR_BUG
		sys_err("%s %u m_pOwner %p, GetCell %d",
				GetName(), GetID(), get_pointer(m_pOwner), GetCell());
		// END_OF_ITEM_OWNER_INVALID_PTR_BUG
		return false;
	}

	if (this != m_pOwner->GetWear(GetCell() - INVENTORY_MAX_NUM))
	{
		sys_err("m_pOwner->GetWear() != this");
		return false;
	}

#ifdef ENABLE_MOUNT_SYSTEM
	if (IsMountItem())
		m_pOwner->MountUnsummon(this);
#endif

#ifdef ENABLE_PET_SYSTEM
	if (IsPetItem())
		m_pOwner->PetUnsummon(this);
#endif

#ifdef ENABLE_MOUNT_PET_SKIN
	if (IsCostumeMountSkin())
		m_pOwner->RemoveCostumeMountSkin(this);

	if (IsCostumePetSkin())
		m_pOwner->RemoveCostumePetSkin(this);
#endif

	if (IsRideItem())
		ClearMountAttributeAndAffect();

#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
	if (GetVnum() == DBONE_VNUM_1 || GetVnum() == DBONE_VNUM_2 || GetVnum() == DBONE_VNUM_3 || GetVnum() == DBONE_VNUM_4 || GetVnum() == DBONE_VNUM_5) { m_pOwner->RemoveAffect(AFFECT_DBONE_BRONZE); }
	if (GetVnum() == DBONE_VNUM_6 || GetVnum() == DBONE_VNUM_7 || GetVnum() == DBONE_VNUM_8) { m_pOwner->RemoveAffect(AFFECT_DBONE_SILVER); }
	if (GetVnum() == DBONE_VNUM_9 || GetVnum() == DBONE_VNUM_10 || GetVnum() == DBONE_VNUM_11) { m_pOwner->RemoveAffect(AFFECT_DBONE_GOLD); }
	if (GetVnum() == DBONE_VNUM_12 || GetVnum() == DBONE_VNUM_13 || GetVnum() == DBONE_VNUM_14 || GetVnum() == DBONE_VNUM_15 || GetVnum() == DBONE_VNUM_16) { m_pOwner->RemoveAffect(AFFECT_DBONE_DIAMOND); }
	if (GetVnum() == DBONE_VNUM_17 || GetVnum() == DBONE_VNUM_18 || GetVnum() == DBONE_VNUM_19) { m_pOwner->RemoveAffect(AFFECT_DBONE_PLATIN); }
	if (GetVnum() == DBONE_VNUM_20 || GetVnum() == DBONE_VNUM_21 || GetVnum() == DBONE_VNUM_22) { m_pOwner->RemoveAffect(AFFECT_DBONE_EMERALD); }
#endif

#ifdef ENABLE_PASSIVE_SYSTEM
	if (CItemVnumHelper::IsPassive(GetVnum()) && GetSubType() == PASSIVE_JOB)
	{
		m_pOwner->RemoveAffect(AFFECT_PASSIVE_RELIC_STONE_DEF);
		m_pOwner->RemoveAffect(AFFECT_PASSIVE_RELIC_DISMOUNT_SPEED);
	}
#endif

	if (IsDragonSoul())
	{
		DSManager::instance().DeactivateDragonSoul(this);
	}
	else
	{
		ModifyPoints(false);
	}

	StopUniqueExpireEvent();

	if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
		StopTimerBasedOnWearExpireEvent();

	// ACCESSORY_REFINE
	StopAccessorySocketExpireEvent();
	// END_OF_ACCESSORY_REFINE
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	StopAuraBoosterSocketExpireEvent();
#endif

	m_pOwner->BuffOnAttr_RemoveBuffsFromItem(this);

	m_pOwner->SetWear(GetCell() - INVENTORY_MAX_NUM, NULL);

	// Fix
	DWORD dwImmuneFlag = 0;
	LPITEM item = NULL;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (item=m_pOwner->GetWear(i))
		{
			if (item->GetImmuneFlag() != 0)
				SET_BIT(dwImmuneFlag, item->GetImmuneFlag());
			if (item->GetAttributeCount() > 0)
			{
				if (item->HasAttr(APPLY_IMMUNE_STUN))
					SET_BIT(dwImmuneFlag, IMMUNE_STUN);
				if (item->HasAttr(APPLY_IMMUNE_SLOW))
					SET_BIT(dwImmuneFlag, IMMUNE_SLOW);
				if (item->HasAttr(APPLY_IMMUNE_FALL))
					SET_BIT(dwImmuneFlag, IMMUNE_FALL);
			}
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);

	m_pOwner->ComputeBattlePoints();
	m_pOwner->UpdatePacket();

#ifdef ENABLE_BUFFI_SYSTEM
	LPCHARACTER pkBuffiOwner = m_pOwner;
	const int iBuffiCell = GetCell() - INVENTORY_MAX_NUM;
#endif

	m_pOwner = NULL;
	m_wCell = 0;
	m_bEquipped	= false;

#ifdef ENABLE_BUFFI_SYSTEM
	if (CBuffiManager::IsBuffiWearCell(iBuffiCell))
		CBuffiManager::instance().OnWearChanged(pkBuffiOwner, iBuffiCell, false);
#endif

	return true;
}

long CItem::GetValue(DWORD idx)
{
	assert(idx < ITEM_VALUES_MAX_NUM);
	return GetProto()->alValues[idx];
}

void CItem::SetExchanging(bool bOn)
{
	m_bExchanging = bOn;
}

void CItem::Save()
{
	if (m_bSkipSave)
		return;

	ITEM_MANAGER::instance().DelayedSave(this);
}

bool CItem::CreateSocket(BYTE bSlot, BYTE bGold)
{
	assert(bSlot < ITEM_SOCKET_MAX_NUM);

	if (m_alSockets[bSlot] != 0)
	{
		sys_err("Item::CreateSocket : socket already exist %s %d", GetName(), bSlot);
		return false;
	}

	if (bGold)
		m_alSockets[bSlot] = 2;
	else
		m_alSockets[bSlot] = 1;

	UpdatePacket();

	Save();
	return true;
}

void CItem::SetSockets(const int32_t * c_al)
{
	thecore_memcpy(m_alSockets, c_al, sizeof(m_alSockets));
	Save();
}

void CItem::SetSocket(int i, int32_t v, bool bLog)
{
	assert(i < ITEM_SOCKET_MAX_NUM);
	m_alSockets[i] = v;
	UpdatePacket();
	Save();
	if (bLog)
		LogManager::instance().ItemLog(i, v, 0, GetID(), "SET_SOCKET", "", "", GetOriginalVnum());
}

#ifdef ENABLE_GOLD_LIMIT
long long CItem::GetGold()
#else
int CItem::GetGold()
#endif
{
	if (IS_SET(GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (GetProto()->dwGold == 0)
			return GetCount();
		else
			return GetCount() / GetProto()->dwGold;
	}
	else
		return GetProto()->dwGold;
}

#ifdef ENABLE_GOLD_LIMIT
long long CItem::GetShopBuyPrice()
#else
int CItem::GetShopBuyPrice()
#endif
{
	return GetProto()->dwShopBuyPrice;
}

#ifdef ENABLE_AUTO_SELL_SYSTEM
int CItem::GetISellItemPrice() const
{
	int basePrice = const_cast<CItem*>(this)->GetGold();
	if (basePrice > 0)
	{
		return basePrice / 5; 
	}
	return 0;
}
#endif

bool CItem::IsOwnership(LPCHARACTER ch)
{
	if (!m_pkOwnershipEvent)
		return true;

	return m_dwOwnershipPID == ch->GetPlayerID() ? true : false;
}

EVENTFUNC(ownership_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "ownership_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	pkItem->SetOwnershipEvent(NULL);

	TPacketGCItemOwnership p;

	p.bHeader	= HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID	= pkItem->GetVID();
	p.szName[0]	= '\0';

	pkItem->PacketAround(&p, sizeof(p));
	return 0;
}

void CItem::SetOwnershipEvent(LPEVENT pkEvent)
{
	m_pkOwnershipEvent = pkEvent;
}

void CItem::SetOwnership(LPCHARACTER ch, int iSec)
{
	if (!ch)
	{
		if (m_pkOwnershipEvent)
		{
			event_cancel(&m_pkOwnershipEvent);
			m_dwOwnershipPID = 0;

			TPacketGCItemOwnership p;

			p.bHeader	= HEADER_GC_ITEM_OWNERSHIP;
			p.dwVID	= m_dwVID;
			p.szName[0]	= '\0';

			PacketAround(&p, sizeof(p));
		}
		return;
	}

	if (m_pkOwnershipEvent)
		return;

	if (iSec <= 10)
		iSec = 30;

	m_dwOwnershipPID = ch->GetPlayerID();

	item_event_info* info = AllocEventInfo<item_event_info>();
	strlcpy(info->szOwnerName, ch->GetName(), sizeof(info->szOwnerName));
	info->item = this;

	SetOwnershipEvent(event_create(ownership_event, info, PASSES_PER_SEC(iSec)));

	TPacketGCItemOwnership p;

	p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID = m_dwVID;
	strlcpy(p.szName, ch->GetName(), sizeof(p.szName));

	PacketAround(&p, sizeof(p));
}

int CItem::GetSocketCount()
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
	{
		if (GetSocket(i) == 0)
			return i;
	}
	return ITEM_SOCKET_MAX_NUM;
}

bool CItem::AddSocket()
{
	int count = GetSocketCount();
	if (count == ITEM_SOCKET_MAX_NUM)
		return false;
	m_alSockets[count] = 1;
	return true;
}

void CItem::AlterToSocketItem(int iSocketCount)
{
	if (iSocketCount >= ITEM_SOCKET_MAX_NUM)
	{
		sys_log(0, "Invalid Socket Count %d, set to maximum", ITEM_SOCKET_MAX_NUM);
		iSocketCount = ITEM_SOCKET_MAX_NUM;
	}

	for (int i = 0; i < iSocketCount; ++i)
		SetSocket(i, 1);
}

void CItem::AlterToMagicItem()
{
	int idx = GetAttributeSetIndex();

	if (idx < 0)
		return;

	//      Appeariance Second Third
	// Weapon 50        20     5
	// Armor  30        10     2
	// Acc    20        10     1

	int iSecondPct;
	int iThirdPct;

	switch (GetType())
	{
		case ITEM_WEAPON:
			iSecondPct = 20;
			iThirdPct = 5;
			break;

		case ITEM_ARMOR:
		case ITEM_COSTUME:
			if (GetSubType() == ARMOR_BODY)
			{
				iSecondPct = 10;
				iThirdPct = 2;
			}
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			else if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_AURA)
			{
				iSecondPct = 0;
				iThirdPct = 0;
			}
#endif
			else
			{
				iSecondPct = 10;
				iThirdPct = 1;
			}
			break;

		default:
			return;
	}

	PutAttribute(aiItemMagicAttributePercentHigh);

	if (number(1, 100) <= iSecondPct)
		PutAttribute(aiItemMagicAttributePercentLow);

	if (number(1, 100) <= iThirdPct)
		PutAttribute(aiItemMagicAttributePercentLow);
}

DWORD CItem::GetRefineFromVnum()
{
	return ITEM_MANAGER::instance().GetRefineFromVnum(GetVnum());
}

int CItem::GetRefineLevel()
{
	const char* name = GetBaseName();
	char* p = const_cast<char*>(strrchr(name, '+'));

	if (!p)
		return 0;

	int	rtn = 0;
	str_to_number(rtn, p+1);

	const char* locale_name = GetName();
	p = const_cast<char*>(strrchr(locale_name, '+'));

	if (p)
	{
		int	locale_rtn = 0;
		str_to_number(locale_rtn, p+1);
		if (locale_rtn != rtn)
		{
			sys_err("refine_level_based_on_NAME(%d) is not equal to refine_level_based_on_LOCALE_NAME(%d).", rtn, locale_rtn);
		}
	}

	return rtn;
}

bool CItem::IsPolymorphItem()
{
	return GetType() == ITEM_POLYMORPH;
}

EVENTFUNC(unique_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "unique_expire_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetValue(2) == 0)
	{
		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= 1)
		{
			sys_log(0, "UNIQUE_ITEM: expire %s %u", pkItem->GetName(), pkItem->GetID());
			pkItem->SetUniqueExpireEvent(NULL);
#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
			if (pkItem->GetVnum() == DBONE_VNUM_1 || pkItem->GetVnum() == DBONE_VNUM_2 || pkItem->GetVnum() == DBONE_VNUM_3 || pkItem->GetVnum() == DBONE_VNUM_4 || pkItem->GetVnum() == DBONE_VNUM_5) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_BRONZE); }
			if (pkItem->GetVnum() == DBONE_VNUM_6 || pkItem->GetVnum() == DBONE_VNUM_7 || pkItem->GetVnum() == DBONE_VNUM_8) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_SILVER); }
			if (pkItem->GetVnum() == DBONE_VNUM_9 || pkItem->GetVnum() == DBONE_VNUM_10 || pkItem->GetVnum() == DBONE_VNUM_11) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_GOLD); }
			if (pkItem->GetVnum() == DBONE_VNUM_12 || pkItem->GetVnum() == DBONE_VNUM_13 || pkItem->GetVnum() == DBONE_VNUM_14 || pkItem->GetVnum() == DBONE_VNUM_15 || pkItem->GetVnum() == DBONE_VNUM_16) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_DIAMOND); }
			if (pkItem->GetVnum() == DBONE_VNUM_17 || pkItem->GetVnum() == DBONE_VNUM_18 || pkItem->GetVnum() == DBONE_VNUM_19) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_PLATIN); }
			if (pkItem->GetVnum() == DBONE_VNUM_20 || pkItem->GetVnum() == DBONE_VNUM_21 || pkItem->GetVnum() == DBONE_VNUM_22) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_EMERALD); }
#endif
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			pkItem->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - 1);
			return PASSES_PER_SEC(60);
		}
	}
	else
	{
		time_t cur = get_global_time();

		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= cur)
		{
			pkItem->SetUniqueExpireEvent(NULL);
#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
			if (pkItem->GetVnum() == DBONE_VNUM_1 || pkItem->GetVnum() == DBONE_VNUM_2 || pkItem->GetVnum() == DBONE_VNUM_3 || pkItem->GetVnum() == DBONE_VNUM_4 || pkItem->GetVnum() == DBONE_VNUM_5) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_BRONZE); }
			if (pkItem->GetVnum() == DBONE_VNUM_6 || pkItem->GetVnum() == DBONE_VNUM_7 || pkItem->GetVnum() == DBONE_VNUM_8) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_SILVER); }
			if (pkItem->GetVnum() == DBONE_VNUM_9 || pkItem->GetVnum() == DBONE_VNUM_10 || pkItem->GetVnum() == DBONE_VNUM_11) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_GOLD); }
			if (pkItem->GetVnum() == DBONE_VNUM_12 || pkItem->GetVnum() == DBONE_VNUM_13 || pkItem->GetVnum() == DBONE_VNUM_14 || pkItem->GetVnum() == DBONE_VNUM_15 || pkItem->GetVnum() == DBONE_VNUM_16) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_DIAMOND); }
			if (pkItem->GetVnum() == DBONE_VNUM_17 || pkItem->GetVnum() == DBONE_VNUM_18 || pkItem->GetVnum() == DBONE_VNUM_19) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_PLATIN); }
			if (pkItem->GetVnum() == DBONE_VNUM_20 || pkItem->GetVnum() == DBONE_VNUM_21 || pkItem->GetVnum() == DBONE_VNUM_22) { pkItem->GetOwner()->RemoveAffect(AFFECT_DBONE_EMERALD); }
#endif
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur < 600)
				return PASSES_PER_SEC(pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur);
			else
				return PASSES_PER_SEC(600);
		}
	}
}

EVENTFUNC(timer_based_on_wear_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "expire_event <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;
	int remain_time = pkItem->GetSocket(ITEM_SOCKET_REMAIN_SEC) - processing_time/passes_per_sec;
	if (remain_time <= 0)
	{
		sys_log(0, "ITEM EXPIRED : expired %s %u", pkItem->GetName(), pkItem->GetID());
		pkItem->SetTimerBasedOnWearExpireEvent(NULL);
		pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, 0);

		if (pkItem->IsDragonSoul())
		{
			DSManager::instance().DeactivateDragonSoul(pkItem);
		}
		else
		{
			ITEM_MANAGER::instance().RemoveItem(pkItem, "TIMER_BASED_ON_WEAR_EXPIRE");
		}

		return 0;
	}

	pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	return PASSES_PER_SEC (MIN (60, remain_time));
}

void CItem::SetUniqueExpireEvent(LPEVENT pkEvent)
{
	m_pkUniqueExpireEvent = pkEvent;
}

void CItem::SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent)
{
	m_pkTimerBasedOnWearExpireEvent = pkEvent;
}

EVENTFUNC(real_time_expire_event)
{
	const item_vid_event_info* info = reinterpret_cast<const item_vid_event_info*>(event->info);

	if (NULL == info)
		return 0;

	const LPITEM item = ITEM_MANAGER::instance().FindByVID( info->item_vid );

	if (NULL == item)
		return 0;

	const time_t current = get_global_time();

	if (current > item->GetSocket(0))
	{
		switch (item->GetVnum())
		{
			if(item->IsNewMountItem())
			{
				if (item->GetSocket(2) != 0)
					item->ClearMountAttributeAndAffect();
			}
			break;
		}

#ifdef ENABLE_GROWTH_PET_SYSTEM
		if (item->GetType() == ITEM_GROWTH_PET && item->GetSubType() == PET_UPBRINGING)
		{
			if (item->GetOwner()->GetActiveGrowthPet())
			{
				if (item->GetOwner()->GetActiveGrowthPet()->GetPetID() == item->GetSocket(2))
					item->GetOwner()->GetActiveGrowthPet()->Unsummon();
			}
		}
#endif

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_RT_REMOVE))
		{
			item->StopRealTimeExpireEvent();
			return 0;
		}

		ITEM_MANAGER::instance().RemoveItem(item, "REAL_TIME_EXPIRE");

		return 0;
	}

	return PASSES_PER_SEC(1);
}

void CItem::StartRealTimeExpireEvent()
{
	if (m_pkRealTimeExpireEvent)
		return;

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType || LIMIT_REAL_TIME_START_FIRST_USE == GetProto()->aLimits[i].bType)
		{
			item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
			info->item_vid = GetVID();

			m_pkRealTimeExpireEvent = event_create( real_time_expire_event, info, PASSES_PER_SEC(1));

			sys_log(0, "REAL_TIME_EXPIRE: StartRealTimeExpireEvent");

			return;
		}
	}
}

void CItem::StopRealTimeExpireEvent()
{
	if (!m_pkRealTimeExpireEvent)
		return;

	event_cancel(&m_pkRealTimeExpireEvent);
	m_pkRealTimeExpireEvent = NULL;
}

bool CItem::IsRealTimeItem()
{
	if(!GetProto())
		return false;

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType)
			return true;
	}
	return false;
}

#ifdef ENABLE_PITTY_REFINE
int CItem::GetLimitPittyRefine()
{
	if(!GetProto())
		return 0;

	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		if (LIMIT_PITTY_REFINE == GetProto()->aLimits[i].bType)
			return GetProto()->aLimits[i].lValue;
	}
	return 0;
}
#endif

void CItem::StartUniqueExpireEvent()
{
	if (GetType() != ITEM_UNIQUE)
		return;

	if (m_pkUniqueExpireEvent)
		return;

	//For fixed-term items, time-based items do not work.
	if (IsRealTimeItem())
		return;

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		m_pOwner->ShowAlignment(false);

	int iSec = GetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME);

	if (iSec == 0)
		iSec = 60;
	else
		iSec = MIN(iSec, 60);

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, 0);

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetUniqueExpireEvent(event_create(unique_expire_event, info, PASSES_PER_SEC(iSec)));
}

// Pay after time
// see timer_based_on_wear_expire_event description
void CItem::StartTimerBasedOnWearExpireEvent()
{
	if (m_pkTimerBasedOnWearExpireEvent)
		return;

	//In case of a fixed-term item, the part-time item does not work.
	if (IsRealTimeItem())
		return;

	if (-1 == GetProto()->cLimitTimerBasedOnWearIndex)
		return;

	int iSec = GetSocket(0);

	// To cut off the remaining time in minutes...
	if (0 != iSec)
	{
		iSec %= 60;
		if (0 == iSec)
			iSec = 60;
	}

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetTimerBasedOnWearExpireEvent(event_create(timer_based_on_wear_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopUniqueExpireEvent()
{
	if (!m_pkUniqueExpireEvent)
		return;

	if (GetValue(2) != 0) // Items other than game-timed cannot stop UniqueExpireEvent.
		return;

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		m_pOwner->ShowAlignment(true);

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, event_time(m_pkUniqueExpireEvent) / passes_per_sec);
	event_cancel(&m_pkUniqueExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::StopTimerBasedOnWearExpireEvent()
{
	if (!m_pkTimerBasedOnWearExpireEvent)
		return;

	int remain_time = GetSocket(ITEM_SOCKET_REMAIN_SEC) - event_processing_time(m_pkTimerBasedOnWearExpireEvent) / passes_per_sec;

	SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::ApplyAddon(int iAddonType)
{
	CItemAddonManager::instance().ApplyAddonTo(iAddonType, this);
}

int CItem::GetSpecialGroup() const
{
	return ITEM_MANAGER::instance().GetSpecialGroupFromItem(GetVnum()); 
}

bool CItem::IsAccessoryForSocket()
{
	return (m_pProto->bType == ITEM_ARMOR && (m_pProto->bSubType == ARMOR_WRIST || m_pProto->bSubType == ARMOR_NECK || m_pProto->bSubType == ARMOR_EAR || m_pProto->bSubType == ARMOR_BELT));
}

void CItem::SetAccessorySocketGrade(int iGrade) 
{
	SetSocket(0, MINMAX(0, iGrade, GetAccessorySocketMaxGrade())); 

	int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

	SetAccessorySocketDownGradeTime(iDownTime);
}

void CItem::SetAccessorySocketMaxGrade(int iMaxGrade) 
{
	SetSocket(1, MINMAX(0, iMaxGrade, ITEM_ACCESSORY_SOCKET_MAX_NUM)); 
}

void CItem::SetAccessorySocketDownGradeTime(DWORD time) 
{
	SetSocket(2, time);

	if (test_server && GetOwner())
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Time left until the removal of the socket of %s: %d"), GetLocaleName(), time);
}

EVENTFUNC(accessory_socket_expire_event)
{
	item_vid_event_info* info = dynamic_cast<item_vid_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "accessory_socket_expire_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);

	if (item->GetAccessorySocketDownGradeTime() <= 1)
	{
degrade:
		item->SetAccessorySocketExpireEvent(NULL);
		item->AccessorySocketDegrade();
		return 0;
	}
	else
	{
		int iTime = item->GetAccessorySocketDownGradeTime() - 60;

		if (iTime <= 1)
			goto degrade;

		item->SetAccessorySocketDownGradeTime(iTime);

		if (iTime > 60)
			return PASSES_PER_SEC(60);
		else
			return PASSES_PER_SEC(iTime);
	}
}

void CItem::StartAccessorySocketExpireEvent()
{
	if (!IsAccessoryForSocket())
		return;

	if (m_pkAccessorySocketExpireEvent)
		return;

	if (GetAccessorySocketMaxGrade() == 0)
		return;

	if (GetAccessorySocketGrade() == 0)
		return;

	int iSec = GetAccessorySocketDownGradeTime();
	SetAccessorySocketExpireEvent(NULL);

	if (iSec <= 1)
		iSec = 5;
	else
		iSec = MIN(iSec, 60);

	item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
	info->item_vid = GetVID();

	SetAccessorySocketExpireEvent(event_create(accessory_socket_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopAccessorySocketExpireEvent()
{
	if (!m_pkAccessorySocketExpireEvent)
		return;

	if (!IsAccessoryForSocket())
		return;

	int new_time = GetAccessorySocketDownGradeTime() - (60 - event_time(m_pkAccessorySocketExpireEvent) / passes_per_sec);

	event_cancel(&m_pkAccessorySocketExpireEvent);

	if (new_time <= 1)
	{
		AccessorySocketDegrade();
	}
	else
	{
		SetAccessorySocketDownGradeTime(new_time);
	}
}

bool CItem::IsRideItem()
{
	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_RIDE == GetSubType())
		return true;

	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == GetSubType())
		return true;

#ifdef ENABLE_MOUNT_SYSTEM
	if (ITEM_MOUNT == GetType())
		return true;
#endif
	return false;
}

bool CItem::IsRamadanRing()
{
	if (GetVnum() == UNIQUE_ITEM_RAMADAN_RING)
		return true;
	return false;
}

void CItem::ClearMountAttributeAndAffect()
{
	LPCHARACTER ch = GetOwner();

	ch->RemoveAffect(AFFECT_MOUNT);
	ch->RemoveAffect(AFFECT_MOUNT_BONUS);

	ch->MountVnum(0);

	ch->PointChange(POINT_ST, 0);
	ch->PointChange(POINT_DX, 0);
	ch->PointChange(POINT_HT, 0);
	ch->PointChange(POINT_IQ, 0);
}

bool CItem::IsNewMountItem()
{
	switch(GetVnum())
	{
		case 76000: case 76001: case 76002: case 76003:
		case 76004: case 76005: case 76006: case 76007:
		case 76008: case 76009: case 76010: case 76011:
		case 76012: case 76013: case 76014:
			return true;
	}
	return false;
}

void CItem::SetAccessorySocketExpireEvent(LPEVENT pkEvent)
{
	m_pkAccessorySocketExpireEvent = pkEvent;
}

void CItem::AccessorySocketDegrade()
{
	if (GetAccessorySocketGrade() > 0)
	{
		LPCHARACTER ch = GetOwner();

		if (ch)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A gem socketed in the %s has vanished."), GetLocaleName());

		ModifyPoints(false);
		SetAccessorySocketGrade(GetAccessorySocketGrade()-1);
		ModifyPoints(true);

		int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

		if (test_server)
			iDownTime /= 60;

		SetAccessorySocketDownGradeTime(iDownTime);

		if (iDownTime)
			StartAccessorySocketExpireEvent();
	}
}

static const bool CanPutIntoRing(LPITEM ring, LPITEM item)
{
	const DWORD vnum = item->GetVnum();
	return false;
}

bool CItem::CanPutInto(LPITEM item)
{
	if (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_BELT)
		return this->GetSubType() == USE_PUT_INTO_BELT_SOCKET;

	else if (item->GetType() == ITEM_RING)
		return CanPutIntoRing(item, this);

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	else if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_AURA)
		return this->GetSubType() == USE_PUT_INTO_AURA_SOCKET;
#endif

	else if (item->GetType() != ITEM_ARMOR)
		return false;

	DWORD vnum = item->GetVnum();

	struct JewelAccessoryInfo
	{
		DWORD jewel;
		DWORD wrist;
		DWORD neck;
		DWORD ear;
	};

	const static JewelAccessoryInfo infos[] = {
		{ 50634, 14420, 16220, 17220 },
		{ 50635, 14500, 16500, 17500 },
		{ 50636, 14520, 16520, 17520 },
		{ 50637, 14540, 16540, 17540 },
		{ 50638, 14560, 16560, 17560 },
	};

	DWORD item_type = (item->GetVnum() / 10) * 10;
	for (int i = 0; i < sizeof(infos) / sizeof(infos[0]); i++)
	{
		const JewelAccessoryInfo& info = infos[i];
		switch(item->GetSubType())
		{
		case ARMOR_WRIST:
			if (info.wrist == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_NECK:
			if (info.neck == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_EAR:
			if (info.ear == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		}
	}
	if (item->GetSubType() == ARMOR_WRIST)
		vnum -= 14000;
	else if (item->GetSubType() == ARMOR_NECK)
		vnum -= 16000;
	else if (item->GetSubType() == ARMOR_EAR)
		vnum -= 17000;
	else
		return false;

	DWORD type = vnum / 20;

	if (type < 0 || type > 11)
	{
		type = (vnum - 170) / 20;

		if (50623 + type != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16210 && item->GetVnum() <= 16219)
	{
		if (50625 != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16230 && item->GetVnum() <= 16239)
	{
		if (50626 != GetVnum())
			return false;
		else
			return true;
	}

	return 50623 + type == GetVnum();
}

bool CItem::CheckItemUseLevel(int nLevel)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			if (this->m_pProto->aLimits[i].lValue > nLevel)
			{
				return false;
			}
		}
	}

	return true;
}

long CItem::FindApplyValue(WORD bApplyType)
{
	if (m_pProto == NULL)
		return 0;

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (m_pProto->aApplies[i].bType == bApplyType)
			return m_pProto->aApplies[i].lValue;
	}

	return 0;
}

void CItem::CopySocketTo(LPITEM pItem)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		pItem->m_alSockets[i] = m_alSockets[i];
	}
}

int CItem::GetAccessorySocketGrade()
{
   	return MINMAX(0, GetSocket(0), GetAccessorySocketMaxGrade());
}

int CItem::GetAccessorySocketMaxGrade()
{
   	return MINMAX(0, GetSocket(1), ITEM_ACCESSORY_SOCKET_MAX_NUM);
}

int CItem::GetAccessorySocketDownGradeTime()
{
	return MINMAX(0, GetSocket(2), aiAccessorySocketDegradeTime[GetAccessorySocketGrade()]);
}

void CItem::AttrLog()
{
	const char * pszIP = NULL;

	if (GetOwner() && GetOwner()->GetDesc())
		pszIP = GetOwner()->GetDesc()->GetHostName();

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (m_alSockets[i])
		{
			LogManager::instance().ItemLog(i, m_alSockets[i], 0, GetID(), "INFO_SOCKET", "", pszIP ? pszIP : "", GetOriginalVnum());
		}
	}

	for (int i = 0; i<ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		int	type	= m_aAttr[i].bType;
		int value	= m_aAttr[i].sValue;

		if (type)
			LogManager::instance().ItemLog(i, type, value, GetID(), "INFO_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum());
	}
}

int CItem::GetLevelLimit()
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			return this->m_pProto->aLimits[i].lValue;
		}
	}
	return 0;
}

#ifdef ENABLE_RENEWAL_AFFECT
bool CItem::IsItemBlend()
{
	return GetType() == ITEM_BLEND;
}

bool CItem::IsItemBlendPlus()
{
	if (GetVnum() >= 50827 && GetVnum() <= 50832)
		return true;
	return false;
}

bool CItem::IsItemDragonGodPlus()
{
	if (GetVnum() >= 72731 && GetVnum() <= 72736)
		return true;
	return false;
}

bool CItem::IsItemDragonGod()
{
	switch(GetVnum())
	{
		case 71044: case 71045: case 72046: case 72047:
		case 72048: case 39024: case 39025: case 72025:
		case 72026: case 72027: case 76035: case 76036:
			return true;
	}

	if (GetVnum() >= 39017 && GetVnum() <= 39020)
		return true;

	if (GetVnum() >= 71027 && GetVnum() <= 71030)
		return true;

	if (GetVnum() >= 72031 && GetVnum() <= 72042)
		return true;

	return false;
}
#endif

bool CItem::OnAfterCreatedItem()
{
	// If an item is used at least once, the time is deducted even if it is not being used after that
	if (-1 != this->GetProto()->cLimitRealTimeFirstUseIndex)
	{
		// Since the number of times an item has been used is recorded in Socket1, an item that has been used even once starts a timer.
		if (0 != GetSocket(1))
		{
			StartRealTimeExpireEvent();
		}
	}

#ifdef ENABLE_RENEWAL_AFFECT
	if (IsItemBlendPlus() || IsItemDragonGodPlus())
	{
		if (0 != GetSocket(1))
		{
			Lock(true);
		}
	}
#endif

	return true;
}

bool CItem::IsDragonSoul()
{
	return GetType() == ITEM_DS;
}

int CItem::GiveMoreTime_Per(float fPercent)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		int remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		int given_time = fPercent * duration / 100;
		if (remain_sec == duration)
			return false;
		if ((given_time + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, given_time + remain_sec);
			return given_time;
		}
	}
	else
		return 0;
}

int CItem::GiveMoreTime_Fix(DWORD dwTime)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		int remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		if (remain_sec == duration)
			return false;
		if ((dwTime + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, dwTime + remain_sec);
			return dwTime;
		}
	}
	else
		return 0;
}


int	CItem::GetDuration()
{
	if(!GetProto())
		return -1;

	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType)
			return GetProto()->aLimits[i].lValue;
	}

	if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
		return GetProto()->aLimits[GetProto()->cLimitTimerBasedOnWearIndex].lValue;	

	return -1;
}

bool CItem::IsSameSpecialGroup(const LPITEM item) const
{
	if (this->GetVnum() == item->GetVnum())
		return true;

	if (GetSpecialGroup() && (item->GetSpecialGroup() == GetSpecialGroup()))
		return true;

	return false;
}

#ifdef ENABLE_RENEWAL_SWITCHBOT
bool CItem::CanSwitchItem()
{
	if (GetType() == ITEM_COSTUME)
		return false;

	if (GetVnum() >= 11901 && GetVnum() <= 11904 || GetVnum() >= 11911 && GetVnum() <= 11914)
		return false;

	if (GetVnum() == 50201 || GetVnum() == 50202)
		return false;

	return true;
}
#endif

#ifdef ENABLE_MOUNT_SYSTEM
bool CItem::IsMountItem()
{
	if (GetType() == ITEM_MOUNT)
		return true;

	return false;
}
#endif

#ifdef ENABLE_PET_SYSTEM
bool CItem::IsPetItem()
{
	return (GetType() == ITEM_PET);
}
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
const char* CItem::GetLocaleName(BYTE locale)
{
	if (locale == LOCALE_MAX_NUM)
	{
		if (m_pOwner != NULL)
			locale = m_pOwner->GetLanguage();
		else
		{
			if (GetDesc())
				locale = GetDesc()->GetLanguage();
			else
				locale = LOCALE_DEFAULT;
		}
	}

	return m_pProto ? LC_LOCALE_ITEM(GetVnum(), locale) : NULL;
}

const char* CItem::GetName()
{
	return m_pProto ? LC_LOCALE_ITEM(GetVnum(), LOCALE_DEFAULT) : NULL;
}
#endif

#ifdef ENABLE_MOUNT_PET_SKIN
bool CItem::IsCostumeMountSkin()
{
	return (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT);
}

bool CItem::IsCostumePetSkin()
{
	return (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_PET);
}
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
bool CItem::IsSkillBook()
{
	return (GetVnum() == 50300);
}

bool CItem::IsUpgradeItem()
{
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	return (GetType() == ITEM_MATERIAL || GetType() == ITEM_RESOURCE);
#else
	return (GetType() == ITEM_MATERIAL);
#endif
}

bool CItem::IsStone()
{
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	return (GetType() == ITEM_METIN || GetSubType() == USE_PUT_INTO_AURA_SOCKET);
#else
	return (GetType() == ITEM_METIN);
#endif
}

bool CItem::IsGiftBox()
{
#ifdef ENABLE_NEW_ITEM_TYPE_GACHA
	return (GetType() == ITEM_GACHA || GetType() == ITEM_GIFTBOX);
#else
	return (GetType() == ITEM_GIFTBOX);
#endif
}

bool CItem::IsChanger()
{
	return (GetType() == ITEM_USE && (GetSubType() == USE_CHANGE_ATTRIBUTE || GetSubType() == USE_ADD_ATTRIBUTE || GetSubType() == USE_ADD_ATTRIBUTE2 || GetSubType() == USE_CHANGE_COSTUME_ATTR || GetSubType() == USE_RESET_COSTUME_ATTR));
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
bool CItem::IsAuraBoosterForSocket()
{
	if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA)
		return this->GetSocket(ITEM_SOCKET_AURA_BOOST) == 0;

	return false;
}

EVENTFUNC(aura_boost_socket_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("aura_boost_socket_expire_event> <Factor> Null pointer");
		return 0;
	}

	LPITEM pkAura = info->item;
	const long c_lBoosterSocket = pkAura->GetSocket(ITEM_SOCKET_AURA_BOOST);
	const long c_lBoosterIndex = c_lBoosterSocket / 100000000;
	if (c_lBoosterIndex <= ITEM_AURA_BOOST_ERASER || c_lBoosterIndex >= ITEM_AURA_BOOST_MAX)
		return 0;

	DWORD dwBoosterItemVnum = ITEM_AURA_BOOST_ITEM_VNUM_BASE + c_lBoosterIndex;
	TItemTable* pBoosterProto = ITEM_MANAGER::instance().GetTable(dwBoosterItemVnum);
	if (!pBoosterProto)
		return 0;

	if (pBoosterProto->alValues[ITEM_AURA_BOOST_UNLIMITED_VALUE] == 1)
		return 0;

	long lBoostTimeSec = (c_lBoosterSocket % 100000000) - processing_time / passes_per_sec;
	if (lBoostTimeSec <= 0)
	{
		sys_log(0, "AURA BOOST EXPIRED : expired %s %lu", pkAura->GetName(), pkAura->GetID());
		pkAura->SetAuraBoosterSocketExpireEvent(NULL);
		pkAura->ModifyPoints(false);
		pkAura->SetSocket(ITEM_SOCKET_AURA_BOOST, 0);
		pkAura->ModifyPoints(true);
		if (pkAura->GetOwner())
		{
			pkAura->GetOwner()->ComputeBattlePoints();
			pkAura->GetOwner()->UpdatePacket();
		}

		LogManager::instance().ItemLog(pkAura->GetOwner(), pkAura, "AURA_BOOST_EXPIRED", pkAura->GetName());

		return 0;
	}

	pkAura->SetSocket(ITEM_SOCKET_AURA_BOOST, c_lBoosterIndex * 100000000 + lBoostTimeSec);
	return PASSES_PER_SEC(MIN(60, lBoostTimeSec));
}

void CItem::StartAuraBoosterSocketExpireEvent()
{
	if (!(m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA))
		return;

	if (GetSocket(ITEM_SOCKET_AURA_BOOST) == 0)
		return;

	if (m_pkAuraBoostSocketExpireEvent)
		return;

	const long c_lBoosterSocket = GetSocket(ITEM_SOCKET_AURA_BOOST);
	const long c_lBoosterIndex = c_lBoosterSocket / 100000000;
	if (c_lBoosterIndex <= ITEM_AURA_BOOST_ERASER || c_lBoosterIndex >= ITEM_AURA_BOOST_MAX)
		return;

	DWORD dwBoosterItemVnum = ITEM_AURA_BOOST_ITEM_VNUM_BASE + c_lBoosterIndex;
	TItemTable* pBoosterProto = ITEM_MANAGER::instance().GetTable(dwBoosterItemVnum);
	if (!pBoosterProto)
		return;

	if (pBoosterProto->alValues[ITEM_AURA_BOOST_UNLIMITED_VALUE] == 1)
		return;

	long lBoostTimeSec = c_lBoosterSocket % 100000000;
	if (0 != lBoostTimeSec)
	{
		lBoostTimeSec %= 60;
		if (0 == lBoostTimeSec)
			lBoostTimeSec = 60;
	}

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetAuraBoosterSocketExpireEvent(event_create(aura_boost_socket_expire_event, info, PASSES_PER_SEC(lBoostTimeSec)));
}

void CItem::StopAuraBoosterSocketExpireEvent()
{
	if (!m_pkAuraBoostSocketExpireEvent)
		return;

	const long c_lBoosterSocket = GetSocket(ITEM_SOCKET_AURA_BOOST);
	const long c_lBoosterIndex = c_lBoosterSocket / 100000000;
	if (c_lBoosterIndex <= ITEM_AURA_BOOST_ERASER || c_lBoosterIndex >= ITEM_AURA_BOOST_MAX)
		return;

	DWORD dwBoosterItemVnum = ITEM_AURA_BOOST_ITEM_VNUM_BASE + c_lBoosterIndex;
	TItemTable* pBoosterProto = ITEM_MANAGER::instance().GetTable(dwBoosterItemVnum);
	if (!pBoosterProto)
		return;

	long lBoostTimeSec = (c_lBoosterSocket % 100000000) - event_processing_time(m_pkAuraBoostSocketExpireEvent) / passes_per_sec;
	SetSocket(ITEM_SOCKET_AURA_BOOST, c_lBoosterIndex * 100000000 + lBoostTimeSec);
	event_cancel(&m_pkAuraBoostSocketExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::SetAuraBoosterSocketExpireEvent(LPEVENT pkEvent)
{
	m_pkAuraBoostSocketExpireEvent = pkEvent;
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
void CItem::SetTransmutationVnum(DWORD blVnum)
{
	m_dwTransmutationVnum = blVnum;
	Save();
}

DWORD CItem::GetTransmutationVnum() const
{
	return m_dwTransmutationVnum;
}
#endif

#ifdef ENABLE_SET_ITEM
void CItem::SetSetValue(DWORD dwSetValue)
{
	m_dwSetValue = dwSetValue;
	Save();
}

DWORD CItem::GetSetValue() const
{
	return m_dwSetValue;
}
#endif
