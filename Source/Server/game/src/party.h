#ifndef __INC_METIN_II_GAME_PARTY_H__
#define __INC_METIN_II_GAME_PARTY_H__

#include "char.h"

#include "../../common/service.h"

enum // unit : minute
{
	PARTY_ENOUGH_MINUTE_FOR_EXP_BONUS = 60,
	PARTY_HEAL_COOLTIME_LONG = 60,
	PARTY_HEAL_COOLTIME_SHORT = 30,
	PARTY_MAX_MEMBER = 8,
	PARTY_DEFAULT_RANGE = 5000,
};

enum EPartyRole
{
	PARTY_ROLE_NORMAL,
	PARTY_ROLE_LEADER,
	PARTY_ROLE_ATTACKER,
	PARTY_ROLE_TANKER,
	PARTY_ROLE_BUFFER,
	PARTY_ROLE_SKILL_MASTER,
	PARTY_ROLE_HASTE,
	PARTY_ROLE_DEFENDER,
	PARTY_ROLE_MAX_NUM,
};

enum EPartyExpDistributionModes
{
	PARTY_EXP_DISTRIBUTION_NON_PARITY,
	PARTY_EXP_DISTRIBUTION_PARITY,
	PARTY_EXP_DISTRIBUTION_MAX_NUM
};

class CParty;
class CDungeon;

class CPartyManager : public singleton<CPartyManager>
{
	public:
		typedef std::map<DWORD, LPPARTY> TPartyMap;
		typedef std::set<LPPARTY> TPCPartySet;

	public:
		CPartyManager();
		virtual ~CPartyManager();

		void		Initialize();

		//void		SendPartyToDB();

		void		EnablePCParty() { m_bEnablePCParty = true; sys_log(0, "PARTY Enable"); }
		void		DisablePCParty() { m_bEnablePCParty = false; sys_log(0, "PARTY Disable"); }
		bool		IsEnablePCParty() { return m_bEnablePCParty; }

		LPPARTY		CreateParty(LPCHARACTER pkLeader);
		void		DeleteParty(LPPARTY pParty);
		void		DeleteAllParty();
		bool		SetParty(LPCHARACTER pkChr);

		void		SetPartyMember(DWORD dwPID, LPPARTY pParty);

		void		P2PLogin(DWORD pid, const char* name);
		void		P2PLogout(DWORD pid);

		LPPARTY		P2PCreateParty(DWORD pid);
		void		P2PDeleteParty(DWORD pid);
		void		P2PJoinParty(DWORD leader, DWORD pid, BYTE role = 0);
		void		P2PQuitParty(DWORD pid);

	private:
		TPartyMap	m_map_pkParty;
		TPartyMap	m_map_pkMobParty;

		TPCPartySet	m_set_pkPCParty;

		bool		m_bEnablePCParty;
};

enum EPartyMessages
{
	PM_ATTACK,		// Attack him
	PM_RETURN,		// Return back to position
	PM_ATTACKED_BY,	// I was attacked by someone
	PM_AGGRO_INCREASE,	// My aggro is increased
};

class CParty
{
	public:
		typedef struct SMember
		{
			LPCHARACTER	pCharacter;
			bool	bNear;
			BYTE	bRole;
#ifdef ENABLE_LEVEL_INT
			int		bLevel;
#else
			BYTE	bLevel;
#endif
			std::string strName;
		} TMember;

		typedef std::map<DWORD, TMember> TMemberMap;

		typedef std::map<std::string, int> TFlagMap;

	public:
		CParty();
		virtual ~CParty();

		void		P2PJoin(DWORD dwPID);
		void		P2PQuit(DWORD dwPID);
		virtual void	Join(DWORD dwPID);
		void		Quit(DWORD dwPID);
		void		Link(LPCHARACTER pkChr);
		void		Unlink(LPCHARACTER pkChr);

		void		ChatPacketToAllMember(BYTE type, DWORD id, const char* format, ...);

		void		UpdateOnlineState(DWORD dwPID, const char* name);
		void		UpdateOfflineState(DWORD dwPID);

		DWORD		GetLeaderPID();
		LPCHARACTER	GetLeaderCharacter();
		LPCHARACTER	GetLeader() { return m_pkChrLeader; }

		DWORD		GetMemberCount();
		DWORD		GetNearMemberCount()	{ return m_iCountNearPartyMember; }

		bool		IsMember(DWORD pid) { return m_memberMap.find(pid) != m_memberMap.end(); }

		bool		IsNearLeader(DWORD pid);

		bool		IsPositionNearLeader(LPCHARACTER ch);

