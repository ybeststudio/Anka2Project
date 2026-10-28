#include "StdAfx.h"
#include "InstanceBase.h"
#include "PythonBackground.h"
#include "PythonNonPlayer.h"
#include "PythonPlayer.h"
#include "PythonCharacterManager.h"
#include "AbstractPlayer.h"
#include "AbstractApplication.h"
#include "packet.h"

#include "../eterlib/StateManager.h"
#include "../gamelib/ItemManager.h"
#include "../gamelib/RaceManager.h"

#ifdef ENABLE_GRAPHIC_ON_OFF
	#include "PythonSystem.h"
	#include "PythonTextTail.h"
#endif

#ifdef ENABLE_CONFIG_MODULE
	#include "PythonConfig.h"
#endif

#ifdef ENABLE_HAIR_SPECULAR
	#include <unordered_map>
#endif

BOOL HAIR_COLOR_ENABLE = FALSE;
BOOL USE_ARMOR_SPECULAR = FALSE;	// Original -> FALSE
BOOL RIDE_HORSE_ENABLE = TRUE;
#ifndef ENABLE_SET_NO_CHARACTER_ROTATION
const float c_fDefaultRotationSpeed = 1200.0f;
#else
const float c_fDefaultRotationSpeed = 2400.0f;
#endif
#ifndef ENABLE_SET_NO_MOUNT_ROTATION
const float c_fDefaultHorseRotationSpeed = 1500.0f;
#else
const float c_fDefaultHorseRotationSpeed = 600.0f;
#endif
#ifdef ENABLE_STANDING_MOUNT
#ifndef ENABLE_SET_NO_STANDING_MOUNT_ROTATION
const float c_fDefaultStandingMountRotationSpeed = 1500.0f;
#else
const float c_fDefaultStandingMountRotationSpeed = 600.0f;
#endif
#endif

bool IsWall(unsigned race)
{
	switch (race)
	{
		case 14201:
		case 14202:
		case 14203:
		case 14204:
			return true;
			break;
	}
	return false;
}

CInstanceBase::SHORSE::SHORSE()
{
	__Initialize();
}

CInstanceBase::SHORSE::~SHORSE()
{
	assert(m_pkActor==nullptr);
}

void CInstanceBase::SHORSE::__Initialize()
{
	m_isMounting=false;
	m_pkActor=NULL;
}

void CInstanceBase::SHORSE::SetAttackSpeed(UINT uAtkSpd)
{
	if (!IsMounting())
		return;

	CActorInstance& rkActor=GetActorRef();
	rkActor.SetAttackSpeed(uAtkSpd/100.0f);
}

void CInstanceBase::SHORSE::SetMoveSpeed(UINT uMovSpd)
{
	if (!IsMounting())
		return;

	CActorInstance& rkActor=GetActorRef();
	rkActor.SetMoveSpeed(uMovSpd/100.0f);
}

void CInstanceBase::SHORSE::Create(const TPixelPosition& c_rkPPos, UINT eRace, UINT eHitEffect)
{
	assert(NULL==m_pkActor && "CInstanceBase::SHORSE::Create - ALREADY MOUNT");

	m_pkActor=new CActorInstance;

	CActorInstance& rkActor=GetActorRef();
	rkActor.SetEventHandler(CActorInstance::IEventHandler::GetEmptyPtr());
	if (!rkActor.SetRace(eRace))
	{
		delete m_pkActor;
		m_pkActor=NULL;
		return;
	}

	rkActor.SetShape(0);
	rkActor.SetBattleHitEffect(eHitEffect);
	rkActor.SetAlphaValue(0.0f);
	rkActor.BlendAlphaValue(1.0f, 0.5f);
	rkActor.SetMoveSpeed(1.0f);
	rkActor.SetAttackSpeed(1.0f);
	rkActor.SetMotionMode(CRaceMotionData::MODE_GENERAL);
	rkActor.Stop();
	rkActor.RefreshActorInstance();

	rkActor.SetCurPixelPosition(c_rkPPos);

	m_isMounting=true;
}

void CInstanceBase::SHORSE::Destroy()
{
	if (m_pkActor)
	{
		m_pkActor->Destroy();
		delete m_pkActor;
	}

	__Initialize();
}

CActorInstance& CInstanceBase::SHORSE::GetActorRef()
{
	assert(NULL!=m_pkActor && "CInstanceBase::SHORSE::GetActorRef");
	return *m_pkActor;
}

CActorInstance* CInstanceBase::SHORSE::GetActorPtr()
{
	return m_pkActor;
}

UINT CInstanceBase::SHORSE::GetLevel()
{
	if (m_pkActor)
	{
		DWORD mount = m_pkActor->GetRace();
		switch (mount)
		{
          // Normal At
			case 20101:
			case 20102:
			case 20103:
				return 1;
          // Zırhlı At
			case 20104:
			case 20105:
			case 20106:
				return 2;
          // Asker At
			case 20107:
			case 20108:
			case 20109:
#ifdef ENABLE_RIDING_EXTENDED
			case 20149:
			case 20150:
			case 20151:
#endif
			case 20110:
			case 20111:
			case 20112:
			case 20113:
			case 20114:
			case 20115:
			case 20116:
			case 20117:
			case 20118:
			case 20120:
			case 20121:
			case 20122:
			case 20123:
			case 20124:
			case 20125:
			case 20226:
			case 20227:
			case 20119:
          // Attack Fix
			case 20201:
			case 20202:
			case 20203:
			case 20204:
			case 20205:
			case 20206:
			case 20207:
			case 20208:
			case 20209:
			case 20210:
			case 20211:
			case 20212:
			case 20213:
			case 20214:
			case 20215:
			case 20216:
			case 20217:
			case 20218:
          // Attack Fix
			case 20219:
			case 20220:
			case 20221:
			case 20222:
			case 20224:
			case 20225:
			case 20229:
			case 20230:
			case 20231:
			case 20232:
			case 20233:
			case 20234:
			case 20235:
			case 20236:
			case 20237:
			case 20238:
			case 20239:
			case 20240:
			case 20241:
			case 20242:
			case 20243:
			case 20244:
			case 20245:
			case 20246:
			case 20247:
			case 20248:
			case 20249:
			case 20250:
			case 20251:
			case 20252:
			case 20253:
			case 20254:
			case 20255:
			case 20256:
			case 20257:
			case 20258:
			case 20259:
			case 20260:
			case 20261:
			case 20262:
			case 20263:
			case 20264:
			case 20265:
			case 20266:
			case 20267:
			case 20268:
			case 20269:
			case 20270:
			case 20271:
			case 20272:
			case 20273:
			case 20274:
			case 20275:
			case 20276:
			case 20277:
			case 20278:
			case 20279:
				return 3;
		}

		{
			if ((20205 <= mount && 20208 >= mount) ||
				(20214 == mount) || (20217 == mount) ||
				(20224 == mount) || (20229 == mount) ||
				(20234 == mount) || (20237 == mount)
				)
				return 2;

			if ((20209 <= mount &&  20212 >= mount) || 
				(20215 == mount) || (20218 == mount) ||
				(20225 == mount) || (20230 == mount) ||
				(20235 == mount) || (20238 == mount)
#ifdef ENABLE_STANDING_MOUNT
				|| (40003 == mount) || (40004 == mount) || (40005 == mount)
#endif
				)
				return 3;
		}
	}
	return 0;
}

bool CInstanceBase::SHORSE::IsNewMount()
{
	if (!m_pkActor)
		return false;
	DWORD mount = m_pkActor->GetRace();

	if ((20205 <= mount &&  20208 >= mount) ||
		(20214 == mount) || (20217 == mount) ||
		(20224 == mount) || (20229 == mount) ||
		(20234 == mount) || (20237 == mount)
		)
		return true;

	// °í±Ş Å»°Í
	if ((20209 <= mount && 20212 >= mount) ||
		(20215 == mount) || (20218 == mount) ||
		(20225 == mount) || (20230 == mount) ||
		(20235 == mount) || (20238 == mount)
		)
		return true;

	return false;
}

#ifdef ENABLE_STANDING_MOUNT
bool CInstanceBase::SHORSE::IsHoverBoard()
{
	if (m_pkActor)
	{
		DWORD dwRace = m_pkActor->GetRace();
		return (dwRace == STANDING_MOUNT_VNUM_1 || 
				dwRace == STANDING_MOUNT_VNUM_2 || 
				dwRace == STANDING_MOUNT_VNUM_3);
	}
	return false;
}
#endif

bool CInstanceBase::SHORSE::CanUseSkill()
{
#ifdef ENABLE_STANDING_MOUNT
	// Standing mount üzerindeyken level kontrolü yapma
	if (IsMounting())
	{
		if (IsHoverBoard())
			return true;
		
		return 2 < GetLevel();
	}
#endif

	return true;
}

bool CInstanceBase::SHORSE::CanAttack()
{
	if (IsMounting())
		if (GetLevel()<=1)
			return false;

	return true;
}

bool CInstanceBase::SHORSE::IsMounting()
{
	return m_isMounting;
}

void CInstanceBase::SHORSE::Deform()
{
	if (!IsMounting())
		return;

	CActorInstance& rkActor=GetActorRef();
	rkActor.INSTANCEBASE_Deform();
}

void CInstanceBase::SHORSE::Render()
{
	if (!IsMounting())
		return;

	CActorInstance& rkActor=GetActorRef();
	rkActor.Render();
}

void CInstanceBase::__AttachHorseSaddle()
{
	if (!IsMountingHorse())
		return;
	m_kHorse.m_pkActor->AttachModelInstance(CRaceData::PART_MAIN, "saddle", m_GraphicThingInstance, CRaceData::PART_MAIN);
}

void CInstanceBase::__DetachHorseSaddle()
{
	if (!IsMountingHorse())
		return;
	m_kHorse.m_pkActor->DetachModelInstance(CRaceData::PART_MAIN, m_GraphicThingInstance, CRaceData::PART_MAIN);
}

void CInstanceBase::BlockMovement()
{
	m_GraphicThingInstance.BlockMovement();
}

bool CInstanceBase::IsBlockObject(const CGraphicObjectInstance& c_rkBGObj)
{
	return m_GraphicThingInstance.IsBlockObject(c_rkBGObj);
}

bool CInstanceBase::AvoidObject(const CGraphicObjectInstance& c_rkBGObj)
{
	return m_GraphicThingInstance.AvoidObject(c_rkBGObj);
}

bool __ArmorVnumToShape(int iVnum, DWORD * pdwShape)
{
	*pdwShape = iVnum;

	if (0 == iVnum || 1 == iVnum)
		return false;

	if (!USE_ARMOR_SPECULAR)
		return false;

	CItemData * pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(iVnum, &pItemData))
		return false;

	enum
	{
		SHAPE_VALUE_SLOT_INDEX = 3,
	};

	*pdwShape = pItemData->GetValue(SHAPE_VALUE_SLOT_INDEX);

	return true;
}

class CActorInstanceBackground : public IBackground
{
	public:
		CActorInstanceBackground() {}
		virtual ~CActorInstanceBackground() {}
		bool IsBlock(int x, int y)
		{
			CPythonBackground& rkBG=CPythonBackground::Instance();
			return rkBG.isAttrOn(x, y, CTerrainImpl::ATTRIBUTE_BLOCK);
		}
};

static CActorInstanceBackground gs_kActorInstBG;

bool CInstanceBase::LessRenderOrder(CInstanceBase* pkInst)
{
	int nMainAlpha=(__GetAlphaValue() < 1.0f) ? 1 : 0;
	int nTestAlpha=(pkInst->__GetAlphaValue() < 1.0f) ? 1 : 0;
	if (nMainAlpha < nTestAlpha)
		return true;
	if (nMainAlpha > nTestAlpha)
		return false;

	if (GetRace()<pkInst->GetRace())
		return true;
	if (GetRace()>pkInst->GetRace())
		return false;

	if (GetShape()<pkInst->GetShape())
		return true;

	if (GetShape()>pkInst->GetShape())
		return false;

	UINT uLeftLODLevel=__LessRenderOrder_GetLODLevel();
	UINT uRightLODLevel=pkInst->__LessRenderOrder_GetLODLevel();
	if (uLeftLODLevel<uRightLODLevel)
		return true;
	if (uLeftLODLevel>uRightLODLevel)
		return false;

	if (m_awPart[CRaceData::PART_WEAPON]<pkInst->m_awPart[CRaceData::PART_WEAPON])
		return true;

	return false;
}

UINT CInstanceBase::__LessRenderOrder_GetLODLevel()
{
	CGrannyLODController* pLODCtrl=m_GraphicThingInstance.GetLODControllerPointer(0);
	if (!pLODCtrl)
		return 0;

	return pLODCtrl->GetLODLevel();
}

bool CInstanceBase::__Background_GetWaterHeight(const TPixelPosition& c_rkPPos, float *pfHeight)
{
	long lHeight;
	if (!CPythonBackground::Instance().GetWaterHeight(int(c_rkPPos.x), int(c_rkPPos.y), &lHeight))
		return false;

	*pfHeight = float(lHeight);

	return true;
}

bool CInstanceBase::__Background_IsWaterPixelPosition(const TPixelPosition& c_rkPPos)
{
	return CPythonBackground::Instance().isAttrOn(c_rkPPos.x, c_rkPPos.y, CTerrainImpl::ATTRIBUTE_WATER);
}

const float PC_DUST_RANGE = 2000.0f;
const float NPC_DUST_RANGE = 1000.0f;

DWORD CInstanceBase::ms_dwUpdateCounter=0;
DWORD CInstanceBase::ms_dwRenderCounter=0;
DWORD CInstanceBase::ms_dwDeformCounter=0;
float CInstanceBase::ms_fCrowdDetailDistance=FLT_MAX;

CDynamicPool<CInstanceBase> CInstanceBase::ms_kPool;

bool CInstanceBase::__IsInDustRange()
{
	if (!__IsExistMainInstance())
		return false;

	CInstanceBase* pkInstMain=__GetMainInstancePtr();

	float fDistance=NEW_GetDistanceFromDestInstance(*pkInstMain);

	if (IsPC())
	{
		if (fDistance<=PC_DUST_RANGE)
			return true;
	}

	if (fDistance<=NPC_DUST_RANGE)
		return true;

	return false;
}

void CInstanceBase::__EnableSkipCollision()
{
	if (__IsMainInstance())
	{
		TraceError("CInstanceBase::__EnableSkipCollision - You should not skip your own collisions!!");
		return;
	}
	m_GraphicThingInstance.EnableSkipCollision();
}

void CInstanceBase::__DisableSkipCollision()
{
	m_GraphicThingInstance.DisableSkipCollision();
}

DWORD CInstanceBase::__GetShadowMapColor(float x, float y)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	return rkBG.GetShadowMapColor(x, y);
}

float CInstanceBase::__GetBackgroundHeight(float x, float y)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	return rkBG.GetHeight(x, y);
}

BOOL CInstanceBase::IsInvisibility()
{
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
	if (IsAffect(AFFECT_INVISIBILITY) && !__MainCanSeeHiddenThing())
		return true;
#else
	if (IsAffect(AFFECT_INVISIBILITY))
		return true;
#endif

	return false;
}

BOOL CInstanceBase::IsParalysis()
{
	return m_GraphicThingInstance.IsParalysis();
}

BOOL CInstanceBase::IsGameMaster()
{
	if (m_kAffectFlagContainer.IsSet(AFFECT_YMIR))
		return true;

#ifdef ENABLE_RENEWAL_TEAM_AFFECT
	if (m_kAffectFlagContainer.IsSet(AFFECT_TEAM_SA))
		return true;

	if (m_kAffectFlagContainer.IsSet(AFFECT_TEAM_GA))
		return true;

	if (m_kAffectFlagContainer.IsSet(AFFECT_TEAM_GM))
		return true;

	if (m_kAffectFlagContainer.IsSet(AFFECT_TEAM_TGM))
		return true;
#endif
	return false;
}

BOOL CInstanceBase::IsSameEmpire(CInstanceBase& rkInstDst)
{
	if (0 == rkInstDst.m_dwEmpireID)
		return TRUE;

	if (IsGameMaster())
		return TRUE;

	if (rkInstDst.IsGameMaster())
		return TRUE;

	if (rkInstDst.m_dwEmpireID==m_dwEmpireID)
		return TRUE;

	return FALSE;
}

DWORD CInstanceBase::GetEmpireID()
{
	return m_dwEmpireID;
}

#ifdef ENABLE_SHOW_MOB_INFO
DWORD CInstanceBase::GetAIFlag()
{
	return m_dwAIFlag;
}
#endif

DWORD CInstanceBase::GetGuildID()
{
	return m_dwGuildID;
}

#ifdef ENABLE_GUILD_LEADER_TEXTAIL
BYTE CInstanceBase::GetGuildLeader()
{
	return m_dwGuildLeader;
}
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
DWORD* CInstanceBase::GetSkillColor(DWORD dwSkillIndex)
{
	DWORD dwSkillSlot = dwSkillIndex + 1;
	CPythonSkill::SSkillData* c_pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(dwSkillSlot, &c_pSkillData))
		return 0;

	WORD dwEffectID = c_pSkillData->GradeData[CPythonSkill::SKILL_GRADE_COUNT].wMotionIndex - CRaceMotionData::NAME_SKILL - (1 * 25);
	return m_GraphicThingInstance.GetSkillColorByMotionID(dwEffectID);
}
#endif

