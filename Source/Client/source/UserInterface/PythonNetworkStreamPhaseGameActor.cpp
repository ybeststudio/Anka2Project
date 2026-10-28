#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "NetworkActorManager.h"
#include "PythonBackground.h"
#include "PythonApplication.h"
#include "AbstractPlayer.h"
#include "PythonCharacterManager.h"
#include "PythonSkill.h"
#include "../gamelib/ActorInstance.h"

void CPythonNetworkStream::__GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY)
{
	CPythonBackground& rkBgMgr = CPythonBackground::Instance();
	rkBgMgr.GlobalPositionToLocalPosition(rGlobalX, rGlobalY);
}

void CPythonNetworkStream::__LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY)
{
	CPythonBackground& rkBgMgr = CPythonBackground::Instance();
	rkBgMgr.LocalPositionToGlobalPosition(rLocalX, rLocalY);
}

bool CPythonNetworkStream::__CanActMainInstance()
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	return pkInstMain->CanAct();
}

void CPythonNetworkStream::__ClearNetworkActorManager()
{
	m_rokNetActorMgr->Destroy();
}

void __SetWeaponPower(IAbstractPlayer& rkPlayer, DWORD dwWeaponID)
{
	DWORD minPower = 0;
	DWORD maxPower = 0;
	DWORD minMagicPower = 0;
	DWORD maxMagicPower = 0;
	DWORD addPower = 0;

	CItemData* pkWeapon;
	if (CItemManager::Instance().GetItemDataPointer(dwWeaponID, &pkWeapon))
	{
		if (pkWeapon->GetType() == CItemData::ITEM_TYPE_WEAPON)
		{
			minPower = pkWeapon->GetValue(3);
			maxPower = pkWeapon->GetValue(4);
			minMagicPower = pkWeapon->GetValue(1);
			maxMagicPower = pkWeapon->GetValue(2);
			addPower = pkWeapon->GetValue(5);
		}
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	CItemData * pItemData;
	if (CItemManager::Instance().GetItemDataPointer(CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, c_Equipment_Weapon)), &pItemData))
	{
		minPower=pItemData->GetValue(3);
		maxPower=pItemData->GetValue(4);
		minMagicPower=pItemData->GetValue(1);
		maxMagicPower=pItemData->GetValue(2);
		addPower=pItemData->GetValue(5);
	}
#endif

	rkPlayer.SetWeaponPower(minPower, maxPower, minMagicPower, maxMagicPower, addPower);
}

bool IsInvisibleRace(WORD raceNum)
{
	switch(raceNum)
	{
		case 20025:
		case 20038:
		case 20039:
			return true;
		default:
			return false;
	}
}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
bool IsNPCType(BYTE bType)
{
	switch (bType)
	{
		case CActorInstance::TYPE_NPC:
		case CActorInstance::TYPE_HORSE:
#ifdef ENABLE_PET_SYSTEM
		case CActorInstance::TYPE_PET:
#endif
#ifdef ENABLE_MOUNT_SYSTEM
		case CActorInstance::TYPE_MOUNT:
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		case CActorInstance::TYPE_GROWTH_PET:
#endif
			return true;
		default:
			return false;
	}
}
#endif

static SNetworkActorData s_kNetActorData;

bool CPythonNetworkStream::RecvCharacterAppendPacket()
{
	TPacketGCCharacterAdd chrAddPacket;
	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;

	__GlobalPositionToLocalPosition(chrAddPacket.x, chrAddPacket.y);

	SNetworkActorData kNetActorData;
	kNetActorData.m_bType = chrAddPacket.bType;
	kNetActorData.m_dwMovSpd = chrAddPacket.bMovingSpeed;
	kNetActorData.m_dwAtkSpd = chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace = chrAddPacket.wRaceNum;

	kNetActorData.m_dwStateFlags = chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID = chrAddPacket.dwVID;
	kNetActorData.m_fRot = chrAddPacket.angle;

	kNetActorData.m_stName = "";

	kNetActorData.m_stName = "";
	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);

	kNetActorData.SetPosition(chrAddPacket.x, chrAddPacket.y);

	kNetActorData.m_sAlignment = 0;
	kNetActorData.m_byPKMode = 0;
	kNetActorData.m_dwGuildID = 0;
	kNetActorData.m_dwEmpireID = 0;
	kNetActorData.m_dwArmor = 0;
	kNetActorData.m_dwWeapon = 0;
	kNetActorData.m_dwHair = 0;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	kNetActorData.m_dwAcce = 0;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	kNetActorData.m_dwAura = 0;
