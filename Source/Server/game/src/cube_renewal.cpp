#define _cube_cpp_

#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "log.h"
#include "char.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "questmanager.h"
#include <sstream>
#include "packet.h"
#include "desc_client.h"

#include <unordered_map>

#include "../../common/service.h"
#define CUBE_NO_ITEM

#ifdef ENABLE_RENEWAL_CUBE
static std::vector<CUBE_RENEWAL_DATA*>	s_cube_proto;

typedef std::vector<CUBE_RENEWAL_VALUE>	TCubeValueVector;

struct SCubeMaterialInfo
{
	SCubeMaterialInfo()
	{
		bHaveComplicateMaterial = false;
	};

	CUBE_RENEWAL_VALUE reward;
	TCubeValueVector material;
#ifdef ENABLE_GOLD_LIMIT
	long long gold;
#else
	DWORD gold;
#endif
#ifdef ENABLE_GAYA_SYSTEM
	DWORD gem;
#endif
	int percent;
	std::string category;
	TCubeValueVector complicateMaterial;
	bool allowCopyAttr;
	std::string infoText;
	bool bHaveComplicateMaterial;
};

struct SItemNameAndLevel
{
	SItemNameAndLevel() { level = 0; }

	std::string name;
	int level;
};


typedef std::vector<SCubeMaterialInfo> TCubeResultList;
typedef std::unordered_map<DWORD, TCubeResultList> TCubeMapByNPC;

TCubeMapByNPC cube_info_map;


static bool FN_check_valid_npc( WORD vnum )
{
	for ( std::vector<CUBE_RENEWAL_DATA*>::iterator iter = s_cube_proto.begin(); iter != s_cube_proto.end(); iter++ )
	{
		if ( std::find((*iter)->npc_vnum.begin(), (*iter)->npc_vnum.end(), vnum) != (*iter)->npc_vnum.end() )
			return true;
	}

	return false;
}

static bool FN_check_cube_data (CUBE_RENEWAL_DATA *cube_data)
{
	DWORD	i = 0;
	DWORD	end_index = 0;

	end_index = cube_data->npc_vnum.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->npc_vnum[i] == 0 ) return false;
	}

	end_index = cube_data->item.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->item[i].vnum == 0 ) return false;
		if ( cube_data->item[i].count == 0 ) return false;
	}

	end_index = cube_data->reward.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->reward[i].vnum == 0 ) return false;
		if ( cube_data->reward[i].count == 0 ) return false;
	}
	return true;
}

static int FN_check_cube_item_vnum_material(const SCubeMaterialInfo& materialInfo, int index)
{
	if (index <= materialInfo.material.size()){
		return materialInfo.material[index-1].vnum;
	}
	return 0;
}

static int FN_check_cube_item_count_material(const SCubeMaterialInfo& materialInfo,int index)
{
	if (index <= materialInfo.material.size()){
		return materialInfo.material[index-1].count;
	}

	return 0;
}

CUBE_RENEWAL_DATA::CUBE_RENEWAL_DATA()
{
	this->gold = 0;
#ifdef ENABLE_GAYA_SYSTEM
	this->gem = 0;
#endif
	this->allowCopyAttr = false;
	this->category = "OUTROS";
}

void Cube_init()
{
	CUBE_RENEWAL_DATA * p_cube = NULL;
	std::vector<CUBE_RENEWAL_DATA*>::iterator iter;

	char file_name[256+1];
	snprintf(file_name, sizeof(file_name), "%s/cube.txt", LocaleService_GetBasePath().c_str());

	sys_log(0, "Cube_Init %s", file_name);

	for (iter = s_cube_proto.begin(); iter!=s_cube_proto.end(); iter++)
	{
		p_cube = *iter;
		M2_DELETE(p_cube);
	}

	s_cube_proto.clear();

	if (false == Cube_load(file_name))
		sys_err("Cube_Init failed");
}

