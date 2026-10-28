#ifndef __INC_MESSENGER_MANAGER_H
#define __INC_MESSENGER_MANAGER_H

#include "db.h"
#include "../../common/service.h"

class MessengerManager : public singleton<MessengerManager>
{
	public:
		typedef std::string keyT;
		typedef const std::string & keyA;

#ifdef ENABLE_MESSENGER_BLOCK
		typedef std::string keyBL;
		typedef const std::string& keyB;
#endif

		MessengerManager();
		virtual ~MessengerManager();

	public:
		void	P2PLogin(keyA account);
		void	P2PLogout(keyA account);

		void	Login(keyA account);
		void	Logout(keyA account);

#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
		void	RegisterRequestToAdd(const char* szAccount, const char* szTarget);
		void	P2PRequestToAdd_Stage1(LPCHARACTER ch, const char* targetName);
		void	P2PRequestToAdd_Stage2(const char* characterName, LPCHARACTER target);
#endif

		void	RequestToAdd(LPCHARACTER ch, LPCHARACTER target);
		bool	AuthToAdd(keyA account, keyA companion, bool bDeny);

		void	__AddToList(keyA account, keyA companion);
		void	AddToList(keyA account, keyA companion);

		void	__RemoveFromList(keyA account, keyA companion);
		void	RemoveFromList(keyA account, keyA companion);

#ifdef ENABLE_MESSENGER_BLOCK
		void	__AddToBlockList(keyB account, keyB companion);
		void	AddToBlockList(keyB account, keyB companion);

		void	__RemoveFromBlockList(keyB account, keyB companion);
		bool	IsBlocked(keyB account, keyB companion);
		void	RemoveFromBlockList(keyB account, keyB companion);
		void	RemoveAllBlockList(keyB account);
#endif

		void	RemoveAllList(keyA account);

		void	Initialize();
		
		bool	IsInList(MessengerManager::keyA account, MessengerManager::keyA companion);

	private:
		void	SendList(keyA account);
		void	SendLogin(keyA account, keyA companion);
		void	SendLogout(keyA account, keyA companion);

		void	LoadList(SQLMsg * pmsg);

#ifdef ENABLE_MESSENGER_TEAM
		void	SendTeamLogin(keyA account, keyA companion);
		void	SendTeamLogout(keyA account, keyA companion);

		void	LoadTeamList(SQLMsg * pmsg);
		void	SendTeamList(keyA account);
#endif

#ifdef ENABLE_MESSENGER_BLOCK
		void SendBlockList(keyB account);
		void SendBlockLogin(keyB account, keyB companion);
		void SendBlockLogout(keyB account, keyB companion);

		void LoadBlockList(SQLMsg* pmsg);
#endif

		void	Destroy();

		std::set<keyT>			m_set_loginAccount;
		std::map<keyT, std::set<keyT> >	m_Relation;
		std::map<keyT, std::set<keyT> >	m_InverseRelation;
		std::set<DWORD>			m_set_requestToAdd;
#ifdef ENABLE_MESSENGER_TEAM
		std::map<keyT, std::set<keyT> >	m_TeamRelation;
		std::map<keyT, std::set<keyT> >	m_InverseTeamRelation;
#endif
#ifdef ENABLE_MESSENGER_BLOCK
		std::map<keyBL, std::set<keyBL> > m_BlockRelation;
		std::map<keyBL, std::set<keyBL> > m_InverseBlockRelation;
		std::set<DWORD> m_set_requestToBlockAdd;
#endif
};

#endif
