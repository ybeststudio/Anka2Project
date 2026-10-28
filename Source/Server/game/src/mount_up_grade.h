#pragma once

#include "../../common/service.h"

#ifdef ENABLE_RIDING_EXTENDED
#	include "../../common/length.h"

class CMountUpGrade : public singleton<CMountUpGrade>
{
	public:
		CMountUpGrade();
		~CMountUpGrade();

		void OpenMountUpGrade(LPCHARACTER ch) const;

		bool SetExp(LPCHARACTER ch) const;
		bool SetLevel(LPCHARACTER ch) const;

		bool Common(LPCHARACTER ch) const;

		void Chat(LPCHARACTER ch, const uint8_t type, const uint16_t value) const;
		void Update(LPCHARACTER ch, const uint8_t level) const;

		void Send(LPCHARACTER ch, const uint8_t iSubHeader, const uint8_t m_level, const uint8_t isFail, const uint32_t curValue) const;

	protected:
		enum EMountUpGradeChatType : uint8_t
		{
			CHAT_TYPE_BANN_WHILE_MOUNTING,
			CHAT_TYPE_LEVEL_UP_YANG_OR_FEED_NOT_ENOUGH,
			CHAT_TYPE_LEVEL_UP_GEM_NOT_ENOUGH,
			CHAT_TYPE_LEVEL_UP_PERCENT_SUCCESSFUL,
			CHAT_TYPE_LEVEL_UP_PERCENT_FAIL,
			CHAT_TYPE_EXP_HORSE_FEED,

			CHAT_TYPE_MAX
		};

		enum EMountUpGradeChatValue : uint8_t
		{
			CHAT_TYPE_NO_VALUE
		};

		enum EMountUpGradeReset : uint8_t
		{
			RESET
		};

		enum EMountUpGradeItem : uint32_t
		{
			HORSE_FEED_ITEM_ID = 50048,
			HORSE_FEED_EXP_COUNT = 1,
			HORSE_FEED_LEVEL_COUNT = 50
		};

		enum class EMountUpGradeRandExp : uint8_t
		{
			MIN = 200,
			MAX = 250
		};

		enum class EMountUpGradeRandFail : uint8_t
		{
			MIN = 0,
			MAX = 1
		};

		enum EMountUpGradeRetryGemCost : uint8_t
		{
			HORSE_LEVEL_DETERMINES_GEM_COST = 30,
			HORSE_LOWER_LEVEL_RETRY_GEM_COST = 50,
			HORSE_UPPER_LEVEL_RETRY_GEM_COST = 100
		};

		enum EMountUpGradeFailType : uint8_t
		{
			MOUNT_UP_GRADE_FAIL_OFF,
			MOUNT_UP_GRADE_FAIL_ON
		};

		enum EMountUpGradeGCSubheaderType : uint8_t
		{
			SUBHEADER_GC_MOUNT_UP_GRADE_OPEN,
			SUBHEADER_GC_MOUNT_UP_GRADE_REFRESH
		};

	public:
		enum EMountUpGradeCGSubheaderType : uint8_t
		{
			SUBHEADER_CG_MOUNT_UP_GRADE_EXP,
			SUBHEADER_CG_MOUNT_UP_GRADE_LEVEL_UP
		};
};

const uint32_t mount_up_grade_exp_table[EMisc::HORSE_MAX_LEVEL + 1] =
{
	/* Done with Quest */
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,	// 0 - 10
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,	// 11 - 21

	/* Real Need Exp */
	12500,	12810,	13130,	13440,	14060,	14380,	14690,	15000,	18750,	37500,		// 22 - 31
	48800,	63490,	66930,	82620,	107510,	139890,	182030,	236860,	308200				// 32 - 40
};

const uint32_t mount_up_grade_price_table[EMisc::HORSE_MAX_LEVEL + 1] =
{
	/* Done with Quest */
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,	// 0 - 10
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,	// 11 - 21

	/* Real Need Price */
	500000,		1000000,	1500000,	2000000,	2500000,	3000000,	3500000,	// 22 - 28
	4000000,	4500000,	5000000,	5000000,	5000000,	5000000,	5000000,	// 29 - 35
	5000000,	5000000,	5000000,	5000000,	5000000								// 36 - 40
};
#endif