bool Cube_load (const char *file)
{
	FILE *fp;


	const char *value_string;

	char one_line[256];
	int value1, value2;
	const char *delim = " \t\r\n";
	char *v, *token_string;
	CUBE_RENEWAL_DATA *cube_data = NULL;
	CUBE_RENEWAL_VALUE cube_value = {0,0};

	if (0 == file || 0 == file[0])
		return false;

	if ((fp = fopen(file, "r")) == 0)
		return false;

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = 0;

		if (one_line[0] == '#')
			continue;

		token_string = strtok(one_line, delim);

		if (NULL == token_string)
			continue;

		if ((v = strtok(NULL, delim)))
			str_to_number(value1, v);

		value_string = v;

		if ((v = strtok(NULL, delim)))
			str_to_number(value2, v);

		TOKEN("section")
		{
			cube_data = M2_NEW CUBE_RENEWAL_DATA;
		}
		else TOKEN("npc")
		{
			cube_data->npc_vnum.push_back((WORD)value1);
		}
		else TOKEN("item")
		{
			cube_value.vnum = value1;
			cube_value.count = value2;

			cube_data->item.push_back(cube_value);
		}
		else TOKEN("reward")
		{
			cube_value.vnum = value1;
			cube_value.count = value2;

			cube_data->reward.push_back(cube_value);
		}
		else TOKEN("percent")
		{

			cube_data->percent = value1;
		}
		else TOKEN("category")
		{
			cube_data->category = value_string;
		}
		else TOKEN("gold")
		{
			cube_data->gold = value1;
		}
#ifdef ENABLE_GAYA_SYSTEM
		else TOKEN("gem")
		{
			cube_data->gem = value1;
		}
#endif
		else TOKEN("allow_copy")
		{
			cube_data->allowCopyAttr = (value1 == 1 ? true : false);
		}
		else TOKEN("end")
		{
			if (false == FN_check_cube_data(cube_data))
			{
				M2_DELETE(cube_data);
				continue;
			}
			s_cube_proto.push_back(cube_data);
		}
	}

	fclose(fp);
	return true;
}

SItemNameAndLevel SplitItemNameAndLevelFromName(const std::string& name)
{
	int level = 0;
	SItemNameAndLevel info;
	info.name = name;

	size_t pos = name.find("+");

	if (std::string::npos != pos)
	{
		const std::string levelStr = name.substr(pos + 1, name.size() - pos - 1);
		str_to_number(level, levelStr.c_str());

		info.name = name.substr(0, pos);
	}

	info.level = level;

	return info;
};

bool Cube_InformationInitialize()
{
	for (int i = 0; i < s_cube_proto.size(); ++i)
	{
		CUBE_RENEWAL_DATA* cubeData = s_cube_proto[i];

		const std::vector<CUBE_RENEWAL_VALUE>& rewards = cubeData->reward;

		if (1 != rewards.size())
		{
			sys_err("[CubeInfo] WARNING! Does not support multiple rewards (count: %d)", rewards.size());
			continue;
		}

		const CUBE_RENEWAL_VALUE& reward = rewards.at(0);
		const WORD& npcVNUM = cubeData->npc_vnum.at(0);
		bool bComplicate = false;
		
		TCubeMapByNPC& cubeMap = cube_info_map;
		TCubeResultList& resultList = cubeMap[npcVNUM];
		SCubeMaterialInfo materialInfo;

		materialInfo.reward = reward;
		materialInfo.gold = cubeData->gold;
#ifdef ENABLE_GAYA_SYSTEM
		materialInfo.gem = cubeData->gem;
#endif
		materialInfo.percent = cubeData->percent;
		materialInfo.material = cubeData->item;
		materialInfo.category = cubeData->category;

		resultList.push_back(materialInfo);
	}

	return true;
}

void Cube_open (LPCHARACTER ch)
{
	LPCHARACTER	npc;
	npc = ch->GetQuestNPC();

	if (npc)
	{
		DWORD npcVNUM = npc->GetRaceNum();

		if (NULL == ch) //Fix
		{
			return;
		}

		if (!npc)
		{
			return;
		}

		if (NULL == npc)
		{
			return;
		}

		if (FN_check_valid_npc(npcVNUM) == false)
		{
			return;
		}

		if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			|| ch->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			|| ch->GetTransmutation()
#endif
			)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
			return;
		}

#ifdef ENABLE_GROWTH_PET_SYSTEM
		if (ch->GetPetWindowType() == PET_WINDOW_ATTR_CHANGE || ch->GetPetWindowType() == PET_WINDOW_PRIMIUM_FEEDSTUFF)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 738, "");
			return;
		}