int CInstanceBase::GetAlignment()
{
	return m_sAlignment;
}

UINT CInstanceBase::GetAlignmentGrade()
{
	if (m_sAlignment >= 12000)
		return 0;
	else if (m_sAlignment >= 8000)
		return 1;
	else if (m_sAlignment >= 4000)
		return 2;
	else if (m_sAlignment >= 1000)
		return 3;
	else if (m_sAlignment >= 0)
		return 4;
	else if (m_sAlignment > -4000)
		return 5;
	else if (m_sAlignment > -8000)
		return 6;
	else if (m_sAlignment > -12000)
		return 7;

	return 8;
}

int CInstanceBase::GetAlignmentType()
{
	switch (GetAlignmentGrade())
	{
		case 0:
		case 1:
		case 2:
		case 3:
		{
			return ALIGNMENT_TYPE_WHITE;
			break;
		}

		case 5:
		case 6:
		case 7:
		case 8:
		{
			return ALIGNMENT_TYPE_DARK;
			break;
		}
	}

	return ALIGNMENT_TYPE_NORMAL;
}

BYTE CInstanceBase::GetPKMode()
{
	return m_byPKMode;
}

bool CInstanceBase::IsKiller()
{
	return m_isKiller;
}

bool CInstanceBase::IsPartyMember()
{
	return m_isPartyMember;
}

BOOL CInstanceBase::IsInSafe()
{
	const TPixelPosition& c_rkPPosCur=m_GraphicThingInstance.NEW_GetCurPixelPositionRef();
	if (CPythonBackground::Instance().isAttrOn(c_rkPPosCur.x, c_rkPPosCur.y, CTerrainImpl::ATTRIBUTE_BANPK))
		return TRUE;

	return FALSE;
}

float CInstanceBase::CalculateDistanceSq3d(const TPixelPosition& c_rkPPosDst)
{
	const TPixelPosition& c_rkPPosSrc=m_GraphicThingInstance.NEW_GetCurPixelPositionRef();
	return SPixelPosition_CalculateDistanceSq3d(c_rkPPosSrc, c_rkPPosDst);
}

void CInstanceBase::OnSelected()
{
	if (IsStoneDoor())
		return;

	if (IsDead())
		return;

	__AttachSelectEffect();
}

void CInstanceBase::OnUnselected()
{
	__DetachSelectEffect();
}

void CInstanceBase::OnTargeted()
{
	if (IsStoneDoor())
		return;

	if (IsDead())
		return;

	__AttachTargetEffect();
}

void CInstanceBase::OnUntargeted()
{
	__DetachTargetEffect();
}

void CInstanceBase::DestroySystem()
{
	ms_kPool.Clear();
}

void CInstanceBase::CreateSystem(UINT uCapacity)
{
	ms_kPool.Create(uCapacity);

	memset(ms_adwCRCAffectEffect, 0, sizeof(ms_adwCRCAffectEffect));

	ms_fDustGap=250.0f;
	ms_fHorseDustGap=500.0f;
}

CInstanceBase* CInstanceBase::New()
{
	return ms_kPool.Alloc();
}

void CInstanceBase::Delete(CInstanceBase* pkInst)
{
	pkInst->Destroy();
	ms_kPool.Free(pkInst);
}

void CInstanceBase::SetMainInstance()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();

	DWORD dwVID=GetVirtualID();
	rkChrMgr.SetMainInstance(dwVID);

	m_GraphicThingInstance.SetMainInstance();
}

CInstanceBase* CInstanceBase::__GetMainInstancePtr()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	return rkChrMgr.GetMainInstancePtr();
}

void CInstanceBase::__ClearMainInstance()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	rkChrMgr.ClearMainInstance();
}

/* ½ÇÁ¦ ÇÃ·¹ÀÌ¾î Ä³¸¯ÅÍÀÎÁö Á¶»ç.*/
bool CInstanceBase::__IsMainInstance()
{
	if (this==__GetMainInstancePtr())
		return true;

	return false;
}

bool CInstanceBase::__IsExistMainInstance()
{
	if(__GetMainInstancePtr())
		return true;
	else
		return false;
}

bool CInstanceBase::__MainCanSeeHiddenThing()
{
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
	CInstanceBase * pInstance = __GetMainInstancePtr();
	return (pInstance) ? TRUE == pInstance->IsGameMaster() : false;
#else
	return false;
#endif
}

float CInstanceBase::__GetBowRange()
{
	float fRange = 2500.0f - 100.0f;

	if (__IsMainInstance())
	{
		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		fRange += float(rPlayer.GetStatus(POINT_BOW_DISTANCE));
	}

	return fRange;
}

CInstanceBase* CInstanceBase::__FindInstancePtr(DWORD dwVID)
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	return rkChrMgr.GetInstancePtr(dwVID);
}

bool CInstanceBase::__FindRaceType(DWORD dwRace, BYTE* pbType)
{
	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
	return rkNonPlayer.GetInstanceType(dwRace, pbType);
}

bool CInstanceBase::Create(const SCreateData& c_rkCreateData)
{
	SetInstanceType(c_rkCreateData.m_bType);

#ifdef ENABLE_NPC_WEAR_ITEM
	const auto npcRaceIndex = CPythonNonPlayer::Instance().GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_RACE);
	if (npcRaceIndex != -1)
	{
		if (!SetRace(npcRaceIndex))
			return false;
	}
	else
	{
		if (!SetRace(c_rkCreateData.m_dwRace))
			return false;
	}
#else
	if (!SetRace(c_rkCreateData.m_dwRace))
		return false;
#endif

	SetVirtualID(c_rkCreateData.m_dwVID);

	if (c_rkCreateData.m_isMain)
		SetMainInstance();

	if (IsGuildWall())
	{
		unsigned center_x;
		unsigned center_y;

		c_rkCreateData.m_kAffectFlags.ConvertToPosition(&center_x, &center_y);

		float center_z = __GetBackgroundHeight(center_x, center_y);
		NEW_SetPixelPosition(TPixelPosition(float(c_rkCreateData.m_lPosX), float(c_rkCreateData.m_lPosY), center_z));
	}
	else
	{
		SCRIPT_SetPixelPosition(float(c_rkCreateData.m_lPosX), float(c_rkCreateData.m_lPosY));
	}

#ifdef ENABLE_NPC_WEAR_ITEM
	if (npcRaceIndex != -1)
	{
		CPythonNonPlayer& nonPlayer = CPythonNonPlayer::Instance();

		const auto mount = nonPlayer.GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_MOUNT);
		if (mount != 0)
			MountHorse(mount);

#ifdef ENABLE_BUFFI_SYSTEM
		// Buffi (34900/34901): armor / weapon / hair / sash are the costumes sent by the server.
		const bool isBuffi = c_rkCreateData.m_dwRace == 34900 || c_rkCreateData.m_dwRace == 34901;
		if (isBuffi)
		{
			SetArmor(c_rkCreateData.m_dwArmor ? c_rkCreateData.m_dwArmor : nonPlayer.GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_ARMOR));
			SetWeapon(c_rkCreateData.m_dwWeapon ? c_rkCreateData.m_dwWeapon : nonPlayer.GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_WEAPON));
			SetHair(c_rkCreateData.m_dwHair);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			SetAcce(c_rkCreateData.m_dwAcce);
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			// the Buffi casts with the colours chosen in its skill colour window
			ChangeSkillColor(*c_rkCreateData.m_dwSkillColor);
			memcpy(m_dwSkillColor, *c_rkCreateData.m_dwSkillColor, sizeof(m_dwSkillColor));
#endif
		}
		else
#endif
		{
		SetArmor(nonPlayer.GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_ARMOR));
		SetWeapon(nonPlayer.GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_WEAPON));
		SetHair(nonPlayer.GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_HAIR));
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		SetAcce(nonPlayer.GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_ACCE));
#endif
		}
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		SetAura(nonPlayer.GetNPCWear(c_rkCreateData.m_dwRace, NPC_WEAR_AURA));
#endif
	}
	else
	{
		if (0 != c_rkCreateData.m_dwMountVnum)
			MountHorse(c_rkCreateData.m_dwMountVnum);
		SetArmor(c_rkCreateData.m_dwArmor);
	}
#else
	if (0 != c_rkCreateData.m_dwMountVnum)
		MountHorse(c_rkCreateData.m_dwMountVnum);

	SetArmor(c_rkCreateData.m_dwArmor);
#endif

	if (IsPC())
	{
		SetHair(c_rkCreateData.m_dwHair);
		SetWeapon(c_rkCreateData.m_dwWeapon);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		SetAcce(c_rkCreateData.m_dwAcce);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		SetAura(c_rkCreateData.m_dwAura);
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		SetLanguage(c_rkCreateData.m_bLanguage);
		SetLanguage2(c_rkCreateData.m_bLanguage2);
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		ChangeSkillColor(*c_rkCreateData.m_dwSkillColor);
		memcpy(m_dwSkillColor, *c_rkCreateData.m_dwSkillColor, sizeof(m_dwSkillColor));
#endif
	}

	__Create_SetName(c_rkCreateData);
#ifdef ENABLE_CONQUEROR_LEVEL
	if(IsPC())
		m_dwConquerorLevel = c_rkCreateData.m_dwConquerorLevel;
#endif
	m_dwLevel = c_rkCreateData.m_dwLevel;
#ifdef ENABLE_SHOW_MOB_INFO
	m_dwAIFlag = c_rkCreateData.m_dwAIFlag;
#endif
	m_dwGuildID = c_rkCreateData.m_dwGuildID;
	m_dwEmpireID = c_rkCreateData.m_dwEmpireID;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	m_dwGuildLeader = c_rkCreateData.m_dwGuildLeader;
#endif
	SetVirtualNumber(c_rkCreateData.m_dwRace);
	SetRotation(c_rkCreateData.m_fRot);
	SetAlignment(c_rkCreateData.m_sAlignment);
	SetPKMode(c_rkCreateData.m_byPKMode);
	SetMoveSpeed(c_rkCreateData.m_dwMovSpd);
	SetAttackSpeed(c_rkCreateData.m_dwAtkSpd);

#ifdef ENABLE_GROWTH_PET_SYSTEM
	m_bCharacterSize = c_rkCreateData.m_bCharacterSize;
	if (IsGrowthPet())
		SetPetLevel(c_rkCreateData.m_dwLevel);
#endif

#ifdef ENABLE_MOB_SCALE
	float fx, fy, fz = 1.0f;

	if (CPythonNonPlayer::Instance().GetScale(c_rkCreateData.m_dwRace, fx, fy, fz)
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		&& !c_rkCreateData.m_dwAcce
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		&& !c_rkCreateData.m_dwAura
#endif
	)
		m_GraphicThingInstance.SetScale(fx, fy, fz, true);
#endif

	if (!IsWearingDress())
	{
		m_GraphicThingInstance.SetAlphaValue(0.0f);
		m_GraphicThingInstance.BlendAlphaValue(1.0f, 0.5f);
	}

	if (!IsGuildWall())
	{
		SetAffectFlagContainer(c_rkCreateData.m_kAffectFlags);
	}

	if (c_rkCreateData.m_dwStateFlags & ADD_CHARACTER_STATE_SPAWN) 
	{
		if (IsAffect(AFFECT_SPAWN))
			__AttachEffect(EFFECT_SPAWN_APPEAR);

		if (IsPC())
		{
			Refresh(CRaceMotionData::NAME_WAIT, true);
		}
		else
		{
			Refresh(CRaceMotionData::NAME_SPAWN, false);
		}
	}
	else
	{
		Refresh(CRaceMotionData::NAME_WAIT, true);
	}

	__AttachEmpireEffect(c_rkCreateData.m_dwEmpireID);

	RegisterBoundingSphere();

	AttachTextTail();
	RefreshTextTail();

#ifdef ENABLE_STONE_SCALE_OPTION
	RefreshStoneScale();
#endif

#ifdef ENABLE_BOSS_EFFECT_OVER_HEAD
	DWORD dwRank = CPythonNonPlayer::Instance().GetMonsterRank(c_rkCreateData.m_dwRace);
	DWORD dwType = CPythonNonPlayer::Instance().GetMonsterType(c_rkCreateData.m_dwRace);

	if (dwRank >= 4 && dwType == 0)
		__AttachBossEffect();
#endif

	if (c_rkCreateData.m_dwStateFlags & ADD_CHARACTER_STATE_DEAD)
		m_GraphicThingInstance.DieEnd();

	SetStateFlags(c_rkCreateData.m_dwStateFlags);

	m_GraphicThingInstance.SetBattleHitEffect(ms_adwCRCAffectEffect[EFFECT_HIT]);

	if (!IsPC())
	{
		DWORD dwBodyColor = CPythonNonPlayer::Instance().GetMonsterColor(c_rkCreateData.m_dwRace);
		if (0 != dwBodyColor)
		{
			SetModulateRenderMode();
			SetAddColor(dwBodyColor);
		}
	}

	__AttachHorseSaddle();

	const int c_iGuildSymbolRace = 14200;
	if (c_iGuildSymbolRace == GetRace())
	{
		std::string strFileName = GetGuildSymbolFileName(m_dwGuildID);
		if (IsFile(strFileName.c_str()))
			m_GraphicThingInstance.ChangeMaterial(strFileName.c_str());
	}

#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
	if (IsAffect(AFFECT_INVISIBILITY) && __MainCanSeeHiddenThing())
		m_GraphicThingInstance.BlendAlphaValue(0.5f, 0.5f);
