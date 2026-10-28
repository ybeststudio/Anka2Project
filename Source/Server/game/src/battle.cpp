#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "char.h"
#include "char_manager.h"
#include "battle.h"
#include "item.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "vector.h"
#include "packet.h"
#include "pvp.h"
#include "profiler.h"
#include "guild.h"
#include "affect.h"
#include "unique_item.h"
#include "lua_incl.h"
#include "arena.h"
#include "sectree.h"
#include "ani.h"
#include "locale_service.h"
#include "../../common/service.h"
#include "log.h"
#include "questmanager.h"
#include "db.h"

#ifdef ENABLE_ULTIMATE_REGEN
	#include "new_mob_timer.h"
#endif

#ifdef ENABLE_QUEEN_NETHIS
	#include "SnakeLair.h"
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
	#include "minigame.h"
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
	#include "sectree_manager.h"
#endif

int battle_hit(LPCHARACTER ch, LPCHARACTER victim, int& iRetDam);


bool battle_distance_valid_by_xy(const CHARACTER* ch, const CHARACTER * victim)
{
	if (!ch || !victim)
		return false;

	const auto distance = DISTANCE_APPROX(ch->GetX() - victim->GetX(), ch->GetY() - victim->GetY());
	float fHitRange = 300;

	if (distance > fHitRange)
		return false;

	return true;
}

bool battle_distance_valid(const CHARACTER* ch, const CHARACTER* victim)
{
	if (!ch || !victim)
		return false;

	return battle_distance_valid_by_xy(ch, victim);
}

bool timed_event_cancel(LPCHARACTER ch)
{
	if (ch->m_pkTimedEvent)
	{
		event_cancel(&ch->m_pkTimedEvent);
		return true;
	}

#ifdef ENABLE_CHANGE_CHANNEL
	if (ch->m_pkChangeChannelEvent)
	{
		event_cancel(&ch->m_pkChangeChannelEvent);
		return true;
	}
#endif

	return false;
}

