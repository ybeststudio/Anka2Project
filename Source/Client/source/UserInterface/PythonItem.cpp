#include "StdAfx.h"

#include "../eterlib/GrpMath.h"
#include "../gamelib/ItemManager.h"
#include "../EffectLib/EffectManager.h"

#include "PythonBackground.h"
#include "PythonItem.h"
#include "PythonTextTail.h"

#ifdef ENABLE_GRAPHIC_ON_OFF
	#include "PythonSystem.h"
#endif

#ifdef ENABLE_RENEWAL_BOOK_NAME
	#include "PythonSkill.h"
	#include "PythonNonPlayer.h"
#endif

#ifdef ENABLE_CONFIG_MODULE
	#include "PythonConfig.h"
#endif

#ifdef ENABLE_CLIENT_PERFORMANCE
	#include "PythonNetworkStream.h"
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	#include "PythonPlayer.h"
#endif

const float c_fDropStartHeight = 100.0f;
const float c_fDropTime = 0.5f;

std::string CPythonItem::TGroundItemInstance::ms_astDropSoundFileName[DROPSOUND_NUM];

void CPythonItem::GetInfo(std::string* pstInfo)
{
	char szInfo[256];
	snprintf(szInfo, sizeof(szInfo), "Item: Inst %d, Pool %d", m_GroundItemInstanceMap.size(), m_GroundItemInstancePool.GetCapacity());

	pstInfo->append(szInfo);
}

void CPythonItem::TGroundItemInstance::Clear()
{
	stOwnership = "";
	ThingInstance.Clear();
	CEffectManager::Instance().DestroyEffectInstance(dwEffectInstanceIndex);
}

void CPythonItem::TGroundItemInstance::__PlayDropSound(DWORD eItemType, const D3DXVECTOR3& c_rv3Pos)
{
	if (eItemType>=DROPSOUND_NUM)
		return;

	CSoundManager::Instance().PlaySound3D(c_rv3Pos.x, c_rv3Pos.y, c_rv3Pos.z, ms_astDropSoundFileName[eItemType].c_str());
}

bool CPythonItem::TGroundItemInstance::Update()
{
	if (bAnimEnded)
		return false;
	if (dwEndTime < CTimer::Instance().GetCurrentMillisecond())
	{
		ThingInstance.SetRotationQuaternion(qEnd);

		D3DXQUATERNION qAdjust(-v3Center.x, -v3Center.y, -v3Center.z, 0.0f);
		D3DXQUATERNION qc;
		D3DXQuaternionConjugate(&qc, &qEnd);
		D3DXQuaternionMultiply(&qAdjust,&qAdjust,&qEnd);
		D3DXQuaternionMultiply(&qAdjust,&qc,&qAdjust);

		ThingInstance.SetPosition(v3EndPosition.x+qAdjust.x, 
			v3EndPosition.y+qAdjust.y,
			v3EndPosition.z+qAdjust.z);
		bAnimEnded = true;

		__PlayDropSound(eDropSoundType, v3EndPosition);
	}
	else
	{
		DWORD time = CTimer::Instance().GetCurrentMillisecond() - dwStartTime;
		DWORD etime = dwEndTime - CTimer::Instance().GetCurrentMillisecond();
		float rate = time * 1.0f / (dwEndTime - dwStartTime);

		D3DXVECTOR3 v3NewPosition=v3EndPosition;
		v3NewPosition.z += 100-100*rate*(3*rate-2);

		D3DXQUATERNION q;
		D3DXQuaternionRotationAxis(&q, &v3RotationAxis, etime * 0.03f *(-1+rate*(3*rate-2)));
		D3DXQuaternionMultiply(&q,&qEnd,&q);

		ThingInstance.SetRotationQuaternion(q);
		D3DXQUATERNION qAdjust(-v3Center.x, -v3Center.y, -v3Center.z, 0.0f);
		D3DXQUATERNION qc;
		D3DXQuaternionConjugate(&qc, &q);
		D3DXQuaternionMultiply(&qAdjust,&qAdjust,&q);
		D3DXQuaternionMultiply(&qAdjust,&qc,&qAdjust);

		ThingInstance.SetPosition(v3NewPosition.x+qAdjust.x,
			v3NewPosition.y+qAdjust.y,
			v3NewPosition.z+qAdjust.z);
	}
	ThingInstance.Transform();
	ThingInstance.Deform();
	return !bAnimEnded;
}

void CPythonItem::Update(const POINT& c_rkPtMouse)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.begin();
	for(; itor != m_GroundItemInstanceMap.end(); ++itor)
	{
		itor->second->Update();
	}

	m_dwPickedItemID=__Pick(c_rkPtMouse);
}

