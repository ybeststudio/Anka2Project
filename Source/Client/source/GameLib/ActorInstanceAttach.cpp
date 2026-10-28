#include "StdAfx.h"
#include "../EffectLib/EffectManager.h"

#include "ActorInstance.h"
#include "ItemData.h"
#include "ItemManager.h"
#include "RaceData.h"
#include "WeaponTrace.h"

BOOL USE_WEAPON_SPECULAR = TRUE;

DWORD CActorInstance::AttachSmokeEffect(DWORD eSmoke)
{
	if (!m_pkCurRaceData)
		return 0;

	DWORD dwSmokeEffectID = m_pkCurRaceData->GetSmokeEffectID(eSmoke);

	return AttachEffectByID(0, m_pkCurRaceData->GetSmokeBone().c_str(), dwSmokeEffectID); 
}

bool CActorInstance::__IsLeftHandWeapon(DWORD type)
{
#ifdef ENABLE_STANDING_MOUNT
	if (CItemData::WEAPON_DAGGER == type)
		return true;
	if (CItemData::WEAPON_FAN == type && __IsMountingHorse())
	{
		// Standing mount (hoverboard) motionlarÃƒÆ’Ã¢â‚¬ï¿½Ãƒâ€šÃ‚Â±nda fan sol ele takÃƒÆ’Ã¢â‚¬ï¿½Ãƒâ€šÃ‚Â±lmamalÃƒÆ’Ã¢â‚¬ï¿½Ãƒâ€šÃ‚Â±.
		// (BazÃƒÆ’Ã¢â‚¬ï¿½Ãƒâ€šÃ‚Â± durumlarda horse race beklenenden farklÃƒÆ’Ã¢â‚¬ï¿½Ãƒâ€šÃ‚Â± gelebiliyor; motion mode ile garantile.)
		const WORD wMotionMode = GetMotionMode();
		if (wMotionMode == CRaceMotionData::MODE_HORSE_STAND ||
			wMotionMode == CRaceMotionData::MODE_HORSE_STAND_FAN)
			return false;

		DWORD dwRace = m_pkHorse->GetRace();
		if (dwRace >= STANDING_MOUNT_VNUM_1 && dwRace <= STANDING_MOUNT_VNUM_3)
			return false;
		if (dwRace != STANDING_MOUNT_VNUM_1 && 
			dwRace != STANDING_MOUNT_VNUM_2 && 
			dwRace != STANDING_MOUNT_VNUM_3)
			return true;
	}
	else if (CItemData::WEAPON_FAN == type)
	{
		// Yaya iken fan tek elde olmalÃ„Â±.
		return false;
	}
#else
	if (CItemData::WEAPON_DAGGER == type || (CItemData::WEAPON_FAN == type && __IsMountingHorse()))
		return true;
#endif
	else if (CItemData::WEAPON_BOW == type)
		return true;
	else
		return false;
}

bool CActorInstance::__IsRightHandWeapon(DWORD type)
{
#ifdef ENABLE_STANDING_MOUNT
	if (CItemData::WEAPON_DAGGER == type)
		return true;
	if (CItemData::WEAPON_FAN == type && __IsMountingHorse())
	{
		DWORD dwRace = m_pkHorse->GetRace();
		if (dwRace != STANDING_MOUNT_VNUM_1 && 
			dwRace != STANDING_MOUNT_VNUM_2 && 
			dwRace != STANDING_MOUNT_VNUM_3)
			return true;
	}
#else
	if (CItemData::WEAPON_DAGGER == type || (CItemData::WEAPON_FAN == type && __IsMountingHorse()))
		return true;
#endif
	else if (CItemData::WEAPON_BOW == type)
		return false;
	else 
		return true;
}

bool CActorInstance::__IsWeaponTrace(DWORD weaponType)
{
	switch(weaponType)
	{
		case CItemData::WEAPON_BELL:
		case CItemData::WEAPON_FAN:
		case CItemData::WEAPON_BOW:
			return false;
		default:
			return true;
	}
}

