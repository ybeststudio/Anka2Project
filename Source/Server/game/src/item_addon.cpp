#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "item.h"
#include "item_addon.h"

CItemAddonManager::CItemAddonManager()
{
}

CItemAddonManager::~CItemAddonManager()
{
}

void CItemAddonManager::ApplyAddonTo(int iAddonType, LPITEM pItem)
{
	if (!pItem)
	{
		sys_err("ITEM pointer null");
		return;
	}

	int iSkillBonus = MINMAX(-30, (int) (gauss_random(0, 5) + 0.5f), 30);
	int iNormalHitBonus = 0;
	if (abs(iSkillBonus) <= 20)
		iNormalHitBonus = -2 * iSkillBonus + abs(number(-8, 8) + number(-8, 8)) + number(1, 4);
	else
		iNormalHitBonus = -2 * iSkillBonus + number(1, 5);

	pItem->RemoveAttributeType(APPLY_SKILL_DAMAGE_BONUS);
#ifdef ENABLE_AVG_PVM_DISABLE
	pItem->RemoveAttributeType(APPLY_ATTBONUS_MEDI_PVM);
#else
	pItem->RemoveAttributeType(APPLY_NORMAL_HIT_DAMAGE_BONUS);
#endif
#ifdef ENABLE_AVG_PVM_DISABLE
	pItem->AddAttribute(APPLY_ATTBONUS_MEDI_PVM, iNormalHitBonus);
#else
	pItem->AddAttribute(APPLY_NORMAL_HIT_DAMAGE_BONUS, iNormalHitBonus);
#endif
	pItem->AddAttribute(APPLY_SKILL_DAMAGE_BONUS, iSkillBonus);
}
