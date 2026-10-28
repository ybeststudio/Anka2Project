#include "stdafx.h"
#include "constants.h"
#include "gm.h"
#include "messenger_manager.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "log.h"
#include "config.h"
#include "p2p.h"
#include "crc32.h"
#include "char.h"
#include "char_manager.h"
#include "questmanager.h"

#include "../../common/service.h"

MessengerManager::MessengerManager()
{
}

MessengerManager::~MessengerManager()
{
}

void MessengerManager::Initialize()
{
}

void MessengerManager::Destroy()
{
}

void MessengerManager::P2PLogin(MessengerManager::keyA account)
{
	Login(account);
}

void MessengerManager::P2PLogout(MessengerManager::keyA account)
{
	Logout(account);
}

void MessengerManager::Login(MessengerManager::keyA account)
{
	if (m_set_loginAccount.find(account) != m_set_loginAccount.end())
		return;

	DBManager::instance().FuncQuery(std::bind(&MessengerManager::LoadList, this, std::placeholders::_1),
			"SELECT account, companion FROM messenger_list%s WHERE account='%s'", get_table_postfix(), account.c_str());

#ifdef ENABLE_MESSENGER_TEAM
	DBManager::instance().FuncQuery(std::bind(&MessengerManager::LoadTeamList, this, std::placeholders::_1),
			"SELECT '%s',mName FROM common.gmlist WHERE mName!='%s'", account.c_str(), account.c_str());
#endif

#ifdef ENABLE_MESSENGER_BLOCK
	DBManager::instance().FuncQuery(std::bind(&MessengerManager::LoadBlockList, this, std::placeholders::_1),
			"SELECT account, companion FROM messenger_block_list%s WHERE account='%s'", get_table_postfix(), account.c_str());
#endif

	m_set_loginAccount.insert(account);
}

#ifdef ENABLE_MESSENGER_TEAM
void MessengerManager::LoadTeamList(SQLMsg * msg)
{
	if (NULL == msg or NULL == msg->Get() or msg->Get()->uiNumRows == 0)
		return;
	
	std::string account;

	for (uint i = 0; i < msg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		if (row[0] && row[1])
		{
			if (account.length() == 0)
				account = row[0];

			m_TeamRelation[row[0]].insert(row[1]);
			m_InverseTeamRelation[row[1]].insert(row[0]);
		}
	}

	SendTeamList(account);

	std::set<MessengerManager::keyT>::iterator it;

	for (it = m_InverseTeamRelation[account].begin(); it != m_InverseTeamRelation[account].end(); ++it)
		SendTeamLogin(*it, account);
}

void MessengerManager::SendTeamList(MessengerManager::keyA account)
{
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());

	if (!ch)
		return;

	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	TPacketGCMessenger pack;

	pack.header		= HEADER_GC_MESSENGER;
	pack.subheader	= MESSENGER_SUBHEADER_GC_TEAM_LIST;
	pack.size		= sizeof(TPacketGCMessenger);

	TPacketGCMessengerTeamListOffline pack_offline;
	TPacketGCMessengerTeamListOnline pack_online;

	TEMP_BUFFER buf(128 * 1024);

	itertype(m_TeamRelation[account]) it = m_TeamRelation[account].begin(), eit = m_TeamRelation[account].end();

	while (it != eit)
	{
		if (m_set_loginAccount.find(*it) != m_set_loginAccount.end())
		{
			pack_online.connected = 1;
			pack_online.length = it->size();

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			pack_online.lang = ch->GetCountryFlagFromGMList(1, it->c_str(), true);
			pack_online.lang2nd = ch->GetCountryFlagFromGMList(2, it->c_str(), true);
#endif

			buf.write(&pack_online, sizeof(TPacketGCMessengerTeamListOnline));
			buf.write(it->c_str(), it->size());
		}
		else
		{
			pack_offline.connected = 0;
			pack_offline.length = it->size();

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			pack_offline.lang = ch->GetCountryFlagFromGMList(1, it->c_str(), true);
			pack_offline.lang2nd = ch->GetCountryFlagFromGMList(2, it->c_str(), true);
#endif

			buf.write(&pack_offline, sizeof(TPacketGCMessengerTeamListOffline));
			buf.write(it->c_str(), it->size());
		}

		++it;
	}

	pack.size += buf.size();

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->Packet(buf.read_peek(), buf.size());
}