bool battle_is_attackable(LPCHARACTER ch, LPCHARACTER victim)
{
	if (!ch || !victim)
		return false;

	// If the opponent dies, stop.
	if (victim->IsDead() || victim->IsObserverMode())	//@fixme452
		return false;

	switch(ch->GetMapIndex())
	{
		case 57:
		case 64:
		case 61:
		case 63:
		case 68:
			{
				if (victim->IsPC() && ch->IsPC())
				{
#ifdef ENABLE_BOT_PLAYER
					// Bot vs bot veya bot vs dusman oyuncu: farkli imparatorluklarda saldiriya izin ver
					if (ch->IsBotCharacter() && victim->GetEmpire() != ch->GetEmpire())
						; // allow - fall through
					else
#endif
						return false;
				}
			}
	}

	// stop in safe zone
	{
		SECTREE	*sectree = NULL;

		sectree = ch->GetSectree();
		if (sectree && sectree->IsAttr(ch->GetX(), ch->GetY(), ATTR_BANPK))
			return false;

		sectree = victim->GetSectree();
		if (sectree && sectree->IsAttr(victim->GetX(), victim->GetY(), ATTR_BANPK))
			return false;
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (victim->IsInvincible())
		return false;
#endif

	// abort if i die
	if (ch->IsStun() || ch->IsDead() || ch->IsObserverMode())	//@fixme452
		return false;

#ifdef ENABLE_ULTIMATE_REGEN
	if (!CNewMobTimer::Instance().CheckDamage(ch, victim))
		return false;
#endif

	if (ch->IsPC() && victim->IsPC())
	{
		CGuild* g1 = ch->GetGuild();
		const CGuild* g2 = victim->GetGuild();

		if (g1 && g2)
		{
			if (g1->UnderWar(g2->GetID()))
				return true;
		}
	}

	if (CArenaManager::instance().CanAttack(ch, victim))
		return true;

#ifdef ENABLE_CONQUEROR_LEVEL
	if(ch->IsPC() && ch->IsConquerorMap(ch->GetMapIndex()) && !(victim->IsPC()))
		if(ch->GetConquerorLevel() == 0)
			return false;
#endif

	return CPVPManager::instance().CanAttack(ch, victim);
}

int battle_melee_attack(LPCHARACTER ch, LPCHARACTER victim)
{
	if (!ch || !victim)
		return BATTLE_NONE;

	if (test_server && ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	if (!victim || ch == victim)
		return BATTLE_NONE;

	if (test_server && ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	if (!battle_is_attackable(ch, victim))
		return BATTLE_NONE;

	if (test_server && ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	// check distance from victim center plus its hit range so large mobs/stones don't require overlap
	const auto distance = DISTANCE_APPROX(ch->GetX() - victim->GetX(), ch->GetY() - victim->GetY());

	if (!victim->IsBuilding())
	{
		int max = battle_get_max_distance(ch, victim);	//@fixme499

		if (!ch->IsPC()
#ifdef ENABLE_BOT_PLAYER
			&& ! ch->IsBotCharacter()
#endif
			)
		{
			// For monsters, use the monster attack distance
			max = static_cast<int>(ch->GetMobAttackRange() * 1.15f);
		}
		else
		{
			// On PC, if the opponent is a melee mob, the mob's attack distance is the maximum attack distance
			if (!victim->IsPC() && BATTLE_TYPE_MELEE == victim->GetMobBattleType()
#ifdef ENABLE_BOT_PLAYER
			&& ! victim->IsBotCharacter()
#endif
				)
				max = MAX(max, static_cast<int>(victim->GetMobAttackRange() * 1.15f));
		}

		if (distance > max)
		{
			if (test_server)
				sys_log(0, "VICTIM_FAR: %s distance: %d max: %d", ch->GetName(), distance, max);

			return BATTLE_NONE;
		}
	}

	if (timed_event_cancel(ch))
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Action cancelled. You have entered a battle."));

	if (timed_event_cancel(victim))
		victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Action cancelled. You have entered a battle."));

	ch->SetPosition(POS_FIGHTING);
	ch->SetVictim(victim);

	const PIXEL_POSITION& vpos = victim->GetXYZ();
	ch->SetRotationToXY(vpos.x, vpos.y);

	int dam;
	const auto ret = battle_hit(ch, victim, dam);
	return (ret);
}

//@fixme499
int battle_get_max_distance(const LPCHARACTER& ch, const LPCHARACTER& victim)
{
	int max = 300;

	if (!ch || !victim)
		return max;

	if (victim->IsMonster() || victim->IsStone())
		max = (max / 100) * static_cast<int>(victim->GetMonsterHitRange());

	if (ch->IsRiding())
		max += 100;

	return max;
}
//@end_fixme499

// Make the actual GET_BATTLE_VICTIM NULL and cancel the event.
void battle_end_ex(LPCHARACTER ch)
{
	if (ch && ch->IsPosition(POS_FIGHTING))
		ch->SetPosition(POS_STANDING);
}

void battle_end(LPCHARACTER ch)
{
	if (!ch)
		return;

	battle_end_ex(ch);
}

// AG = Attack Grade
// AL = Attack Limit
int CalcBattleDamage(int iDam, int iAttackerLev, int iVictimLev) noexcept
{
	if (iDam < 3)
		iDam = number(1, 5);

	//return CALCULATE_DAMAGE_LVDELTA(iAttackerLev, iVictimLev, iDam);
	return iDam;
}

int CalcMagicDamageWithValue(int iDam, const CHARACTER* pkAttacker, const CHARACTER* pkVictim) noexcept
{
	if (!pkAttacker || !pkVictim)
		return 0;

	return CalcBattleDamage(iDam, pkAttacker->GetLevel(), pkVictim->GetLevel());
}

int CalcMagicDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
{
	if (!pkAttacker)
		return 0;

	int iDam = 0;

	if (pkAttacker->IsNPC())
	{
		iDam = CalcMeleeDamage(pkAttacker, pkVictim, false, false);
	}

	iDam += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);

	return CalcMagicDamageWithValue(iDam, pkAttacker, pkVictim);
}

float CalcAttackRating(const CHARACTER* pkAttacker, const CHARACTER* pkVictim, bool bIgnoreTargetRating)
{
	if (!pkAttacker || !pkVictim)
		return 0.0f;

	int iARSrc = 0;
	int iERSrc = 0;

	{
		const auto attacker_dx = pkAttacker->GetPolymorphPoint(POINT_DX);
		const auto attacker_lv = pkAttacker->GetLevel();

		const auto victim_dx = pkVictim->GetPolymorphPoint(POINT_DX);
		const auto victim_lv = pkAttacker->GetLevel();

		iARSrc = MIN(90, (attacker_dx * 4 + attacker_lv * 2) / 6);
		iERSrc = MIN(90, (victim_dx * 4 + victim_lv * 2) / 6);
	}

	const float fAR = static_cast<float>(iARSrc + 210.0f) / 300.0f; // fAR = 0.7 ~ 1.0

	if (bIgnoreTargetRating)
		return fAR;

	// ((Edx * 2 + 20) / (Edx + 110)) * 0.3
	const float fER = (static_cast<float>(iERSrc * 2 + 5) / (iERSrc + 95)) * 3.0f / 10.0f;

	return fAR - fER;
}

int CalcAttBonus(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, int iAtk)
{
	if (!pkAttacker || !pkVictim)
		return 0;

	// Does not apply to PvP
	if (!pkVictim->IsPC()
#ifdef ENABLE_BOT_PLAYER
		&& !pkVictim->IsBotCharacter()
#endif
		)
		iAtk += pkAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_ATTACK_BONUS);

	// Does not apply to PvP
	if (!pkAttacker->IsPC()
#ifdef ENABLE_BOT_PLAYER
		&& !pkAttacker->IsBotCharacter()
#endif
		)
	{
		const auto iReduceDamagePct = pkVictim->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_TRANSFER_DAMAGE);
		iAtk = iAtk * (100 + iReduceDamagePct) / 100;
	}

	if (pkAttacker->IsNPC() && pkVictim->IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| pkVictim->IsBotCharacter()
		// && !pkVictim->IsBotCharacter()
#endif
		)
	{
		iAtk = (iAtk * CHARACTER_MANAGER::instance().GetMobDamageRate(pkAttacker)) / 100;
	}

	if (pkVictim->IsNPC())
	{
		/*
		* NOTE : This was changed following the last wiki information:
		* https://fr-wiki.metin2.gameforge.com/index.php/Force_contre_les_animaux_(%2BX%25_de_la_valeur_totale)
		*/

		if (pkVictim->IsRaceFlag(RACE_FLAG_ANIMAL))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ANIMAL)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_UNDEAD))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_UNDEAD)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_DEVIL))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DEVIL)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_HUMAN))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_HUMAN)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_ORC))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ORC)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_MILGYO))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MILGYO)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_INSECT))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_INSECT)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_FIRE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_FIRE)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_ICE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ICE)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_DESERT))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DESERT)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_TREE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_TREE)) / 100;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_CZ))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_CZ)) / 100;
		if (pkVictim->IsMonster())
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MONSTER)) / 100;
#ifdef ENABLE_NEW_BONUS_SYSTEM
		if (pkVictim->IsStone())
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_STONE)) / 100;
		if (pkVictim->GetMobRank() >= MOB_RANK_BOSS)
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_BOSS)) / 100;
		iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ELEMENTS)) / 100;
