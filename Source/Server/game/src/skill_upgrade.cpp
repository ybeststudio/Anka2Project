#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_SKILL_UPGRADE
// Skill Upgrader (reference uiSkillUpgrade.SkillUpgradeWindow, command "/upgrade_skill"): reads skill books and
// soul stones one after another. Every read keeps the normal rules (read delay, failure chance); the Exorcism Scroll
// (71001) skips the delay and the Hermit's Advice (71094) improves the chance, one scroll per read.
#include "utils.h"
#include "config.h"
#include "constants.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "affect.h"
#include "skill.h"
#include "cmd.h"
#include "locale.hpp"
#ifdef ENABLE_BUFFI_SKILL_BOOK
#include "buffi_system.h"
#endif

namespace
{
	enum
	{
		UPGRADE_STANDARD_SKILL,
		UPGRADE_PASSIVE_SKILL,
		UPGRADE_BUFFI_SKILL,
	};

	enum
	{
		TIME_ITEM_VNUM = 71001,
		CHANCE_ITEM_VNUM = 71094,
		ZEN_BEAN_VNUM = 70102,
		SOUL_STONE_VNUM = 50513,
		SKILL_BOOK_VNUM = 50300,
		UPGRADE_MAX_COUNT = 20,
	};

	enum EReadResult
	{
		READ_STOP,		// nothing was read
		READ_FAIL,		// a book was used, the skill did not improve
		READ_SUCCESS,	// a book was used, the skill improved
	};

	struct SPassiveBook
	{
		DWORD dwSkill;
		DWORD adwBook[3];
	};

	// passive tab: books handled by CHARACTER::UseItem (level ranges / chance come from the item proto)
	const SPassiveBook s_aPassiveBook[] =
	{
		{ SKILL_LEADERSHIP,	{ 50301, 50302, 50303 } },
		{ SKILL_COMBO,		{ 50304, 50305, 50306 } },
		{ SKILL_MINING,		{ 50600, 0, 0 } },
		{ SKILL_POLYMORPH,	{ 50314, 50315, 50316 } },
	};

	struct SUpgradeOption
	{
		bool bExorcism;
		bool bLuck;
		bool bBean;
		bool bAllowNegative;
		bool bMaster;
	};

	std::map<DWORD, DWORD> s_mapLastRequest;

	const SPassiveBook* FindPassive(DWORD dwSkill)
	{
		for (size_t i = 0; i < sizeof(s_aPassiveBook) / sizeof(s_aPassiveBook[0]); ++i)
			if (s_aPassiveBook[i].dwSkill == dwSkill)
				return &s_aPassiveBook[i];
		return NULL;
	}

	bool UseItemByVnum(LPCHARACTER ch, DWORD dwVnum)
	{
		LPITEM item = ch->FindSpecifyItem(dwVnum);
		if (!item)
			return false;
		return ch->UseItem(TItemPos(item->GetWindow(), item->GetCell()));
	}

	// the scrolls are used like from the inventory, so their affect works for the next read exactly as usual
	bool HasBoost(LPCHARACTER ch, DWORD dwVnum, DWORD dwAffect)
	{
		if (ch->FindAffect(dwAffect))
			return true;
		UseItemByVnum(ch, dwVnum);
		return ch->FindAffect(dwAffect) != NULL;
	}

	bool PrepareRead(LPCHARACTER ch, time_t tNextRead, const SUpgradeOption& opt)
	{
		const time_t now = get_global_time();
		if (now < tNextRead && !(opt.bExorcism && HasBoost(ch, TIME_ITEM_VNUM, AFFECT_SKILL_NO_BOOK_DELAY)))
		{
			ch->SkillLearnWaitMoreTimeMessage(tNextRead - now);
			return false;
		}
		if (opt.bLuck)
			HasBoost(ch, CHANCE_ITEM_VNUM, AFFECT_SKILL_BOOK_BONUS);
		return true;
	}