#endif

	return true;
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CInstanceBase::ChangeSkillColor(const DWORD *c_dwSkillColor)
{
	unsigned long ulSkillVnumColor[CRaceMotionData::SKILL_NUM][ESkillColorLength::MAX_EFFECT_COUNT];
	memset(ulSkillVnumColor, 0, sizeof(ulSkillVnumColor));

	unsigned long ulSkillColorSlot[ESkillColorLength::MAX_SKILL_COUNT + MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memcpy(ulSkillColorSlot, c_dwSkillColor, sizeof(ulSkillColorSlot));

	for (uint16_t byRace = 0; byRace < NPlayerData::MAIN_RACE_MAX_NUM; byRace++)
	{
		uint16_t bySkillVnum = 0;
		switch (byRace)
		{
			case NPlayerData::MAIN_RACE_WARRIOR_M:
				bySkillVnum = 1;
				break;

			case NPlayerData::MAIN_RACE_ASSASSIN_W:
				bySkillVnum = 31;
				break;

			case NPlayerData::MAIN_RACE_SURA_M:
				bySkillVnum = 76;
				break;

			case NPlayerData::MAIN_RACE_SHAMAN_W:
				bySkillVnum = 111;
				break;

			case NPlayerData::MAIN_RACE_WARRIOR_W:
				bySkillVnum = 16;
				break;

			case NPlayerData::MAIN_RACE_ASSASSIN_M:
				bySkillVnum = 46;
				break;

			case NPlayerData::MAIN_RACE_SURA_W:
				bySkillVnum = 61;
				break;

			case NPlayerData::MAIN_RACE_SHAMAN_M:
				bySkillVnum = 91;
				break;
		}
		uint16_t byNextSkillVnum = bySkillVnum;

		for (uint16_t bySkillSlot = 0; bySkillSlot < ESkillColorLength::MAX_SKILL_COUNT; bySkillSlot++)
		{
			if (bySkillSlot > 6)
				continue;

			for (uint16_t byLayer = 0; byLayer < ESkillColorLength::MAX_EFFECT_COUNT; ++byLayer)
			{
				ulSkillVnumColor[byNextSkillVnum][byLayer] = ulSkillColorSlot[byNextSkillVnum - bySkillVnum][byLayer];
			}
			++byNextSkillVnum;
		}
	}

	for (uint16_t byBuffSlot = ESkillColorLength::BUFF_BEGIN; byBuffSlot < ESkillColorLength::MAX_COLOR_SLOTS; byBuffSlot++)
	{
		BYTE byBuffSkillVnum = 0;
		switch (byBuffSlot)
		{
			case BUFF_BEGIN + 0:
				byBuffSkillVnum = 94;
				break;

			case BUFF_BEGIN + 1:
				byBuffSkillVnum = 95;
				break;

			case BUFF_BEGIN + 2:
				byBuffSkillVnum = 96;
				break;

			case BUFF_BEGIN + 3:
				byBuffSkillVnum = 110;
				break;

			case BUFF_BEGIN + 4:
				byBuffSkillVnum = 111;
				break;

#ifdef ENABLE_NINETH_SKILL
			case BUFF_BEGIN + 5:
				byBuffSkillVnum = 182;
				break;
#endif
#ifdef ENABLE_WOLFMAN_CHARACTER
			case BUFF_BEGIN + 6:
				byBuffSkillVnum = 175;
				break;
#endif
		}

		if (byBuffSkillVnum == 0)
			continue;

		for (uint16_t byLayer = 0; byLayer < ESkillColorLength::MAX_EFFECT_COUNT; ++byLayer)
			ulSkillVnumColor[byBuffSkillVnum][byLayer] = ulSkillColorSlot[byBuffSlot][byLayer];
	}

	m_GraphicThingInstance.ChangeSkillColor(*ulSkillVnumColor);
}
#endif

void CInstanceBase::__Create_SetName(const SCreateData& c_rkCreateData)
{
	if (IsGoto())
	{
		SetNameString("", 0);
		return;
	}
	if (IsWarp())
	{
		__Create_SetWarpName(c_rkCreateData);
		return;
	}

	SetNameString(c_rkCreateData.m_stName.c_str(), c_rkCreateData.m_stName.length());
}

void CInstanceBase::__Create_SetWarpName(const SCreateData& c_rkCreateData)
{
	const char *c_szName;
	if (CPythonNonPlayer::Instance().GetName(c_rkCreateData.m_dwRace, &c_szName))
	{
		std::string strName = c_szName;
		int iFindingPos = strName.find_first_of(" ", 0);
		if (iFindingPos > 0)
		{
			strName.resize(iFindingPos);
		}
		SetNameString(strName.c_str(), strName.length());
	}
	else
	{
		SetNameString(c_rkCreateData.m_stName.c_str(), c_rkCreateData.m_stName.length());
	}
}

void CInstanceBase::SetNameString(const char *c_szName, int len)
{
	m_stName.assign(c_szName, len);
}


bool CInstanceBase::SetRace(DWORD eRace)
{
	m_dwRace = eRace;

	if (!m_GraphicThingInstance.SetRace(eRace))
		return false;

	if (!__FindRaceType(m_dwRace, &m_eRaceType))
		m_eRaceType=CActorInstance::TYPE_PC;

	return true;
}

BOOL CInstanceBase::__IsChangableWeapon(int iWeaponID)
{
	if (IsWearingDress())
	{
		const int c_iBouquets[] =
		{
			50201, // Bouquet for Assassin
			50202, // Bouquet for Shaman
			50203,
			50204,
			0,
		};

		for (int i = 0; c_iBouquets[i] != 0; ++i)
			if (iWeaponID == c_iBouquets[i])
				return true;

		return false;
	}
	else
		return true;
}

BOOL CInstanceBase::IsWearingDress()
{
	const int c_iWeddingDressShape = 201;
	return c_iWeddingDressShape == m_eShape;
}

BOOL CInstanceBase::IsHoldingPickAxe()
{
	const int c_iPickAxeStart = 29101;
	const int c_iPickAxeEnd = 29110;
	return m_awPart[CRaceData::PART_WEAPON] >= c_iPickAxeStart && m_awPart[CRaceData::PART_WEAPON] <= c_iPickAxeEnd;
}

#ifdef ENABLE_STANDING_MOUNT
BOOL CInstanceBase::IsHoldingFishRod()
{
	const int c_iFishRodStart = 27400;
	const int c_iFishRodEnd = 27590;
	return m_awPart[CRaceData::PART_WEAPON] >= c_iFishRodStart && m_awPart[CRaceData::PART_WEAPON] <= c_iFishRodEnd;
}
#endif

BOOL CInstanceBase::IsNewMount()
{
	return m_kHorse.IsNewMount();
}

#ifdef ENABLE_STANDING_MOUNT
BOOL CInstanceBase::IsMountingHoverBoard()
{
	// Sadece oyuncu karakterleri için kontrol et
	if (!IsPC())
		return false;
		
	return m_kHorse.IsHoverBoard();
}
#endif

BOOL CInstanceBase::IsMountingHorse()
{
	return m_kHorse.IsMounting();
}

void CInstanceBase::MountHorse(UINT eRace)
{
	m_kHorse.Destroy();
	m_kHorse.Create(m_GraphicThingInstance.NEW_GetCurPixelPositionRef(), eRace, ms_adwCRCAffectEffect[EFFECT_HIT]);

	SetMotionMode(CRaceMotionData::MODE_HORSE);
#ifdef ENABLE_STANDING_MOUNT
	if (m_kHorse.IsHoverBoard())
		SetRotationSpeed(c_fDefaultStandingMountRotationSpeed);
	else
		SetRotationSpeed(c_fDefaultHorseRotationSpeed);
#else
	SetRotationSpeed(c_fDefaultHorseRotationSpeed);
#endif

	m_GraphicThingInstance.MountHorse(m_kHorse.GetActorPtr());
	m_GraphicThingInstance.Stop();
	m_GraphicThingInstance.RefreshActorInstance();

#ifdef ENABLE_STANDING_MOUNT
	// Standing mount'a ilk bini?te silah attach s?ras? nedeniyle fan bazen çift elde kalabiliyor.
	// Tak/ç?kar yap?nca düzeldi?i için, hoverboard + fan durumunda burada bir kere yeniden attach ediyoruz.
	if (m_kHorse.IsHoverBoard())
	{
		if (GetWeaponType() == CItemData::WEAPON_FAN)
		{
			const DWORD dwWeapon = GetPart(CRaceData::PART_WEAPON);
			m_GraphicThingInstance.AttachWeapon(dwWeapon);
			m_GraphicThingInstance.RefreshActorInstance();
		}
	}
#endif
}

void CInstanceBase::DismountHorse()
{
	m_kHorse.Destroy();
}

void CInstanceBase::GetInfo(std::string* pstInfo)
{
	char szInfo[256];
	sprintf(szInfo, "Inst - UC %d, RC %d Pool - %d ",
		ms_dwUpdateCounter,
		ms_dwRenderCounter,
		ms_kPool.GetCapacity()
	);

	pstInfo->append(szInfo);
}

void CInstanceBase::ResetPerformanceCounter()
{
	ms_dwUpdateCounter=0;
	ms_dwRenderCounter=0;
	ms_dwDeformCounter=0;
}

bool CInstanceBase::NEW_IsLastPixelPosition()
{
	return m_GraphicThingInstance.IsPushing();
}

const TPixelPosition& CInstanceBase::NEW_GetLastPixelPositionRef()
{
	return m_GraphicThingInstance.NEW_GetLastPixelPositionRef();
}

void CInstanceBase::NEW_SetDstPixelPositionZ(FLOAT z)
{
	m_GraphicThingInstance.NEW_SetDstPixelPositionZ(z);
}

void CInstanceBase::NEW_SetDstPixelPosition(const TPixelPosition& c_rkPPosDst)
{
	m_GraphicThingInstance.NEW_SetDstPixelPosition(c_rkPPosDst);
}

void CInstanceBase::NEW_SetSrcPixelPosition(const TPixelPosition& c_rkPPosSrc)
{
	m_GraphicThingInstance.NEW_SetSrcPixelPosition(c_rkPPosSrc);
}

const TPixelPosition& CInstanceBase::NEW_GetCurPixelPositionRef()
{
	return m_GraphicThingInstance.NEW_GetCurPixelPositionRef();
}

const TPixelPosition& CInstanceBase::NEW_GetDstPixelPositionRef()
{
	return m_GraphicThingInstance.NEW_GetDstPixelPositionRef();
}

const TPixelPosition& CInstanceBase::NEW_GetSrcPixelPositionRef()
{
	return m_GraphicThingInstance.NEW_GetSrcPixelPositionRef();
}

void CInstanceBase::OnSyncing()
{
	m_GraphicThingInstance.__OnSyncing();
}

void CInstanceBase::OnWaiting()
{
	m_GraphicThingInstance.__OnWaiting();
}

void CInstanceBase::OnMoving()
{
	m_GraphicThingInstance.__OnMoving();
}

#ifdef ENABLE_GUILD_LEADER_TEXTAIL
void CInstanceBase::ChangeGuild(DWORD dwGuildID, DWORD dwGuildLeader)
#else
void CInstanceBase::ChangeGuild(DWORD dwGuildID)
#endif
{
	m_dwGuildID = dwGuildID;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	m_dwGuildLeader = dwGuildLeader;
#endif

	DetachTextTail();
	AttachTextTail();
	RefreshTextTail();
}

DWORD CInstanceBase::GetPart(CRaceData::EParts part)
{
	assert(part >= 0 && part < CRaceData::PART_MAX_NUM);
	return m_awPart[part];
}

DWORD CInstanceBase::GetShape()
{
	return m_eShape;
}

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
bool CInstanceBase::CanInteract()
{
	if (!CanAct())
		return false;

	if (IsStun())
		return false;

	if (IsDead())
		return false;

	if (m_GraphicThingInstance.isAttacking())
		return false;

	if (m_GraphicThingInstance.IsAttacked())
		return false;

	return true;
}
#endif

bool CInstanceBase::CanAct()
{
	return m_GraphicThingInstance.CanAct();
}

bool CInstanceBase::CanMove()
{
	return m_GraphicThingInstance.CanMove();
}

bool CInstanceBase::CanUseSkill()
{
	if (IsPoly())
		return false;

	if (IsWearingDress())
		return false;

	if (IsHoldingPickAxe())
		return false;

#ifdef ENABLE_STANDING_MOUNT
	if (IsHoldingFishRod() && IsMountingHoverBoard())
		return false;
#endif

	if (!m_kHorse.CanUseSkill())
		return false;

	if (!m_GraphicThingInstance.CanUseSkill())
		return false;

	return true;
}

bool CInstanceBase::CanAttack()
{
	if (!m_kHorse.CanAttack())
		return false;

	if (IsWearingDress())
		return false;

	if (IsHoldingPickAxe())
		return false;

#ifdef ENABLE_STANDING_MOUNT
	if (IsHoldingFishRod() && IsMountingHoverBoard())
		return false;
#endif

	return m_GraphicThingInstance.CanAttack();
}



bool CInstanceBase::CanFishing()
{
	return m_GraphicThingInstance.CanFishing();
}


BOOL CInstanceBase::IsBowMode()
{
	return m_GraphicThingInstance.IsBowMode();
}

BOOL CInstanceBase::IsHandMode()
{
	return m_GraphicThingInstance.IsHandMode();
}

BOOL CInstanceBase::IsFishingMode()
{
	if (CRaceMotionData::MODE_FISHING == m_GraphicThingInstance.GetMotionMode())
		return true;

	return false;
}

BOOL CInstanceBase::IsFishing()
{
	return m_GraphicThingInstance.IsFishing();
}

BOOL CInstanceBase::IsDead()
{
	return m_GraphicThingInstance.IsDead();
}

BOOL CInstanceBase::IsStun()
{
	return m_GraphicThingInstance.IsStun();
}

BOOL CInstanceBase::IsSleep()
{
	return m_GraphicThingInstance.IsSleep();
}


BOOL CInstanceBase::__IsSyncing()
{
	return m_GraphicThingInstance.__IsSyncing();
}



void CInstanceBase::NEW_SetOwner(DWORD dwVIDOwner)
{
	m_GraphicThingInstance.SetOwner(dwVIDOwner);
}

float CInstanceBase::GetLocalTime()
{
	return m_GraphicThingInstance.GetLocalTime();
}


void CInstanceBase::PushUDPState(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
}

DWORD	ELTimer_GetServerFrameMSec();

void CInstanceBase::PushTCPStateExpanded(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg, UINT uTargetVID)
{
	SCommand kCmdNew;
	kCmdNew.m_kPPosDst = c_rkPPosDst;
	kCmdNew.m_dwChkTime = dwCmdTime+100;
	kCmdNew.m_dwCmdTime = dwCmdTime;
	kCmdNew.m_fDstRot = fDstRot;
	kCmdNew.m_eFunc = eFunc;
	kCmdNew.m_uArg = uArg;
	kCmdNew.m_uTargetVID = uTargetVID;
	m_kQue_kCmdNew.push_back(kCmdNew);
}

void CInstanceBase::PushTCPState(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
	if (__IsMainInstance())
	{
		TraceError("CInstanceBase::PushTCPState You can't send move packets to yourself!");
		return;
	}

	int nNetworkGap=ELTimer_GetServerFrameMSec()-dwCmdTime;

	m_nAverageNetworkGap=(m_nAverageNetworkGap*70+nNetworkGap*30)/100;

	SCommand kCmdNew;
	kCmdNew.m_kPPosDst = c_rkPPosDst;
	kCmdNew.m_dwChkTime = dwCmdTime+m_nAverageNetworkGap;//m_dwBaseChkTime + (dwCmdTime - m_dwBaseCmdTime);// + nNetworkGap;
	kCmdNew.m_dwCmdTime = dwCmdTime;
	kCmdNew.m_fDstRot = fDstRot;
	kCmdNew.m_eFunc = eFunc;
	kCmdNew.m_uArg = uArg;
	m_kQue_kCmdNew.push_back(kCmdNew);
}

BOOL CInstanceBase::__CanProcessNetworkStatePacket()
{
	if (m_GraphicThingInstance.IsDead())
		return FALSE;
	if (m_GraphicThingInstance.IsKnockDown())
		return FALSE;
	if (m_GraphicThingInstance.IsUsingSkill())
		if (!m_GraphicThingInstance.CanCancelSkill())
			return FALSE;

	return TRUE;
}

BOOL CInstanceBase::__IsEnableTCPProcess(UINT eCurFunc)
{
	if (m_GraphicThingInstance.IsActEmotion())
	{
		return FALSE;
	}

	if (!m_bEnableTCPState)
	{
		if (FUNC_EMOTION != eCurFunc)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CInstanceBase::StateProcess()
{
	while (1)
	{
		if (m_kQue_kCmdNew.empty())
			return;

		DWORD dwDstChkTime = m_kQue_kCmdNew.front().m_dwChkTime;
		DWORD dwCurChkTime = ELTimer_GetServerFrameMSec();

		if (dwCurChkTime < dwDstChkTime)
			return;

		SCommand kCmdTop = m_kQue_kCmdNew.front();
		m_kQue_kCmdNew.pop_front();

		TPixelPosition kPPosDst = kCmdTop.m_kPPosDst;

		FLOAT fRotDst = kCmdTop.m_fDstRot;
		UINT eFunc = kCmdTop.m_eFunc;
		UINT uArg = kCmdTop.m_uArg;
		UINT uVID = GetVirtualID();
		UINT uTargetVID = kCmdTop.m_uTargetVID;

		TPixelPosition kPPosCur;
		NEW_GetPixelPosition(&kPPosCur);

		TPixelPosition kPPosDir = kPPosDst - kPPosCur;
		float fDirLen = (float)sqrt(kPPosDir.x * kPPosDir.x + kPPosDir.y * kPPosDir.y);

		if (!__CanProcessNetworkStatePacket())
		{
			Lognf(0, "vid=%d Skip State as unable to process IsDead=%d, IsKnockDown=%d", uVID, m_GraphicThingInstance.IsDead(), m_GraphicThingInstance.IsKnockDown());
			return;
		}

		if (!__IsEnableTCPProcess(eFunc))
		{
			return;
		}

		switch (eFunc)
		{
			case FUNC_WAIT:
			{
				if (fDirLen > 1.0f)
				{
					NEW_SetSrcPixelPosition(kPPosCur);
					NEW_SetDstPixelPosition(kPPosDst);

					__EnableSkipCollision();

					m_fDstRot = fRotDst;
					m_isGoing = TRUE;

					m_kMovAfterFunc.eFunc = FUNC_WAIT;

					if (!IsWalking())
						StartWalking();
				}
				else
				{
					m_isGoing = FALSE;

					if (!IsWaiting())
						EndWalking();

					SCRIPT_SetPixelPosition(kPPosDst.x, kPPosDst.y);
					SetAdvancingRotation(fRotDst);
					SetRotation(fRotDst);
				}
				break;
			}

			case FUNC_MOVE:
			{
				NEW_SetSrcPixelPosition(kPPosCur);
				NEW_SetDstPixelPosition(kPPosDst);
				m_fDstRot = fRotDst;
				m_isGoing = TRUE;
				__EnableSkipCollision();

				m_kMovAfterFunc.eFunc = FUNC_MOVE;

				if (!IsWalking())
				{
					StartWalking();
				}
				else
				{
				}
				break;
			}

			case FUNC_COMBO:
			{
				if (fDirLen >= 50.0f)
				{
					NEW_SetSrcPixelPosition(kPPosCur);
					NEW_SetDstPixelPosition(kPPosDst);
					m_fDstRot = fRotDst;
					m_isGoing = TRUE;
					__EnableSkipCollision();

					m_kMovAfterFunc.eFunc = FUNC_COMBO;
					m_kMovAfterFunc.uArg = uArg;

					if (!IsWalking())
						StartWalking();
				}
				else
				{
					m_isGoing = FALSE;

					if (IsWalking())
						EndWalking();

					SCRIPT_SetPixelPosition(kPPosDst.x, kPPosDst.y);
					RunComboAttack(fRotDst, uArg);
				}
				break;
			}

			case FUNC_ATTACK:
			{
				if (fDirLen >= 50.0f)
				{
					NEW_SetSrcPixelPosition(kPPosCur);
					NEW_SetDstPixelPosition(kPPosDst);
					m_fDstRot = fRotDst;
					m_isGoing = TRUE;
					__EnableSkipCollision();

					m_kMovAfterFunc.eFunc = FUNC_ATTACK;

					if (!IsWalking())
						StartWalking();
				}
				else
				{
					m_isGoing = FALSE;

					if (IsWalking())
						EndWalking();

					SCRIPT_SetPixelPosition(kPPosDst.x, kPPosDst.y);
					BlendRotation(fRotDst);

					RunNormalAttack(fRotDst);
				}
				break;
			}

			case FUNC_MOB_SKILL:
			{
				if (fDirLen >= 50.0f)
				{
					NEW_SetSrcPixelPosition(kPPosCur);
					NEW_SetDstPixelPosition(kPPosDst);
					m_fDstRot = fRotDst;
					m_isGoing = TRUE;
					__EnableSkipCollision();

					m_kMovAfterFunc.eFunc = FUNC_MOB_SKILL;
					m_kMovAfterFunc.uArg = uArg;

					if (!IsWalking())
						StartWalking();
				}
				else
				{
					m_isGoing = FALSE;

					if (IsWalking())
						EndWalking();

					SCRIPT_SetPixelPosition(kPPosDst.x, kPPosDst.y);
					BlendRotation(fRotDst);

					m_GraphicThingInstance.InterceptOnceMotion(CRaceMotionData::NAME_SPECIAL_1 + uArg);
				}
				break;
			}

			case FUNC_EMOTION:
			{
				if (fDirLen > 100.0f)
				{
					NEW_SetSrcPixelPosition(kPPosCur);
					NEW_SetDstPixelPosition(kPPosDst);
					m_fDstRot = fRotDst;
					m_isGoing = TRUE;

					if (__IsMainInstance())
						__EnableSkipCollision();

					m_kMovAfterFunc.eFunc = FUNC_EMOTION;
					m_kMovAfterFunc.uArg = uArg;
					m_kMovAfterFunc.uArgExpanded = uTargetVID;
					m_kMovAfterFunc.kPosDst = kPPosDst;

					if (!IsWalking())
						StartWalking();
				}
				else
				{
					__ProcessFunctionEmotion(uArg, uTargetVID, kPPosDst);
				}
				break;
			}

			default:
			{
				if (eFunc & FUNC_SKILL)
				{
					if (fDirLen >= 50.0f)
					{
						NEW_SetSrcPixelPosition(kPPosCur);
						NEW_SetDstPixelPosition(kPPosDst);
						m_fDstRot = fRotDst;
						m_isGoing = TRUE;

						__EnableSkipCollision();

						m_kMovAfterFunc.eFunc = eFunc;
						m_kMovAfterFunc.uArg = uArg;

						if (!IsWalking())
							StartWalking();
					}
					else
					{
						m_isGoing = FALSE;

						if (IsWalking())
							EndWalking();

						SCRIPT_SetPixelPosition(kPPosDst.x, kPPosDst.y);
						SetAdvancingRotation(fRotDst);
						SetRotation(fRotDst);

						NEW_UseSkill(0, eFunc & 0x7f, uArg & 0x0f, (uArg >> 4) ? true : false);
					}
				}
				break;
			}
		}
	}
}


void CInstanceBase::MovementProcess()
{
	TPixelPosition kPPosCur;
	NEW_GetPixelPosition(&kPPosCur);

	TPixelPosition kPPosNext;
	{
		const D3DXVECTOR3 & c_rkV3Mov = m_GraphicThingInstance.GetMovementVectorRef();

		kPPosNext.x = kPPosCur.x + (+c_rkV3Mov.x);
		kPPosNext.y = kPPosCur.y + (-c_rkV3Mov.y);
		kPPosNext.z = kPPosCur.z + (+c_rkV3Mov.z);
	}

	TPixelPosition kPPosDeltaSC = kPPosCur - NEW_GetSrcPixelPositionRef();
	TPixelPosition kPPosDeltaSN = kPPosNext - NEW_GetSrcPixelPositionRef();
	TPixelPosition kPPosDeltaSD = NEW_GetDstPixelPositionRef() - NEW_GetSrcPixelPositionRef();

	float fCurLen = sqrtf(kPPosDeltaSC.x * kPPosDeltaSC.x + kPPosDeltaSC.y * kPPosDeltaSC.y);
	float fNextLen = sqrtf(kPPosDeltaSN.x * kPPosDeltaSN.x + kPPosDeltaSN.y * kPPosDeltaSN.y);
	float fTotalLen = sqrtf(kPPosDeltaSD.x * kPPosDeltaSD.x + kPPosDeltaSD.y * kPPosDeltaSD.y);
	float fRestLen = fTotalLen - fCurLen;

	if (__IsMainInstance())
	{
		if (m_isGoing && IsWalking())
		{
			float fDstRot = NEW_GetAdvancingRotationFromPixelPosition(NEW_GetSrcPixelPositionRef(), NEW_GetDstPixelPositionRef());

			SetAdvancingRotation(fDstRot);

			if (fRestLen <= 0.0)
			{
				if (IsWalking())
					EndWalking();

				m_isGoing = FALSE;

				BlockMovement();

				if (FUNC_EMOTION == m_kMovAfterFunc.eFunc)
				{
					DWORD dwMotionNumber = m_kMovAfterFunc.uArg;
					DWORD dwTargetVID = m_kMovAfterFunc.uArgExpanded;
					__ProcessFunctionEmotion(dwMotionNumber, dwTargetVID, m_kMovAfterFunc.kPosDst);
					m_kMovAfterFunc.eFunc = FUNC_WAIT;
					return;
				}
			}
		}
	}
	else
	{
		if (m_isGoing && IsWalking())
		{
			float fDstRot = NEW_GetAdvancingRotationFromPixelPosition(NEW_GetSrcPixelPositionRef(), NEW_GetDstPixelPositionRef());

			SetAdvancingRotation(fDstRot);

			if (fRestLen < -100.0f)
			{
				NEW_SetSrcPixelPosition(kPPosCur);

				float fDstRot = NEW_GetAdvancingRotationFromPixelPosition(kPPosCur, NEW_GetDstPixelPositionRef());
				SetAdvancingRotation(fDstRot);

				if (FUNC_MOVE == m_kMovAfterFunc.eFunc)
				{
					m_kMovAfterFunc.eFunc = FUNC_WAIT;
				}
			}
			else if (fCurLen <= fTotalLen && fTotalLen <= fNextLen)
			{
				if (m_GraphicThingInstance.IsDead() || m_GraphicThingInstance.IsKnockDown())
				{
					__DisableSkipCollision();

					m_isGoing = FALSE;
				}
				else
				{
					switch (m_kMovAfterFunc.eFunc)
					{
						case FUNC_ATTACK:
						{
							if (IsWalking())
								EndWalking();

							__DisableSkipCollision();
							m_isGoing = FALSE;

							BlockMovement();
							SCRIPT_SetPixelPosition(NEW_GetDstPixelPositionRef().x, NEW_GetDstPixelPositionRef().y);
							SetAdvancingRotation(m_fDstRot);
							SetRotation(m_fDstRot);

							RunNormalAttack(m_fDstRot);
							break;
						}

						case FUNC_COMBO:
						{
							if (IsWalking())
								EndWalking();

							__DisableSkipCollision();
							m_isGoing = FALSE;

							BlockMovement();
							SCRIPT_SetPixelPosition(NEW_GetDstPixelPositionRef().x, NEW_GetDstPixelPositionRef().y);
							RunComboAttack(m_fDstRot, m_kMovAfterFunc.uArg);
							break;
						}

						case FUNC_EMOTION:
						{
							m_isGoing = FALSE;
							m_kMovAfterFunc.eFunc = FUNC_WAIT;
							__DisableSkipCollision();
							BlockMovement();

							DWORD dwMotionNumber = m_kMovAfterFunc.uArg;
							DWORD dwTargetVID = m_kMovAfterFunc.uArgExpanded;
							__ProcessFunctionEmotion(dwMotionNumber, dwTargetVID, m_kMovAfterFunc.kPosDst);
							break;
						}

						case FUNC_MOVE:
						{
							if (!IsWaiting())
								EndWalkingWithoutBlending();
							break;
						}

						case FUNC_MOB_SKILL:
						{
							if (IsWalking())
								EndWalking();

							__DisableSkipCollision();
							m_isGoing = FALSE;

							BlockMovement();
							SCRIPT_SetPixelPosition(NEW_GetDstPixelPositionRef().x, NEW_GetDstPixelPositionRef().y);
							SetAdvancingRotation(m_fDstRot);
							SetRotation(m_fDstRot);

							m_GraphicThingInstance.InterceptOnceMotion(CRaceMotionData::NAME_SPECIAL_1 + m_kMovAfterFunc.uArg);
							break;
						}

						default:
						{
							if (m_kMovAfterFunc.eFunc & FUNC_SKILL)
							{
								SetAdvancingRotation(m_fDstRot);
								BlendRotation(m_fDstRot);
								NEW_UseSkill(0, m_kMovAfterFunc.eFunc & 0x7f, m_kMovAfterFunc.uArg&0x0f, (m_kMovAfterFunc.uArg>>4) ? true : false);
							}
							else
							{
								__DisableSkipCollision();
								m_isGoing = FALSE;

								BlockMovement();
								SCRIPT_SetPixelPosition(NEW_GetDstPixelPositionRef().x, NEW_GetDstPixelPositionRef().y);
								SetAdvancingRotation(m_fDstRot);
								BlendRotation(m_fDstRot);
								if (!IsWaiting())
								{
									EndWalking();
								}
							}
							break;
						}
					}

				}
			}

		}
	}

	if (IsWalking() || m_GraphicThingInstance.IsUsingMovingSkill())
	{
		float fRotation = m_GraphicThingInstance.GetRotation();
		float fAdvancingRotation = m_GraphicThingInstance.GetAdvancingRotation();
		int iDirection = GetRotatingDirection(fRotation, fAdvancingRotation);

		if (DEGREE_DIRECTION_SAME != m_iRotatingDirection)
		{
			if (DEGREE_DIRECTION_LEFT == iDirection)
			{
				fRotation = fmodf(fRotation + m_fRotSpd*m_GraphicThingInstance.GetSecondElapsed(), 360.0f);
			}
			else if (DEGREE_DIRECTION_RIGHT == iDirection)
			{
				fRotation = fmodf(fRotation - m_fRotSpd*m_GraphicThingInstance.GetSecondElapsed() + 360.0f, 360.0f);
			}

			if (m_iRotatingDirection != GetRotatingDirection(fRotation, fAdvancingRotation))
			{
				m_iRotatingDirection = DEGREE_DIRECTION_SAME;
				fRotation = fAdvancingRotation;
			}

			m_GraphicThingInstance.SetRotation(fRotation);
		}

		if (__IsInDustRange())
		{ 
			float fDustDistance = NEW_GetDistanceFromDestPixelPosition(m_kPPosDust);
			if (IsMountingHorse())
			{
				if (fDustDistance > ms_fHorseDustGap)
				{
					NEW_GetPixelPosition(&m_kPPosDust);
					__AttachEffect(EFFECT_HORSE_DUST);
				}
			}
			else
			{
				if (fDustDistance > ms_fDustGap)
				{
					NEW_GetPixelPosition(&m_kPPosDust);
					__AttachEffect(EFFECT_DUST);
				}
			}
		}
	}
}

void CInstanceBase::__ProcessFunctionEmotion(DWORD dwMotionNumber, DWORD dwTargetVID, const TPixelPosition & c_rkPosDst)
{
	if (IsWalking())
		EndWalkingWithoutBlending();

	__EnableChangingTCPState();
	SCRIPT_SetPixelPosition(c_rkPosDst.x, c_rkPosDst.y);

	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwTargetVID);
	if (pTargetInstance)
	{
		pTargetInstance->__EnableChangingTCPState();

		if (pTargetInstance->IsWalking())
			pTargetInstance->EndWalkingWithoutBlending();

		WORD wMotionNumber1 = HIWORD(dwMotionNumber);
		WORD wMotionNumber2 = LOWORD(dwMotionNumber);

		int src_job = RaceToJob(GetRace());
		int dst_job = RaceToJob(pTargetInstance->GetRace());

		NEW_LookAtDestInstance(*pTargetInstance);
		m_GraphicThingInstance.InterceptOnceMotion(wMotionNumber1 + dst_job);
		m_GraphicThingInstance.SetRotation(m_GraphicThingInstance.GetTargetRotation());
		m_GraphicThingInstance.SetAdvancingRotation(m_GraphicThingInstance.GetTargetRotation());

		pTargetInstance->NEW_LookAtDestInstance(*this);
		pTargetInstance->m_GraphicThingInstance.InterceptOnceMotion(wMotionNumber2 + src_job);
		pTargetInstance->m_GraphicThingInstance.SetRotation(pTargetInstance->m_GraphicThingInstance.GetTargetRotation());
		pTargetInstance->m_GraphicThingInstance.SetAdvancingRotation(pTargetInstance->m_GraphicThingInstance.GetTargetRotation());

		if (pTargetInstance->__IsMainInstance())
		{
			IAbstractPlayer & rPlayer=IAbstractPlayer::GetSingleton();
			rPlayer.EndEmotionProcess();
		}
	}

	if (__IsMainInstance())
	{
		IAbstractPlayer & rPlayer=IAbstractPlayer::GetSingleton();
		rPlayer.EndEmotionProcess();
	}
}

int g_iAccumulationTime = 0;

void CInstanceBase::Update()
{
    const auto& options = CPythonSystem::Instance();
    auto* mainActor = CPythonCharacterManager::Instance().GetMainInstancePtr();
    const bool otherPlayer = IsPC() && !__IsMainInstance() && mainActor;
    const float fDistanceToMain = otherPlayer ? GetDistance(mainActor) : 0.0f;
    const bool distantPlayer = otherPlayer && fDistanceToMain > 2500.0f;
    m_bBeyondCrowdBudget = otherPlayer && fDistanceToMain >= ms_fCrowdDetailDistance;
    const bool crowdReduced = m_bBeyondCrowdBudget || (distantPlayer && options.GetAdvancedOption("high_performance_crowd_mode"));
    m_GraphicThingInstance.SetOptionLODDistanceBias(crowdReduced ? 3.0f : 1.0f);
    const bool filter = IsHiddenByOptions()
        || crowdReduced
        || options.GetAdvancedOption("hide_effects_except_damage")
        || (IsPC() && !__IsMainInstance() && options.GetAdvancedOption("hide_effects_from_others_players"))
        || (IsStone() && options.GetAdvancedOption("hide_stone_effect"))
        || (IsEnemy() && options.GetAdvancedOption("hide_monster_effect"));
    m_GraphicThingInstance.SetAttachingEffectsFiltered(filter);
    if (options.GetAdvancedOption("hide_shaman_buff"))
        for (const auto affect : {AFFECT_HOSIN, AFFECT_BOHO, AFFECT_KWAESOK, AFFECT_GICHEON, AFFECT_JEUNGRYEOK})
            if (m_adwCRCAffectEffect[affect]) CEffectManager::Instance().SetOptionFiltered(m_adwCRCAffectEffect[affect], true);

	++ms_dwUpdateCounter;

	StateProcess();
	m_GraphicThingInstance.PhysicsProcess();
	m_GraphicThingInstance.RotationProcess();
	m_GraphicThingInstance.ComboProcess();
	m_GraphicThingInstance.AccumulationMovement();

	if (m_GraphicThingInstance.IsMovement())
	{
		TPixelPosition kPPosCur;
		NEW_GetPixelPosition(&kPPosCur);

		DWORD dwCurTime=ELTimer_GetFrameMSec();

		{
			m_dwNextUpdateHeightTime=dwCurTime;
			kPPosCur.z = __GetBackgroundHeight(kPPosCur.x, kPPosCur.y);
			NEW_SetPixelPosition(kPPosCur);
		}

		{
			DWORD dwMtrlColor=__GetShadowMapColor(kPPosCur.x, kPPosCur.y);
			m_GraphicThingInstance.SetMaterialColor(dwMtrlColor);
		}
	}

	m_GraphicThingInstance.UpdateAdvancingPointInstance();

	AttackProcess();
	MovementProcess();

	m_GraphicThingInstance.MotionProcess(IsPC());

	if (IsPoly())
		__ClearArmorRefineEffect(); // Fix

#ifdef ENABLE_GRAPHIC_ON_OFF
	const int effLevel = CPythonSystem::instance().GetEffectLevel();
	// effLevel:
	// 0 or 1: Show all (Tümünü göster)
	// 2: Only mine and monsters/NPCs (Sadece benimkileri ve canavar efektlerini) -> hide other PCs
	// 3: Only mine and other players (Sadece benimkileri ve diğer oyuncuları) -> hide monsters/NPCs
	// 4: Only mine (Sadece benimkileri göster) -> hide other PCs, monsters, NPCs
	// 5: Hide all (Hiçbirini gösterme) -> hide all
	if (effLevel >= 5)
	{
		m_GraphicThingInstance.SetDeactiveAllAttachingEffect();
	}
	else if (effLevel == 4)
	{
		if (!__IsMainInstance())
			m_GraphicThingInstance.SetDeactiveAllAttachingEffect();
		else
			if (!IsInvisibility())
				m_GraphicThingInstance.SetActiveAllAttachingEffect();
	}
	else if (effLevel == 3)
	{
		if (!IsPC())
			m_GraphicThingInstance.SetDeactiveAllAttachingEffect();
		else
			if (!IsInvisibility())
				m_GraphicThingInstance.SetActiveAllAttachingEffect();
	}
	else if (effLevel == 2)
	{
		if (IsPC() && !__IsMainInstance())
			m_GraphicThingInstance.SetDeactiveAllAttachingEffect();
		else
			if (!IsInvisibility())
				m_GraphicThingInstance.SetActiveAllAttachingEffect();
	}
	else // 0 or 1 -> Show All (Tümünü göster)
	{
		if (!IsInvisibility())
			m_GraphicThingInstance.SetActiveAllAttachingEffect();
	}


#endif

	if (IsMountingHorse())
	{
		m_kHorse.m_pkActor->HORSE_MotionProcess(FALSE);
	}

	__ComboProcess();

	ProcessRemoveOldDamage();
	ProcessDamage();
}

void CInstanceBase::Transform()
{
	if (__IsSyncing())
	{
		//OnSyncing();
	}
	else
	{
		if (IsWalking() || m_GraphicThingInstance.IsUsingMovingSkill())
		{
			const D3DXVECTOR3& c_rv3Movment=m_GraphicThingInstance.GetMovementVectorRef();

			float len=(c_rv3Movment.x*c_rv3Movment.x)+(c_rv3Movment.y*c_rv3Movment.y);
			if (len>1.0f)
				OnMoving();
			else
				OnWaiting();
		}
	}

	m_GraphicThingInstance.INSTANCEBASE_Transform();
}

void CInstanceBase::Deform()
{
	if (!__CanRender())
		return;

	++ms_dwDeformCounter;

	m_GraphicThingInstance.INSTANCEBASE_Deform();

	m_kHorse.Deform();
}

void CInstanceBase::RenderTrace()
{
	if (!__CanRender())
		return;

	m_GraphicThingInstance.RenderTrace();
}

void CInstanceBase::Render()
{
	if (!__CanRender())
		return;

	++ms_dwRenderCounter;

	if (!CPythonSystem::Instance().GetAdvancedOption("hide_mount") &&
        !(CPythonSystem::Instance().GetAdvancedOption("auto_hide_companion_map1") && CPythonSystem::Instance().GetAdvancedOption("in_map1")))
		m_kHorse.Render();
	m_GraphicThingInstance.Render();
    if (m_optionQueueIndex && IsStone() && !IsDead())
    {
        struct MarkerScreen : CScreen { DWORD SavedColor() { return ms_diffuseColor; } };
        static MarkerScreen screen;
        const DWORD savedColor=screen.SavedColor();
        auto& settings=CPythonSystem::Instance();
        D3DXMATRIX identity; D3DXMatrixIdentity(&identity);
        STATEMANAGER.SaveTransform(D3DTS_WORLD,&identity);
        STATEMANAGER.SaveRenderState(D3DRS_LIGHTING,FALSE);
        STATEMANAGER.SaveTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
        STATEMANAGER.SaveTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
        STATEMANAGER.SaveTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
        LPDIRECT3DBASETEXTURE9 optionTexture0=nullptr, optionTexture1=nullptr;
        STATEMANAGER.GetDevice()->GetTexture(0,&optionTexture0);
        STATEMANAGER.GetDevice()->GetTexture(1,&optionTexture1);
        STATEMANAGER.SaveFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        LPDIRECT3DVERTEXSHADER9 vertex=nullptr;
        LPDIRECT3DPIXELSHADER9 pixel=nullptr;
        STATEMANAGER.GetDevice()->GetVertexShader(&vertex);
        STATEMANAGER.GetDevice()->GetPixelShader(&pixel);
        STATEMANAGER.SetVertexShader(nullptr); STATEMANAGER.SetPixelShader(nullptr);
        const auto& pos=m_GraphicThingInstance.GetPosition();
        screen.SetDiffuseColor(settings.GetAdvancedOption("queue_color_r",1),settings.GetAdvancedOption("queue_color_g",1),settings.GetAdvancedOption("queue_color_b",1));
        const int mode=int(settings.GetAdvancedOption("queue_effect_index"));
        screen.RenderCircle3d(pos.x,pos.y,pos.z+8,140,32);
        if (mode >= 1) screen.RenderCircle3d(pos.x,pos.y,pos.z+20,180,32);
        if (mode >= 2) screen.RenderLine3d(pos.x,pos.y,pos.z+20,pos.x,pos.y,pos.z+400);
        if (mode == 3 && m_optionQueueIndex >= 1 && m_optionQueueIndex <= 8)
        {
            // Seven-segment world marker for the numbered reference option.
            static const unsigned char digits[] = {0, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f};
            const unsigned char segments = digits[m_optionQueueIndex];
            const float x = pos.x, y = pos.y, z = pos.z + 410.0f;
            const float left = -45.0f, right = 45.0f, top = -65.0f, mid = 0.0f, bottom = 65.0f;
            if (segments & 0x01) screen.RenderLine3d(x+left,y+top,z,x+right,y+top,z);
            if (segments & 0x02) screen.RenderLine3d(x+right,y+top,z,x+right,y+mid,z);
            if (segments & 0x04) screen.RenderLine3d(x+right,y+mid,z,x+right,y+bottom,z);
            if (segments & 0x08) screen.RenderLine3d(x+left,y+bottom,z,x+right,y+bottom,z);
            if (segments & 0x10) screen.RenderLine3d(x+left,y+mid,z,x+left,y+bottom,z);
            if (segments & 0x20) screen.RenderLine3d(x+left,y+top,z,x+left,y+mid,z);
            if (segments & 0x40) screen.RenderLine3d(x+left,y+mid,z,x+right,y+mid,z);
        }
        screen.SetDiffuseColor(savedColor);
        STATEMANAGER.RestoreFVF();
        STATEMANAGER.SetTexture(0,optionTexture0); STATEMANAGER.SetTexture(1,optionTexture1);
        if (optionTexture0) optionTexture0->Release(); if (optionTexture1) optionTexture1->Release();
        STATEMANAGER.SetVertexShader(vertex); STATEMANAGER.SetPixelShader(pixel);
        if (vertex) vertex->Release(); if (pixel) pixel->Release();
        STATEMANAGER.RestoreTransform(D3DTS_WORLD);
        STATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);
        STATEMANAGER.RestoreTextureStageState(0,D3DTSS_COLORARG1);
        STATEMANAGER.RestoreTextureStageState(0,D3DTSS_COLOROP);
        STATEMANAGER.RestoreTextureStageState(0,D3DTSS_ALPHAOP);
    }

	if (CActorInstance::IsDirLine())
	{
		if (NEW_GetDstPixelPositionRef().x != 0.0f)
		{
			static CScreen s_kScreen;

			STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			STATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
			STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
			STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

			TPixelPosition px;
			m_GraphicThingInstance.GetPixelPosition(&px);
			D3DXVECTOR3 kD3DVt3Cur(px.x, px.y, px.z);
			D3DXVECTOR3 kD3DVt3Dest(NEW_GetDstPixelPositionRef().x, -NEW_GetDstPixelPositionRef().y, NEW_GetDstPixelPositionRef().z);

			s_kScreen.SetDiffuseColor(0.0f, 0.0f, 1.0f);
			s_kScreen.RenderLine3d(kD3DVt3Cur.x, kD3DVt3Cur.y, px.z, kD3DVt3Dest.x, kD3DVt3Dest.y, px.z);
			STATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);
			STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, TRUE);
			STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);
		}
	}
}

