#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_BUFFI_SYSTEM
#include "buffi_system.h"
#include "utils.h"
#include "config.h"
#include "constants.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "affect.h"
#include "skill.h"
#include "sectree.h"
#include "packet.h"
#include "desc.h"
#include "locale.hpp"
#include "vector.h"
#include "db.h"
#include "desc_client.h"

namespace
{
	const DWORD s_adwBuffiSkill[BUFFI_SKILL_COUNT] = { SKILL_HOSIN, SKILL_REFLECT, SKILL_GICHEON, SKILL_KWAESOK, SKILL_JEUNGRYEOK };
	// Level cap before the next evolution (wiki: evolutions at 55 / 75 / 90).
	const int s_aiLevelCap[BUFFI_MAX_EVOLUTION + 1] = { 55, 75, 90, BUFFI_MAX_LEVEL };
	const int s_aiEvolutionInt[BUFFI_MAX_EVOLUTION + 1] = { 120, 140, 160, 180 };

	const float BUFFI_FOLLOW_DISTANCE = 250.0f;
	const float BUFFI_RUN_DISTANCE = 900.0f;
	const float BUFFI_RESPAWN_DISTANCE = 4500.0f;
	const int BUFFI_APPROACH = 180;
	const int BUFFI_CAST_RANGE = 1000;
	const DWORD BUFFI_CAST_INTERVAL = 1500;		// ms between two casts
	const int BUFFI_EXP_SHARE_PCT = 50;			// share of the owner's kill exp

	std::string Flag(const char* key) { return std::string("buffi.") + key; }
}

EVENTINFO(buffi_event_info)
{
	CBuffiActor* pActor;
	buffi_event_info() : pActor(NULL) {}
};

EVENTFUNC(buffi_update_event)
{
	buffi_event_info* info = dynamic_cast<buffi_event_info*>(event->info);
	if (!info || !info->pActor)
		return 0;

	info->pActor->Update();
	return PASSES_PER_SEC(1) / 4;
}

CBuffiActor::CBuffiActor(LPCHARACTER owner) : m_pkOwner(owner), m_pkChar(NULL), m_pkUpdateEvent(NULL),
	m_strName("Buffi"), m_iLevel(1), m_dwExp(0), m_iEvolution(0), m_iSex(0), m_iDurationEnd(0), m_bDurationPerm(false),
	m_iSkillPoints(0), m_dwNextCastTime(0), m_dwLastSendTime(0), m_bDirty(false)
{
	memset(m_aiRune, 0, sizeof(m_aiRune));
	memset(m_aiSkillLevel, 0, sizeof(m_aiSkillLevel));
	memset(m_adwNextCast, 0, sizeof(m_adwNextCast));
	memset(m_adwCostume, 0, sizeof(m_adwCostume));
	memset(m_abRecast, 0, sizeof(m_abRecast));
	memset(m_adwSkillColor, 0, sizeof(m_adwSkillColor));
	memset(m_aiBookProgress, 0, sizeof(m_aiBookProgress));
	m_bKeepBuffs = false;
	Load();
}

CBuffiActor::~CBuffiActor()
{
	Unsummon(false);
}

void CBuffiActor::Load()
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"SELECT name, sex, level, exp, evolution, duration_end, duration_perm, skill_points, "
		"rune0, rune1, rune2, skill0, skill1, skill2, skill3, skill4, "
		"costume_body, costume_hair, costume_weapon, costume_sash, skill_color, skill_book "
		"FROM player.player_buffi WHERE pid = %u", m_pkOwner->GetPlayerID()));

	if (!pMsg->Get() || pMsg->Get()->uiNumRows == 0)
	{
		if (!LoadFromQuestFlags())
		{
			// First seal: level 1, starting duration, skills not learnt yet: like the reference, opening a skill
			// costs a point too (0 -> M1 = 17 points).
			m_iDurationEnd = get_global_time() + BUFFI_START_DURATION;
		}
		Save();
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	int col = 0;
	if (row[col] && *row[col])
		m_strName = row[col];
	++col;
	m_iSex = atoi(row[col++]) ? 1 : 0;
	m_iLevel = MINMAX(1, atoi(row[col++]), (int) BUFFI_MAX_LEVEL);
	m_dwExp = strtoul(row[col++], NULL, 10);
	m_iEvolution = MINMAX(0, atoi(row[col++]), (int) BUFFI_MAX_EVOLUTION);
	m_iDurationEnd = atoi(row[col++]);
	m_bDurationPerm = atoi(row[col++]) != 0;
	m_iSkillPoints = MAX(0, atoi(row[col++]));
	for (int i = 0; i < BUFFI_RUNE_COUNT; ++i)
		m_aiRune[i] = atoi(row[col++]);
	for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
		m_aiSkillLevel[i] = MINMAX(0, atoi(row[col++]), (int) SKILL_MAX_LEVEL);
	for (int i = 0; i < BUFFI_COSTUME_MAX; ++i)
		m_adwCostume[i] = strtoul(row[col++], NULL, 10);

	// "c,c,c,..." 5 skills x 5 layers
	const char* p = row[col++];
	for (int i = 0; p && *p && i < BUFFI_SKILL_COUNT * 5; ++i)
	{
		m_adwSkillColor[i / 5][i % 5] = strtoul(p, NULL, 10);
		p = strchr(p, ',');
		if (p)
			++p;
	}

	p = row[col++];
	for (int i = 0; p && *p && i < BUFFI_SKILL_COUNT; ++i)
	{
		m_aiBookProgress[i] = atoi(p);
		p = strchr(p, ',');
		if (p)
			++p;
	}
}

bool CBuffiActor::LoadFromQuestFlags()
{
	// Characters from the first version kept the Buffi in "buffi.*" quest flags.
	if (!m_pkOwner->GetQuestFlag(Flag("init")))
		return false;

	m_iLevel = MINMAX(1, m_pkOwner->GetQuestFlag(Flag("level")), (int) BUFFI_MAX_LEVEL);
	m_dwExp = (DWORD) m_pkOwner->GetQuestFlag(Flag("exp"));
	m_iEvolution = MINMAX(0, m_pkOwner->GetQuestFlag(Flag("evo")), (int) BUFFI_MAX_EVOLUTION);
	m_iSex = m_pkOwner->GetQuestFlag(Flag("sex")) ? 1 : 0;
	m_iDurationEnd = m_pkOwner->GetQuestFlag(Flag("dur_end"));
	m_bDurationPerm = m_pkOwner->GetQuestFlag(Flag("dur_perm")) != 0;
	m_iSkillPoints = m_pkOwner->GetQuestFlag(Flag("pts"));

	char key[32];
	for (int i = 0; i < BUFFI_RUNE_COUNT; ++i)
	{
		snprintf(key, sizeof(key), "rune%d", i);
		m_aiRune[i] = m_pkOwner->GetQuestFlag(Flag(key));
	}
	for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
	{
		snprintf(key, sizeof(key), "sk%d", i);
		m_aiSkillLevel[i] = MINMAX(0, m_pkOwner->GetQuestFlag(Flag(key)), (int) SKILL_MAX_LEVEL);
	}
	return true;
}