#endif
	}
	else if (pkVictim->IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| pkVictim->IsBotCharacter()
#endif
		)
	{
		iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_HUMAN)) / 100;
#ifdef ENABLE_NEW_BONUS_SYSTEM
		iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_CHARACTERS)) / 100;
#endif

		switch (pkVictim->GetJob())
		{
			case JOB_WARRIOR:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_WARRIOR)) / 100;
				break;

			case JOB_ASSASSIN:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ASSASSIN)) / 100;
				break;

			case JOB_SURA:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_SURA)) / 100;
				break;

			case JOB_SHAMAN:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_SHAMAN)) / 100;
				break;

#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_WOLFMAN)) / 100;
				break;
#endif

			default:
				break;
		}
	}

	if (pkAttacker->IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| pkAttacker->IsBotCharacter()
#endif
		)
	{
#ifdef ENABLE_NEW_BONUS_SYSTEM
		iAtk -= (iAtk * pkVictim->GetPoint(POINT_ENCHANT_CHARACTERS)) / 150;
#endif

		switch (pkAttacker->GetJob())
		{
			case JOB_WARRIOR:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_WARRIOR)) / 100;
				break;

			case JOB_ASSASSIN:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_ASSASSIN)) / 100;
				break;

			case JOB_SURA:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_SURA)) / 100;
				break;

			case JOB_SHAMAN:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_SHAMAN)) / 100;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_WOLFMAN)) / 100;
				break;
#endif
			default:
				break;
		}
	}

	// [mob -> PC] Apply elemental defense
	// 20130117.
	// Resistance is applied to the value corresponding to x%
	// of the damage caused by a monster's elemental attack.

	if (pkAttacker->IsNPC() && pkVictim->IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| pkVictim->IsBotCharacter()
#endif
		)
	{
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ELEC))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_ELEC)) / 10000;
		else if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_FIRE))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_FIRE)) / 10000;
		else if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ICE))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_ICE)) / 10000;
		else if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_WIND))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_WIND)) / 10000;
		else if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_EARTH))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_EARTH)) / 10000;
		else if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_DARK)) / 10000;
