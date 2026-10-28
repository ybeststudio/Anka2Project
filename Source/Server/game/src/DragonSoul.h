#ifndef __INC_METIN_II_GAME_DRAGON_SOUL_H__
#define __INC_METIN_II_GAME_DRAGON_SOUL_H__

#include "../../common/length.h"

class CHARACTER;
class CItem;

class DragonSoulTable;

class DSManager : public singleton<DSManager>
{
	public:
		DSManager();
		~DSManager();
		bool	ReadDragonSoulTableFile(const char * c_pszFileName);

		void	GetDragonSoulInfo(DWORD dwVnum, OUT BYTE& bType, OUT BYTE& bGrade, OUT BYTE& bStep, OUT BYTE& bRefine) const;
		WORD	GetBasePosition(const LPITEM pItem) const;
		bool	IsValidCellForThisItem(const LPITEM pItem, const TItemPos& Cell) const;
		int		GetDuration(const LPITEM pItem) const;

		bool	ExtractDragonHeart(LPCHARACTER ch, LPITEM pItem, LPITEM pExtractor = NULL);
		bool	PullOut(LPCHARACTER ch, TItemPos DestCell, IN OUT LPITEM& pItem, LPITEM pExtractor = NULL);
		bool	DoRefineGrade(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
		bool	DoRefineStep(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
		bool	DoRefineStrength(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
		bool	DragonSoulItemInitialize(LPITEM pItem);
		bool	IsTimeLeftDragonSoul(LPITEM pItem) const;
		int		LeftTime(LPITEM pItem) const;
		bool	ActivateDragonSoul(LPITEM pItem);
		bool	DeactivateDragonSoul(LPITEM pItem, bool bSkipRefreshOwnerActiveState = false);
		bool	IsActiveDragonSoul(LPITEM pItem) const;

	private:
		void	SendRefineResultPacket(LPCHARACTER ch, BYTE bSubHeader, const TItemPos& pos);
		void	RefreshDragonSoulState(LPCHARACTER ch);

		DWORD	MakeDragonSoulVnum(BYTE bType, BYTE grade, BYTE step, BYTE refine);
		bool	PutAttributes(LPITEM pDS);
		bool	RefreshItemAttributes(LPITEM pItem);

		DragonSoulTable*	m_pTable;
};

#endif
