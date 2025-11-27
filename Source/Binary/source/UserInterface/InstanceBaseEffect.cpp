#include "StdAfx.h"
#include "InstanceBase.h"
#include "PythonTextTail.h"
#include "AbstractApplication.h"
#include "AbstractPlayer.h"
#include "PythonPlayer.h"
#include "PythonSystem.h"
#include "PythonBackground.h"
#include "PythonApplication.h"
#include "PythonCharacterManager.h"

#include "../EffectLib/EffectManager.h"
#include "../EffectLib/ParticleSystemData.h"
#include "../EterLib/Camera.h"

#define BYTE_COLOR_TO_D3DX_COLOR(r, g, b) D3DXCOLOR(float(r)/255.0f, float(g)/255.0f, float(b)/255.0f, 1.0f)

float CInstanceBase::ms_fDustGap;
float CInstanceBase::ms_fHorseDustGap;
DWORD CInstanceBase::ms_adwCRCAffectEffect[CInstanceBase::EFFECT_NUM];
std::string CInstanceBase::ms_astAffectEffectAttachBone[EFFECT_NUM];

D3DXCOLOR g_akD3DXClrTitle[CInstanceBase::TITLE_NUM];
D3DXCOLOR g_akD3DXClrName[CInstanceBase::NAMECOLOR_NUM];
#ifdef ENABLE_TITLE_SYSTEM
D3DXCOLOR g_akD3DXClrTitleSystem[CInstanceBase::TITLE_SYSTEM_NUM];
#endif

std::map<int, std::string> g_TitleNameMap;
#ifdef ENABLE_TITLE_SYSTEM
std::map<int, std::string> g_TitleSystemNameMap;
#endif
std::set<DWORD> g_kSet_dwPVPReadyKey;
std::set<DWORD> g_kSet_dwPVPKey;
std::set<DWORD> g_kSet_dwGVGKey;
std::set<DWORD> g_kSet_dwDUELKey;

bool g_isEmpireNameMode=false;

void  CInstanceBase::SetEmpireNameMode(bool isEnable)
{
	g_isEmpireNameMode=isEnable;

	if (isEnable)
	{
		g_akD3DXClrName[NAMECOLOR_MOB]=g_akD3DXClrName[NAMECOLOR_EMPIRE_MOB];
		g_akD3DXClrName[NAMECOLOR_NPC]=g_akD3DXClrName[NAMECOLOR_EMPIRE_NPC];
		g_akD3DXClrName[NAMECOLOR_PC]=g_akD3DXClrName[NAMECOLOR_NORMAL_PC];

		for (UINT uEmpire=1; uEmpire<EMPIRE_NUM; ++uEmpire)
			g_akD3DXClrName[NAMECOLOR_PC+uEmpire]=g_akD3DXClrName[NAMECOLOR_EMPIRE_PC+uEmpire];
		
	}
	else
	{
		g_akD3DXClrName[NAMECOLOR_MOB]=g_akD3DXClrName[NAMECOLOR_NORMAL_MOB];
		g_akD3DXClrName[NAMECOLOR_NPC]=g_akD3DXClrName[NAMECOLOR_NORMAL_NPC];

		for (UINT uEmpire=0; uEmpire<EMPIRE_NUM; ++uEmpire)
			g_akD3DXClrName[NAMECOLOR_PC+uEmpire]=g_akD3DXClrName[NAMECOLOR_NORMAL_PC];
	}
}

int CInstanceBase::HasAffect(DWORD dwIndex)
{
	if (dwIndex >= AFFECT_NUM)
		return 0;

	return m_adwCRCAffectEffect[dwIndex];
}

const D3DXCOLOR& CInstanceBase::GetIndexedNameColor(UINT eNameColor)
{
	if (eNameColor>=NAMECOLOR_NUM)
	{
		static D3DXCOLOR s_kD3DXClrNameDefault(0xffffffff);
		return s_kD3DXClrNameDefault;
	}

	return g_akD3DXClrName[eNameColor];
}
#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
void CInstanceBase::AddDamageEffect(DWORD damage, BYTE flag, BOOL bSelf, BOOL bTarget, DWORD dwVictimVID, DWORD dwAttackerVID)
{
    if (CPythonSystem::Instance().IsShowDamage())
    {
        SEffectDamage sDamage;
        sDamage.bSelf = bSelf;
        sDamage.bTarget = bTarget;
        sDamage.damage = damage;
        sDamage.flag = flag;
        sDamage.dwVictimVID = dwVictimVID;
        sDamage.dwAttackerVID = dwAttackerVID;
        m_DamageQueue.push(sDamage);
        if (m_DamageQueue.size() > 20)
            m_DamageQueue.pop();
    }
}
#else
void CInstanceBase::AddDamageEffect(DWORD damage,BYTE flag,BOOL bSelf,BOOL bTarget, DWORD dwVictimVID, DWORD dwAttackerVID)
{
	if (CPythonSystem::Instance().IsShowDamage() && !CPythonApplication::Instance().IsMinimizedWnd())
	{
		SEffectDamage sDamage;
		sDamage.bSelf = bSelf;
		sDamage.bTarget = bTarget;
		sDamage.damage = damage;
		sDamage.flag = flag;
		sDamage.dwVictimVID = dwVictimVID;
		sDamage.dwAttackerVID = dwAttackerVID;
		m_DamageQueue.push_back(sDamage);
	}
}
#endif

