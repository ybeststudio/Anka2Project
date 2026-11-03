#pragma once

#ifdef ENABLE_BONUS_FIX
// Official item bonuses without a character point (APPLY_SKILL_DAMAGE_<skill>, APPLY_USE_SKILL_*, ...) and the
// "in combat" / "for N seconds" bonuses. Behaviour after the official GF tooltips (locale_game.txt TOOLTIP_*).
// The values are summed per character in CHARACTER::m_mapCustomApply (char.cpp ApplyPoint).
namespace BonusCustom
{
	// skills
	int GetSkillDamagePct(LPCHARACTER ch, DWORD dwSkillVnum);
	void OnSkillHit(LPCHARACTER ch, LPCHARACTER pkVictim, DWORD dwSkillVnum, int iDamage);
	void OnSkillCooltime(LPCHARACTER ch, DWORD dwSkillVnum, int& riCooltime);
	void OnSkillUsed(LPCHARACTER ch, DWORD dwSkillVnum);
	int GetSkillDurationBonus(LPCHARACTER ch, DWORD dwSkillVnum);

	// combat
	void OnDamageDealt(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, int iDamage, int iDamageType);
	bool TryEvadeDeath(LPCHARACTER ch);
	bool IgnoreBlock(LPCHARACTER pkAttacker, LPCHARACTER pkVictim);
	int GetHitPct(const CHARACTER* ch);
	int GetAttBonusPct(LPCHARACTER pkAttacker, LPCHARACTER pkVictim);
	bool IsKnockbackBlocked(LPCHARACTER pkAttacker);

	// misc
	bool IsExpRefused(LPCHARACTER ch);
	int GetKillBossDropPct(LPCHARACTER pkKiller, LPCHARACTER pkVictim);
	bool IsPoisonImmune(LPCHARACTER ch);
	void OnMount(LPCHARACTER ch, DWORD dwNewVnum, DWORD dwOldVnum);
	void OnComputePoints(LPCHARACTER ch);
	int GetItemAttributeBonusPct(LPITEM item, int iAttrIndex);
}
#endif
