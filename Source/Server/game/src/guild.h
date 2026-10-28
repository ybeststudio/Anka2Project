#ifndef __INC_GUILD_H
#define __INC_GUILD_H

#include "skill.h"
#include "../../common/service.h"

typedef struct _SQLMsg SQLMsg;

enum
{
	GUILD_GRADE_NAME_MAX_LEN = 8,
	GUILD_GRADE_COUNT = 15,
	GUILD_COMMENT_MAX_COUNT = 12,
	GUILD_COMMENT_MAX_LEN = 50,
	GUILD_LEADER_GRADE = 1,
	GUILD_BASE_POWER = 400,
	GUILD_POWER_PER_SKILL_LEVEL = 200,
	GUILD_POWER_PER_LEVEL = 100,
	GUILD_MINIMUM_LEADERSHIP = 40,
	GUILD_WAR_MIN_MEMBER_COUNT = 8,
	GUILD_LADDER_POINT_PER_LEVEL = 1000,
	GUILD_CREATE_ITEM_VNUM = 70101,
	GUILD_MAX_MEMBERS = 500,
};

struct SGuildMaster
{
	DWORD pid;
};


typedef struct SGuildMember
{
	DWORD pid;
	BYTE grade;
	BYTE is_general;
	BYTE job;
#ifdef ENABLE_LEVEL_INT
	int level;
#else
	BYTE level;
#endif
	DWORD offer_exp;
	BYTE _dummy;

	std::string name;

	SGuildMember(LPCHARACTER ch, BYTE grade, DWORD offer_exp);
#ifdef ENABLE_LEVEL_INT
	SGuildMember(DWORD pid, BYTE grade, BYTE is_general, BYTE job, int level, DWORD offer_exp, char* name);
#else
	SGuildMember(DWORD pid, BYTE grade, BYTE is_general, BYTE job, BYTE level, DWORD offer_exp, char* name);
#endif

} TGuildMember;

#pragma pack(1)
typedef struct SGuildMemberPacketData
{
#ifdef ENABLE_LEVEL_INT
	DWORD pid;
	BYTE byGrade;
	BYTE byIsGeneral;
	BYTE byJob;
	int	 byLevel;
	DWORD dwOffer;
	BYTE byNameFlag;
#else
	DWORD pid;
	BYTE grade;
	BYTE is_general;
	BYTE job;
	BYTE level;
	DWORD offer;
	BYTE name_flag;
	char name[CHARACTER_NAME_MAX_LEN+1];
#endif
} TGuildMemberPacketData;

typedef struct packet_guild_sub_info
{
	WORD member_count;
	WORD max_member_count;
	DWORD guild_id;
	DWORD master_pid;
	DWORD exp;
	BYTE level;
	char name[GUILD_NAME_MAX_LEN+1];
	DWORD gold;
	BYTE has_land;
} TPacketGCGuildInfo;

typedef struct SGuildGrade
{
	char grade_name[GUILD_GRADE_NAME_MAX_LEN+1];
	BYTE auth_flag;
} TGuildGrade;

struct TOneGradeNamePacket
{
	BYTE grade;
	char grade_name[GUILD_GRADE_NAME_MAX_LEN+1];
};

struct TOneGradeAuthPacket
{
	BYTE grade;
	BYTE auth;
};
#pragma pack()

enum
{
	GUILD_AUTH_ADD_MEMBER	= (1 << 0),
	GUILD_AUTH_REMOVE_MEMBER	= (1 << 1),
	GUILD_AUTH_NOTICE		= (1 << 2),
	GUILD_AUTH_USE_SKILL	= (1 << 3),
};

typedef struct SGuildData
{
	DWORD	guild_id;
	DWORD	master_pid;
	DWORD	exp;
	BYTE	level;
	char	name[GUILD_NAME_MAX_LEN+1];

	TGuildGrade	grade_array[GUILD_GRADE_COUNT];

	BYTE	skill_point;
	BYTE	abySkill[GUILD_SKILL_COUNT];

	int		power;
	int		max_power;

	int		ladder_point;

	int		win;
	int		draw;
	int		loss;

	int		gold;
} TGuildData;

struct TGuildCreateParameter
{
	LPCHARACTER master;
	char name[GUILD_NAME_MAX_LEN+1];
};

typedef struct SGuildWar
{
	DWORD war_start_time;
	DWORD score;
	DWORD state;
	BYTE type;
	DWORD map_index;

	SGuildWar(BYTE type)
		: war_start_time(0),
	score(0),
	state(GUILD_WAR_RECV_DECLARE),
	type(type),
	map_index(0)
	{
	}
	bool IsWarBegin() const
	{
		return state == GUILD_WAR_ON_WAR;
	}
} TGuildWar;

class CGuild
{
	public:
		CGuild(TGuildCreateParameter& cp);
		explicit CGuild(DWORD guild_id) { Load(guild_id); }
		~CGuild();