void CPythonItem::Render()
{
	CPythonGraphic::Instance().SetDiffuseOperation();
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.begin();
	for (; itor != m_GroundItemInstanceMap.end(); ++itor)
	{
		CGraphicThingInstance & rInstance = itor->second->ThingInstance;
		rInstance.Render();
		rInstance.BlendRender();

#ifdef ENABLE_GRAPHIC_ON_OFF
		TGroundItemInstance * pGroundItemInstance = itor->second;
		if (CPythonSystem::instance().GetDropItemLevel() >= 3)
			pGroundItemInstance->ThingInstance.Hide();
		else
			pGroundItemInstance->ThingInstance.Show();

		CEffectManager & rem = CEffectManager::Instance();
		rem.SelectEffectInstance(pGroundItemInstance->dwEffectInstanceIndex);
		if (CPythonSystem::instance().GetDropItemLevel() > 0)
			rem.HideEffect();
		else
			rem.ShowEffect();
#endif
	}
}

void CPythonItem::SetUseSoundFileName(DWORD eItemType, const std::string& c_rstFileName)
{
	if (eItemType>=USESOUND_NUM)
		return;

	m_astUseSoundFileName[eItemType]=c_rstFileName;
}

void CPythonItem::SetDropSoundFileName(DWORD eItemType, const std::string& c_rstFileName)
{
	if (eItemType>=DROPSOUND_NUM)
		return;

	Tracenf("SetDropSoundFile %d : %s", eItemType, c_rstFileName.c_str());

	SGroundItemInstance::ms_astDropSoundFileName[eItemType]=c_rstFileName;
}

void CPythonItem::PlayUseSound(DWORD dwItemID)
{
	CItemData* pkItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwItemID, &pkItemData))
		return;

	DWORD eItemType=__GetUseSoundType(*pkItemData);
	if (eItemType==USESOUND_NONE)
		return;
	if (eItemType>=USESOUND_NUM)
		return;

	CSoundManager::Instance().PlaySound2D(m_astUseSoundFileName[eItemType].c_str());
}

void CPythonItem::PlayDropSound(DWORD dwItemID)
{
	CItemData* pkItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwItemID, &pkItemData))
		return;

	DWORD eItemType=__GetDropSoundType(*pkItemData);
	if (eItemType>=DROPSOUND_NUM)
		return;

	CSoundManager::Instance().PlaySound2D(SGroundItemInstance::ms_astDropSoundFileName[eItemType].c_str());
}

void CPythonItem::PlayUsePotionSound()
{
	CSoundManager::Instance().PlaySound2D(m_astUseSoundFileName[USESOUND_POTION].c_str());
}

DWORD CPythonItem::__GetDropSoundType(const CItemData& c_rkItemData)
{
	switch (c_rkItemData.GetType())
	{
		case CItemData::ITEM_TYPE_WEAPON:
			switch (c_rkItemData.GetWeaponType())
			{
				case CItemData::WEAPON_BOW:
					return DROPSOUND_BOW;
					break;
				case CItemData::WEAPON_ARROW:
					return DROPSOUND_DEFAULT;
					break;
				default:
					return DROPSOUND_WEAPON;
					break;
			}
			break;
		case CItemData::ITEM_TYPE_ARMOR:
			switch (c_rkItemData.GetSubType())
			{
				case CItemData::ARMOR_NECK:
				case CItemData::ARMOR_EAR:
					return DROPSOUND_ACCESSORY;
					break;
				case CItemData::ARMOR_BODY:
					return DROPSOUND_ARMOR;
				default:
					return DROPSOUND_DEFAULT;
					break;
			}
			break;
		default:
			return DROPSOUND_DEFAULT;
			break;
	}

	return DROPSOUND_DEFAULT;
}


DWORD	CPythonItem::__GetUseSoundType(const CItemData& c_rkItemData)
{
	switch (c_rkItemData.GetType())
	{
		case CItemData::ITEM_TYPE_WEAPON:
			switch (c_rkItemData.GetWeaponType())
			{
				case CItemData::WEAPON_BOW:
					return USESOUND_BOW;
					break;
				case CItemData::WEAPON_ARROW:
					return USESOUND_DEFAULT;
					break;
				default:
					return USESOUND_WEAPON;
					break;
			}
			break;
		case CItemData::ITEM_TYPE_ARMOR:
			switch (c_rkItemData.GetSubType())
			{
				case CItemData::ARMOR_NECK:
				case CItemData::ARMOR_EAR:
					return USESOUND_ACCESSORY;
					break;
				case CItemData::ARMOR_BODY:
					return USESOUND_ARMOR;
				default:
					return USESOUND_DEFAULT;
					break;
			}
			break;
		case CItemData::ITEM_TYPE_USE:
			switch (c_rkItemData.GetSubType())
			{
				case CItemData::USE_ABILITY_UP:
					return USESOUND_POTION;
					break;
				case CItemData::USE_POTION:
					return USESOUND_NONE;
					break;
				case CItemData::USE_TALISMAN:
					return USESOUND_PORTAL;
					break;
				default:
					return USESOUND_DEFAULT;
					break;
			}
			break;
		default:
			return USESOUND_DEFAULT;
			break;
	}

	return USESOUND_DEFAULT;
}