	// reads that do not go through LearnSkillByBook use up the scroll affect themselves
	void ConsumeDelayAffect(LPCHARACTER ch, time_t tNextRead)
	{
		if (get_global_time() < tNextRead && ch->FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
		{
			ch->RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have escaped the evil ghost curse with the help of an Exorcism Scroll."));
		}
	}

	// Soul stone: costs rank points (1000 + 500 per Grand Master step, doubled with a negative rank).
	// Zen Beans lift a negative rank first; going below zero needs the player's confirmation.
	int GetSoulStoneCost(LPCHARACTER ch, int iLevel)
	{
		int iCost = 1000 + 500 * (iLevel - 30);
		if (ch->GetRealAlignment() < 0)
			iCost *= 2;
		return iCost * 10;
	}

	bool PaySoulStoneGrade(LPCHARACTER ch, int iLevel, const SUpgradeOption& opt, int& riCost)
	{
		if (opt.bBean)
		{
			while (ch->GetRealAlignment() < 0)
			{
				const int iBefore = ch->GetRealAlignment();
				if (!UseItemByVnum(ch, ZEN_BEAN_VNUM) || ch->GetRealAlignment() == iBefore)
					break;
			}
		}

		riCost = GetSoulStoneCost(ch, iLevel);
		if (ch->GetRealAlignment() - riCost < 0 && !opt.bAllowNegative)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "SkillUpgradeNegativeGradeConfirm");
			return false;
		}
		return true;
	}

	bool RollGrandMaster(int iReadCount, int iLevel, bool bBonus)
	{
		const int idx = MINMAX(0, iLevel - 30, 9);
		int iBookCount = aiGrandMasterSkillBookCountForLevelUp[idx];
		if (bBonus)
			iBookCount = (iBookCount & 1) ? iBookCount / 2 + 1 : iBookCount / 2;

		if (iReadCount < aiGrandMasterSkillBookMinCount[idx])
			return false;
		if (iReadCount > aiGrandMasterSkillBookMaxCount[idx])
			return true;
		return number(1, MAX(1, iBookCount)) == 2;
	}

	// ------------------------------------------------------------------ standard skills
	LPITEM FindSkillBook(LPCHARACTER ch, DWORD dwSkill, int* piCount)
	{
		LPITEM pkFound = NULL;
		int iCount = 0;
		for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		{
			LPITEM item = ch->GetInventoryItem(i);
			if (!item || item->IsEquipped() || item->GetType() != ITEM_SKILLBOOK)
				continue;

			const DWORD dwBookSkill = item->GetVnum() == SKILL_BOOK_VNUM ? (DWORD) item->GetSocket(0) : (DWORD) item->GetValue(0);
			if (dwBookSkill != dwSkill)
				continue;

			iCount += item->GetCount();
			if (!pkFound)
				pkFound = item;
		}
		if (piCount)
			*piCount = iCount;
		return pkFound;
	}

	bool CanUpgradeStandard(LPCHARACTER ch, DWORD dwSkill)
	{
		const CSkillProto* pkSk = CSkillManager::instance().Get(dwSkill);
		return pkSk && pkSk->dwType != 0 && ch->GetSkillGroup() && ch->IsLearnableSkill(dwSkill);
	}

	EReadResult ReadStandard(LPCHARACTER ch, DWORD dwSkill, const SUpgradeOption& opt)
	{
		if (!CanUpgradeStandard(ch, dwSkill))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot train this skill."));
			return READ_STOP;
		}
		if (ch->IsPolymorphed())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read a book while you are transformed."));
			return READ_STOP;
		}

		const int iLevel = ch->GetSkillLevel(dwSkill);
		const time_t tNextRead = ch->GetSkillNextReadTime(dwSkill);

		if (ch->GetSkillMasterType(dwSkill) == SKILL_MASTER)
		{
			LPITEM book = FindSkillBook(ch, dwSkill, NULL);
			if (!book)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have the required skill book."));
				return READ_STOP;
			}
			if (!PrepareRead(ch, tNextRead, opt) || !ch->LearnSkillByBook(dwSkill))
				return READ_STOP;

			book->SetCount(book->GetCount() - 1);
			ch->SetSkillNextReadTime(dwSkill, get_global_time() + number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX));
			return ch->GetSkillLevel(dwSkill) != iLevel ? READ_SUCCESS : READ_FAIL;
		}

		if (ch->GetSkillMasterType(dwSkill) == SKILL_GRAND_MASTER)
		{
			LPITEM stone = ch->FindSpecifyItem(SOUL_STONE_VNUM);
			if (!stone)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a Soul Stone to train this skill further."));
				return READ_STOP;
			}

			int iCost = 0;
			if (!PaySoulStoneGrade(ch, iLevel, opt, iCost) || !PrepareRead(ch, tNextRead, opt))
				return READ_STOP;

			ConsumeDelayAffect(ch, tNextRead);
			ch->UpdateAlignment(-iCost);
			stone->SetCount(stone->GetCount() - 1);
			ch->LearnGrandMasterSkill(dwSkill);	// rolls the chance and sets the next read time
			return ch->GetSkillLevel(dwSkill) != iLevel ? READ_SUCCESS : READ_FAIL;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, iLevel >= 40 ? LC_TEXT("This skill is already at maximum level.") : LC_TEXT("This skill's level is not high enough to be trained with a Book."));
		return READ_STOP;
	}

	// ------------------------------------------------------------------ passive skills
	LPITEM FindPassiveBook(LPCHARACTER ch, DWORD dwSkill, DWORD* pdwVnum, int* piCount)
	{
		const SPassiveBook* p = FindPassive(dwSkill);
		if (!p)
			return NULL;

		const int iLevel = ch->GetSkillLevel(dwSkill);
		for (int i = 0; i < 3; ++i)
		{
			if (!p->adwBook[i])
				continue;
			const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(p->adwBook[i]);
			if (!pTable)
				continue;
			// leadership / polymorph books cover a level range (value0 <= level < value1)
			if ((dwSkill == SKILL_LEADERSHIP || dwSkill == SKILL_POLYMORPH) && (iLevel < pTable->alValues[0] || iLevel >= pTable->alValues[1]))
				continue;

			if (pdwVnum && !*pdwVnum)
				*pdwVnum = p->adwBook[i];
			LPITEM item = ch->FindSpecifyItem(p->adwBook[i]);
			if (item)
			{
				if (pdwVnum)
					*pdwVnum = p->adwBook[i];
				if (piCount)
					*piCount = ch->CountSpecifyItem(p->adwBook[i]);
				return item;
			}
		}
		return NULL;
	}

	EReadResult ReadPassive(LPCHARACTER ch, DWORD dwSkill, const SUpgradeOption& opt)
	{
		LPITEM book = FindPassiveBook(ch, dwSkill, NULL, NULL);
		if (!book)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have the required skill book."));
			return READ_STOP;
		}

		const int iLevel = ch->GetSkillLevel(dwSkill);
		const DWORD dwBook = book->GetVnum();
		const int iBefore = ch->CountSpecifyItem(dwBook);
		if (!PrepareRead(ch, ch->GetSkillNextReadTime(dwSkill), opt))
			return READ_STOP;

		ch->UseItem(TItemPos(book->GetWindow(), book->GetCell()));
		if (ch->CountSpecifyItem(dwBook) >= iBefore)
			return READ_STOP;
		return ch->GetSkillLevel(dwSkill) != iLevel ? READ_SUCCESS : READ_FAIL;
	}

