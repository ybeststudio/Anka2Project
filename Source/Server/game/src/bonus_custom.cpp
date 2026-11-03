#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_BONUS_FIX
#include "utils.h"
#include "vector.h"
#include "config.h"
#include "constants.h"
#include "char.h"
#include "item.h"
#include "affect.h"
#include "skill.h"
#include "sectree.h"
#include "battle.h"
#include "bonus_custom.h"

namespace
{
	// wolfman (ENABLE_WOLFMAN_CHARACTER is off) and 9th skill vnums, skill.h only declares them inside #ifdefs
	enum
	{
		BONUS_SKILL_CHAYEOL = 170,
		BONUS_SKILL_SALPOONG = 171,
		BONUS_SKILL_GONGDAB = 172,
		BONUS_SKILL_PASWAE = 173,
		BONUS_SKILL_JEOKRANG = 174,
		BONUS_SKILL_CHEONGRANG = 175,
		BONUS_SKILL_ILIPUNGU = 183,
	};

	enum
	{
		// own affect slots (affect.h AFFECT_MOUNT_MELEE_MAGIC_BONUS = 920)
		AFFECT_BONUS_SKILL_ATT = 921,
		AFFECT_BONUS_SKILL_DEF,
		AFFECT_BONUS_SKILL_CAST,
		AFFECT_BONUS_SKILL_MOV,
		AFFECT_BONUS_SKILL_CRIT,
		AFFECT_BONUS_HIT_BUFF_BASE = 930,		// 12 elemental enchant / resist slots
		AFFECT_BONUS_HIT_SUNGMA_BASE = 942,		// 4 Sung Ma slots
		AFFECT_BONUS_HIT_STONE_ATT = 946,
		AFFECT_BONUS_HIT_STONE_DEF,
		AFFECT_BONUS_NO_FALLEN,
		AFFECT_BONUS_DISMOUNT_SPEED,
		AFFECT_BONUS_MOUNT_NO_KNOCKBACK,
		AFFECT_BONUS_MOUNT_NO_KNOCKBACK_CD,
		AFFECT_BONUS_AUTO_HP_CD,
		AFFECT_BONUS_AUTO_SP_CD,
	};

	// Not given by the official tooltips ("<bonus> for %d sec. with <skill>" only names the duration).
	// Estimated strength of the timed buffs, change here if the official values become known.
	const int SKILL_BUFF_ATT_BONUS = 10;		// POINT_ATT_BONUS %
	const int SKILL_BUFF_DEF_BONUS = 10;		// POINT_DEF_BONUS %
	const int SKILL_BUFF_CASTING_SPEED = 20;
	const int SKILL_BUFF_MOV_SPEED = 20;
	const int SKILL_BUFF_CRITICAL_PCT = 10;
	const int HIT_BUFF_ELEMENT_VALUE = 10;		// "Earn Power of Fire for %d sec. in combat"
	const int HIT_BUFF_SUNGMA_VALUE = 15;		// official: "by 15 for %d seconds"
	const int AUTO_RECOVERY_HP_PCT = 10;		// Sun / Moon Elixir effect: % of max HP / SP
	const int AUTO_RECOVERY_BELOW_PCT = 50;
	const int AUTO_RECOVERY_COOLTIME = 10;

	struct SSkillApply
	{
		WORD wApply;
		DWORD dwSkill;
	};

