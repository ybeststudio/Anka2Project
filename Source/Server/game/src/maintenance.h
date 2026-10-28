#ifndef __INC_METIN_II_GAME_MAINTENANCE_SYSTEM_H__
#define __INC_METIN_II_GAME_MAINTENANCE_SYSTEM_H__

#pragma once

#ifdef ENABLE_MAINTENANCE_SYSTEM

class MaintenanceManager : public singleton<MaintenanceManager>
{
	public:
		MaintenanceManager();
		~MaintenanceManager();

	void Send_UpdateBinary(LPCHARACTER ch);
	void Send_CheckTable(LPCHARACTER ch);
	void Send_Text(LPCHARACTER ch, const char* reason);
	void Send_DisableSecurity(LPCHARACTER ch);
	void Send_ActiveMaintenance(LPCHARACTER ch, long int time_maintenance, long int duration_maintenance);
};
#endif // ENABLE_MAINTENANCE_SYSTEM

#endif