void CInstanceBase::RenderToShadowMap()
{
	if (IsDoor())
		return;

	if (IsBuilding())
		return;

	if (!__CanRender())
		return;

	if (!__IsExistMainInstance())
		return;

	CInstanceBase* pkInstMain=__GetMainInstancePtr();

	const float SHADOW_APPLY_DISTANCE = 2500.0f;

	float fDistance=NEW_GetDistanceFromDestInstance(*pkInstMain);
	if (fDistance>=SHADOW_APPLY_DISTANCE)
		return;

	m_GraphicThingInstance.RenderToShadowMap();	
}

void CInstanceBase::RenderCollision()
{
	m_GraphicThingInstance.RenderCollisionData();
}

void CInstanceBase::SetVirtualID(DWORD dwVirtualID)
{
	m_GraphicThingInstance.SetVirtualID(dwVirtualID);
}

void CInstanceBase::SetVirtualNumber(DWORD dwVirtualNumber)
{
	m_dwVirtualNumber = dwVirtualNumber;
}

void CInstanceBase::SetInstanceType(int iInstanceType)
{
	m_GraphicThingInstance.SetActorType(iInstanceType);
}

#ifdef ENABLE_CONQUEROR_LEVEL
void CInstanceBase::SetConquerorLevelText(int sLevel)
{
	m_dwConquerorLevel = sLevel;
	UpdateTextTailConquerorLevel(sLevel);
}
#endif