	// TOOLTIP_SKILL_DAMAGE_<skill>: "<skill> Damage +X%"
	const SSkillApply s_aSkillDamage[] =
	{
		{ APPLY_SKILL_DAMAGE_SAMYEON, SKILL_SAMYEON }, { APPLY_SKILL_DAMAGE_TANHWAN, SKILL_TANHWAN },
		{ APPLY_SKILL_DAMAGE_PALBANG, SKILL_PALBANG }, { APPLY_SKILL_DAMAGE_GIGONGCHAM, SKILL_GIGONGCHAM },
		{ APPLY_SKILL_DAMAGE_GYOKSAN, SKILL_GYOKSAN }, { APPLY_SKILL_DAMAGE_GEOMPUNG, SKILL_GEOMPUNG },
		{ APPLY_SKILL_DAMAGE_AMSEOP, SKILL_AMSEOP }, { APPLY_SKILL_DAMAGE_GUNGSIN, SKILL_GUNGSIN },
		{ APPLY_SKILL_DAMAGE_CHARYUN, SKILL_CHARYUN }, { APPLY_SKILL_DAMAGE_SANGONG, SKILL_SANGONG },
		{ APPLY_SKILL_DAMAGE_YEONSA, SKILL_YEONSA }, { APPLY_SKILL_DAMAGE_KWANKYEOK, SKILL_KWANKYEOK },
		{ APPLY_SKILL_DAMAGE_GIGUNG, SKILL_GIGUNG }, { APPLY_SKILL_DAMAGE_HWAJO, SKILL_HWAJO },
		{ APPLY_SKILL_DAMAGE_SWAERYUNG, SKILL_SWAERYUNG }, { APPLY_SKILL_DAMAGE_YONGKWON, SKILL_YONGKWON },
		{ APPLY_SKILL_DAMAGE_PABEOB, SKILL_PABEOB }, { APPLY_SKILL_DAMAGE_MARYUNG, SKILL_MARYUNG },
		{ APPLY_SKILL_DAMAGE_HWAYEOMPOK, SKILL_HWAYEOMPOK }, { APPLY_SKILL_DAMAGE_MAHWAN, SKILL_MAHWAN },
		{ APPLY_SKILL_DAMAGE_BIPABU, SKILL_BIPABU }, { APPLY_SKILL_DAMAGE_YONGBI, SKILL_YONGBI },
		{ APPLY_SKILL_DAMAGE_PAERYONG, SKILL_PAERYONG }, { APPLY_SKILL_DAMAGE_NOEJEON, SKILL_NOEJEON },
		{ APPLY_SKILL_DAMAGE_BYEURAK, SKILL_BYEURAK }, { APPLY_SKILL_DAMAGE_CHAIN, SKILL_CHAIN },
		{ APPLY_SKILL_DAMAGE_CHAYEOL, BONUS_SKILL_CHAYEOL }, { APPLY_SKILL_DAMAGE_SALPOONG, BONUS_SKILL_SALPOONG },
		{ APPLY_SKILL_DAMAGE_GONGDAB, BONUS_SKILL_GONGDAB }, { APPLY_SKILL_DAMAGE_PASWAE, BONUS_SKILL_PASWAE },
	};

	// TOOLTIP_USE_SKILL_<skill>_HP_ABSORB: "Absorbs X% HP with <skill>"
	const SSkillApply s_aHpAbsorb[] =
	{
		{ APPLY_USE_SKILL_PALBANG_HP_ABSORB, SKILL_PALBANG }, { APPLY_USE_SKILL_AMSEOP_HP_ABSORB, SKILL_AMSEOP },
		{ APPLY_USE_SKILL_YEONSA_HP_ABSORB, SKILL_YEONSA }, { APPLY_USE_SKILL_YONGBI_HP_ABSORB, SKILL_YONGBI },
		{ APPLY_USE_SKILL_CHAIN_HP_ABSORB, SKILL_CHAIN }, { APPLY_USE_SKILL_TUSOK_HP_ABSORB, SKILL_TUSOK },
		{ APPLY_USE_SKILL_PAERYONG_HP_ABSORB, SKILL_PAERYONG }, { APPLY_USE_SKILL_BYEURAK_HP_ABSORB, SKILL_BYEURAK },
		{ APPLY_USE_SKILL_CHAYEOL_HP_ABSORB, BONUS_SKILL_CHAYEOL },
	};
	const SSkillApply s_aSpAbsorb[] = { { APPLY_USE_SKILL_PASWAE_SP_ABSORB, BONUS_SKILL_PASWAE } };

	// TOOLTIP_USE_SKILL_<skill>_STUN / _KNOCKBACK: "X% stun / knockback chance with <skill>"
	const SSkillApply s_aStun[] =
	{
		{ APPLY_USE_SKILL_GIGONGCHAM_STUN, SKILL_GIGONGCHAM }, { APPLY_USE_SKILL_CHARYUN_STUN, SKILL_CHARYUN },
		{ APPLY_USE_SKILL_PABEOB_STUN, SKILL_PABEOB }, { APPLY_USE_SKILL_MAHWAN_STUN, SKILL_MAHWAN },
		{ APPLY_USE_SKILL_GONGDAB_STUN, BONUS_SKILL_GONGDAB }, { APPLY_USE_SKILL_SAMYEON_STUN, SKILL_SAMYEON },
	};
	const SSkillApply s_aKnockback[] =
	{
		{ APPLY_USE_SKILL_GYOKSAN_KNOCKBACK, SKILL_GYOKSAN }, { APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK, SKILL_SWAERYUNG },
		{ APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK, SKILL_HWAYEOMPOK }, { APPLY_USE_SKILL_GONGDAB_KNOCKBACK, BONUS_SKILL_GONGDAB },
		{ APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK, SKILL_KWANKYEOK },
	};