void CBuffiActor::Save()
{
	char szName[CHARACTER_NAME_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szName, sizeof(szName), m_strName.c_str(), m_strName.length());

	char szColor[BUFFI_SKILL_COUNT * 5 * 11 + 1];
	int len = 0;
	for (int i = 0; i < BUFFI_SKILL_COUNT * 5; ++i)
		len += snprintf(szColor + len, sizeof(szColor) - len, i ? ",%u" : "%u", m_adwSkillColor[i / 5][i % 5]);

	char szBook[BUFFI_SKILL_COUNT * 4 + 1];
	snprintf(szBook, sizeof(szBook), "%d,%d,%d,%d,%d", m_aiBookProgress[0], m_aiBookProgress[1], m_aiBookProgress[2],
		m_aiBookProgress[3], m_aiBookProgress[4]);

	DBManager::instance().Query(
		"REPLACE INTO player.player_buffi (pid, name, sex, level, exp, evolution, duration_end, duration_perm, skill_points, "
		"rune0, rune1, rune2, skill0, skill1, skill2, skill3, skill4, "
		"costume_body, costume_hair, costume_weapon, costume_sash, skill_color, skill_book) VALUES "
		"(%u, '%s', %d, %d, %u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %u, %u, %u, %u, '%s', '%s')",
		m_pkOwner->GetPlayerID(), szName, m_iSex, m_iLevel, m_dwExp, m_iEvolution, m_iDurationEnd, m_bDurationPerm ? 1 : 0,
		m_iSkillPoints, m_aiRune[0], m_aiRune[1], m_aiRune[2],
		m_aiSkillLevel[0], m_aiSkillLevel[1], m_aiSkillLevel[2], m_aiSkillLevel[3], m_aiSkillLevel[4],
		m_adwCostume[BUFFI_COSTUME_BODY], m_adwCostume[BUFFI_COSTUME_HAIR], m_adwCostume[BUFFI_COSTUME_WEAPON], m_adwCostume[BUFFI_COSTUME_SASH],
		szColor, szBook);
	m_bDirty = false;
}

int CBuffiActor::GetLevelCap() const
{
	return s_aiLevelCap[MINMAX(0, m_iEvolution, (int) BUFFI_MAX_EVOLUTION)];
}

DWORD CBuffiActor::GetNeedExp() const
{
	if (m_iLevel >= BUFFI_MAX_LEVEL || !exp_table)
		return 0;
	return MAX(1, exp_table[m_iLevel] / 4);
}

bool CBuffiActor::IsExpired() const
{
	return !m_bDurationPerm && m_iDurationEnd <= get_global_time();
}

int CBuffiActor::GetRemainDuration() const
{
	if (m_bDurationPerm)
		return -1;
	return MAX(0, m_iDurationEnd - get_global_time());
}

int CBuffiActor::GetRuneValue(BYTE idx) const
{
	// Sent to the client: -1 permanent, 0 empty, otherwise remaining seconds.
	const int v = m_aiRune[idx];
	if (v <= 0)
		return v;
	return MAX(0, v - get_global_time());
}

void CBuffiActor::ExpireRunes()
{
	const int now = get_global_time();
	for (int i = 0; i < BUFFI_RUNE_COUNT; ++i)
	{
		if (m_aiRune[i] > 0 && m_aiRune[i] <= now)
		{
			m_aiRune[i] = 0;
			m_bDirty = true;
			RequestRecast();
		}
	}
}

int CBuffiActor::GetIntelligence() const
{
	// __LoadBuffiWindow INT table: default 120, evolution 1/2/3 -> 140/160/180; +3 per ore (max 189).
	int iInt = s_aiEvolutionInt[MINMAX(0, m_iEvolution, (int) BUFFI_MAX_EVOLUTION)];
	const int now = get_global_time();
	for (int i = 0; i < BUFFI_RUNE_COUNT; ++i)
		if (m_aiRune[i] < 0 || m_aiRune[i] > now)	// permanent, or timed and not run out yet
			iInt += BUFFI_ORE_INT;
	return iInt;
}

void CBuffiActor::ApplyStats()
{
	if (!m_pkChar)
		return;
	m_pkChar->SetLevel((BYTE) m_iLevel);
	m_pkChar->SetPoint(POINT_IQ, GetIntelligence());
	// The skill formulas also use dex (Blessing, Swiftness). The reference values (Blessing M1 22%,
	// Swiftness 1 +1 movement) both need a dex of exactly 1; the level-45 mob stats gave more.
	m_pkChar->SetPoint(POINT_DX, 1);
}

