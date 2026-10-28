#include "stdafx.h"

#include "../../common/service.h"

#include "constants.h"
#include "priv_manager.h"
#include "char.h"
#include "desc_client.h"
#include "guild.h"
#include "guild_manager.h"
#include "unique_item.h"
#include "utils.h"
#include "log.h"

static const char * GetEmpireName(int priv)
{
	return c_apszEmpireNames[priv];
}

static const char * GetPrivName(int priv)
{
	return c_apszPrivNames[priv];
}

CPrivManager::CPrivManager()
{
	memset(m_aakPrivEmpireData, 0, sizeof(m_aakPrivEmpireData));
}

void CPrivManager::RequestGiveGuildPriv(DWORD guild_id, BYTE type, int value, time_t duration_sec)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: RequestGiveGuildPriv: wrong guild priv type(%u)", type);
		return;
	}

	value = MINMAX(0, value, 1000);
	duration_sec = MINMAX(0, duration_sec, 60*60*24*7);

	TPacketGiveGuildPriv p;
	p.type = type;
	p.value = value;
	p.guild_id = guild_id;
	p.duration_sec = duration_sec;

	db_clientdesc->DBPacket(HEADER_GD_REQUEST_GUILD_PRIV, 0, &p, sizeof(p));
}

void CPrivManager::RequestGiveEmpirePriv(BYTE empire, BYTE type, int value, time_t duration_sec)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: RequestGiveEmpirePriv: wrong empire priv type(%u)", type);
		return;
	}

	value = MINMAX(0, value, 1000);
	duration_sec = MINMAX(0, duration_sec, 60*60*24*7);

	TPacketGiveEmpirePriv p;
	p.type = type;
	p.value = value;
	p.empire = empire;
	p.duration_sec = duration_sec;

	db_clientdesc->DBPacket(HEADER_GD_REQUEST_EMPIRE_PRIV, 0, &p, sizeof(p));
}

void CPrivManager::RequestGiveCharacterPriv(DWORD pid, BYTE type, int value)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: RequestGiveCharacterPriv: wrong char priv type(%u)", type);
		return;
	}

	value = MINMAX(0, value, 1000);

	TPacketGiveCharacterPriv p;
	p.type = type;
	p.value = value;
	p.pid = pid;

	db_clientdesc->DBPacket(HEADER_GD_REQUEST_CHARACTER_PRIV, 0, &p, sizeof(p));
}

void CPrivManager::GiveGuildPriv(DWORD guild_id, BYTE type, int value, BYTE bLog, time_t end_time_sec)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: GiveGuildPriv: wrong guild priv type(%u)", type);
		return;
	}

	sys_log(0,"Set Guild Priv: guild_id(%u) type(%d) value(%d) duration_sec(%d)", guild_id, type, value, end_time_sec - get_global_time());

	value = MINMAX(0, value, 1000);
	end_time_sec = MINMAX(0, end_time_sec, get_global_time()+60*60*24*7);

	m_aPrivGuild[type][guild_id].value = value;
	m_aPrivGuild[type][guild_id].end_time_sec = end_time_sec;

	CGuild* g = CGuildManager::instance().FindGuild(guild_id);

	if (g)
	{
		if (value)
			SendLocaleNotice(CHAT_TYPE_NOTICE, 0, 0, 543, "%s#%s#%d", GetPrivName(type), g->GetName(), value);
		else
			SendLocaleNotice(CHAT_TYPE_NOTICE, 0, 0, 544, "%s#%s", GetPrivName(type), g->GetName());

		if (bLog)
			LogManager::instance().CharLog(0, guild_id, type, value, "GUILD_PRIV", "", "");
	}
}

void CPrivManager::GiveCharacterPriv(DWORD pid, BYTE type, int value, BYTE bLog)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: GiveCharacterPriv: wrong char priv type(%u)", type);
		return;
	}

	sys_log(0,"Set Character Priv %u %d %d", pid, type, value);

	value = MINMAX(0, value, 1000);

	m_aPrivChar[type][pid] = value;

	if (bLog)
		LogManager::instance().CharLog(pid, 0, type, value, "CHARACTER_PRIV", "", "");
}