void MessengerManager::SendTeamLogin(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : NULL;

	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	BYTE bLen = companion.size();

	TPacketGCMessenger pack;

	pack.header			= HEADER_GC_MESSENGER;
	pack.subheader		= MESSENGER_SUBHEADER_GC_TEAM_LOGIN;
	pack.size			= sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}

void MessengerManager::SendTeamLogout(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	if (!companion.size())
		return;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : NULL;

	if (!d)
		return;

	BYTE bLen = companion.size();

	TPacketGCMessenger pack;

	pack.header		= HEADER_GC_MESSENGER;
	pack.subheader	= MESSENGER_SUBHEADER_GC_TEAM_LOGOUT;
	pack.size		= sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}
#endif

#ifdef ENABLE_MESSENGER_BLOCK
void MessengerManager::SendBlockList(MessengerManager::keyB account)
{
	LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindPC(account.c_str());

	if (!ch)
		return;

	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	TPacketGCMessenger pack;

	pack.header = HEADER_GC_MESSENGER;
	pack.subheader = MESSENGER_SUBHEADER_GC_BLOCK_LIST;
	pack.size = sizeof(TPacketGCMessenger);

	TPacketGCMessengerBlockListOnline pack_online{};
	TPacketGCMessengerBlockListOffline pack_offline{};

	TEMP_BUFFER buf(128 * 1024);

	itertype(m_BlockRelation[account]) it = m_BlockRelation[account].begin(), eit = m_BlockRelation[account].end();

	while (it != eit)
	{
		if (m_set_loginAccount.find(*it) != m_set_loginAccount.end())
		{
			pack_online.connected = 1;
			pack_online.length = it->size();

			buf.write(&pack_online, sizeof(TPacketGCMessengerBlockListOnline));
			buf.write(it->c_str(), it->size());
		}
		else
		{
			pack_offline.connected = 0;
			pack_offline.length = it->size();

			buf.write(&pack_offline, sizeof(TPacketGCMessengerBlockListOffline));
			buf.write(it->c_str(), it->size());
		}

		++it;
	}

	pack.size += buf.size();

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->Packet(buf.read_peek(), buf.size());
}

void MessengerManager::SendBlockLogin(MessengerManager::keyB account, MessengerManager::keyB companion)
{
	LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : nullptr;

	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(companion.c_str()) != GM_PLAYER)
		return;

	BYTE bLen = companion.size();

	TPacketGCMessenger pack;

	pack.header = HEADER_GC_MESSENGER;
	pack.subheader = MESSENGER_SUBHEADER_GC_BLOCK_LOGIN;
	pack.size = sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}

void MessengerManager::SendBlockLogout(MessengerManager::keyB account, MessengerManager::keyB companion)
{
	if (!companion.size())
		return;

	LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : nullptr;

	if (!d)
		return;

	if (m_BlockRelation.find(account) == m_BlockRelation.end())
		return;

	if (m_BlockRelation[account].empty())
		return;

	BYTE bLen = companion.size();

	TPacketGCMessenger pack;

	pack.header = HEADER_GC_MESSENGER;
	pack.subheader = MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT;
	pack.size = sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}

void MessengerManager::LoadBlockList(SQLMsg* msg)
{
	if (nullptr == msg || nullptr == msg->Get() || msg->Get()->uiNumRows == 0)
		return;

	std::string account;

	for (uint i = 0; i < msg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		if (row[0] && row[1])
		{
			if (account.length() == 0)
				account = row[0];

			m_BlockRelation[row[0]].insert(row[1]);
			m_InverseBlockRelation[row[1]].insert(row[0]);
		}
	}

	SendBlockList(account);

	std::set<MessengerManager::keyBL>::iterator it;

	for (it = m_InverseBlockRelation[account].begin(); it != m_InverseBlockRelation[account].end(); ++it)
		SendBlockLogin(*it, account);
}

void MessengerManager::__AddToBlockList(MessengerManager::keyB account, MessengerManager::keyB companion)
{
	m_BlockRelation[account].insert(companion);
	m_InverseBlockRelation[companion].insert(account);

	LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : nullptr;

	if (d)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s is now blocked."), companion.c_str());
	}

	LPCHARACTER tch = CHARACTER_MANAGER::Instance().FindPC(companion.c_str());

	if (tch)
		SendBlockLogin(account, companion);
	else
		SendBlockLogout(account, companion);
}