#endif

		long distance = DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY());

		if (distance >= CUBE_MAX_DISTANCE)
		{
			sys_log(1, "CUBE: TOO_FAR: %s distance %d", ch->GetName(), distance);
			return;
		}

		SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE);
		SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,npcVNUM);
		SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_DATES_LOADING);
		SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE);

		ch->SetCubeNpc(npc);
	}
	else
	{
		return;
		sys_log(1, "CUBE: Return Wait()");
	}
}

void Cube_close(LPCHARACTER ch)
{
	ch->SetCubeNpc(NULL);
}

void Cube_Make(LPCHARACTER ch, int index, int count_item, int index_item_improve)
{

	if (NULL == ch) //Fix
		return;

	// SECURITY: client count/index dogrulamasi (negatif count ile gold kazanma acigi)
	if (count_item <= 0 || count_item > 9999 || index < 0) // client make_mul_max = 9999
		return;

	// SECURITY: client improve item'i hic gondermiyor (uicuberenewal: -1); herhangi bir
	// stack'i (or. pot) improve olarak gosterip basari oranini %40 artirma engellenir
	index_item_improve = -1;

#ifdef CUBE_NO_ITEM
	if (count_item <= 0 || count_item > 100)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Invalid craft count.");
		return;
	}

	if (index < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Geçersiz küp tarif indeksi.");
		return;
	}
#endif

	LPCHARACTER	npc;

	npc = ch->GetQuestNPC();

	if (!ch->IsCubeOpen())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 224, "");
		return;
	}

	if (NULL == npc)
	{
		return;
	}

	bool canCopy = false;
	int index_value = 0;
	bool material_check = true;
	LPITEM pItem;

	int iEmptyPos;

	DWORD copyAttr[ITEM_ATTRIBUTE_MAX_NUM][2];
	long copySockets[ITEM_SOCKET_MAX_NUM];

	memset(copyAttr, 0, sizeof(copyAttr));
	memset(copySockets, 0, sizeof(copySockets));

	const TCubeResultList& resultList = cube_info_map[npc->GetRaceNum()];

	for (TCubeResultList::const_iterator iter = resultList.begin(); resultList.end() != iter; ++iter)
	{
		if (index_value == index)
		{
			const SCubeMaterialInfo& materialInfo = *iter;

#ifdef CUBE_NO_ITEM
			if (materialInfo.material.empty())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Bu tarifte malzeme bulunmuyor (geçersiz).");
				return;
			}

			if (materialInfo.reward.vnum == 0 || materialInfo.reward.count == 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Bu tarif geçersiz ödül içeriyor (istismar engellendi).");
				return;
			}
#endif

			for (int i = 0; i < materialInfo.material.size(); ++i)
			{
#ifdef CUBE_NO_ITEM
				long long required = (long long)materialInfo.material[i].count * count_item;
				if (required > 0x7FFFFFFF)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Geçersiz malzeme sayýsý (taþma tespit edildi).");
					return;
				}
				if (ch->CountSpecifyItem(materialInfo.material[i].vnum) < required)
				{
					material_check = false;
					break;
				}
#else
				if (ch->CountSpecifyItem(materialInfo.material[i].vnum) < (materialInfo.material[i].count * count_item))
				{
					material_check = false;
					break;
				}
#endif
			}

			if (materialInfo.gold != 0)
			{
				if (ch->GetGold() < (materialInfo.gold*count_item))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 225, "");
					return;
				}
			}
#ifdef ENABLE_GAYA_SYSTEM
			if (materialInfo.gem != 0){
				if (ch->GetGem() < (materialInfo.gem*count_item))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("He doesn't have the necessary amount of gem."));
					return;
				}
			}
#endif
			bool pAntiStack = false;
			TItemTable* p = ITEM_MANAGER::instance().GetTable(materialInfo.reward.vnum);

			if (p)
				if (p->dwFlags & ITEM_FLAG_STACKABLE)
					pAntiStack = true;

			if (pAntiStack == false)
				count_item = 1;

			if (material_check)
			{
				int percent_number;
				int total_items_give = 0;
				int porcent_item_improve = 0;

				if (index_item_improve != -1)
				{

					LPITEM item = ch->GetInventoryItem(index_item_improve);
					if(item != NULL)
					{

						if(item->GetCount() <= 40){
							if (materialInfo.percent+item->GetCount() <= 100){
								porcent_item_improve = item->GetCount();
							}

							if(materialInfo.percent < 100)
							{
								if (materialInfo.percent+item->GetCount() > 100){
									porcent_item_improve = 100 - materialInfo.percent;
								}
							}
						}
					}

					if(porcent_item_improve != 0)
					{
						item->SetCount(item->GetCount()-porcent_item_improve);
					}
				}

				for (int i = 0; i < count_item; ++i)
				{
					percent_number = number(1,100);
					if ( percent_number<=materialInfo.percent+porcent_item_improve)
					{
						total_items_give++;
					}
				}
				
				pItem = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum,(materialInfo.reward.count*count_item));
				if (!pItem)
				{
					sys_err("Cube_Make: failed to create preview item %u", materialInfo.reward.vnum);
					return;
				}
				if (pItem->IsDragonSoul())
				{
					iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
				}
