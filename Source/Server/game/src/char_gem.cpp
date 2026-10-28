#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"
#include "questmanager.h"

const char* txt_gem[] = {
	"<Gaya> Yeterli gaya yok.",
	"<Gaya> Hata.",
	"<Gaya> %d adet %s gerekli.",
	"<Gaya> Yeterli yang yok.",
	"<Gaya> Kazanýlan gaya miktarý: %d",
	"<Gaya> Bileme iþlemi baþarýlý oldu.",
	"<Gaya> Bileme iþlemi baþarýsýz oldu."
};

void CHARACTER::LoadGemSystem()
{
	if (!IsPC())
		return;

	FILE	*fp;
	char	one_line[256];
	int		value1, value2, value3;
	const char	*delim = " \t\r\n";
	char	*v, *token_string;
	char	file_name[256+1];

	Gem_Load_Values gem_values = {0, 0, 0};

	snprintf(file_name, sizeof(file_name), "%s/gem.txt", LocaleService_GetBasePath().c_str());
	fp = fopen(file_name, "r");
	if (!fp)
		return;

	load_gem_items.clear();

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = value3 = 0;

		if (one_line[0] == '#')
			continue;

		token_string = strtok(one_line, delim);
		if (NULL == token_string)
			continue;

		if ((v = strtok(NULL, delim)))
			str_to_number(value1, v);

		if ((v = strtok(NULL, delim)))
			str_to_number(value2, v);

		if ((v = strtok(NULL, delim)))
			str_to_number(value3, v);

		TOKEN("ITEM")
		{
			gem_values.items = value1;
			gem_values.gem = value2;
			gem_values.count = value3;
			load_gem_items.push_back(gem_values);
		}
		else TOKEN("GLIMMERSTONE") { load_gem_values.glimmerstone = value1; }
		else TOKEN("GEM_EXPANSION") { load_gem_values.gem_expansion = value1; }
		else TOKEN("GEM_REFRESH") { load_gem_values.gem_refresh = value1; }
		else TOKEN("GLIMMERSTONE_COUNT") { load_gem_values.glimmerstone_count = value1; }
		else TOKEN("GRADE_STONE") { load_gem_values.grade_stone = value1; }
		else TOKEN("GIVE_GEM") { load_gem_values.give_gem = value1; }
		else TOKEN("PROB_GEM") { load_gem_values.prob_gem = value1; }
		else TOKEN("COST_GEM_YANG") { load_gem_values.cost_gem_yang = value1; }
		else TOKEN("GEM_EXPANSION_COUNT") { load_gem_values.gem_expansion_count = value1; }
		else TOKEN("GEM_REFRESH_COUNT") { load_gem_values.gem_refresh_count = value1; }
	}

	fclose(fp);
}

bool CHARACTER::CheckItemsFull()
{
	FILE	*fp;
	char	file_name[256+1];

	snprintf(file_name, sizeof(file_name), "%s/gem/%s_gem_info.txt", LocaleService_GetBasePath().c_str(), GetName());

	if ((fp = fopen(file_name, "r")) == 0)
		return false;
	else
		return true;
}

void CHARACTER::ClearGemMarket()
{
	info_items.clear();
	info_slots.clear();
}

void CHARACTER::InfoGemMarker()
{
	ClearGemMarket();
	UpdateItemsGemMarker0();
	ChatPacket(CHAT_TYPE_COMMAND, "GemMarketClear");

	for (int i = 0; i < info_items.size(); ++i)
	{
		ChatPacket(CHAT_TYPE_COMMAND, "GemMarketItems %d %d %d",
			info_items[i].value_1,
			info_items[i].value_2,
			info_items[i].value_3);
	}

	if (info_slots.empty())
		return;

	ChatPacket(CHAT_TYPE_COMMAND, "GemMarketSlotsDesblock %d %d %d %d %d %d",
		info_slots[0].value_1,
		info_slots[0].value_2,
		info_slots[0].value_3,
		info_slots[0].value_4,
		info_slots[0].value_5,
		info_slots[0].value_6);
}