void CInstanceBase::SetAlignment(short sAlignment)
{
	m_sAlignment = sAlignment;
	RefreshTextTailTitle();
}

void CInstanceBase::SetPKMode(BYTE byPKMode)
{
	if (m_byPKMode == byPKMode)
	{
		return;
	}

	m_byPKMode = byPKMode;

	if (__IsMainInstance())
	{
		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		rPlayer.NotifyChangePKMode();
	}
}

void CInstanceBase::SetKiller(bool bFlag)
{
	if (m_isKiller == bFlag)
	{
		return;
	}

	m_isKiller = bFlag;
	RefreshTextTail();
}

void CInstanceBase::SetPartyMemberFlag(bool bFlag)
{
	m_isPartyMember = bFlag;
}

void CInstanceBase::SetStateFlags(DWORD dwStateFlags)
{
	if (dwStateFlags & ADD_CHARACTER_STATE_KILLER)
	{
		SetKiller (TRUE);
	}
	else
	{
		SetKiller (FALSE);
	}

	if (dwStateFlags & ADD_CHARACTER_STATE_PARTY)
	{
		SetPartyMemberFlag (TRUE);
	}
	else
	{
		SetPartyMemberFlag (FALSE);
	}
}

void CInstanceBase::SetComboType(UINT uComboType)
{
	m_GraphicThingInstance.SetComboType(uComboType);
}

const char *CInstanceBase::GetNameString()
{
	return m_stName.c_str();
}

void CInstanceBase::SetLevel(DWORD dwLevel)
{
	m_dwLevel = dwLevel;
}

#ifdef ENABLE_GROWTH_PET_SYSTEM
void CInstanceBase::SetPetLevel(DWORD dwLevel)
{
	m_dwLevel = dwLevel;
	UpdateTextTailLevel(dwLevel);

	float fScale = m_dwLevel * 0.20f + (0.10f + 0.10f * m_bCharacterSize);
	m_GraphicThingInstance.SetScale(fScale, fScale, fScale, true);
}
#endif

DWORD CInstanceBase::GetRace()
{
	return m_dwRace;
}

DWORD CInstanceBase::GetLevel()
{
	return m_dwLevel;
}

bool CInstanceBase::IsConflictAlignmentInstance(CInstanceBase& rkInstVictim)
{
	if (PK_MODE_PROTECT == rkInstVictim.GetPKMode())
		return false;

	switch (GetAlignmentType())
	{
		case ALIGNMENT_TYPE_NORMAL:
		case ALIGNMENT_TYPE_WHITE:
			if (ALIGNMENT_TYPE_DARK == rkInstVictim.GetAlignmentType())
				return true;
			break;
		case ALIGNMENT_TYPE_DARK:
			if (GetAlignmentType() != rkInstVictim.GetAlignmentType())
				return true;
			break;
	}

	return false;
}

void CInstanceBase::SetDuelMode(DWORD type)
{
	m_dwDuelMode = type;
}

DWORD CInstanceBase::GetDuelMode()
{
	return m_dwDuelMode;
}

bool CInstanceBase::IsAttackableInstance(CInstanceBase& rkInstVictim)
{	
	if (__IsMainInstance())
	{		
		CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
		if(rkPlayer.IsObserverMode())
			return false;
	}

	if (GetVirtualID() == rkInstVictim.GetVirtualID())
		return false;

	if (IsStone())
	{
		if (rkInstVictim.IsPC())
			return true;
	}
	else if (IsPC())
	{
		if (rkInstVictim.IsStone())
			return true;

		if (rkInstVictim.IsPC())
		{
#ifdef ENABLE_ULTIMATE_REGEN
			CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
			if (rkPlayer.CheckBossSafeRange())
				return false;
#endif
			if (GetDuelMode())
			{
				switch(GetDuelMode())
				{
				case DUEL_CANNOTATTACK:
					return false;
				case DUEL_START:
					if(__FindDUELKey(GetVirtualID(),rkInstVictim.GetVirtualID()))
						return true;
					else
						return false;
				}
			}

			if (PK_MODE_GUILD == GetPKMode())
				if (GetGuildID() == rkInstVictim.GetGuildID())
					return false;

			if (rkInstVictim.IsKiller())
				if (!IAbstractPlayer::GetSingleton().IsSamePartyMember(GetVirtualID(), rkInstVictim.GetVirtualID()))
					return true;

			if (PK_MODE_PROTECT != GetPKMode())
			{
				if (PK_MODE_FREE == GetPKMode())
				{
					if (PK_MODE_PROTECT != rkInstVictim.GetPKMode())
						if (!IAbstractPlayer::GetSingleton().IsSamePartyMember(GetVirtualID(), rkInstVictim.GetVirtualID()))
							return true;
				}
				if (PK_MODE_GUILD == GetPKMode())
				{
					if (PK_MODE_PROTECT != rkInstVictim.GetPKMode())
						if (!IAbstractPlayer::GetSingleton().IsSamePartyMember(GetVirtualID(), rkInstVictim.GetVirtualID()))
							if (GetGuildID() != rkInstVictim.GetGuildID())
								return true;
				}
			}

			if (IsSameEmpire(rkInstVictim))
			{
				if (IsPVPInstance(rkInstVictim))
					return true;

				if (PK_MODE_REVENGE == GetPKMode())
					if (!IAbstractPlayer::GetSingleton().IsSamePartyMember(GetVirtualID(), rkInstVictim.GetVirtualID()))
						if (IsConflictAlignmentInstance(rkInstVictim))
							return true;
			}
			else
			{
				return true;
			}
		}

		if (rkInstVictim.IsEnemy())
			return true;

		if (rkInstVictim.IsWoodenDoor())
			return true;
	}
	else if (IsEnemy())
	{
		if (rkInstVictim.IsPC())
			return true;

		if (rkInstVictim.IsBuilding())
			return true;

	}
	else if (IsPoly())
	{
		if (rkInstVictim.IsPC())
			return true;

		if (rkInstVictim.IsEnemy())
			return true;
	}
	return false;
}

bool CInstanceBase::IsTargetableInstance(CInstanceBase& rkInstVictim)
{
	return rkInstVictim.CanPickInstance();
}

bool CInstanceBase::CanChangeTarget()
{
	return m_GraphicThingInstance.CanChangeTarget();
}

bool CInstanceBase::CanPickInstance()
{
	if (!__IsInViewFrustum())
		return false;

	if (IsDoor())
	{
		if (IsDead())
			return false;
	}

	if (IsPC())
	{
		if (IsAffect(AFFECT_EUNHYEONG))
		{
			if (!__MainCanSeeHiddenThing())
				return false;
		}

#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
		if (IsAffect(AFFECT_REVIVE_INVISIBILITY) && !__MainCanSeeHiddenThing())
			return false;
#else
		if (IsAffect(AFFECT_REVIVE_INVISIBILITY))
			return false;
#endif

#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
		if (IsAffect(AFFECT_INVISIBILITY) && !__MainCanSeeHiddenThing())
			return false;
#else
		if (IsAffect(AFFECT_INVISIBILITY))
			return false;
#endif
	}

	if (IsDead())
		return false;

	return true;
}

bool CInstanceBase::CanViewTargetHP(CInstanceBase& rkInstVictim)
{
	if (rkInstVictim.IsStone())
		return true;
	if (rkInstVictim.IsWoodenDoor())
		return true;
	if (rkInstVictim.IsEnemy())
		return true;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
	if (rkInstVictim.IsPC())
		return true;
#endif
#ifdef ENABLE_DEFENSAWESHIP
	if (rkInstVictim.IsHydraNPC())
		return true;
#endif

	return false;
}

BOOL CInstanceBase::IsPoly()
{
	return m_GraphicThingInstance.IsPoly();
}

#ifdef ENABLE_RENEWAL_OFFLINESHOP
BOOL CInstanceBase::IsShop()
{
	return m_GraphicThingInstance.IsShop();
}
#endif

BOOL CInstanceBase::IsPC()
{
	return m_GraphicThingInstance.IsPC();
}

BOOL CInstanceBase::IsNPC()
{
	return m_GraphicThingInstance.IsNPC();
}

BOOL CInstanceBase::IsEnemy()
{
	return m_GraphicThingInstance.IsEnemy();
}

BOOL CInstanceBase::IsStone()
{
	return m_GraphicThingInstance.IsStone();
}

BOOL CInstanceBase::IsHorse()
{
	return m_GraphicThingInstance.IsHorse();
}

BOOL CInstanceBase::IsPet()
{
	return m_GraphicThingInstance.IsPet();
}

#ifdef ENABLE_GROWTH_PET_SYSTEM
BOOL CInstanceBase::IsGrowthPet()
{
	return m_GraphicThingInstance.IsGrowthPet();
}
#endif

BOOL CInstanceBase::IsMount()
{
	return m_GraphicThingInstance.IsMount();
}

BOOL CInstanceBase::IsGuildWall()
{
	return IsWall(m_dwRace);
}

