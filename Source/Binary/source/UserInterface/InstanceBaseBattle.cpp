#include "StdAfx.h"
#include "InstanceBase.h"
#include "PythonBackground.h"
#include "PythonCharacterManager.h"
#include "../PRTerrainLib/Terrain.h"
#ifdef __AUTO_HUNT__
	#include "PythonPlayer.h"
#endif

float NEW_UnsignedDegreeToSignedDegree(float fUD)
{
	float fSD;
	if (fUD>180.0f)
		fSD=-(360.0f-fUD);
	else if (fUD<-180.0f)
		fSD=+(360.0f+fUD);
	else
		fSD=fUD;

	return fSD;
}

float NEW_GetSignedDegreeFromDirPixelPosition(const TPixelPosition& kPPosDir)
{
	D3DXVECTOR3 vtDir(kPPosDir.x, -kPPosDir.y, kPPosDir.z);
	D3DXVECTOR3 vtDirNormal;
	D3DXVec3Normalize(&vtDirNormal, &vtDir);

	D3DXVECTOR3 vtDirNormalStan(0, -1, 0);
	float fDirRot = D3DXToDegree(acosf(D3DXVec3Dot(&vtDirNormal, &vtDirNormalStan)));

	if (vtDirNormal.x<0.0f)
		fDirRot=-fDirRot;

	return fDirRot;
}

bool CInstanceBase::IsFlyTargetObject()
{
	return m_GraphicThingInstance.IsFlyTargetObject();
}

float CInstanceBase::GetFlyTargetDistance()
{
	return m_GraphicThingInstance.GetFlyTargetDistance();
}

void CInstanceBase::ClearFlyTargetInstance()
{
	m_GraphicThingInstance.ClearFlyTarget();
}

void CInstanceBase::SetFlyTargetInstance(CInstanceBase& rkInstDst)
{
	m_GraphicThingInstance.SetFlyTarget(rkInstDst.GetGraphicThingInstancePtr());
}

void CInstanceBase::AddFlyTargetPosition(const TPixelPosition& c_rkPPosDst)
{
	m_GraphicThingInstance.AddFlyTarget(c_rkPPosDst);
}

void CInstanceBase::AddFlyTargetInstance(CInstanceBase& rkInstDst)
{
	m_GraphicThingInstance.AddFlyTarget(rkInstDst.GetGraphicThingInstancePtr());
}


float CInstanceBase::NEW_GetDistanceFromDestInstance(CInstanceBase& rkInstDst)
{
	TPixelPosition kPPosDst;
	rkInstDst.NEW_GetPixelPosition(&kPPosDst);

	return NEW_GetDistanceFromDestPixelPosition(kPPosDst);
}

float CInstanceBase::NEW_GetDistanceFromDestPixelPosition(const TPixelPosition& c_rkPPosDst)
{
	TPixelPosition kPPosCur;
	NEW_GetPixelPosition(&kPPosCur);

	TPixelPosition kPPosDir;
	kPPosDir=c_rkPPosDst-kPPosCur;

	return NEW_GetDistanceFromDirPixelPosition(kPPosDir);
}

float CInstanceBase::NEW_GetDistanceFromDirPixelPosition(const TPixelPosition& c_rkPPosDir)
{
	return sqrtf(c_rkPPosDir.x*c_rkPPosDir.x+c_rkPPosDir.y*c_rkPPosDir.y);
}

float CInstanceBase::NEW_GetRotation()
{
	float fCurRot=GetRotation();
	return NEW_UnsignedDegreeToSignedDegree(fCurRot);
}

float CInstanceBase::NEW_GetRotationFromDirPixelPosition(const TPixelPosition& c_rkPPosDir)
{
	return NEW_GetSignedDegreeFromDirPixelPosition(c_rkPPosDir);
}

float CInstanceBase::NEW_GetRotationFromDestPixelPosition(const TPixelPosition& c_rkPPosDst)
{
	TPixelPosition kPPosCur;
	NEW_GetPixelPosition(&kPPosCur);

	TPixelPosition kPPosDir;
	kPPosDir=c_rkPPosDst-kPPosCur;

	return NEW_GetRotationFromDirPixelPosition(kPPosDir);
}

float CInstanceBase::NEW_GetRotationFromDestInstance(CInstanceBase& rkInstDst)
{
	TPixelPosition kPPosDst;
	rkInstDst.NEW_GetPixelPosition(&kPPosDst);

	return NEW_GetRotationFromDestPixelPosition(kPPosDst);
}