void CBuffiActor::ApplyLook()
{
	if (!m_pkChar)
		return;

	static const BYTE s_abWear[BUFFI_COSTUME_MAX] = { WEAR_BUFFI_ARMOR, WEAR_BUFFI_HAIR, WEAR_BUFFI_WEAPON, WEAR_BUFFI_ACCE };
	for (int i = 0; i < BUFFI_COSTUME_MAX; ++i)
	{
		LPITEM pkCostume = m_pkOwner->GetWear(s_abWear[i]);
		m_adwCostume[i] = pkCostume ? pkCostume->GetVnum() : 0;
	}

	const DWORD dwArmor = m_adwCostume[BUFFI_COSTUME_BODY] ? m_adwCostume[BUFFI_COSTUME_BODY] : (DWORD) BUFFI_DEFAULT_ARMOR;
	const DWORD dwWeapon = m_adwCostume[BUFFI_COSTUME_WEAPON] ? m_adwCostume[BUFFI_COSTUME_WEAPON] : (DWORD) BUFFI_DEFAULT_WEAPON;
	m_pkChar->SetPart(PART_MAIN, (WORD) dwArmor);
	m_pkChar->SetPart(PART_WEAPON, (WORD) dwWeapon);

	// same part values as a player's costume (CItem::ModifyPoints): hair = the hair model (value 3),
	// sash = vnum - ACCE_BASE_VNUM (+ ACCE_EFFECT_VNUM from ACCE_EFFECT_FROM_ABS absorption)
	LPITEM pkHair = m_pkOwner->GetWear(WEAR_BUFFI_HAIR);
	m_pkChar->SetPart(PART_HAIR, (WORD) (pkHair ? pkHair->GetValue(3) : 0));
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	LPITEM pkAcce = m_pkOwner->GetWear(WEAR_BUFFI_ACCE);
	DWORD dwAcce = 0;
	if (pkAcce)
	{
		dwAcce = pkAcce->GetVnum() - ACCE_BASE_VNUM;
		if (pkAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
			dwAcce += ACCE_EFFECT_VNUM;
	}
	m_pkChar->SetPart(PART_ACCE, (WORD) dwAcce);
#endif
}

void CBuffiActor::ApplySkillColor(int iCastIdx)
{
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	if (!m_pkChar)
		return;

	// Client (CInstanceBase::ChangeSkillColor): the effects of 94/95/96/110/111 take their colour from the
	// buff slots BUFF_BEGIN+0..4, so the Buffi's own casts are coloured from there.
	DWORD adwData[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memset(adwData, 0, sizeof(adwData));
	for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
		for (int l = 0; l < 5 && l < ESkillColorLength::MAX_EFFECT_COUNT; ++l)
			adwData[ESkillColorLength::BUFF_BEGIN + i][l] = m_adwSkillColor[i][l];

	// CHARACTER::ComputeSkill copies the caster's skill slot (94->3, 95->4, 96->5, 110->4, 111->5) into the
	// target's buff slot: put the colour of the skill being cast there.
	static const BYTE s_abCasterSlot[BUFFI_SKILL_COUNT] = { 3, 4, 5, 4, 5 };
	if (iCastIdx >= 0 && iCastIdx < BUFFI_SKILL_COUNT)
		for (int l = 0; l < 5 && l < ESkillColorLength::MAX_EFFECT_COUNT; ++l)
			adwData[s_abCasterSlot[iCastIdx]][l] = m_adwSkillColor[iCastIdx][l];

	m_pkChar->SetSkillColor(adwData[0]);
#endif
}

void CBuffiActor::GiveOwnerBuffColor(int idx)
{
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	// Same as CHARACTER::UseSkill does for a player buff: the target keeps the caster's colour in its buff slot
	// (94/95/96/110/111 -> BUFF_BEGIN+0..4), so the buff effect on the owner shows the Buffi colour.
	DWORD adwData[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memcpy(adwData, m_pkOwner->GetSkillColor(), sizeof(adwData));

	bool bChanged = false;
	for (int l = 0; l < 5 && l < ESkillColorLength::MAX_EFFECT_COUNT; ++l)
	{
		if (adwData[ESkillColorLength::BUFF_BEGIN + idx][l] != m_adwSkillColor[idx][l])
		{
			adwData[ESkillColorLength::BUFF_BEGIN + idx][l] = m_adwSkillColor[idx][l];
			bChanged = true;
		}
	}
	if (!bChanged)
		return;

	m_pkOwner->SetSkillColor(adwData[0]);
	m_pkOwner->UpdatePacket();

	TSkillColor GDPacket;
	memcpy(GDPacket.dwSkillColor, adwData, sizeof(adwData));
	GDPacket.dwPlayerID = m_pkOwner->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_SKILL_COLOR_SAVE, 0, sizeof(TSkillColor));
	db_clientdesc->Packet(&GDPacket, sizeof(TSkillColor));
#endif
}

void CBuffiActor::SetSkillColor(BYTE idx, const DWORD* adwColor)
{
	if (idx >= BUFFI_SKILL_COUNT)
		return;

	for (int l = 0; l < 5; ++l)
		m_adwSkillColor[idx][l] = adwColor[l];

	Save();
	SendSkillColors();
	if (m_pkChar)
	{
		ApplySkillColor(-1);
		m_pkChar->UpdatePacket();
		RequestRecast();	// show the new colour on the owner's buffs right away
	}
}

void CBuffiActor::SendSkillColors()
{
	for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "SetSupportSkillColor %d %u %u %u %u %u", i,
			m_adwSkillColor[i][0], m_adwSkillColor[i][1], m_adwSkillColor[i][2], m_adwSkillColor[i][3], m_adwSkillColor[i][4]);
}

void CBuffiActor::RefreshLook()
{
	if (!m_pkChar)
		return;
	ApplyLook();
	m_pkChar->UpdatePacket();
}

void CBuffiActor::SendUseSkill(DWORD dwVnum, int iLevel)
{
	// The client plays the shaman skill motion + effects on the Buffi (NEW_UseSkill).
	TPacketGCBuffiUseSkill pack;
	pack.bHeader = HEADER_GC_BUFFI_USE_SKILL;
	pack.dwVID = m_pkChar->GetVID();
	pack.dwSkillVnum = dwVnum;
	pack.bSkillLevel = (BYTE) iLevel;
	m_pkChar->PacketAround(&pack, sizeof(pack));
}

void CBuffiActor::RequestRecast()
{
	// The buffs scale with the Buffi's INT: refresh them right away instead of waiting for them to run out.
	for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
	{
		m_abRecast[i] = true;
		m_adwNextCast[i] = 0;
	}
}

void CBuffiActor::CancelUpdateEvent()
{
	if (m_pkUpdateEvent)
		event_cancel(&m_pkUpdateEvent);
	m_pkUpdateEvent = NULL;
}

bool CBuffiActor::Summon()
{
	if (m_pkChar)
		return true;

	if (IsExpired())
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's duration has expired. Use a Buffi Duration to revive it."));
		return false;
	}

	if (!m_pkOwner->GetSectree())
		return false;

	const long x = m_pkOwner->GetX() + number(-100, 100);
	const long y = m_pkOwner->GetY() + number(-100, 100);

	m_pkChar = CHARACTER_MANAGER::instance().SpawnMob(m_iSex ? BUFFI_NPC_MALE : BUFFI_NPC_FEMALE,
		m_pkOwner->GetMapIndex(), x, y, m_pkOwner->GetZ(), false, (int)(m_pkOwner->GetRotation() + 180), false);

	if (!m_pkChar)
	{
		sys_err("[Buffi] cannot spawn npc for %s", m_pkOwner->GetName());
		return false;
	}

	m_pkChar->SetPet();
	m_pkChar->SetName(m_strName);
	m_pkChar->SetEmpire(m_pkOwner->GetEmpire());
	ApplyStats();
	ApplyLook();
	ApplySkillColor(-1);
	m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y, m_pkOwner->GetZ());

	memset(m_adwNextCast, 0, sizeof(m_adwNextCast));
	m_dwNextCastTime = get_dword_time() + 1000;

	buffi_event_info* info = AllocEventInfo<buffi_event_info>();
	info->pActor = this;
	m_pkUpdateEvent = event_create(buffi_update_event, info, PASSES_PER_SEC(1) / 4);

	m_pkOwner->SetQuestFlag(Flag("summoned"), 1);
	SendAll();
	return true;
}

void CBuffiActor::Unsummon(bool bRemoveBuffs)
{
	CancelUpdateEvent();

	if (m_bDirty)
		Save();

	if (!m_pkChar)
		return;

	M2_DESTROY_CHARACTER(m_pkChar);
	m_pkChar = NULL;

	// itemdesc: "Manually unequipping Buffi removes the skill effects it granted."
	if (bRemoveBuffs)
	{
		for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
			m_pkOwner->RemoveAffect(s_adwBuffiSkill[i]);
		m_pkOwner->SetQuestFlag(Flag("summoned"), 0);
	}

	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "SupportLeave");
}