#ifdef ENABLE_RENEWAL_BOOK_NAME
void CPythonItem::CreateItem(DWORD dwVirtualID, DWORD dwVirtualNumber, float x, float y, float z, long lSocket0, bool bDrop)
#else
void CPythonItem::CreateItem(DWORD dwVirtualID, DWORD dwVirtualNumber, float x, float y, float z, bool bDrop)
#endif
{
	CItemData * pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwVirtualNumber, &pItemData))
		return;

	CGraphicThing* pItemModel = pItemData->GetDropModelThing();

	TGroundItemInstance *	pGroundItemInstance = m_GroundItemInstancePool.Alloc();	
	pGroundItemInstance->dwVirtualNumber = dwVirtualNumber;

	bool bStabGround = false;

	if (bDrop)
	{
		z = CPythonBackground::Instance().GetHeight(x, y) + 10.0f;

		if (pItemData->GetType() == CItemData::ITEM_TYPE_WEAPON && (pItemData->GetWeaponType() == CItemData::WEAPON_SWORD || pItemData->GetWeaponType() == CItemData::WEAPON_ARROW))
			bStabGround = true;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		if (pItemData->GetType() == CItemData::ITEM_TYPE_COSTUME && pItemData->GetSubType() == CItemData::COSTUME_WEAPON || pItemData->GetType() == CItemData::ITEM_TYPE_WEAPON && (pItemData->GetWeaponType() == CItemData::WEAPON_SWORD || pItemData->GetWeaponType() == CItemData::WEAPON_ARROW))
			bStabGround = true;
#endif

		bStabGround = false;
		pGroundItemInstance->bAnimEnded = false;
	}
	else
	{
		pGroundItemInstance->bAnimEnded = true;
	}

	{
#ifdef ENABLE_FALLING_EFFECT_SYSTEM
        DWORD dwEffectIndex = __AttachEffect(dwVirtualNumber, pItemData->GetType(), pItemData->GetSubType());
        if(dwEffectIndex < DROP_ITEM_EFFECT_NUM && dwEffectIndex >= 0)
        {
            dwEffectIndex = m_dwDropItemEffectID[dwEffectIndex];
            if(dwEffectIndex != 0)
            {
                CEffectManager & rem =CEffectManager::Instance();
                pGroundItemInstance->dwEffectInstanceIndex =
                rem.CreateEffect(dwEffectIndex, D3DXVECTOR3(x, -y, z), D3DXVECTOR3(0,0,0));      
            }
        }
#else
		CEffectManager & rem =CEffectManager::Instance();
		pGroundItemInstance->dwEffectInstanceIndex = 
		rem.CreateEffect(m_dwDropItemEffectID, D3DXVECTOR3(x, -y, z), D3DXVECTOR3(0,0,0));
#endif

		pGroundItemInstance->eDropSoundType=__GetDropSoundType(*pItemData);
	}


	D3DXVECTOR3 normal;
	if (!CPythonBackground::Instance().GetNormal(int(x),int(y),&normal))
		normal = D3DXVECTOR3(0.0f,0.0f,1.0f);

	pGroundItemInstance->ThingInstance.Clear();
	pGroundItemInstance->ThingInstance.ReserveModelThing(1);
	pGroundItemInstance->ThingInstance.ReserveModelInstance(1);
	pGroundItemInstance->ThingInstance.RegisterModelThing(0, pItemModel);
	pGroundItemInstance->ThingInstance.SetModelInstance(0, 0, 0);
	if (bDrop)
	{
		pGroundItemInstance->v3EndPosition = D3DXVECTOR3(x,-y,z);
		pGroundItemInstance->ThingInstance.SetPosition(0,0,0);
	}
	else
		pGroundItemInstance->ThingInstance.SetPosition(x, -y, z);

	pGroundItemInstance->ThingInstance.Update();
	pGroundItemInstance->ThingInstance.Transform();
	pGroundItemInstance->ThingInstance.Deform();

	if (bDrop)
	{
		D3DXVECTOR3 vMin, vMax;
		pGroundItemInstance->ThingInstance.GetBoundBox(&vMin,&vMax);
		pGroundItemInstance->v3Center = (vMin + vMax) * 0.5f;

		std::pair<float,int> f[3] = 
			{
				std::make_pair(vMax.x - vMin.x,0),
				std::make_pair(vMax.y - vMin.y,1),
				std::make_pair(vMax.z - vMin.z,2)
			};

		std::sort(f,f+3);

		D3DXVECTOR3 rEnd;

		if (bStabGround)
		{
			if (f[2].second == 0)
			{
				rEnd.y = 90.0f + frandom(-15.0f, 15.0f);
				rEnd.x = frandom(0.0f, 360.0f);
				rEnd.z = frandom(-15.0f, 15.0f);
			}
			else if (f[2].second == 1)
			{
				rEnd.y = frandom(0.0f, 360.0f);
				rEnd.x = frandom(-15.0f, 15.0f);
				rEnd.z = 180.0f + frandom(-15.0f, 15.0f);
			}
			else
			{
				rEnd.y = 180.0f + frandom(-15.0f, 15.0f);
				rEnd.x = 0.0f+frandom(-15.0f, 15.0f);
				rEnd.z = frandom(0.0f, 360.0f);
			}
		}
		else
		{
			if (f[0].second == 0)
			{
				pGroundItemInstance->qEnd =
					RotationArc(
						D3DXVECTOR3(
						((float)(random()%2))*2-1+frandom(-0.1f,0.1f),
						0+frandom(-0.1f,0.1f),
						0+frandom(-0.1f,0.1f)),
						D3DXVECTOR3(0,0,1));
			}
			else if (f[0].second == 1)
			{
				pGroundItemInstance->qEnd =
					RotationArc(
						D3DXVECTOR3(
							0+frandom(-0.1f,0.1f),
							((float)(random()%2))*2-1+frandom(-0.1f,0.1f),
							0+frandom(-0.1f,0.1f)),
						D3DXVECTOR3(0,0,1));
			}
			else
			{
				pGroundItemInstance->qEnd =
					RotationArc(
					D3DXVECTOR3(
					0+frandom(-0.1f,0.1f),
					0+frandom(-0.1f,0.1f),
					((float)(random()%2))*2-1+frandom(-0.1f,0.1f)),
					D3DXVECTOR3(0,0,1));
			}
		}
		float rot = frandom(0, 2*3.1415926535f);
		D3DXQUATERNION q(0,0,cosf(rot),sinf(rot));
		D3DXQuaternionMultiply(&pGroundItemInstance->qEnd, &pGroundItemInstance->qEnd, &q);
		q = RotationArc(D3DXVECTOR3(0,0,1),normal);
		D3DXQuaternionMultiply(&pGroundItemInstance->qEnd, &pGroundItemInstance->qEnd, &q);

		pGroundItemInstance->dwStartTime = CTimer::Instance().GetCurrentMillisecond();
		pGroundItemInstance->dwEndTime = pGroundItemInstance->dwStartTime+300;
		pGroundItemInstance->v3RotationAxis.x = sinf(rot+0);
		pGroundItemInstance->v3RotationAxis.y = cosf(rot+0);
		pGroundItemInstance->v3RotationAxis.z = 0;

		D3DXVECTOR3 v3Adjust = -pGroundItemInstance->v3Center;
		D3DXMATRIX mat;
		D3DXMatrixRotationQuaternion(&mat, &pGroundItemInstance->qEnd);
		D3DXVec3TransformCoord(&v3Adjust,&v3Adjust,&mat);
	}

