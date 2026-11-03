#include "stdafx.h"
#include "constants.h"
#include "log.h"
#include "item.h"
#include "char.h"
#include "desc.h"
#include "item_manager.h"
#ifdef ENABLE_NEWSTUFF
#include "config.h"
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
#include "AchievementTitle.h"
#endif

#ifndef ENABLE_SWITCHBOT
const int MAX_NORM_ATTR_NUM = ITEM_MANAGER::MAX_NORM_ATTR_NUM;
const int MAX_RARE_ATTR_NUM = ITEM_MANAGER::MAX_RARE_ATTR_NUM;
#endif

int CItem::GetAttributeSetIndex()
{
	if (GetType() == ITEM_WEAPON)
	{
		if (GetSubType() == WEAPON_ARROW)
			return -1;
		
#ifdef ENABLE_QUIVER_SYSTEM
		if (GetSubType() == WEAPON_QUIVER)
			return -1;
#endif

		return ATTRIBUTE_SET_WEAPON;
	}

	if (GetType() == ITEM_ARMOR)
	{
		switch (GetSubType())
		{
			case ARMOR_BODY:
				return ATTRIBUTE_SET_BODY;

			case ARMOR_WRIST:
				return ATTRIBUTE_SET_WRIST;

			case ARMOR_FOOTS:
				return ATTRIBUTE_SET_FOOTS;

			case ARMOR_NECK:
				return ATTRIBUTE_SET_NECK;

			case ARMOR_HEAD:
				return ATTRIBUTE_SET_HEAD;

			case ARMOR_SHIELD:
				return ATTRIBUTE_SET_SHIELD;

			case ARMOR_EAR:
				return ATTRIBUTE_SET_EAR;

#ifdef ENABLE_BELT_ATTRIBUTES
			case ARMOR_GLOVE:
				return ATTRIBUTE_SET_GLOVE;
#endif
#ifdef __PENDANT__
			case ARMOR_PENDANT_FIRE:
			case ARMOR_PENDANT_ICE:
			case ARMOR_PENDANT_THUNDER:
			case ARMOR_PENDANT_WIND:
			case ARMOR_PENDANT_DARK:
			case ARMOR_PENDANT_EARTH:
			case ARMOR_PENDANT_SOUL:
				return ATTRIBUTE_SET_PENDANT;
#endif
		}
	}
#ifdef ENABLE_BELT_ATTRIBUTES
	else if (GetType() == ITEM_BELT)
	{
		return ATTRIBUTE_SET_BELT;
	}
#endif

	if (GetVnum() >= 53001 && GetVnum() <= 53999)
		return ATTRIBUTE_SET_PET;

	else if (GetType() == ITEM_COSTUME)
	{
		switch (GetSubType())
		{
			case COSTUME_BODY:
#ifdef ENABLE_ITEM_ATTR_COSTUME
				return ATTRIBUTE_SET_COSTUME_BODY;
#else
				return ATTRIBUTE_SET_BODY;
#endif

			case COSTUME_HAIR:
#ifdef ENABLE_ITEM_ATTR_COSTUME
				return ATTRIBUTE_SET_COSTUME_HAIR;
#else
				return ATTRIBUTE_SET_HEAD;
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			case COSTUME_MOUNT:
				break;
#endif
#ifdef ENABLE_NEW_MOUNT_SYSTEM
			case COSTUME_MOUNT:
				break;
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
			case COSTUME_WEAPON:
#ifdef ENABLE_ITEM_ATTR_COSTUME
				return ATTRIBUTE_SET_COSTUME_WEAPON;
#else
				return ATTRIBUTE_SET_WEAPON;
#endif
#endif
#ifdef ENABLE_STOLE_COSTUME
			case COSTUME_STOLE:
				return ATTRIBUTE_SET_STOLE;
#endif

		}
	}

	return -1;
}

bool CItem::HasAttr(BYTE bApply)
{
	// for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	// 	if (m_pProto->aApplies[i].bType == bApply)
	// 		return true;

	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
		if (GetAttributeType(i) == bApply)
			return true;

	return false;
}

bool CItem::HasRareAttr(BYTE bApply)
{
	for (int i = 0; i < MAX_RARE_ATTR_NUM; ++i)
		if (GetAttributeType(i + 5) == bApply)
			return true;

	return false;
}

void CItem::AddAttribute(BYTE bApply, short sValue)
{
	if (HasAttr(bApply))
		return;

	int i = GetAttributeCount();

	if (i >= MAX_NORM_ATTR_NUM)
		sys_err("item attribute overflow!");
	else
	{
		if (sValue)
			SetAttribute(i, bApply, sValue);
	}
}