#ifdef ENABLE_AURA_COSTUME_SYSTEM
void CActorInstance::ChangePart(DWORD dwPartIndex, DWORD dwItemIndex)
{
	m_adwPartItemID[dwPartIndex] = dwItemIndex;
}
#endif

void CActorInstance::AttachWeapon(DWORD dwItemIndex, DWORD dwParentPartIndex, DWORD dwPartIndex)
{
	if (dwPartIndex>=CRaceData::PART_MAX_NUM)
		return;

	m_adwPartItemID[dwPartIndex]=dwItemIndex;

	CItemData * pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwItemIndex, &pItemData))
	{
		RegisterModelThing(dwPartIndex, nullptr);
		SetModelInstance(dwPartIndex, dwPartIndex, 0);

		RegisterModelThing(CRaceData::PART_WEAPON_LEFT, nullptr);
		SetModelInstance(CRaceData::PART_WEAPON_LEFT, CRaceData::PART_WEAPON_LEFT, 0);

		RefreshActorInstance();
		return;
	}

	__DestroyWeaponTrace();

	if (pItemData->GetType() != CItemData::ITEM_TYPE_COSTUME)
	{
		const DWORD weaponType = pItemData->GetWeaponType();
		const bool bRight = __IsRightHandWeapon(weaponType);
		const bool bLeft = __IsLeftHandWeapon(weaponType);

		if (!bLeft)
		{
			RegisterModelThing(CRaceData::PART_WEAPON_LEFT, nullptr);
			SetModelInstance(CRaceData::PART_WEAPON_LEFT, CRaceData::PART_WEAPON_LEFT, 0);
		}

		if (bRight)
			AttachWeapon(dwParentPartIndex, CRaceData::PART_WEAPON, pItemData);
		
		if (bLeft)
			AttachWeapon(dwParentPartIndex, CRaceData::PART_WEAPON_LEFT, pItemData);
	}
	else
	{
		DWORD typeDec = pItemData->GetValue(3);
		const bool bRight = __IsRightHandWeapon(typeDec);
		const bool bLeft = __IsLeftHandWeapon(typeDec);

		if (!bLeft)
		{
			RegisterModelThing(CRaceData::PART_WEAPON_LEFT, nullptr);
			SetModelInstance(CRaceData::PART_WEAPON_LEFT, CRaceData::PART_WEAPON_LEFT, 0);
		}

		if (bRight)
			AttachWeapon(dwParentPartIndex, CRaceData::PART_WEAPON, pItemData);
		
		if (bLeft)
			AttachWeapon(dwParentPartIndex, CRaceData::PART_WEAPON_LEFT, pItemData);
	}
}

BOOL CActorInstance::GetAttachingBoneName(DWORD dwPartIndex, const char ** c_pszBoneName)
{
	return m_pkCurRaceData->GetAttachingBoneName(dwPartIndex, c_pszBoneName);
}