	// TOOLTIP_USE_SKILL_<skill>_NEXT_COOLTIME_DECREASE_10PER / 20PER: "<skill> Cooldown: -X sec. with 10% / 20% chance"
	const SSkillApply s_aCool10[] =
	{
		{ APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER, SKILL_SAMYEON }, { APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER, SKILL_GEOMPUNG },
		{ APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER, SKILL_GUNGSIN }, { APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER, SKILL_KWANKYEOK },
		{ APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER, SKILL_YONGKWON }, { APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER, SKILL_MARYUNG },
		{ APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER, SKILL_BIPABU }, { APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER, SKILL_NOEJEON },
		{ APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER, BONUS_SKILL_SALPOONG }, { APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER, BONUS_SKILL_PASWAE },
	};
	const SSkillApply s_aCool20[] =
	{
		{ APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER, SKILL_SAMYEON }, { APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER, SKILL_GEOMPUNG },
		{ APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER, SKILL_GUNGSIN }, { APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER, SKILL_KWANKYEOK },
		{ APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER, SKILL_YONGKWON }, { APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER, SKILL_MARYUNG },
		{ APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER, SKILL_BIPABU }, { APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER, SKILL_NOEJEON },
		{ APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER, BONUS_SKILL_SALPOONG }, { APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER, BONUS_SKILL_PASWAE },
	};
	// 9th skills: "When using <skill>, there is a X% chance to reduce its next cooldown by 5 seconds."
	const SSkillApply s_aCoolPct[] =
	{
		{ APPLY_USE_SKILL_FINISH_NEXT_COOLTIME_DECREASE_PCT, SKILL_FINISH }, { APPLY_USE_SKILL_ILGWANGPYO_NEXT_COOLTIME_DECREASE_PCT, SKILL_ILGWANGPYO },
		{ APPLY_USE_SKILL_PUNGLOEPO_NEXT_COOLTIME_DECREASE_PCT, SKILL_PUNGLOEPO }, { APPLY_USE_SKILL_GEOMAGGWI_NEXT_COOLTIME_DECREASE_PCT, SKILL_GEOMAGGWI },
		{ APPLY_USE_SKILL_MABEOBAGGWI_NEXT_COOLTIME_DECREASE_PCT, SKILL_MABEOBAGGWI }, { APPLY_USE_SKILL_METEO_NEXT_COOLTIME_DECREASE_PCT, SKILL_METEO },
		{ APPLY_USE_SKILL_CHEONUN_NEXT_COOLTIME_DECREASE_PCT, SKILL_CHEONUN }, { APPLY_USE_SKILL_ILIPUNGU_NEXT_COOLTIME_DECREASE_PCT, BONUS_SKILL_ILIPUNGU },
	};

	// TOOLTIP_SKILL_DURATION_INCREASE_<skill>: "<skill> Duration +X sec."
	const SSkillApply s_aDuration[] =
	{
		{ APPLY_SKILL_DURATION_INCREASE_EUNHYUNG, SKILL_EUNHYUNG }, { APPLY_SKILL_DURATION_INCREASE_GYEONGGONG, SKILL_GYEONGGONG },
		{ APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG, SKILL_GEOMKYUNG }, { APPLY_SKILL_DURATION_INCREASE_JEOKRANG, BONUS_SKILL_JEOKRANG },
	};