#ifdef ENABLE_BUFFI_SKILL_BOOK
	// ------------------------------------------------------------------ Buffi skills
	int BuffiSkillIndex(DWORD dwSkill)
	{
		for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
			if (CBuffiActor::GetSkillVnum(i) == dwSkill)
				return i;
		return -1;
	}

	std::string BuffiReadFlag(int idx)
	{
		char szFlag[32];
		snprintf(szFlag, sizeof(szFlag), "buffi.read_time%d", idx);
		return szFlag;
	}

	int BuffiNeedBooks(int iLevel)
	{
		if (iLevel < 30)
			return iLevel - 19;	// M1 needs 1 book ... M10 needs 10 books
		return aiGrandMasterSkillBookMaxCount[MINMAX(0, iLevel - 30, 9)];
	}

	EReadResult ReadBuffi(LPCHARACTER ch, DWORD dwSkill, const SUpgradeOption& opt)
	{
		CBuffiActor* pActor = CBuffiManager::instance().Get(ch);
		const int idx = BuffiSkillIndex(dwSkill);
		if (!pActor || idx < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please summon your Buffi before doing that."));
			return READ_STOP;
		}

		const int iLevel = pActor->GetSkillLevel(idx);
		if (iLevel < 20 || iLevel >= 40)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, iLevel >= 40 ? LC_TEXT("This skill is already at maximum level.") : LC_TEXT("This skill's level is not high enough to be trained with a Book."));
			return READ_STOP;
		}

		if (opt.bMaster)	// Buffi Mastery Book / Stone: one level step without delay or chance
			return pActor->UseMasterBook(idx) ? READ_SUCCESS : READ_STOP;

		const bool bMasterLevel = iLevel < 30;
		const DWORD dwBook = bMasterLevel ? (DWORD) BUFFI_SKILL_BOOK_VNUM : (DWORD) SOUL_STONE_VNUM;
		LPITEM book = ch->FindSpecifyItem(dwBook);
		if (!book)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, bMasterLevel ? LC_TEXT("You don't have the required skill book.") : LC_TEXT("You need a Soul Stone to train this skill further."));
			return READ_STOP;
		}

		const time_t tNextRead = ch->GetQuestFlag(BuffiReadFlag(idx));
		int iCost = 0;
		if (!bMasterLevel && !PaySoulStoneGrade(ch, iLevel, opt, iCost))
			return READ_STOP;
		if (!PrepareRead(ch, tNextRead, opt))
			return READ_STOP;

		ConsumeDelayAffect(ch, tNextRead);
		const bool bBonus = ch->FindAffect(AFFECT_SKILL_BOOK_BONUS) != NULL;
		if (bBonus)
			ch->RemoveAffect(AFFECT_SKILL_BOOK_BONUS);
		if (iCost)
			ch->UpdateAlignment(-iCost);
		book->SetCount(book->GetCount() - 1);

		int iProgress = pActor->GetBookProgress(idx);
		bool bLevelUp = false;
		if (bMasterLevel)
		{
			// same as a character's Master book: 35% (Hermit's Advice: 100%) to understand it
			if (bBonus || number(1, 100) > 65)
				bLevelUp = ++iProgress >= BuffiNeedBooks(iLevel);
			ch->SetQuestFlag(BuffiReadFlag(idx), get_global_time() + number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX));
		}
		else
		{
			bLevelUp = RollGrandMaster(++iProgress, iLevel, bBonus);
			ch->SetQuestFlag(BuffiReadFlag(idx), get_global_time() + number(28800, 43200));
		}

		if (bLevelUp)
		{
			pActor->OnBookLevelUp(idx);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's skill has improved."));
			return READ_SUCCESS;
		}

		pActor->SetBookProgress(idx, iProgress);
		if (bMasterLevel)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You read a Buffi skill book (%d/%d)."), iProgress, BuffiNeedBooks(iLevel));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Training has failed. Please try again later."));
		return READ_FAIL;
	}