void CInstanceBase::ProcessDamage()
{
	if(m_DamageQueue.empty())
		return;

	SEffectDamage sDamage = m_DamageQueue.front();

#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
	m_DamageQueue.pop();
#else
	m_DamageQueue.pop_front();
#endif

	DWORD damage = sDamage.damage;
	BYTE flag = sDamage.flag;
	BOOL bSelf = sDamage.bSelf;
	BOOL bTarget = sDamage.bTarget;

	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();	
	float cameraAngle = GetDegreeFromPosition2(pCamera->GetTarget().x,pCamera->GetTarget().y,pCamera->GetEye().x,pCamera->GetEye().y);

	DWORD FONT_WIDTH = 30;

	CEffectManager& rkEftMgr=CEffectManager::Instance();

	D3DXVECTOR3 v3Pos = m_GraphicThingInstance.GetPosition();
	v3Pos.z += float(m_GraphicThingInstance.GetHeight());

	D3DXVECTOR3 v3Rot = D3DXVECTOR3(0.0f, 0.0f, cameraAngle);

	if ( (flag & DAMAGE_DODGE) || (flag & DAMAGE_BLOCK) )
	{
		if(bSelf)
			rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[EFFECT_DAMAGE_MISS],v3Pos,v3Rot);
		else
			rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[EFFECT_DAMAGE_TARGETMISS],v3Pos,v3Rot);
		return;
	}
	else if (flag & DAMAGE_CRITICAL)
	{}

	std::string strDamageType;
	DWORD rdwCRCEft = 0;

	{
		if(bSelf)
		{
			strDamageType = "damage_";
			if(m_bDamageEffectType==0)
				rdwCRCEft = EFFECT_DAMAGE_SELFDAMAGE;
			else
				rdwCRCEft = EFFECT_DAMAGE_SELFDAMAGE2;
			m_bDamageEffectType = !m_bDamageEffectType;
		}
		else if(bTarget == false)
		{
			strDamageType = "nontarget_";
			rdwCRCEft = EFFECT_DAMAGE_NOT_TARGET;
			return;
		}
		else
		{
			strDamageType = (flag & DAMAGE_CRITICAL) ? "target_crit_" : "target_";
			rdwCRCEft = EFFECT_DAMAGE_TARGET;
		}
	}

	DWORD index = 0;
	DWORD num = 0;
	std::vector<std::string> textures;
	BYTE idxMultiplier = 0;
	while(damage>0)
	{
		if(index > 7)
		{
			TraceError("ProcessDamage¹«ÇÑ·çÇÁ °¡´É¼º");
			break;
		}

		num = damage%10;
		damage /= 10;
		char numBuf[MAX_PATH];
		sprintf(numBuf,"%d.dds",num);
		textures.push_back("d:/ymir work/effect/affect/damagevalue/"+strDamageType+numBuf);

		rkEftMgr.SetEffectTextures(ms_adwCRCAffectEffect[rdwCRCEft],textures);

		D3DXMATRIX matrix,matTrans;
		D3DXMatrixIdentity(&matrix);
		matrix._41 = v3Pos.x;
		matrix._42 = v3Pos.y;
		matrix._43 = v3Pos.z;
		D3DXMatrixTranslation(&matrix,v3Pos.x,v3Pos.y,v3Pos.z);
		D3DXMatrixMultiply(&matrix,&pCamera->GetInverseViewMatrix(),&matrix);
		D3DXMatrixTranslation(&matTrans, FONT_WIDTH * idxMultiplier, 0, 0);
		matTrans._41 = -matTrans._41;
		matrix = matTrans*matrix;
		D3DXMatrixMultiply(&matrix,&pCamera->GetViewMatrix(),&matrix);

		rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[rdwCRCEft],D3DXVECTOR3(matrix._41,matrix._42,matrix._43) ,v3Rot);

		textures.clear();

		index++;
		idxMultiplier++;
		if (damage > 0 && (index % 3) == 0)
		{
			textures.push_back("d:/ymir work/effect/affect/damagevalue/" + strDamageType + "dot.dds");
			rkEftMgr.SetEffectTextures(ms_adwCRCAffectEffect[rdwCRCEft], textures);
			D3DXMATRIX matrix, matTrans;
			D3DXMatrixIdentity(&matrix);
			matrix._41 = v3Pos.x;
			matrix._42 = v3Pos.y;
			matrix._43 = v3Pos.z;
			D3DXMatrixTranslation(&matrix, v3Pos.x, v3Pos.y, v3Pos.z);
			D3DXMatrixMultiply(&matrix, &pCamera->GetInverseViewMatrix(), &matrix);
			D3DXMatrixTranslation(&matTrans, FONT_WIDTH * idxMultiplier, 0, 0);
			matTrans._41 = -matTrans._41;
			matrix = matTrans * matrix;
			D3DXMatrixMultiply(&matrix, &pCamera->GetViewMatrix(), &matrix);
			rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[rdwCRCEft], D3DXVECTOR3(matrix._41, matrix._42, matrix._43), v3Rot);
			textures.clear();
			idxMultiplier++;
		}
	}
}

void CInstanceBase::ProcessRemoveOldDamage()
{
	if (m_DamageQueue.empty())
		return;

#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
	// std::queue için remove_if kullanýlamaz, manuel temizleme yapýyoruz
	CommandDamageQueue tempQueue;
	while (!m_DamageQueue.empty())
	{
		SEffectDamage sed = m_DamageQueue.front();
		m_DamageQueue.pop();
		
		// Eski damage'leri filtrele
		if (!((CPythonCharacterManager::instance().IsDeadVID(sed.dwAttackerVID) || CPythonPlayer::Instance().NEW_GetMainActorPtr()->IsDead()) && (sed.flag != DAMAGE_POISON) && sed.bSelf))
		{
			tempQueue.push(sed);
		}
	}
	m_DamageQueue = tempQueue;
#else
	m_DamageQueue.remove_if([](CInstanceBase::SEffectDamage sed){ return ((CPythonCharacterManager::instance().IsDeadVID(sed.dwAttackerVID) || CPythonPlayer::Instance().NEW_GetMainActorPtr()->IsDead()) && (sed.flag != DAMAGE_POISON) && sed.bSelf); });
#endif
}

void CInstanceBase::AttachSpecialEffect(DWORD effect
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	,float fScale
#endif
)
{
	__AttachEffect(effect
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		, fScale
#endif
	);
}

void CInstanceBase::LevelUp()
{
	__AttachEffect(EFFECT_LEVELUP);
}

void CInstanceBase::SkillUp()
{
	__AttachEffect(EFFECT_SKILLUP);
}

void CInstanceBase::CreateSpecialEffect(DWORD iEffectIndex
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	,float fScale
#endif
)
{
	const D3DXMATRIX & c_rmatGlobal = m_GraphicThingInstance.GetTransform();

	DWORD dwEffectIndex = CEffectManager::Instance().GetEmptyIndex();
	DWORD dwEffectCRC = ms_adwCRCAffectEffect[iEffectIndex];

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	CEffectManager::Instance().CreateEffectInstanceWithScale(dwEffectIndex, dwEffectCRC, fScale);
#else
	CEffectManager::Instance().CreateEffectInstance(dwEffectIndex, dwEffectCRC);
#endif

	CEffectManager::Instance().SelectEffectInstance(dwEffectIndex);
	CEffectManager::Instance().SetEffectInstanceGlobalMatrix(c_rmatGlobal);
}