	// "<Attack / Defence bonus / casting speed / movement speed / critical> for X sec. with <skill>"
	struct SSkillBuff
	{
		WORD wApply;
		DWORD dwSkill;
		DWORD dwAffect;
		BYTE bPoint;
		int iValue;
	};
	const SSkillBuff s_aSkillBuff[] =
	{
		{ APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED, BONUS_SKILL_CHEONGRANG, AFFECT_BONUS_SKILL_MOV, POINT_MOV_SPEED, SKILL_BUFF_MOV_SPEED },
		{ APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED, BONUS_SKILL_CHEONGRANG, AFFECT_BONUS_SKILL_CAST, POINT_CASTING_SPEED, SKILL_BUFF_CASTING_SPEED },
		{ APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT, BONUS_SKILL_CHAYEOL, AFFECT_BONUS_SKILL_CRIT, POINT_CRITICAL_PCT, SKILL_BUFF_CRITICAL_PCT },
		{ APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS, SKILL_SANGONG, AFFECT_BONUS_SKILL_ATT, POINT_ATT_BONUS, SKILL_BUFF_ATT_BONUS },
		{ APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS, SKILL_GIGUNG, AFFECT_BONUS_SKILL_ATT, POINT_ATT_BONUS, SKILL_BUFF_ATT_BONUS },
		{ APPLY_USE_SKILL_JEOKRANG_DEF_BONUS, BONUS_SKILL_JEOKRANG, AFFECT_BONUS_SKILL_DEF, POINT_DEF_BONUS, SKILL_BUFF_DEF_BONUS },
		{ APPLY_USE_SKILL_GWIGEOM_DEF_BONUS, SKILL_GWIGEOM, AFFECT_BONUS_SKILL_DEF, POINT_DEF_BONUS, SKILL_BUFF_DEF_BONUS },
		{ APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS, SKILL_TERROR, AFFECT_BONUS_SKILL_ATT, POINT_ATT_BONUS, SKILL_BUFF_ATT_BONUS },
		{ APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS, SKILL_MUYEONG, AFFECT_BONUS_SKILL_ATT, POINT_ATT_BONUS, SKILL_BUFF_ATT_BONUS },
		{ APPLY_USE_SKILL_MANASHILED_CASTING_SPEED, SKILL_MANASHILED, AFFECT_BONUS_SKILL_CAST, POINT_CASTING_SPEED, SKILL_BUFF_CASTING_SPEED },
		{ APPLY_USE_SKILL_HOSIN_DEF_BONUS, SKILL_HOSIN, AFFECT_BONUS_SKILL_DEF, POINT_DEF_BONUS, SKILL_BUFF_DEF_BONUS },
		{ APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS, SKILL_GICHEON, AFFECT_BONUS_SKILL_ATT, POINT_ATT_BONUS, SKILL_BUFF_ATT_BONUS },
		{ APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS, SKILL_JEONGEOP, AFFECT_BONUS_SKILL_ATT, POINT_ATT_BONUS, SKILL_BUFF_ATT_BONUS },
		{ APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS, SKILL_JEUNGRYEOK, AFFECT_BONUS_SKILL_DEF, POINT_DEF_BONUS, SKILL_BUFF_DEF_BONUS },
		{ APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED, SKILL_CHUNKEON, AFFECT_BONUS_SKILL_CAST, POINT_CASTING_SPEED, SKILL_BUFF_CASTING_SPEED },
	};

	// "Earn Power of / Resistance to <element> for X sec. in combat"
	struct SHitBuff
	{
		BYTE bTrigger;	// POINT_HIT_BUFF_* (value = seconds)
		BYTE bPoint;
	};
	const SHitBuff s_aHitBuff[] =
	{
		{ POINT_HIT_BUFF_ENCHANT_FIRE, POINT_ENCHANT_FIRE }, { POINT_HIT_BUFF_ENCHANT_ICE, POINT_ENCHANT_ICE },
		{ POINT_HIT_BUFF_ENCHANT_ELEC, POINT_ENCHANT_ELECT }, { POINT_HIT_BUFF_ENCHANT_WIND, POINT_ENCHANT_WIND },
		{ POINT_HIT_BUFF_ENCHANT_DARK, POINT_ENCHANT_DARK }, { POINT_HIT_BUFF_ENCHANT_EARTH, POINT_ENCHANT_EARTH },
		{ POINT_HIT_BUFF_RESIST_FIRE, POINT_RESIST_FIRE }, { POINT_HIT_BUFF_RESIST_ICE, POINT_RESIST_ICE },
		{ POINT_HIT_BUFF_RESIST_ELEC, POINT_RESIST_ELEC }, { POINT_HIT_BUFF_RESIST_WIND, POINT_RESIST_WIND },
		{ POINT_HIT_BUFF_RESIST_DARK, POINT_RESIST_DARK }, { POINT_HIT_BUFF_RESIST_EARTH, POINT_RESIST_EARTH },
	};
	const SHitBuff s_aHitSungma[] =
	{
		{ POINT_HIT_BUFF_SUNGMA_STR, POINT_SUNGMA_STR }, { POINT_HIT_BUFF_SUNGMA_HP, POINT_SUNGMA_HP },
		{ POINT_HIT_BUFF_SUNGMA_MOVE, POINT_SUNGMA_MOVE }, { POINT_HIT_BUFF_SUNGMA_IMMUNE, POINT_SUNGMA_IMMUNE },
	};

	// monsters for "Strong against Dragons" (Beran-Setaou); mystery dungeon / moonshadow maps do not exist in Anka2 yet
	const DWORD s_adwDragonRace[] = { 2493, 2494, 2495 };
	const long s_alMysteryMap[] = { 0 };
	const long s_alMoonMap[] = { 0 };

	template <size_t N>
	int SumFor(LPCHARACTER ch, const SSkillApply (&table)[N], DWORD dwSkill)
	{
		int iSum = 0;
		for (size_t i = 0; i < N; ++i)
			if (table[i].dwSkill == dwSkill)
				iSum += ch->GetCustomApply(table[i].wApply);
		return iSum;
	}

	template <size_t N>
	bool InList(const long (&list)[N], long lValue)
	{
		for (size_t i = 0; i < N; ++i)
			if (list[i] && list[i] == lValue)
				return true;
		return false;
	}