void CItem::AddAttr(BYTE bApply, BYTE bLevel)
{
	if (HasAttr(bApply))
		return;

	if (bLevel <= 0)
		return;

	int i = GetAttributeCount();

	if (i == MAX_NORM_ATTR_NUM)
		sys_err("item attribute overflow!");
	else
	{
		const TItemAttrTable & r = g_map_itemAttr[bApply];
		int32_t lVal = r.lValues[MIN(4, bLevel - 1)];

		if (lVal)
			SetAttribute(i, bApply, lVal);
	}
}

void CItem::PutAttributeWithLevel(BYTE bLevel)
{
	int iAttributeSet = GetAttributeSetIndex();
	if (iAttributeSet < 0)
		return;

	if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
		return;

	std::vector<int> avail;

	int total = 0;

	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_itemAttr[i];

		if (r.bMaxLevelBySet[iAttributeSet] && !HasAttr(i))
		{
			avail.emplace_back(i);
			total += r.dwProb;
		}
	}

	unsigned int prob = number(1, total);
	int attr_idx = APPLY_NONE;

	for (DWORD i = 0; i < avail.size(); ++i)
	{
		const TItemAttrTable & r = g_map_itemAttr[avail[i]];

		if (prob <= r.dwProb)
		{
			attr_idx = avail[i];
			break;
		}

		prob -= r.dwProb;
	}

	if (!attr_idx)
	{
		sys_err("Cannot put item attribute %d %d", iAttributeSet, bLevel);
		return;
	}

	const TItemAttrTable & r = g_map_itemAttr[attr_idx];

	if (bLevel > r.bMaxLevelBySet[iAttributeSet])
		bLevel = r.bMaxLevelBySet[iAttributeSet];

	AddAttr(attr_idx, bLevel);
}

void CItem::PutAttribute(const int * aiAttrPercentTable)
{
	int iAttrLevelPercent = number(1, 100);
	int i;

	for (i = 0; i < ITEM_ATTRIBUTE_MAX_LEVEL; ++i)
	{
		if (iAttrLevelPercent <= aiAttrPercentTable[i])
			break;

		iAttrLevelPercent -= aiAttrPercentTable[i];
	}

	PutAttributeWithLevel(i + 1);
}

void CItem::ChangeAttribute(const int* aiChangeProb)
{
	int iAttributeCount = GetAttributeCount();
	ClearAttribute();
	if (iAttributeCount == 0)
		return;
	TItemTable const * pProto = GetProto();
	if (pProto && pProto->sAddonType)
	{
		ApplyAddon(pProto->sAddonType);
	}
	static const int tmpChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		0, 10, 40, 35, 15,
	};
	
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	// Increases chance for 4th and 5th rolls (indices 3 and 4)
	static const int tmpChangeProbTitle8[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		0, 5, 20, 30, 45,
	};
	
	LPCHARACTER owner = GetOwner();
	bool useTitle8Bonus = (owner && (owner->GetTitleAchievement() == 8 || owner->GetTitleAchievementPremium() == 8));
#endif
	
	for (int i = GetAttributeCount(); i < iAttributeCount; ++i)
	{
		if (aiChangeProb == NULL)
		{
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			if (useTitle8Bonus)
			{
				PutAttribute(tmpChangeProbTitle8);
			}
			else
#endif
			{
				PutAttribute(tmpChangeProb);
			}
		}
		else
		{
			PutAttribute(aiChangeProb);
		}
	}
}

#ifdef ENABLE_BELT_ATTRIBUTES
void CItem::AddBeltAttribute()
{
	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		40, 50, 10, 0, 0
	};

	if (GetAttributeCount() < MAX_NORM_ATTR_NUM)
		PutBeltAttribute(aiItemAddAttributePercent);
}
void CItem::PutBeltAttributeWithLevel(BYTE bLevel)
{
	int iAttributeSet = GetAttributeSetIndex();
	if (iAttributeSet < 0)
		return;

	if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
		return;

	std::vector<int> avail;

	int total = 0;

	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_beltAttr[i];

		if (r.bMaxLevelBySet[iAttributeSet] && !HasAttr(i))
		{
			avail.emplace_back(i);
			total += r.dwProb;
		}
	}

	unsigned int prob = number(1, total);
	int attr_idx = APPLY_NONE;

	for (DWORD i = 0; i < avail.size(); ++i)
	{
		const TItemAttrTable & r = g_map_beltAttr[avail[i]];

		if (prob <= r.dwProb)
		{
			attr_idx = avail[i];
			break;
		}

		prob -= r.dwProb;
	}

	if (!attr_idx)
	{
		sys_err("Cannot put belt attribute %d %d", iAttributeSet, bLevel);
		return;
	}

	const TItemAttrTable & r = g_map_beltAttr[attr_idx];

	if (bLevel > r.bMaxLevelBySet[iAttributeSet])
		bLevel = r.bMaxLevelBySet[iAttributeSet];

	AddBeltAttr(attr_idx, bLevel);
}