void CInstanceBase::NEW_GetRandomPositionInFanRange(CInstanceBase& rkInstTarget, TPixelPosition* pkPPosDst)
{
	float fDstDirRot=NEW_GetRotationFromDestInstance(rkInstTarget);	

	float fRot=frandom(fDstDirRot-10.0f, fDstDirRot+10.0f);

	D3DXMATRIX kMatRot;
	D3DXMatrixRotationZ(&kMatRot, D3DXToRadian(-fRot));

	D3DXVECTOR3 v3Src(0.0f, 8000.0f, 0.0f);
	D3DXVECTOR3 v3Pos;
	D3DXVec3TransformCoord(&v3Pos, &v3Src, &kMatRot);

	const TPixelPosition& c_rkPPosCur=NEW_GetCurPixelPositionRef();

	pkPPosDst->x=c_rkPPosCur.x+v3Pos.x;
	pkPPosDst->y=c_rkPPosCur.y+v3Pos.y;
	pkPPosDst->z=__GetBackgroundHeight(c_rkPPosCur.x, c_rkPPosCur.y);
}

bool CInstanceBase::NEW_GetFrontInstance(CInstanceBase ** ppoutTargetInstance, float fDistance)
{
	const float HALF_FAN_ROT_MIN = 10.0f;
	const float HALF_FAN_ROT_MAX = 50.0f;
	const float HALF_FAN_ROT_MIN_DISTANCE = 1000.0f;
	const float RPM = (HALF_FAN_ROT_MAX-HALF_FAN_ROT_MIN)/HALF_FAN_ROT_MIN_DISTANCE;

	float fDstRot=NEW_GetRotation();

	std::multimap<float, CInstanceBase*> kMap_pkInstNear;
	{
		CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
		CPythonCharacterManager::CharacterIterator i;
		for(i = rkChrMgr.CharacterInstanceBegin(); i!=rkChrMgr.CharacterInstanceEnd(); ++i)
		{
			CInstanceBase* pkInstEach=*i;
			if (pkInstEach==this)
				continue;

			if (!IsAttackableInstance(*pkInstEach))
				continue;

			if (NEW_GetDistanceFromDestInstance(*pkInstEach) > fDistance)
				continue;

			float fEachInstDistance=min(NEW_GetDistanceFromDestInstance(*pkInstEach), HALF_FAN_ROT_MIN_DISTANCE);
			float fEachInstDirRot=NEW_GetRotationFromDestInstance(*pkInstEach);

			float fHalfFanRot=(HALF_FAN_ROT_MAX-HALF_FAN_ROT_MIN)-RPM*fEachInstDistance+HALF_FAN_ROT_MIN;

			float fMinDstDirRot=fDstRot-fHalfFanRot;
			float fMaxDstDirRot=fDstRot+fHalfFanRot;

			if (fEachInstDirRot>=fMinDstDirRot && fEachInstDirRot<=fMaxDstDirRot)
				kMap_pkInstNear.insert(std::multimap<float, CInstanceBase*>::value_type(fEachInstDistance, pkInstEach));
		}
	}

	if (kMap_pkInstNear.empty())
		return false;

	*ppoutTargetInstance = kMap_pkInstNear.begin()->second;

	return true;
}

