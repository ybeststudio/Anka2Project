#ifndef __INC_METIN_II_GAME_PVP_H__
#define __INC_METIN_II_GAME_PVP_H__

class CHARACTER;

class CPVP
{
	public:
		friend class CPVPManager;

		typedef struct _player
		{
			DWORD	dwPID;
			DWORD	dwVID;
			bool	bAgree;
			bool	bCanRevenge;

			_player() : dwPID(0), dwVID(0), bAgree(false), bCanRevenge(false)
			{
			}
		} TPlayer;

		CPVP(DWORD dwID1, DWORD dwID2);
		CPVP(CPVP & v);
		~CPVP();

		void	Win(DWORD dwPID);
		bool	CanRevenge(DWORD dwPID);
		bool	IsFight();
		bool	Agree(DWORD dwPID);

		void	SetVID(DWORD dwPID, DWORD dwVID);
		void	Packet(bool bDelete = false);

		void	SetLastFightTime();
		DWORD	GetLastFightTime();

		DWORD 	GetCRC() { return m_dwCRC; }

	protected:
		TPlayer	m_players[2];
		DWORD	m_dwCRC;
		bool	m_bRevenge;

		DWORD   m_dwLastFightTime;
};

class CPVPManager : public singleton<CPVPManager>
{
	typedef std::map<DWORD, std::unordered_set<CPVP*> > CPVPSetMap;

	public:
	CPVPManager();
	virtual ~CPVPManager();

	void			Insert(LPCHARACTER pkChr, LPCHARACTER pkVictim);
	bool			CanAttack(LPCHARACTER pkChr, LPCHARACTER pkVictim);
	bool			Dead(LPCHARACTER pkChr, DWORD dwKillerPID);
	void			GiveUp(LPCHARACTER pkChr, DWORD dwKillerPID);
	void			Connect(LPCHARACTER pkChr);
	void			Disconnect(LPCHARACTER pkChr);

	void			SendList(LPDESC d);
	void			Delete(CPVP * pkPVP);

	void			Process();

	public:
	CPVP *			Find(DWORD dwCRC);
	protected:
	void			ConnectEx(LPCHARACTER pkChr, bool bDisconnect);

	std::map<DWORD, CPVP *>	m_map_pkPVP;
	CPVPSetMap		m_map_pkPVPSetByID;
};

#endif