void CBuffiActor::OnCharacterDestroyed()
{
	// The NPC was destroyed elsewhere (map/dungeon close, shutdown): forget it without destroying it again.
	CancelUpdateEvent();
	m_pkChar = NULL;
}

void CBuffiActor::Follow()
{
	const long ownerX = m_pkOwner->GetX();
	const long ownerY = m_pkOwner->GetY();
	const float fDist = DISTANCE_APPROX(m_pkChar->GetX() - ownerX, m_pkChar->GetY() - ownerY);

	if (fDist >= BUFFI_RESPAWN_DISTANCE)
	{
		const float fRot = m_pkOwner->GetRotation() * 3.141592f / 180.f;
		m_pkChar->Show(m_pkOwner->GetMapIndex(), ownerX - (long)(BUFFI_APPROACH * cos(fRot)), ownerY - (long)(BUFFI_APPROACH * sin(fRot)));
		return;
	}

	if (fDist < BUFFI_FOLLOW_DISTANCE)
		return;

	m_pkChar->SetNowWalking(fDist < BUFFI_RUN_DISTANCE);
	m_pkChar->SetRotationToXY(ownerX, ownerY);

	float fx, fy;
	GetDeltaByDegree(m_pkChar->GetRotation(), fDist - BUFFI_APPROACH, &fx, &fy);
	if (m_pkChar->Goto((long)(m_pkChar->GetX() + fx + 0.5f), (long)(m_pkChar->GetY() + fy + 0.5f)))
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);
}

bool CBuffiActor::TryCastBuffs()
{
	const DWORD now = get_dword_time();
	if (now < m_dwNextCastTime)
		return false;

	if (m_pkChar->IsStateMove() || m_pkOwner->IsDead())
		return false;

	if (DISTANCE_APPROX(m_pkChar->GetX() - m_pkOwner->GetX(), m_pkChar->GetY() - m_pkOwner->GetY()) > BUFFI_CAST_RANGE)
		return false;

	for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
	{
		const int iLevel = m_aiSkillLevel[i];
		if (iLevel <= 0 || now < m_adwNextCast[i])
			continue;

		const DWORD dwVnum = s_adwBuffiSkill[i];
		if (!m_abRecast[i] && m_pkOwner->FindAffect(dwVnum))
			continue;

		CSkillProto* pkSk = CSkillManager::instance().Get(dwVnum);
		if (!pkSk)
			continue;

		m_pkChar->SetRotationToXY(m_pkOwner->GetX(), m_pkOwner->GetY());
		SendUseSkill(dwVnum, iLevel);
		m_abRecast[i] = false;

		m_pkChar->ComputeSkill(dwVnum, m_pkOwner, (BYTE) iLevel);
		GiveOwnerBuffColor(i);

		// Reference Buffi Swiftness: movement speed takes the casting speed value (level 1: 1/1, level 4: 2/2).
		// Only the Buffi's cast is changed; the players' Swiftness keeps its own movement formula.
		if (dwVnum == SKILL_KWAESOK)
		{
			CAffect* pkMov = m_pkOwner->FindAffect(SKILL_KWAESOK, POINT_MOV_SPEED);
			const CAffect* pkCast = m_pkOwner->FindAffect(SKILL_KWAESOK, POINT_CASTING_SPEED);
			if (pkMov && pkCast && pkMov->lApplyValue != pkCast->lApplyValue)
			{
				const long lValue = pkCast->lApplyValue;
				const long lDuration = pkMov->lDuration;
				const DWORD dwFlag = pkMov->dwFlag;
				m_pkOwner->RemoveAffect(pkMov);
				m_pkOwner->AddAffect(SKILL_KWAESOK, POINT_MOV_SPEED, lValue, dwFlag, lDuration, 0, false);
			}
		}

		const float k = 1.0f * m_pkChar->GetSkillPower(dwVnum, (BYTE) iLevel) * pkSk->bMaxLevel / 100;
		pkSk->kCooldownPoly.SetVar("k", k);
		const int iCooltime = MAX(1, (int) pkSk->kCooldownPoly.Eval());
		m_adwNextCast[i] = now + iCooltime * 1000;
		m_dwNextCastTime = now + BUFFI_CAST_INTERVAL;

		SendSkills();
		SendCooldowns();
		return true;
	}
	return false;
}

void CBuffiActor::Update()
{
	if (!m_pkChar)
		return;

	if (!CBuffiManager::IsSealEquipped(m_pkOwner))
	{
		Unsummon(true);
		return;
	}

	if (m_pkOwner->IsDead())
		return;

	if (IsExpired())
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's duration has expired. Use a Buffi Duration to revive it."));
		Unsummon(true);
		return;
	}

	ExpireRunes();
	ApplyStats();
	Follow();
	TryCastBuffs();

	const DWORD now = get_dword_time();
	if (now - m_dwLastSendTime >= 60 * 1000)
	{
		SendData();
		if (m_bDirty)
			Save();
	}
}

void CBuffiActor::OnOwnerGainExp(int iExp)
{
	if (!m_pkChar || iExp <= 0)
		return;

	if (m_iLevel >= GetLevelCap())
		return;

	int iGain = iExp * BUFFI_EXP_SHARE_PCT / 100;
	if (m_pkOwner->GetQuestFlag(Flag("exp_ring_end")) > get_global_time())
		iGain *= 2;
	if (iGain <= 0)
		return;

	m_dwExp += iGain;
	bool bLevelUp = false;
	while (m_iLevel < GetLevelCap() && GetNeedExp() && m_dwExp >= GetNeedExp())
	{
		m_dwExp -= GetNeedExp();
		++m_iLevel;
		++m_iSkillPoints;
		bLevelUp = true;
	}

	if (m_iLevel >= GetLevelCap())
	{
		m_dwExp = 0;
		if (bLevelUp)
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi has reached its maximum. Evolve it to keep progressing it."));
	}

	m_bDirty = true;
	if (bLevelUp)
	{
		ApplyStats();
		Save();
		m_pkChar->ViewReencode();	// refresh the "Lv N" name tag
		m_pkChar->EffectPacket(SE_SUCCESS);
		SendSkills();
	}
	SendData();
}

bool CBuffiActor::RaiseSkill(BYTE idx)
{
	if (idx >= BUFFI_SKILL_COUNT)
		return false;

	// Points take a skill from 0 to M1 (17 points); M -> G and G -> P need books.
	int& lv = m_aiSkillLevel[idx];
	if (lv >= 20)
	{
#ifdef ENABLE_BUFFI_SKILL_BOOK
		return ReadSkillBook(idx);
#else
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a Buffi skill book to train this skill further."));
		return false;
#endif
	}
	if (m_iSkillPoints <= 0)
		return false;

	lv = (lv >= 16) ? 20 : lv + 1;
	--m_iSkillPoints;
	Save();
	SendSkills();
	return true;
}