	void Knockback(LPCHARACTER ch, LPCHARACTER pkVictim)
	{
		if (pkVictim->IsStone() || pkVictim->IsDead())
			return;
		float fx, fy;
		const float degree = GetDegreeFromPositionXY(ch->GetX(), ch->GetY(), pkVictim->GetX(), pkVictim->GetY());
		GetDeltaByDegree(degree, 300.0f, &fx, &fy);
		const long tx = static_cast<long>(pkVictim->GetX() + fx);
		const long ty = static_cast<long>(pkVictim->GetY() + fy);
		pkVictim->Sync(tx, ty);
		pkVictim->Goto(tx, ty);
		pkVictim->CalculateMoveDuration();
		pkVictim->SyncPacket();
	}

	void Heal(LPCHARACTER ch, BYTE bPoint, int iAmount)
	{
		if (iAmount <= 0 || ch->IsDead())
			return;
		if (bPoint == POINT_HP)
			iAmount = MIN(iAmount, MAX(0, ch->GetMaxHP() - ch->GetHP()));
		else
			iAmount = MIN(iAmount, MAX(0, ch->GetMaxSP() - ch->GetSP()));
		if (iAmount > 0)
			ch->PointChange(bPoint, iAmount);
	}

	// "Attack nearby monsters with lightning bolts with damage equal to your normal attacks"
	struct FuncAreaElec
	{
		LPCHARACTER m_ch;
		LPCHARACTER m_pkTarget;
		int m_iDamage;
		int m_iCount;
		void operator()(LPENTITY ent)
		{
			if (m_iCount >= 5 || !ent->IsType(ENTITY_CHARACTER))
				return;
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch == m_pkTarget || ch == m_ch || !ch->IsMonster() || ch->IsDead())
				return;
			if (DISTANCE_APPROX(ch->GetX() - m_pkTarget->GetX(), ch->GetY() - m_pkTarget->GetY()) > 400)
				return;
			++m_iCount;
			ch->Damage(m_ch, m_iDamage, DAMAGE_TYPE_ELEC);
		}
	};
	bool s_bInAreaElec = false;
}

namespace BonusCustom
{
	int GetSkillDamagePct(LPCHARACTER ch, DWORD dwSkillVnum)
	{
		if (!ch || !ch->IsPC())
			return 0;
		// APPLY_SKILL (51) adds to one skill too, it was stored but never read
		return SumFor(ch, s_aSkillDamage, dwSkillVnum) + ch->GetSkillDamageBonusPct(dwSkillVnum);
	}

	void OnSkillHit(LPCHARACTER ch, LPCHARACTER pkVictim, DWORD dwSkillVnum, int iDamage)
	{
		if (!ch || !pkVictim || !ch->IsPC() || iDamage <= 0)
			return;

		if (const int iHp = SumFor(ch, s_aHpAbsorb, dwSkillVnum))
			Heal(ch, POINT_HP, iDamage * iHp / 100);
		if (const int iSp = SumFor(ch, s_aSpAbsorb, dwSkillVnum))
			Heal(ch, POINT_SP, iDamage * iSp / 100);

		if (pkVictim->IsDead())
			return;

		if (const int iStun = SumFor(ch, s_aStun, dwSkillVnum))
			SkillAttackAffect(pkVictim, iStun * 10, IMMUNE_STUN, AFFECT_STUN, POINT_NONE, 0, AFF_STUN, 2, "BONUS_SKILL_STUN");

		if (const int iKnock = SumFor(ch, s_aKnockback, dwSkillVnum))
			if (number(1, 100) <= iKnock && !pkVictim->IsImmune(IMMUNE_FALL))
				Knockback(ch, pkVictim);
	}

	void OnSkillCooltime(LPCHARACTER ch, DWORD dwSkillVnum, int& riCooltime)
	{
		if (!ch || !ch->IsPC() || riCooltime <= 0)
			return;

		if (const int iSec = SumFor(ch, s_aCool10, dwSkillVnum))
			if (number(1, 100) <= 10)
				riCooltime -= iSec;
		if (const int iSec = SumFor(ch, s_aCool20, dwSkillVnum))
			if (number(1, 100) <= 20)
				riCooltime -= iSec;
		if (const int iPct = SumFor(ch, s_aCoolPct, dwSkillVnum))
			if (number(1, 100) <= iPct)
				riCooltime -= 5;
		riCooltime = MAX(0, riCooltime);
	}

	void OnSkillUsed(LPCHARACTER ch, DWORD dwSkillVnum)
	{
		if (!ch || !ch->IsPC())
			return;
		for (size_t i = 0; i < sizeof(s_aSkillBuff) / sizeof(s_aSkillBuff[0]); ++i)
		{
			const SSkillBuff& b = s_aSkillBuff[i];
			if (b.dwSkill != dwSkillVnum)
				continue;
			const int iSec = ch->GetCustomApply(b.wApply);
			if (iSec <= 0)
				continue;
			ch->RemoveAffect(b.dwAffect);
			ch->AddAffect(b.dwAffect, b.bPoint, b.iValue, AFF_NONE, iSec, 0, true);
		}
	}

