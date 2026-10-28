#include "stdafx.h"

#ifdef ENABLE_RIDING_EXTENDED
#	include "char.h"
#	include "desc.h"
#	include "mount_up_grade.h"
#	include <algorithm>

CMountUpGrade::CMountUpGrade() = default;
CMountUpGrade::~CMountUpGrade() = default;

	/* Open Window */
void CMountUpGrade::OpenMountUpGrade(LPCHARACTER ch) const
{
	if (!ch)
	{
		sys_err("CMountUpGrade::OpenMountUpGrade - Unknown identifier)");
		return;
	}

	const uint8_t m_level = ch->GetHorseLevel();
	const uint8_t isFail = ch->IsMountUpGradeFail();
	const uint32_t curValue = ch->GetMountUpGradeExp();

	/* Send to Client */
	Send(ch, EMountUpGradeGCSubheaderType::SUBHEADER_GC_MOUNT_UP_GRADE_OPEN, m_level, isFail, curValue);
}

void CMountUpGrade::Send(LPCHARACTER ch, const uint8_t iSubHeader, const uint8_t m_level, const uint8_t isFail, const uint32_t curValue) const
{
	if (!ch)
	{
		sys_err("CMountUpGrade::Send - Unknown identifier)");
		return;
	}

	if (iSubHeader > SUBHEADER_GC_MOUNT_UP_GRADE_REFRESH)
	{
		sys_err("CMountUpGrade::Send - Unknown iSubHeader (Name: %s) - (iSubHeader: %d)", ch->GetName(), iSubHeader);
		return;
	}

	auto* d = ch->GetDesc();
	if (!d) return;

	TPacketGCMountUpGrade p {
		static_cast<uint8_t>(HEADER_GC_MOUNT_UP_GRADE),
		static_cast<uint8_t>(iSubHeader),
		static_cast<uint8_t>(m_level),
		static_cast<uint8_t>(isFail),
		static_cast<uint32_t>(curValue)
	};

	d->Packet(&p, sizeof(p));
}

void CMountUpGrade::Chat(LPCHARACTER ch, const uint8_t type, const uint16_t value) const
{
	if (!ch)
	{
		sys_err("CMountUpGrade::Chat - Unknown identifier)");
		return;
	}

	auto* d = ch->GetDesc();
	if (!d) return;

	TPacketGCMountUpGradeChat p {
		static_cast<uint8_t>(HEADER_GC_MOUNT_UP_GRADE_CHAT),
		static_cast<uint8_t>(type),
		static_cast<uint16_t>(value),
	};

	d->Packet(&p, sizeof(p));
}

bool CMountUpGrade::SetExp(LPCHARACTER ch) const
{
	if (!ch)
	{
		sys_err("CMountUpGrade::SetExp - Unknown identifier)");
		return false;
	}

	if (!Common(ch))
		return false;

	/* No Chat */
	if (ch->CountSpecifyItem(EMountUpGradeItem::HORSE_FEED_ITEM_ID) < EMountUpGradeItem::HORSE_FEED_EXP_COUNT)
		return false;

	const uint8_t m_level = ch->GetHorseLevel();
	const uint8_t isFail = ch->IsMountUpGradeFail();
	const uint32_t current_exp = ch->GetMountUpGradeExp();
	const uint32_t max_exp = mount_up_grade_exp_table[m_level + 1];

	if (current_exp >= max_exp)
		return false;

	const uint8_t random_exp = number(static_cast<uint8_t>(EMountUpGradeRandExp::MIN), static_cast<uint8_t>(EMountUpGradeRandExp::MAX));
	const uint16_t item_count = ch->CountSpecifyItem(EMountUpGradeItem::HORSE_FEED_ITEM_ID);
	const uint32_t remaining_exp_to_max = max_exp - current_exp;
	uint16_t used_item_count = (remaining_exp_to_max + random_exp - 1) / random_exp;
	used_item_count = std::min(used_item_count, item_count);
	const uint32_t new_exp = std::min(current_exp + used_item_count * random_exp, max_exp);

	ch->SetMountUpGradeExp(new_exp);

	ch->RemoveSpecifyItem(EMountUpGradeItem::HORSE_FEED_ITEM_ID, used_item_count);

	Chat(ch, EMountUpGradeChatType::CHAT_TYPE_EXP_HORSE_FEED, used_item_count);

	/* Send to Client */
	Send(ch, EMountUpGradeGCSubheaderType::SUBHEADER_GC_MOUNT_UP_GRADE_REFRESH, m_level, isFail, new_exp);

	return true;
}