#ifdef ENABLE_GRAPHIC_ON_OFF
	if (CPythonSystem::instance().GetDropItemLevel() >= 3)
		pGroundItemInstance->ThingInstance.Hide();
	else
		pGroundItemInstance->ThingInstance.Show();
#else
	pGroundItemInstance->ThingInstance.Show();
#endif

	m_GroundItemInstanceMap.insert(TGroundItemInstanceMap::value_type(dwVirtualID, pGroundItemInstance));

	CPythonTextTail& rkTextTail=CPythonTextTail::Instance();

#ifdef ENABLE_RENEWAL_BOOK_NAME
	const char *cItemName;
	std::string sItemName = pItemData->GetName();
	if ((pItemData->GetType() == CItemData::ITEM_TYPE_SKILLBOOK) && (lSocket0 > 0))
	{
		CPythonSkill::SSkillData * c_pSkillData;
		if (CPythonSkill::Instance().GetSkillData(lSocket0, &c_pSkillData))
		{
			sItemName = c_pSkillData->GradeData[0].strName.c_str();
			sItemName += " - ";
		}
		sItemName += pItemData->GetName();
	}

	else if((pItemData->GetType() == CItemData::ITEM_TYPE_POLYMORPH) && (lSocket0 > 0))
	{
		CPythonSkill::SSkillData * c_pSkillData;
		if (CPythonSkill::Instance().GetSkillData(lSocket0, &c_pSkillData))
		{
			const char *c_szName;
			sItemName = " - ";
			CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
			rkNonPlayer.GetName(lSocket0, &c_szName);
			sItemName.append(c_szName);
		}
 
		sItemName += pItemData->GetName();
	}
	cItemName = sItemName.c_str();
	rkTextTail.RegisterItemTextTail(dwVirtualID, cItemName, &pGroundItemInstance->ThingInstance);
