#include "StdAfx.h"
#include "PythonExchange.h"

void CPythonExchange::SetSelfName(const char *name)
{
	strncpy(m_self.name, name, CHARACTER_NAME_MAX_LEN);
}

void CPythonExchange::SetTargetName(const char *name)
{
	strncpy(m_victim.name, name, CHARACTER_NAME_MAX_LEN);
}

char *CPythonExchange::GetNameFromSelf()
{
	return m_self.name;
}

char *CPythonExchange::GetNameFromTarget()
{
	return m_victim.name;
}

#ifdef ENABLE_GOLD_LIMIT
void CPythonExchange::SetElkToTarget(long long elk)
#else
void CPythonExchange::SetElkToTarget(DWORD	elk)
#endif
{
	m_victim.elk = elk;
}

#ifdef ENABLE_GOLD_LIMIT
void CPythonExchange::SetElkToSelf(long long elk)
#else
void CPythonExchange::SetElkToSelf(DWORD elk)
#endif
{
	m_self.elk = elk;
}

#ifdef ENABLE_GOLD_LIMIT
long long CPythonExchange::GetElkFromTarget()
#else
DWORD CPythonExchange::GetElkFromTarget()
#endif
{
	return m_victim.elk;
}

#ifdef ENABLE_GOLD_LIMIT
long long CPythonExchange::GetElkFromSelf()
#else
DWORD CPythonExchange::GetElkFromSelf()
#endif
{
	return m_self.elk;
}

void CPythonExchange::SetSelfRace(DWORD race)
{
	m_self.race = race;
}

void CPythonExchange::SetTargetRace(DWORD race)
{
	m_victim.race = race;
}

DWORD CPythonExchange::GetRaceFromSelf()
{
	return m_self.race;
}

DWORD CPythonExchange::GetRaceFromTarget()
{
	return m_victim.race;
}

void CPythonExchange::SetSelfLevel(DWORD level)
{
	m_self.level = level;
}

void CPythonExchange::SetTargetLevel(DWORD level)
{
	m_victim.level = level;
}

DWORD CPythonExchange::GetLevelFromSelf()
{
	return m_self.level;
}

DWORD CPythonExchange::GetLevelFromTarget()
{
	return m_victim.level;
}

#ifdef ENABLE_STACK_LIMIT
void CPythonExchange::SetItemToTarget(DWORD pos, DWORD vnum, WORD count)
#else
void CPythonExchange::SetItemToTarget(DWORD pos, DWORD vnum, BYTE count)
#endif
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_victim.item_vnum[pos] = vnum;
	m_victim.item_count[pos] = count;
}

#ifdef ENABLE_STACK_LIMIT
void CPythonExchange::SetItemToSelf(DWORD pos, DWORD vnum, WORD count)
#else
void CPythonExchange::SetItemToSelf(DWORD pos, DWORD vnum, BYTE count)
#endif
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_self.item_vnum[pos] = vnum;
	m_self.item_count[pos] = count;
}

void CPythonExchange::SetItemMetinSocketToTarget(int pos, int imetinpos, DWORD vnum)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_victim.item_metin[pos][imetinpos] = vnum;
}

void CPythonExchange::SetItemMetinSocketToSelf(int pos, int imetinpos, DWORD vnum)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_self.item_metin[pos][imetinpos] = vnum;
}

void CPythonExchange::SetItemAttributeToTarget(int pos, int iattrpos, BYTE byType, short sValue)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_victim.item_attr[pos][iattrpos].bType = byType;
	m_victim.item_attr[pos][iattrpos].sValue = sValue;
}

void CPythonExchange::SetItemAttributeToSelf(int pos, int iattrpos, BYTE byType, short sValue)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_self.item_attr[pos][iattrpos].bType = byType;
	m_self.item_attr[pos][iattrpos].sValue = sValue;
}

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
void CPythonExchange::SetItemChangeLookVnumToSelf(const int pos, const DWORD blVnum)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_self.dwTransmutationVnum[pos] = blVnum;
}

void CPythonExchange::SetItemChangeLookVnumToTarget(const int pos, const DWORD blVnum)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_victim.dwTransmutationVnum[pos] = blVnum;
}

DWORD CPythonExchange::GetItemChangeLookVnumToSelf(const BYTE pos) const
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_self.dwTransmutationVnum[pos];
}

DWORD CPythonExchange::GetItemChangeLookVnumToTarget(const BYTE pos) const
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_victim.dwTransmutationVnum[pos];
}
#endif

void CPythonExchange::DelItemOfTarget(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_victim.item_vnum[pos] = 0;
	m_victim.item_count[pos] = 0;
}

void CPythonExchange::DelItemOfSelf(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	m_self.item_vnum[pos] = 0;
	m_self.item_count[pos] = 0;
}

DWORD CPythonExchange::GetItemVnumFromTarget(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_victim.item_vnum[pos];
}

DWORD CPythonExchange::GetItemVnumFromSelf(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_self.item_vnum[pos];
}

#ifdef ENABLE_STACK_LIMIT
WORD CPythonExchange::GetItemCountFromTarget(BYTE pos)
#else
BYTE CPythonExchange::GetItemCountFromTarget(BYTE pos)
#endif
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_victim.item_count[pos];
}

#ifdef ENABLE_STACK_LIMIT
WORD CPythonExchange::GetItemCountFromSelf(BYTE pos)
#else
BYTE CPythonExchange::GetItemCountFromSelf(BYTE pos)
#endif
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_self.item_count[pos];
}

DWORD CPythonExchange::GetItemMetinSocketFromTarget(BYTE pos, int iMetinSocketPos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_victim.item_metin[pos][iMetinSocketPos];
}

DWORD CPythonExchange::GetItemMetinSocketFromSelf(BYTE pos, int iMetinSocketPos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return 0;

	return m_self.item_metin[pos][iMetinSocketPos];
}

void CPythonExchange::GetItemAttributeFromTarget(BYTE pos, int iAttrPos, BYTE * pbyType, short * psValue)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	*pbyType = m_victim.item_attr[pos][iAttrPos].bType;
	*psValue = m_victim.item_attr[pos][iAttrPos].sValue;
}

void CPythonExchange::GetItemAttributeFromSelf(BYTE pos, int iAttrPos, BYTE * pbyType, short * psValue)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return;

	*pbyType = m_self.item_attr[pos][iAttrPos].bType;
	*psValue = m_self.item_attr[pos][iAttrPos].sValue;
}

void CPythonExchange::SetAcceptToTarget(BYTE Accept)
{
	m_victim.accept = Accept ? true : false;
}

void CPythonExchange::SetAcceptToSelf(BYTE Accept)
{
	m_self.accept = Accept ? true : false;
}

bool CPythonExchange::GetAcceptFromTarget()
{
	return m_victim.accept ? true : false;
}

bool CPythonExchange::GetAcceptFromSelf()
{
	return m_self.accept ? true : false;
}

bool CPythonExchange::GetElkMode()
{
	return m_elk_mode;
}

void CPythonExchange::SetElkMode(bool value)
{
	m_elk_mode = value;
}

void CPythonExchange::Start()
{
	m_isTrading = true;
}

void CPythonExchange::End()
{
	m_isTrading = false;
}

bool CPythonExchange::isTrading()
{
	return m_isTrading;
}

void CPythonExchange::Clear()
{
	memset(&m_self, 0, sizeof(m_self));
	memset(&m_victim, 0, sizeof(m_victim));
}

CPythonExchange::CPythonExchange()
{
	Clear();
	m_isTrading = false;
	m_elk_mode = false;
}

CPythonExchange::~CPythonExchange() {}