void CActorInstance::AttachWeapon(DWORD dwParentPartIndex, DWORD dwPartIndex, CItemData * pItemData)
{
	if (!pItemData)
		return;

	const char *szBoneName;
	if (!GetAttachingBoneName(dwPartIndex, &szBoneName))
		return;

	if (CRaceData::PART_WEAPON_LEFT == dwPartIndex)
	{
		RegisterModelThing(dwPartIndex, pItemData->GetSubModelThing());
	}
	else
	{
		RegisterModelThing(dwPartIndex, pItemData->GetModelThing());
	}

	for (DWORD i = 0; i < pItemData->GetLODModelThingCount(); ++i)
	{
		CGraphicThing * pThing;

		if (!pItemData->GetLODModelThingPointer(i, &pThing))
			continue;

		RegisterLODThing(dwPartIndex, pThing);
	}

	SetModelInstance(dwPartIndex, dwPartIndex, 0);
	AttachModelInstance(dwParentPartIndex, szBoneName, dwPartIndex);

	if (USE_WEAPON_SPECULAR)
	{
		SMaterialData kMaterialData;
		kMaterialData.pImage = nullptr;
		kMaterialData.isSpecularEnable = TRUE;
		kMaterialData.fSpecularPower = pItemData->GetSpecularPowerf();
		kMaterialData.bSphereMapIndex = 1;
		SetMaterialData(dwPartIndex, nullptr, kMaterialData);
	}

	if (pItemData->GetType() != CItemData::ITEM_TYPE_COSTUME)
	{
		if (__IsWeaponTrace(pItemData->GetWeaponType()))
		{
			CWeaponTrace * pWeaponTrace = CWeaponTrace::New();
			pWeaponTrace->SetWeaponInstance(this, dwPartIndex, szBoneName);
			m_WeaponTraceVector.push_back(pWeaponTrace);
		}
	}
	else
	{
		DWORD typeDec = pItemData->GetValue(3);
		if (__IsWeaponTrace(typeDec))
		{
			CWeaponTrace * pWeaponTrace = CWeaponTrace::New();
			pWeaponTrace->SetWeaponInstance(this, dwPartIndex, szBoneName);
			m_WeaponTraceVector.push_back(pWeaponTrace);
		}
	}
}

void  CActorInstance::DettachEffect(DWORD dwEID)
{
	std::list<TAttachingEffect>::iterator i = m_AttachingEffectList.begin();

	while (i != m_AttachingEffectList.end())
	{
		TAttachingEffect & rkAttEft = (*i);

		if (rkAttEft.dwEffectIndex == dwEID)
		{
			i = m_AttachingEffectList.erase(i);
			CEffectManager::Instance().DestroyEffectInstance(dwEID);
		}
		else
		{
			++i;
		}
	}
}

DWORD CActorInstance::AttachEffectByName(DWORD dwParentPartIndex, const char *c_pszBoneName, const char *c_pszEffectName)
{
	std::string str;
	DWORD dwCRC;
	StringPath(c_pszEffectName, str);
	dwCRC = GetCaseCRC32(str.c_str(), str.length());

	return AttachEffectByID(dwParentPartIndex, c_pszBoneName, dwCRC);
}

DWORD CActorInstance::AttachEffectByID(DWORD dwParentPartIndex, const char *c_pszBoneName, DWORD dwEffectID, const D3DXVECTOR3 *c_pv3Position
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	, DWORD *dwSkillColor
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	, float fParticleScale, const D3DXVECTOR3 *c_pv3MeshScale
#endif
)
{
	TAttachingEffect ae;
	ae.iLifeType = EFFECT_LIFE_INFINITE;
	ae.dwEndTime = 0;
	ae.dwModelIndex = dwParentPartIndex;
	ae.dwEffectIndex = CEffectManager::Instance().GetEmptyIndex();
	ae.isAttaching = TRUE;

	if (c_pv3Position)
	{
		D3DXMatrixTranslation(&ae.matTranslation, c_pv3Position->x, c_pv3Position->y, c_pv3Position->z);
	}
	else
	{
		D3DXMatrixIdentity(&ae.matTranslation);
	}

	CEffectManager& rkEftMgr=CEffectManager::Instance();

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	if (c_pv3MeshScale)
		rkEftMgr.CreateEffectInstanceWithScale(ae.dwEffectIndex, dwEffectID
			, fParticleScale, c_pv3MeshScale
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, dwSkillColor
#endif
		);
	else
		rkEftMgr.CreateEffectInstanceWithScale(ae.dwEffectIndex, dwEffectID
			, fParticleScale, NULL
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, dwSkillColor
#endif
		);
#else
	rkEftMgr.CreateEffectInstance(ae.dwEffectIndex, dwEffectID
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, dwSkillColor
#endif
	);
#endif

	if (c_pszBoneName)
	{
		int iBoneIndex;
		
		if (!FindBoneIndex(dwParentPartIndex,c_pszBoneName, &iBoneIndex))
		{
			ae.iBoneIndex = -1;
		}
		else
		{
			ae.iBoneIndex = iBoneIndex;
		}
	}
	else
	{
		ae.iBoneIndex = -1;
	}

	m_AttachingEffectList.push_back(ae);

	return ae.dwEffectIndex;
}