void MessengerManager::AddToBlockList(MessengerManager::keyB account, MessengerManager::keyB companion)
{
	if (companion.size() == 0)
		return;

	if (m_BlockRelation[account].find(companion) != m_BlockRelation[account].end())
		return;

	sys_log(0, "Messenger_Block Add %s %s", account.c_str(), companion.c_str());
	DBManager::Instance().Query("INSERT INTO messenger_block_list%s VALUES ('%s', '%s', NOW())",
		get_table_postfix(), account.c_str(), companion.c_str());

	__AddToBlockList(account, companion);

	TPacketGGMessenger p2ppck{};

	p2ppck.bHeader = HEADER_GG_MESSENGER_BLOCK_ADD;
	strlcpy(p2ppck.szAccount, account.c_str(), sizeof(p2ppck.szAccount));
	strlcpy(p2ppck.szCompanion, companion.c_str(), sizeof(p2ppck.szCompanion));
	P2P_MANAGER::Instance().Send(&p2ppck, sizeof(TPacketGGMessenger));
}

void MessengerManager::__RemoveFromBlockList(MessengerManager::keyB account, MessengerManager::keyB companion)
{
	m_BlockRelation[account].erase(companion);
	m_InverseBlockRelation[companion].erase(account);

	LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : nullptr;

	if (d)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s is no longer blocked."), companion.c_str());
}

bool MessengerManager::IsBlocked(MessengerManager::keyB account, MessengerManager::keyB companion)
{
	if (m_BlockRelation.find(account) == m_BlockRelation.end())
		return false;

	if (m_BlockRelation[account].empty())
		return false;

	return m_BlockRelation[account].find(companion) != m_BlockRelation[account].end();
}

void MessengerManager::RemoveFromBlockList(MessengerManager::keyB account, MessengerManager::keyB companion)
{
	if (companion.size() == 0)
		return;

	sys_log(1, "Messenger Remove %s %s", account.c_str(), companion.c_str());
	DBManager::Instance().Query("DELETE FROM messenger_block_list%s WHERE account='%s' AND companion = '%s'",
		get_table_postfix(), account.c_str(), companion.c_str());

	__RemoveFromBlockList(account, companion);

	TPacketGGMessenger p2ppck{};

	p2ppck.bHeader = HEADER_GG_MESSENGER_BLOCK_REMOVE;
	strlcpy(p2ppck.szAccount, account.c_str(), sizeof(p2ppck.szAccount));
	strlcpy(p2ppck.szCompanion, companion.c_str(), sizeof(p2ppck.szCompanion));
	P2P_MANAGER::Instance().Send(&p2ppck, sizeof(TPacketGGMessenger));
}

void MessengerManager::RemoveAllBlockList(keyB account)
{
	std::set<keyBL>	company(m_BlockRelation[account]);

	DBManager::Instance().Query("DELETE FROM messenger_block_list%s WHERE account='%s' OR companion='%s'",
		get_table_postfix(), account.c_str(), account.c_str());

	for (std::set<keyBL>::iterator iter = company.begin(); iter != company.end(); ++iter)	//@fixme541
	{
		this->RemoveFromBlockList(account, *iter);
	}

	for (std::set<keyBL>::iterator iter = company.begin(); iter != company.end();)
	{
		company.erase(iter++);
	}

	company.clear();
}
#endif

void MessengerManager::LoadList(SQLMsg * msg)
{
	if (NULL == msg)
		return;

	if (NULL == msg->Get())
		return;

	if (msg->Get()->uiNumRows == 0)
		return;

	std::string account;

	for (uint i = 0; i < msg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		if (row[0] && row[1])
		{
			if (account.length() == 0)
				account = row[0];

			m_Relation[row[0]].insert(row[1]);
			m_InverseRelation[row[1]].insert(row[0]);
		}
	}

	SendList(account);

	std::set<MessengerManager::keyT>::iterator it;

	for (it = m_InverseRelation[account].begin(); it != m_InverseRelation[account].end(); ++it)
		SendLogin(*it, account);
}