#ifdef ENABLE_BUFFI_SKILL_BOOK
// Reference: M1 -> G1 = 55 Buffi books (M1 needs 1 book, M2 needs 2, ... M10 needs 10) or one Mastery Book;
// G1 -> P = 10 Soul Stones (one per level) or one Mastery Stone.
bool CBuffiActor::ReadSkillBook(BYTE idx)
{
#ifdef ENABLE_SKILL_UPGRADE
	// books and soul stones are read in the Skill Upgrader (read delay, chance, scrolls)
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Use the Skill Upgrader to train this skill further."));
	return false;
#else
	int& lv = m_aiSkillLevel[idx];
	if (lv >= 40)
		return false;

	if (lv < 30)
	{
		if (m_pkOwner->CountSpecifyItem(BUFFI_SKILL_BOOK_VNUM) > 0)
		{
			m_pkOwner->RemoveSpecifyItem(BUFFI_SKILL_BOOK_VNUM, 1);
			const int iNeed = lv - 19;	// M1 -> 1 book ... M10 -> 10 books
			if (++m_aiBookProgress[idx] >= iNeed)
			{
				m_aiBookProgress[idx] = 0;
				++lv;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's skill has improved."));
			}
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You read a Buffi skill book (%d/%d)."), m_aiBookProgress[idx], iNeed);
		}
		else if (m_pkOwner->CountSpecifyItem(BUFFI_MASTER_BOOK_VNUM) > 0)
		{
			m_pkOwner->RemoveSpecifyItem(BUFFI_MASTER_BOOK_VNUM, 1);
			m_aiBookProgress[idx] = 0;
			lv = 30;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's skill has improved."));
		}
		else
		{
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a Buffi skill book to train this skill further."));
			return false;
		}
	}
	else
	{
		if (m_pkOwner->CountSpecifyItem(BUFFI_SOUL_STONE_VNUM) > 0)
		{
			m_pkOwner->RemoveSpecifyItem(BUFFI_SOUL_STONE_VNUM, 1);
			++lv;
		}
		else if (m_pkOwner->CountSpecifyItem(BUFFI_MASTER_STONE_VNUM) > 0)
		{
			m_pkOwner->RemoveSpecifyItem(BUFFI_MASTER_STONE_VNUM, 1);
			lv = 40;
		}
		else
		{
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a Soul Stone to train this skill further."));
			return false;
		}
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's skill has improved."));
	}

	if (m_pkChar)
		m_pkChar->EffectPacket(SE_SUCCESS);
	RequestRecast();
	Save();
	SendSkills();
	return true;
#endif
}
#endif

#if defined(ENABLE_SKILL_UPGRADE) && defined(ENABLE_BUFFI_SKILL_BOOK)
DWORD CBuffiActor::GetSkillVnum(int idx)
{
	return idx >= 0 && idx < BUFFI_SKILL_COUNT ? s_adwBuffiSkill[idx] : 0;
}

void CBuffiActor::SetBookProgress(BYTE idx, int iProgress)
{
	if (idx >= BUFFI_SKILL_COUNT)
		return;
	m_aiBookProgress[idx] = iProgress;
	Save();
}

void CBuffiActor::OnBookLevelUp(BYTE idx)
{
	if (idx >= BUFFI_SKILL_COUNT || m_aiSkillLevel[idx] >= 40)
		return;
	m_aiBookProgress[idx] = 0;
	++m_aiSkillLevel[idx];
	if (m_pkChar)
		m_pkChar->EffectPacket(SE_SUCCESS);
	RequestRecast();
	Save();
	SendSkills();
}

// Buffi Mastery Book (M -> G1) / Mastery Stone (G -> P)
bool CBuffiActor::UseMasterBook(BYTE idx)
{
	if (idx >= BUFFI_SKILL_COUNT)
		return false;
	int& lv = m_aiSkillLevel[idx];
	if (lv < 20 || lv >= 40)
		return false;
	const DWORD dwVnum = lv < 30 ? BUFFI_MASTER_BOOK_VNUM : BUFFI_MASTER_STONE_VNUM;
	if (m_pkOwner->CountSpecifyItem(dwVnum) <= 0)
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have the required skill book."));
		return false;
	}
	m_pkOwner->RemoveSpecifyItem(dwVnum, 1);
	m_aiBookProgress[idx] = 0;
	lv = lv < 30 ? 30 : 40;
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's skill has improved."));
	if (m_pkChar)
		m_pkChar->EffectPacket(SE_SUCCESS);
	RequestRecast();
	Save();
	SendSkills();
	return true;
}
#endif

#ifdef ENABLE_BUFFI_EVOLUTION
// Reference uiCompanion.BUFFI_EVOLVE_REQUIREMENTS (26002 = yang). Evolution n is possible at the level cap 55/75/90.
static const DWORD s_aadwEvolveItem[BUFFI_MAX_EVOLUTION][5][2] = {
	{ { 40912, 5 }, { 40812, 5 }, { 40714, 10 }, { 50628, 10 }, { 40914, 3 } },
	{ { 40942, 5 }, { 40914, 3 }, { 50632, 10 }, { 40718, 3 }, { 40820, 5 } },
	{ { 40917, 10 }, { 40919, 5 }, { 40920, 5 }, { 40718, 5 }, { 40821, 5 } },
};
static const long long s_allEvolveYang[BUFFI_MAX_EVOLUTION] = { 10000000LL, 50000000LL, 100000000LL };

bool CBuffiActor::Evolve()
{
	if (m_iEvolution >= BUFFI_MAX_EVOLUTION)
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi cannot evolve any further."));
		return false;
	}
	if (m_iLevel < GetLevelCap())
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi must reach level %d to evolve."), GetLevelCap());
		return false;
	}

	const int evo = m_iEvolution;
	for (int i = 0; i < 5; ++i)
	{
		if (m_pkOwner->CountSpecifyItem(s_aadwEvolveItem[evo][i][0]) < (int) s_aadwEvolveItem[evo][i][1])
		{
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have the materials for the evolution."));
			return false;
		}
	}
	if ((long long) m_pkOwner->GetGold() < s_allEvolveYang[evo])
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Yang."));
		return false;
	}

	for (int i = 0; i < 5; ++i)
		m_pkOwner->RemoveSpecifyItem(s_aadwEvolveItem[evo][i][0], s_aadwEvolveItem[evo][i][1]);
	m_pkOwner->PointChange(POINT_GOLD, -s_allEvolveYang[evo], true);

	++m_iEvolution;
	ApplyStats();
	RequestRecast();	// the INT went up
	Save();
	SendData();
	if (m_pkChar)
		m_pkChar->EffectPacket(SE_SUCCESS);
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi has evolved."));
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "SupportEvolutionResult 1");
	return true;
}
#endif