#else
	rkTextTail.RegisterItemTextTail(dwVirtualID, pItemData->GetName(), &pGroundItemInstance->ThingInstance);
#endif
}

void CPythonItem::SetOwnership(DWORD dwVID, const char *c_pszName)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVID);

	if (m_GroundItemInstanceMap.end() == itor)
		return;

	TGroundItemInstance * pGroundItemInstance = itor->second;
	pGroundItemInstance->stOwnership.assign(c_pszName);

	CPythonTextTail& rkTextTail = CPythonTextTail::Instance();
	rkTextTail.SetItemTextTailOwner(dwVID, c_pszName);
}

bool CPythonItem::GetOwnership(DWORD dwVID, const char ** c_pszName)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVID);

	if (m_GroundItemInstanceMap.end() == itor)
		return false;

	TGroundItemInstance * pGroundItemInstance = itor->second;
	*c_pszName = pGroundItemInstance->stOwnership.c_str();

	return true;
}

void CPythonItem::DeleteAllItems()
{
	CPythonTextTail& rkTextTail=CPythonTextTail::Instance();

	TGroundItemInstanceMap::iterator i;
	for (i= m_GroundItemInstanceMap.begin(); i!=m_GroundItemInstanceMap.end(); ++i)
	{
		TGroundItemInstance* pGroundItemInst=i->second;
		rkTextTail.DeleteItemTextTail(i->first);
		pGroundItemInst->Clear();
		m_GroundItemInstancePool.Free(pGroundItemInst);
	}
	m_GroundItemInstanceMap.clear();
}

void CPythonItem::DeleteItem(DWORD dwVirtualID)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVirtualID);
	if (m_GroundItemInstanceMap.end() == itor)
		return;

	TGroundItemInstance * pGroundItemInstance = itor->second;
	pGroundItemInstance->Clear();
	m_GroundItemInstancePool.Free(pGroundItemInstance);
	m_GroundItemInstanceMap.erase(itor);

	// Text Tail
	CPythonTextTail::Instance().DeleteItemTextTail(dwVirtualID);
}


bool CPythonItem::GetCloseMoney(const TPixelPosition & c_rPixelPosition, DWORD * pdwItemID, DWORD dwDistance)
{
	DWORD dwCloseItemID = 0;
	DWORD dwCloseItemDistance = 1000 * 1000;

	TGroundItemInstanceMap::iterator i;
	for (i = m_GroundItemInstanceMap.begin(); i != m_GroundItemInstanceMap.end(); ++i)
	{
		TGroundItemInstance * pInstance = i->second;

		if (pInstance->dwVirtualNumber!=VNUM_MONEY)
			continue;

		DWORD dwxDistance = DWORD(c_rPixelPosition.x-pInstance->v3EndPosition.x);
		DWORD dwyDistance = DWORD(c_rPixelPosition.y-(-pInstance->v3EndPosition.y));
		DWORD dwDistance = DWORD(dwxDistance*dwxDistance + dwyDistance*dwyDistance);

		if (dwxDistance*dwxDistance + dwyDistance*dwyDistance < dwCloseItemDistance)
		{
			dwCloseItemID = i->first;
			dwCloseItemDistance = dwDistance;
		}
	}

	if (dwCloseItemDistance>float(dwDistance)*float(dwDistance))
		return false;

	*pdwItemID=dwCloseItemID;

	return true;
}

// Fix for picking up items
// https://metin2.dev/board/topic/24691-fix-pickup-distances-bug/
int DISTANCE_APPROX(int dx, int dy)
{
	int min, max;

	if (dx < 0)
		dx = -dx;

	if (dy < 0)
		dy = -dy;

	if (dx < dy)
	{
		min = dx;
		max = dy;
	}
	else
	{
		min = dy;
		max = dx;
	}

	// coefficients equivalent to ( 123/128 * max ) and ( 51/128 * min )
	return (((max << 8) + (max << 3) - (max << 4) - (max << 1) +
		(min << 7) - (min << 5) + (min << 3) - (min << 1)) >> 8);
}