#endif
	kNetActorData.m_dwMountVnum = 0;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	kNetActorData.m_dwGuildLeader = 0;
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	kNetActorData.m_bLanguage = 0;
	kNetActorData.m_bLanguage2 = 0;
#endif

#ifdef ENABLE_SHOW_MOB_INFO
	kNetActorData.m_dwLevel = chrAddPacket.dwLevel;
	kNetActorData.m_dwAIFlag = chrAddPacket.dwAIFlag;
#else
	kNetActorData.m_dwLevel = 0;
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
	kNetActorData.m_dwConquerorLevel = 0;
#endif
	if (kNetActorData.m_bType != CActorInstance::TYPE_PC && !IsNPCType(kNetActorData.m_bType))
	{
		const char *c_szName;
		CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
		if (rkNonPlayer.GetName(kNetActorData.m_dwRace, &c_szName))
			kNetActorData.m_stName = c_szName;

		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		s_kNetActorData = kNetActorData;
	}

	return true;
}

bool CPythonNetworkStream::RecvCharacterAdditionalInfo()
{
	TPacketGCCharacterAdditionalInfo chrInfoPacket;
	if (!Recv(sizeof(chrInfoPacket), &chrInfoPacket))
		return false;

	SNetworkActorData kNetActorData = s_kNetActorData;
	if (IsInvisibleRace(kNetActorData.m_dwRace))
		return true;

	if (kNetActorData.m_dwVID == chrInfoPacket.dwVID)
	{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		const char *c_szName;
		if (CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szName))
		{
			switch (kNetActorData.m_bType)
			{
				case CActorInstance::TYPE_NPC:
					kNetActorData.m_stName = c_szName;
					break;

#ifdef ENABLE_PET_SYSTEM
				case CActorInstance::TYPE_PET:
#ifdef ENABLE_BUFFI_SYSTEM
					// Buffi: only its own name ("Buffi"), not "owner - mob name"
					if (kNetActorData.m_dwRace == 34900 || kNetActorData.m_dwRace == 34901)
					{
						kNetActorData.m_stName = chrInfoPacket.name;
						break;
					}
#endif
					char szPetName[1024];
					snprintf(szPetName, sizeof(szPetName), "%s - %s", chrInfoPacket.name, c_szName);
					kNetActorData.m_stName = szPetName;
					break;
#endif

#ifdef ENABLE_MOUNT_SYSTEM
				case CActorInstance::TYPE_MOUNT:
					char szMountName[1024];
					snprintf(szMountName, sizeof(szMountName), "%s - %s", chrInfoPacket.name, c_szName);
					kNetActorData.m_stName = szMountName;
					break;
#endif

				default:
					kNetActorData.m_stName = chrInfoPacket.name;
					break;
			}
		}
		else
			kNetActorData.m_stName = chrInfoPacket.name;
#else
		kNetActorData.m_stName = chrInfoPacket.name;
#endif
		kNetActorData.m_dwGuildID = chrInfoPacket.dwGuildID;
		kNetActorData.m_dwLevel = chrInfoPacket.dwLevel;
#ifdef ENABLE_CONQUEROR_LEVEL
		kNetActorData.m_dwConquerorLevel = chrInfoPacket.dwConquerorLevel;
#endif
		kNetActorData.m_sAlignment = chrInfoPacket.sAlignment;
		kNetActorData.m_byPKMode = chrInfoPacket.bPKMode;
		kNetActorData.m_dwGuildID = chrInfoPacket.dwGuildID;
		kNetActorData.m_dwEmpireID = chrInfoPacket.bEmpire;
		kNetActorData.m_dwArmor = chrInfoPacket.awPart[CHR_EQUIPPART_ARMOR];
		kNetActorData.m_dwWeapon = chrInfoPacket.awPart[CHR_EQUIPPART_WEAPON];
		kNetActorData.m_dwHair = chrInfoPacket.awPart[CHR_EQUIPPART_HAIR];
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		kNetActorData.m_dwAcce = chrInfoPacket.awPart[CHR_EQUIPPART_ACCE];
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		kNetActorData.m_dwAura = chrInfoPacket.awPart[CHR_EQUIPPART_AURA];
#endif
		kNetActorData.m_dwMountVnum = chrInfoPacket.dwMountVnum;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
		kNetActorData.m_dwGuildLeader = chrInfoPacket.dwGuildLeader;
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		kNetActorData.m_bLanguage = chrInfoPacket.bLanguage;
		kNetActorData.m_bLanguage2 = chrInfoPacket.bLanguage2;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		memcpy(kNetActorData.m_dwSkillColor, chrInfoPacket.dwSkillColor, sizeof(kNetActorData.m_dwSkillColor));
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
		kNetActorData.m_bCharacterSize = chrInfoPacket.bCharacterSize;
#endif

		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		TraceError("TPacketGCCharacterAdditionalInfo name=%s vid=%d race=%d Error",chrInfoPacket.name,chrInfoPacket.dwVID,kNetActorData.m_dwRace);
	}
	return true;
}