BOOL CInstanceBase::IsResource()
{
	switch (m_dwVirtualNumber)
	{
		case 20047:
		case 20048:
		case 20049:
		case 20050:
		case 20051:
		case 20052:
		case 20053:
		case 20054:
		case 20055:
		case 20056:
		case 20057:
		case 20058:
		case 20059:
		case 30301:
		case 30302:
		case 30303:
		case 30304:
		case 30305:
			return TRUE;
	}

	return FALSE;
}

BOOL CInstanceBase::IsWarp()
{
	return m_GraphicThingInstance.IsWarp();
}

BOOL CInstanceBase::IsGoto()
{
	return m_GraphicThingInstance.IsGoto();
}

BOOL CInstanceBase::IsObject()
{
	return m_GraphicThingInstance.IsObject();
}

BOOL CInstanceBase::IsBuilding()
{
	return m_GraphicThingInstance.IsBuilding();
}

BOOL CInstanceBase::IsDoor()
{
	return m_GraphicThingInstance.IsDoor();
}

BOOL CInstanceBase::IsWoodenDoor()
{
	if (m_GraphicThingInstance.IsDoor())
	{
		int vnum = GetVirtualNumber();
		if (vnum == 13000)
			return true;
		else if (vnum >= 30111 && vnum <= 30119)
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}

BOOL CInstanceBase::IsStoneDoor()
{
	return m_GraphicThingInstance.IsDoor() && 13001 == GetVirtualNumber();
}

#ifdef NAMECOLOR_BOSS_CLIENT
BOOL CInstanceBase::IsBoss()
{
    if (GetRace() == 691)
        return TRUE;
    if (GetRace() == 692)
        return TRUE;
    if (GetRace() == 693)
        return TRUE;
    if (GetRace() == 791)
        return TRUE;
    if (GetRace() == 991)
        return TRUE;
    if (GetRace() == 992)
        return TRUE;
    if (GetRace() == 993)
        return TRUE;
    if (GetRace() == 1091)
        return TRUE;
    if (GetRace() == 1092)
        return TRUE;
    if (GetRace() == 1093)
        return TRUE;
    if (GetRace() == 1094)
        return TRUE;
    if (GetRace() == 1095)
        return TRUE;
    if (GetRace() == 2191)
        return TRUE;
    if (GetRace() == 1191)
        return TRUE;
    if (GetRace() == 1192)
        return TRUE;
    if (GetRace() == 1304)
        return TRUE;
    if (GetRace() == 1306)
        return TRUE;
    if (GetRace() == 1307)
        return TRUE;
    if (GetRace() == 1901)
        return TRUE;
    if (GetRace() == 1902)
        return TRUE;
    if (GetRace() == 1903)
        return TRUE;
    if (GetRace() == 2206)
        return TRUE;
    if (GetRace() == 2207)
        return TRUE;
    if (GetRace() == 2291)
        return TRUE;
    if (GetRace() == 2306)
        return TRUE;
    if (GetRace() == 2307)
        return TRUE;
    if (GetRace() == 2492)
        return TRUE;
    if (GetRace() == 2493)
        return TRUE;
    if (GetRace() == 2494)
        return TRUE;
    if (GetRace() == 2598)
        return TRUE;
    if (GetRace() == 3090)
        return TRUE;
    if (GetRace() == 3091)
        return TRUE;
    if (GetRace() == 3190)
        return TRUE;
    if (GetRace() == 3191)
        return TRUE;
    if (GetRace() == 3290)
        return TRUE;
    if (GetRace() == 3291)
        return TRUE;
    if (GetRace() == 3390)
        return TRUE;
    if (GetRace() == 3391)
        return TRUE;
    if (GetRace() == 3490)
        return TRUE;
    if (GetRace() == 3491)
        return TRUE;
    if (GetRace() == 3590)
        return TRUE;
    if (GetRace() == 3591)
        return TRUE;
    if (GetRace() == 3690)
        return TRUE;
    if (GetRace() == 3691)
        return TRUE;
    if (GetRace() == 3790)
        return TRUE;
    if (GetRace() == 3791)
        return TRUE;
    if (GetRace() == 3890)
        return TRUE;
    if (GetRace() == 3891)
        return TRUE;
    if (GetRace() == 5001)
        return TRUE;
    if (GetRace() == 5004)
        return TRUE;
    if (GetRace() == 5002)
        return TRUE;
    if (GetRace() == 5161)
        return TRUE;
    if (GetRace() == 5162)
        return TRUE;
    if (GetRace() == 5163)
        return TRUE;
    if (GetRace() == 6091)
        return TRUE;
    if (GetRace() == 6191)
        return TRUE;
    return FALSE;
}
#endif

BOOL CInstanceBase::IsFlag()
{
	if (GetRace() == 20035)
		return TRUE;
	if (GetRace() == 20036)
		return TRUE;
	if (GetRace() == 20037)
		return TRUE;

	return FALSE;
}

BOOL CInstanceBase::IsForceVisible()
{
	if (IsAffect(AFFECT_SHOW_ALWAYS))
		return TRUE;

	if (IsObject() || IsBuilding() || IsDoor() )
		return TRUE;

	return FALSE;
}

#ifdef ENABLE_EVENT_BANNER_FLAG
BOOL CInstanceBase::IsBannerFlag()
{
	if (GetRace() >= 20127 && GetRace() <= 20143)
		return TRUE;

	return FALSE;
}
#endif

int	CInstanceBase::GetInstanceType()
{
	return m_GraphicThingInstance.GetActorType();
}

DWORD CInstanceBase::GetVirtualID()
{
	return m_GraphicThingInstance.GetVirtualID();
}

DWORD CInstanceBase::GetVirtualNumber()
{
	return m_dwVirtualNumber;
}

bool CInstanceBase::__IsInViewFrustum()
{
	return m_GraphicThingInstance.isShow();
}

bool CInstanceBase::IsHiddenByOptions()
{
    if (__IsMainInstance()) return false;
    auto& options = CPythonSystem::Instance();
    const bool map1 = options.GetAdvancedOption("auto_hide_companion_map1") && options.GetAdvancedOption("in_map1");
    bool pet = IsPet();
#ifdef ENABLE_GROWTH_PET_SYSTEM
    pet = pet || IsGrowthPet();
#endif
    // Buffi support shaman (NPC 34900 female / 34901 male): own "hide_buffi" option, not the pet one.
    const DWORD vnum = GetVirtualNumber();
    if (vnum == 34900 || vnum == 34901)
        return map1 || options.GetAdvancedOption("hide_buffi") != 0.0f;
#ifdef ENABLE_GRAPHIC_ON_OFF
    if ((pet || IsMount()) && options.IsPetStatus()) return true;
#endif
    if (pet && (map1 || options.GetAdvancedOption("hide_pet"))) return true;
    if (IsMount() && (map1 || options.GetAdvancedOption("hide_mount"))) return true;
    if (IsShop())
    {
        if (map1) return true;
#ifdef ENABLE_GRAPHIC_ON_OFF
        if (options.GetPrivateShopLevel() >= 3) return true;
#endif
        auto* mainActor = CPythonCharacterManager::Instance().GetMainInstancePtr();
        const float range = options.GetAdvancedOption("shop_range_pos", 1.0f);
        if (range < 1.0f && mainActor && GetDistance(mainActor) > 500.0f + 9500.0f * range) return true;
    }
    return false;
}

bool CInstanceBase::__CanRender()
{
    if (IsHiddenByOptions()) return false;
	if (!__IsInViewFrustum())
		return false;

#ifdef ENABLE_RENDER_TARGET
	if (IsAlwaysRender())
		return true;
#endif

#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
	if (IsAffect(AFFECT_INVISIBILITY) && !__MainCanSeeHiddenThing())
		return false;
#else
	if (IsAffect(AFFECT_INVISIBILITY))
		return false;
#endif

	return true;
}

bool CInstanceBase::IntersectBoundingBox()
{
	float u, v, t;
	return m_GraphicThingInstance.Intersect(&u, &v, &t);
}

bool CInstanceBase::IntersectDefendingSphere()
{
	return m_GraphicThingInstance.IntersectDefendingSphere();
}

float CInstanceBase::GetDistance(CInstanceBase * pkTargetInst)
{
	TPixelPosition TargetPixelPosition;
	pkTargetInst->m_GraphicThingInstance.GetPixelPosition(&TargetPixelPosition);
	return GetDistance(TargetPixelPosition);
}

float CInstanceBase::GetDistance(const TPixelPosition & c_rPixelPosition)
{
	TPixelPosition PixelPosition;
	m_GraphicThingInstance.GetPixelPosition(&PixelPosition);

	float fdx = PixelPosition.x - c_rPixelPosition.x;
	float fdy = PixelPosition.y - c_rPixelPosition.y;

	return sqrtf((fdx*fdx) + (fdy*fdy));
}

CActorInstance& CInstanceBase::GetGraphicThingInstanceRef()
{
	return m_GraphicThingInstance;
}

CActorInstance* CInstanceBase::GetGraphicThingInstancePtr()
{
	return &m_GraphicThingInstance;
}

void CInstanceBase::RefreshActorInstance()
{
	m_GraphicThingInstance.RefreshActorInstance();
}

void CInstanceBase::Refresh(DWORD dwMotIndex, bool isLoop)
{
	RefreshState(dwMotIndex, isLoop);
}

void CInstanceBase::RestoreRenderMode()
{
	m_GraphicThingInstance.RestoreRenderMode();
}

void CInstanceBase::SetAddRenderMode()
{
	m_GraphicThingInstance.SetAddRenderMode();
}

void CInstanceBase::SetModulateRenderMode()
{
	m_GraphicThingInstance.SetModulateRenderMode();
}

void CInstanceBase::SetRenderMode(int iRenderMode)
{
	m_GraphicThingInstance.SetRenderMode(iRenderMode);
}

void CInstanceBase::SetAddColor(const D3DXCOLOR & c_rColor)
{
	m_GraphicThingInstance.SetAddColor(c_rColor);
}

void CInstanceBase::__SetBlendRenderingMode()
{
	m_GraphicThingInstance.SetBlendRenderMode();
}

void CInstanceBase::__SetAlphaValue(float fAlpha)
{
	m_GraphicThingInstance.SetAlphaValue(fAlpha);
}

float CInstanceBase::__GetAlphaValue()
{
	return m_GraphicThingInstance.GetAlphaValue();
}

void CInstanceBase::SetHair(DWORD eHair)
{
	if (!HAIR_COLOR_ENABLE)
		return;

#ifdef ENABLE_NPC_WEAR_ITEM
	if (IsPC() == false && GetRace() > 7)
#else
	if (IsPC() == false)
#endif
		return;

	m_awPart[CRaceData::PART_HAIR] = eHair;

#ifdef ENABLE_HAIR_SPECULAR
	float fSpecularPower = 0.0f;

	static std::unordered_map<DWORD, float> setAllowedHair = {
		{5057, 50.0f}, // Azrael's Helmet
	};

	auto it = setAllowedHair.find(eHair);
	if (it != setAllowedHair.end())
		fSpecularPower = it->second;
#endif

	m_GraphicThingInstance.SetHair(eHair
#ifdef ENABLE_HAIR_SPECULAR
		, fSpecularPower
#endif
	);
}

void CInstanceBase::ChangeHair(DWORD eHair)
{
	if (!HAIR_COLOR_ENABLE)
		return;

#ifdef ENABLE_BUFFI_SYSTEM
	// the Buffi (34900/34901) changes its hair with its costume
	if (IsPC() == false && GetVirtualNumber() != 34900 && GetVirtualNumber() != 34901)
#else
	if (IsPC() == false)
#endif
		return;

	if (GetPart(CRaceData::PART_HAIR) == eHair)
		return;

	SetHair(eHair);

	RefreshState(CRaceMotionData::NAME_WAIT, true);
}

void CInstanceBase::SetArmor(DWORD dwArmor)
{
	DWORD dwShape;
	if (__ArmorVnumToShape(dwArmor, &dwShape))
	{
		CItemData * pItemData;
		if (CItemManager::Instance().GetItemDataPointer(dwArmor, &pItemData))
		{
			float fSpecularPower = pItemData->GetSpecularPowerf();
			SetShape(dwShape, fSpecularPower);
			__GetRefinedEffect(pItemData);
			return;
		}
		else
			__ClearArmorRefineEffect();
	}

	SetShape(dwArmor);
}

#ifdef ENABLE_RENDER_TARGET
DWORD CInstanceBase::GetArmor()
{
	return GetPart(CRaceData::PART_MAIN);
}

DWORD CInstanceBase::GetHair()
{
	return GetPart(CRaceData::PART_HAIR);
}

DWORD CInstanceBase::GetWeapon()
{
	return GetPart(CRaceData::PART_WEAPON);
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
DWORD CInstanceBase::GetAcce()
{
	return GetPart(CRaceData::PART_ACCE);
}
#endif
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
bool CInstanceBase::SetAcce(DWORD dwAcce)
{
#ifdef ENABLE_NPC_WEAR_ITEM
	if ((IsPC() && GetRace() > 7) || IsPoly() || IsWearingDress() || __IsShapeAnimalWear())
#else
	if (!IsPC() || IsPoly() || IsWearingDress() || __IsShapeAnimalWear())
#endif
		return false;

	ClearAcceEffect();

	float fSpecular = 65.0f;

	dwAcce += 85000;
	if (dwAcce > 87000)
	{
		dwAcce -= 2000;
		fSpecular += 35;

		m_dwAcceEffect = EFFECT_REFINED + EFFECT_ACCE;
		__EffectContainer_AttachEffect(m_dwAcceEffect);
	}
	fSpecular /= 100.0f;

	m_awPart[CRaceData::PART_ACCE] = dwAcce;

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwAcce, &pItemData))
	{
		ClearAcceEffect();

		m_GraphicThingInstance.SetAcceScale(1.0f, 1.0f, 1.0f);
		m_awPart[CRaceData::PART_ACCE] = 0;
		m_GraphicThingInstance.AttachAcce(nullptr, 0.0f);
		return true;
	}

	m_GraphicThingInstance.AttachAcce(pItemData, fSpecular);

	BYTE bRace = (BYTE)GetRace();
	BYTE bJob = (BYTE)RaceToJob(bRace);
	BYTE bSex = (BYTE)RaceToSex(bRace);

	m_GraphicThingInstance.SetAcceScale(pItemData->GetItemScaleVector(bJob, bSex), bRace);
	m_awPart[CRaceData::PART_ACCE] = dwAcce;

	return true;
}

void CInstanceBase::ChangeAcce(DWORD dwAcce)
{
#ifdef ENABLE_BUFFI_SYSTEM
	if (!IsPC() && GetVirtualNumber() != 34900 && GetVirtualNumber() != 34901)
#else
	if (!IsPC())
#endif
		return;

	SetAcce(dwAcce);
}

void CInstanceBase::ClearAcceEffect()
{
	if (!m_dwAcceEffect)
		return;

	__EffectContainer_DetachEffect(m_dwAcceEffect);
	m_dwAcceEffect = 0;
}
#endif

void CInstanceBase::SetShape(DWORD eShape, float fSpecular)
{
	if (IsPoly())
	{
		m_GraphicThingInstance.SetShape(0);
	}
	else
	{
		m_GraphicThingInstance.SetShape(eShape, fSpecular);
	}

	m_eShape = eShape;
}

DWORD CInstanceBase::GetWeaponType()
{
	DWORD dwWeapon = GetPart(CRaceData::PART_WEAPON);
	CItemData * pItemData;

	if (!CItemManager::Instance().GetItemDataPointer(dwWeapon, &pItemData))
		return CItemData::WEAPON_NONE;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pItemData->GetType() == CItemData::ITEM_TYPE_COSTUME)
		return pItemData->GetValue(3);
#endif

	return pItemData->GetWeaponType();
}

void CInstanceBase::__ClearWeaponRefineEffect()
{
	if (m_swordRefineEffectRight)
	{
		__DetachEffect(m_swordRefineEffectRight);
		m_swordRefineEffectRight = 0;
	}
	if (m_swordRefineEffectLeft)
	{
		__DetachEffect(m_swordRefineEffectLeft);
		m_swordRefineEffectLeft = 0;
	}

#ifdef ENABLE_MDE_EFFECT
	if (m_dwSpecialEffect)
	{
		__DetachEffect(m_dwSpecialEffect);
		__DetachEffect(m_dwSpecialEffectLeft);

		m_dwSpecialEffect = 0;
		m_dwSpecialEffectLeft = 0;
	}
#endif
}

void CInstanceBase::__ClearArmorRefineEffect()
{
	if (m_armorRefineEffect[0])
	{
		__DetachEffect(m_armorRefineEffect[0]);
		m_armorRefineEffect[0] = 0;
	}

	if (m_armorRefineEffect[1])
	{
		__DetachEffect(m_armorRefineEffect[1]);
		m_armorRefineEffect[1] = 0;
	}
}