bool CInstanceBase::NEW_GetInstanceVectorInFanRange(float fSkillDistance, CInstanceBase& rkInstTarget, std::vector<CInstanceBase*>* pkVct_pkInst)
{
	const float HALF_FAN_ROT_MIN = 20.0f;
	const float HALF_FAN_ROT_MAX = 40.0f;
	const float HALF_FAN_ROT_MIN_DISTANCE = 1000.0f;
	const float RPM = (HALF_FAN_ROT_MAX-HALF_FAN_ROT_MIN)/HALF_FAN_ROT_MIN_DISTANCE;

	float fDstDirRot=NEW_GetRotationFromDestInstance(rkInstTarget);

	std::multimap<float, CInstanceBase*> kMap_pkInstNear;
	{
		CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
		CPythonCharacterManager::CharacterIterator i;
		for(i = rkChrMgr.CharacterInstanceBegin(); i!=rkChrMgr.CharacterInstanceEnd(); ++i)
		{
			CInstanceBase* pkInstEach=*i;
			if (pkInstEach==this)
				continue;

			if (!IsAttackableInstance(*pkInstEach))
				continue;

			if (m_GraphicThingInstance.IsClickableDistanceDestInstance(pkInstEach->m_GraphicThingInstance, fSkillDistance))
			{
				float fEachInstDistance=min(NEW_GetDistanceFromDestInstance(*pkInstEach), HALF_FAN_ROT_MIN_DISTANCE);
				float fEachInstDirRot=NEW_GetRotationFromDestInstance(*pkInstEach);

				float fHalfFanRot=(HALF_FAN_ROT_MAX-HALF_FAN_ROT_MIN)-RPM*fEachInstDistance+HALF_FAN_ROT_MIN;

				float fMinDstDirRot=fDstDirRot-fHalfFanRot;
				float fMaxDstDirRot=fDstDirRot+fHalfFanRot;

				if (fEachInstDirRot>=fMinDstDirRot && fEachInstDirRot<=fMaxDstDirRot)
					kMap_pkInstNear.insert(std::multimap<float, CInstanceBase*>::value_type(fEachInstDistance, pkInstEach));
			}
		}
	}

	{
		std::multimap<float, CInstanceBase*>::iterator i=kMap_pkInstNear.begin();
		for (i=kMap_pkInstNear.begin(); i!=kMap_pkInstNear.end(); ++i)
			pkVct_pkInst->push_back(i->second);
	}

	if (pkVct_pkInst->empty())
		return false;

	return true;
}

bool CInstanceBase::NEW_GetInstanceVectorInCircleRange(float fSkillDistance, std::vector<CInstanceBase*>* pkVct_pkInst)
{
	std::multimap<float, CInstanceBase*> kMap_pkInstNear;

	{
		CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
		CPythonCharacterManager::CharacterIterator i;
		for(i = rkChrMgr.CharacterInstanceBegin(); i!=rkChrMgr.CharacterInstanceEnd(); ++i)
		{
			CInstanceBase* pkInstEach=*i;

			if (pkInstEach==this)
				continue;

			if (!IsAttackableInstance(*pkInstEach))
				continue;

			if (m_GraphicThingInstance.IsClickableDistanceDestInstance(pkInstEach->m_GraphicThingInstance, fSkillDistance))
			{
				float fEachInstDistance=NEW_GetDistanceFromDestInstance(*pkInstEach);
				kMap_pkInstNear.insert(std::make_pair(fEachInstDistance, pkInstEach));
			}
		}
	}

	{
		std::multimap<float, CInstanceBase*>::iterator i=kMap_pkInstNear.begin();
		for (i=kMap_pkInstNear.begin(); i!=kMap_pkInstNear.end(); ++i)
			pkVct_pkInst->push_back(i->second);
	}

	if (pkVct_pkInst->empty())
		return false;

	return true;
}

BOOL CInstanceBase::NEW_IsClickableDistanceDestPixelPosition(const TPixelPosition& c_rkPPosDst)
{
	float fDistance=NEW_GetDistanceFromDestPixelPosition(c_rkPPosDst);

	if (fDistance>150.0f)
		return FALSE;

	return TRUE;
}

BOOL CInstanceBase::NEW_IsClickableDistanceDestInstance(CInstanceBase& rkInstDst)
{
	float fDistance = 150.0f;

	if (IsBowMode())
		fDistance = __GetBowRange();

	if (rkInstDst.IsNPC())
		fDistance = 500.0f;

	if (rkInstDst.IsPet()
#ifdef ENABLE_GROWTH_PET_SYSTEM
		|| rkInstDst.IsGrowthPet()
#endif
		)
		fDistance = 500.0f;

	if (rkInstDst.IsResource())
		fDistance = 100.0f;

	return m_GraphicThingInstance.IsClickableDistanceDestInstance(rkInstDst.m_GraphicThingInstance, fDistance);
}

