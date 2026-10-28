#ifndef __HORSE_H
#define __HORSE_H

#include "constants.h"
#include "cmd.h"

#if !defined(ENABLE_RIDING_EXTENDED)
const int HORSE_MAX_LEVEL = 30;
#endif

struct THorseStat
{
	int iMinLevel;
	int iNPCRace;
	int iMaxHealth;
	int iMaxStamina;
	int iST;
	int iDX;
	int iHT;
	int iIQ;
/*	#Ýleriye dönük Yohara Sistemi eklersen bu kod bloðunu aktif et.
	#If you add the forward-looking Yohara System, activate this code block.If you add the forward-looking Yohara System, activate this code block.
#ifdef ENABLE_RIDING_EXTENDED
	int iSungMaST;
	int iSungMaDX;
	int iSungMaHT;
	int iSungMaIQ;
#endif
*/
	int iDamMean;
	int iDamMin;
	int iDamMax;
	int iArmor;
};

extern THorseStat c_aHorseStat[HORSE_MAX_LEVEL + 1];

class CHorseRider
{
	public:
		CHorseRider();
		virtual ~CHorseRider();

		BYTE		GetHorseLevel() const { return m_Horse.bLevel; }
		BYTE		GetHorseGrade();
		short		GetHorseHealth() const	{ return m_Horse.sHealth; }
		short		GetHorseStamina() const	{ return m_Horse.sStamina; }
		short		GetHorseMaxHealth();
		short		GetHorseMaxStamina();

		int		GetHorseST()		{ return c_aHorseStat[GetHorseLevel()].iST; }
		int		GetHorseDX()		{ return c_aHorseStat[GetHorseLevel()].iDX; }
		int		GetHorseHT()		{ return c_aHorseStat[GetHorseLevel()].iHT; }
		int		GetHorseIQ()		{ return c_aHorseStat[GetHorseLevel()].iIQ; }
/*	#Ýleriye dönük Yohara Sistemi eklersen bu kod bloðunu aktif et.
	#If you add the forward-looking Yohara System, activate this code block.If you add the forward-looking Yohara System, activate this code block.
#ifdef ENABLE_RIDING_EXTENDED
		int GetHorseSungMaST() { return c_aHorseStat[GetHorseLevel()].iSungMaST; }
		int GetHorseSungMaDX() { return c_aHorseStat[GetHorseLevel()].iSungMaDX; }
		int GetHorseSungMaHT() { return c_aHorseStat[GetHorseLevel()].iSungMaHT; }
		int GetHorseSungMaIQ() { return c_aHorseStat[GetHorseLevel()].iSungMaIQ; }
#endif
*/
		int		GetHorseArmor()		{ return c_aHorseStat[GetHorseLevel()].iArmor; }

		virtual bool ReviveHorse();
		void FeedHorse();
		virtual void HorseDie();

		bool IsHorseRiding() const		{ return m_Horse.bRiding; }

		void ResetHorseHealthDropTime();

		virtual void SetHorseLevel(int iLevel);

		void EnterHorse();

		virtual void SendHorseInfo() {}
		virtual void ClearHorseInfo() {}

		virtual void UpdateRideTime(int interval) {}

	protected:
		void SetHorseData(const THorseInfo& crInfo);
		const THorseInfo& GetHorseData() const { return m_Horse; }

		void UpdateHorseDataByLogoff(DWORD dwLogoffTime);

		virtual bool StartRiding();
		virtual bool StopRiding();

		virtual	DWORD GetMyHorseVnum() const { return 20030; }

	private:
		void UpdateHorseStamina(int iStamina, bool bSend = true);

		void StartStaminaConsumeEvent();
		void StartStaminaRegenEvent();

		void UpdateHorseHealth(int iHealth, bool bSend = true);
		void CheckHorseHealthDropTime(bool bSend = true);

		void Initialize();
		void Destroy();

		THorseInfo m_Horse;

		LPEVENT	m_eventStaminaRegen;
		LPEVENT	m_eventStaminaConsume;

		friend EVENTFUNC(horse_stamina_regen_event);
		friend EVENTFUNC(horse_stamina_consume_event);
		friend ACMD(do_horse_set_stat);
};

#endif