bool CPythonNetworkStream::RecvCharacterAppendPacketNew()
{
	TraceError("TPacketGCCharacterAdd2 is packet that doesn't write.");
	TPacketGCCharacterAdd2 chrAddPacket;

	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;

	if(IsInvisibleRace(chrAddPacket.wRaceNum))
		return true;

	__GlobalPositionToLocalPosition(chrAddPacket.x, chrAddPacket.y);

	SNetworkActorData kNetActorData;
	kNetActorData.m_dwLevel = 0;
#ifdef ENABLE_CONQUEROR_LEVEL
	kNetActorData.m_dwConquerorLevel = 0;
#endif
	kNetActorData.m_bType = chrAddPacket.bType;
	kNetActorData.m_dwGuildID = chrAddPacket.dwGuild;
	kNetActorData.m_dwEmpireID = chrAddPacket.bEmpire;
	kNetActorData.m_dwMovSpd = chrAddPacket.bMovingSpeed;
	kNetActorData.m_dwAtkSpd = chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace = chrAddPacket.wRaceNum;
	kNetActorData.m_dwArmor = chrAddPacket.awPart[CHR_EQUIPPART_ARMOR];
	kNetActorData.m_dwWeapon = chrAddPacket.awPart[CHR_EQUIPPART_WEAPON];
	kNetActorData.m_dwHair = chrAddPacket.awPart[CHR_EQUIPPART_HAIR];
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	kNetActorData.m_dwAcce = chrAddPacket.awPart[CHR_EQUIPPART_ACCE];
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	kNetActorData.m_dwAura = chrAddPacket.awPart[CHR_EQUIPPART_AURA];
#endif
	kNetActorData.m_dwStateFlags = chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID = chrAddPacket.dwVID;
	kNetActorData.m_dwMountVnum = chrAddPacket.dwMountVnum;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	kNetActorData.m_dwGuildLeader = chrAddPacket.dwGuildLeader;
#endif
	kNetActorData.m_fRot = chrAddPacket.angle;
	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);
	kNetActorData.SetPosition(chrAddPacket.x, chrAddPacket.y);
	kNetActorData.m_sAlignment = chrAddPacket.sAlignment;
	kNetActorData.m_byPKMode = chrAddPacket.bPKMode;
	kNetActorData.m_stName = chrAddPacket.name;
	__RecvCharacterAppendPacket(&kNetActorData);

	return true;
}