void CInstanceBase::__EffectContainer_Destroy()
{
	SEffectContainer::Dict& rkDctEftID=__EffectContainer_GetDict();

	SEffectContainer::Dict::iterator i;
	for (i=rkDctEftID.begin(); i!=rkDctEftID.end(); ++i)
		__DetachEffect(i->second);

	rkDctEftID.clear();
}

void CInstanceBase::__EffectContainer_Initialize()
{
	SEffectContainer::Dict& rkDctEftID=__EffectContainer_GetDict();
	rkDctEftID.clear();	
}

CInstanceBase::SEffectContainer::Dict& CInstanceBase::__EffectContainer_GetDict()
{
	return m_kEffectContainer.m_kDct_dwEftID;
}

DWORD CInstanceBase::__EffectContainer_AttachEffect(DWORD dwEftKey)
{
	SEffectContainer::Dict& rkDctEftID=__EffectContainer_GetDict();
	SEffectContainer::Dict::iterator f=rkDctEftID.find(dwEftKey);

	if (rkDctEftID.end()!=f)
		return 0;

	DWORD dwEftID = __AttachEffect(dwEftKey
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		, m_GraphicThingInstance.GetScale().x
#endif
	);

	rkDctEftID.insert(SEffectContainer::Dict::value_type(dwEftKey, dwEftID));
	return dwEftID;
}

void CInstanceBase::__EffectContainer_DetachEffect(DWORD dwEftKey)
{
	SEffectContainer::Dict& rkDctEftID=__EffectContainer_GetDict();
	SEffectContainer::Dict::iterator f=rkDctEftID.find(dwEftKey);
	if (rkDctEftID.end()==f)
		return;

	__DetachEffect(f->second);

	rkDctEftID.erase(f);
}

#ifdef ENABLE_BOSS_EFFECT_OVER_HEAD
void CInstanceBase::__AttachBossEffect()
{
	if (!__IsExistMainInstance())
		return;

	CInstanceBase* pkInstMain = __GetMainInstancePtr();

	if (IsWarp())
		return;
	if (IsObject())
		return;
	if (IsFlag())
		return;
	if (IsResource())
		return;

	__EffectContainer_AttachEffect(EFFECT_BOSS);
}
#endif

void CInstanceBase::__AttachEmpireEffect(DWORD eEmpire)
{
	if (!__IsExistMainInstance())
		return;

	CInstanceBase* pkInstMain=__GetMainInstancePtr();

	if (IsWarp())
		return;
	if (IsObject())
		return;
	if (IsFlag())
		return;
	if (IsResource())
		return;
	if (IsNPC())
		return;
	if (IsPet())
		return;
#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (IsGrowthPet())
		return;
#endif
	if (IsMount())
		return;

	if (pkInstMain->IsGameMaster())
	{
	}
	else
	{
		if (pkInstMain->IsSameEmpire(*this))
			return;

		if (IsAffect(AFFECT_EUNHYEONG))
			return;
	}

	if (IsGameMaster())
		return;

	__EffectContainer_AttachEffect(EFFECT_EMPIRE+eEmpire);
}

void CInstanceBase::__AttachSelectEffect()
{
	__EffectContainer_AttachEffect(EFFECT_SELECT);
}

void CInstanceBase::__DetachSelectEffect()
{
	__EffectContainer_DetachEffect(EFFECT_SELECT);
}

void CInstanceBase::__AttachTargetEffect()
{
	__EffectContainer_AttachEffect(EFFECT_TARGET);
}

void CInstanceBase::__DetachTargetEffect()
{
	__EffectContainer_DetachEffect(EFFECT_TARGET);
}


void CInstanceBase::__StoneSmoke_Inialize()
{
	m_kStoneSmoke.m_dwEftID=0;
}

void CInstanceBase::__StoneSmoke_Destroy()
{
	if (!m_kStoneSmoke.m_dwEftID)
		return;

	__DetachEffect(m_kStoneSmoke.m_dwEftID);
	m_kStoneSmoke.m_dwEftID=0;
}

void CInstanceBase::__StoneSmoke_Create(DWORD eSmoke)
{
	m_kStoneSmoke.m_dwEftID=m_GraphicThingInstance.AttachSmokeEffect(eSmoke);
}

void CInstanceBase::SetAlpha(float fAlpha)
{
	__SetBlendRenderingMode();
	__SetAlphaValue(fAlpha);
}

bool CInstanceBase::UpdateDeleting()
{
	Update();
	Transform();

	IAbstractApplication& rApp=IAbstractApplication::GetSingleton();

	float fAlpha = __GetAlphaValue() - (rApp.GetGlobalElapsedTime() * 1.5f);
	__SetAlphaValue(fAlpha);

	if (fAlpha < 0.0f)
		return false;

	return true;
}

void CInstanceBase::DeleteBlendOut()
{
	__SetBlendRenderingMode();
	__SetAlphaValue(1.0f);
	DetachTextTail();

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.NotifyDeletingCharacterInstance(GetVirtualID());
}

void CInstanceBase::ClearPVPKeySystem()
{
	g_kSet_dwPVPReadyKey.clear();
	g_kSet_dwPVPKey.clear();
	g_kSet_dwGVGKey.clear();
	g_kSet_dwDUELKey.clear();
}

void CInstanceBase::InsertPVPKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	g_kSet_dwPVPKey.insert(dwPVPKey);
}

void CInstanceBase::InsertPVPReadyKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPReadyKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	g_kSet_dwPVPKey.insert(dwPVPReadyKey);
}

void CInstanceBase::RemovePVPKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	g_kSet_dwPVPKey.erase(dwPVPKey);
}

void CInstanceBase::InsertGVGKey(DWORD dwSrcGuildVID, DWORD dwDstGuildVID)
{
	DWORD dwGVGKey = __GetPVPKey(dwSrcGuildVID, dwDstGuildVID);
	g_kSet_dwGVGKey.insert(dwGVGKey);
}

void CInstanceBase::RemoveGVGKey(DWORD dwSrcGuildVID, DWORD dwDstGuildVID)
{
	DWORD dwGVGKey = __GetPVPKey(dwSrcGuildVID, dwDstGuildVID);
	g_kSet_dwGVGKey.erase(dwGVGKey);
}