void CItem::PutBeltAttribute(const int * aiAttrPercentTable)
{
	int iAttrLevelPercent = number(1, 100);
	int i;

	for (i = 0; i < ITEM_ATTRIBUTE_MAX_LEVEL; ++i)
	{
		if (iAttrLevelPercent <= aiAttrPercentTable[i])
			break;

		iAttrLevelPercent -= aiAttrPercentTable[i];
	}

	PutBeltAttributeWithLevel(i + 1);
}

void CItem::AddBeltAttr(BYTE bApply, BYTE bLevel)
{
	if (HasAttr(bApply))
		return;

	if (bLevel <= 0)
		return;

	int i = GetAttributeCount();

	if (i == MAX_NORM_ATTR_NUM)
		sys_err("belt attribute overflow!");
	else
	{
		const TItemAttrTable & r = g_map_beltAttr[bApply];
		int32_t lVal = r.lValues[MIN(4, bLevel - 1)];

		if (lVal)
			SetAttribute(i, bApply, lVal);
	}
}
void CItem::ChangeBeltAttribute(const int* aiChangeProb)
{
	int iAttributeCount = GetAttributeCount();

	ClearAttribute();

	if (iAttributeCount == 0)
		return;

	TItemTable const * pProto = GetProto();

	if (pProto && pProto->sAddonType)
	{
		ApplyAddon(pProto->sAddonType);
	}

	static const int tmpChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		0, 10, 40, 35, 15,
	};

	for (int i = GetAttributeCount(); i < iAttributeCount; ++i)
	{
		if (aiChangeProb == NULL)
		{
			PutBeltAttribute(tmpChangeProb);
		}
		else
		{
			PutBeltAttribute(aiChangeProb);
		}
	}
}
#endif

#ifdef ENABLE_GLOVE_ATTRIBUTES
bool CItem::HasGloveAttr(BYTE bApply)
{
	// for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	// 	if (m_pProto->aApplies[i].bType == bApply)
	// 		return true;

	for (int i = 3; i < MAX_NORM_ATTR_NUM; ++i)
		if (GetAttributeType(i) == bApply)
			return true;

	return false;
}

void CItem::AddGloveAttribute()
{
	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		50,
		50,
		50,
		50,
		50,
	};

	if (GetAttributeCount() < MAX_NORM_ATTR_NUM)
		PutGloveAttribute(aiItemAddAttributePercent);
}
void CItem::PutGloveAttributeWithLevel(BYTE bLevel)
{
	int iAttributeSet = GetAttributeSetIndex();
	if (iAttributeSet < 0)
		return;

	if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
		return;

	std::vector<int> avail;

	int total = 0;

	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_gloveAttr[i];

		if (r.bMaxLevelBySet[iAttributeSet] && !HasGloveAttr(i))
		{
			avail.emplace_back(i);
			total += r.dwProb;
		}
	}

	unsigned int prob = number(1, total);
	int attr_idx = APPLY_NONE;

	for (DWORD i = 0; i < avail.size(); ++i)
	{
		const TItemAttrTable & r = g_map_gloveAttr[avail[i]];

		if (prob <= r.dwProb)
		{
			attr_idx = avail[i];
			break;
		}

		prob -= r.dwProb;
	}

	if (!attr_idx)
	{
		sys_err("Cannot put glove attribute %d %d", iAttributeSet, bLevel);
		return;
	}

	const TItemAttrTable & r = g_map_gloveAttr[attr_idx];

	if (bLevel > r.bMaxLevelBySet[iAttributeSet])
		bLevel = r.bMaxLevelBySet[iAttributeSet];

	AddGloveAttr(attr_idx, bLevel);
}

void CItem::PutGloveAttribute(const int * aiAttrPercentTable)
{
	int iAttrLevelPercent = number(1, 100);
	int i;

	for (i = 0; i < ITEM_ATTRIBUTE_MAX_LEVEL; ++i)
	{
		if (iAttrLevelPercent <= aiAttrPercentTable[i])
			break;

		iAttrLevelPercent -= aiAttrPercentTable[i];
	}

	PutGloveAttributeWithLevel(i + 1);
}