#ifdef ENABLE_BUFFI_CHANGE_SEX
bool CBuffiActor::ChangeSex(LPITEM item)
{
	// the worn Buffi costumes have to fit the other sex too
	const bool bToMale = m_iSex == 0;
	static const BYTE s_abWear[] = { WEAR_BUFFI_ARMOR, WEAR_BUFFI_HAIR, WEAR_BUFFI_WEAPON, WEAR_BUFFI_ACCE };
	for (BYTE wear : s_abWear)
	{
		LPITEM pkCostume = m_pkOwner->GetWear(wear);
		if (pkCostume && IS_SET(pkCostume->GetAntiFlag(), bToMale ? ITEM_ANTIFLAG_MALE : ITEM_ANTIFLAG_FEMALE))
		{
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Take off the Buffi costumes that do not fit the other sex first."));
			return false;
		}
	}

	item->SetCount(item->GetCount() - 1);
	m_iSex = bToMale ? 1 : 0;
	Save();

	if (m_pkChar)	// a new NPC with the other look, the buffs stay
	{
		Unsummon(false);
		Summon();
	}
	SendData();
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's sex has been changed."));
	return true;
}
#endif

bool CBuffiActor::UseOre(LPITEM item)
{
	const bool bPerm = item->GetVnum() == BUFFI_ORE_PERM_VNUM;
	// socket 0: remaining seconds of an ore taken out of a slot, otherwise a fresh 24h ore
	const int iOreDuration = (!bPerm && item->GetSocket(0) > 0) ? (int) item->GetSocket(0) : (int) BUFFI_ORE_DURATION;
	ExpireRunes();

	for (int i = 0; i < BUFFI_RUNE_COUNT; ++i)
	{
		if (m_aiRune[i] != 0)
			continue;
		m_aiRune[i] = bPerm ? -1 : get_global_time() + iOreDuration;
		item->SetCount(item->GetCount() - 1);
		ApplyStats();
		RequestRecast();
		Save();
		SendData();
		return true;
	}

	// All slots used: a 24h ore refreshes the oldest timed rune.
	if (!bPerm)
	{
		int iBest = -1;
		for (int i = 0; i < BUFFI_RUNE_COUNT; ++i)
			if (m_aiRune[i] > 0 && (iBest < 0 || m_aiRune[i] < m_aiRune[iBest]))
				iBest = i;
		if (iBest >= 0)
		{
			m_aiRune[iBest] = get_global_time() + BUFFI_ORE_DURATION;
			item->SetCount(item->GetCount() - 1);
			Save();
			SendData();
			return true;
		}
	}

	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("All of your Buffi's ore slots are full."));
	return false;
}

bool CBuffiActor::RemoveOre(BYTE idx)
{
	ExpireRunes();
	if (idx >= BUFFI_RUNE_COUNT || m_aiRune[idx] == 0)
		return false;

	if (m_pkOwner->GetEmptyInventory(1) == -1)
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough space in your inventory."));
		return false;
	}

	const bool bPerm = m_aiRune[idx] < 0;
	const int iRemain = bPerm ? 0 : m_aiRune[idx] - get_global_time();
	LPITEM item = m_pkOwner->AutoGiveItem(bPerm ? BUFFI_ORE_PERM_VNUM : BUFFI_ORE_VNUM, 1);
	if (!item)
		return false;
	if (!bPerm)
		item->SetSocket(0, MAX(1, iRemain));

	m_aiRune[idx] = 0;
	ApplyStats();
	RequestRecast();
	Save();
	SendData();
	return true;
}

bool CBuffiActor::UseDuration(LPITEM item)
{
	if (m_bDurationPerm)
	{
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's duration is already permanent."));
		return false;
	}

	if (item->GetVnum() == BUFFI_DURATION_PERM_VNUM)
		m_bDurationPerm = true;
	else
	{
		// Reference window shows the duration as "<remain> / 30g": it cannot be stacked past 30 days.
		const int now = get_global_time();
		const int iRemain = MAX(0, m_iDurationEnd - now);
		if (iRemain + 3 * 24 * 60 * 60 > BUFFI_MAX_DURATION)
		{
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's duration cannot exceed 30 days."));
			return false;
		}
		m_iDurationEnd = now + iRemain + 3 * 24 * 60 * 60;
	}

	item->SetCount(item->GetCount() - 1);
	Save();
	SendData();
	return true;
}

int CBuffiActor::UseAllDuration()
{
	int iUsed = 0;
	while (!m_bDurationPerm)
	{
		LPITEM item = m_pkOwner->FindSpecifyItem(BUFFI_DURATION_VNUM);
		if (!item || !UseDuration(item))
			break;
		++iUsed;
	}
	return iUsed;
}

int CBuffiActor::GetSkillBonus(BYTE idx) const
{
	// the main bonus of the skill (Swiftness also adds CASTING_SPEED under the same affect type)
	const CSkillProto* pkSk = CSkillManager::instance().Get(s_adwBuffiSkill[idx]);
	const CAffect* pAff = m_pkOwner->FindAffect(s_adwBuffiSkill[idx], pkSk ? pkSk->bPointOn : APPLY_NONE);
	return pAff ? pAff->lApplyValue : 0;
}

void CBuffiActor::SendAll()
{
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "SetSupportName %s", m_strName.c_str());
	SendData();
	SendSkills();
	SendCooldowns();
	SendSkillColors();
}

void CBuffiActor::SendData()
{
	m_dwLastSendTime = get_dword_time();
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "SetSupportData %d %d %u %u %d %d %d %d %d",
		m_iLevel, GetIntelligence(), m_dwExp, GetNeedExp(), m_iEvolution, GetRemainDuration(),
		GetRuneValue(0), GetRuneValue(1), GetRuneValue(2));
}

void CBuffiActor::SendSkills()
{
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "SetSupportSkill %d %d %d %d %d %d %d %d %d %d %d",
		m_iSkillPoints,
		GetSkillBonus(0), m_aiSkillLevel[0], GetSkillBonus(1), m_aiSkillLevel[1], GetSkillBonus(2), m_aiSkillLevel[2],
		GetSkillBonus(3), m_aiSkillLevel[3], GetSkillBonus(4), m_aiSkillLevel[4]);

	const CAffect* pCast = m_pkOwner->FindAffect(SKILL_KWAESOK, POINT_CASTING_SPEED);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "SetSupportCastSpeed %d", pCast ? (int) pCast->lApplyValue : 0);
}

void CBuffiActor::SendCooldowns()
{
	const DWORD now = get_dword_time();
	int left[BUFFI_SKILL_COUNT];
	for (int i = 0; i < BUFFI_SKILL_COUNT; ++i)
		left[i] = m_adwNextCast[i] > now ? (int)((m_adwNextCast[i] - now) / 1000) : 0;
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "SetSupportSkillCooldown %d %d %d %d %d", left[0], left[1], left[2], left[3], left[4]);
}

CBuffiManager::CBuffiManager()
{
}

CBuffiManager::~CBuffiManager()
{
	for (auto& it : m_map)
		delete it.second;
	m_map.clear();
}