UINT CInstanceBase::__GetRefinedEffect(CItemData* pItem)
{
	DWORD refine = max(pItem->GetRefine() + pItem->GetSocketCount(),CItemData::ITEM_SOCKET_MAX_NUM) - CItemData::ITEM_SOCKET_MAX_NUM;
	DWORD vnum = pItem->GetIndex();

	switch (pItem->GetType())
	{
		case CItemData::ITEM_TYPE_WEAPON:
		{
			__ClearWeaponRefineEffect();

			bool enable_refine = true;

			switch (pItem->GetSubType())
			{
				case CItemData::WEAPON_SWORD:
				case CItemData::WEAPON_TWO_HANDED:
				{
					bool default_effect = true;

#ifdef ENABLE_LVL96_WEAPON_EFFECT
					if ((330 <= vnum && vnum <= 335) ||		// Kara ejderha kılıcı
						(350 <= vnum && vnum <= 355) ||		// Kara ejderha jileti
						(3240 <= vnum && vnum <= 3245) ||	// Kara ejderha bıçağı
						(370 <= vnum && vnum <= 375) ||		// Yılan kılıcı
						(390 <= vnum && vnum <= 395) ||		// Yılan bıçağı
						(3260 <= vnum && vnum <= 3265))		// Yılan büyük kılıcı
					{
						default_effect = false;
						enable_refine = false;
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SWORD_REFINED7TH;
					}
#endif

					if (default_effect)
					{
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SWORD_REFINED7 + refine - 7;
					}

					break;
				}

				case CItemData::WEAPON_DAGGER:
				{
					bool default_effect = true;

#ifdef ENABLE_LVL96_WEAPON_EFFECT
					if ((1200 <= vnum && vnum <= 1205) ||	// Kara ejderha hançeri
						(1220 <= vnum && vnum <= 1225))		// Yılan hançeri
					{
						default_effect = false;
						enable_refine = false;
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SMALLSWORD_REFINED7TH;
						m_swordRefineEffectLeft = EFFECT_REFINED + EFFECT_SMALLSWORD_REFINED7TH_LEFT;
					}
#endif

					if (default_effect)
					{
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SMALLSWORD_REFINED7 + refine - 7;
						m_swordRefineEffectLeft = EFFECT_REFINED + EFFECT_SMALLSWORD_REFINED7_LEFT + refine - 7;
					}

					break;
				}//WEAPON_DAGGER

				case CItemData::WEAPON_BOW:
				{
					bool default_effect = true;

#ifdef ENABLE_LVL96_WEAPON_EFFECT
					if ((2220 <= vnum && vnum <= 2225) ||	// Kara ejderha yayı
						(2240 <= vnum && vnum <= 2245))		// Yılan yayı
					{
						default_effect = false;
						enable_refine = false;
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_BOW_REFINED7TH;
					}
#endif

					if (default_effect)
					{
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_BOW_REFINED7 + refine - 7;
					}

					break;
				}//WEAPON_BOW

				case CItemData::WEAPON_BELL:
				case CItemData::WEAPON_ARROW:
				{
					bool default_effect = true;

#ifdef ENABLE_LVL96_WEAPON_EFFECT
					if ((5180 <= vnum && vnum <= 5185) ||	// Kara ejderha çanı
						(5210 <= vnum && vnum <= 5215))		// Yılan çanı
					{
						default_effect = false;
						enable_refine = false;
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SMALLSWORD_REFINED7TH;
					}
#endif

					if (default_effect)
					{
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SMALLSWORD_REFINED7 + refine - 7;
					}

					break;
				}//WEAPON_TWO_HANDED | WEAPON_BELL

				case CItemData::WEAPON_FAN:
				{
					bool default_effect = true;

#ifdef ENABLE_LVL96_WEAPON_EFFECT
					if ((7320 <= vnum && vnum <= 7325) ||	// Kara ejder. yelpazesi
						(7340 <= vnum && vnum <= 7345))		// Yılan yelpazesi
					{
						default_effect = false;
						enable_refine = false;
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_FANBELL_REFINED7TH;
					}
#endif

					if (default_effect)
					{
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_FANBELL_REFINED7 + refine - 7;
					}

					break;
				}//WEAPON_FAN

#ifdef ENABLE_WOLFMAN_CHARACTER
				case CItemData::WEAPON_CLAW:	//8
				{
					bool default_effect = true;

#ifdef ENABLE_LVL96_WEAPON_EFFECT
					if ((6140 <= vnum && vnum <= 6145) ||	// Dunkeldrachenkralle
						(6160 <= vnum && vnum <= 6165))		// Schlangenkralle
					{
						default_effect = false;
						enable_refine = false;
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SWORD_REFINED7TH_W;
						m_swordRefineEffectLeft = EFFECT_REFINED + EFFECT_SWORD_REFINED7TH_W_LEFT;
					}
#endif

					if (default_effect)
					{
						m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SWORD_REFINED7_W + refine - 7;
						m_swordRefineEffectLeft = EFFECT_REFINED + EFFECT_SWORD_REFINED7_W_LEFT + refine - 7;
					}

					break;
				}//WEAPON_CLAW
#endif

				default:
					TraceError("Weapon set Default!");
					m_swordRefineEffectRight = EFFECT_REFINED + EFFECT_SWORD_REFINED7 + refine - 7;
					break;
			}

			if ((refine < 7) && enable_refine)
				return 0;

			if (m_swordRefineEffectRight)
				m_swordRefineEffectRight = __AttachEffect(m_swordRefineEffectRight);
			if (m_swordRefineEffectLeft)
				m_swordRefineEffectLeft = __AttachEffect(m_swordRefineEffectLeft);

			break;
		}//END ITEM_TYPE_WEAPON

		case CItemData::ITEM_TYPE_ARMOR:
		{
			__ClearArmorRefineEffect();

			if (pItem->GetSubType() == CItemData::ARMOR_BODY)
			{
				bool default_effect = true;
				bool enable_refine = true;

	/*----------Kabarcık-Zırh----------*/
				if ((12010 <= vnum && vnum <= 12019) ||	// Mavi Plaka Zırhı
					(12020 <= vnum && vnum <= 12029) ||	// Mavi Ejderha Suiti
					(12030 <= vnum && vnum <= 12039) ||	// Aura Taş Zırhı
					(12040 <= vnum && vnum <= 12049)	// Ejderha Elbisesi
#ifdef ENABLE_WOLFMAN_CHARACTER
					|| (21080 <= vnum && vnum <= 21089)	// Ormancı zırhı
#endif
					)
				{
					default_effect = false;
					m_armorRefineEffect[0] = EFFECT_REFINED + EFFECT_BODYARMOR_SPECIAL;	// Kabarcık-Efekti
					m_armorRefineEffect[1] = EFFECT_REFINED + EFFECT_BODYARMOR_SPECIAL2;	// Mavi Parlama Efekti
				}

#ifdef ENABLE_LVL96_ARMOR_EFFECT
	/*----------96-Zırh----------*/
				if ((21220 <= vnum && vnum <= 21225) ||	// Kara ejderha zırhı		Savaşçı
					(21240 <= vnum && vnum <= 21245) ||	// Rüzgar ejderha zırhı		Ninja
					(21260 <= vnum && vnum <= 21265) ||	// Büyü ejderha zırhı		Sura
					(21280 <= vnum && vnum <= 21285)	// Tanrılar ejderha zırhı	Şaman
#ifdef ENABLE_WOLFMAN_CHARACTER
					|| (21300 <= vnum && vnum <= 21305)	// Gök ejderha zırhı	Wolfman
#endif
					|| (21320 <= vnum && vnum <= 21325)	// Yılan Mantosu
					|| (21340 <= vnum && vnum <= 21345)	// Yılan Yeleği
					|| (21350 <= vnum && vnum <= 21365)	// Kötü yılan mantosu
					|| (21370 <= vnum && vnum <= 21385)	// Saf yılan mantosu
					)
				{
					default_effect = false;
					enable_refine = false;
					m_armorRefineEffect[0] = EFFECT_REFINED + EFFECT_BODYARMOR_SPECIAL_TH;
				}
#endif

#ifdef ENABLE_LVL115_ARMOR_EFFECT
	/*----------Yıldırım[105-115]-Zırh----------*/
				if (
					// Savaşçı
					(20760 <= vnum && vnum <= 20769) ||	// Kutup Işığı Zırhı
					(20860 <= vnum && vnum <= 20869) ||	// Balina Kemiği Zırhı
					(20810 <= vnum && vnum <= 20819) ||	// Magma Koru Zırhı
					(20910 <= vnum && vnum <= 20919) ||	// Kül Yağmuru Zırhı

					// Sura
					(20780 <= vnum && vnum <= 20789) ||	// Şeytan Pençesi Zırhı
					(20880 <= vnum && vnum <= 20889) ||	// Yanıltıcı Işık Zırhı
					(20830 <= vnum && vnum <= 20839) ||	// Cehennem Koşucu Zırhı
					(20930 <= vnum && vnum <= 20930) ||	// Kara Ateş Zırhı

					// Ninja
					(20770 <= vnum && vnum <= 20770) ||	// Suikast Kıyafeti
					(20870 <= vnum && vnum <= 20870) ||	// Dönüşüm Kıyafeti
					(20820 <= vnum && vnum <= 20820) ||	// Kanlı Ay Kıyafeti
					(20920 <= vnum && vnum <= 20920) ||	// Ölüm Gecesi Kıyafeti

					// Şaman
					(20790 <= vnum && vnum <= 20799) ||	// İnanç Kıyafeti
					(20890 <= vnum && vnum <= 20899) ||	// Ahenk Kıyafeti
					(20840 <= vnum && vnum <= 20849) ||	// Ateş Kıyafeti
					(20940 <= vnum && vnum <= 20949)	// Ay Kıyafeti

#ifdef ENABLE_WOLFMAN_CHARACTER
					// Wolfman
					|| (20800 <= vnum && vnum <= 20800)	// Gölge Savaşçı Tankı
					|| (20900 <= vnum && vnum <= 20900)	// Aytaşı levha zırh
					|| (20850 <= vnum && vnum <= 20850)	// Agat levha zırh
					|| (20950 <= vnum && vnum <= 20950)	// Onyx zırhı
#endif
					)
				{
					enable_refine = false;
					default_effect = false;
					m_armorRefineEffect[0] = EFFECT_REFINED + EFFECT_BODYARMOR_SPECIAL3;
					if (refine > 6)
						m_armorRefineEffect[1] = EFFECT_REFINED + EFFECT_BODYARMOR_REFINED7 + refine - 7;
				}
#endif

				if (default_effect)
				{
					m_armorRefineEffect[0] = EFFECT_REFINED + EFFECT_BODYARMOR_REFINED7 + refine - 7;
				}

				if ((refine < 7) && enable_refine)
					return 0;

				if (m_armorRefineEffect[0])
					m_armorRefineEffect[0] = __AttachEffect(m_armorRefineEffect[0]);
				if (m_armorRefineEffect[1])
					m_armorRefineEffect[1] = __AttachEffect(m_armorRefineEffect[1]);
			}

			break;
		}	//END ITEM_TYPE_ARMOR
#ifdef ENABLE_MDE_EFFECT
		case CItemData::ITEM_TYPE_COSTUME:
			if (pItem->GetSubType() == CItemData::COSTUME_WEAPON)
			{
				__ClearWeaponRefineEffect();

				switch (pItem->GetValue(3))
				{
				case CItemData::WEAPON_FAN:
					if (40025 == vnum)
					{
						if (m_kHorse.IsMounting()
#ifdef ENABLE_STANDING_MOUNT
							&& !m_kHorse.IsHoverBoard()
#endif
							)
						{
							m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_FAN_30;
							m_dwSpecialEffectLeft = EFFECT_REFINED + EFFECT_SPECIAL_FAN_30_LEFT;
						}
						else
						{
							m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_FAN_30;
						}
					}
					else if (40026 == vnum)
					{
						if (m_kHorse.IsMounting()
#ifdef ENABLE_STANDING_MOUNT
							&& !m_kHorse.IsHoverBoard()
#endif
							)
						{
							m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_FAN_90;
							m_dwSpecialEffectLeft = EFFECT_REFINED + EFFECT_SPECIAL_FAN_90_LEFT;
						}
						else
						{
							m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_FAN_90;
						}
					}
					break;

				case CItemData::WEAPON_BELL:
					if (40019 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BELL_30;
					}
					else if (40020 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BELL_70;
					}
					else if (40021 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BELL_90;
					}
					break;

				case CItemData::WEAPON_DAGGER:
					if (40022 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_DAGGER_30;
						m_dwSpecialEffectLeft = EFFECT_REFINED + EFFECT_SPECIAL_DAGGER_30_LEFT;
					}
					else if (40023 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_DAGGER_75;
						m_dwSpecialEffectLeft = EFFECT_REFINED + EFFECT_SPECIAL_DAGGER_75_LEFT;
					}
					else if (40024 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_DAGGER_90;
						m_dwSpecialEffectLeft = EFFECT_REFINED + EFFECT_SPECIAL_DAGGER_90_LEFT;
					}
					break;

				case CItemData::WEAPON_BOW:
					if (40016 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BOW_30;
					}
					else if (40017 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BOW_75;
					}
					else if (40018 == vnum)
					{
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BOW_90;
					}
					break;

				default:
					if (40008 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_SWORD_30_1;

					if (40009 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_SWORD_90_1;

					if (40010 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_SWORD_75_1;

					if (40011 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_SWORD_75_2;

					if (40012 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_SWORD_90_2;

					if (40013 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_SWORD_90_3;

					if (40014 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BLADE_30;

					if (40015 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BLADE_75;

					if (40027 == vnum)
						m_dwSpecialEffect = EFFECT_REFINED + EFFECT_SPECIAL_BLADE_90;
				}

				if (m_dwSpecialEffect)
					m_dwSpecialEffect = __AttachEffect(m_dwSpecialEffect);

				if (m_dwSpecialEffectLeft)
					m_dwSpecialEffectLeft = __AttachEffect(m_dwSpecialEffectLeft);
				break;
			}
			break;
#endif
	}
	return 0;
}

bool CInstanceBase::SetWeapon(DWORD eWeapon)
{
	if (IsPoly())
		return false;

	if (__IsShapeAnimalWear())
		return false;

	if (__IsChangableWeapon(eWeapon) == false)
		eWeapon = 0;

	m_GraphicThingInstance.AttachWeapon(eWeapon);
	m_awPart[CRaceData::PART_WEAPON] = eWeapon;

	//Weapon Effect
	CItemData * pItemData;
	if (CItemManager::Instance().GetItemDataPointer(eWeapon, &pItemData))
	{
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		if (pItemData->GetType() == CItemData::ITEM_TYPE_COSTUME)
			__ClearWeaponRefineEffect();
#endif
		__GetRefinedEffect(pItemData);
	}
	else
		__ClearWeaponRefineEffect();

	return true;
}

void CInstanceBase::ChangeWeapon(DWORD eWeapon)
{
	if (eWeapon == m_GraphicThingInstance.GetPartItemID(CRaceData::PART_WEAPON))
		return;

	if (SetWeapon(eWeapon))
		RefreshState(CRaceMotionData::NAME_WAIT, true);
}

bool CInstanceBase::ChangeArmor(DWORD dwArmor)
{
	DWORD eShape;
	__ArmorVnumToShape(dwArmor, &eShape);

	if (GetShape()==eShape)
		return false;

	CAffectFlagContainer kAffectFlagContainer;
	kAffectFlagContainer.CopyInstance(m_kAffectFlagContainer);

	DWORD dwVID = GetVirtualID();
	DWORD dwRace = GetRace();
	DWORD eHair = GetPart(CRaceData::PART_HAIR);
	DWORD eWeapon = GetPart(CRaceData::PART_WEAPON);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	DWORD dwAcce = GetPart(CRaceData::PART_ACCE);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	DWORD eAura = GetPart(CRaceData::PART_AURA);
#endif

	float fRot = GetRotation();
	float fAdvRot = GetAdvancingRotation();

	if (IsWalking())
		EndWalking();

	__ClearAffects();

	if (!SetRace(dwRace))
	{
		TraceError("CPythonCharacterManager::ChangeArmor - SetRace VID[%d] Race[%d] ERROR", dwVID, dwRace);
		return false;
	}

	SetArmor(dwArmor);
	SetHair(eHair);
	SetWeapon(eWeapon);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SetAcce(dwAcce);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	SetAura(eAura);
#endif

	SetRotation(fRot);
	SetAdvancingRotation(fAdvRot);

	__AttachHorseSaddle();

	RefreshState(CRaceMotionData::NAME_WAIT, TRUE);
	SetAffectFlagContainer(kAffectFlagContainer);

	CActorInstance::IEventHandler& rkEventHandler=GetEventHandlerRef();
	rkEventHandler.OnChangeShape();

	return true;
}

bool CInstanceBase::__IsShapeAnimalWear()
{
	if (100 == GetShape() ||
		101 == GetShape() ||
		102 == GetShape() ||
		103 == GetShape())
		return true;

	return false;
}

DWORD CInstanceBase::__GetRaceType()
{
	return m_eRaceType;
}


void CInstanceBase::RefreshState(DWORD dwMotIndex, bool isLoop)
{
	DWORD dwPartItemID = m_GraphicThingInstance.GetPartItemID(CRaceData::PART_WEAPON);

	BYTE byItemType = 0xff;
	BYTE bySubType = 0xff;

	CItemManager & rkItemMgr = CItemManager::Instance();
	CItemData * pItemData;

	if (rkItemMgr.GetItemDataPointer(dwPartItemID, &pItemData))
	{
		byItemType = pItemData->GetType();
		bySubType = pItemData->GetWeaponType();
	}

	if (IsPoly())
	{
		SetMotionMode(CRaceMotionData::MODE_GENERAL);
	}
	else if (IsWearingDress())
	{
		SetMotionMode(CRaceMotionData::MODE_WEDDING_DRESS);
	}
	else if (IsHoldingPickAxe())
	{
#ifdef ENABLE_STANDING_MOUNT
		if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
#else
		if (m_kHorse.IsMounting())
#endif
		{
			SetMotionMode(CRaceMotionData::MODE_HORSE);
		}
#ifdef ENABLE_STANDING_MOUNT
		else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
		{
			SetMotionMode(CRaceMotionData::MODE_HORSE_STAND);
		}
#endif
		else
		{
			SetMotionMode(CRaceMotionData::MODE_GENERAL);
		}
	}
	else if (CItemData::ITEM_TYPE_ROD == byItemType)
	{
#ifdef ENABLE_STANDING_MOUNT
		if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
#else
		if (m_kHorse.IsMounting())
#endif
		{
			SetMotionMode(CRaceMotionData::MODE_HORSE);
		}
#ifdef ENABLE_STANDING_MOUNT
		else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
		{
			SetMotionMode(CRaceMotionData::MODE_HORSE_STAND);
		}
#endif
		else
		{
			SetMotionMode(CRaceMotionData::MODE_FISHING);
		}
	}
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	else if (byItemType == CItemData::ITEM_TYPE_COSTUME)
	{
		switch (pItemData->GetValue(3))
		{
			case CItemData::WEAPON_SWORD:
#ifdef ENABLE_STANDING_MOUNT
				if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
				else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_SWORD);
#else
				if (m_kHorse.IsMounting())
					SetMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
#endif
				else
					SetMotionMode(CRaceMotionData::MODE_ONEHAND_SWORD);
				break;
			case CItemData::WEAPON_DAGGER:
#ifdef ENABLE_STANDING_MOUNT
				if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD);
				else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD);
#else
				if (m_kHorse.IsMounting())
					SetMotionMode(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD);
#endif
				else
					SetMotionMode(CRaceMotionData::MODE_DUALHAND_SWORD);
				break;
			case CItemData::WEAPON_BOW:
#ifdef ENABLE_STANDING_MOUNT
				if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_BOW);
				else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_BOW);
#else
				if (m_kHorse.IsMounting())
					SetMotionMode(CRaceMotionData::MODE_HORSE_BOW);
#endif
				else
#ifdef ENABLE_NEW_ARROW_SYSTEM
				{
					if (m_awPart[CRaceData::PART_ARROW_TYPE] == CItemData::WEAPON_UNLIMITED_ARROW)
						SetMotionMode(CRaceMotionData::MODE_BOW_SPECIAL);
					else
						SetMotionMode(CRaceMotionData::MODE_BOW);
				}
#else
					SetMotionMode(CRaceMotionData::MODE_BOW);
#endif
				break;
			case CItemData::WEAPON_TWO_HANDED:
#ifdef ENABLE_STANDING_MOUNT
				if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD);
				else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD);
#else
				if (m_kHorse.IsMounting())
					SetMotionMode(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD);
#endif
				else
					SetMotionMode(CRaceMotionData::MODE_TWOHAND_SWORD);
				break;
			case CItemData::WEAPON_BELL:
#ifdef ENABLE_STANDING_MOUNT
				if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_BELL);
				else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_BELL);
#else
				if (m_kHorse.IsMounting())
					SetMotionMode(CRaceMotionData::MODE_HORSE_BELL);
#endif
				else
					SetMotionMode(CRaceMotionData::MODE_BELL);
				break;
			case CItemData::WEAPON_FAN:
#ifdef ENABLE_STANDING_MOUNT
				if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_FAN);
				else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_FAN);
