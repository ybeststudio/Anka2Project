#include "StdAfx.h"
#include "SceneEffect.h"
#include "../../EterLib/Camera.h"

void CSceneEffect::OnKeyDown (int iChar)
{
	switch (iChar)
	{
		case VK_SPACE:
			Play();
			break;
		case VK_ESCAPE:
			Stop();
			break;
		case 0x43: // C
			if (m_dwGrippedPositionIndex != 0xffffffff)
			{
				if (m_iGrippedPointType == POINT_TYPE_POSITION)
				{
					CEffectAccessor::TEffectElement * pElement;
					if (m_pEffectAccessor->GetElement (m_dwSelectedIndex, &pElement))
					{
						CEffectElementBaseAccessor * pBase = pElement->pBase;
						TEffectPosition * pPosition;
						if (pBase->GetValuePosition (m_dwGrippedPositionIndex, &pPosition))
						{
							pPosition->m_vecPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
							RefreshTranslationDialog();
						}
					}
				}
			}
			break;
	}
}
void CSceneEffect::OnKeyUp (int iChar)
{
}
BOOL CSceneEffect::OnMouseWheel (short zDelta)
{
	if (GetAsyncKeyState (VK_LSHIFT))
	{
		D3DXVECTOR3 pv3Position = ms_Camera->GetTarget();

		if (pv3Position.z + zDelta <= -10000.0f)
		{
			zDelta = -pv3Position.z - 10000.0f;
		}
		else if (pv3Position.z + zDelta >= 10000.0f)
		{
			zDelta = -pv3Position.z + 10000.0f;
		}
		CCameraManager::Instance().GetCurrentCamera()->Move (D3DXVECTOR3 (0.0f, 0.0f, zDelta));
		return TRUE;
	}

	return FALSE;
}

void CSceneEffect::OnMouseMove (long ix, long iy)
{
	GetCursorPosition (&ms_vecMousePosition.x, &ms_vecMousePosition.y, &ms_vecMousePosition.z);

	if (0xffffffff == m_dwGrippedPositionIndex)
	{
		return;
	}

	if (-1 == m_iGrippedDirection)
	{
		return;
	}

	CEffectAccessor::TEffectElement * pElement;
	if (!m_pEffectAccessor->GetElement (m_dwSelectedIndex, &pElement))
	{
		OnLButtonUp();
		return;
	}

	CEffectElementBaseAccessor * pBase = pElement->pBase;

	TEffectPosition * pPosition;
	if (!pBase->GetValuePosition (m_dwGrippedPositionIndex, &pPosition))
	{
		OnLButtonUp();
		return;
	}

	D3DXVECTOR3 ChangingValue = m_vecGrippedValue;
	switch (m_iGrippedDirection)
	{
		case CPickingArrows::DIRECTION_X:
			ChangingValue.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
			break;
		case CPickingArrows::DIRECTION_Y:
			ChangingValue.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
			break;
		case CPickingArrows::DIRECTION_Z:
			ChangingValue.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
			break;
		case CPickingArrows::DIRECTION_XY:
			ChangingValue.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
			ChangingValue.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
			break;
		case CPickingArrows::DIRECTION_YZ:
			ChangingValue.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
			ChangingValue.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
			break;
		case CPickingArrows::DIRECTION_ZX:
			ChangingValue.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
			ChangingValue.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
			break;
	}

	switch (m_iGrippedPointType)
	{
		case POINT_TYPE_POSITION:
			pPosition->m_vecPosition = ChangingValue;
			break;
		case POINT_TYPE_CONTROL_POINT:
			pPosition->m_vecControlPoint = ChangingValue;
			break;
	}

	RefreshTranslationDialog();
}
void CSceneEffect::OnLButtonDown (UINT nFlags, CPoint point)
{
	PickingPositionGraph();
}
void CSceneEffect::OnLButtonUp()
{
	m_iGrippedDirection = -1;
}
void CSceneEffect::OnRButtonDown()
{
}
void CSceneEffect::OnRButtonUp()
{
}

void CSceneEffect::OnMovePosition (float fx, float fy)
{
	D3DXVECTOR3 pv3Position = ms_Camera->GetTarget();

	if (pv3Position.x + fx <= -1000.0f)
	{
		fx = -pv3Position.x - 1000.0f;
	}
	else if (pv3Position.x + fx >= 1000.0f)
	{
		fx = -pv3Position.x + 1000.0f;
	}
	if (pv3Position.y + fy <= -1000.0f)
	{
		fy = -pv3Position.y - 1000.0f;
	}
	else if (pv3Position.y + fy >= 1000.0f)
	{
		fy = -pv3Position.y + 1000.0f;
	}

	CCameraManager::Instance().GetCurrentCamera()->Move (D3DXVECTOR3 (fx, fy, 0.0f));
}

void CSceneEffect::OnChangeEffectPosition()
{
	if (m_dwGrippedPositionIndex == 0xffffffff)
	{
		return;
	}

	if (m_iGrippedPointType == POINT_TYPE_POSITION)
	{
		CEffectAccessor::TEffectElement * pElement;
		if (m_pEffectAccessor->GetElement (m_dwSelectedIndex, &pElement))
		{
			CEffectElementBaseAccessor * pBase = pElement->pBase;
			TEffectPosition * pPosition;
			if (pBase->GetValuePosition (m_dwGrippedPositionIndex, &pPosition))
			{
				m_pEffectTranslationDialog->GetPosition (&pPosition->m_vecPosition.x, &pPosition->m_vecPosition.y, &pPosition->m_vecPosition.z);
			}
		}
	}
}

void CSceneEffect::RefreshTranslationDialog()
{
	if (m_dwGrippedPositionIndex == 0xffffffff)
	{
		return;
	}

	if (m_iGrippedPointType == POINT_TYPE_POSITION)
	{
		CEffectAccessor::TEffectElement * pElement;
		if (m_pEffectAccessor->GetElement (m_dwSelectedIndex, &pElement))
		{
			CEffectElementBaseAccessor * pBase = pElement->pBase;
			TEffectPosition * pPosition;
			if (pBase->GetValuePosition (m_dwGrippedPositionIndex, &pPosition))
			{
				m_pEffectTranslationDialog->SetPosition (pPosition->m_vecPosition.x, pPosition->m_vecPosition.y, pPosition->m_vecPosition.z);
			}
		}
	}
}