void CItem::AddGloveAttr(BYTE bApply, BYTE bLevel)
{
	if (HasGloveAttr(bApply))
		return;

	if (bLevel <= 0)
		return;

	int i = GetAttributeCount();

	if (i == MAX_NORM_ATTR_NUM)
		sys_err("glove attribute overflow!");
	else
	{
		const TItemAttrTable & r = g_map_gloveAttr[bApply];
		int32_t lVal = r.lValues[MIN(4, bLevel - 1)];

		if (lVal)
			SetAttribute(i, bApply, lVal);
	}
}
void CItem::ChangeGloveAttribute(const int* aiChangeProb)
{
	int iAttributeCount = GetAttributeCount();

	//ClearAttribute();

	if (iAttributeCount == 0)
		return;

	TItemTable const * pProto = GetProto();

	if (pProto && pProto->sAddonType)
	{
		ApplyAddon(pProto->sAddonType);
	}

	static const int tmpChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		20, 20, 20, 20, 20,
	};

    for (int i = 3; i < iAttributeCount; ++i)
    {
        ClearGloveAttribute(i);
    }

    for (int i = 3; i < iAttributeCount; ++i)
    {
        if (aiChangeProb == NULL)
        {
            PutGloveAttribute(tmpChangeProb);
        }
        else
        {
            PutGloveAttribute(aiChangeProb);
        }
    }
}

void CItem::ClearGloveAttribute(int e)
{
	for (int i = e; i < MAX_NORM_ATTR_NUM; ++i)
	{
		m_aAttr[i].bType = 0;
		m_aAttr[i].sValue = 0;
	}
}
#endif

#ifdef ENABLE_TALISMAN_ATTRIBUTES
bool CItem::HasTalismanAttr(BYTE bApply)
{
	// for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	// 	if (m_pProto->aApplies[i].bType == bApply)
	// 		return true;

	for (int i = 0; i < 4; ++i)
		if (GetAttributeType(i) == bApply)
			return true;

	return false;
}

void CItem::AddTalismanAttribute()
{
	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		50,
		50,
		50,
		50,
		50,
	};

	if (GetTalismanAttributeCount() < 5)
		PutTalismanAttribute(aiItemAddAttributePercent);
}
void CItem::PutTalismanAttributeWithLevel(BYTE bLevel)
{
	int iAttributeSet = GetAttributeSetIndex();
	if (iAttributeSet < 0)
		return;

	if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
		return;

	std::vector<int> avail;

	int total = 0;

	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_talismanAttr[i];

		if (r.bMaxLevelBySet[iAttributeSet] && !HasTalismanAttr(i))
		{
			avail.emplace_back(i);
			total += r.dwProb;
		}
	}

	unsigned int prob = number(1, total);
	int attr_idx = APPLY_NONE;

	for (DWORD i = 0; i < avail.size(); ++i)
	{
		const TItemAttrTable & r = g_map_talismanAttr[avail[i]];

		if (prob <= r.dwProb)
		{
			attr_idx = avail[i];
			break;
		}

		prob -= r.dwProb;
	}

	if (!attr_idx)
	{
		sys_err("Cannot put talisman attribute %d %d", iAttributeSet, bLevel);
		return;
	}

	const TItemAttrTable & r = g_map_talismanAttr[attr_idx];

	if (bLevel > r.bMaxLevelBySet[iAttributeSet])
		bLevel = r.bMaxLevelBySet[iAttributeSet];

	AddTalismanAttr(attr_idx, bLevel);
}

void CItem::PutTalismanAttribute(const int * aiAttrPercentTable)
{
	int iAttrLevelPercent = number(1, 100);
	int i;

	for (i = 0; i < 5; ++i)
	{
		if (iAttrLevelPercent <= aiAttrPercentTable[i])
			break;

		iAttrLevelPercent -= aiAttrPercentTable[i];
	}

	PutTalismanAttributeWithLevel(i + 1);
}

void CItem::AddTalismanAttr(BYTE bApply, BYTE bLevel)
{
	if (HasTalismanAttr(bApply))
		return;

	if (bLevel <= 0)
		return;

	int i = GetTalismanAttributeCount();

	if (i == 5)
		sys_err("Talisman attribute overflow!");
	else
	{
		const TItemAttrTable & r = g_map_talismanAttr[bApply];
		int32_t lVal = r.lValues[MIN(4, bLevel - 1)];

		if (lVal)
			SetAttributeTalisman(i, bApply, lVal);
	}
}
void CItem::ChangeTalismanAttribute(const int* aiChangeProb)
{
	int iAttributeCount = GetTalismanAttributeCount();

	//ClearAttribute();

	if (iAttributeCount == 0)
		return;

	TItemTable const * pProto = GetProto();

	if (pProto && pProto->sAddonType)
	{
		ApplyAddon(pProto->sAddonType);
	}

	static const int tmpChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		20, 20, 20, 20, 20,
	};

    for (int i = 0; i < iAttributeCount; ++i)
    {
        ClearTalismanAttribute(i);
    }

    for (int i = 0; i < iAttributeCount; ++i)
    {
        if (aiChangeProb == NULL)
        {
            PutTalismanAttribute(tmpChangeProb);
        }
        else
        {
            PutTalismanAttribute(aiChangeProb);
        }
    }
}