void CInstanceBase::InsertDUELKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	g_kSet_dwDUELKey.insert(dwPVPKey);
}

DWORD CInstanceBase::__GetPVPKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	if (dwVIDSrc>dwVIDDst)
		std::swap(dwVIDSrc, dwVIDDst);

	DWORD awSrc[2];
	awSrc[0]=dwVIDSrc;
	awSrc[1]=dwVIDDst;

	const BYTE * s = (const BYTE *) awSrc;
	const BYTE * end = s + sizeof(awSrc);
	unsigned long h = 0;

	while (s < end)
	{
		h *= 16777619;
		h ^= (BYTE) *(BYTE *) (s++);
	}

	return h;
}

bool CInstanceBase::__FindPVPKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	if (g_kSet_dwPVPKey.end()==g_kSet_dwPVPKey.find(dwPVPKey))
		return false;

	return true;
}

bool CInstanceBase::__FindPVPReadyKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	if (g_kSet_dwPVPReadyKey.end()==g_kSet_dwPVPReadyKey.find(dwPVPKey))
		return false;

	return true;
}

bool CInstanceBase::__FindGVGKey(DWORD dwSrcGuildID, DWORD dwDstGuildID)
{
	DWORD dwGVGKey=__GetPVPKey(dwSrcGuildID, dwDstGuildID);

	if (g_kSet_dwGVGKey.end()==g_kSet_dwGVGKey.find(dwGVGKey))
		return false;

	return true;
}

bool CInstanceBase::__FindDUELKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwDUELKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	if (g_kSet_dwDUELKey.end()==g_kSet_dwDUELKey.find(dwDUELKey))
		return false;

	return true;
}

bool CInstanceBase::IsPVPInstance(CInstanceBase& rkInstSel)
{
	DWORD dwVIDSrc=GetVirtualID();
	DWORD dwVIDDst=rkInstSel.GetVirtualID();

	DWORD dwGuildIDSrc=GetGuildID();
	DWORD dwGuildIDDst=rkInstSel.GetGuildID();

	if (GetDuelMode())
		return true;

	return __FindPVPKey(dwVIDSrc, dwVIDDst) || __FindGVGKey(dwGuildIDSrc, dwGuildIDDst);
}

const D3DXCOLOR& CInstanceBase::GetNameColor()
{
	return GetIndexedNameColor(GetNameColorIndex());
}

UINT CInstanceBase::GetNameColorIndex()
{
	if (IsPC())
	{
		if (m_isKiller)
		{
			return NAMECOLOR_PK;
		}

		if (__IsExistMainInstance() && !__IsMainInstance())
		{			
			CInstanceBase* pkInstMain=__GetMainInstancePtr();
			if (!pkInstMain)
			{
				TraceError("CInstanceBase::GetNameColorIndex - MainInstance is NULL");
				return NAMECOLOR_PC;
			}
			DWORD dwVIDMain=pkInstMain->GetVirtualID();
			DWORD dwVIDSelf=GetVirtualID();

			if (pkInstMain->GetDuelMode())
			{
				switch(pkInstMain->GetDuelMode())
				{
				case DUEL_CANNOTATTACK:
					return NAMECOLOR_PC + GetEmpireID();
				case DUEL_START:
					if(__FindDUELKey(dwVIDMain, dwVIDSelf))
						return NAMECOLOR_PVP;
					else
						return NAMECOLOR_PC + GetEmpireID();
				}
			}

			if (pkInstMain->IsSameEmpire(*this))
			{
				if (__FindPVPKey(dwVIDMain, dwVIDSelf))
				{
					return NAMECOLOR_PVP;
				}

				DWORD dwGuildIDMain=pkInstMain->GetGuildID();
				DWORD dwGuildIDSelf=GetGuildID();
				if (__FindGVGKey(dwGuildIDMain, dwGuildIDSelf))
				{
					return NAMECOLOR_PVP;
				}
			}
			else
			{
				return NAMECOLOR_PVP;
			}
		}

		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		if (rPlayer.IsPartyMemberByVID(GetVirtualID()))
			return NAMECOLOR_PARTY;

		return NAMECOLOR_PC + GetEmpireID();

	}
	else if (IsNPC() || IsPet()
#ifdef ENABLE_GROWTH_PET_SYSTEM
		|| IsGrowthPet()
#endif
		)
	{
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		if (IsShop())
			return NAMECOLOR_SHOP;
#endif
		return NAMECOLOR_NPC;
	}
	else if (IsEnemy())
	{
		return NAMECOLOR_MOB;
	}
	else if (IsPoly())
	{
		return NAMECOLOR_MOB;
	}
#ifdef ENABLE_METIN_STONES_MINIMAP
	else if (IsStone())
	{
		return NAMECOLOR_METIN;
	}
#endif

	return D3DXCOLOR(0xffffffff);
}

const D3DXCOLOR& CInstanceBase::GetTitleColor()
{
	UINT uGrade = GetAlignmentGrade();
	if ( uGrade >= TITLE_NUM)
	{
		static D3DXCOLOR s_kD3DXClrTitleDefault(0xffffffff);
		return s_kD3DXClrTitleDefault;
	}

	return g_akD3DXClrTitle[uGrade];
}

void CInstanceBase::AttachTextTail()
{
	if (m_isTextTail)
	{
		TraceError("CInstanceBase::AttachTextTail - VID [%d] ALREADY EXIST", GetVirtualID());
		return;
	}

	m_isTextTail = true;

	DWORD dwVID = GetVirtualID();
	float fTextTailHeight = GetBaseHeight() + 10.0f;
	static D3DXCOLOR s_kD3DXClrTextTail = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

#ifdef ENABLE_STONE_SCALE_OPTION
	if (IsStone())
	{
		const float fStoneScale = CPythonSystem::Instance().GetStoneScale() * 100;

		if (CPythonSystem::Instance().GetStoneScale() <= 0.20)
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID,
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
				m_dwGuildLeader,
#endif
				dwVID, s_kD3DXClrTextTail, fTextTailHeight + (fStoneScale - 50));
		else if (CPythonSystem::Instance().GetStoneScale() <= 0.45)
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID,
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
				m_dwGuildLeader,
#endif
				dwVID, s_kD3DXClrTextTail, fTextTailHeight + (fStoneScale - 30));
		else if (CPythonSystem::Instance().GetStoneScale() <= 0.64)
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID,
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
				m_dwGuildLeader,