		void		SendMessage(LPCHARACTER ch, BYTE bMsg, DWORD dwArg1, DWORD dwArg2);

		void		SendPartyJoinOneToAll(DWORD dwPID);
		void		SendPartyJoinAllToOne(LPCHARACTER ch);
		void		SendPartyRemoveOneToAll(DWORD dwPID);

		void		SendPartyInfoOneToAll(DWORD pid);
		void		SendPartyInfoOneToAll(LPCHARACTER ch);
		void		SendPartyInfoAllToOne(LPCHARACTER ch);

		void		SendPartyLinkOneToAll(LPCHARACTER ch);
		void		SendPartyLinkAllToOne(LPCHARACTER ch);
		void		SendPartyUnlinkOneToAll(LPCHARACTER ch);
#ifdef ENABLE_PARTY_POSITION
		void		SendPositionInfo() const;
#endif

		int		GetPartyBonusExpPercent()	{ return m_iExpBonus; }
		int		GetPartyBonusAttackGrade()	{ return m_iAttBonus; }
		int		GetPartyBonusDefenseGrade()	{ return m_iDefBonus; }

		int	ComputePartyBonusExpPercent();
		inline int	ComputePartyBonusAttackGrade();
		inline int	ComputePartyBonusDefenseGrade();

		template <class Func> void ForEachMember(Func & f);
		template <class Func> void ForEachMemberPtr(Func & f);
		template <class Func> void ForEachOnlineMember(Func & f);
		template <class Func> void ForEachNearMember(Func & f);
		template <class Func> void ForEachOnMapMember (Func & f, long lMapIndex);
		template <class Func> bool ForEachOnMapMemberBool (Func & f, long lMapIndex);

		void		Update();

		int		GetExpBonusPercent();

		bool		SetRole(DWORD pid, BYTE bRole, bool on);
		BYTE		GetRole(DWORD pid);
		bool		IsRole(DWORD pid, BYTE bRole);

#ifdef ENABLE_LEVEL_INT
		int			GetMemberMaxLevel();
		int			GetMemberMinLevel();
#else
		BYTE		GetMemberMaxLevel();
		BYTE		GetMemberMinLevel();
#endif

		void		ComputeRolePoint(LPCHARACTER ch, BYTE bRole, bool bAdd);

		void		HealParty();
		void		SummonToLeader(DWORD pid);

		void		SetPCParty(bool b) { m_bPCParty = b; }

		LPCHARACTER	GetNextOwnership(LPCHARACTER ch, long x, long y);

		void		SetFlag(const std::string& name, int value);
		int		GetFlag(const std::string& name);

		void		SetDungeon(LPDUNGEON pDungeon);
		LPDUNGEON	GetDungeon();

		void		RemoveBonuses();

		BYTE		CountMemberByVnum(DWORD dwVnum);

		void		SetParameter(int iMode);
		int		GetExpDistributionMode();

		void		SetExpCentralizeCharacter(DWORD pid);
		LPCHARACTER	GetExpCentralizeCharacter();

#ifdef ENABLE_LEVEL_INT
		void		RequestSetMemberLevel(DWORD pid, int level);
		void		P2PSetMemberLevel(DWORD pid, int level);
#else
		void		RequestSetMemberLevel(DWORD pid, BYTE level);
		void		P2PSetMemberLevel(DWORD pid, BYTE level);
#endif

		bool		IsPartyInDungeon(int mapIndex);
		bool		IsPartyInAnyDungeon();

	protected:
		void		IncreaseOwnership();

		virtual void	Initialize();
		void		Destroy();
		void		RemovePartyBonus();

		void		RemoveBonus();
		void		RemoveBonusForOne(DWORD pid);

		void		SendParameter(LPCHARACTER ch);
		void		SendParameterToAll();

		TMemberMap	m_memberMap;
		DWORD		m_dwLeaderPID;
		LPCHARACTER	m_pkChrLeader;

		LPEVENT		m_eventUpdate;

#ifdef ENABLE_PARTY_POSITION
		LPEVENT		m_PositionInfoEvent;
#endif

		TMemberMap::iterator m_itNextOwner;

	private:
		int		m_iExpDistributionMode;
		LPCHARACTER	m_pkChrExpCentralize;

		DWORD		m_dwPartyStartTime;

		DWORD		m_dwPartyHealTime;
		bool		m_bPartyHealReady;
		bool		m_bCanUsePartyHeal;

		int		m_anRoleCount[PARTY_ROLE_MAX_NUM];
		int		m_anMaxRole[PARTY_ROLE_MAX_NUM];

		int		m_iLongTimeExpBonus;

		// used in Update
		int		m_iLeadership;
		int		m_iExpBonus;
		int		m_iAttBonus;
		int		m_iDefBonus;