void CActorInstance::RefreshActorInstance()
{
	if (!m_pkCurRaceData)
	{
		TraceError("void CActorInstance::RefreshActorInstance() - m_pkCurRaceData=NULL");
		return;
	}

	// This is Temporary place before making the weapon detection system
	// Setup Collison Detection Data
	m_BodyPointInstanceList.clear();
	//m_AttackingPointInstanceList.clear();
	m_DefendingPointInstanceList.clear();

	// Base
	for (DWORD i = 0; i < m_pkCurRaceData->GetAttachingDataCount(); ++i)
	{
		const NRaceData::TAttachingData * c_pAttachingData;

		if (!m_pkCurRaceData->GetAttachingDataPointer(i, &c_pAttachingData))
			continue;

		switch (c_pAttachingData->dwType)
		{
			case NRaceData::ATTACHING_DATA_TYPE_COLLISION_DATA:
			{
				const NRaceData::TCollisionData * c_pCollisionData = c_pAttachingData->pCollisionData;

				TCollisionPointInstance PointInstance;
				if (NRaceData::COLLISION_TYPE_ATTACKING == c_pCollisionData->iCollisionType)
					continue;

				if (!CreateCollisionInstancePiece(CRaceData::PART_MAIN, c_pAttachingData, &PointInstance))
					continue;

				switch (c_pCollisionData->iCollisionType)
				{
					case NRaceData::COLLISION_TYPE_ATTACKING:
						break;
					case NRaceData::COLLISION_TYPE_DEFENDING:
						m_DefendingPointInstanceList.push_back(PointInstance);
						break;
					case NRaceData::COLLISION_TYPE_BODY:
						m_BodyPointInstanceList.push_back(PointInstance);
						break;
				}
			}
			break;

			case NRaceData::ATTACHING_DATA_TYPE_EFFECT:
				if (c_pAttachingData->isAttaching)
				{
					AttachEffectByName(0, c_pAttachingData->strAttachingBoneName.c_str(), c_pAttachingData->pEffectData->strFileName.c_str());
				}
				else
				{
					AttachEffectByName(0, 0, c_pAttachingData->pEffectData->strFileName.c_str());
				}
				break;

			case NRaceData::ATTACHING_DATA_TYPE_OBJECT:
				break;

			default:
				assert(false);
				break;
		}
	}

	for (DWORD j = 0; j < CRaceData::PART_MAX_NUM; ++j)
	{
		if (0 == m_adwPartItemID[j])
			continue;

		CItemData * pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(m_adwPartItemID[j], &pItemData))
			return;

		for (DWORD k = 0; k < pItemData->GetAttachingDataCount(); ++k)
		{
			const NRaceData::TAttachingData * c_pAttachingData;

			if (!pItemData->GetAttachingDataPointer(k, &c_pAttachingData))
				continue;

			switch(c_pAttachingData->dwType)
			{
				case NRaceData::ATTACHING_DATA_TYPE_COLLISION_DATA:
					{
						const NRaceData::TCollisionData * c_pCollisionData = c_pAttachingData->pCollisionData;

						TCollisionPointInstance PointInstance;
						if (NRaceData::COLLISION_TYPE_ATTACKING == c_pCollisionData->iCollisionType)
							continue;
						if (!CreateCollisionInstancePiece(j, c_pAttachingData, &PointInstance))
							continue;

						switch (c_pCollisionData->iCollisionType)
						{
						case NRaceData::COLLISION_TYPE_ATTACKING:
							break;
						case NRaceData::COLLISION_TYPE_DEFENDING:
							m_DefendingPointInstanceList.push_back(PointInstance);
							break;
						case NRaceData::COLLISION_TYPE_BODY:
							m_BodyPointInstanceList.push_back(PointInstance);
							break;
						}
					}
					break;

				case NRaceData::ATTACHING_DATA_TYPE_EFFECT:
					if (!m_bEffectInitialized)
					{
						DWORD dwCRC;
						StringPath(c_pAttachingData->pEffectData->strFileName);
						dwCRC = GetCaseCRC32(c_pAttachingData->pEffectData->strFileName.c_str(),c_pAttachingData->pEffectData->strFileName.length());

						TAttachingEffect ae;
						ae.iLifeType = EFFECT_LIFE_INFINITE;
						ae.dwEndTime = 0;
						ae.dwModelIndex = j;
						ae.dwEffectIndex = CEffectManager::Instance().GetEmptyIndex();
						ae.isAttaching = TRUE;
						CEffectManager::Instance().CreateEffectInstance(ae.dwEffectIndex, dwCRC);

						int iBoneIndex;
						if (!FindBoneIndex(j,c_pAttachingData->strAttachingBoneName.c_str(), &iBoneIndex))
						{
							Tracef("Cannot get Bone Index\n");
							assert(false/*Cannot get Bone Index*/);
						}
						Tracef("Creating %p %d %d\n", this, j,k);

						ae.iBoneIndex = iBoneIndex;

						m_AttachingEffectList.push_back(ae);
					}
					break;

				case NRaceData::ATTACHING_DATA_TYPE_OBJECT:
					break;

				default:
					assert(false/*NOT_IMPLEMENTED*/);
					break;
			}
		}
	}

	m_bEffectInitialized = true;
}