void MessengerManager::Logout(MessengerManager::keyA account)
{
	if (m_set_loginAccount.find(account) == m_set_loginAccount.end())
		return;

	m_set_loginAccount.erase(account);

	std::set<MessengerManager::keyT>::iterator it;

	for (it = m_InverseRelation[account].begin(); it != m_InverseRelation[account].end(); ++it)
	{
		SendLogout(*it, account);
	}

	std::map<keyT, std::set<keyT> >::iterator it2 = m_Relation.begin();

	while (it2 != m_Relation.end())
	{
		it2->second.erase(account);
		++it2;
	}

	m_Relation.erase(account);

#ifdef ENABLE_MESSENGER_TEAM
	std::set<MessengerManager::keyT>::iterator it5;
	
	for (it5 = m_InverseTeamRelation[account].begin(); it5 != m_InverseTeamRelation[account].end(); ++it5)
	{
		SendTeamLogout(*it5, account);
	}

	std::map<keyT, std::set<keyT> >::iterator it6 = m_TeamRelation.begin();

	while (it6 != m_TeamRelation.end())
	{
		it6->second.erase(account);
		++it6;
	}

	m_TeamRelation.erase(account);
#endif

#ifdef ENABLE_MESSENGER_BLOCK
	std::set<MessengerManager::keyBL>::iterator it3;

	for (it3 = m_InverseBlockRelation[account].begin(); it3 != m_InverseBlockRelation[account].end(); ++it3)
	{
		SendBlockLogout(*it3, account);
	}

	std::map<keyBL, std::set<keyBL> >::iterator it4 = m_BlockRelation.begin();

	while (it4 != m_BlockRelation.end())
	{
		it4->second.erase(account);
		++it4;
	}
	m_BlockRelation.erase(account);
#endif
}

#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
void MessengerManager::RegisterRequestToAdd(const char* name, const char* targetName)
{
	uint32_t dw1 = GetCRC32(name, strlen(name));
	uint32_t dw2 = GetCRC32(targetName, strlen(targetName));

	char buf[64]{0,};
	snprintf(buf, sizeof(buf), "%u:%u", dw1, dw2);
	buf[63] = '\0';

	uint32_t dwComplex = GetCRC32(buf, strlen(buf));

	m_set_requestToAdd.insert(dwComplex);
}

void MessengerManager::P2PRequestToAdd_Stage1(LPCHARACTER ch, const char* targetName)
{
	LPCHARACTER pkTarget = CHARACTER_MANAGER::Instance().FindPC(targetName);

	if (!pkTarget)
	{
		if (!ch || !ch->IsPC())
			return;

		if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot add the person standing opposite you to your friends."));
			return;
		}

		TPacketGGMessengerRequest p2pp{};
		p2pp.header = HEADER_GG_MESSENGER_REQUEST_ADD;
		strlcpy(p2pp.account, ch->GetName(), CHARACTER_NAME_MAX_LEN + 1);
		strlcpy(p2pp.target, targetName, CHARACTER_NAME_MAX_LEN + 1);
		P2P_MANAGER::Instance().Send(&p2pp, sizeof(TPacketGGMessengerRequest));
	}
	else
		RequestToAdd(ch, pkTarget);
}

void MessengerManager::P2PRequestToAdd_Stage2(const char* characterName, LPCHARACTER target)
{
	if (!target || !target->IsPC())
		return;

	if (quest::CQuestManager::instance().GetPCForce(target->GetPlayerID())->IsRunning())
		return;

	if (target->IsBlockMode(BLOCK_MESSENGER_INVITE))
		return;

	MessengerManager::Instance().RegisterRequestToAdd(characterName, target->GetName());
	target->ChatPacket(CHAT_TYPE_COMMAND, "messenger_auth %s", characterName);
}
#endif

void MessengerManager::RequestToAdd(LPCHARACTER ch, LPCHARACTER target)
{
	if (!ch->IsPC() || !target->IsPC())
		return;

	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot add the person standing opposite you to your friends."));
		return;
	}

	if (quest::CQuestManager::instance().GetPCForce(target->GetPlayerID())->IsRunning() == true)
		return;

	DWORD dw1 = GetCRC32(ch->GetName(), strlen(ch->GetName()));
	DWORD dw2 = GetCRC32(target->GetName(), strlen(target->GetName()));

	char buf[64];
	snprintf(buf, sizeof(buf), "%u:%u", dw1, dw2);
	DWORD dwComplex = GetCRC32(buf, strlen(buf));

	m_set_requestToAdd.insert(dwComplex);

	target->ChatPacket(CHAT_TYPE_COMMAND, "messenger_auth %s", ch->GetName());
}