bool CMountUpGrade::Common(LPCHARACTER ch) const
{
	/* No Chat */
	if (ch->GetHorseLevel() >= EMisc::HORSE_MAX_LEVEL)
		return false;

	if (ch->IsRiding())
	{
		Chat(ch, EMountUpGradeChatType::CHAT_TYPE_BANN_WHILE_MOUNTING, EMountUpGradeChatValue::CHAT_TYPE_NO_VALUE);
		return false;
	}

	return true;
}

bool CMountUpGrade::SetLevel(LPCHARACTER ch) const
{
	if (!ch)
	{
		sys_err("CMountUpGrade::SetLevel - Unknown identifier)");
		return false;
	}

	if (!Common(ch))
		return false;

	const uint8_t m_level = ch->GetHorseLevel();
	const uint32_t curValue = ch->GetMountUpGradeExp();
	const uint32_t maxValue = mount_up_grade_exp_table[m_level + 1];
	const uint32_t yangCost = mount_up_grade_price_table[m_level + 1];

	/* No Chat */
	if (curValue < maxValue)
		return false;

	/* Check Yang or item cost. */
	if (ch->CountSpecifyItem(EMountUpGradeItem::HORSE_FEED_ITEM_ID) < EMountUpGradeItem::HORSE_FEED_LEVEL_COUNT || ch->GetGold() < yangCost)
	{
		Chat(ch, EMountUpGradeChatType::CHAT_TYPE_LEVEL_UP_YANG_OR_FEED_NOT_ENOUGH, EMountUpGradeChatValue::CHAT_TYPE_NO_VALUE);
		return false;
	}

	/* Fail On (1) */
	if (ch->IsMountUpGradeFail() > EMountUpGradeFailType::MOUNT_UP_GRADE_FAIL_OFF)
	{
		ch->SetMountUpGradeFail(EMountUpGradeReset::RESET);
		Update(ch, m_level);
	}

	const uint8_t fail = number(static_cast<uint8_t>(EMountUpGradeRandFail::MIN), static_cast<uint8_t>(EMountUpGradeRandFail::MAX));

	if (ch->GetMountUpGradeExp() >= maxValue)
		ch->SetMountUpGradeFail(fail);

	/* Fail Off (0) */
	if (ch->IsMountUpGradeFail() < EMountUpGradeFailType::MOUNT_UP_GRADE_FAIL_ON)
	{
		Update(ch, m_level);
	}

	// Yang and item costs are always charged.
	ch->PointChange(POINT_GOLD, -yangCost);
	ch->RemoveSpecifyItem(EMountUpGradeItem::HORSE_FEED_ITEM_ID, EMountUpGradeItem::HORSE_FEED_LEVEL_COUNT);

	/* Fail On (1) */
	if (ch->IsMountUpGradeFail() > EMountUpGradeFailType::MOUNT_UP_GRADE_FAIL_OFF)
	{
		/* Fail Chat */
		Chat(ch, EMountUpGradeChatType::CHAT_TYPE_LEVEL_UP_PERCENT_FAIL, EMountUpGradeChatValue::CHAT_TYPE_NO_VALUE);

		/* Send to Client */
		Send(ch, EMountUpGradeGCSubheaderType::SUBHEADER_GC_MOUNT_UP_GRADE_REFRESH,
			m_level, EMountUpGradeFailType::MOUNT_UP_GRADE_FAIL_ON, curValue);
	}
	/* Fail Off (0) */
	else
	{
		/* Success Chat */
		Chat(ch, EMountUpGradeChatType::CHAT_TYPE_LEVEL_UP_PERCENT_SUCCESSFUL, ch->GetHorseLevel());

		/* Send to Client */
		Send(ch, EMountUpGradeGCSubheaderType::SUBHEADER_GC_MOUNT_UP_GRADE_REFRESH,
			ch->GetHorseLevel(), EMountUpGradeFailType::MOUNT_UP_GRADE_FAIL_OFF,
			ch->GetMountUpGradeExp());
	}

	return true;
}

void CMountUpGrade::Update(LPCHARACTER ch, const uint8_t level) const
{
	if (!ch)
	{
		sys_err("CMountUpGrade::Update - Unknown identifier)");
		return;
	}

	ch->SetMountUpGradeExp(EMountUpGradeReset::RESET);
	ch->SetHorseLevel(level + 1);
	ch->ComputePoints();
	ch->SkillLevelPacket();
}
#endif