bool CPythonItem::GetCloseItem(const TPixelPosition& c_rPixelPosition, DWORD* pdwItemID, DWORD dwDistance)
{
	DWORD dwCloseItemID = 0;
	DWORD dwCloseItemDistance = 0;
	int aproMin = 0;

	TGroundItemInstanceMap::iterator i;
	for (i = m_GroundItemInstanceMap.begin(); i != m_GroundItemInstanceMap.end(); ++i)
	{
		TGroundItemInstance* pInstance = i->second;

		int iDist = DISTANCE_APPROX((int)c_rPixelPosition.x - (int)pInstance->v3EndPosition.x, (int)c_rPixelPosition.y - (-(int)pInstance->v3EndPosition.y));
		if (aproMin == 0)
			aproMin = iDist;
		if (aproMin >= iDist) {
			aproMin = iDist;
			dwCloseItemID = i->first;
			dwCloseItemDistance = iDist;
		}
	}

	if (dwCloseItemDistance > 300)
		return false;

	*pdwItemID = dwCloseItemID;

	return true;
}

#if defined(ENABLE_INSTANT_PICKUP) || defined(ENABLE_AUTOMATIC_PICK_UP_SYSTEM)
bool CPythonItem::GetCloseItemVector(const std::string& myName, const TPixelPosition& c_rPixelPosition, std::vector<DWORD>& itemVidList)
{
	DWORD dwCloseItemDistance = 1000 * 1000;
	TGroundItemInstanceMap::iterator i;

	for (i = m_GroundItemInstanceMap.begin(); i != m_GroundItemInstanceMap.end(); ++i)
	{
		TGroundItemInstance * pInstance = i->second;

		DWORD dwxDistance = DWORD(c_rPixelPosition.x - pInstance->v3EndPosition.x);
		DWORD dwyDistance = DWORD(c_rPixelPosition.y - (-pInstance->v3EndPosition.y));
		DWORD dwDistance = DWORD(dwxDistance * dwxDistance + dwyDistance * dwyDistance);

		if (dwDistance < dwCloseItemDistance && (pInstance->stOwnership == "" || pInstance->stOwnership == myName))
		{
			itemVidList.push_back(i->first);
		}
	}
	return true;
}
#endif

BOOL CPythonItem::GetGroundItemPosition(DWORD dwVirtualID, TPixelPosition * pPosition)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVirtualID);
	if (m_GroundItemInstanceMap.end() == itor)
		return FALSE;

	TGroundItemInstance * pInstance = itor->second;

	const D3DXVECTOR3& rkD3DVct3=pInstance->ThingInstance.GetPosition();

	pPosition->x=+rkD3DVct3.x;
	pPosition->y=-rkD3DVct3.y;
	pPosition->z=+rkD3DVct3.z;

	return TRUE;
}

DWORD CPythonItem::__Pick(const POINT& c_rkPtMouse)
{
	float fu, fv, ft;

	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.begin();
	for (; itor != m_GroundItemInstanceMap.end(); ++itor)
	{
		TGroundItemInstance * pInstance = itor->second;

		if (pInstance->ThingInstance.Intersect(&fu, &fv, &ft))
		{
			return itor->first;
		}
	}

	CPythonTextTail& rkTextTailMgr=CPythonTextTail::Instance();
	return rkTextTailMgr.Pick(c_rkPtMouse.x, c_rkPtMouse.y);
}

bool CPythonItem::GetPickedItemID(DWORD* pdwPickedItemID)
{
	if (INVALID_ID==m_dwPickedItemID)
		return false;

	*pdwPickedItemID=m_dwPickedItemID;
	return true;
}

DWORD CPythonItem::GetVirtualNumberOfGroundItem(DWORD dwVID)
{
	TGroundItemInstanceMap::iterator itor = m_GroundItemInstanceMap.find(dwVID);

	if (itor == m_GroundItemInstanceMap.end())
		return 0;
	else
		return itor->second->dwVirtualNumber;
}

void CPythonItem::BuildNoGradeNameData(int iType)
{
	/*
	CMapIterator<std::string, CItemData *> itor = CItemManager::Instance().GetItemNameMapIterator();

	m_NoGradeNameItemData.clear();
	m_NoGradeNameItemData.reserve(1024);

	while (++itor)
	{
		CItemData * pItemData = *itor;
		if (iType == pItemData->GetType())
			m_NoGradeNameItemData.push_back(pItemData);
	}
	*/
}

DWORD CPythonItem::GetNoGradeNameDataCount()
{
	return m_NoGradeNameItemData.size();
}