bool CInstanceBase::NEW_UseSkill(UINT uSkill, UINT uMot, UINT uMotLoopCount, bool isMovingSkill)
{
	if (IsDead())
		return false;

	if (IsStun())
		return false;

	if (IsKnockDown())
		return false;

#ifdef ENABLE_STANDING_MOUNT
	// Standing mount üzerindeyken motion mode'u doðru ayarla
	if (m_kHorse.IsMounting() && m_kHorse.IsHoverBoard())
	{
		RefreshState(0, false);
	}
#endif

	if (isMovingSkill)
	{
		if (!IsWalking())
			StartWalking();

		m_isGoing = TRUE;
	}
	else
	{
		if (IsWalking())
			EndWalking();

		m_isGoing = FALSE;
	}

#ifdef ENABLE_STANDING_MOUNT
	if (IsMountingHoverBoard())
		m_kHorse.m_pkActor->Stop(0.15f);
#endif

	float fCurRot=m_GraphicThingInstance.GetTargetRotation();
	SetAdvancingRotation(fCurRot);

	m_GraphicThingInstance.InterceptOnceMotion(CRaceMotionData::NAME_SKILL + uMot, 0.1f, uSkill, 1.0f);

	m_GraphicThingInstance.__OnUseSkill(uMot, uMotLoopCount, isMovingSkill);

	if (uMotLoopCount > 0)
		m_GraphicThingInstance.SetMotionLoopCount(uMotLoopCount);

	return true;
}

void CInstanceBase::NEW_Attack()
{
	float fDirRot=GetRotation();
	NEW_Attack(fDirRot);
}

void CInstanceBase::NEW_Attack(float fDirRot)
{
	if (IsDead())
		return;

	if (IsStun())
		return;

	if (IsKnockDown())
		return;

	if (IsUsingSkill())
		return;
	
	if (IsWalking())
		EndWalking();

	m_isGoing = FALSE;

	if (IsPoly())
	{
		InputNormalAttack(fDirRot);
	}
	else
	{
		if (m_kHorse.IsMounting())
		{
			InputComboAttack(fDirRot);
		}
		else
		{
			InputComboAttack(fDirRot);
		}
	}
}


void CInstanceBase::NEW_AttackToDestPixelPositionDirection(const TPixelPosition& c_rkPPosDst)
{
	float fDirRot=NEW_GetRotationFromDestPixelPosition(c_rkPPosDst);
	
	NEW_Attack(fDirRot);
}

bool CInstanceBase::NEW_AttackToDestInstanceDirection(CInstanceBase& rkInstDst, IFlyEventHandler* pkFlyHandler)
{
	return NEW_AttackToDestInstanceDirection(rkInstDst);
}

bool CInstanceBase::NEW_AttackToDestInstanceDirection(CInstanceBase& rkInstDst)
{
	TPixelPosition kPPosDst;
	rkInstDst.NEW_GetPixelPosition(&kPPosDst);
	NEW_AttackToDestPixelPositionDirection(kPPosDst);

	return true;
}

void CInstanceBase::AttackProcess()
{
	if (!m_GraphicThingInstance.CanCheckAttacking())
		return;
   
	CInstanceBase * pkInstLast = nullptr;
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CPythonCharacterManager::CharacterIterator i = rkChrMgr.CharacterInstanceBegin();
	while (rkChrMgr.CharacterInstanceEnd()!=i)
	{
		CInstanceBase* pkInstEach=*i;
		++i;

		if (!IsAttackableInstance(*pkInstEach))
			continue;

		if (pkInstEach!=this)
		{
			if (CheckAttacking(*pkInstEach))
			{
				pkInstLast=pkInstEach;
			}
		}
	}

	if (pkInstLast)
	{
		m_dwLastDmgActorVID=pkInstLast->GetVirtualID();
	}
}

void CInstanceBase::InputNormalAttack(float fAtkDirRot)
{
	m_GraphicThingInstance.InputNormalAttackCommand(fAtkDirRot);
}

void CInstanceBase::InputComboAttack(float fAtkDirRot)
{
#ifdef ENABLE_STANDING_MOUNT
	if (IsMountingHoverBoard())
		m_kHorse.m_pkActor->Stop(0.15f);
#endif
	m_GraphicThingInstance.InputComboAttackCommand(fAtkDirRot);
	__ComboProcess();
}

void CInstanceBase::RunNormalAttack(float fAtkDirRot)
{
	EndGoing();
	m_GraphicThingInstance.NormalAttack(fAtkDirRot);
}

void CInstanceBase::RunComboAttack(float fAtkDirRot, DWORD wMotionIndex)
{
	EndGoing();
	m_GraphicThingInstance.ComboAttack(wMotionIndex, fAtkDirRot);
}