	int GetSkillDurationBonus(LPCHARACTER ch, DWORD dwSkillVnum)
	{
		return (ch && ch->IsPC()) ? SumFor(ch, s_aDuration, dwSkillVnum) : 0;
	}

	void OnDamageDealt(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, int iDamage, int iDamageType)
	{
		if (!pkAttacker || !pkVictim || iDamage <= 0 || s_bInAreaElec)
			return;

		if (pkAttacker->IsPC())
		{
			// "Absorbs X% damage as HP / SP"
			if (const int iHp = pkAttacker->GetCustomApply(APPLY_DAMAGE_HP_RECOVERY))
				Heal(pkAttacker, POINT_HP, iDamage * iHp / 100);
			if (const int iSp = pkAttacker->GetCustomApply(APPLY_DAMAGE_SP_RECOVERY))
				Heal(pkAttacker, POINT_SP, iDamage * iSp / 100);

			// "Earn <element> for X sec. in combat" / "Sung Ma's will +15 for X sec. in combat"
			for (size_t i = 0; i < sizeof(s_aHitBuff) / sizeof(s_aHitBuff[0]); ++i)
			{
				const int iSec = pkAttacker->GetPoint(s_aHitBuff[i].bTrigger);
				const DWORD dwAffect = AFFECT_BONUS_HIT_BUFF_BASE + i;
				if (iSec > 0 && !pkAttacker->FindAffect(dwAffect))
					pkAttacker->AddAffect(dwAffect, s_aHitBuff[i].bPoint, HIT_BUFF_ELEMENT_VALUE, AFF_NONE, iSec, 0, true);
			}
			for (size_t i = 0; i < sizeof(s_aHitSungma) / sizeof(s_aHitSungma[0]); ++i)
			{
				const int iSec = pkAttacker->GetPoint(s_aHitSungma[i].bTrigger);
				const DWORD dwAffect = AFFECT_BONUS_HIT_SUNGMA_BASE + i;
				if (iSec > 0 && !pkAttacker->FindAffect(dwAffect))
					pkAttacker->AddAffect(dwAffect, s_aHitSungma[i].bPoint, HIT_BUFF_SUNGMA_VALUE, AFF_NONE, iSec, 0, true);
			}

			// "When attacking a Metin stone, you receive +X% (Strong against Metin stones) / +X defence for 5 seconds"
			if (pkVictim->IsStone())
			{
				const int iAtt = pkAttacker->GetCustomApply(APPLY_HIT_STONE_ATTBONUS_STONE);
				if (iAtt > 0 && !pkAttacker->FindAffect(AFFECT_BONUS_HIT_STONE_ATT))
					pkAttacker->AddAffect(AFFECT_BONUS_HIT_STONE_ATT, POINT_ATTBONUS_STONE, iAtt, AFF_NONE, 5, 0, true);
				const int iDef = pkAttacker->GetPoint(POINT_HIT_STONE_DEF_GRADE_BONUS);
				if (iDef > 0 && !pkAttacker->FindAffect(AFFECT_BONUS_HIT_STONE_DEF))
					pkAttacker->AddAffect(AFFECT_BONUS_HIT_STONE_DEF, POINT_DEF_GRADE, iDef, AFF_NONE, 5, 0, true);
			}

			if (pkVictim->IsMonster())
			{
				// "When attacking there is a X% chance that all the monsters standing around you will attack you"
				if (const int iAggr = pkAttacker->GetCustomApply(APPLY_MOB_HIT_MOB_AGGRESSIVE))
					if (number(1, 100) <= iAggr)
						pkAttacker->AggregateMonster();

				// "Attack nearby monsters with lightning bolts with damage equal to your normal attacks"
				if (iDamageType == DAMAGE_TYPE_NORMAL)
				{
					const int iElec = pkAttacker->GetCustomApply(APPLY_HIT_AREA_ELEC_ATTACK);
					if (iElec > 0 && number(1, 100) <= iElec && pkVictim->GetSectree())
					{
						s_bInAreaElec = true;
						FuncAreaElec f = { pkAttacker, pkVictim, iDamage, 0 };
						pkVictim->GetSectree()->ForEachAround(f);
						s_bInAreaElec = false;
					}
				}

				// "You have a X% chance to gather nearby items automatically for 30 seconds" (on a kill)
				if (pkVictim->GetHP() <= 0)
					if (const int iPick = pkAttacker->GetCustomApply(APPLY_AUTO_PICKUP))
						if (number(1, 100) <= iPick && !pkAttacker->FindAffect(AFFECT_AUTO_PICK_UP))
							pkAttacker->AddAffect(AFFECT_AUTO_PICK_UP, POINT_NONE, 0, AFF_NONE, 30, 0, false);
			}
		}

		if (pkVictim->IsPC() && !pkVictim->IsDead())
		{
			// "In combat there is a X% chance to receive Fall Resistance for 15 seconds."
			if (const int iFall = pkVictim->GetCustomApply(APPLY_NO_FALLEN))
				if (number(1, 100) <= iFall && !pkVictim->FindAffect(AFFECT_BONUS_NO_FALLEN))
					pkVictim->AddAffect(AFFECT_BONUS_NO_FALLEN, POINT_IMMUNE_FALL, 1, AFF_NONE, 15, 0, true);

			// "You receive the Sun / Moon Elixir effect during combat"
			if (pkVictim->GetPoint(POINT_HIT_AUTO_HP_RECOVERY) > 0 && !pkVictim->FindAffect(AFFECT_BONUS_AUTO_HP_CD)
				&& (long long) pkVictim->GetHP() * 100 < (long long) pkVictim->GetMaxHP() * AUTO_RECOVERY_BELOW_PCT)
			{
				Heal(pkVictim, POINT_HP, pkVictim->GetMaxHP() * AUTO_RECOVERY_HP_PCT / 100);
				pkVictim->AddAffect(AFFECT_BONUS_AUTO_HP_CD, POINT_NONE, 0, AFF_NONE, AUTO_RECOVERY_COOLTIME, 0, true);
			}
			if (pkVictim->GetPoint(POINT_HIT_AUTO_SP_RECOVERY) > 0 && !pkVictim->FindAffect(AFFECT_BONUS_AUTO_SP_CD)
				&& (long long) pkVictim->GetSP() * 100 < (long long) pkVictim->GetMaxSP() * AUTO_RECOVERY_BELOW_PCT)
			{
				Heal(pkVictim, POINT_SP, pkVictim->GetMaxSP() * AUTO_RECOVERY_HP_PCT / 100);
				pkVictim->AddAffect(AFFECT_BONUS_AUTO_SP_CD, POINT_NONE, 0, AFF_NONE, AUTO_RECOVERY_COOLTIME, 0, true);
			}
		}
	}