		DWORD		GetID() const	{ return m_data.guild_id; }
		const char*	GetName() const	{ return m_data.name; }
		int		GetSP() const		{ return m_data.power; }
		int		GetMaxSP() { return m_data.max_power; }
		DWORD		GetMasterPID() const	{ return m_data.master_pid; }
		LPCHARACTER	GetMasterCharacter();
		BYTE		GetLevel() const		{ return m_data.level; }

		void		Reset() { m_data.power = m_data.max_power; }

		void		RequestDisband(DWORD pid);
		void		Disband();

		void		RequestAddMember(LPCHARACTER ch, int grade = 15);
		void		AddMember(TPacketDGGuildMember * p);

		bool		RequestRemoveMember(DWORD pid);
		bool		RemoveMember(DWORD pid);

		void		LoginMember(LPCHARACTER ch);
		void		P2PLoginMember(DWORD pid);

		void		LogoutMember(LPCHARACTER ch);
		void		P2PLogoutMember(DWORD pid);

		void		ChangeMemberGrade(DWORD pid, BYTE grade);
		bool		OfferExp(LPCHARACTER ch, int amount);

#ifdef ENABLE_LEVEL_INT
		void		LevelChange(DWORD pid, int level);
		void		ChangeMemberData(DWORD pid, DWORD offer, int level, BYTE grade);
#else
		void		LevelChange(DWORD pid, BYTE level);
		void		ChangeMemberData(DWORD pid, DWORD offer, BYTE level, BYTE grade);
#endif

		void		ChangeGradeName(BYTE grade, const char* grade_name);
		void		ChangeGradeAuth(BYTE grade, BYTE auth);
		void		P2PChangeGrade(BYTE grade);

		bool		ChangeMemberGeneral(DWORD pid, BYTE is_general);

		bool		ChangeMasterTo(DWORD dwPID);

		void		Packet(const void* buf, int size);

		void		SendOnlineRemoveOnePacket(DWORD pid);
		void		SendAllGradePacket(LPCHARACTER ch);
		void		SendListPacket(LPCHARACTER ch);
		void		SendListOneToAll(DWORD pid);
		void		SendListOneToAll(LPCHARACTER ch);
		void		SendLoginPacket(LPCHARACTER ch, LPCHARACTER chLogin);
		void		SendLogoutPacket(LPCHARACTER ch, LPCHARACTER chLogout);
		void		SendLoginPacket(LPCHARACTER ch, DWORD pid);
		void		SendLogoutPacket(LPCHARACTER ch, DWORD pid);
		void		SendGuildInfoPacket(LPCHARACTER ch);
		void		SendGuildDataUpdateToAllMember(SQLMsg* pmsg);

		void		Load(DWORD guild_id);
		void		SaveLevel();
		void		SaveSkill();
		void		SaveMember(DWORD pid);

		int		GetMaxMemberCount();
		int		GetMemberCount() { return m_member.size(); }
		int		GetTotalLevel() const;

		// GUILD_MEMBER_COUNT_BONUS
		void		SetMemberCountBonus(int iBonus);
		void		BroadcastMemberCountBonus();
		// END_OF_GUILD_MEMBER_COUNT_BONUS

		int		GetMaxGeneralCount() const	{ return 1 /*+ GetSkillLevel(GUILD_SKILL_DEUNGYONG)/3*/;}
		int		GetGeneralCount() const		{ return m_general_count; }

		TGuildMember*	GetMember(DWORD pid);
		DWORD			GetMemberPID(const std::string& strName);
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
		BYTE			IsGuildGeneral(DWORD pid);
#endif

		bool		HasGradeAuth(int grade, int auth_flag) const	{ return (bool)(m_data.grade_array[grade-1].auth_flag & auth_flag);}

		void		AddComment(LPCHARACTER ch, const std::string& str);
		void		DeleteComment(LPCHARACTER ch, DWORD comment_id);

		void		RefreshComment(LPCHARACTER ch);
		void		RefreshCommentForce(DWORD player_id);

		int			GetSkillLevel(DWORD vnum);
		void		SkillLevelUp(DWORD dwVnum);
		void		UseSkill(DWORD dwVnum, LPCHARACTER ch, DWORD pid);

		void		SendSkillInfoPacket(LPCHARACTER ch) const;
		void		ComputeGuildPoints();

		void		GuildPointChange( BYTE type, int amount, bool save = false );

		void		UpdateSkill(BYTE grade, BYTE* skill_levels);
		void		SendDBSkillUpdate(int amount = 0);

		void		SkillRecharge();
		bool		ChargeSP(LPCHARACTER ch, int iSP);

		void		Chat(const char* c_pszText);
		void		P2PChat(const char* c_pszText);

		void		SkillUsableChange(DWORD dwSkillVnum, bool bUsable);
		void		AdvanceLevel(int iLevel);

		// Guild Money
		void		RequestDepositMoney(LPCHARACTER ch, int iGold);
		void		RequestWithdrawMoney(LPCHARACTER ch, int iGold);

		void		RecvMoneyChange(int iGold);
		void		RecvWithdrawMoneyGive(int iChangeGold);

		int		GetGuildMoney() const	{ return m_data.gold; }

