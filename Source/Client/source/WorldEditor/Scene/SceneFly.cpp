#include "Stdafx.h"
#include "SceneFly.h"
#include "../../GameLib/FlyingData.h"
#include "../../GameLib/FlyingInstance.h"
#include "../../EffectLib/EffectManager.h"
#include "../../EterLib/GrpMath.h"
CSceneFly::CSceneFly()
	: start (0.0f, 300.0f, 0.0f)
{
	m_pFlyingInstance = NULL;
	m_pFlyingData = new CFlyingData;
	m_iGrippedDirection = -1;
	m_v3Target.x = start.x;
	m_v3Target.y = -start.y;
	m_v3Target.z = start.z + 100;
}

CSceneFly::~CSceneFly()
{
	if (m_pFlyingInstance)
	{
		delete m_pFlyingInstance;
	}
	m_pFlyingInstance = NULL;

	delete m_pFlyingData;
}

void CSceneFly::OnUpdate()
{

	if (m_pFlyingInstance)
	{
		D3DXVECTOR3 v3LastPosition = m_pFlyingInstance->m_v3Position;
		if (m_pFlyingData->m_bIsHoming &&
			m_pFlyingData->m_fHomingStartTime + m_pFlyingInstance->m_fStartTime < CTimer::Instance().GetCurrentSecond())
		{
			m_pFlyingInstance->AdjustDirectionForHoming (m_v3Target);
		}
		m_pFlyingInstance->Update();
		if (!m_pFlyingInstance->IsAlive())
		{
			delete m_pFlyingInstance;
			m_pFlyingInstance = 0;
		}
		else if (square_distance_between_linesegment_and_point (v3LastPosition, m_pFlyingInstance->m_v3Position,
																m_v3Target) < m_pFlyingData->m_fBombRange * m_pFlyingData->m_fBombRange)
		{
			m_pFlyingInstance->__Explode();
		}
		//else
		//Tracenf("#D:%f",sqrt(square_distance_between_linesegment_and_point(v3LastPosition,m_pFlyingInstance->m_v3Position,m_v3Target)));
	}
	CEffectManager::Instance().Update();
}

void CSceneFly::RenderFlyPosition()
{
	if (m_pFlyingInstance)
	{
		//DWORD dwCurTime = CTimer::Instance().GetCurrentMillisecond();
		const D3DXVECTOR3 & p = m_pFlyingInstance->GetPosition();
		STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0xffffff00);
		// 		RenderSphere(NULL, p.x, p.y, p.z, 5, TRUE);
		RenderSphere (NULL, p.x, p.y, p.z, 5);
		STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);
	}
}

void CSceneFly::OnRender (BOOL bClear)
{
	//CScreen::SetClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b);
	if (bClear)
	{
		CScreen::SetClearColor (0.4882f, 0.4882f, 0.4882f, 1.0f);
		CScreen::Clear();
	}

	//RenderBackGroundCharacter(start.x,start.y,start.z);
	CEffectManager::Instance().Render();

	//RenderFlyPosition();

	if (m_pFlyingInstance)
	{
		m_pFlyingInstance->Render();
		SetDiffuseColor (1, 0, 0);
		RenderLine3d (
			m_pFlyingInstance->m_v3Position.x,
			m_pFlyingInstance->m_v3Position.y,
			m_pFlyingInstance->m_v3Position.z,
			m_pFlyingInstance->m_v3Position.x + m_pFlyingInstance->m_v3Velocity.x,
			m_pFlyingInstance->m_v3Position.y + m_pFlyingInstance->m_v3Velocity.y,
			m_pFlyingInstance->m_v3Position.z + m_pFlyingInstance->m_v3Velocity.z);

		SetDiffuseColor (1, 1, 0);
		RenderLine3d (
			m_pFlyingInstance->m_v3Position.x,
			m_pFlyingInstance->m_v3Position.y,
			m_pFlyingInstance->m_v3Position.z,
			m_pFlyingInstance->m_v3Position.x + m_pFlyingInstance->m_v3Accel.x,
			m_pFlyingInstance->m_v3Position.y + m_pFlyingInstance->m_v3Accel.y,
			m_pFlyingInstance->m_v3Position.z + m_pFlyingInstance->m_v3Accel.z);
	}

	RenderGrid();

	CPickingArrows pa;
	pa.SetCenterPosition (m_v3Target);
	pa.Render();

	STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0x88ffffff);

	RenderSphere (NULL, m_v3Target.x, m_v3Target.y, m_v3Target.z, m_pFlyingData->m_fBombRange);

	STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);

}

void CSceneFly::OnRenderUI (float fx, float fy)
{
}

void CSceneFly::OnKeyDown (int iChar)
{
	if (VK_SPACE == iChar)
	{
		Play();
	}
	else if (VK_ESCAPE == iChar)
	{
		Stop();
	}
}

void CSceneFly::Play()
{
	m_dwPlayStartTime = CTimer::Instance().GetCurrentMillisecond();

	if (m_pFlyingInstance)
	{
		// Á¦°Å
		delete m_pFlyingInstance;
	}
	m_pFlyingInstance = new CFlyingInstance();
	m_pFlyingInstance->SetDataPointer (m_pFlyingData, D3DXVECTOR3 (start.x, start.y, start.z + 100));
	m_pFlyingInstance->SetFlyTarget (m_v3Target);
}

void CSceneFly::Stop()
{
	m_dwPlayStartTime = 0xffffffff;
	if (m_pFlyingInstance)
	{
		delete m_pFlyingInstance;
		m_pFlyingInstance = NULL;
	}
}

void CSceneFly::OnKeyUp (int iChar)
{
}

void CSceneFly::OnMouseMove (LONG x, LONG y)
{
	GetCursorPosition (&ms_vecMousePosition.x, &ms_vecMousePosition.y, &ms_vecMousePosition.z);

	switch (m_iGrippedDirection)
	{
		case CPickingArrows::DIRECTION_X:
			m_v3Target.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
			break;
		case CPickingArrows::DIRECTION_Y:
			m_v3Target.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
			break;
		case CPickingArrows::DIRECTION_Z:
			m_v3Target.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
			break;
		case CPickingArrows::DIRECTION_XY:
			m_v3Target.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
			m_v3Target.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
			break;
		case CPickingArrows::DIRECTION_YZ:
			m_v3Target.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
			m_v3Target.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
			break;
		case CPickingArrows::DIRECTION_ZX:
			m_v3Target.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
			m_v3Target.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
			break;
	}
}

void CSceneFly::OnLButtonDown (UINT nFlags, CPoint point)
{
	// Checking for target picking arrow
	CPickingArrows pa;
	pa.SetCenterPosition (m_v3Target);
	int iPickingDirection = pa.Picking();
	if (iPickingDirection != -1)
	{
		m_iGrippedDirection = iPickingDirection;
		m_vecGrippedPosition = ms_vecMousePosition;
		m_vecGrippedValue = m_v3Target;
	}
}

void CSceneFly::OnLButtonUp()
{
	m_iGrippedDirection = -1;
}

void CSceneFly::OnRButtonDown()
{
}

void CSceneFly::OnRButtonUp()
{
}

BOOL CSceneFly::OnMouseWheel (short zDelta)
{
	return FALSE;
}

void CSceneFly::OnMovePosition (float fx, float fy)
{
}


void CSceneFly::Initialize()
{

}