	// "You have a X% chance of evading death and restoring 30% of your maximum HP"
	bool TryEvadeDeath(LPCHARACTER ch)
	{
		if (!ch || !ch->IsPC())
			return false;
		const int iPct = ch->GetCustomApply(APPLY_NO_DEATH_AND_HP_RECOVERY30);
		if (iPct <= 0 || number(1, 100) > iPct)
			return false;
		ch->PointChange(POINT_HP, ch->GetMaxHP() * 30 / 100 - ch->GetHP());
		return true;
	}

	// "You have a chance to inflict damage ignoring your enemy's block (only in PvP)."
	bool IgnoreBlock(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
	{
		if (!pkAttacker || !pkVictim || !pkAttacker->IsPC() || !pkVictim->IsPC())
			return false;
		const int iPct = pkAttacker->GetCustomApply(APPLY_BLOCK_PENETRATE_PCT) + pkAttacker->GetCustomApply(APPLY_BONUS_BLOCK_PENETRATE_PCT);
		return iPct > 0 && number(1, 100) <= iPct;
	}

	// "Precision": chance to hit
	int GetHitPct(const CHARACTER* ch)
	{
		return ch ? ch->GetPoint(POINT_HIT_PCT) + ch->GetCustomApply(APPLY_BONUS_HIT_PCT) : 0;
	}

	int GetAttBonusPct(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
	{
		if (!pkAttacker || !pkVictim || !pkAttacker->IsPC())
			return 0;
		int iPct = 0;
		if (pkVictim->IsNPC())
		{
			for (size_t i = 0; i < sizeof(s_adwDragonRace) / sizeof(s_adwDragonRace[0]); ++i)
				if (pkVictim->GetRaceNum() == s_adwDragonRace[i])
					iPct += pkAttacker->GetPoint(POINT_ATTBONUS_DRAGON);
			if (InList(s_alMysteryMap, pkVictim->GetMapIndex() >= 10000 ? pkVictim->GetMapIndex() / 10000 : pkVictim->GetMapIndex()))
				iPct += pkAttacker->GetCustomApply(APPLY_ATTBONUS_MYSTERY_DUNGEON);
			if (InList(s_alMoonMap, pkVictim->GetMapIndex() >= 10000 ? pkVictim->GetMapIndex() / 10000 : pkVictim->GetMapIndex()))
				iPct += pkAttacker->GetCustomApply(APPLY_ATTBONUS_MOON);
		}
		// "Damage is increased at lower rank points" (estimated: full value at -20000 rank points)
		if (const int iAlign = pkAttacker->GetPoint(POINT_ALIGNMENT_DAMAGE_BONUS))
			if (pkAttacker->GetRealAlignment() < 0)
				iPct += iAlign * MIN(200000, -pkAttacker->GetRealAlignment()) / 200000;
		return iPct;
	}

	bool IsKnockbackBlocked(LPCHARACTER pkAttacker)
	{
		return pkAttacker && pkAttacker->FindAffect(AFFECT_BONUS_MOUNT_NO_KNOCKBACK);
	}

	bool IsExpRefused(LPCHARACTER ch)
	{
		return ch && ch->GetCustomApply(APPLY_REFUSE_EXP) > 0;
	}

	// "When a boss is defeated, your drop chance increases by X%"
	int GetKillBossDropPct(LPCHARACTER pkKiller, LPCHARACTER pkVictim)
	{
		if (!pkKiller || !pkVictim || pkVictim->GetMobRank() < MOB_RANK_BOSS)
			return 0;
		return pkKiller->GetCustomApply(APPLY_KILL_BOSS_ITEM_BONUS);
	}

	bool IsPoisonImmune(LPCHARACTER ch)
	{
		return ch && ch->GetCustomApply(APPLY_IMMUNE_POISON100) > 0;
	}

	void OnMount(LPCHARACTER ch, DWORD dwNewVnum, DWORD dwOldVnum)
	{
		if (!ch || !ch->IsPC())
			return;
		if (dwNewVnum)
		{
			// "X% chance of not knocking enemies back for 20 seconds. Cooldown: 2 min."
			const int iPct = ch->GetCustomApply(APPLY_MOUNT_NO_KNOCKBACK);
			if (iPct > 0 && !ch->FindAffect(AFFECT_BONUS_MOUNT_NO_KNOCKBACK_CD) && number(1, 100) <= iPct)
			{
				ch->AddAffect(AFFECT_BONUS_MOUNT_NO_KNOCKBACK, POINT_NONE, 0, AFF_NONE, 20, 0, true);
				ch->AddAffect(AFFECT_BONUS_MOUNT_NO_KNOCKBACK_CD, POINT_NONE, 0, AFF_NONE, 120, 0, true);
			}
		}
		else if (dwOldVnum)
		{
			// "When getting off your mount, your movement speed is increased by X% for 10 seconds"
			if (const int iSpeed = ch->GetPoint(POINT_DISMOUNT_MOVE_SPEED_BONUS_PER))
			{
				ch->RemoveAffect(AFFECT_BONUS_DISMOUNT_SPEED);
				ch->AddAffect(AFFECT_BONUS_DISMOUNT_SPEED, POINT_MOV_SPEED, iSpeed, AFF_NONE, 10, 0, true);
			}
		}
	}

	// after items: "X% Sung Ma's will" adds X% of the current value
	void OnComputePoints(LPCHARACTER ch)
	{
		if (!ch || !ch->IsPC())
			return;
		static const struct { WORD wApply; BYTE bPoint; } s_aSungmaPer[] =
		{
			{ APPLY_SUNGMA_PER_STR, POINT_SUNGMA_STR }, { APPLY_SUNGMA_PER_HP, POINT_SUNGMA_HP },
			{ APPLY_SUNGMA_PER_MOVE, POINT_SUNGMA_MOVE }, { APPLY_SUNGMA_PER_IMMUNE, POINT_SUNGMA_IMMUNE },
		};
		for (size_t i = 0; i < sizeof(s_aSungmaPer) / sizeof(s_aSungmaPer[0]); ++i)
			if (const int iPer = ch->GetCustomApply(s_aSungmaPer[i].wApply))
				ch->PointChange(s_aSungmaPer[i].bPoint, ch->GetPoint(s_aSungmaPer[i].bPoint) * iPer / 100);
	}

	// "1st..5th bonus +X%": the item's own attribute in that slot is stronger
	int GetItemAttributeBonusPct(LPITEM item, int iAttrIndex)
	{
		if (!item || iAttrIndex < 0 || iAttrIndex > 4)
			return 0;
		static const WORD s_awApply[5] = { APPLY_FIRST_ATTRIBUTE_BONUS, APPLY_SECOND_ATTRIBUTE_BONUS, APPLY_THIRD_ATTRIBUTE_BONUS,
			APPLY_FOURTH_ATTRIBUTE_BONUS, APPLY_FIFTH_ATTRIBUTE_BONUS };
		const TItemTable* pProto = item->GetProto();
		if (!pProto)
			return 0;
		int iPct = 0;
		for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			if (pProto->aApplies[i].bType == s_awApply[iAttrIndex])
				iPct += pProto->aApplies[i].lValue;
		return iPct;
	}
}
#endif
