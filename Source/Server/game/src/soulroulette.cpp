#include "stdafx.h"
#include "../../common/service.h"
#include "char.h"
#include "packet.h"
#include "desc.h"
#include "group_text_parse_tree.h"
#include "locale_service.h"
#include "log.h"
#include "config.h"
#include "sectree_manager.h"
#include "db.h"

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
static int RoulettePrice = 250000;
static int RouletteSoul = 50;
static bool EventActive;
static const char* mTableName = "soul_roulette_log";
static std::vector<CSoulRoulette::SRoulette*> v_RouletteItem;

CSoulRoulette::CSoulRoulette(LPCHARACTER m_ch)
	: ch(m_ch), gift_vnum(0), gift_count(1), turn_count(0)
{
	SendPacket(CSoulRoulette::Packet::OPEN);
}

CSoulRoulette::~CSoulRoulette() {
	if (GetGiftVnum())
		StateError(Error::ADD, ch);
}

template <typename T> std::string NumberToString(T Number)
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

bool CSoulRoulette::ReadRouletteData(bool NoMoreItem)
{
	for (std::vector<CSoulRoulette::SRoulette*>::const_iterator it = v_RouletteItem.begin(); it != v_RouletteItem.end(); ++it)
		delete (*it);

	v_RouletteItem.clear();

	if (NoMoreItem)
		return true;
	
	v_RouletteItem.reserve(ROULETTE_ITEM_MAX);

	char c_pszFileName[FILE_MAX_LEN];
	snprintf(c_pszFileName, sizeof(c_pszFileName), "%s/roulette_items.txt", LocaleService_GetBasePath().c_str());

	//std::auto_ptr<CGroupTextParseTreeLoader> loader(new CGroupTextParseTreeLoader());
	std::unique_ptr<CGroupTextParseTreeLoader> loader(new CGroupTextParseTreeLoader());

	if (!loader->Load(c_pszFileName)) {
		sys_err("ReadRouletteData %s: load error", c_pszFileName);
		return false;
	}

	static const char* GroupName = "main";
	CGroupNode* RouletteGroup = loader->GetGroup(GroupName);

	if (!RouletteGroup) {
		sys_err("%s isn't exist!", GroupName);
		return false;
	}

	if (!RouletteGroup->GetRowCount()) {
		sys_err("Group %s is empty!", GroupName);
		return false;
	}

	if (!RouletteGroup->GetValue("active", 0, EventActive)) {
		sys_err("Group %s does not have: active.", RouletteGroup->GetNodeName().c_str());
		return false;
	}

	if (!EventActive) // event disabled, no need to add items
		return true;


	if (!RouletteGroup->GetValue("price", 0, RoulettePrice)) {
		sys_err("Group %s does not have: price.", RouletteGroup->GetNodeName().c_str());
		return false;
	}

	if (!RouletteGroup->GetValue("soul", 0, RouletteSoul)) {
		sys_err("Group %s does not have: soul.", RouletteGroup->GetNodeName().c_str());
		return false;
	}

	int RowCount = RouletteGroup->GetRowCount() - 2; // -2: active, price
	for (int i = 1; i <= ROULETTE_ITEM_MAX && i <= RowCount; i++) {
		DWORD vnum;
		BYTE count, chance;

		//std::string id = std::to_string(i); // C++11(+)
		std::string id = NumberToString(i);

		// Why string? Because CGroupNode m_map_rows is map(sorted). It's storing strings not indexes.
		// Example GetValue(0) gives you row 1, GetValue(1) gives you row 10
		// Now I can access value with string directly

		if (!RouletteGroup->GetValue(id, "vnum", vnum)) {
			sys_err("CSoulRoulette::RouletteGroup vnum error. (line: %s)", id.c_str());
			return false;
		}

		if (!RouletteGroup->GetValue(id, "count", count)) {
			sys_err("CSoulRoulette::RouletteGroup count error. (line: %s)", id.c_str());
			return false;
		}

		if (!RouletteGroup->GetValue(id, "chance", chance)) {
			sys_err("CSoulRoulette::RouletteGroup chance error. (line: %s)", id.c_str());
			return false;
		}

		v_RouletteItem.push_back(new CSoulRoulette::SRoulette(vnum, count, chance));
	}

	return true;
}

void CSoulRoulette::SendPacket(BYTE option, int arg1, int arg2)
{
	if (!ch->GetDesc())
		return;

	TPacketGCSoulRoulette p;
	memset(&p, 0, sizeof(p));

	p.header = HEADER_GC_SOUL_ROULETTE;
	p.option = option;

	switch (option) {
	case CSoulRoulette::Packet::OPEN:
		p.yang = RoulettePrice;
		p.soul = RouletteSoul;
		for (size_t i = 0; i < v_RouletteItem.size(); i++) {
			p.ItemInfo[i].vnum = v_RouletteItem[i]->vnum;
			p.ItemInfo[i].count = v_RouletteItem[i]->count;
		}
		break;
	case CSoulRoulette::Packet::TURN:
		p.ItemInfo[0].vnum = arg1;
		p.ItemInfo[0].count = arg2;
		break;
	}

	ch->GetDesc()->Packet(&p, sizeof(TPacketGCSoulRoulette));
}

template<typename T> std::string MoneyString(T val)
{
	const int comma = 3;
	std::string str = NumberToString(val);
	int pos = static_cast<int>(str.length()) - comma;

	while (pos > 0) {
		str.insert(pos, ".");
		pos -= comma;
	}

	return str;
}