void CItem::ClearTalismanAttribute(int e)
{
	for (int i = e; i < 5; ++i)
	{
		m_aAttr[i].bType = 0;
		m_aAttr[i].sValue = 0;
	}
}
#endif

bool CItem::HasTalismanRareAttr(BYTE bApply)
{
    // Check slots 6-7 which are used for talisman rare attributes (7th and 8th bonuses)
    // Normal talisman bonuses use slots 0-5, so we use 6-7 for rare bonuses
    for (int i = 5; i < 7; ++i)
        if (GetAttributeType(i) == bApply)
            return true;

    return false;
}

int CItem::GetTalismanRareAttrCount()
{
    int count = 0;
    // Check slots 6-7 for talisman rare attributes (7th and 8th bonuses)
    for (int i = 5; i < 7; ++i)
    {
        if (GetAttributeType(i) != 0)
            count++;
    }
    return count;
}

bool CItem::AddTalismanRareAttribute()
{
    int count = GetTalismanRareAttrCount();
    if (count >= 2) // Maximum 2 rare attributes for talisman (7th and 8th)
        return false;

    static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
    {
        0, 0, 0, 0, 100  // Always maximum level for talisman rare attributes
    };

    PutTalismanRareAttribute(aiItemAddAttributePercent);
    return true;
}

void CItem::PutTalismanRareAttribute(const int* aiAttrPercentTable)
{
    int iAttrLevelPercent = number(1, 100);
    int i;

    for (i = 0; i < ITEM_ATTRIBUTE_MAX_LEVEL; ++i)
    {
        if (iAttrLevelPercent <= aiAttrPercentTable[i])
            break;

        iAttrLevelPercent -= aiAttrPercentTable[i];
    }

    PutTalismanRareAttributeWithLevel(i + 1);
}

void CItem::PutTalismanRareAttributeWithLevel(BYTE bLevel)
{
    // For talismans, we use a specific attribute set (ATTRIBUTE_SET_PET or create ATTRIBUTE_SET_TALISMAN_RARE)
    int iAttributeSet = ATTRIBUTE_SET_PENDANT; // You might want to create ATTRIBUTE_SET_TALISMAN_RARE
    
    if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
        return;

    std::vector<int> avail;
    int total = 0;

    // Use g_map_talismanAttrRare instead of g_map_itemRare for different bonus pool
    for (int i = 0; i < MAX_APPLY_NUM; ++i)
    {
        const TItemAttrTable& r = g_map_talismanAttrRare[i]; // This needs to be defined in item_manager

        if (r.bMaxLevelBySet[iAttributeSet] && !HasTalismanRareAttr(i))
        {
            avail.emplace_back(i);
            total += r.dwProb;
        }
    }

    if (avail.empty() || total == 0)
    {
        sys_err("No available talisman rare attributes");
        return;
    }

    unsigned int prob = number(1, total);
    int attr_idx = APPLY_NONE;

    for (DWORD i = 0; i < avail.size(); ++i)
    {
        const TItemAttrTable& r = g_map_talismanAttrRare[avail[i]];

        if (prob <= r.dwProb)
        {
            attr_idx = avail[i];
            break;
        }

        prob -= r.dwProb;
    }

    if (!attr_idx)
    {
        sys_err("Cannot put talisman rare attribute %d %d", iAttributeSet, bLevel);
        return;
    }

    const TItemAttrTable& r = g_map_talismanAttrRare[attr_idx];

    if (bLevel > r.bMaxLevelBySet[iAttributeSet])
        bLevel = r.bMaxLevelBySet[iAttributeSet];

    AddTalismanRareAttr(attr_idx, bLevel);
}

void CItem::AddTalismanRareAttr(BYTE bApply, BYTE bLevel)
{
    if (HasTalismanRareAttr(bApply))
        return;

    if (bLevel <= 0)
        return;

    int count = GetTalismanRareAttrCount();
    if (count >= 2)
    {
        sys_err("talisman rare attribute overflow!");
        return;
    }

    // Use slots 6-7 for talisman rare attributes (7th and 8th bonuses)
    int i = 5 + count;

    const TItemAttrTable& r = g_map_talismanAttrRare[bApply];
    int32_t lVal = r.lValues[MIN(4, bLevel - 1)];

    if (lVal)
    {
        SetForceAttribute(i, bApply, lVal);
        
        const char* pszIP = NULL;
        if (GetOwner() && GetOwner()->GetDesc())
            pszIP = GetOwner()->GetDesc()->GetHostName();

        LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(i, bApply, lVal, GetID(), "SET_TALISMAN_RARE", "", pszIP ? pszIP : "", GetOriginalVnum()));
    }
}