CBuffiActor* CBuffiManager::Get(LPCHARACTER owner, bool bCreate)
{
	if (!owner || !owner->IsPC())
		return NULL;

	auto it = m_map.find(owner->GetPlayerID());
	if (it != m_map.end())
		return it->second;

	if (!bCreate)
		return NULL;

	CBuffiActor* pActor = new CBuffiActor(owner);
	m_map[owner->GetPlayerID()] = pActor;
	return pActor;
}

bool CBuffiManager::IsSummoned(LPCHARACTER owner)
{
	CBuffiActor* pActor = Get(owner);
	return pActor && pActor->IsSummoned();
}

bool CBuffiManager::IsBuffiCharacter(LPCHARACTER ch) const
{
	return ch && !ch->IsPC() && (ch->GetRaceNum() == BUFFI_NPC_FEMALE || ch->GetRaceNum() == BUFFI_NPC_MALE);
}

bool CBuffiManager::IsSealEquipped(LPCHARACTER owner)
{
	LPITEM seal = owner ? owner->GetWear(WEAR_BUFFI) : NULL;
	return seal && seal->GetVnum() == BUFFI_SEAL_VNUM;
}

bool CBuffiManager::HasSeal(LPCHARACTER owner)
{
	return IsSealEquipped(owner) || (owner && owner->FindSpecifyItem(BUFFI_SEAL_VNUM));
}

bool CBuffiManager::EquipToBuffi(LPCHARACTER owner, LPITEM item, int cell)
{
	if (!owner || !item || !IsBuffiWearCell(cell) || item->IsExchanging())
		return false;

	if (owner->GetWear(cell))
		return false;

	if (cell == WEAR_BUFFI)
	{
		if (item->GetVnum() != BUFFI_SEAL_VNUM)
			return false;
	}
	else
	{
		// Buffi costumes: a costume of the matching kind the Buffi (a shaman of its sex) can wear
		if (!HasSeal(owner))
		{
			owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have any buffi summoned"));
			return false;
		}
		if (item->GetType() != ITEM_COSTUME)
			return false;

		bool bKind = false;
		switch (cell)
		{
			case WEAR_BUFFI_ARMOR:	bKind = item->GetSubType() == COSTUME_BODY; break;
			case WEAR_BUFFI_HAIR:	bKind = item->GetSubType() == COSTUME_HAIR; break;
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
			case WEAR_BUFFI_WEAPON:	bKind = item->GetSubType() == COSTUME_WEAPON && (item->GetValue(3) == WEAPON_BELL || item->GetValue(3) == WEAPON_FAN); break;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			case WEAR_BUFFI_ACCE:	bKind = item->GetSubType() == COSTUME_ACCE; break;
#endif
		}
		if (!bKind || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SHAMAN))
			return false;

		CBuffiActor* pActor = Get(owner, true);
		const bool bMale = pActor && pActor->GetSex();
		if ((bMale && IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE)) || (!bMale && IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)))
		{
			owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be equipped by your Buffi."));
			return false;
		}
	}

	return item->EquipTo(owner, (BYTE) cell);
}

void CBuffiManager::OnWearChanged(LPCHARACTER owner, int cell, bool bEquipped)
{
	if (!owner || !owner->IsPC())
		return;

	if (cell == WEAR_BUFFI)
	{
		if (bEquipped)
		{
			CBuffiActor* pActor = Get(owner, true);
			if (pActor && pActor->CanSummon())
				pActor->Summon();
			else if (pActor)
			{
				owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Buffi's duration has expired. Use a Buffi Duration to revive it."));
				pActor->SendAll();
				owner->ChatPacket(CHAT_TYPE_COMMAND, "SupportLeave");
			}
		}
		else if (CBuffiActor* pActor = Get(owner))	// no new actor while the owner is being destroyed
		{
			// itemdesc: "Manually unequipping Buffi removes the skill effects it granted."
			const bool bKeep = pActor->IsKeepBuffsOnUnequip();
			pActor->SetKeepBuffsOnUnequip(false);
			pActor->Unsummon(!bKeep);
		}
		return;
	}

	if (CBuffiActor* pActor = Get(owner))
		pActor->RefreshLook();
}

bool CBuffiManager::Summon(LPCHARACTER owner)
{
	if (!IsSealEquipped(owner))
		return false;

	CBuffiActor* pActor = Get(owner, true);
	return pActor && pActor->Summon();
}

// Game options (client sends /buffi_option <pvp> <pvm>):
//  auto_unequip_buffi_pvp: "Removes your Buffi before damaging another player."
//  auto_equip_buffi_pvm:   "Equips your Buffi when damaging a monster or Metin."
void CBuffiManager::OnOwnerAttack(LPCHARACTER owner, LPCHARACTER victim)
{
	if (!owner || !victim || owner == victim || !owner->IsPC())
		return;

	if (victim->IsPC())
	{
		if (owner->GetQuestFlag("buffi.opt_pvp") && IsSealEquipped(owner))
		{
			CBuffiActor* pActor = Get(owner, true);
			pActor->SetKeepBuffsOnUnequip(true);	// automatic removal keeps the granted buffs
			if (!owner->UnequipItem(owner->GetWear(WEAR_BUFFI)))
				pActor->SetKeepBuffsOnUnequip(false);
		}
		return;
	}

	if ((victim->IsMonster() || victim->IsStone()) && owner->GetQuestFlag("buffi.opt_pvm") && !IsSealEquipped(owner))
	{
		LPITEM seal = owner->FindSpecifyItem(BUFFI_SEAL_VNUM);
		CBuffiActor* pActor = seal ? Get(owner, true) : NULL;
		if (pActor && pActor->CanSummon())
			owner->EquipItem(seal, WEAR_BUFFI);
	}
}

void CBuffiManager::Unsummon(LPCHARACTER owner)
{
	CBuffiActor* pActor = Get(owner);
	if (pActor)
		pActor->Unsummon(true);
}

void CBuffiManager::OnLogin(LPCHARACTER owner)
{
	if (!HasSeal(owner))
		return;

	if (IsSealEquipped(owner) && Summon(owner))
		return;

	// not summoned: the client still needs level / skills / duration for the seal tooltip
	if (CBuffiActor* actor = Get(owner, true))
	{
		actor->SendAll();
		owner->ChatPacket(CHAT_TYPE_COMMAND, "SupportLeave");
	}
}

void CBuffiManager::OnDestroy(LPCHARACTER owner)
{
	if (!owner)
		return;

	if (!owner->IsPC())
	{
		if (IsBuffiCharacter(owner))
			for (auto& it : m_map)
				if (it.second->GetCharacter() == owner)
					it.second->OnCharacterDestroyed();
		return;
	}

	auto it = m_map.find(owner->GetPlayerID());
	if (it == m_map.end())
		return;

	CBuffiActor* pActor = it->second;
	m_map.erase(it);
	delete pActor;	// keeps "buffi.summoned" so the Buffi comes back after a warp/login
}

