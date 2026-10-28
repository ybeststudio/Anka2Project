#include "stdafx.h"
#include "utils.h"
#include "char.h"
#include "db.h"
#include "item_manager.h"
#include "config.h"
#include "CollectionsSystem.hpp"

#ifdef __ENABLE_COLLECTIONS_SYSTEM__

CSystemCollections::CSystemCollections()
{
	LoadCategories();
	LoadItems();
	PrintCollections();
}

CSystemCollections::~CSystemCollections()
{
	m_Collections.clear();
}

static inline bool CanParseRecord(MYSQL_ROW & row, const int iRowNum)
{
	return (*row && row[iRowNum]);
}

bool CSystemCollections::LoadCategories() 
{
    std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(
        "SELECT collection_index, name, reward_type0, reward_value0, reward_type1, reward_value1, reward_type2, reward_value2, reward_type3, reward_value3, reward_type4, reward_value4, percent, affect_id FROM player.collections_categories"));

    if (!msg->Get() || !msg->Get()->uiNumRows)
    {
        sys_err("CSystemCollections::LoadCategories: collections_categories table is empty.");
        return false;
    }

    if (!m_Collections.empty())
    {
        m_Collections.clear();
    }

    m_Collections.reserve(msg->Get()->uiNumRows);

    MYSQL_ROW row = nullptr;
    while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
    {
        SCollection collection {};
        collection.index = std::strtoul(row[0], NULL, 10);
        collection.collectName = row[1];

        for (int i = 0; i < 5; i++)
        {
            collection.applyReward[i].bType = std::strtoul(row[2 + i * 2], NULL, 10);   // reward_typeX
            collection.applyReward[i].lValue = std::strtoul(row[3 + i * 2], NULL, 10);  // reward_valueX
        }

        collection.percent = std::strtoul(row[12], NULL, 10); // percent is at index 12
		collection.affect_id = std::strtoul(row[13], NULL, 10);

        m_Collections.emplace_back(std::move(collection));
        sys_log(0, "CSystemCollections::LoadCategories: Adding category %s with index %d to the collections", collection.collectName.c_str(), collection.index);
    }

    return true;
}

bool CSystemCollections::LoadItems()
{
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT collectionIndex, itemIndex, itemVnum, itemCount FROM player.collections_items"));

	if (!msg->Get() || !msg->Get()->uiNumRows)
	{
		sys_err("CSystemCollections::LoadItems: collections_items table is empty.");
		return false;
	}

	MYSQL_ROW row = nullptr;
	while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
	{
		for (auto i = 0; i < 4; i++)
		{
			if (!CanParseRecord(row, i))
			{
				sys_err("CSystemCollections::LoadItems: cannot parse fucking record.");
				continue;
			}
		}

		BYTE collectionIndex = std::strtoul(row[0], NULL, 10);

		SCollectionItem item {};
		item.index = std::strtoul(row[1], NULL, 10);
		item.iVnum = std::strtoul(row[2], NULL, 10);
		item.iCount = std::strtoul(row[3], NULL, 10);

		auto it = std::find_if(m_Collections.begin(), m_Collections.end(), [&collectionIndex] (const SCollection & collection)
		{
			return collection.index == collectionIndex;
		});

		if (it != m_Collections.end())
		{
			it->vec_Items.emplace_back(std::move(item));
			sys_log(0, "CSystemCollections::LoadItems: Adding item %d to the collection %d", item.iVnum, collectionIndex);
		}
		else
		{
			// Collection index bulunamadý - uyarý ver ama devam et (veritabanýnda kategori olmayabilir)
			sys_log(0, "CSystemCollections::LoadItems: collection index %d not found in categories, skipping item: idx %d ivnum %d icount %d", collectionIndex, item.index, item.iVnum, item.iCount);
		}
	}

	return true;
}