bool CItem::ChangeTalismanRareAttribute()
{
    int count = GetTalismanRareAttrCount();
    if (count == 0)
        return false;

    // Clear existing talisman rare attributes
    ClearTalismanRareAttribute();

    if (GetOwner() && GetOwner()->GetDesc())
        LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(GetOwner(), this, "CHANGE_TALISMAN_RARE", ""))
    else
        LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(0, 0, 0, GetID(), "CHANGE_TALISMAN_RARE", "", "", GetOriginalVnum()))

    // Add new rare attributes
    for (int i = 0; i < count; ++i)
    {
        AddTalismanRareAttribute();
    }

    return true;
}

void CItem::ClearTalismanRareAttribute()
{
    // Clear only the talisman rare attribute slots (slots 6-7)
    for (int i = 5; i < 7; ++i)
    {
        m_aAttr[i].bType = 0;
        m_aAttr[i].sValue = 0;
    }
    UpdatePacket();
    Save();
}


void CItem::AddAttribute()
{
	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		40, 50, 10, 0, 0
	};

	if (GetAttributeCount() < MAX_NORM_ATTR_NUM)
		PutAttribute(aiItemAddAttributePercent);
}

void CItem::ClearAttribute()
{
	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
		m_aAttr[i].bType = 0;
		m_aAttr[i].sValue = 0;
	}
}

void CItem::ClearAttribute2()
{
	for (int i = 3; i < 6; ++i)
	{
		m_aAttr[i].bType = 0;
		m_aAttr[i].sValue = 0;
	}
}

void CItem::ClearAllAttribute()
{
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		m_aAttr[i].bType = 0;
		m_aAttr[i].sValue = 0;
	}
}

int CItem::GetAttributeCount()
{
	int i;

	for (i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
		if (GetAttributeType(i) == 0)
			break;
	}

	return i;
}

int CItem::GetTalismanAttributeCount()
{
	int i;

	for (i = 0; i < 5; ++i)
	{
		if (GetAttributeType(i) == 0)
			break;
	}

	return i;
}

int CItem::FindAttribute(BYTE bType)
{
	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
		if (GetAttributeType(i) == bType)
			return i;
	}

	return -1;
}

bool CItem::RemoveAttributeAt(int index)
{
	if (GetAttributeCount() <= index)
		return false;

	for (int i = index; i < MAX_NORM_ATTR_NUM - 1; ++i)
	{
		SetAttribute(i, GetAttributeType(i + 1), GetAttributeValue(i + 1));
	}

	SetAttribute(MAX_NORM_ATTR_NUM - 1, APPLY_NONE, 0);
	return true;
}

bool CItem::RemoveAttributeType(BYTE bType)
{
	int index = FindAttribute(bType);
	return index != -1 && RemoveAttributeType(index);
}

void CItem::SetAttributes(const TPlayerItemAttribute* c_pAttribute)
{
	thecore_memcpy(m_aAttr, c_pAttribute, sizeof(m_aAttr));
	Save();
}

void CItem::SetAttribute(int i, BYTE bType, short sValue)
{
	assert(i < MAX_NORM_ATTR_NUM);

	m_aAttr[i].bType = bType;
	m_aAttr[i].sValue = sValue;
	UpdatePacket();
	Save();

	if (bType)
	{
		const char * pszIP = NULL;

		if (GetOwner() && GetOwner()->GetDesc())
			pszIP = GetOwner()->GetDesc()->GetHostName();

		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(i, bType, sValue, GetID(), "SET_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum()));
	}
}

void CItem::SetAttributeTalisman(int i, BYTE bType, short sValue)
{
	assert(i < 5);

	m_aAttr[i].bType = bType;
	m_aAttr[i].sValue = sValue;
	UpdatePacket();
	Save();

	if (bType)
	{
		const char * pszIP = NULL;

		if (GetOwner() && GetOwner()->GetDesc())
			pszIP = GetOwner()->GetDesc()->GetHostName();

		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(i, bType, sValue, GetID(), "SET_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum()));
	}
}

void CItem::SetForceAttribute(int i, BYTE bType, short sValue, bool sendUpdate)
{
	assert(i < ITEM_ATTRIBUTE_MAX_NUM);

	m_aAttr[i].bType = bType;
	m_aAttr[i].sValue = sValue;
	if (sendUpdate)
	{
		UpdatePacket();
		Save();
	}

	if (bType)
	{
		const char * pszIP = NULL;

		if (GetOwner() && GetOwner()->GetDesc())
			pszIP = GetOwner()->GetDesc()->GetHostName();

		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(i, bType, sValue, GetID(), "SET_FORCE_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum()));
	}
}

void CItem::CopyAttributeTo(LPITEM pItem)
{
	pItem->SetAttributes(m_aAttr);
}

int CItem::GetRareAttrCount()
{
	int ret = 0;

	for (DWORD dwIdx = ITEM_ATTRIBUTE_RARE_START; dwIdx < ITEM_ATTRIBUTE_RARE_END; dwIdx++)
	{
		if (m_aAttr[dwIdx].bType != 0)
			ret++;
	}

	return ret;
}

bool CItem::ChangeRareAttribute()
{
	if (GetRareAttrCount() == 0)
		return false;

	int cnt = GetRareAttrCount();

	for (int i = 0; i < cnt; ++i)
	{
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].bType = 0;
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].sValue = 0;
	}

	if (GetOwner() && GetOwner()->GetDesc())
		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(GetOwner(), this, "SET_RARE_CHANGE", ""))
	else
		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(0, 0, 0, GetID(), "SET_RARE_CHANGE", "", "", GetOriginalVnum()))

	for (int i = 0; i < cnt; ++i)
	{
		AddRareAttribute();
	}

	return true;
}

