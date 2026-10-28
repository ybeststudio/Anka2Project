#pragma once
#include "StdAfx.h"

#ifdef ENABLE_BIOLOG_SYSTEM
#include "Packet.h"

class CPythonBiologManager : public CSingleton<CPythonBiologManager>
{
	public:
		CPythonBiologManager();
		~CPythonBiologManager();

		void Initialize();

		void _LoadBiologInformation(const TPacketGCBiologManagerInfo* pBiologInfo);

		BYTE Biolog_GetCurrentLevel() const { return m_BiologInfo.bRequiredLevel; };
		DWORD Biolog_GetCurrentItem() const { return m_BiologInfo.iRequiredItem; };
		WORD Biolog_GetCurrentGivenItems() const { return m_BiologInfo.wGivenItems; };
		WORD Biolog_GetCurrentRequiredCount() const { return m_BiologInfo.wRequiredItemCount; };
		time_t Biolog_GetCurrentGlobalCooldown() const { return m_BiologInfo.iGlobalCooldown; };
		time_t Biolog_GetCurrentCooldown() const { return m_BiologInfo.iCooldown; };
		bool Biolog_GetCurrentCooldownReminder() const { return m_BiologInfo.iCooldownReminder; }
		DWORD Biolog_GetCurrentRewardType(int index) const { return m_BiologInfo.bApplyType[index]; };
		long Biolog_GetCurrentRewardValue(int index) const { return m_BiologInfo.lApplyValue[index]; }
		DWORD Biolog_GetCurrentRewardItem() const { return m_BiologInfo.dRewardItem; }
		WORD Biolog_GetCurrentRewardItemCount() const { return m_BiologInfo.wRewardItemCount; }

	private:
		TPacketGCBiologManagerInfo	m_BiologInfo;
};

extern void initBiologManager();
#endif