void CPrivManager::GiveEmpirePriv(BYTE empire, BYTE type, int value, BYTE bLog, time_t end_time_sec)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: GiveEmpirePriv: wrong empire priv type(%u)", type);
		return;
	}

	sys_log(0, "Set Empire Priv: empire(%d) type(%d) value(%d) duration_sec(%d)", empire, type, value, end_time_sec-get_global_time());

	value = MINMAX(0, value, 1000);
	end_time_sec = MINMAX(0, end_time_sec, get_global_time()+60*60*24*7);

	SPrivEmpireData& rkPrivEmpireData=m_aakPrivEmpireData[type][empire];
	rkPrivEmpireData.m_value = value;
	rkPrivEmpireData.m_end_time_sec = end_time_sec;

	if (value)
		SendLocaleNotice(CHAT_TYPE_NOTICE, 0, 0, 541, "%s#%s#%d", GetPrivName(type), GetEmpireName(empire), value);
	else
		SendLocaleNotice(CHAT_TYPE_NOTICE, 0, 0, 542, "%s#%s", GetPrivName(type), GetEmpireName(empire));

	if (bLog)
		LogManager::instance().CharLog(0, empire, type, value, "EMPIRE_PRIV", "", "");
}

void CPrivManager::RemoveGuildPriv(DWORD guild_id, BYTE type)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: RemoveGuildPriv: wrong guild priv type(%u)", type);
		return;
	}

	m_aPrivGuild[type][guild_id].value = 0;
	m_aPrivGuild[type][guild_id].end_time_sec = 0;
}

void CPrivManager::RemoveEmpirePriv(BYTE empire, BYTE type)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: RemoveEmpirePriv: wrong empire priv type(%u)", type);
		return;
	}

	SPrivEmpireData& rkPrivEmpireData=m_aakPrivEmpireData[type][empire];
	rkPrivEmpireData.m_value = 0;
	rkPrivEmpireData.m_end_time_sec = 0;
}

void CPrivManager::RemoveCharacterPriv(DWORD pid, BYTE type)
{
	if (MAX_PRIV_NUM <= type)
	{
		sys_err("PRIV_MANAGER: RemoveCharacterPriv: wrong char priv type(%u)", type);
		return;
	}

	itertype(m_aPrivChar[type]) it = m_aPrivChar[type].find(pid);

	if (it != m_aPrivChar[type].end())
		m_aPrivChar[type].erase(it);
}

int CPrivManager::GetPriv(LPCHARACTER ch, BYTE type)
{
	int val_ch = GetPrivByCharacter(ch->GetPlayerID(), type);

	if (val_ch < 0 && !ch->IsEquipUniqueItem(UNIQUE_ITEM_NO_BAD_LUCK_EFFECT))
		return val_ch;
	else
	{
		int val;

		val = MAX(val_ch, GetPrivByEmpire(0, type));
		val = MAX(val, GetPrivByEmpire(ch->GetEmpire(), type));

		if (ch->GetGuild())
			val = MAX(val, GetPrivByGuild(ch->GetGuild()->GetID(), type));

		return val;
	}
}

int CPrivManager::GetPrivByEmpire(BYTE bEmpire, BYTE type)
{
	SPrivEmpireData* pkPrivEmpireData = GetPrivByEmpireEx(bEmpire, type);

	if (pkPrivEmpireData)
		return pkPrivEmpireData->m_value;

	return 0;
}

CPrivManager::SPrivEmpireData* CPrivManager::GetPrivByEmpireEx(BYTE bEmpire, BYTE type)
{
	if (type >= MAX_PRIV_NUM)
		return NULL;

	if (bEmpire >= EMPIRE_MAX_NUM)
		return NULL;

	return &m_aakPrivEmpireData[type][bEmpire];
}

int CPrivManager::GetPrivByGuild(DWORD guild_id, BYTE type)
{
	if (type >= MAX_PRIV_NUM)
		return 0;

	itertype( m_aPrivGuild[ type ] ) itFind = m_aPrivGuild[ type ].find( guild_id );

	if ( itFind == m_aPrivGuild[ type ].end() )
		return 0;

	return itFind->second.value;
}

const CPrivManager::SPrivGuildData* CPrivManager::GetPrivByGuildEx( DWORD dwGuildID, BYTE byType ) const
{
	if ( byType >= MAX_PRIV_NUM )
		return NULL;

	itertype( m_aPrivGuild[ byType ] ) itFind = m_aPrivGuild[ byType ].find( dwGuildID );

	if ( itFind == m_aPrivGuild[ byType ].end() )
		return NULL;

	return &itFind->second;
}

int CPrivManager::GetPrivByCharacter(DWORD pid, BYTE type)
{
	if (type >= MAX_PRIV_NUM)
		return 0;

	itertype(m_aPrivChar[type]) it = m_aPrivChar[type].find(pid);

	if (it != m_aPrivChar[type].end())
		return it->second;

	return 0;
}