		// War general
		void		GuildWarPacket(DWORD guild_id, BYTE bWarType, BYTE bWarState);
		void		SendEnemyGuild(LPCHARACTER ch);

		int		GetGuildWarState(DWORD guild_id);
		bool		CanStartWar(BYTE bGuildWarType);
		DWORD		GetWarStartTime(DWORD guild_id);
		bool		UnderWar(DWORD guild_id);
		DWORD		UnderAnyWar(BYTE bType = GUILD_WAR_TYPE_MAX_NUM);

		// War map relative
		void		SetGuildWarMapIndex(DWORD dwGuildID, long lMapIndex);
		int			GetGuildWarType(DWORD dwGuildOpponent);
		DWORD		GetGuildWarMapIndex(DWORD dwGuildOpponent);

		// War entry question
		void		GuildWarEntryAsk(DWORD guild_opp);
		void		GuildWarEntryAccept(DWORD guild_opp, LPCHARACTER ch);

		// War state relative
		void		NotifyGuildMaster(const char* msg);
		void		RequestDeclareWar(DWORD guild_id, BYTE type);
		void		RequestRefuseWar(DWORD guild_id);

		bool		DeclareWar(DWORD guild_id, BYTE type, BYTE state);
		void		RefuseWar(DWORD guild_id);
		bool		WaitStartWar(DWORD guild_id);
		bool		CheckStartWar(DWORD guild_id);	// check if StartWar method fails (call it before StartWar)
		void		StartWar(DWORD guild_id);
		void		EndWar(DWORD guild_id);
		void		ReserveWar(DWORD guild_id, BYTE type);

		// War points relative
		void		SetWarScoreAgainstTo(DWORD guild_opponent, int newpoint);
		int			GetWarScoreAgainstTo(DWORD guild_opponent);

		int			GetLadderPoint() const	{ return m_data.ladder_point; }
		void		SetLadderPoint(int point);

		void		SetWarData(int iWin, int iDraw, int iLoss) { m_data.win = iWin, m_data.draw = iDraw, m_data.loss = iLoss; }

		void		ChangeLadderPoint(int iChange);

		int			GetGuildWarWinCount() const { return m_data.win; }
		int			GetGuildWarDrawCount() const { return m_data.draw; }
		int			GetGuildWarLossCount() const { return m_data.loss; }

		bool		HasLand();

		// GUILD_JOIN_BUG_FIX

		void		Invite( LPCHARACTER pchInviter, LPCHARACTER pchInvitee );

		void		InviteAccept( LPCHARACTER pchInvitee );

		void		InviteDeny( DWORD dwPID );
		// END_OF_GUILD_JOIN_BUG_FIX

	private:
		void		Initialize();

		TGuildData	m_data;
		int		m_general_count;

		// GUILD_MEMBER_COUNT_BONUS
		int		m_iMemberCountBonus;
		// END_OF_GUILD_MEMBER_COUNT_BONUS

		typedef std::map<DWORD, TGuildMember> TGuildMemberContainer;
		TGuildMemberContainer m_member;

		typedef CHARACTER_SET TGuildMemberOnlineContainer;
		TGuildMemberOnlineContainer m_memberOnline;

		typedef std::set<DWORD>	TGuildMemberP2POnlineContainer;
		TGuildMemberP2POnlineContainer m_memberP2POnline;

		void LoadGuildData(SQLMsg* pmsg);
		void LoadGuildGradeData(SQLMsg* pmsg);
		void LoadGuildMemberData(SQLMsg* pmsg);

		void __P2PUpdateGrade(SQLMsg* pmsg);

		typedef std::map<DWORD, TGuildWar> TEnemyGuildContainer;
		TEnemyGuildContainer m_EnemyGuild;

		std::map<DWORD, DWORD> m_mapGuildWarEndTime;

		bool	abSkillUsable[GUILD_SKILL_COUNT];

		// GUILD_JOIN_BUG_FIX
		enum GuildJoinErrCode {
			GERR_NONE			= 0,
			GERR_WITHDRAWPENALTY,
			GERR_COMMISSIONPENALTY,
			GERR_ALREADYJOIN,
			GERR_GUILDISFULL,
			GERR_GUILD_IS_IN_WAR,
			GERR_INVITE_LIMIT,
			GERR_MAX
		};

		GuildJoinErrCode	VerifyGuildJoinableCondition( const LPCHARACTER pchInvitee );

		typedef std::map< DWORD, LPEVENT >	EventMap;
		EventMap	m_GuildInviteEventMap;
		// END_OF_GUILD_JOIN_BUG_FIX
};

#define ENABLE_NEWGUILDMAKE

#ifdef ENABLE_D_NJGUILD
#include "char.h"
template <class Func> void CGuild::ForEachOnMapMember (Func & f, long lMapIndex)
{
	TGuildMemberOnlineContainer::iterator it;

	for (it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		LPCHARACTER ch = *it;
		if (ch)
		{
			if (ch->GetMapIndex () == lMapIndex)
				f(ch);
		}
	}
}
#endif

#endif