#ifdef ENABLE_SPECIAL_INVENTORY
				else if (pItem->IsSkillBook())
				{
					iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
				}
				else if (pItem->IsUpgradeItem())
				{
					iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
				}
				else if (pItem->IsStone())
				{
					iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
				}
				else if (pItem->IsGiftBox())
				{
					iEmptyPos = ch->GetEmptyGiftBoxInventory(pItem->GetSize());
				}
				else if (pItem->IsChanger())
				{
					iEmptyPos = ch->GetEmptyChangersInventory(pItem->GetSize());
				}
#endif
				else
				{
					iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());
				}

				if (iEmptyPos < 0)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 1063, "");
					M2_DESTROY_ITEM(pItem); // Fix
					return;
				}

				WORD objectSlot = -1;

				for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
				{
					LPITEM object = ch->GetInventoryItem(i);
					if (NULL == object)
						continue;

					if (object->GetType() == ITEM_WEAPON || object->GetType() == ITEM_ARMOR || object->GetType() == ITEM_UNIQUE || object->GetType() == ITEM_COSTUME)
					{
						if (object->GetVnum() == materialInfo.material[0].vnum)
						{
							objectSlot = object->GetCell();
							canCopy = true;
							break;
						}
					}
				}

				if (canCopy)
				{
					if (objectSlot >= 0)
					{
						LPITEM BonusItem = ch->GetInventoryItem(objectSlot);

						for (int a = 0; a < ITEM_ATTRIBUTE_MAX_NUM; a++)
						{
							if (BonusItem->GetAttributeType(a) != 0)
							{
								copyAttr[a][0] = BonusItem->GetAttributeType(a);
								copyAttr[a][1] = BonusItem->GetAttributeValue(a);
							}
						}

						if (BonusItem->GetType() == ITEM_WEAPON || BonusItem->GetType() == ITEM_ARMOR || BonusItem->GetType() == ITEM_UNIQUE || BonusItem->GetType() == ITEM_COSTUME)
						{
							for (int a = 0; a < BonusItem->GetSocketCount(); a++)
							{
								copySockets[a] = BonusItem->GetSocket(a);
							}
						}
					}
				}

				for (int i = 0; i < materialInfo.material.size(); ++i)
				{
					ch->RemoveSpecifyItem(materialInfo.material[i].vnum, (materialInfo.material[i].count*count_item));
				}

				if (materialInfo.gold != 0){
#ifdef ENABLE_GOLD_LIMIT
					ch->PointChange(POINT_GOLD, -static_cast<long long>(materialInfo.gold*count_item), false);
#else
					ch->PointChange(POINT_GOLD, -(materialInfo.gold*count_item), false);
#endif
				}
#ifdef ENABLE_GAYA_SYSTEM
				if (materialInfo.gem != 0){
					ch->PointChange(POINT_GEM, -static_cast<long long>(materialInfo.gem*count_item), false);

				}
#endif
				if(total_items_give <= 0)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 73, "");
					M2_DESTROY_ITEM(pItem); // Fix
					return;
				}

				M2_DESTROY_ITEM(pItem);
				pItem = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum,(materialInfo.reward.count*total_items_give));
				if (!pItem)
				{
					sys_err("Cube_Make: failed to create reward item %u", materialInfo.reward.vnum);
					return;
				}
				if (pItem->IsDragonSoul())
				{
					iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
					pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
				}