#ifdef ENABLE_NEW_BONUS_SYSTEM
		iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_MONSTER)) / 10000;
#endif
	}

	if (pkAttacker->IsNPC() && pkVictim->IsPC())
	{
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_ELEC))
			iAtk += (iAtk * 30 * pkAttacker->GetPoint(POINT_ENCHANT_ELECT)) / 10000;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_FIRE))
			iAtk += (iAtk * 30 * pkAttacker->GetPoint(POINT_ENCHANT_FIRE)) / 10000;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_ICE))
			iAtk += (iAtk * 30 * pkAttacker->GetPoint(POINT_ENCHANT_ICE)) / 10000;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_WIND))
			iAtk += (iAtk * 30 * pkAttacker->GetPoint(POINT_ENCHANT_WIND)) / 10000;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_EARTH))
			iAtk += (iAtk * 30 * pkAttacker->GetPoint(POINT_ENCHANT_EARTH)) / 10000;
		else if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk += (iAtk * 30 * pkAttacker->GetPoint(POINT_ENCHANT_DARK)) / 10000;
	}

	return iAtk;
}

void Item_GetDamage(LPITEM pkItem, int* pdamMin, int* pdamMax)
{
	if (!pdamMin || !pdamMax)
		return;

	*pdamMin = 0;
	*pdamMax = 1;

	if (!pkItem)
		return;

	switch (pkItem->GetType())
	{
		case ITEM_ROD:
		case ITEM_PICK:
			return;

		default:
			break;
	}

	if (pkItem->GetType() != ITEM_WEAPON)
		sys_err("Item_GetDamage - !ITEM_WEAPON vnum=%d, type=%d", pkItem->GetOriginalVnum(), pkItem->GetType());

	auto damMin = pkItem->GetValue(3);
	auto damMax = pkItem->GetValue(4);

	*pdamMin = damMin;
	*pdamMax = damMax;
}

int CalcMeleeDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, bool bIgnoreDefense, bool bIgnoreTargetRating)
{
	LPITEM pWeapon = pkAttacker->GetWear(WEAR_WEAPON);
	const auto bPolymorphed = pkAttacker->IsPolymorphed();

	if (pWeapon && !(bPolymorphed && !pkAttacker->IsPolyMaintainStat()))
	{
		if (pWeapon->GetType() != ITEM_WEAPON)
			return 0;

		switch (pWeapon->GetSubType())
		{
			case WEAPON_SWORD:
			case WEAPON_DAGGER:
			case WEAPON_TWO_HANDED:
			case WEAPON_BELL:
			case WEAPON_FAN:
			case WEAPON_MOUNT_SPEAR:
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
#endif
				break;

			case WEAPON_BOW:
				sys_log(0, "CalcMeleeDamage should not handle bows (name: %s)", pkAttacker->GetName());

				return 0;

			default:
				return 0;
		}
	}

	int iDam = 0;
	const auto fAR = CalcAttackRating(pkAttacker, pkVictim, bIgnoreTargetRating);
	int iDamMin = 0, iDamMax = 0;

	// TESTSERVER_SHOW_ATTACKINFO
	int DEBUG_iDamCur = 0;
	int DEBUG_iDamBonus = 0;
	// END_OF_TESTSERVER_SHOW_ATTACKINFO

	if (bPolymorphed && !pkAttacker->IsPolyMaintainStat())
	{
		// MONKEY_ROD_ATTACK_BUG_FIX
		Item_GetDamage(pWeapon, &iDamMin, &iDamMax);
		// END_OF_MONKEY_ROD_ATTACK_BUG_FIX

		const auto dwMobVnum = pkAttacker->GetPolymorphVnum();
		const CMob* pMob = CMobManager::instance().Get(dwMobVnum);

		if (pMob)
		{
			const auto iPower = pkAttacker->GetPolymorphPower();
			iDamMin += pMob->m_table.dwDamageRange[0] * iPower / 100;
			iDamMax += pMob->m_table.dwDamageRange[1] * iPower / 100;
		}
	}
	else if (pWeapon)
	{
		// MONKEY_ROD_ATTACK_BUG_FIX
		Item_GetDamage(pWeapon, &iDamMin, &iDamMax);
		// END_OF_MONKEY_ROD_ATTACK_BUG_FIX
	}
	else if (pkAttacker->IsNPC())
	{
		iDamMin = pkAttacker->GetMobDamageMin();
		iDamMax = pkAttacker->GetMobDamageMax();
	}

	iDam = number(iDamMin, iDamMax) * 2;

	// TESTSERVER_SHOW_ATTACKINFO
	DEBUG_iDamCur = iDam;
	// END_OF_TESTSERVER_SHOW_ATTACKINFO
	//
	int iAtk = 0;

	// level must be ignored when multiply by fAR, so subtract it before calculation.
	if (pkAttacker->IsPC() && pkVictim->IsPvM())
	{
		iAtk = (pkAttacker->GetPoint(POINT_ATT_GRADE) + pkAttacker->GetPoint(POINT_ATTBONUS_PVM_STR)) + iDam - (pkAttacker->GetLevel() * 2);
	}
	else
	{
		iAtk = pkAttacker->GetPoint(POINT_ATT_GRADE) + iDam - (pkAttacker->GetLevel() * 2);
	}
	iAtk = static_cast<int>(iAtk * fAR);
	iAtk += pkAttacker->GetLevel() * 2; // and add again

	if (pWeapon)
	{
		iAtk += pWeapon->GetValue(5) * 2;

		// 2004.11.12.myevan.TESTSERVER_SHOW_ATTACKINFO
		DEBUG_iDamBonus = pWeapon->GetValue(5) * 2;
	}

	iAtk += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS); // party attacker role bonus
	if (pkAttacker->IsPC() && pkVictim->IsPvM())
	{
		iAtk = (int)(iAtk * (100 + (pkAttacker->GetPoint(POINT_ATT_BONUS) + pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER) + pkAttacker->GetPoint(POINT_ATTBONUS_PVM_BERSERKER))) / 100);
	}
	else
	{
		iAtk = (int)(iAtk * (100 + (pkAttacker->GetPoint(POINT_ATT_BONUS) + pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100);
	}

	iAtk = CalcAttBonus(pkAttacker, pkVictim, iAtk);

	int iDef = 0;

	if (!bIgnoreDefense)
	{
		iDef = (pkVictim->GetPoint(POINT_DEF_GRADE) * (100 + pkVictim->GetPoint(POINT_DEF_BONUS)) / 100);

		if (!pkAttacker->IsPC())
			iDef += pkVictim->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_DEFENSE_BONUS);
	}

	if (pkAttacker->IsNPC())
		iAtk = static_cast<int>(iAtk * pkAttacker->GetMobDamageMultiply());

	iDam = MAX(0, iAtk - iDef);

	if (test_server)
	{
		const auto DEBUG_iLV = pkAttacker->GetLevel() * 2;
		const auto DEBUG_iST = static_cast<int>((pkAttacker->GetPoint(POINT_ATT_GRADE) - DEBUG_iLV) * fAR);
		const auto DEBUG_iPT = pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);
		int DEBUG_iWP = 0;
		int DEBUG_iPureAtk = 0;
		int DEBUG_iPureDam = 0;
		char szRB[32] = "";
		char szGradeAtkBonus[32] = "";

		DEBUG_iWP = static_cast<int>(DEBUG_iDamCur * fAR);
		DEBUG_iPureAtk = DEBUG_iLV + DEBUG_iST + DEBUG_iWP + DEBUG_iDamBonus;
		DEBUG_iPureDam = iAtk - iDef;

		if (pkAttacker->IsNPC())
		{
			snprintf(szGradeAtkBonus, sizeof(szGradeAtkBonus), "=%d*%.1f", DEBUG_iPureAtk, pkAttacker->GetMobDamageMultiply());
			DEBUG_iPureAtk = static_cast<int>(DEBUG_iPureAtk * pkAttacker->GetMobDamageMultiply());
		}

		if (DEBUG_iDamBonus != 0)
			snprintf(szRB, sizeof(szRB), "+RB(%d)", DEBUG_iDamBonus);

		char szPT[32] = "";

		if (DEBUG_iPT != 0)
			snprintf(szPT, sizeof(szPT), ", PT=%d", DEBUG_iPT);

		char szUnknownAtk[32] = "";

		if (iAtk != DEBUG_iPureAtk)
			snprintf(szUnknownAtk, sizeof(szUnknownAtk), "+?(%d)", iAtk - DEBUG_iPureAtk);

		char szUnknownDam[32] = "";

		if (iDam != DEBUG_iPureDam)
			snprintf(szUnknownDam, sizeof(szUnknownDam), "+?(%d)", iDam - DEBUG_iPureDam);

		char szMeleeAttack[128];

		snprintf(szMeleeAttack, sizeof(szMeleeAttack),
			"%s(%d)-%s(%d)=%d%s, ATK=LV(%d)+ST(%d)+WP(%d)%s%s%s, AR=%.3g%s",
			pkAttacker->GetName(),
			iAtk,
			pkVictim->GetName(),
			iDef,
			iDam,
			szUnknownDam,
			DEBUG_iLV,
			DEBUG_iST,
			DEBUG_iWP,
			szRB,
			szUnknownAtk,
			szGradeAtkBonus,
			fAR,
			szPT);

		pkAttacker->ChatPacket(CHAT_TYPE_TALKING, "%s", szMeleeAttack);
		pkVictim->ChatPacket(CHAT_TYPE_TALKING, "%s", szMeleeAttack);
	}

	return CalcBattleDamage(iDam, pkAttacker->GetLevel(), pkVictim->GetLevel());
}

int CalcArrowDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, LPITEM pkBow, LPITEM pkArrow, bool bIgnoreDefense)
{
	if (!pkAttacker || !pkVictim)
		return 0;

	if (!pkBow || pkBow->GetType() != ITEM_WEAPON || pkBow->GetSubType() != WEAPON_BOW)
		return 0;

	if (!pkArrow)
		return 0;

	// hit calculator
	const auto iDist = static_cast<int>(DISTANCE_SQRT(pkAttacker->GetX() - pkVictim->GetX(), pkAttacker->GetY() - pkVictim->GetY()));
	//int iGap = (iDist / 100) - 5 - pkBow->GetValue(5) - pkAttacker->GetPoint(POINT_BOW_DISTANCE);
	const auto iGap = (iDist / 100) - 5 - pkAttacker->GetPoint(POINT_BOW_DISTANCE);
	auto iPercent = 100 - (iGap * 5);

	if (iPercent <= 0)
		return 0;
	else if (iPercent > 100)
		iPercent = 100;

	int iDam = 0;

	const auto fAR = CalcAttackRating(pkAttacker, pkVictim, false);
	iDam = number(pkBow->GetValue(3), pkBow->GetValue(4)) * 2 + pkArrow->GetValue(3);
	int iAtk;

	// level must be ignored when multiply by fAR, so subtract it before calculation.
	if (pkAttacker->IsPC() && pkVictim->IsPvM())
	{
		iAtk = (pkAttacker->GetPoint(POINT_ATT_GRADE) + pkAttacker->GetPoint(POINT_ATTBONUS_PVM_STR)) + iDam - (pkAttacker->GetLevel() * 2);
	}
	else
	{
		iAtk = pkAttacker->GetPoint(POINT_ATT_GRADE) + iDam - (pkAttacker->GetLevel() * 2);
	}
	iAtk = static_cast<int>(iAtk * fAR);
	iAtk += pkAttacker->GetLevel() * 2; // and add again

	// Refine Grade
	iAtk += pkBow->GetValue(5) * 2;

	iAtk += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);
	if (pkAttacker->IsPC() && pkVictim->IsPvM())
	{
		iAtk = (int)(iAtk * (100 + (pkAttacker->GetPoint(POINT_ATT_BONUS) + pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER) + pkAttacker->GetPoint(POINT_ATTBONUS_PVM_BERSERKER))) / 100);
	}
	else
	{
		iAtk = (int)(iAtk * (100 + (pkAttacker->GetPoint(POINT_ATT_BONUS) + pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100);
	}

	iAtk = CalcAttBonus(pkAttacker, pkVictim, iAtk);

	int iDef = 0;

	if (!bIgnoreDefense)
		iDef = (pkVictim->GetPoint(POINT_DEF_GRADE) * (100 + pkAttacker->GetPoint(POINT_DEF_BONUS)) / 100);

	if (pkAttacker->IsNPC())
		iAtk = static_cast<int>(iAtk * pkAttacker->GetMobDamageMultiply());

	iDam = MAX(0, iAtk - iDef);

	int iPureDam = iDam;

	iPureDam = (iPureDam * iPercent) / 100;

	if (test_server)
	{
		pkAttacker->ChatPacket(CHAT_TYPE_INFO, "ARROW %s -> %s, DAM %d DIST %d GAP %d %% %d",
			pkAttacker->GetName(),
			pkVictim->GetName(),
			iPureDam,
			iDist, iGap, iPercent);
	}

	return iPureDam;
	//return iDam;
}

void NormalAttackAffect(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
{
	if (!pkAttacker || !pkVictim)
		return;

	// Poison attacks are unique, so special treatment
	if (pkAttacker->GetPoint(POINT_POISON_PCT) && !pkVictim->IsAffectFlag(AFF_POISON))
	{
		int poison_pct = pkAttacker->GetPoint(POINT_POISON_PCT);

		if (number(1, 100) <= poison_pct)
		{
			pkVictim->AttackedByPoison(pkAttacker);
		}
	}
#ifdef ENABLE_WOLFMAN_CHARACTER
	if (pkAttacker->GetPoint(POINT_BLEEDING_PCT) && !pkVictim->IsAffectFlag(AFF_BLEEDING))
	{
		int bleeding_pct = pkAttacker->GetPoint(POINT_BLEEDING_PCT);

		if (number(1, 100) <= bleeding_pct)
			pkVictim->AttackedByBleeding(pkAttacker);
	}
#endif


	int iStunDuration = 2;
	if (pkAttacker->IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| pkAttacker->IsBotCharacter()
#endif
		&& !pkVictim->IsPC())
		iStunDuration = 4;

	AttackAffect(pkAttacker, pkVictim, POINT_STUN_PCT, IMMUNE_STUN, AFFECT_STUN, POINT_NONE, 0, AFF_STUN, iStunDuration, "STUN");
	AttackAffect(pkAttacker, pkVictim, POINT_SLOW_PCT, IMMUNE_SLOW, AFFECT_SLOW, POINT_MOV_SPEED, -30, AFF_SLOW, 20, "SLOW");
}

int battle_hit(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, int& iRetDam)
{
	if (!pkAttacker || !pkVictim)
		return 0;

	//PROF_UNIT puHit("Hit");
	if (test_server)
		sys_log(0, "battle_hit : [%s] attack to [%s] : dam :%d", pkAttacker->GetName(), pkVictim->GetName(), iRetDam);

	int iDam = CalcMeleeDamage(pkAttacker, pkVictim);

	if (iDam <= 0)
		return (BATTLE_DAMAGE);

	NormalAttackAffect(pkAttacker, pkVictim);

	LPITEM pkWeapon = pkAttacker->GetWear(WEAR_WEAPON);

	if (pkWeapon)
		switch (pkWeapon->GetSubType())
		{
#ifdef ENABLE_NEW_BONUS_SYSTEM
			case WEAPON_SWORD:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_SWORD) - pkVictim->GetPoint(POINT_ATTBONUS_SWORD))) / 100;
				break;

			case WEAPON_TWO_HANDED:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_TWOHAND) - pkVictim->GetPoint(POINT_ATTBONUS_TWOHAND))) / 100;
				break;

			case WEAPON_DAGGER:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_DAGGER) - pkVictim->GetPoint(POINT_ATTBONUS_DAGGER))) / 100;
				break;

			case WEAPON_BELL:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_BELL) - pkVictim->GetPoint(POINT_ATTBONUS_BELL))) / 100;
				break;

			case WEAPON_FAN:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_FAN) - pkVictim->GetPoint(POINT_ATTBONUS_FAN))) / 100;
				break;

			case WEAPON_BOW:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_BOW) - pkVictim->GetPoint(POINT_ATTBONUS_BOW))) / 100;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