bool CPythonNetworkStream::RecvCharacterUpdatePacket()
{
	TPacketGCCharacterUpdate chrUpdatePacket;
	if (!Recv(sizeof(chrUpdatePacket), &chrUpdatePacket))
		return false;

	SNetworkUpdateActorData kNetUpdateActorData;
#ifdef ENABLE_CONQUEROR_LEVEL
	kNetUpdateActorData.m_dwConquerorLevel=chrUpdatePacket.dwConquerorLevel;
#endif
	kNetUpdateActorData.m_dwGuildID = chrUpdatePacket.dwGuildID;
	kNetUpdateActorData.m_dwMovSpd = chrUpdatePacket.bMovingSpeed;
	kNetUpdateActorData.m_dwAtkSpd = chrUpdatePacket.bAttackSpeed;
	kNetUpdateActorData.m_dwArmor = chrUpdatePacket.awPart[CHR_EQUIPPART_ARMOR];
	kNetUpdateActorData.m_dwWeapon = chrUpdatePacket.awPart[CHR_EQUIPPART_WEAPON];
	kNetUpdateActorData.m_dwHair = chrUpdatePacket.awPart[CHR_EQUIPPART_HAIR];
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	kNetUpdateActorData.m_dwAcce = chrUpdatePacket.awPart[CHR_EQUIPPART_ACCE];
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	kNetUpdateActorData.m_dwAura = chrUpdatePacket.awPart[CHR_EQUIPPART_AURA];
#endif
	kNetUpdateActorData.m_dwVID = chrUpdatePacket.dwVID;
	kNetUpdateActorData.m_kAffectFlags.CopyData(0, sizeof(chrUpdatePacket.dwAffectFlag[0]), &chrUpdatePacket.dwAffectFlag[0]);
	kNetUpdateActorData.m_kAffectFlags.CopyData(32, sizeof(chrUpdatePacket.dwAffectFlag[1]), &chrUpdatePacket.dwAffectFlag[1]);
	kNetUpdateActorData.m_sAlignment = chrUpdatePacket.sAlignment;
	kNetUpdateActorData.m_byPKMode = chrUpdatePacket.bPKMode;
	kNetUpdateActorData.m_dwStateFlags = chrUpdatePacket.bStateFlag;
	kNetUpdateActorData.m_dwMountVnum = chrUpdatePacket.dwMountVnum;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	kNetUpdateActorData.m_dwGuildLeader = chrUpdatePacket.dwGuildLeader;
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	kNetUpdateActorData.m_bLanguage = chrUpdatePacket.bLanguage;
	kNetUpdateActorData.m_bLanguage2 = chrUpdatePacket.bLanguage2;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	memcpy(kNetUpdateActorData.m_dwSkillColor, chrUpdatePacket.dwSkillColor, sizeof(kNetUpdateActorData.m_dwSkillColor));
#endif

	__RecvCharacterUpdatePacket(&kNetUpdateActorData);

	return true;
}

