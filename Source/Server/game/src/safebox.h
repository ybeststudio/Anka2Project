#ifndef __INC_METIN_II_GAME_SAFEBOX_H__
#define __INC_METIN_II_GAME_SAFEBOX_H__

#include "../../common/service.h"

class CHARACTER;
class CItem;
class CGrid;

#ifdef ENABLE_GROWTH_PET_SYSTEM
	#include "growth_pet.h"
#endif

class CSafebox
{
	public:
		CSafebox(LPCHARACTER pkChrOwner, int iSize, DWORD dwGold);
		~CSafebox();

		bool Add(DWORD dwPos, LPITEM pkItem);
		LPITEM Get(DWORD dwPos);
		LPITEM Remove(DWORD dwPos);
		void ChangeSize(int iSize);

#if defined(ENABLE_EXTENDED_SAFEBOX) && defined(ENABLE_STACK_LIMIT)
		bool MoveItem(DWORD bCell, DWORD bDestCell, WORD count);
#elif defined(ENABLE_EXTENDED_SAFEBOX) && !defined(ENABLE_STACK_LIMIT)
		bool MoveItem(DWORD bCell, DWORD bDestCell, BYTE count);
#elif defined(ENABLE_STACK_LIMIT) && !defined(ENABLE_EXTENDED_SAFEBOX)
		bool MoveItem(BYTE bCell, BYTE bDestCell, WORD count);
#else
		bool MoveItem(BYTE bCell, BYTE bDestCell, BYTE count);
#endif

#ifdef ENABLE_EXTENDED_SAFEBOX
		LPITEM GetItem(DWORD bCell);
#else
		LPITEM GetItem(BYTE bCell);
#endif

		void Save();

		bool IsEmpty(DWORD dwPos, BYTE bSize);
		bool IsValidPosition(DWORD dwPos);

		void SetWindowMode(BYTE bWindowMode);

#ifdef ENABLE_GROWTH_PET_SYSTEM
		void LoadPet(DWORD dwCount, TGrowthPet* pPets);
		void AddPet(LPGROWTH_PET pPet);
		bool RemovePet(LPITEM pSummonItem);
#endif

	protected:
		void __Destroy();

		LPCHARACTER m_pkChrOwner;
		LPITEM m_pkItems[SAFEBOX_MAX_NUM];
		CGrid *m_pkGrid;
		int m_iSize;
		long m_lGold;

		BYTE m_bWindowMode;

#ifdef ENABLE_GROWTH_PET_SYSTEM
		CGrowthPetManager::TGrowthPetMap m_growthPetMap;
#endif
};
#endif