void CSystemCollections::AddItem(CHARACTER* ch, const BYTE collectionIndex, const BYTE itemIndex, bool isAll)
{
	if (!ch)
	{
		return;
	}

	if (!ch->CanWarp() || !ch->CanHandleItem()) // Change CanDoAction on CanWarp if u havent got this function.
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot actually do it."));
		return;
	}

	if (test_server) {
		ch->ChatPacket(CHAT_TYPE_INFO, "Adding item: CollectionIdx %d ItemIndex %d", collectionIndex, itemIndex);
	}
	const auto& itCollection = std::find_if(m_Collections.begin(), m_Collections.end(), [&collectionIndex] (const SCollection & collection)
	{
		return collection.index == collectionIndex;
	});

	if (itCollection != m_Collections.end())
	{
		if (itCollection->IsComplete(ch))
		{
			if (test_server) {
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Adding item: This collection is end."));
			}
			return;
		}

		if (isAll)
		{
			itCollection->AddAllItems(ch, itemIndex);
		}
		else
		{
			if (itCollection->AddItem(ch, itemIndex))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Added item succesfully."));
			}
		}

	}

}

void CSystemCollections::SendCollections(CHARACTER* ch)
{
	if (!ch)
	{
		return;
	}

	ResetCollections(ch);

	for (const auto& it : m_Collections)
	{
		bool isComplete = it.IsComplete(ch);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RECV_Collection %d %s %d %d", it.index, it.collectName.c_str(), it.percent, isComplete ? 1 : 0);

		for (const auto& itemIt : it.vec_Items)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionItem %d %d %d %d %d", it.index, itemIt.index, itemIt.iVnum, itemIt.iCount, it.GetActualCount(ch, itemIt.index));
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionIncrease %d", ch->GetQuestFlag("collection.percent_up"));
	ch->ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionBuild");
}

bool CSystemCollections::SCollection::IsComplete(CHARACTER* ch) const
{
	if (!ch)
	{
		return false;
	}

	const std::string strCollectName = "collection.complete" + std::to_string(index);
	if (ch->GetQuestFlag(strCollectName))
	{
		return true;
	}

	return false;
}

void CSystemCollections::SCollection::CompleteCheck(CHARACTER* ch)
{
	if (!ch)
	{
		return;
	}

	size_t needSize = vec_Items.size();
	size_t myCollections = 0;

	for (const auto& rkItem : vec_Items)
	{
		const std::string strCollectName = "collection.idx" + std::to_string(index) + "item_idx" + std::to_string(rkItem.index);
		if (ch->GetQuestFlag(strCollectName) >= rkItem.iCount)
		{
			myCollections++;
		}

	}

	if (needSize == myCollections)
	{
		const std::string strCollectName = "collection.complete" + std::to_string(index);
		ch->SetQuestFlag(strCollectName, 1);
		GiveReward(ch);
	}
}

void CSystemCollections::SCollection::AddAllItems(CHARACTER* ch, const BYTE itemIndex)
{
	if (!ch)
	{
		return;
	}
	
	auto it = std::find_if(vec_Items.begin(), vec_Items.end(), [&itemIndex] (const SCollectionItem & collectItem)
	{
		return collectItem.index == itemIndex;
	});

	if (it != vec_Items.end())
	{
		const std::string strCollectName = "collection.idx" + std::to_string(index) + "item_idx" + std::to_string(itemIndex);
		uint16_t actualCount = ch->GetQuestFlag(strCollectName);
		if (actualCount >= it->iCount)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You've added all of this items."));
			return;
		}

		if (ch->CountSpecifyItem(it->iVnum) == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You havent got this item."));
			return;
		}

		uint16_t iFails = 0;
		uint16_t iSuccess = 0;
		uint16_t iCounter = 0;

		for (uint16_t i=0; i < ch->CountSpecifyItem(it->iVnum); i++)
		{
			if (actualCount >= it->iCount)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This collection is complete."));
				break;
			}

			if (number(1, 100) <= percent)
			{
				iFails++;
			}
			else
			{
				iSuccess++;
				actualCount++;
			}
			
			iCounter++;
		}

		ch->RemoveSpecifyItem(it->iVnum, iCounter);
		ch->SetQuestFlag(strCollectName, actualCount);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionRefresh %d %d %d", index, itemIndex, actualCount);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Collections Multiple Add: %d Successfully %d Unsucessfully."), iSuccess, iFails);
		CompleteCheck(ch);

		return;
	}

	return;
}