CItemData * CPythonItem::GetNoGradeNameDataPtr(DWORD dwIndex)
{
	if (dwIndex >= m_NoGradeNameItemData.size())
		return NULL;

	return m_NoGradeNameItemData[dwIndex];
}

void CPythonItem::Destroy()
{
	DeleteAllItems();
	m_GroundItemInstancePool.Clear();
}

#ifdef ENABLE_FALLING_EFFECT_SYSTEM
void CPythonItem::Create()
{
    //Default
    __RegisterEffect(DROP_ITEM_EFFECT_NORMAL, "d:/ymir work/effect/etc/dropitem/dropitem.mse");
   
    //Rare
    __RegisterEffect(DROP_ITEM_EFFECT_RARE, "d:/ymir work/effect/etc/masic_camp/masic_camp_loop.mse");
   
    //Epic
    __RegisterEffect(DROP_ITEM_EFFECT_EPIC, "d:/ymir work/effect/etc/masic_camp/masic_camp_vanish.mse");
   
    //Legenary
    __RegisterEffect(DROP_ITEM_EFFECT_LEGENARY, "d:/ymir work/effect/etc/forked_load/victory.mse");
}
#else
void CPythonItem::Create()
{
	CEffectManager::Instance().RegisterEffect2("d:/ymir work/effect/etc/dropitem/dropitem.mse", &m_dwDropItemEffectID);
}
#endif

CPythonItem::CPythonItem()
{
	m_GroundItemInstancePool.SetName("CDynamicPool<TGroundItemInstance>");	
	m_dwPickedItemID = INVALID_ID;
#ifdef ENABLE_FALLING_EFFECT_SYSTEM
	memset(m_dwDropItemEffectID, 0, sizeof(m_dwDropItemEffectID));
#endif
}

CPythonItem::~CPythonItem()
{
	assert(m_GroundItemInstanceMap.empty());
}

void CPythonItem::__RegisterEffect(int iIndex, const char* szFile)
{
    if(iIndex >= DROP_ITEM_EFFECT_NUM || iIndex < 0)
    {
        TraceError("CPythonItem::__RegisterEffect - Invalid index: %d - %s", iIndex, szFile);
        return;
    }
   
    CEffectManager::Instance().RegisterEffect2(szFile, &m_dwDropItemEffectID[iIndex]);
}

#ifdef ENABLE_FALLING_EFFECT_SYSTEM
int CPythonItem::__AttachEffect(DWORD dwVnum, BYTE byType, BYTE bySubType)
{
    // itemin Vnumuna göre efekt
    switch(dwVnum)
    {
        //İtemin Vnumuna Göre Rare Olarak
		// Kılıç
        case 149:
        case 159:
        case 169:
        case 179:
        case 189:
        case 199:
        case 209:
        case 249:
        case 259:
        case 269:
        case 279:
        case 289:
        case 299:
        case 309:
        case 319:
        case 335:
        case 355:
        case 375:
        case 395:
        case 469:
        case 479:
        case 509:
		// Bıçak
        case 1109:
        case 1119:
        case 1129:
        case 1139:
        case 1179:
        case 1189:
        case 1205:
        case 1225:
        case 1349:
        case 1509:
        case 4049:
		// Yay
        case 2139:
        case 2149:
        case 2159:
        case 2169:
        case 2179:
        case 2189:
        case 2199:
        case 2209:
        case 2225:
        case 2245:
        case 2379:
        case 2509:
		// Çift El
        case 3139:
        case 3149:
        case 3159:
        case 3169:
        case 3199:
        case 3219:
        case 3229:
        case 3245:
        case 3265:
        case 3509:
		// Çan
        case 5099:
        case 5109:
        case 5119:
        case 5129:
        case 5169:
        case 5185:
        case 5215:
        case 5339:
        case 5349:
        case 5509:
		// Yelpaze
        case 7139:
        case 7149:
        case 7159:
        case 7169:
        case 7199:
        case 7309:
        case 7325:
        case 7345:
        case 7379:
        case 7509:
            return DROP_ITEM_EFFECT_RARE;
           
        //İtemin Vnumuna Göre Epic Olarak(Daha Fazla Ekleme Yapılabilir)
        // Kask - Savaşçı
        case 12269:
        case 12289:
        case 12699:
        case 12739:
        case 12795:
        // Kask - Ninja
        case 12399:
        case 12409:
        case 12709:
        case 12749:
        case 12815:
        // Kask - Sura
        case 12539:
        case 12549:
        case 12719:
        case 12759:
        case 12835:
        // Kask - Şaman
        case 12679:
        case 12689:
        case 12729:
        case 12769:
        case 12855:
            return DROP_ITEM_EFFECT_EPIC;
    }
   
    // itemin type/subtype'ine göre ekleme
    switch(byType)
    {
        //Epic Olarak istenin efekt type ayrımı ile (        case CItemData::ITEM_TYPE_WEAPON: Vesaire eklenebilir)
        case CItemData::ITEM_TYPE_ARMOR:
        // Savaşçı
        case 11269:
        case 11279:
        case 11289:
        case 11299:
        case 11309:
        case 12019:
        case 12059:
        // Ninja
        case 11469:
        case 11479:
        case 11489:
        case 11499:
        case 11509:
        case 12029:
        case 12069:
        // Sura
        case 11669:
        case 11679:
        case 11689:
        case 11699:
        case 11709:
        case 12039:
        case 12079:
        // Şaman
        case 11869:
        case 11879:
        case 11889:
        case 11899:
        case 11989:
        case 12049:
        case 12089:
            return DROP_ITEM_EFFECT_EPIC;

        //İtem Type Material olan efektler isteğe bağlı
        case CItemData::ITEM_TYPE_MATERIAL:
        {
            switch(bySubType)
            {
                //Epic İtem ayrımı (Subtype Ayrımı olarak)
                case CItemData::MATERIAL_LEATHER:
                    return DROP_ITEM_EFFECT_EPIC;
                   
                //İtem Typesi kolye Küpe Bilezik felan
                case CItemData::MATERIAL_JEWEL:
                    return DROP_ITEM_EFFECT_LEGENARY;
            }
           
            //ITEM_TYPE_MATERIAL dışında kalanlar isteğe bağlı
            return DROP_ITEM_EFFECT_RARE; //boş
        }
    }
   
    return DROP_ITEM_EFFECT_NORMAL; //boş
}
#endif