bool CItem::ChangeRareAttribute2()
{
	if (GetRareAttrCount() == 0)
		return false;

	int cnt = GetRareAttrCount();

	for (int i = 0; i < cnt; ++i)
	{
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].bType = 0;
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].sValue = 0;
	}

	if (GetOwner() && GetOwner()->GetDesc())
		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(GetOwner(), this, "SET_RARE_CHANGE", ""))
	else
		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(0, 0, 0, GetID(), "SET_RARE_CHANGE", "", "", GetOriginalVnum()))

	for (int i = 0; i < cnt; ++i)
	{
		AddRareAttribute2();
	}

	return true;
}

bool CItem::ChangeRareAttributeEvent()
{
	if (GetRareAttrCount() == 0)
		return false;

	int cnt = GetRareAttrCount();

	for (int i = 0; i < cnt; ++i)
	{
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].bType = 0;
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].sValue = 0;
	}

	if (GetOwner() && GetOwner()->GetDesc())
		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(GetOwner(), this, "SET_RARE_CHANGE", ""))
	else
		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(0, 0, 0, GetID(), "SET_RARE_CHANGE", "", "", GetOriginalVnum()))

	for (int i = 0; i < cnt; ++i)
	{
		AddRareAttributeEvent();
	}

	return true;
}

bool CItem::AddRareAttributeEvent(const int * aiAttrPercentTable)
{
	int count = GetRareAttrCount();
	if (count >= ITEM_ATTRIBUTE_RARE_NUM)
		return false;

	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		0, 0, 0, 0, 100
	};
	if (aiAttrPercentTable == NULL)
		aiAttrPercentTable = aiItemAddAttributePercent;

	if (GetRareAttrCount() < MAX_RARE_ATTR_NUM)
		PutRareAttribute(aiAttrPercentTable);

	return true;
}

bool CItem::AddRareAttribute()
{
	int count = GetRareAttrCount();

	if (count >= ITEM_ATTRIBUTE_RARE_NUM)
		return false;

	int pos = count + ITEM_ATTRIBUTE_RARE_START;
	TPlayerItemAttribute & attr = m_aAttr[pos];

	int nAttrSet = GetAttributeSetIndex();
	std::vector<int> avail;

	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_itemRare[i];

		if (r.dwApplyIndex != 0 && r.bMaxLevelBySet[nAttrSet] > 0 && HasRareAttr(i) != true)
		{
			avail.emplace_back(i);
		}
	}

	const TItemAttrTable& r = g_map_itemRare[avail[number(0, avail.size() - 1)]];
	int nAttrLevel = 5;

	if (nAttrLevel > r.bMaxLevelBySet[nAttrSet])
		nAttrLevel = r.bMaxLevelBySet[nAttrSet];

	attr.bType = r.dwApplyIndex;
	attr.sValue = r.lValues[nAttrLevel - 1];

	UpdatePacket();

	Save();

	const char * pszIP = NULL;

	if (GetOwner() && GetOwner()->GetDesc())
		pszIP = GetOwner()->GetDesc()->GetHostName();

	LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(pos, attr.bType, attr.sValue, GetID(), "SET_RARE", "", pszIP ? pszIP : "", GetOriginalVnum()));
	return true;
}

bool CItem::AddRareAttribute2(const int * aiAttrPercentTable)
{
	int count = GetRareAttrCount();
	if (count >= ITEM_ATTRIBUTE_RARE_NUM)
		return false;

	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		40, 50, 10, 0, 0
	};
	if (aiAttrPercentTable == NULL)
		aiAttrPercentTable = aiItemAddAttributePercent;

	if (GetRareAttrCount() < MAX_RARE_ATTR_NUM)
		PutRareAttribute(aiAttrPercentTable);

	return true;
}

void CItem::PutRareAttribute(const int * aiAttrPercentTable)
{
	int iAttrLevelPercent = number(1, 100);
	int i;

	for (i = 0; i < ITEM_ATTRIBUTE_MAX_LEVEL; ++i)
	{
		if (iAttrLevelPercent <= aiAttrPercentTable[i])
			break;

		iAttrLevelPercent -= aiAttrPercentTable[i];
	}

	PutRareAttributeWithLevel(i + 1);
}