		// changed only in Update
		int		m_iCountNearPartyMember;

		bool		m_bPCParty;

		TFlagMap	m_map_iFlag;

		LPDUNGEON	m_pkDungeon;
		LPDUNGEON	m_pkDungeon_for_Only_party;
	public:
		void SetDungeon_for_Only_party(LPDUNGEON pDungeon);
		LPDUNGEON GetDungeon_for_Only_party();
};

template <class Func> void CParty::ForEachMember(Func & f)
{
	TMemberMap::iterator it;

	for (it = m_memberMap.begin(); it != m_memberMap.end(); ++it)
		f(it->first);
}

template <class Func> void CParty::ForEachMemberPtr(Func & f)
{
	TMemberMap::iterator it;

	for (it = m_memberMap.begin(); it != m_memberMap.end(); ++it)
		f(it->second.pCharacter);
}

template <class Func> void CParty::ForEachOnlineMember(Func & f)
{
	TMemberMap::iterator it;

	for (it = m_memberMap.begin(); it != m_memberMap.end(); ++it)
		if (it->second.pCharacter)
			f(it->second.pCharacter);
}

template <class Func> void CParty::ForEachNearMember(Func & f)
{
	TMemberMap::iterator it;

	for (it = m_memberMap.begin(); it != m_memberMap.end(); ++it)
		if (it->second.pCharacter && it->second.bNear)
			f(it->second.pCharacter);
}

template <class Func> void CParty::ForEachOnMapMember (Func & f, long lMapIndex)
{
	TMemberMap::iterator it;

	for (it = m_memberMap.begin(); it != m_memberMap.end(); ++it)
	{
		LPCHARACTER ch = it->second.pCharacter;
		if (ch)
		{
			if (ch->GetMapIndex () == lMapIndex)
				f(ch);
		}
	}
}

template <class Func> bool CParty::ForEachOnMapMemberBool(Func & f, long lMapIndex)
{
	TMemberMap::iterator it;

	for (it = m_memberMap.begin(); it != m_memberMap.end(); ++it)
	{
		LPCHARACTER ch = it->second.pCharacter;
		if (ch)
		{
			if (ch->GetMapIndex () == lMapIndex)
			{
				if(f(ch) == false)
				{
					return false;
				}
			}
		}
	}
	return true;
}

inline int CParty::ComputePartyBonusAttackGrade()
{
	return 0;
}

inline int CParty::ComputePartyBonusDefenseGrade()
{
	return 0;
}

#ifdef ENABLE_DICE_SYSTEM
#include "item.h"

struct FPartyDropDiceRoll
{
	const LPITEM m_itemDrop;
	LPCHARACTER m_itemOwner;
	int m_lastNumber;

	FPartyDropDiceRoll(const LPITEM itemDrop, LPCHARACTER itemOwner) : m_itemDrop(itemDrop), m_itemOwner(itemOwner), m_lastNumber(0)
	{
	};

	void Process(const LPCHARACTER mobVictim)
	{
		if ((!mobVictim || (mobVictim->GetMobRank() >= MOB_RANK_BOSS && mobVictim->GetMobRank() <= MOB_RANK_KING)) && m_itemOwner->GetParty() && m_itemOwner->GetParty()->GetNearMemberCount() > 1)
		{
			LPPARTY pParty = m_itemOwner->GetParty();
			pParty->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, 552, "%s", m_itemDrop->GetLocaleName());

			pParty->ForEachNearMember(*this);
			if (m_itemOwner)
			{
				m_itemDrop->SetOwnership(m_itemOwner);
				pParty->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, 553, "%s#%s", m_itemOwner->GetName(), m_itemDrop->GetLocaleName());
			}
		}
		else
			m_itemDrop->SetOwnership(m_itemOwner);
	}
	LPCHARACTER GetItemOwner()
	{
		return m_itemOwner;
	}
	const LPITEM GetItemDrop()
	{
		return m_itemDrop;
	}
	void operator () (LPCHARACTER ch)
	{
		if (!ch)
			return;

		LPPARTY pParty = ch->GetParty();
		if (!pParty)
			return;

		while (true)
		{
			int pickedNumber = number(10000, 99999);
			if (pickedNumber > m_lastNumber)
			{
				m_lastNumber = pickedNumber;
				m_itemOwner = ch;
			}
			else if (pickedNumber == m_lastNumber)
			{
				continue;
			}
			else // if (pickedNumber < m_lastNumber)
			{
			}
			pParty->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, 554, "%s#%d", ch->GetName(), pickedNumber);
			break;
		}
	}
};
#endif

#endif