bool CHARACTER::CheckSlotGemMarket(int slot)
{
	if (slot == 3)
	{
		if (info_slots[0].value_1 == 0) return false;
		else return true;
	}
	else if (slot == 4)
	{
		if (info_slots[0].value_2 == 0) return false;
		else return true;
	}
	else if(slot == 5)
	{
		if (info_slots[0].value_3 == 0) return false;
		else return true;
	}
	else if(slot == 6)
	{
		if (info_slots[0].value_4 == 0) return false;
		else return true;
	}
	else if(slot == 7)
	{
		if (info_slots[0].value_5 == 0) return false;
		else return true;
	}
	else if(slot == 8)
	{
		if (info_slots[0].value_6 == 0) return false;
		else return true;
	}
}

void CHARACTER::BuyItemsGemMarket(int slot)
{
	if (GetGem() >= info_items[slot].value_2)
	{
		PointChange(POINT_GEM, -info_items[slot].value_2);
#ifdef ENABLE_HIGHLIGHT_SYSTEM
		AutoGiveItem(info_items[slot].value_1, info_items[slot].value_3, -1, true, true);
#else
		AutoGiveItem(info_items[slot].value_1, info_items[slot].value_3, -1, true);
#endif
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO,txt_gem[0]);
		return;
	}
}

void CHARACTER::RefreshItemsGemMarket()
{
	if (load_gem_items.empty())
		return;

	FILE 	*fileID;
	char file_name[256+1];

	snprintf(file_name, sizeof(file_name), "%s/gem/%s_gem_info.txt", LocaleService_GetBasePath().c_str(),GetName());
	fileID = fopen(file_name, "w");
	
	if (NULL == fileID)
		return;

	for (int i = 0; i<9; ++i){
		int number = number(1, load_gem_items.size()-1);
		fprintf(fileID,"Item	%d	%d	%d\n",load_gem_items[number].items,load_gem_items[number].gem,load_gem_items[number].count);
	}
	
	if (info_slots.empty())
		return;

	fprintf(fileID, "Slots_Desblock	%d	%d	%d	%d	%d	%d\n",info_slots[0].value_1,info_slots[0].value_2,info_slots[0].value_3,info_slots[0].value_4,info_slots[0].value_5,info_slots[0].value_6);
	fclose(fileID);
}

void CHARACTER::UpdateSlotGemMarket(int slot)
{
	FILE	*fileID;
	char	file_name[256+1];

	snprintf(file_name, sizeof(file_name), "%s/gem/%s_gem_info.txt", LocaleService_GetBasePath().c_str(), GetName());
	fileID = fopen(file_name, "w");
	if (NULL == fileID)
		return;

	for (int i = 0; i < 9; ++i)
	{
		fprintf(fileID, "Item	%d	%d	%d\n",
			info_items[i].value_1,
			info_items[i].value_2,
			info_items[i].value_3);
	}

	if (slot == 3)
		info_slots[0].value_1 = 1;
	else if (slot == 4)
		info_slots[0].value_2 = 1;
	else if (slot == 5)
		info_slots[0].value_3 = 1;
	else if (slot == 6)
		info_slots[0].value_4 = 1;
	else if (slot == 7)
		info_slots[0].value_5 = 1;
	else if (slot == 8)
		info_slots[0].value_6 = 1;

	fprintf(fileID, "Slots_Desblock	%d	%d	%d	%d	%d	%d\n",
		info_slots[0].value_1,
		info_slots[0].value_2,
		info_slots[0].value_3,
		info_slots[0].value_4,
		info_slots[0].value_5,
		info_slots[0].value_6);

	fclose(fileID);
	InfoGemMarker();
}