void CSoulRoulette::TurnWheel()
{
	if (GetGiftVnum()) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Lütfen bekleyin, <Kan Ritüeli> şu anda aktif."));
		return;
	}

	if (ch->GetGold() < RoulettePrice) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Kan Ritüeli> için yeterli yangın yok."));
		return;
	}
	if (ch->GetSoulPoint() < RouletteSoul) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Kan Ritüeli> için ruh gerekiyor."));
		return;
	}

	int Rand = PickAGift();
	if (Rand == -1) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Kan Ritüeli> şu anda devre dışı. Lütfen daha sonra tekrar deneyin."));
		return;
	}

	ch->PointChange(POINT_GOLD, -RoulettePrice);
	ch->PointChange(POINT_SOUL, -RouletteSoul);
	ch->PointChange(POINT_SOUL_RE, RouletteSoul);

	//spin count, pos
	SendPacket(CSoulRoulette::Packet::TURN, number(3, 5), Rand);

	turn_count++;
}
#include <algorithm>
int CSoulRoulette::PickAGift()
{
	const BYTE Chance = GetChance();
	std::vector<CSoulRoulette::SRoulette*>::const_iterator it = std::min_element(v_RouletteItem.begin(), v_RouletteItem.end(), CSoulRoulette::SRoulette::ByChance());

	if (it != v_RouletteItem.end()) {
		const BYTE MinChance = (*it)->chance;

		while (Chance >= MinChance) {
			const int rand_pos = number(0, static_cast<int>(v_RouletteItem.size()) - 1);
			const CSoulRoulette::SRoulette* Roulette = v_RouletteItem[rand_pos];

			if (Chance >= Roulette->chance) {
				SetGift(Roulette->vnum, Roulette->count);
				return rand_pos;
			}
		}
	}

	return -1;
}

void CSoulRoulette::GiveGift()
{
	const DWORD GiftVnum = GetGiftVnum();

	if (GiftVnum) {
		ch->AutoGiveItem(GiftVnum, GetGiftCount());
		LogManager::instance().SoulRouletteLog(mTableName, ch->GetName(), GiftVnum, GetGiftCount(), true);
		SetGift(0, 1); // reset
	}
	else
		sys_err("CSoulRoulette::GiveGift: <player: %s> is trying to get item without item data.", ch->GetName());
}

void CSoulRoulette::StateError(BYTE option, LPCHARACTER ch)
{
	static std::vector<CSoulRoulette::ErrorData*> v_Err;

	switch (option)
	{
	case CSoulRoulette::Error::LOAD: // from mysql
	{
		char szQuery[80];
		snprintf(szQuery, sizeof(szQuery), "SELECT vnum, count, name FROM log.%s WHERE state = 'ERROR'", mTableName);
		std::unique_ptr<SQLMsg> pMsg1(DBManager::instance().DirectQuery(szQuery));
		
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pMsg1->Get()->pSQLResult))) {
			DWORD vnum; BYTE count;
			str_to_number(vnum, row[0]);
			str_to_number(count, row[1]);
			v_Err.push_back(new CSoulRoulette::ErrorData(vnum, count, row[2]));
		}

		std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery("DELETE FROM log.%s WHERE state = 'ERROR'", mTableName));
		break;
	}	
	case CSoulRoulette::Error::ADD: // from game
		if (ch) {
			const CSoulRoulette* Roulette = ch->GetSoulRoulette();
			if (Roulette)
				v_Err.push_back(new CSoulRoulette::ErrorData(Roulette->GetGiftVnum(), Roulette->GetGiftCount(), ch->GetName()));
		}
		break;
	case CSoulRoulette::Error::GIVE: // when player login again
		if (ch) {
			std::vector<CSoulRoulette::ErrorData*>::const_iterator it = std::find_if(v_Err.begin(), v_Err.end(), CSoulRoulette::ErrorData::FindName(ch->GetName()));
			// we don't need iterate all data
			if (it != v_Err.end()) {
				const CSoulRoulette::ErrorData* p_Error = (*it);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Kan Ritüeli> Endişelenme, ödülünü aldın!"));
				ch->AutoGiveItem(p_Error->vnum, p_Error->count);
				v_Err.erase(it);
				delete p_Error;
			}
		}
		break;
	case CSoulRoulette::Error::SHUTDOWN: // add log if player still doesn't get his gift(Server is closing)
		for (std::vector<CSoulRoulette::ErrorData*>::const_iterator it = v_Err.begin(); it != v_Err.end(); ++it) {
			const CSoulRoulette::ErrorData* p_Error = (*it);
			LogManager::instance().SoulRouletteLog(mTableName, p_Error->name.c_str(), p_Error->vnum, p_Error->count, false);
			delete p_Error;
		}
		break;
	}
}

void CSoulRoulette::SetGift(const DWORD vnum, const BYTE count)
{
	gift_vnum = vnum;
	gift_count = count;
}

BYTE CSoulRoulette::GetChance() const
{
	const WORD TurnCount = GetTurnCount();
	if (TurnCount >= 10 && TurnCount < 25)
		return 25;
	if (TurnCount >= 25 && TurnCount < 40)
		return 50;
	if (TurnCount >= 40)
		return 255; // max
	return 0;
}

DWORD CSoulRoulette::GetGiftVnum() const
{
	return gift_vnum;
}

BYTE CSoulRoulette::GetGiftCount() const
{
	return gift_count;
}

WORD CSoulRoulette::GetTurnCount() const
{
	return turn_count;
}
#endif