#ifdef ENABLE_CLIENT_PERFORMANCE
void CPythonItem::SendPythonData(PyObject *obj, const char *funcname)
{
	CPythonNetworkStream::Instance().SendPythonData(obj, funcname);
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
static bool _CanAddTransmutationItem(const CItemData* item)
{
	const bool bType = CPythonPlayer::Instance().GetChangeLookWindowType();
	const BYTE bItemType = item->GetType();
	const BYTE bItemSubType = item->GetSubType();

	switch (static_cast<ETRANSMUTATIONTYPE>(bType))
	{
		case ETRANSMUTATIONTYPE::TRANSMUTATION_TYPE_MOUNT:
			/// Edit here for your mount system
			break;

		case ETRANSMUTATIONTYPE::TRANSMUTATION_TYPE_ITEM:
			if (bItemType == CItemData::EItemType::ITEM_TYPE_WEAPON && bItemSubType != CItemData::EWeaponSubTypes::WEAPON_ARROW)
				return true;

			if (bItemType == CItemData::EItemType::ITEM_TYPE_ARMOR && bItemSubType == CItemData::EArmorSubTypes::ARMOR_BODY)
				return true;

			if (bItemType == CItemData::EItemType::ITEM_TYPE_COSTUME && bItemSubType == CItemData::ECostumeSubTypes::COSTUME_BODY)
				return true;
			break;
	}

	return false;
}

static bool _CheckOtherTransmutationItem(const CItemData* item, const CItemData* other_item)
{
	if (item == other_item)
		return false;

	if (other_item->GetIndex() == item->GetIndex())
		return false;

	if (other_item->GetType() != item->GetType())
		return false;

	if (other_item->GetSubType() != item->GetSubType())
		return false;

	if (other_item->GetAntiFlags() != item->GetAntiFlags())
		return false;

	return true;
}

bool CPythonItem::CanAddChangeLookItem(const CItemData* item, const CItemData* other_item) const
{
	if (item == nullptr)
		return false;

	if (other_item)
		if (_CheckOtherTransmutationItem(item, other_item) == false)
			return false;
	else if (_CanAddTransmutationItem(item) == false)
		return false;

	return true;
}

bool CPythonItem::CanAddChangeLookFreeItem(const DWORD dwVnum) const
{
	return dwVnum == static_cast<DWORD>(ETRANSMUTATIONSETTINGS::TRANSMUTATION_TICKET_1)
		|| dwVnum == static_cast<DWORD>(ETRANSMUTATIONSETTINGS::TRANSMUTATION_TICKET_2);
}

bool CPythonItem::IsChangeLookClearScrollItem(const DWORD dwVnum) const
{
	return dwVnum == static_cast<DWORD>(ETRANSMUTATIONSETTINGS::TRANSMUTATION_CLEAR_SCROLL);
}
#endif