void CBuffiManager::OnOwnerGainExp(LPCHARACTER owner, int iExp)
{
	CBuffiActor* pActor = Get(owner);
	if (pActor)
		pActor->OnOwnerGainExp(iExp);
}

bool CBuffiManager::OnUseItem(LPCHARACTER owner, LPITEM item)
{
	switch (item->GetVnum())
	{
		case BUFFI_SEAL_VNUM:
			if (item->IsEquipped())
				owner->UnequipItem(item);
			else
				owner->EquipItem(item, WEAR_BUFFI);
			return true;

#ifdef ENABLE_BUFFI_CHANGE_SEX
		case BUFFI_CHANGE_SEX_VNUM:
		{
			if (!HasSeal(owner))
			{
				owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have any buffi summoned"));
				return true;
			}
			if (CBuffiActor* pActor = Get(owner, true))
				pActor->ChangeSex(item);
			return true;
		}
#endif
#ifdef ENABLE_BUFFI_SKILL_BOOK
		case BUFFI_SKILL_BOOK_VNUM:
		case BUFFI_MASTER_BOOK_VNUM:
		case BUFFI_MASTER_STONE_VNUM:
			owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Use the + button of a Buffi skill in the Buffi window."));
			return true;
#endif
		case BUFFI_ORE_VNUM:
		case BUFFI_ORE_PERM_VNUM:
		case BUFFI_DURATION_VNUM:
		case BUFFI_DURATION_PERM_VNUM:
		{
			if (!HasSeal(owner))
			{
				owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have any buffi summoned"));
				return true;
			}
			CBuffiActor* pActor = Get(owner, true);
			if (item->GetVnum() == BUFFI_ORE_VNUM || item->GetVnum() == BUFFI_ORE_PERM_VNUM)
				pActor->UseOre(item);
			else
				pActor->UseDuration(item);
			return true;
		}
	}
	return false;
}

ACMD(do_buffi_summon)
{
	if (!CBuffiManager::instance().Summon(ch))
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have any buffi summoned"));
}

ACMD(do_buffi_unsummon)
{
	CBuffiManager::instance().Unsummon(ch);
}

ACMD(do_support_skill_power)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CBuffiActor* pActor = CBuffiManager::instance().Get(ch);
	if (!pActor || !pActor->IsSummoned())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please summon your Buffi before doing that."));
		return;
	}

	int idx = 0;
	str_to_number(idx, arg1);
	if (idx < 0 || idx >= BUFFI_SKILL_COUNT)
		return;
	pActor->RaiseSkill((BYTE) idx);
}

ACMD(do_support_add_ore)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int cell = -1;
	str_to_number(cell, arg1);
	if (cell < 0)
		return;

	LPITEM item = ch->GetInventoryItem((WORD) cell);
	if (!item || (item->GetVnum() != BUFFI_ORE_VNUM && item->GetVnum() != BUFFI_ORE_PERM_VNUM))
		return;

	CBuffiManager::instance().OnUseItem(ch, item);
}

ACMD(do_support_remove_ore)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int idx = -1;
	str_to_number(idx, arg1);
	if (idx < 0 || idx >= BUFFI_RUNE_COUNT || !CBuffiManager::HasSeal(ch))
		return;

	if (CBuffiActor* pActor = CBuffiManager::instance().Get(ch, true))
		pActor->RemoveOre((BYTE) idx);
}

ACMD(do_buffi_skill_color)
{
	// /buffi_skill_color <skill 0..4> <c0> <c1> <c2> <c3> <c4>
	char arg[6][64];
	const char* rest = argument;
	for (int i = 0; i < 6; ++i)
		rest = one_argument(rest, arg[i], sizeof(arg[i]));

	int idx = -1;
	str_to_number(idx, arg[0]);
	if (idx < 0 || idx >= BUFFI_SKILL_COUNT || !CBuffiManager::HasSeal(ch))
		return;

	DWORD adwColor[5];
	for (int l = 0; l < 5; ++l)
	{
		adwColor[l] = 0;
		str_to_number(adwColor[l], arg[l + 1]);
	}

	if (CBuffiActor* pActor = CBuffiManager::instance().Get(ch, true))
		pActor->SetSkillColor((BYTE) idx, adwColor);
}

#ifdef ENABLE_BUFFI_EVOLUTION
ACMD(do_support_set_evolution)
{
	if (!CBuffiManager::HasSeal(ch))
		return;
	if (CBuffiActor* pActor = CBuffiManager::instance().Get(ch, true))
		pActor->Evolve();
}
#endif

ACMD(do_support_full_duration)
{
	if (!CBuffiManager::HasSeal(ch))
		return;

	CBuffiActor* pActor = CBuffiManager::instance().Get(ch, true);
	if (pActor->UseAllDuration() == 0)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have any Buffi Duration."));
}

// GM: /buffi_set <level> [skill_points] [evolution]
ACMD(do_buffi_set)
{
	char arg1[256], arg2[256], arg3[256];
	argument = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(argument, arg3, sizeof(arg3));

	int level = 0, pts = -1, evo = -1;
	str_to_number(level, arg1);
	if (*arg2) str_to_number(pts, arg2);
	if (*arg3) str_to_number(evo, arg3);

	if (level < 1 || level > BUFFI_MAX_LEVEL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "usage: /buffi_set <level 1-%d> [skill_points] [evolution 0-%d]", BUFFI_MAX_LEVEL, BUFFI_MAX_EVOLUTION);
		return;
	}

	CBuffiManager::instance().Unsummon(ch);
	CBuffiManager::instance().OnDestroy(ch);

	CBuffiActor* pTmp = CBuffiManager::instance().Get(ch, true);	// creates defaults on first use
	CBuffiManager::instance().OnDestroy(ch);
	(void) pTmp;

	ch->SetQuestFlag("buffi.level", level);
	ch->SetQuestFlag("buffi.exp", 0);
	if (pts >= 0) ch->SetQuestFlag("buffi.pts", pts);
	if (evo >= 0) ch->SetQuestFlag("buffi.evo", MINMAX(0, evo, (int) BUFFI_MAX_EVOLUTION));
	ch->ChatPacket(CHAT_TYPE_INFO, "Buffi: level %d, points %d, evolution %d", level, ch->GetQuestFlag("buffi.pts"), ch->GetQuestFlag("buffi.evo"));
	CBuffiManager::instance().Summon(ch);
}

ACMD(do_buffi_option)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	int pvp = 0, pvm = 0;
	str_to_number(pvp, arg1);
	str_to_number(pvm, arg2);
	ch->SetQuestFlag("buffi.opt_pvp", pvp ? 1 : 0);
	ch->SetQuestFlag("buffi.opt_pvm", pvm ? 1 : 0);
}

ACMD(do_buffi_info)
{
	CBuffiActor* pActor = CBuffiManager::instance().Get(ch, CBuffiManager::HasSeal(ch));
	if (pActor)
		pActor->SendAll();
}
#endif