#endif
				dwVID, s_kD3DXClrTextTail, fTextTailHeight + (fStoneScale - 10));
		else if (CPythonSystem::Instance().GetStoneScale() <= 0.83)
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID,
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
				m_dwGuildLeader,
#endif
				dwVID, s_kD3DXClrTextTail, fTextTailHeight + (fStoneScale + 10));
		else if (CPythonSystem::Instance().GetStoneScale() <= 1.0)
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID,
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
				m_dwGuildLeader,
#endif
				dwVID, s_kD3DXClrTextTail, fTextTailHeight + (fStoneScale + 30));
		else
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID,
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
				m_dwGuildLeader,
#endif
				dwVID, s_kD3DXClrTextTail, fTextTailHeight);

		if (m_dwLevel)
			UpdateTextTailLevel(m_dwLevel);

		return;
	}
#endif

	CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID,
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
		m_dwGuildLeader,
#endif
		dwVID, s_kD3DXClrTextTail, fTextTailHeight);

	if (m_dwLevel)
		UpdateTextTailLevel(m_dwLevel);
}

void CInstanceBase::DetachTextTail()
{
	if (!m_isTextTail)
		return;

	m_isTextTail=false;
	CPythonTextTail::Instance().DeleteCharacterTextTail(GetVirtualID());
}

void CInstanceBase::UpdateTextTailLevel(DWORD level)
{
	static D3DXCOLOR s_kPlayerLevelColor = D3DXCOLOR(152.0f/255.0f, 255.0f/255.0f, 51.0f/255.0f, 1.0f);

#ifdef ENABLE_SHOW_MOB_INFO
	static D3DXCOLOR s_kMobLevelColor = D3DXCOLOR(152.0f/255.0f, 255.0f/255.0f, 51.0f/255.0f, 1.0f);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	static D3DXCOLOR s_kPetColor = D3DXCOLOR(255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f);
#endif

	// Fix
	m_dwLevel = level;

	char szText[256];
#ifdef ENABLE_RENEWAL_PREMIUM_SYSTEM
	if (IsPremium())
		sprintf(szText, "Lv %d |cFFFFC125VIP|r", level);
	else
		sprintf(szText, "Lv %d", level);
#else
	sprintf(szText, "Lv %d", level);
#endif

#ifdef ENABLE_SHOW_MOB_INFO
	if (IsPC())
		CPythonTextTail::Instance().AttachLevel(GetVirtualID(), szText, s_kPlayerLevelColor);
	else if (IsEnemy() || IsStone())
		CPythonTextTail::Instance().AttachLevel(GetVirtualID(), szText, s_kMobLevelColor);
#ifdef ENABLE_GROWTH_PET_SYSTEM
	else if (IsGrowthPet())
		CPythonTextTail::Instance().AttachLevel(GetVirtualID(), szText, s_kPetColor);
#endif
	else
		CPythonTextTail::Instance().DetachLevel(GetVirtualID());
#else
#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (IsGrowthPet())
		CPythonTextTail::Instance().AttachLevel(GetVirtualID(), szText, s_kPetColor);
	else
#endif
	CPythonTextTail::Instance().AttachLevel(GetVirtualID(), szText, s_kPlayerLevelColor);
#endif
}

void CInstanceBase::RefreshTextTail()
{
	CPythonTextTail::Instance().SetCharacterTextTailColor(GetVirtualID(), GetNameColor());

	int iAlignmentGrade = GetAlignmentGrade();
	if (TITLE_NONE == iAlignmentGrade)
	{
		CPythonTextTail::Instance().DetachTitle(GetVirtualID());
	}
	else
	{
		std::map<int, std::string>::iterator itor = g_TitleNameMap.find(iAlignmentGrade);
		if (g_TitleNameMap.end() != itor)
		{
			const std::string & c_rstrTitleName = itor->second;
			CPythonTextTail::Instance().AttachTitle(GetVirtualID(), c_rstrTitleName.c_str(), GetTitleColor());
		}
	}

#ifdef ENABLE_TITLE_SYSTEM
	int iTitle = GetTitleID();
	if (TITLE_SYSTEM_NONE == iTitle)
	{
		CPythonTextTail::Instance().DetachTitleSystem(GetVirtualID());
	}
	else
	{
		std::map<int, std::string>::iterator itor = g_TitleSystemNameMap.find(iTitle);
		if (g_TitleSystemNameMap.end() != itor)
		{
			const std::string & c_rstrTitleName = itor->second;
			CPythonTextTail::Instance().AttachTitleSystem(GetVirtualID(), c_rstrTitleName.c_str(), GetTitleSystemColor());
		}
	}
#endif
}

void CInstanceBase::RefreshTextTailTitle()
{
	RefreshTextTail();
}

void CInstanceBase::__ClearAffectFlagContainer()
{
	m_kAffectFlagContainer.Clear();
}

void CInstanceBase::__ClearAffects()
{
	if (IsStone())
	{
		__StoneSmoke_Destroy();
	}
	else
	{
		for (int iAffect=0; iAffect<AFFECT_NUM; ++iAffect)
		{
			__DetachEffect(m_adwCRCAffectEffect[iAffect]);
			m_adwCRCAffectEffect[iAffect]=0;
		}

		__ClearAffectFlagContainer();
	}

	m_GraphicThingInstance.__OnClearAffects();
}

void CInstanceBase::__SetNormalAffectFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer)
{
	for (int i=0; i<CAffectFlagContainer::BIT_SIZE; ++i)
	{
		bool isOldSet=m_kAffectFlagContainer.IsSet(i);
		bool isNewSet=c_rkAffectFlagContainer.IsSet(i);

		if (isOldSet != isNewSet)
		{
			__SetAffect(i, isNewSet);

			if (isNewSet)
				m_GraphicThingInstance.__OnSetAffect(i);
			else
				m_GraphicThingInstance.__OnResetAffect(i);
		}
	}

	m_kAffectFlagContainer.CopyInstance(c_rkAffectFlagContainer);
}