//				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_CLAW) - pkVictim->GetPoint(POINT_ATTBONUS_CLAW))) / 100;
//#if defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_ITEM_CLAW_AS_DAGGER)
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_DAGGER) - pkVictim->GetPoint(POINT_ATTBONUS_CLAW))) / 100;
//#endif
				break;
#endif

#else
			case WEAPON_SWORD:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_SWORD)) / 100;
				break;

			case WEAPON_TWO_HANDED:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_TWOHAND)) / 100;
				break;

			case WEAPON_DAGGER:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_DAGGER)) / 100;
				break;

			case WEAPON_BELL:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_BELL)) / 100;
				break;

			case WEAPON_FAN:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_FAN)) / 100;
				break;

			case WEAPON_BOW:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_BOW)) / 100;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
				//iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_CLAW)) / 100;
//#if defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_ITEM_CLAW_AS_DAGGER)
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_DAGGER)) / 100;
//#endif
				break;
#endif

#endif
		}

	float attMul = pkAttacker->GetAttMul();
	float tempIDam = iDam;

	iDam = attMul * tempIDam + 0.5f;

	iRetDam = iDam;

#ifdef ENABLE_QUEEN_NETHIS
	SnakeLair::CSnk::Instance().QueenDebuffAttack(pkAttacker, pkVictim);
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
	EDamageType damageType = (pkVictim->GetRaceNum() >= 6500 && pkVictim->GetRaceNum() <= 6504) ? DAMAGE_TYPE_SPECIAL : DAMAGE_TYPE_NORMAL;

	CMiniGame::instance().AttendanceMonsterAttack(pkAttacker, pkVictim);

	if (pkVictim->Damage(pkAttacker, iDam, damageType))
		return (BATTLE_DEAD);
