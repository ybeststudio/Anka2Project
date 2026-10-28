#pragma once

#include "../../common/tables.h"


#ifdef __ENABLE_COLLECTIONS_SYSTEM__
class CSystemCollections : public singleton<CSystemCollections>
{
	struct SCollectionItem
	{
		BYTE index;
		uint32_t iVnum;
		uint16_t iCount;
	};
	
	using TItems = std::vector<SCollectionItem>;
	
	struct SCollection
	{
		BYTE index;
		BYTE percent;
		int affect_id;
		std::string collectName;
		TItems vec_Items;
		struct Reward
		{
			BYTE bType;
			long lValue;
		};

		Reward applyReward[5];

		bool IsComplete(CHARACTER* ch) const;
		bool AddItem(CHARACTER* ch, const BYTE itemIndex);
		void AddAllItems(CHARACTER* ch, const BYTE itemIndex);
		
		void CompleteCheck(CHARACTER* ch);
		void GiveReward(CHARACTER* ch);
		uint16_t GetActualCount(CHARACTER* ch, BYTE itemIndex) const;


		// Due to wrong configuration we have to reset affect's at first time when log in
		void Reset(CHARACTER* ch);
	};
	
	public:
		CSystemCollections();
		virtual ~CSystemCollections();
	
		bool LoadCategories();
		bool LoadItems();
		
		void AddItem(CHARACTER* ch, const BYTE collectionIndex, const BYTE itemIndex, bool isAll);
		
		// Debug
		void PrintCollections();
		// Server -> Client
		void SendCollections(CHARACTER* ch);

		void ResetCollections(CHARACTER* ch);

	protected:
		std::vector<SCollection> m_Collections;
};

#endif // __ENABLE_COLLECTIONS_SYSTEM__
