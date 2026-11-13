#pragma once

// Baerim "Hasar Takibi" (damage tracker): records the steps of the damage formula for one real hit of a player
// on the target the player is watching and sends them to the client (DamageTraceBegin / Row / End commands).
#ifdef ENABLE_DAMAGE_TRACE
namespace DamageTrace
{
	// "/damage_trace <vid> <request>": watch a target (vid 0 stops)
	void Request(LPCHARACTER ch, const char* argument);
	void Forget(LPCHARACTER ch);

	// one CHARACTER::Attack call; at most one recorded hit per second
	void BeginHit(LPCHARACTER attacker, LPCHARACTER victim, BYTE bType);
	void EndHit(LPCHARACTER attacker);

	bool IsActive(const CHARACTER* attacker, const CHARACTER* victim);
	// the running value became `value` because of step `key` (no row when it did not change)
	void Step(const CHARACTER* attacker, const CHARACTER* victim, const char* key, long long value);
	// final damage of the hit and the HP the target really lost
	void Final(const CHARACTER* attacker, const CHARACTER* victim, int displayed, int hpLoss);

	enum { HIT_ARROW = 255 };	// FHitScope bType for a normal bow shot

	struct FHitScope
	{
		LPCHARACTER m_ch;
		FHitScope(LPCHARACTER attacker, LPCHARACTER victim, BYTE bType) : m_ch(attacker) { BeginHit(attacker, victim, bType); }
		~FHitScope() { EndHit(m_ch); }
	};
}

#define DAMAGE_TRACE_STEP(attacker, victim, key, value) \
	do { if (DamageTrace::IsActive((attacker), (victim))) DamageTrace::Step((attacker), (victim), (key), (long long) (value)); } while (0)
#else
#define DAMAGE_TRACE_STEP(attacker, victim, key, value) do { } while (0)
#endif