#else
	if (pkVictim->Damage(pkAttacker, iDam, DAMAGE_TYPE_NORMAL))
		return (BATTLE_DEAD);
#endif

	return (BATTLE_DAMAGE);
}

DWORD GET_ATTACK_SPEED(LPCHARACTER ch)
{
	if (NULL == ch)
		return 1000;

	LPITEM item = ch->GetWear(WEAR_WEAPON);
	DWORD default_bonus = SPEEDHACK_LIMIT_BONUS; // Yuduri attack speed (base 80)
	DWORD riding_bonus = 0;

	if (ch->IsRiding())
	{
		// 50 bonus attack speed when riding something
		riding_bonus = 50;
	}

	DWORD ani_speed = ani_attack_speed(ch);
	DWORD real_speed = (ani_speed * 100) / (default_bonus + ch->GetPoint(POINT_ATT_SPEED) + riding_bonus);

	// Double attack speed for daggers
	if (item && item->GetSubType() == WEAPON_DAGGER)
		real_speed /= 2;
#ifdef ENABLE_WOLFMAN_CHARACTER
	else if (item && item->GetSubType() == WEAPON_CLAW)
		real_speed /= 2;
#endif

	return real_speed;
}

void SET_ATTACK_TIME(LPCHARACTER ch, LPCHARACTER victim, DWORD current_time)
{
	if (NULL == ch || NULL == victim)
		return;

	if (!ch->IsPC())
		return;

	ch->m_kAttackLog.dwVID = victim->GetVID();
	ch->m_kAttackLog.dwTime = current_time;
}

void SET_ATTACKED_TIME(LPCHARACTER ch, LPCHARACTER victim, DWORD current_time)
{
	if (NULL == ch || NULL == victim)
		return;

	if (!ch->IsPC())
		return;

	victim->m_AttackedLog.dwPID = ch->GetPlayerID();
	victim->m_AttackedLog.dwAttackedTime = current_time;
}

bool IS_SPEED_HACK(LPCHARACTER ch, LPCHARACTER victim, DWORD current_time)
{
	if (NULL == ch || NULL == victim)
		return false;

	if (!gHackCheckEnable) return false;

	if (ch->m_kAttackLog.dwVID == victim->GetVID())
	{
		if (current_time - ch->m_kAttackLog.dwTime < GET_ATTACK_SPEED(ch))
		{
			INCREASE_SPEED_HACK_COUNT(ch);

			if (test_server)
			{
				sys_log(0, "%s attack hack! time (delta, limit)=(%u, %u) hack_count %d",
					ch->GetName(),
					current_time - ch->m_kAttackLog.dwTime,
					GET_ATTACK_SPEED(ch),
					ch->m_speed_hack_count);

				ch->ChatPacket(CHAT_TYPE_INFO, "%s attack hack! time (delta, limit)=(%u, %u) hack_count %d",
					ch->GetName(),
					current_time - ch->m_kAttackLog.dwTime,
					GET_ATTACK_SPEED(ch),
					ch->m_speed_hack_count);
			}

			SET_ATTACK_TIME(ch, victim, current_time);
			SET_ATTACKED_TIME(ch, victim, current_time);

			if (ch->m_speed_hack_count > 3)	//@fixme503
				ch->SetWaitHackCounter();

			return true;
		}
	}

	SET_ATTACK_TIME(ch, victim, current_time);

	if (victim->m_AttackedLog.dwPID == ch->GetPlayerID())
	{
		if (current_time - victim->m_AttackedLog.dwAttackedTime < GET_ATTACK_SPEED(ch))
		{
			INCREASE_SPEED_HACK_COUNT(ch);

			if (test_server)
			{
				sys_log(0, "%s Attack Speed HACK! time (delta, limit)=(%u, %u), hack_count = %d",
					ch->GetName(),
					current_time - victim->m_AttackedLog.dwAttackedTime,
					GET_ATTACK_SPEED(ch),
					ch->m_speed_hack_count);

				ch->ChatPacket(CHAT_TYPE_INFO, "Attack Speed Hack(%s), (delta, limit)=(%u, %u)",
					ch->GetName(),
					current_time - victim->m_AttackedLog.dwAttackedTime,
					GET_ATTACK_SPEED(ch));
			}

			SET_ATTACKED_TIME(ch, victim, current_time);
			return true;
		}
	}

	SET_ATTACKED_TIME(ch, victim, current_time);
	return false;
}