#endif

	// "SkillUpgradeInfo <type> <skill> <book vnum> <book count> <progress> <need> <remaining delay> <maxed>"
	void SendInfo(LPCHARACTER ch, int iType, DWORD dwSkill)
	{
		DWORD dwBook = 0;
		int iCount = 0, iProgress = 0, iNeed = 0, iMaxed = 0;
		time_t tNextRead = 0;

		if (iType == UPGRADE_STANDARD_SKILL)
		{
			const int iLevel = ch->GetSkillLevel(dwSkill);
			tNextRead = ch->GetSkillNextReadTime(dwSkill);
			if (iLevel >= 40)
				iMaxed = 1;
			else if (iLevel >= 30)
			{
				dwBook = SOUL_STONE_VNUM;
				iCount = ch->CountSpecifyItem(SOUL_STONE_VNUM);
				char szFlag[64];
				snprintf(szFlag, sizeof(szFlag), "training_grandmaster_skill.skill%u", dwSkill);
				iNeed = aiGrandMasterSkillBookMaxCount[MINMAX(0, iLevel - 30, 9)];
				iProgress = MIN(iNeed, ch->GetQuestFlag(szFlag));
			}
			else if (iLevel >= 20)
			{
				LPITEM book = FindSkillBook(ch, dwSkill, &iCount);
				dwBook = book ? book->GetVnum() : (DWORD) SKILL_BOOK_VNUM;
				iNeed = 1;
			}
		}
		else if (iType == UPGRADE_PASSIVE_SKILL)
		{
			tNextRead = ch->GetSkillNextReadTime(dwSkill);
			FindPassiveBook(ch, dwSkill, &dwBook, &iCount);
			iMaxed = dwBook ? 0 : 1;
			iNeed = 1;
		}
#ifdef ENABLE_BUFFI_SKILL_BOOK
		else if (iType == UPGRADE_BUFFI_SKILL)
		{
			CBuffiActor* pActor = CBuffiManager::instance().Get(ch);
			const int idx = BuffiSkillIndex(dwSkill);
			if (pActor && idx >= 0)
			{
				const int iLevel = pActor->GetSkillLevel(idx);
				tNextRead = ch->GetQuestFlag(BuffiReadFlag(idx));
				if (iLevel >= 40)
					iMaxed = 1;
				else if (iLevel >= 20)
				{
					dwBook = iLevel < 30 ? (DWORD) BUFFI_SKILL_BOOK_VNUM : (DWORD) SOUL_STONE_VNUM;
					iCount = ch->CountSpecifyItem(dwBook);
					iNeed = BuffiNeedBooks(iLevel);
					iProgress = MIN(iNeed, pActor->GetBookProgress(idx));
				}
			}
		}
#endif

		const time_t now = get_global_time();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SkillUpgradeInfo %d %u %u %d %d %d %d %d", iType, dwSkill, dwBook, iCount, iProgress, iNeed,
			tNextRead > now ? (int) (tNextRead - now) : 0, iMaxed);
	}

	bool CanUseUpgrader(LPCHARACTER ch)
	{
		if (ch->IsDead() || ch->IsStun())
			return false;
		if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close all other windows first."));
			return false;
		}
		return true;
	}
}