bool CPythonNetworkStream::RecvCharacterUpdatePacketNew()
{
	TPacketGCCharacterUpdate2 chrUpdatePacket;
	if (!Recv(sizeof(chrUpdatePacket), &chrUpdatePacket))
		return false;

	SNetworkUpdateActorData kNetUpdateActorData;
	kNetUpdateActorData.m_dwGuildID = chrUpdatePacket.dwGuildID;
	kNetUpdateActorData.m_dwMovSpd = chrUpdatePacket.bMovingSpeed;
	kNetUpdateActorData.m_dwAtkSpd = chrUpdatePacket.bAttackSpeed;
	kNetUpdateActorData.m_dwArmor = chrUpdatePacket.awPart[CHR_EQUIPPART_ARMOR];
	kNetUpdateActorData.m_dwWeapon = chrUpdatePacket.awPart[CHR_EQUIPPART_WEAPON];
	kNetUpdateActorData.m_dwHair = chrUpdatePacket.awPart[CHR_EQUIPPART_HAIR];
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	kNetUpdateActorData.m_dwAcce = chrUpdatePacket.awPart[CHR_EQUIPPART_ACCE];
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	kNetUpdateActorData.m_dwAura = chrUpdatePacket.awPart[CHR_EQUIPPART_AURA];
#endif
	kNetUpdateActorData.m_dwVID = chrUpdatePacket.dwVID;
	kNetUpdateActorData.m_kAffectFlags.CopyData(0, sizeof(chrUpdatePacket.dwAffectFlag[0]), &chrUpdatePacket.dwAffectFlag[0]);
	kNetUpdateActorData.m_kAffectFlags.CopyData(32, sizeof(chrUpdatePacket.dwAffectFlag[1]), &chrUpdatePacket.dwAffectFlag[1]);
	kNetUpdateActorData.m_sAlignment = chrUpdatePacket.sAlignment;
	kNetUpdateActorData.m_byPKMode = chrUpdatePacket.bPKMode;
	kNetUpdateActorData.m_dwStateFlags = chrUpdatePacket.bStateFlag;
	kNetUpdateActorData.m_dwMountVnum = chrUpdatePacket.dwMountVnum;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	kNetUpdateActorData.m_dwGuildLeader = chrUpdatePacket.dwGuildLeader;
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	kNetUpdateActorData.m_bLanguage = chrUpdatePacket.bLanguage;
	kNetUpdateActorData.m_bLanguage2 = chrUpdatePacket.bLanguage2;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	memcpy(kNetUpdateActorData.m_dwSkillColor, chrUpdatePacket.dwSkillColor, sizeof(kNetUpdateActorData.m_dwSkillColor));
#endif

	__RecvCharacterUpdatePacket(&kNetUpdateActorData);

	return true;
}

void CPythonNetworkStream::__RecvCharacterAppendPacket(SNetworkActorData * pkNetActorData)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetActorData->m_dwVID))
	{
		rkPlayer.SetRace(pkNetActorData->m_dwRace);

		__SetWeaponPower(rkPlayer, pkNetActorData->m_dwWeapon);

		if (rkPlayer.NEW_GetMainActorPtr())
		{
			CPythonBackground::Instance().Update(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY, 0.0f);
			CPythonCharacterManager::Instance().Update();

			{
				std::string strMapName = CPythonBackground::Instance().GetWarpMapName();
				if (strMapName == "metin2_map_deviltower1")
					__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
			}
		}
		else
		{
			__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
		}
	}

	m_rokNetActorMgr->AppendActor(*pkNetActorData);

	if (GetMainActorVID()==pkNetActorData->m_dwVID)
	{
		rkPlayer.SetTarget(0);
		if (m_bComboSkillFlag)
			rkPlayer.SetComboSkillFlag(m_bComboSkillFlag);

		__SetGuildID(pkNetActorData->m_dwGuildID);
	}
}

void CPythonNetworkStream::__RecvCharacterUpdatePacket(SNetworkUpdateActorData * pkNetUpdateActorData)
{
	m_rokNetActorMgr->UpdateActor(*pkNetUpdateActorData);

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetUpdateActorData->m_dwVID))
	{
		__SetGuildID(pkNetUpdateActorData->m_dwGuildID);
		__SetWeaponPower(rkPlayer, pkNetUpdateActorData->m_dwWeapon);

		__RefreshStatus();
		__RefreshAlignmentWindow();
		__RefreshEquipmentWindow();
		__RefreshInventoryWindow();
		m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].wHairPart = pkNetUpdateActorData->m_dwHair;
		m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].wMainPart = pkNetUpdateActorData->m_dwArmor;
	}
	else
	{
		rkPlayer.NotifyCharacterUpdate(pkNetUpdateActorData->m_dwVID);
	}
}