void CInstanceBase::__SetStoneSmokeFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer)
{
	m_kAffectFlagContainer.CopyInstance(c_rkAffectFlagContainer);

	DWORD eSmoke;
	if (m_kAffectFlagContainer.IsSet(STONE_SMOKE8))
		eSmoke=3;
	else if (m_kAffectFlagContainer.IsSet(STONE_SMOKE5)|m_kAffectFlagContainer.IsSet(STONE_SMOKE6)|m_kAffectFlagContainer.IsSet(STONE_SMOKE7))
		eSmoke=2;
	else if (m_kAffectFlagContainer.IsSet(STONE_SMOKE2)|m_kAffectFlagContainer.IsSet(STONE_SMOKE3)|m_kAffectFlagContainer.IsSet(STONE_SMOKE4))
		eSmoke=1;
	else
		eSmoke=0;

	__StoneSmoke_Destroy();
	__StoneSmoke_Create(eSmoke);
}

void CInstanceBase::SetAffectFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer)
{
	if (IsBuilding())
	{
		return;		
	}
	else if (IsStone())
	{
		__SetStoneSmokeFlagContainer(c_rkAffectFlagContainer);
	}
	else
	{
		__SetNormalAffectFlagContainer(c_rkAffectFlagContainer);
	}
}


void CInstanceBase::SCRIPT_SetAffect(UINT eAffect, bool isVisible)
{
	__SetAffect(eAffect, isVisible);
}

void CInstanceBase::__SetReviveInvisibilityAffect(bool isVisible)
{
	if (isVisible)
	{
		if (IsWearingDress())
			return;

		m_GraphicThingInstance.BlendAlphaValue(0.5f, 1.0f);
	}
	else
	{
		m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);	
	}
}

void CInstanceBase::__Assassin_SetEunhyeongAffect(bool isVisible)
{
	if (isVisible)
	{
		if (IsWearingDress())
			return;

		if (__IsMainInstance() || __MainCanSeeHiddenThing())
		{
			m_GraphicThingInstance.BlendAlphaValue(0.5f, 1.0f);
		}
		else
		{
			m_GraphicThingInstance.BlendAlphaValue(0.0f, 1.0f);
#ifdef ENABLE_GRAPHIC_ON_OFF
			m_GraphicThingInstance.SetDeactiveAllAttachingEffect();
#else
			m_GraphicThingInstance.HideAllAttachingEffect();
#endif
		}
	}
	else
	{
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
		if (IsAffect(AFFECT_INVISIBILITY) && __MainCanSeeHiddenThing())
			return;
#endif
		m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);
#ifdef ENABLE_GRAPHIC_ON_OFF
		m_GraphicThingInstance.SetActiveAllAttachingEffect();
#else
		m_GraphicThingInstance.ShowAllAttachingEffect();
#endif
	}
}

void CInstanceBase::__Shaman_SetParalysis(bool isParalysis)
{
	m_GraphicThingInstance.SetParalysis(isParalysis);
}

void CInstanceBase::__Warrior_SetGeomgyeongAffect(bool isVisible)
{
	if (isVisible)
	{
		if (IsWearingDress())
			return;

		if (m_kWarrior.m_dwGeomgyeongEffect)
			__DetachEffect(m_kWarrior.m_dwGeomgyeongEffect);

		m_GraphicThingInstance.SetReachScale(1.5f);
		if (m_GraphicThingInstance.IsTwoHandMode())
			m_kWarrior.m_dwGeomgyeongEffect=__AttachEffect(EFFECT_WEAPON+WEAPON_TWOHAND);
		else
			m_kWarrior.m_dwGeomgyeongEffect=__AttachEffect(EFFECT_WEAPON+WEAPON_ONEHAND);
	}
	else
	{
		m_GraphicThingInstance.SetReachScale(1.0f);

		__DetachEffect(m_kWarrior.m_dwGeomgyeongEffect);
		m_kWarrior.m_dwGeomgyeongEffect=0;
	}
}

void CInstanceBase::__SetAffect(UINT eAffect, bool isVisible)
{
	switch (eAffect)
	{
		case AFFECT_YMIR:
#ifdef ENABLE_RENEWAL_TEAM_AFFECT
		case AFFECT_TEAM_SA:
		case AFFECT_TEAM_GA:
		case AFFECT_TEAM_GM:
		case AFFECT_TEAM_TGM:
#endif
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
			if (IsAffect(AFFECT_INVISIBILITY) && !__MainCanSeeHiddenThing())
				return;
#else
			if (IsAffect(AFFECT_INVISIBILITY))
				return;
#endif
			break;
		case AFFECT_CHEONGEUN:
			m_GraphicThingInstance.SetResistFallen(isVisible);
			break;
		case AFFECT_GEOMGYEONG:
			__Warrior_SetGeomgyeongAffect(isVisible);
			return;
			break;
		case AFFECT_REVIVE_INVISIBILITY:
			__Assassin_SetEunhyeongAffect(isVisible);
			break;
		case AFFECT_EUNHYEONG:
			__Assassin_SetEunhyeongAffect(isVisible);
			break;
		case AFFECT_GYEONGGONG:
		case AFFECT_KWAESOK:
			if (isVisible)
				if (!IsWalking())
					return;
			break;
		case AFFECT_INVISIBILITY:
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
			if (__MainCanSeeHiddenThing())
			{
				if (isVisible)
					m_GraphicThingInstance.BlendAlphaValue(0.5f, 1.0f);
				else
					m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);
				break;
			}
#endif
#ifdef ENABLE_INVISIBLE_MODE_EFFECT_FIX
			if (isVisible)
			{
				m_GraphicThingInstance.HideAllAttachingEffect();
				__EffectContainer_Destroy();
				DetachTextTail();
			}
			else
			{
				m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);
				m_GraphicThingInstance.ShowAllAttachingEffect();
				AttachTextTail();
				RefreshTextTail();
			}
			return;
			break;
#else
			if (isVisible)
			{
				m_GraphicThingInstance.ClearAttachingEffect();
				__EffectContainer_Destroy();
				DetachTextTail();
			}
			else
			{
				m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);
				AttachTextTail();
				RefreshTextTail();
			}
			return;
			break;
