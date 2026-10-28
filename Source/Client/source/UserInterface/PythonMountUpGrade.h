#pragma once

#include "Locale_inc.h"

#ifdef ENABLE_RIDING_EXTENDED
	#include "GameType.h"

class CPythonMountUpGrade : public CSingleton<CPythonMountUpGrade>
{
	public:
		CPythonMountUpGrade();
		virtual ~CPythonMountUpGrade();

		void Reset();
		void Refresh();
		void Chat(const uint8_t type, const uint8_t value);

		void SetWindow(PyObject * ppyObject);
		void DestroyWindow();

		void SetHorseLevel(const uint8_t level);
		uint8_t GetHorseLevel() const;

		void SetMountUpGradeFail(const uint8_t fail);
		uint8_t IsMountUpGradeFail() const;

		void SetMountUpGradeExistingExp(const uint32_t exp);
		uint32_t GetMountUpGradeExistingExp() const;

		uint32_t GetMountUpGradeNecessaryExp() const;
		uint32_t GetMountUpGradePrice() const;

	private:
		uint8_t m_HorseLevel;
		uint8_t m_MountUpGradeFail;
		uint32_t m_MountUpGradeExistingExp;

	protected:
		PyObject* m_window;

	public:
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

		enum EMountUpGradeRetryGemCost : uint8_t
		{
			HORSE_LEVEL_DETERMINES_GEM_COST = 30,
			HORSE_LOWER_LEVEL_RETRY_GEM_COST = 50,
			HORSE_UPPER_LEVEL_RETRY_GEM_COST = 100,
		};

		enum EMountUpGradeTooltip : uint8_t
		{
			YANG,
			GEM
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

		enum EMountUpGradeCGSubheaderType : uint8_t
		{
			SUBHEADER_CG_MOUNT_UP_GRADE_EXP,
			SUBHEADER_CG_MOUNT_UP_GRADE_LEVEL_UP
		};
};

const uint32_t mount_up_grade_exp_table[HORSE_MAX_LEVEL + 1] =
{
	/* Done with Quest */
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,	// 0 - 10
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,	// 11 - 21

	/* Real Need Exp */
	12500,	12810,	13130,	13440,	14060,	14380,	14690,	15000,	18750,	37500,		// 22 - 31
	48800,	63490,	66930,	82620,	107510,	139890,	182030,	236860,	308200				// 32 - 40
};

const uint32_t mount_up_grade_price_table[HORSE_MAX_LEVEL + 1] =
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