void CActorInstance::SetWeaponTraceTexture(const char * szTextureName)
{
	std::vector<CWeaponTrace*>::iterator it;
	for (it = m_WeaponTraceVector.begin(); it != m_WeaponTraceVector.end(); ++it)
	{
		(*it)->SetTexture(szTextureName);
	}
}

void CActorInstance::UseTextureWeaponTrace()
{
	for_each(
			m_WeaponTraceVector.begin(),
			m_WeaponTraceVector.end(),
			std::mem_fn(&CWeaponTrace::UseTexture)
			);
}

void CActorInstance::UseAlphaWeaponTrace()
{
	for_each(
			m_WeaponTraceVector.begin(),
			m_WeaponTraceVector.end(),
			std::mem_fn(&CWeaponTrace::UseAlpha)
			);
}

void CActorInstance::UpdateAttachingInstances()
{
	CEffectManager& rkEftMgr=CEffectManager::Instance();

	std::list<TAttachingEffect>::iterator it;
	DWORD dwCurrentTime = CTimer::Instance().GetCurrentMillisecond();
	for (it = m_AttachingEffectList.begin(); it!= m_AttachingEffectList.end();)
	{
		if (EFFECT_LIFE_WITH_MOTION == it->iLifeType)
		{
			++it;
			continue;
		}

		if ((EFFECT_LIFE_NORMAL == it->iLifeType && it->dwEndTime < dwCurrentTime) ||
			!rkEftMgr.IsAliveEffect(it->dwEffectIndex))
		{
			rkEftMgr.DestroyEffectInstance(it->dwEffectIndex);
			it = m_AttachingEffectList.erase(it);
		}
		else
		{
			if (it->isAttaching)
			{
				rkEftMgr.SelectEffectInstance(it->dwEffectIndex);

				if (it->iBoneIndex == -1)
				{
					D3DXMATRIX matTransform;
					matTransform = it->matTranslation;
					matTransform *= m_worldMatrix;
					rkEftMgr.SetEffectInstanceGlobalMatrix(matTransform);
				}
				else
				{
					D3DXMATRIX * pBoneMat;
					if (GetBoneMatrix(it->dwModelIndex, it->iBoneIndex, &pBoneMat))
					{
						D3DXMATRIX matTransform;
						matTransform = *pBoneMat;
						matTransform *= it->matTranslation;
						matTransform *= m_worldMatrix;
						rkEftMgr.SetEffectInstanceGlobalMatrix(matTransform);
					}
					else
					{
					}
				}
			}

			++it;
		}
	}
}