BOOL CInstanceBase::CheckAdvancing()
{
	if (!__IsMainInstance() && !IsAttacking())
	{
		if (IsPC() && IsWalking())
		{
			CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
			for(CPythonCharacterManager::CharacterIterator i = rkChrMgr.CharacterInstanceBegin(); i!=rkChrMgr.CharacterInstanceEnd();++i)
			{
				CInstanceBase* pkInstEach=*i;
				if (pkInstEach==this)
					continue;
				if (!pkInstEach->IsDoor())
					continue;

				if (m_GraphicThingInstance.TestActorCollision(pkInstEach->GetGraphicThingInstanceRef()))
				{
					BlockMovement();
					return true;
				}
			}
		}
		return FALSE;
	}

	if (m_GraphicThingInstance.CanSkipCollision())
	{
		return FALSE;
	}


	BOOL bUsingSkill = m_GraphicThingInstance.IsUsingSkill();

	m_dwAdvActorVID = 0;
	UINT uCollisionCount=0;

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	for(CPythonCharacterManager::CharacterIterator i = rkChrMgr.CharacterInstanceBegin(); i!=rkChrMgr.CharacterInstanceEnd();++i)
	{
		CInstanceBase* pkInstEach=*i;
		if (pkInstEach==this)
			continue;

		CActorInstance& rkActorSelf=m_GraphicThingInstance;
		CActorInstance& rkActorEach=pkInstEach->GetGraphicThingInstanceRef();

		if( bUsingSkill && !rkActorEach.IsDoor() )
			continue;

		CInstanceBase * pkInstBase = CPythonCharacterManager::Instance().GetInstancePtr(rkActorEach.GetVirtualID());

		if (pkInstBase)
		{
			if (pkInstBase->HasAffect(AFFECT_INVISIBILITY) || pkInstBase->HasAffect(AFFECT_EUNHYEONG))
				continue;
		}

		if (rkActorSelf.TestActorCollision(rkActorEach))
		{
			uCollisionCount++;
			if (uCollisionCount==2)
			{
				rkActorSelf.BlockMovement();
				return TRUE;
			}
			rkActorSelf.AdjustDynamicCollisionMovement(&rkActorEach);

			if (rkActorSelf.TestActorCollision(rkActorEach))
			{
				rkActorSelf.BlockMovement();
				return TRUE;
			}
			else
			{
				NEW_MoveToDestPixelPositionDirection(NEW_GetDstPixelPositionRef());
			}
		}
	}

#ifdef ENABLE_NEW_STOP_ON_BLOCK
	CPythonBackground& rkBG=CPythonBackground::Instance();
	const D3DXVECTOR3 & rv3Position = m_GraphicThingInstance.GetPosition();
	const D3DXVECTOR3 & rv3MoveDirection = m_GraphicThingInstance.GetMovementVectorRef();

	int iStep = int(D3DXVec3Length(&rv3MoveDirection) / 10.0f);
	D3DXVECTOR3 v3CheckStep = rv3MoveDirection / float(iStep);
	D3DXVECTOR3 v3CheckPosition = rv3Position;
	for (int j = 0; j < iStep; ++j)
	{
		v3CheckPosition += v3CheckStep;

		if (rkBG.isAttrOn(v3CheckPosition.x, -v3CheckPosition.y, CTerrainImpl::ATTRIBUTE_BLOCK))
		{
			NEW_Stop();
			BlockMovement();
		}
	}

	D3DXVECTOR3 v3NextPosition = rv3Position + rv3MoveDirection;
	if (rkBG.isAttrOn(v3NextPosition.x, -v3NextPosition.y, CTerrainImpl::ATTRIBUTE_BLOCK))
	{
		NEW_Stop();
		BlockMovement();
		return TRUE;
	}
#else
	CPythonBackground& rkBG=CPythonBackground::Instance();
	const D3DXVECTOR3 & rv3Position = m_GraphicThingInstance.GetPosition();
	const D3DXVECTOR3 & rv3MoveDirection = m_GraphicThingInstance.GetMovementVectorRef();

	int iStep = int(D3DXVec3Length(&rv3MoveDirection) / 10.0f);
	D3DXVECTOR3 v3CheckStep = rv3MoveDirection / float(iStep);
	D3DXVECTOR3 v3CheckPosition = rv3Position;
	for (int j = 0; j < iStep; ++j)
	{
		v3CheckPosition += v3CheckStep;

		if (rkBG.isAttrOn(v3CheckPosition.x, -v3CheckPosition.y, CTerrainImpl::ATTRIBUTE_BLOCK))
		{
			BlockMovement();
		}
	}

	D3DXVECTOR3 v3NextPosition = rv3Position + rv3MoveDirection;
	if (rkBG.isAttrOn(v3NextPosition.x, -v3NextPosition.y, CTerrainImpl::ATTRIBUTE_BLOCK))
	{
		BlockMovement();
		return TRUE;
	}
#endif

	return FALSE;
}