// /upgrade_skill <type> <skill> <count> <exorcism> <hermit's advice> <zen bean> <allow negative rank> [mastery book]
ACMD(do_upgrade_skill)
{
	int iType = 0, iCount = 0, iExorcism = 0, iLuck = 0, iBean = 0, iNegative = 0, iMaster = 0;
	unsigned int uSkill = 0;
	if (sscanf(argument, "%d %u %d %d %d %d %d %d", &iType, &uSkill, &iCount, &iExorcism, &iLuck, &iBean, &iNegative, &iMaster) < 7)
		return;

	const DWORD dwNow = get_dword_time();
	DWORD& rdwLast = s_mapLastRequest[ch->GetPlayerID()];
	if (dwNow - rdwLast < 300)
		return;
	rdwLast = dwNow;

	if (!CanUseUpgrader(ch))
		return;

	SUpgradeOption opt;
	opt.bExorcism = iExorcism != 0;
	opt.bLuck = iLuck != 0;
	opt.bBean = iBean != 0;
	opt.bAllowNegative = iNegative != 0;
	opt.bMaster = iMaster != 0;

	int iRead = 0, iSuccess = 0;
	iCount = MINMAX(1, iCount, (int) UPGRADE_MAX_COUNT);
	for (int i = 0; i < iCount; ++i)
	{
		EReadResult eResult = READ_STOP;
		switch (iType)
		{
			case UPGRADE_STANDARD_SKILL:	eResult = ReadStandard(ch, uSkill, opt); break;
			case UPGRADE_PASSIVE_SKILL:		eResult = ReadPassive(ch, uSkill, opt); break;
#ifdef ENABLE_BUFFI_SKILL_BOOK
			case UPGRADE_BUFFI_SKILL:		eResult = ReadBuffi(ch, uSkill, opt); break;
#endif
			default:						break;
		}

		if (eResult == READ_STOP)
			break;
		++iRead;
		if (eResult == READ_SUCCESS)
			++iSuccess;
	}

	if (iRead)
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SkillUpgradeResult %d", iSuccess ? 1 : 0);
	SendInfo(ch, iType, uSkill);
}

// /upgrade_skill_info <type> <skill>
ACMD(do_upgrade_skill_info)
{
	int iType = 0;
	unsigned int uSkill = 0;
	if (sscanf(argument, "%d %u", &iType, &uSkill) < 2)
		return;
	SendInfo(ch, iType, uSkill);
}
#endif