#ifdef ENABLE_GRAPHIC_ON_OFF
void CActorInstance::SetActiveAllAttachingEffect()
{
	std::list<TAttachingEffect>::iterator it;
	for (it = m_AttachingEffectList.begin(); it != m_AttachingEffectList.end(); ++it)
	{
		CEffectManager::Instance().ActiveEffectInstance(it->dwEffectIndex);
	}
}

void CActorInstance::SetDeactiveAllAttachingEffect()
{
	std::list<TAttachingEffect>::iterator it;
	for (it = m_AttachingEffectList.begin(); it != m_AttachingEffectList.end(); ++it)
	{
		CEffectManager::Instance().DeactiveEffectInstance(it->dwEffectIndex);
	}
}
#endif

void CActorInstance::ShowAllAttachingEffect()
{
	std::list<TAttachingEffect>::iterator it;
	for(it = m_AttachingEffectList.begin(); it!= m_AttachingEffectList.end();++it)
	{
		CEffectManager::Instance().SelectEffectInstance(it->dwEffectIndex);
		CEffectManager::Instance().ShowEffect();
#ifdef __ENABLE_STEALTH_FIX__
		CEffectManager::Instance().ReleaseAlwaysHidden();
#endif
	}
}

void CActorInstance::HideAllAttachingEffect()
{
	std::list<TAttachingEffect>::iterator it;
	for(it = m_AttachingEffectList.begin(); it!= m_AttachingEffectList.end();++it)
	{
		CEffectManager::Instance().SelectEffectInstance(it->dwEffectIndex);
		CEffectManager::Instance().HideEffect();
#ifdef __ENABLE_STEALTH_FIX__
		CEffectManager::Instance().ApplyAlwaysHidden();
#endif
	}
}

void CActorInstance::__ClearAttachingEffect()
{
	m_bEffectInitialized = false;

	std::list<TAttachingEffect>::iterator it;
	for(it = m_AttachingEffectList.begin(); it!= m_AttachingEffectList.end();++it)
	{
		CEffectManager::Instance().DestroyEffectInstance(it->dwEffectIndex);
	}
	m_AttachingEffectList.clear();
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CActorInstance::AttachAcce(CItemData * pItemData, float fSpecular)
{
	if (!pItemData)
	{
		RegisterModelThing(CRaceData::PART_ACCE, nullptr);
		SetModelInstance(CRaceData::PART_ACCE, CRaceData::PART_ACCE, 0);
		RefreshActorInstance();
		return;
	}

	RegisterModelThing(CRaceData::PART_ACCE, pItemData->GetModelThing());
	SetModelInstance(CRaceData::PART_ACCE, CRaceData::PART_ACCE, 0);
	AttachModelInstance(CRaceData::PART_MAIN, "Bip01 Spine2", CRaceData::PART_ACCE);

	if (fSpecular > 0.0f)
	{
		SMaterialData kMaterialData;
		kMaterialData.pImage = nullptr;
		kMaterialData.isSpecularEnable = TRUE;
		kMaterialData.fSpecularPower = fSpecular;
		kMaterialData.bSphereMapIndex = 1;
		SetMaterialData(CRaceData::PART_ACCE, nullptr, kMaterialData);
	}
}
#endif

#ifdef ENABLE_RENDER_TARGET
void CActorInstance::RenderAllAttachingEffect()
{
	for (const auto& it : m_AttachingEffectList)
		CEffectManager::Instance().RenderOne(it.dwEffectIndex);
}
#endif

#if defined(ENABLE_WIKI_SYSTEM) || defined(INSIDE_RENDER)
void CActorInstance::RenderAllAttachingEffectWiki()
{
	for (const auto& it : m_AttachingEffectList)
		CEffectManager::Instance().RenderOneWiki(it.dwEffectIndex);
}
#endif

void CActorInstance::SetAttachingEffectsFiltered(bool filtered)
{
    for (const auto& attachment : m_AttachingEffectList)
        CEffectManager::Instance().SetOptionFiltered(attachment.dwEffectIndex, filtered);
}