bool CSystemCollections::SCollection::AddItem(CHARACTER* ch, const BYTE itemIndex)
{
	if (!ch)
	{
		return false;
	}

	if (test_server) {
		ch->ChatPacket(CHAT_TYPE_INFO, "Adding item: itemIdx %d", itemIndex);
	}
	auto it = std::find_if(vec_Items.begin(), vec_Items.end(), [&itemIndex] (const SCollectionItem & collectItem)
	{
		return collectItem.index == itemIndex;
	});

	if (it != vec_Items.end())
	{
		const std::string strCollectName = "collection.idx" + std::to_string(index) + "item_idx" + std::to_string(itemIndex);
		uint16_t actualCount = ch->GetQuestFlag(strCollectName);
		if (actualCount >= it->iCount)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Odda³eœ ju¿ ten przedmiot!");
			return false;
		}


		if (ch->CountSpecifyItem(it->iVnum) < 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz tego przedmiotu.");
			return false;
		}

		BYTE byPercent = (ch->GetQuestFlag("collection.percent_up")) ? percent + (percent / 2) : percent;
		ch->SetQuestFlag("collection.percent_up", 0);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionIncrease %d", 0);

		if (number(1, 100) <= byPercent)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Ten przedmiot by³ z³y.");
			return false;
		}

		ch->RemoveSpecifyItem(it->iVnum, 1);

		ch->SetQuestFlag(strCollectName, ++actualCount);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionRefresh %d %d %d", index, itemIndex, actualCount);
		CompleteCheck(ch);
		return true;
	}

	return false;
}

uint16_t CSystemCollections::SCollection::GetActualCount(CHARACTER* ch, BYTE itemIndex) const
{
	if (!ch)
	{
		return 0;
	}

	const std::string strCollectName = "collection.idx" + std::to_string(index) + "item_idx" + std::to_string(itemIndex);
	return ch->GetQuestFlag(strCollectName);
}

void CSystemCollections::SCollection::GiveReward(CHARACTER* ch)
{
    if (!ch)
    {
        return;
    }

    for (int i = 0; i < 5; i++) 
    {
        if (applyReward[i].bType == 0)
        {
            continue;
        }

		if (test_server) {
			ch->ChatPacket(CHAT_TYPE_INFO, "Adding Reward %d: Type %d, Value %d", i, applyReward[i].bType, applyReward[i].lValue);
		}
        ch->AddAffect(this->affect_id, aApplyInfo[applyReward[i].bType].bPointType, applyReward[i].lValue, 0, 60 * 60 * 60 * 365, 0, false);
    }
}

void CSystemCollections::PrintCollections()
{
	for (const auto& it : m_Collections)
	{
		sys_log(0, "PrintCollections: Index: %d", it.index);

		for (auto itemIt : it.vec_Items)
		{
			sys_log(0, "PrintCollections: Item: %d %d %d", itemIt.index, itemIt.iVnum, itemIt.iCount);
		}
	}
}

void CSystemCollections::ResetCollections(CHARACTER* ch) {
	if (ch->GetQuestFlag("collection.reseted")) return;

	for (auto& collection : m_Collections) {		
		collection.Reset(ch);
	}

	ch->SetQuestFlag("collection.reseted", 1);
}

void CSystemCollections::SCollection::Reset(CHARACTER* ch) {

	if (IsComplete(ch)) {
		GiveReward(ch);
	}
}

#endif // __ENABLE_COLLECTIONS_SYSTEM__