void CItem::PutRareAttributeWithLevel(BYTE bLevel)
{
	int iAttributeSet = GetAttributeSetIndex();
	if (iAttributeSet < 0)
		return;

	if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
		return;

	std::vector<int> avail;

	int total = 0;

	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_itemRare[i];

		if (r.bMaxLevelBySet[iAttributeSet] && !HasRareAttr(i))
		{
			avail.emplace_back(i);
			total += r.dwProb;
		}
	}

	unsigned int prob = number(1, total);
	int attr_idx = APPLY_NONE;

	for (DWORD i = 0; i < avail.size(); ++i)
	{
		const TItemAttrTable & r = g_map_itemRare[avail[i]];

		if (prob <= r.dwProb)
		{
			attr_idx = avail[i];
			break;
		}

		prob -= r.dwProb;
	}

	if (!attr_idx)
	{
		sys_err("Cannot put item rare attribute %d %d", iAttributeSet, bLevel);
		return;
	}

	const TItemAttrTable & r = g_map_itemRare[attr_idx];

	if (bLevel > r.bMaxLevelBySet[iAttributeSet])
		bLevel = r.bMaxLevelBySet[iAttributeSet];

	AddRareAttr(attr_idx, bLevel);
}

void CItem::AddRareAttr(BYTE bApply, BYTE bLevel)
{
	if (HasRareAttr(bApply))
		return;

	if (bLevel <= 0)
		return;

	int i = ITEM_ATTRIBUTE_RARE_START + GetRareAttrCount();

	if (i == ITEM_ATTRIBUTE_RARE_END)
		sys_err("item rare attribute overflow!");
	else
	{
		const TItemAttrTable & r = g_map_itemRare[bApply];
		int32_t lVal = r.lValues[MIN(4, bLevel - 1)];

		if (lVal)
			SetForceAttribute(i, bApply, lVal);
	}
}

#if defined(__ITEM_APPLY_RANDOM__)
void CItem::SetRandomApply(BYTE bIndex, BYTE bType, short sValue, BYTE bPath)
{
	assert(bIndex < ITEM_APPLY_MAX_NUM);

	m_aApplyRandom[bIndex].bType = bType;
	m_aApplyRandom[bIndex].sValue = sValue;
	m_aApplyRandom[bIndex].bPath = bPath;
	UpdatePacket();
	Save();
}

void CItem::SetForceRandomApply(BYTE bIndex, BYTE bType, short sValue, BYTE bPath)
{
	assert(bIndex < ITEM_APPLY_MAX_NUM);

	m_aApplyRandom[bIndex].bType = bType;
	m_aApplyRandom[bIndex].sValue = sValue;
	m_aApplyRandom[bIndex].bPath = bPath;
	UpdatePacket();
	Save();
}

void CItem::SetRandomApplies(const TPlayerItemAttribute* c_pApplyRandom)
{
	thecore_memcpy(m_aApplyRandom, c_pApplyRandom, sizeof(m_aApplyRandom));
	Save();
}

TPlayerItemAttribute* CItem::GetNextRandomApplies()
{
	int iRefineLevel = GetRefineLevel();
	const TItemTable* c_pItemProto = GetProto();
	if (c_pItemProto == nullptr)
		return nullptr;

	if (iRefineLevel > ITEM_REFINE_MAX_LEVEL)
		iRefineLevel = ITEM_REFINE_MAX_LEVEL;
	else
		++iRefineLevel;

	TPlayerItemAttribute* ApplyRandomTable = m_aApplyRandom;
	for (BYTE bApplySlot = 0; bApplySlot < ITEM_APPLY_MAX_NUM; ++bApplySlot)
	{
		BYTE bApplyRandomType = c_pItemProto->aApplies[bApplySlot].bType;
		int32_t lApplyRandomValue = c_pItemProto->aApplies[bApplySlot].lValue;
		if (bApplyRandomType == APPLY_RANDOM)
		{
			ApplyRandomTable[bApplySlot].bType = m_aApplyRandom[bApplySlot].bType;
			ApplyRandomTable[bApplySlot].sValue = ITEM_MANAGER::instance().GetNextApplyRandomValue(lApplyRandomValue, iRefineLevel, m_aApplyRandom[bApplySlot].bPath, m_aApplyRandom[bApplySlot].bType);
			ApplyRandomTable[bApplySlot].bPath = m_aApplyRandom[bApplySlot].bPath;
		}
	}
	return ApplyRandomTable;
}
#endif


bool CItem::isLocked() const {
    if (m_isLocked)
        return true;

    return false;
}