void CHARACTER::UpdateItemsGemMarker0()
{
	FILE		*fp;
	char		one_line[256];
	int			value1, value2, value3, value4, value5, value6;
	const char	*delim = " \t\r\n";
	char		*v, *token_string;
	char		file_name[256+1];

	Gem_Shop_Values market_gem_values_0 = {0, 0, 0};
	Gem_Shop_Values market_gem_values_1 = {0, 0, 0, 0, 0, 0};

	snprintf(file_name, sizeof(file_name), "%s/gem/%s_gem_info.txt", LocaleService_GetBasePath().c_str(), GetName());
	fp = fopen(file_name, "r");
	if (!fp)
		return;

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = value3 = value4 = value5 = value6 = 0;

		if (one_line[0] == '#')
			continue;

		token_string = strtok(one_line, delim);
		if (NULL == token_string)
			continue;

		if ((v = strtok(NULL, delim)))
			str_to_number(value1, v);

		if ((v = strtok(NULL, delim)))
			str_to_number(value2, v);

		if ((v = strtok(NULL, delim)))
			str_to_number(value3, v);

		if ((v = strtok(NULL, delim)))
			str_to_number(value4, v);

		if ((v = strtok(NULL, delim)))
			str_to_number(value5, v);

		if ((v = strtok(NULL, delim)))
			str_to_number(value6, v);

		TOKEN("Item")
		{
			market_gem_values_0.value_1 = value1;
			market_gem_values_0.value_2 = value2;
			market_gem_values_0.value_3 = value3;
			info_items.push_back(market_gem_values_0);
		}
		else TOKEN("Slots_Desblock")
		{
			market_gem_values_1.value_1 = value1;
			market_gem_values_1.value_2 = value2;
			market_gem_values_1.value_3 = value3;
			market_gem_values_1.value_4 = value4;
			market_gem_values_1.value_5 = value5;
			market_gem_values_1.value_6 = value6;
			info_slots.push_back(market_gem_values_1);
		}
	}

	fclose(fp);
}

void CHARACTER::UpdateItemsGemMarker()
{
	if (load_gem_items.empty())
		return;

	FILE	*fileID;
	char file_name[256+1];
	snprintf(file_name, sizeof(file_name), "%s/gem/%s_gem_info.txt", LocaleService_GetBasePath().c_str(),GetName());
	fileID = fopen(file_name, "a");
	if (NULL == fileID)
		return;

	for (int i = 0; i<9; ++i)
	{
		int number = number(0, load_gem_items.size()-1);
		fprintf(fileID,"Item	%d	%d	%d\n", load_gem_items[number].items, load_gem_items[number].gem, load_gem_items[number].count);
	}

	fprintf(fileID, "Slots_Desblock	%d	%d	%d	%d	%d	%d\n", 0,0,0,0,0,0);
	fclose(fileID);
}

void CHARACTER::CraftGemItems(int slot)
{
	LPITEM	item = GetItem(TItemPos(INVENTORY, slot));

	int ID_Glimmerstone = load_gem_values.glimmerstone;
	int Count_Glimmerstone = load_gem_values.glimmerstone_count;
	int Grade_Stone = load_gem_values.grade_stone;
	int gem_points = load_gem_values.give_gem;
	int Random_POINT_GEM = number(1, 100);
	int Prob_Gem = load_gem_values.prob_gem;
	int Cost_Gem_Yang = load_gem_values.cost_gem_yang;

	// FIX(leak): sadece isim icin her cagrida item olusturulup silinmiyordu
	const TItemTable* glimmerTable = ITEM_MANAGER::instance().GetTable(ID_Glimmerstone);
	const char* glimmerName = glimmerTable ? glimmerTable->szLocaleName : "";

	if (NULL == item)
	{
		return;
	}

	// SECURITY: trade/lock/equip durumundaki item islenmesin (exchange scam / UAF)
	if (!CanHandleItem() || GetExchange() || item->IsExchanging() || item->isLocked() || item->IsEquipped())
		return;

	if (item->GetType() != ITEM_METIN || item->GetRefineLevel() > Grade_Stone)
	{
		ChatPacket(CHAT_TYPE_INFO, txt_gem[1]);
		return;
	}

	if (CountSpecifyItem(ID_Glimmerstone) < Count_Glimmerstone)
	{
		ChatPacket(CHAT_TYPE_INFO, txt_gem[2], Count_Glimmerstone, glimmerName);
		return;
	}

	if(GetGold() < Cost_Gem_Yang)
	{
		ChatPacket(CHAT_TYPE_INFO, txt_gem[3]);
		return;
	}

	if (Random_POINT_GEM <= Prob_Gem)
	{
		PointChange(POINT_GEM, gem_points);
		ChatPacket(CHAT_TYPE_INFO, txt_gem[4], gem_points);
		ChatPacket(CHAT_TYPE_INFO, txt_gem[5]);
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, txt_gem[6]);
	}

	RemoveSpecifyItem(ID_Glimmerstone, Count_Glimmerstone);
	PointChange(POINT_GOLD, -Cost_Gem_Yang);
	item->SetCount(item->GetCount() - 1);
	ChatPacket(CHAT_TYPE_COMMAND, "GemCheck");

}

