#pragma once

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
#include "../../common/length.h"
#include <array>

class CTransmutation
{
	public:
		CTransmutation(const LPCHARACTER ch, const bool bType);
		~CTransmutation();

		static void Open(const LPCHARACTER ch, const bool bType);

		void ItemCheckIn(const TItemPos& pos, const size_t bSlotType);
		void ItemCheckOut(const size_t bSlotType);

		void FreeItemCheckIn(const TItemPos& pos);
		void FreeItemCheckOut();

		void Accept();

		bool IsTypeItem() const;
		bool IsTypeMount() const;

	private:
		bool CanAddItem(const LPITEM item) const;
		bool CheckOtherItem(const LPITEM item) const;
		bool CanAddFreeItem(const LPITEM item) const;

		LPITEM GetLeftItem() const;
		LPITEM GetRightItem() const;
		LPITEM GetFreeItem() const;

		DWORD GetPrice() const;

	private:
		enum class ETRANSMUTATIONTYPE : BYTE
		{
			TRANSMUTATION_TYPE_MOUNT,
			TRANSMUTATION_TYPE_ITEM
		};

		enum class ETRANSMUTATIONSLOTTYPE : size_t
		{
			TRANSMUTATION_SLOT_LEFT,
			TRANSMUTATION_SLOT_RIGHT,

			TRANSMUTATION_SLOT_MAX
		};

		enum class ETRANSMUTATIONSETTINGS : DWORD
		{
			TRANSMUTATION_ITEM_PRICE = 50000000, // 50M
			TRANSMUTATION_MOUNT_PRICE = 30000000, // 30M
		};

	private:
		LPCHARACTER m_ch;
		ETRANSMUTATIONTYPE m_type;
		std::array<LPITEM, static_cast<size_t>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_MAX)> m_Item;
		LPITEM m_FreeItem;
};

#endif