bool CPythonNetworkStream::RecvCharacterDeletePacket()
{
	TPacketGCCharacterDelete chrDelPacket;

	if (!Recv(sizeof(chrDelPacket), &chrDelPacket))
	{
		TraceError("CPythonNetworkStream::RecvCharacterDeletePacket - Recv Error");
		return false;
	}

	m_rokNetActorMgr->RemoveActor(chrDelPacket.dwVID);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"BINARY_PrivateShop_Disappear",
		Py_BuildValue("(i)", chrDelPacket.dwVID)
	);

	return true;
}

bool CPythonNetworkStream::RecvCharacterMovePacket()
{
	TPacketGCMove kMovePacket;
	if (!Recv(sizeof(TPacketGCMove), &kMovePacket))
	{
		Tracen("CPythonNetworkStream::RecvCharacterMovePacket - PACKET READ ERROR");
		return false;
	}

	__GlobalPositionToLocalPosition(kMovePacket.lX, kMovePacket.lY);

	SNetworkMoveActorData kNetMoveActorData;
	kNetMoveActorData.m_dwArg=kMovePacket.bArg;
	kNetMoveActorData.m_dwFunc=kMovePacket.bFunc;
	kNetMoveActorData.m_dwTime=kMovePacket.dwTime;
	kNetMoveActorData.m_dwVID=kMovePacket.dwVID;
	kNetMoveActorData.m_fRot=kMovePacket.bRot*5.0f;
	kNetMoveActorData.m_lPosX=kMovePacket.lX;
	kNetMoveActorData.m_lPosY=kMovePacket.lY;
	kNetMoveActorData.m_dwDuration=kMovePacket.dwDuration;

	m_rokNetActorMgr->MoveActor(kNetMoveActorData);

	return true;
}

bool CPythonNetworkStream::RecvOwnerShipPacket()
{
	TPacketGCOwnership kPacketOwnership;

	if (!Recv(sizeof(kPacketOwnership), &kPacketOwnership))
		return false;

	m_rokNetActorMgr->SetActorOwner(kPacketOwnership.dwOwnerVID, kPacketOwnership.dwVictimVID);

	return true;
}

bool CPythonNetworkStream::RecvSyncPositionPacket()
{
	TPacketGCSyncPosition kPacketSyncPos;
	if (!Recv(sizeof(kPacketSyncPos), &kPacketSyncPos))
		return false;

	TPacketGCSyncPositionElement kSyncPos;

	UINT uSyncPosCount=(kPacketSyncPos.wSize-sizeof(kPacketSyncPos))/sizeof(kSyncPos);
	for (UINT iSyncPos=0; iSyncPos<uSyncPosCount; ++iSyncPos)
	{
		if (!Recv(sizeof(TPacketGCSyncPositionElement), &kSyncPos))
			return false;

		__GlobalPositionToLocalPosition(kSyncPos.lX, kSyncPos.lY);
		m_rokNetActorMgr->SyncActor(kSyncPos.dwVID, kSyncPos.lX, kSyncPos.lY);
	}

	return true;
}

#ifdef ENABLE_BUFFI_SYSTEM
// The Buffi (support shaman NPC) casts a buff: play the shaman skill motion and its effects on it.
bool CPythonNetworkStream::RecvBuffiUseSkill()
{
	TPacketGCBuffiUseSkill packet;
	if (!Recv(sizeof(TPacketGCBuffiUseSkill), &packet))
		return false;

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(packet.dwVID);
	if (!pInstance)
		return true;

	CPythonSkill::TSkillData* pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(packet.dwSkillVnum, &pSkillData))
		return true;

	int iGrade = 0;
	if (packet.bSkillLevel >= 40)
		iGrade = 3;
	else if (packet.bSkillLevel >= 30)
		iGrade = 2;
	else if (packet.bSkillLevel >= 20)
		iGrade = 1;

	pInstance->NEW_UseSkill(packet.dwSkillVnum, pSkillData->GetSkillMotionIndex(iGrade), 1, false);
	return true;
}
#endif