#else
				if (m_kHorse.IsMounting())
					SetMotionMode(CRaceMotionData::MODE_HORSE_FAN);
#endif
				else
					SetMotionMode(CRaceMotionData::MODE_FAN);
				break;

			default:
#ifdef ENABLE_STANDING_MOUNT
				if (m_kHorse.IsMounting() && !m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE);
				else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
					SetMotionMode(CRaceMotionData::MODE_HORSE_STAND);
#else
				if (m_kHorse.IsMounting())
					SetMotionMode(CRaceMotionData::MODE_HORSE);
#endif
				else
					SetMotionMode(CRaceMotionData::MODE_GENERAL);
				break;
		}
	}
#endif
#ifdef ENABLE_STANDING_MOUNT
	else if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
	{
		switch (bySubType)
		{
			case CItemData::WEAPON_SWORD:
				SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_SWORD);
				break;

			case CItemData::WEAPON_TWO_HANDED:
				SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD);
				break;

			case CItemData::WEAPON_DAGGER:
				SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD);
				break;

			case CItemData::WEAPON_FAN:
				SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_FAN);
				break;

			case CItemData::WEAPON_BELL:
				SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_BELL);
				break;

			case CItemData::WEAPON_BOW:
				SetMotionMode(CRaceMotionData::MODE_HORSE_STAND_BOW);
				break;

			default:
				SetMotionMode(CRaceMotionData::MODE_HORSE_STAND);
				break;
		}
	}
#endif
	else if (m_kHorse.IsMounting())
	{
		switch (bySubType)
		{
			case CItemData::WEAPON_SWORD:
				SetMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
				break;

			case CItemData::WEAPON_TWO_HANDED:
				SetMotionMode(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD); // Only Warrior
				break;

			case CItemData::WEAPON_DAGGER:
				SetMotionMode(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD); // Only Assassin
				break;

			case CItemData::WEAPON_FAN:
				SetMotionMode(CRaceMotionData::MODE_HORSE_FAN); // Only Shaman
				break;

			case CItemData::WEAPON_BELL:
				SetMotionMode(CRaceMotionData::MODE_HORSE_BELL); // Only Shaman
				break;

			case CItemData::WEAPON_BOW:
				SetMotionMode(CRaceMotionData::MODE_HORSE_BOW); // Only Shaman
				break;

			default:
				SetMotionMode(CRaceMotionData::MODE_HORSE);
				break;
		}
	}
	else
	{
		switch (bySubType)
		{
			case CItemData::WEAPON_SWORD:
				SetMotionMode(CRaceMotionData::MODE_ONEHAND_SWORD);
				break;

			case CItemData::WEAPON_TWO_HANDED:
				SetMotionMode(CRaceMotionData::MODE_TWOHAND_SWORD); // Only Warrior
				break;

			case CItemData::WEAPON_DAGGER:
				SetMotionMode(CRaceMotionData::MODE_DUALHAND_SWORD); // Only Assassin
				break;

			case CItemData::WEAPON_BOW:
#ifdef ENABLE_NEW_ARROW_SYSTEM
				if (m_awPart[CRaceData::PART_ARROW_TYPE] == CItemData::WEAPON_UNLIMITED_ARROW)
					SetMotionMode(CRaceMotionData::MODE_BOW_SPECIAL);
				else
					SetMotionMode(CRaceMotionData::MODE_BOW);
#else
				SetMotionMode(CRaceMotionData::MODE_BOW); // Only Assassin
#endif
				break;

			case CItemData::WEAPON_FAN:
				SetMotionMode(CRaceMotionData::MODE_FAN); // Only Shaman
				break;

			case CItemData::WEAPON_BELL:
				SetMotionMode(CRaceMotionData::MODE_BELL); // Only Shaman
				break;

			case CItemData::WEAPON_ARROW:
			case CItemData::WEAPON_QUIVER:
			default:
				SetMotionMode(CRaceMotionData::MODE_GENERAL);
				break;
		}
	}

	if (isLoop)
		m_GraphicThingInstance.InterceptLoopMotion(dwMotIndex);
	else
		m_GraphicThingInstance.InterceptOnceMotion(dwMotIndex);

	RefreshActorInstance();
}

void CInstanceBase::RegisterBoundingSphere()
{
	if (!IsStone())
	{
		m_GraphicThingInstance.DeformNoSkin();
	}

	m_GraphicThingInstance.RegisterBoundingSphere();
}

bool CInstanceBase::CreateDeviceObjects()
{
	return m_GraphicThingInstance.CreateDeviceObjects();
}

void CInstanceBase::DestroyDeviceObjects()
{
	m_GraphicThingInstance.DestroyDeviceObjects();
}

void CInstanceBase::Destroy()
{
	DetachTextTail();

	DismountHorse();

	m_kQue_kCmdNew.clear();

	__EffectContainer_Destroy();
	__StoneSmoke_Destroy();

	if (__IsMainInstance())
		__ClearMainInstance();

	m_GraphicThingInstance.Destroy();

	__Initialize();
}

void CInstanceBase::__InitializeRotationSpeed()
{
	SetRotationSpeed(c_fDefaultRotationSpeed);
}

void CInstanceBase::__Warrior_Initialize()
{
	m_kWarrior.m_dwGeomgyeongEffect=0;
}

#ifdef ENABLE_PVP_BALANCE
void CInstanceBase::__Assassin_Initialize()
{
	m_kAssassin.m_dwGyeongGongEffect = 0;
}
#endif

void CInstanceBase::__Initialize()
{
    m_optionQueueIndex=0;
	__Warrior_Initialize();
#ifdef ENABLE_PVP_BALANCE
	__Assassin_Initialize();
#endif
	__StoneSmoke_Inialize();
	__EffectContainer_Initialize();
	__InitializeRotationSpeed();

	SetEventHandler(CActorInstance::IEventHandler::GetEmptyPtr());

	m_kAffectFlagContainer.Clear();

	m_dwLevel = 0;
#ifdef ENABLE_CONQUEROR_LEVEL
	m_dwConquerorLevel = 0;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
	m_dwAIFlag = 0;
#endif
	m_dwGuildID = 0;
	m_dwEmpireID = 0;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	m_dwGuildLeader = 0;
#endif

	m_eType = 0;
	m_eRaceType = 0;
	m_eShape = 0;
	m_dwRace = 0;
	m_dwVirtualNumber = 0;

	m_dwBaseCmdTime = 0;
	m_dwBaseChkTime = 0;
	m_dwSkipTime = 0;

	m_GraphicThingInstance.Initialize();

	m_dwAdvActorVID = 0;
	m_dwLastDmgActorVID = 0;

	m_nAverageNetworkGap = 0;
	m_dwNextUpdateHeightTime = 0;

	// Moving by keyboard
	m_iRotatingDirection = DEGREE_DIRECTION_SAME;

	// Moving by mouse
	m_isTextTail = FALSE;
	m_isGoing = FALSE;
	NEW_SetSrcPixelPosition(TPixelPosition(0, 0, 0));
	NEW_SetDstPixelPosition(TPixelPosition(0, 0, 0));

	m_kPPosDust = TPixelPosition(0, 0, 0);


	m_kQue_kCmdNew.clear();

	m_dwLastComboIndex = 0;

	m_swordRefineEffectRight = 0;
	 m_swordRefineEffectLeft = 0;
	 m_armorRefineEffect[0] = 0;
	 m_armorRefineEffect[1] = 0;
#ifdef ENABLE_TITLE_SYSTEM
	 m_dwOverHeadSpecialEffect = 0;
#endif
#ifdef ENABLE_MDE_EFFECT
	m_dwSpecialEffect = 0;
	m_dwSpecialEffectLeft = 0;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_dwAcceEffect = 0;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	m_auraRefineEffect = 0;
#endif

	m_sAlignment = 0;
	m_byPKMode = 0;
	m_isKiller = false;
	m_isPartyMember = false;

	m_bEnableTCPState = TRUE;

	m_stName = "";

	memset(m_awPart, 0, sizeof(m_awPart));
	memset(m_adwCRCAffectEffect, 0, sizeof(m_adwCRCAffectEffect));
	memset(&m_kMovAfterFunc, 0, sizeof(m_kMovAfterFunc));

	m_bDamageEffectType = false;
	m_dwDuelMode = DUEL_NONE;
	m_dwEmoticonTime = 0;

#ifdef ENABLE_RENDER_TARGET
	m_IsAlwaysRender = false;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	m_bCharacterSize = 0;
#endif
}

CInstanceBase::CInstanceBase()
{
	__Initialize();
}

CInstanceBase::~CInstanceBase()
{
	Destroy();
}

DWORD CInstanceBase::GetPartData(BYTE partIndex)
{
	if (partIndex >= _countof(m_awPart))
		return 0;

	return m_awPart[partIndex];
}

void CInstanceBase::GetBoundBox(D3DXVECTOR3 * vtMin, D3DXVECTOR3 * vtMax)
{
	m_GraphicThingInstance.GetBoundBox(vtMin, vtMax);
}

float CInstanceBase::GetBaseHeight()
{
	CActorInstance* pkHorse = m_kHorse.GetActorPtr();
	if (!m_kHorse.IsMounting() || !pkHorse)
		return 0.0f;

	DWORD dwHorseVnum = m_kHorse.m_pkActor->GetRace();
	if ((dwHorseVnum >= 20101 && dwHorseVnum <= 20109) || (dwHorseVnum == 20029 || dwHorseVnum == 20030))
		return 100.0f;

	if ((dwHorseVnum >= 20110 && dwHorseVnum <= 20125) || (dwHorseVnum >= 20201 && dwHorseVnum <= 20281))
		return 50.0f;

	float fRaceHeight = CRaceManager::instance().GetRaceHeight(dwHorseVnum);
	if (fRaceHeight == 0.0f)
		return 100.0f;
	else
		return fRaceHeight;
}

#ifdef ENABLE_RENDER_TARGET
bool CInstanceBase::IsAlwaysRender()
{
	return m_IsAlwaysRender;
}

void CInstanceBase::SetAlwaysRender(bool val)
{
	m_IsAlwaysRender = val;
}

void CInstanceBase::AttachWikiAffect(DWORD effectIndex)
{
	const DWORD newAffect = __AttachEffect(effectIndex);
	m_vecWikiEffects.push_back(newAffect);
}

void CInstanceBase::RemoveWikiAffect(DWORD effectIndex)
{
	const auto it = std::find(m_vecWikiEffects.begin(), m_vecWikiEffects.end(), effectIndex);
	if (it != m_vecWikiEffects.end())
	{
		__DetachEffect(effectIndex);
		m_vecWikiEffects.erase(it);
	}
}

void CInstanceBase::ClearWikiAffect()
{
	for (const auto& effectIndex : m_vecWikiEffects)
		__DetachEffect(effectIndex);
	m_vecWikiEffects.clear();
}
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
void CInstanceBase::RefreshStoneScale()
{
	if (!IsStone())
		return;

	const float StoneScale = 1 + CPythonSystem::Instance().GetStoneScale();
	m_GraphicThingInstance.SetScale(StoneScale, StoneScale, StoneScale, true);
	DetachTextTail();
	AttachTextTail();
	RefreshTextTail();
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
void CInstanceBase::ChangeAura(DWORD eAura)
{
	if (m_GraphicThingInstance.GetPartItemID(CRaceData::PART_AURA) != eAura)
		SetAura(eAura);
}

bool CInstanceBase::SetAura(DWORD eAura)
{
#ifdef ENABLE_NPC_WEAR_ITEM
	if ((IsPC() && GetRace() > 7) || IsPoly() || IsWearingDress() || __IsShapeAnimalWear())
#else
	if (!IsPC() || IsPoly() || IsWearingDress() || __IsShapeAnimalWear())
#endif
		return false;

	m_GraphicThingInstance.ChangePart(CRaceData::PART_AURA, eAura);
	if (!eAura)
	{
		if (m_auraRefineEffect)
		{
			__DetachEffect(m_auraRefineEffect);
			m_auraRefineEffect = 0;
		}
		m_awPart[CRaceData::PART_AURA] = 0;
		return true;
	}

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(eAura, &pItemData))
	{
		if (m_auraRefineEffect)
		{
			__DetachEffect(m_auraRefineEffect);
			m_auraRefineEffect = 0;
		}
		m_awPart[CRaceData::PART_AURA] = 0;
		return true;
	}

	BYTE byRace = (BYTE)GetRace();
	BYTE byJob = (BYTE)RaceToJob(byRace);
	BYTE bySex = (BYTE)RaceToSex(byRace);

	D3DXVECTOR3 v3MeshScale = pItemData->GetAuraMeshScaleVector(byJob, bySex);
	float fParticleScale = pItemData->GetAuraParticleScale(byJob, bySex);
	m_auraRefineEffect = m_GraphicThingInstance.AttachEffectByID(0, "Bip01 Spine2", pItemData->GetAuraEffectID(), NULL
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, NULL
#endif
		, fParticleScale, &v3MeshScale);
	m_awPart[CRaceData::PART_AURA] = eAura;
	return true;
}
#endif

#ifdef ENABLE_DEFENSAWESHIP
BOOL CInstanceBase::IsHydraNPC()
{
	switch (m_dwVirtualNumber)
	{
		case 6801:
			return TRUE;
	}
	return FALSE;
}
#endif


bool CInstanceBase::__Background_IsGrassPixelPosition(const TPixelPosition& c_rkPPos)
{
	return CPythonBackground::Instance().isAttrOn(c_rkPPos.x, c_rkPPos.y, CTerrainImpl::ATTRIBUTE_GRASS);
}