#endif
		case AFFECT_STUN:
			m_GraphicThingInstance.SetSleep(isVisible);
			break;
	}

	if (eAffect>=AFFECT_NUM)
	{
		TraceError("CInstanceBase[VID:%d]::SetAffect(eAffect:%d<AFFECT_NUM:%d, isVisible=%d)", GetVirtualID(), eAffect, isVisible);
		return;
	}

	if (isVisible)
	{
		if (!m_adwCRCAffectEffect[eAffect])
		{
			m_adwCRCAffectEffect[eAffect]=__AttachEffect(EFFECT_AFFECT+eAffect);
		}
	}
	else
	{
		if (m_adwCRCAffectEffect[eAffect])
		{
			__DetachEffect(m_adwCRCAffectEffect[eAffect]);
			m_adwCRCAffectEffect[eAffect]=0;
		}
	}
}

bool CInstanceBase::IsPossibleEmoticon()
{
	CEffectManager& rkEftMgr=CEffectManager::Instance();
	for(DWORD eEmoticon = 0; eEmoticon < EMOTICON_NUM; eEmoticon++)
	{
		DWORD effectID = ms_adwCRCAffectEffect[EFFECT_EMOTICON+eEmoticon];
		if( effectID &&	rkEftMgr.IsAliveEffect(effectID) )
			return false;
	}

	if(ELTimer_GetMSec() - m_dwEmoticonTime < 1000)
	{
		// Emoticon spam önleme: 1 saniyeden kýsa sürede emoticon gösterilmesini engelle
		// TraceError kaldýrýldý - gereksiz log mesajý
		return false;
	}

	return true;
}

void CInstanceBase::SetFishEmoticon()
{
	SetEmoticon(EMOTICON_FISH);
}

void CInstanceBase::SetEmoticon(UINT eEmoticon)
{
	if (eEmoticon>=EMOTICON_NUM)
	{
		TraceError("CInstanceBase[VID:%d]::SetEmoticon(eEmoticon:%d<EMOTICON_NUM:%d, isVisible=%d)",
			GetVirtualID(), eEmoticon);
		return;
	}

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if (IsShop())
		return;
#endif

	if (IsPossibleEmoticon())
	{
		D3DXVECTOR3 v3Pos = m_GraphicThingInstance.GetPosition();
		v3Pos.z += float(GetBaseHeight() + m_GraphicThingInstance.GetHeight());

		CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();

		D3DXVECTOR3 v3Dir = (pCamera->GetEye()-v3Pos)*9/10;	
		v3Pos = pCamera->GetEye()-v3Dir;

		v3Pos = D3DXVECTOR3(0, 0, 0);
		v3Pos.z += float(GetBaseHeight() + m_GraphicThingInstance.GetHeight());

		m_GraphicThingInstance.AttachEffectByID(0, nullptr, ms_adwCRCAffectEffect[EFFECT_EMOTICON+eEmoticon],&v3Pos);
		m_dwEmoticonTime = ELTimer_GetMSec();
	}
}

void CInstanceBase::SetDustGap(float fDustGap)
{
	ms_fDustGap=fDustGap;
}

void CInstanceBase::SetHorseDustGap(float fDustGap)
{
	ms_fHorseDustGap=fDustGap;
}

void CInstanceBase::__DetachEffect(DWORD dwEID)
{
	m_GraphicThingInstance.DettachEffect(dwEID);
}