bool MessengerManager::AuthToAdd(MessengerManager::keyA account, MessengerManager::keyA companion, bool bDeny)
{
	DWORD dw1 = GetCRC32(companion.c_str(), companion.length());
	DWORD dw2 = GetCRC32(account.c_str(), account.length());

	char buf[64];
	snprintf(buf, sizeof(buf), "%u:%u", dw1, dw2);
	DWORD dwComplex = GetCRC32(buf, strlen(buf));

	if (m_set_requestToAdd.find(dwComplex) == m_set_requestToAdd.end())
	{
		sys_log(0, "MessengerManager::AuthToAdd : request not exist %s -> %s", companion.c_str(), account.c_str());
		return false;
	}

	m_set_requestToAdd.erase(dwComplex);

	if (!bDeny)
	{
		AddToList(companion, account);
		AddToList(account, companion);
	}

	return true;
}

void MessengerManager::__AddToList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	m_Relation[account].insert(companion);
	m_InverseRelation[companion].insert(account);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : NULL;

	if (d)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Friends] You have added %s as a friend."), companion.c_str());
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(companion.c_str());

#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
	if (tch || P2P_MANAGER::Instance().Find(companion.c_str()))
#else
	if (tch)
#endif
		SendLogin(account, companion);
	else
		SendLogout(account, companion);
}

void MessengerManager::AddToList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	if (companion.size() == 0)
		return;

	if (m_Relation[account].find(companion) != m_Relation[account].end())
		return;

	sys_log(0, "Messenger Add %s %s", account.c_str(), companion.c_str());
	DBManager::instance().Query("INSERT INTO messenger_list%s VALUES ('%s', '%s')", 
			get_table_postfix(), account.c_str(), companion.c_str());

	__AddToList(account, companion);

	TPacketGGMessenger p2ppck;

	p2ppck.bHeader = HEADER_GG_MESSENGER_ADD;
	strlcpy(p2ppck.szAccount, account.c_str(), sizeof(p2ppck.szAccount));
	strlcpy(p2ppck.szCompanion, companion.c_str(), sizeof(p2ppck.szCompanion));
	P2P_MANAGER::instance().Send(&p2ppck, sizeof(TPacketGGMessenger));
}

void MessengerManager::__RemoveFromList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	m_Relation[account].erase(companion);
	m_InverseRelation[companion].erase(account);

	m_Relation[companion].erase(account);
	m_InverseRelation[account].erase(companion);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : NULL;

	if (d)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Friends] You have deleted %s from the list."), companion.c_str());

	LPCHARACTER tch = CHARACTER_MANAGER::Instance().FindPC(companion.c_str());
	if (tch && tch->GetDesc())
	{
		TPacketGCMessenger p;
		p.header = HEADER_GC_MESSENGER;
		p.subheader = MESSENGER_SUBHEADER_GC_REMOVE_FRIEND;
		p.size = sizeof(TPacketGCMessenger) + sizeof(BYTE) + account.size();

		BYTE bLen = account.size();
		tch->GetDesc()->BufferedPacket(&p, sizeof(p));
		tch->GetDesc()->BufferedPacket(&bLen, sizeof(BYTE));
		tch->GetDesc()->Packet(account.c_str(), account.size());
	}
}

bool MessengerManager::IsInList(MessengerManager::keyA account, MessengerManager::keyA companion) // Fix
{
	if (m_Relation.find(account) == m_Relation.end())
		return false;

	if (m_Relation[account].empty())
		return false;

	return m_Relation[account].find(companion) != m_Relation[account].end();
}

