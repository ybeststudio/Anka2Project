#pragma once

#include "StdAfx.h"
#include "AbstractSingleton.h"

class CInstanceBase;

class IAbstractPlayer : public TAbstractSingleton<IAbstractPlayer>
{
	public:
		IAbstractPlayer() {}
		virtual ~IAbstractPlayer() {}

		virtual DWORD	GetMainCharacterIndex() = 0;
		virtual void	SetMainCharacterIndex(int iIndex) = 0;
		virtual bool	IsMainCharacterIndex(DWORD dwIndex) = 0;

#ifdef ENABLE_GOLD_LIMIT
		virtual long long	GetStatus(DWORD dwType) = 0;
#else
		virtual int		GetStatus(DWORD dwType) = 0;
#endif

		virtual const char *	GetName() = 0;

#ifdef ENABLE_GUILD_RANK_SYSTEM
		virtual DWORD			GetGuildID() = 0;
#endif

		virtual void	SetRace(DWORD dwRace) = 0;

		virtual void	StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina) = 0;
		virtual void	StopStaminaConsume(DWORD dwCurrentStamina) = 0;

		virtual bool	IsPartyMemberByVID(DWORD dwVID) = 0;
		virtual bool	PartyMemberVIDToPID(DWORD dwVID, DWORD * pdwPID) = 0;
		virtual bool	IsSamePartyMember(DWORD dwVID1, DWORD dwVID2) = 0;

		virtual void	SetItemData(TItemPos itemPos, const TItemData & c_rkItemInst) = 0;
#ifdef ENABLE_STACK_LIMIT
		virtual void	SetItemCount(TItemPos itemPos, WORD byCount) = 0;
#else
		virtual void	SetItemCount(TItemPos itemPos, BYTE byCount) = 0;
#endif
		virtual void	SetItemMetinSocket(TItemPos itemPos, DWORD dwMetinSocketIndex, DWORD dwMetinNumber) = 0;
		virtual void	SetItemAttribute(TItemPos itemPos, DWORD dwAttrIndex, BYTE byType, short sValue) = 0;

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		virtual void	SetItemTransmutationVnum(TItemPos& Cell, const DWORD blVnum) = 0;
#endif
#ifdef ENABLE_SET_ITEM
		virtual void	SetItemSetValue(const TItemPos& Cell, const DWORD dwSetValue) = 0;
#endif

		virtual DWORD	GetItemIndex(TItemPos itemPos) = 0;
		virtual DWORD	GetItemFlags(TItemPos itemPos) = 0;
		virtual DWORD	GetItemCount(TItemPos itemPos) = 0;

		virtual bool	IsEquipItemInSlot(TItemPos itemPos) = 0;

		virtual void	AddQuickSlot(int QuickslotIndex, char IconType, char IconPosition) = 0;
		virtual void	DeleteQuickSlot(int QuickslotIndex) = 0;
		virtual void	MoveQuickSlot(int Source, int Target) = 0;

		virtual void	SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower) = 0;

#ifdef __AUTO_HUNT__
		virtual void	SetTarget(DWORD dwVID, BOOL bForceChange = TRUE, bool isAutoAttack = false) = 0;
#else
		virtual void	SetTarget(DWORD dwVID, BOOL bForceChange = TRUE) = 0;
#endif
		virtual void	NotifyCharacterUpdate(DWORD dwVID) = 0;
		virtual void	NotifyCharacterDead(DWORD dwVID) = 0;
		virtual void	NotifyDeletingCharacterInstance(DWORD dwVID) = 0;
		virtual void	NotifyChangePKMode() = 0;

		virtual void	SetObserverMode(bool isEnable) = 0;
		virtual void	SetComboSkillFlag(BOOL bFlag) = 0;

		virtual void	StartEmotionProcess() = 0;
		virtual void	EndEmotionProcess() = 0;

		virtual CInstanceBase* NEW_GetMainActorPtr() = 0;
};