DWORD CInstanceBase::__AttachEffect(UINT eEftType
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	, float fScale
#endif
)
{
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	switch (eEftType)
	{
		case EFFECT_AFFECT + AFFECT_GONGPO:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/sura/effect/fear_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_JUMAGAP:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/sura/effect/jumagap_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_HOSIN:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/shaman/effect/3hosin_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_BOHO:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/shaman/effect/boho_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_KWAESOK:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/shaman/effect/10kwaesok_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_HEUKSIN:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/sura/effect/heuksin_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_MUYEONG:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/sura/effect/muyeong_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_FIRE:
			RegisterEffect(eEftType, "Bip01", "d:/ymir work/effect/hit/blow_flame/flame_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_GICHEON:
			RegisterEffect(eEftType, "Bip01 R Hand", "d:/ymir work/pc/shaman/effect/6gicheon_hand.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_JEUNGRYEOK:
			RegisterEffect(eEftType, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_PABEOP:
			RegisterEffect(eEftType, "Bip01 Head", "d:/ymir work/pc/sura/effect/pabeop_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_CHEONGEUN:
		case EFFECT_AFFECT + AFFECT_FALLEN_CHEONGEUN:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_GWIGEOM:
			RegisterEffect(eEftType, "Bip01 R Finger2", "d:/ymir work/pc/sura/effect/gwigeom_loop.mse", false, GetNameString());
			break;
		case EFFECT_AFFECT + AFFECT_GYEONGGONG:
			RegisterEffect(eEftType, "", "d:/ymir work/pc/assassin/effect/gyeonggong_loop.mse", false, GetNameString());
			break;
		case EFFECT_WEAPON + WEAPON_ONEHAND:
			RegisterEffect(eEftType, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_sword_loop.mse", false, GetNameString());
			break;
		case EFFECT_WEAPON + WEAPON_TWOHAND:
			RegisterEffect(eEftType, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_spear_loop.mse", false, GetNameString());
			break;
		default:
			break;
	}
#endif

#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
	if (IsAffect(AFFECT_INVISIBILITY) && !__MainCanSeeHiddenThing())
		return 0;
#else
	if (IsAffect(AFFECT_INVISIBILITY))
		return 0;
#endif

	if (eEftType >= EFFECT_NUM)
		return 0;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	DWORD *dwSkillColor = m_GraphicThingInstance.GetSkillColorByEffectID(eEftType);
#endif

	if (ms_astAffectEffectAttachBone[eEftType].empty())
	{
#if defined(ENABLE_SKILL_COLOR_SYSTEM) && defined(ENABLE_RENEWAL_PREMIUM_SYSTEM)
		if (IsPremium() || IsGameMaster())
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			return m_GraphicThingInstance.AttachEffectByID(0, nullptr, ms_adwCRCAffectEffect[eEftType], nullptr, dwSkillColor, fScale, (const D3DXVECTOR3*)nullptr);
#else
			return m_GraphicThingInstance.AttachEffectByID(0, nullptr, ms_adwCRCAffectEffect[eEftType], nullptr, dwSkillColor);
#endif
		else
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			return m_GraphicThingInstance.AttachEffectByID(0, nullptr, ms_adwCRCAffectEffect[eEftType], nullptr, nullptr, fScale, (const D3DXVECTOR3*)nullptr);
#else
			return m_GraphicThingInstance.AttachEffectByID(0, nullptr, ms_adwCRCAffectEffect[eEftType]);
#endif
	}
	else
	{
		std::string & rstrBoneName = ms_astAffectEffectAttachBone[eEftType];
		const char *c_szBoneName;

		if (0 == rstrBoneName.compare("PART_WEAPON"))
		{
			if (m_GraphicThingInstance.GetAttachingBoneName(CRaceData::PART_WEAPON, &c_szBoneName))
			{
#if defined(ENABLE_SKILL_COLOR_SYSTEM) && defined(ENABLE_RENEWAL_PREMIUM_SYSTEM)
				if (IsPremium() || IsGameMaster())
					return m_GraphicThingInstance.AttachEffectByID(0, c_szBoneName, ms_adwCRCAffectEffect[eEftType], nullptr, dwSkillColor);
				else
#endif
					return m_GraphicThingInstance.AttachEffectByID(0, c_szBoneName, ms_adwCRCAffectEffect[eEftType]);
			}
		}
		else if (0 == rstrBoneName.compare("PART_WEAPON_LEFT"))
		{
			if (m_GraphicThingInstance.GetAttachingBoneName(CRaceData::PART_WEAPON_LEFT, &c_szBoneName))
			{
#if defined(ENABLE_SKILL_COLOR_SYSTEM) && defined(ENABLE_RENEWAL_PREMIUM_SYSTEM)
				if (IsPremium() || IsGameMaster())
					return m_GraphicThingInstance.AttachEffectByID(0, c_szBoneName, ms_adwCRCAffectEffect[eEftType], nullptr, dwSkillColor);
				else
#endif
					return m_GraphicThingInstance.AttachEffectByID(0, c_szBoneName, ms_adwCRCAffectEffect[eEftType]);
			}
		}
		else
		{
#if defined(ENABLE_SKILL_COLOR_SYSTEM) && defined(ENABLE_RENEWAL_PREMIUM_SYSTEM)
			if (IsPremium() || IsGameMaster())
#ifdef ENABLE_AURA_COSTUME_SYSTEM
				return m_GraphicThingInstance.AttachEffectByID(0, rstrBoneName.c_str(), ms_adwCRCAffectEffect[eEftType], nullptr, dwSkillColor, fScale, (const D3DXVECTOR3*)nullptr);
#else
				return m_GraphicThingInstance.AttachEffectByID(0, rstrBoneName.c_str(), ms_adwCRCAffectEffect[eEftType], nullptr, dwSkillColor);
#endif
			else
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
				return m_GraphicThingInstance.AttachEffectByID(0, rstrBoneName.c_str(), ms_adwCRCAffectEffect[eEftType], nullptr, nullptr, fScale, (const D3DXVECTOR3*)nullptr);
#else
				return m_GraphicThingInstance.AttachEffectByID(0, rstrBoneName.c_str(), ms_adwCRCAffectEffect[eEftType]);
#endif
		}
	}

	return 0;
}

void CInstanceBase::__ComboProcess()
{
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
bool CInstanceBase::RegisterEffect(UINT eEftType, const char *c_szEftAttachBone, const char *c_szEftName, bool isCache, const char *name)
#else
bool CInstanceBase::RegisterEffect(UINT eEftType, const char *c_szEftAttachBone, const char *c_szEftName, bool isCache)
#endif
{
	if (eEftType>=EFFECT_NUM)
		return false;

	ms_astAffectEffectAttachBone[eEftType] = c_szEftAttachBone;

	DWORD& rdwCRCEft = ms_adwCRCAffectEffect[eEftType];

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	if (!CEffectManager::Instance().RegisterEffect2(c_szEftName, &rdwCRCEft, isCache, name))
#else
	if (!CEffectManager::Instance().RegisterEffect2(c_szEftName, &rdwCRCEft, isCache))
#endif
	{
		TraceError("CInstanceBase::RegisterEffect(eEftType=%d, c_szEftAttachBone=%s, c_szEftName=%s, isCache=%d) - Error", eEftType, c_szEftAttachBone, c_szEftName, isCache);
		rdwCRCEft = 0;
		return false;
	}

	return true;
}

void CInstanceBase::RegisterTitleName(int iIndex, const char *c_szTitleName)
{
	g_TitleNameMap.insert(std::make_pair(iIndex, c_szTitleName));
}

D3DXCOLOR __RGBToD3DXColoru(UINT r, UINT g, UINT b)
{
	DWORD dwColor=0xff;dwColor<<=8;
	dwColor|=r;dwColor<<=8;
	dwColor|=g;dwColor<<=8;
	dwColor|=b;

	return D3DXCOLOR(dwColor);
}

bool CInstanceBase::RegisterNameColor(UINT uIndex, UINT r, UINT g, UINT b)
{
	if (uIndex>=NAMECOLOR_NUM)
		return false;

	g_akD3DXClrName[uIndex]=__RGBToD3DXColoru(r, g, b);
	return true;
}

bool CInstanceBase::RegisterTitleColor(UINT uIndex, UINT r, UINT g, UINT b)
{
	if (uIndex>=TITLE_NUM)
		return false;

	g_akD3DXClrTitle[uIndex]=__RGBToD3DXColoru(r, g, b);
	return true;
}

#ifdef ENABLE_TITLE_SYSTEM
void CInstanceBase::RegisterTitleSystemName(int iIndex, const char *c_szTitleName)
{
	g_TitleSystemNameMap.insert(std::make_pair(iIndex, c_szTitleName));
}

bool CInstanceBase::RegisterTitleSystemColor(UINT uIndex, UINT r, UINT g, UINT b)
{
	if (uIndex >= TITLE_SYSTEM_NUM)
		return false;

	g_akD3DXClrTitleSystem[uIndex]=__RGBToD3DXColoru(r, g, b);
	return true;
}

const D3DXCOLOR& CInstanceBase::GetTitleSystemColor()
{
	int iTitle = GetTitleID();
	if (iTitle >= TITLE_SYSTEM_NUM)
	{
		static D3DXCOLOR s_kD3DXClrTitleDefault(0xffffffff);
		return s_kD3DXClrTitleDefault;
	}

	return g_akD3DXClrTitleSystem[iTitle];
}
#endif