#ifdef ENABLE_MESSENGER_MANAGER_FIX
void MessengerManager::RemoveFromList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	if (companion.empty())
		return;

	if (companion.size() == 0)
		return;

	if (!IsInList(account, companion))
		return;

	if (m_Relation[account].find(companion) == m_Relation[account].end() || m_InverseRelation[companion].find(account) == m_InverseRelation[companion].end())
	{
		LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindPC(account.c_str());
		if (ch)
		{
			sys_err("MessengerManager::RemoveFromList: %s tried to use messenger sql injection", ch->GetName());
			DBManager::Instance().Query("UPDATE account.account SET status = 'BAN' WHERE id = %u", ch->GetAID()); // FIX(leak)
			if (ch->GetDesc())
				ch->GetDesc()->DelayedDisconnect(3);
		}
		else
			sys_err("MessengerManager::RemoveFromList: Omg! The ghost tried to use this function!");
		return;
	}

	sys_log(1, "MessengerManager::RemoveFromList: Remove %s %s", account.c_str(), companion.c_str());
	DBManager::instance().Query("DELETE FROM messenger_list%s WHERE (account='%s' AND companion = '%s') OR (account = '%s' AND companion = '%s')", get_table_postfix(), account.c_str(), companion.c_str(), companion.c_str(), account.c_str());

	__RemoveFromList(account, companion);

	TPacketGGMessenger p2ppck;

	p2ppck.bHeader = HEADER_GG_MESSENGER_REMOVE;
	strlcpy(p2ppck.szAccount, account.c_str(), sizeof(p2ppck.szAccount));
	strlcpy(p2ppck.szCompanion, companion.c_str(), sizeof(p2ppck.szCompanion));
	P2P_MANAGER::instance().Send(&p2ppck, sizeof(TPacketGGMessenger));
}
#else
void MessengerManager::RemoveFromList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	if (companion.size() == 0)
		return;

	sys_log(1, "Messenger Remove %s %s", account.c_str(), companion.c_str());
	DBManager::instance().Query("DELETE FROM messenger_list%s WHERE account='%s' AND companion = '%s'",
			get_table_postfix(), account.c_str(), companion.c_str());

	__RemoveFromList(account, companion);

	TPacketGGMessenger p2ppck;

	p2ppck.bHeader = HEADER_GG_MESSENGER_REMOVE;
	strlcpy(p2ppck.szAccount, account.c_str(), sizeof(p2ppck.szAccount));
	strlcpy(p2ppck.szCompanion, companion.c_str(), sizeof(p2ppck.szCompanion));
	P2P_MANAGER::instance().Send(&p2ppck, sizeof(TPacketGGMessenger));
}
#endif

void MessengerManager::RemoveAllList(keyA account)
{
	std::set<keyT>	company(m_Relation[account]);

	DBManager::instance().Query("DELETE FROM messenger_list%s WHERE account='%s' OR companion='%s'",
			get_table_postfix(), account.c_str(), account.c_str());

	for (std::set<keyT>::iterator iter = company.begin();
			iter != company.end();
			++iter)	//@fixme541
	{
		this->RemoveFromList(account, *iter);
	}

	for (std::set<keyT>::iterator iter = company.begin();
			iter != company.end();
			)
	{
		company.erase(iter++);
	}

	company.clear();
}

void MessengerManager::SendList(MessengerManager::keyA account)
{
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());

	if (!ch)
		return;

	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	if (m_Relation.find(account) == m_Relation.end())
		return;

	if (m_Relation[account].empty())
		return;

	TPacketGCMessenger pack;

	pack.header		= HEADER_GC_MESSENGER;
	pack.subheader	= MESSENGER_SUBHEADER_GC_LIST;
	pack.size		= sizeof(TPacketGCMessenger);

	TPacketGCMessengerListOffline pack_offline;
	TPacketGCMessengerListOnline pack_online;

	TEMP_BUFFER buf(128 * 1024); // 128k

	itertype(m_Relation[account]) it = m_Relation[account].begin(), eit = m_Relation[account].end();

	while (it != eit)
	{
		if (m_set_loginAccount.find(*it) != m_set_loginAccount.end())
		{
			pack_online.connected = 1;

			// Online
			pack_online.length = it->size();

			buf.write(&pack_online, sizeof(TPacketGCMessengerListOnline));
			buf.write(it->c_str(), it->size());
		}
		else
		{
			pack_offline.connected = 0;

			// Offline
			pack_offline.length = it->size();

			buf.write(&pack_offline, sizeof(TPacketGCMessengerListOffline));
			buf.write(it->c_str(), it->size());
		}

		++it;
	}

	pack.size += buf.size();

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->Packet(buf.read_peek(), buf.size());
}

void MessengerManager::SendLogin(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : NULL;

	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(companion.c_str()) != GM_PLAYER)
		return;

	BYTE bLen = companion.size();

	TPacketGCMessenger pack;

	pack.header			= HEADER_GC_MESSENGER;
	pack.subheader		= MESSENGER_SUBHEADER_GC_LOGIN;
	pack.size			= sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}

void MessengerManager::SendLogout(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	if (!companion.size())
		return;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : NULL;

	if (!d)
		return;

	BYTE bLen = companion.size();

	TPacketGCMessenger pack;

	pack.header		= HEADER_GC_MESSENGER;
	pack.subheader	= MESSENGER_SUBHEADER_GC_LOGOUT;
	pack.size		= sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}