#ifdef ENABLE_SPECIAL_INVENTORY
				else if (pItem->IsSkillBook())
				{
					iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
				else if (pItem->IsUpgradeItem())
				{
					iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
				else if (pItem->IsStone())
				{
					iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
				else if (pItem->IsGiftBox())
				{
					iEmptyPos = ch->GetEmptyGiftBoxInventory(pItem->GetSize());
					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
				else if (pItem->IsChanger())
				{
					iEmptyPos = ch->GetEmptyChangersInventory(pItem->GetSize());
					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
#endif
				else
				{
					if (materialInfo.allowCopyAttr == true && copyAttr != NULL)
					{
						pItem->ClearAttribute();

						for (int a = 0; a < ITEM_ATTRIBUTE_MAX_NUM; a++)
						{
							if (copyAttr[a][0] > 0)
								pItem->SetForceAttribute(a, copyAttr[a][0], copyAttr[a][1]);
						}

						if (pItem->GetType() == ITEM_WEAPON || pItem->GetType() == ITEM_ARMOR)
						{
							for (int a = 0; a < ITEM_SOCKET_MAX_NUM; a++)
							{
								if (copySockets[a])
								{
									pItem->SetSocket(a, copySockets[a]);
								}
							}
						}
					}

					iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());
					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
			}
			else
			{
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 226, "");
#ifdef CUBE_NO_ITEM
				return;
#endif
			}
		}

		index_value++;
	}
}

void SendDateCubeRenewalPackets(LPCHARACTER ch, BYTE subheader, DWORD npcVNUM)
{
	TPacketGCCubeRenewalReceive pack;
	pack.subheader = subheader;

	if(subheader == CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE)
	{
		const TCubeResultList& resultList = cube_info_map[npcVNUM];
		for (TCubeResultList::const_iterator iter = resultList.begin(); resultList.end() != iter; ++iter)
		{
			const SCubeMaterialInfo& materialInfo = *iter;

			pack.date_cube_renewal.vnum_reward = materialInfo.reward.vnum;
			pack.date_cube_renewal.count_reward = materialInfo.reward.count;

			const TItemTable* pRewardProto = ITEM_MANAGER::instance().GetTable(materialInfo.reward.vnum);
			if (pRewardProto && (IS_SET(pRewardProto->dwFlags, ITEM_FLAG_STACKABLE) || !IS_SET(pRewardProto->dwAntiFlags, ITEM_ANTIFLAG_STACK)))
			{
				pack.date_cube_renewal.item_reward_stackable = true;
			}
			else
			{
				pack.date_cube_renewal.item_reward_stackable = false;
			}

			pack.date_cube_renewal.vnum_material_1 = FN_check_cube_item_vnum_material(materialInfo,1);
			pack.date_cube_renewal.count_material_1 = FN_check_cube_item_count_material(materialInfo,1);

			pack.date_cube_renewal.vnum_material_2 = FN_check_cube_item_vnum_material(materialInfo,2);
			pack.date_cube_renewal.count_material_2 = FN_check_cube_item_count_material(materialInfo,2);

			pack.date_cube_renewal.vnum_material_3 = FN_check_cube_item_vnum_material(materialInfo,3);
			pack.date_cube_renewal.count_material_3 = FN_check_cube_item_count_material(materialInfo,3);

			pack.date_cube_renewal.vnum_material_4 = FN_check_cube_item_vnum_material(materialInfo,4);
			pack.date_cube_renewal.count_material_4 = FN_check_cube_item_count_material(materialInfo,4);

			pack.date_cube_renewal.vnum_material_5 = FN_check_cube_item_vnum_material(materialInfo,5);
			pack.date_cube_renewal.count_material_5 = FN_check_cube_item_count_material(materialInfo,5);

			pack.date_cube_renewal.gold = materialInfo.gold;
#ifdef ENABLE_GAYA_SYSTEM
			pack.date_cube_renewal.gem = materialInfo.gem;
#endif
			pack.date_cube_renewal.percent = materialInfo.percent;

			memcpy (pack.date_cube_renewal.category, materialInfo.category.c_str(), sizeof(pack.date_cube_renewal.category));

			LPDESC d = ch->GetDesc();

			if (NULL == d)
			{
				sys_err ("User SendDateCubeRenewalPackets (%s)'s DESC is NULL POINT.", ch->GetName());
				return ;
			}

			d->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		LPDESC d = ch->GetDesc();

		if (NULL == d)
		{
			sys_err ("User SendDateCubeRenewalPackets (%s)'s DESC is NULL POINT.", ch->GetName());
			return;
		}

		d->Packet(&pack, sizeof(pack));
	}
}
#endif