void CHARACTER::MarketGemItems(int slot)
{
	if(CheckItemsFull() == false)
	{
		ChatPacket(CHAT_TYPE_INFO, txt_gem[1]);
		return;
	}

	// SECURITY: negatif slot ile info_items[] OOB okuma (rastgele item/gem)
	if (slot < 0 || slot > 8 || slot >= (int)info_items.size())
	{
		ChatPacket(CHAT_TYPE_INFO, txt_gem[1]);
		return;
	}

	int ID_GemMarketExpansion = load_gem_values.gem_expansion;
	// FIX(leak): sadece isim icin item olusturuluyordu
	const TItemTable* expansionTable = ITEM_MANAGER::instance().GetTable(ID_GemMarketExpansion);
	const char* expansionName = expansionTable ? expansionTable->szLocaleName : "";

	if (slot >= 3)
	{
		if (CheckSlotGemMarket(slot) == false)
		{
			if (CountSpecifyItem(ID_GemMarketExpansion) >= load_gem_values.gem_expansion_count)
			{
				RemoveSpecifyItem(ID_GemMarketExpansion, load_gem_values.gem_expansion_count);
				UpdateSlotGemMarket(slot);
				return;
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, txt_gem[2], load_gem_values.gem_expansion_count, expansionName);
				return;
			}
		}
		else
		{
			BuyItemsGemMarket(slot);
			return;
		}
	}
	else
	{
		BuyItemsGemMarket(slot);
		return;
	}
	return;
}

void CHARACTER::RefreshGemItems()
{
	if (CheckItemsFull() == false)
	{
		ChatPacket(CHAT_TYPE_INFO, txt_gem[1]);
		return;
	}

	int ID_GemMarketRefresh = load_gem_values.gem_refresh;
	// FIX(leak): sadece isim icin item olusturuluyordu
	const TItemTable* refreshTable = ITEM_MANAGER::instance().GetTable(ID_GemMarketRefresh);
	const char* refreshName = refreshTable ? refreshTable->szLocaleName : "";

	if (CountSpecifyItem(ID_GemMarketRefresh) < load_gem_values.gem_refresh_count)
	{
		ChatPacket(CHAT_TYPE_INFO, txt_gem[2], load_gem_values.gem_refresh_count, refreshName);
		return;
	}

	RemoveSpecifyItem(ID_GemMarketRefresh, load_gem_values.gem_refresh_count);
	RefreshItemsGemMarket();
	InfoGemMarker();
}

int CHARACTER::GetGemState(const std::string& state) const
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	return pPC->GetFlag(state);

	//return quest::CQuestManager::instance().GetEventFlag(state);
}

void CHARACTER::SetGemState(const std::string& state, int szValue)
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	return pPC->SetFlag(state, szValue);

	//return quest::CQuestManager::instance().SetEventFlag(state, szValue);
}

EVENTFUNC(check_time_market_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if (info == NULL)
	{
		sys_err("check_time_market_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	if (ch->GetGemState("gem_system.gem_refresh_time") - init_gemTime() <= 0)
	{
		ch->SetGemState("gem_system.gem_refresh_time", init_gemTime() + (60 * 60 * 5)); // 5h
		ch->RefreshItemsGemMarket();
		ch->InfoGemMarker();
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "GemMarketTime %d", ch->GetGemState("gem_system.gem_refresh_time"));

	return PASSES_PER_SEC(2);
}

void CHARACTER::StartCheckTimeMarket()
{
	if (GemUpdateTime)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	GemUpdateTime = event_create(check_time_market_event, info, PASSES_PER_SEC(1));
}