BOOL CInstanceBase::CheckAttacking(CInstanceBase& rkInstVictim)
{
	if (IsInSafe())
		return FALSE;

	if (rkInstVictim.IsInSafe())
		return FALSE;

	if (!m_GraphicThingInstance.AttackingProcess(rkInstVictim.m_GraphicThingInstance))
		return FALSE;

	return TRUE;
}

BOOL CInstanceBase::isNormalAttacking()
{
	return m_GraphicThingInstance.isNormalAttacking();
}

BOOL CInstanceBase::isComboAttacking()
{
	return m_GraphicThingInstance.isComboAttacking();
}

BOOL CInstanceBase::IsUsingSkill()
{
	return m_GraphicThingInstance.IsUsingSkill();
}

BOOL CInstanceBase::IsUsingMovingSkill()
{
	return m_GraphicThingInstance.IsUsingMovingSkill();
}

BOOL CInstanceBase::CanCancelSkill()
{
	return m_GraphicThingInstance.CanCancelSkill();
}

BOOL CInstanceBase::CanAttackHorseLevel()
{
	if (!IsMountingHorse())
		return FALSE;

	return m_kHorse.CanAttack();
}

bool CInstanceBase::IsAffect(UINT uAffect)
{
	return m_kAffectFlagContainer.IsSet(uAffect);
}

MOTION_KEY CInstanceBase::GetNormalAttackIndex()
{
	return m_GraphicThingInstance.GetNormalAttackIndex();
}

DWORD CInstanceBase::GetComboIndex()
{
	return m_GraphicThingInstance.GetComboIndex();
}

float CInstanceBase::GetAttackingElapsedTime()
{
	return m_GraphicThingInstance.GetAttackingElapsedTime();
}

void CInstanceBase::ProcessHitting(DWORD dwMotionKey, CInstanceBase * pVictimInstance)
{
	assert(!"-_-" && "CInstanceBase::ProcessHitting");
}

void CInstanceBase::ProcessHitting(DWORD dwMotionKey, BYTE byEventIndex, CInstanceBase * pVictimInstance)
{
	assert(!"-_-" && "CInstanceBase::ProcessHitting");
}

void CInstanceBase::GetBlendingPosition(TPixelPosition * pPixelPosition)
{
	m_GraphicThingInstance.GetBlendingPosition(pPixelPosition);
}

void CInstanceBase::SetBlendingPosition(const TPixelPosition & c_rPixelPosition)
{
	m_GraphicThingInstance.SetBlendingPosition(c_rPixelPosition);
}

void CInstanceBase::Revive()
{
	m_isGoing=FALSE;
	m_GraphicThingInstance.Revive();

	__AttachHorseSaddle();
}

void CInstanceBase::Stun()
{
	NEW_Stop();
	m_GraphicThingInstance.Stun();

	__AttachEffect(EFFECT_STUN);
}

void CInstanceBase::Die()
{
	__DetachHorseSaddle();

	if (IsAffect(AFFECT_SPAWN))
		__AttachEffect(EFFECT_SPAWN_DISAPPEAR);

	__ClearAffects();

	OnUnselected();
	OnUntargeted();

	m_GraphicThingInstance.Die();

#ifdef __AUTO_HUNT__
	if (__IsMainInstance() && CPythonPlayer::Instance().GetAutoHuntStatus())
		CPythonPlayer::Instance().SetAutoHuntDieCheck(12);
#endif
}

void CInstanceBase::Hide()
{
	m_GraphicThingInstance.SetAlphaValue(0.0f);
	m_GraphicThingInstance.BlendAlphaValue(0.0f, 0.1f);
}

void CInstanceBase::Show()
{
	m_GraphicThingInstance.SetAlphaValue(1.0f);
	m_GraphicThingInstance.BlendAlphaValue(1.0f, 0.1f);
}