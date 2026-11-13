/*********************************************************************
 * File Name : damage_trace.cpp
 * Author    : BestStudio
 * Version   : 1.0.0
 * Date      : 10-11-2026  
 */

#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_DAMAGE_TRACE
#include "utils.h"
#include "char.h"
#include "damage_trace.h"

#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace
{
	const size_t MAX_STEPS = 128;			// Baerim trackdamage.MAX_STEPS
	const DWORD MIN_INTERVAL_MS = 1000;	// Baerim: "at most one real server hit per second"

	struct SRequest
	{
		DWORD dwVID;
		DWORD dwRequest;
		DWORD dwLastSent;
	};

	struct SRow
	{
		std::string strKey;
		long long llBefore;
		long long llAfter;
	};

	struct SHit
	{
		bool bActive;
		DWORD dwAttackerVID;
		DWORD dwVictimVID;
		DWORD dwPID;
		DWORD dwRequest;
		BYTE bType;
		long long llCurrent;
		std::vector<SRow> vecRows;
		bool bHasFinal;
		bool bInputSeen;
		int iDisplayed;
		int iHPLoss;

		SHit() : bActive(false), dwAttackerVID(0), dwVictimVID(0), dwPID(0), dwRequest(0), bType(0), llCurrent(0), bHasFinal(false), bInputSeen(false), iDisplayed(0), iHPLoss(0) {}
	};

	std::map<DWORD, SRequest> s_mapRequest;	// player id -> watched target
	SHit s_kHit;								// the hit being recorded (the game core is single threaded)
}

namespace DamageTrace
{
	void Request(LPCHARACTER ch, const char* argument)
	{
		if (!ch || !ch->IsPC())
			return;

		char arg1[256], arg2[256];
		two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

		DWORD dwVID = 0, dwRequest = 0;
		str_to_number(dwVID, arg1);
		str_to_number(dwRequest, arg2);

		if (!dwVID)
		{
			s_mapRequest.erase(ch->GetPlayerID());
			return;
		}

		SRequest& r = s_mapRequest[ch->GetPlayerID()];
		r.dwVID = dwVID;
		r.dwRequest = dwRequest;
		r.dwLastSent = 0;
	}

	void Forget(LPCHARACTER ch)
	{
		if (ch && ch->IsPC())
			s_mapRequest.erase(ch->GetPlayerID());
	}

	void BeginHit(LPCHARACTER attacker, LPCHARACTER victim, BYTE bType)
	{
		if (s_kHit.bActive || !attacker || !victim || !attacker->IsPC() || !attacker->GetDesc())
			return;

		std::map<DWORD, SRequest>::iterator it = s_mapRequest.find(attacker->GetPlayerID());
		if (it == s_mapRequest.end() || it->second.dwVID != (DWORD) victim->GetVID())
			return;

		if (it->second.dwLastSent && get_dword_time() - it->second.dwLastSent < MIN_INTERVAL_MS)
			return;

		s_kHit = SHit();
		s_kHit.bActive = true;
		s_kHit.dwAttackerVID = attacker->GetVID();
		s_kHit.dwVictimVID = victim->GetVID();
		s_kHit.dwPID = attacker->GetPlayerID();
		s_kHit.dwRequest = it->second.dwRequest;
		// 0 normal hit, 1 skill hit, 2 arrow (Baerim TRACK_DAMAGE_HIT_NORMAL / _SKILL / _ARROW)
		s_kHit.bType = (bType == 0) ? 0 : ((bType == HIT_ARROW) ? 2 : 1);
	}

	bool IsActive(const CHARACTER* attacker, const CHARACTER* victim)
	{
		return s_kHit.bActive && attacker && victim
			&& (DWORD) attacker->GetVID() == s_kHit.dwAttackerVID && (DWORD) victim->GetVID() == s_kHit.dwVictimVID;
	}

	void Step(const CHARACTER* attacker, const CHARACTER* victim, const char* key, long long value)
	{
		if (!IsActive(attacker, victim))
			return;

		// a skill hit: the attack values computed inside the skill formula are not the hit itself, so the table
		// starts at the damage the formula produced ("Beceri formulu sonucu")
		if (!s_kHit.bInputSeen && 0 == strcmp(key, "INPUT"))
		{
			s_kHit.bInputSeen = true;
			if (s_kHit.bType == 1)
			{
				s_kHit.vecRows.clear();
				s_kHit.llCurrent = 0;
				key = "SKILL_FORMULA";
			}
		}

		if (value == s_kHit.llCurrent || s_kHit.vecRows.size() >= MAX_STEPS)
			return;

		SRow row;
		row.strKey = key;
		row.llBefore = s_kHit.llCurrent;
		row.llAfter = value;
		s_kHit.vecRows.push_back(row);
		s_kHit.llCurrent = value;
	}

	void Final(const CHARACTER* attacker, const CHARACTER* victim, int displayed, int hpLoss)
	{
		if (!IsActive(attacker, victim))
			return;

		s_kHit.bHasFinal = true;
		s_kHit.iDisplayed = displayed;
		s_kHit.iHPLoss = hpLoss;
	}

	void EndHit(LPCHARACTER attacker)
	{
		if (!s_kHit.bActive || !attacker || (DWORD) attacker->GetVID() != s_kHit.dwAttackerVID)
			return;

		s_kHit.bActive = false;
		if (s_kHit.vecRows.empty() || !attacker->GetDesc())
			return;

		std::map<DWORD, SRequest>::iterator it = s_mapRequest.find(s_kHit.dwPID);
		if (it == s_mapRequest.end())
			return;

		it->second.dwLastSent = get_dword_time();

		attacker->ChatPacket(CHAT_TYPE_COMMAND, "DamageTraceBegin %u %u %u %u",
			s_kHit.dwRequest, s_kHit.dwVictimVID, (unsigned) s_kHit.bType, (unsigned) s_kHit.vecRows.size());

		for (size_t i = 0; i < s_kHit.vecRows.size(); ++i)
		{
			const SRow& row = s_kHit.vecRows[i];
			attacker->ChatPacket(CHAT_TYPE_COMMAND, "DamageTraceRow %u %u %s %lld %lld",
				s_kHit.dwRequest, (unsigned) i, row.strKey.c_str(), row.llBefore, row.llAfter);
		}

		attacker->ChatPacket(CHAT_TYPE_COMMAND, "DamageTraceEnd %u %d %d",
			s_kHit.dwRequest, s_kHit.bHasFinal ? s_kHit.iDisplayed : (int) s_kHit.llCurrent, s_kHit.bHasFinal ? s_kHit.iHPLoss : 0);
	}
}
#endif
