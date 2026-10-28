#include "StdAfx.h"
#include "SceneEffect.h"
#include "../MainFrm.h"
#include "../WorldEditor.h"
#include "../../EterLib/Camera.h"

class CPositionChangeEventReceiver : public CEffectTranslationDialog::IEventReciever
{
	public:
		CPositionChangeEventReceiver (CSceneEffect * pOwner) : m_pOwner (pOwner), CEffectTranslationDialog::IEventReciever() {}
		void OnEvent()
		{
			m_pOwner->OnChangeEffectPosition();
		}
	protected:
		CSceneEffect* m_pOwner;
};

void CSceneEffect::OnUpdate()
{
	m_iStartTime = timeGetTime();

	m_fCurrentTime = float (timeGetTime()) / 1000.0f;
	static float s_fLastTime = m_fCurrentTime;

	float fElapsedTime = m_fCurrentTime - s_fLastTime;
	s_fLastTime = m_fCurrentTime;

	for (TEffectInstanceAccessorList::iterator itorInstance = m_EffectInstanceAccessorList.begin(); itorInstance != m_EffectInstanceAccessorList.end();)
	{
		CEffectInstanceAccessor * pEffectInstanceAccessor = *itorInstance;

		pEffectInstanceAccessor->Update (/*fElapsedTime*/);

		if (!pEffectInstanceAccessor->isAlive())
		{
			pEffectInstanceAccessor->Clear();
			m_EffectInstanceAccessorPool.Free (pEffectInstanceAccessor);
			itorInstance = m_EffectInstanceAccessorList.erase (itorInstance);
		}
		else
		{
			++itorInstance;
		}
	}

	DWORD dwCount = 0;
	bool bMakingFlag = false;
	for (TStartTimeVector::iterator itor = m_StartTimeVector.begin(); itor != m_StartTimeVector.end();)
	{
		float fTime = *itor;

		if (m_fCurrentTime - fTime > m_pEffectAccessor->GetLifeTime())
		{
			itor = m_StartTimeVector.erase (itor);

			if (m_isLoop)
				if (dwCount == m_dwLoopStartTimeIndex)
				{
					bMakingFlag = true;
				}
		}
		else
		{
			++itor;
		}

		++dwCount;
	}

	if (bMakingFlag)
	{
		Play();
		m_dwLoopStartTimeIndex = m_StartTimeVector.size() - 1;
	}

	// TODO[FIXME]: Display Object and Motion in SceneEffect using SceneObject
	if (m_bUsingSceneObject)
	{
		CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
		CSceneObject * pObject = pApp->GetSceneObject();
		pObject->OnUpdate();
	}
}

void CSceneEffect::OnRenderUI (float fx, float fy)
{
	CScreen::SetDiffuseColor (0.0f, 0.0f, 0.0f, 0.3f);
	CScreen::RenderBar2d (fx, fy, fx + 300.0f, fy + 60.0f);

	char szMsg[128 + 1];

	// draw time; iCurrentTime - iLastTime;
	if (1000.0 / (m_iMaxTime - m_iMinTime) < 60)
	{
		m_TextInstanceFrame.SetColor (1.0f, 0.0f, 0.0f);
	}
	else if (1000.0 / (m_iMaxTime - m_iMinTime) < 100)
	{
		m_TextInstanceFrame.SetColor (1.0f, 0.5f, 0.0f);
	}
	else
	{
		m_TextInstanceFrame.SetColor (1.0f, 1.0f, 1.0f);
	}

	_snprintf (szMsg, 128, "Max FPS : %d", 1000 / max (1, m_iMaxTime - m_iMinTime));
	m_TextInstanceFrame.SetValue (szMsg, strlen (szMsg));
	m_TextInstanceFrame.SetPosition (fx + 10.0f, fy + 10.0f);
	m_TextInstanceFrame.Update();
	m_TextInstanceFrame.Render();

	_snprintf (szMsg, 128, "Effect Time : %d", m_iMaxTime - m_iMinTime);
	m_TextInstanceElapsedTime.SetValue (szMsg, strlen (szMsg));
	m_TextInstanceElapsedTime.SetPosition (fx + 10.0f, fy + 30.0f);
	m_TextInstanceElapsedTime.Update();
	m_TextInstanceElapsedTime.Render();
}

void CSceneEffect::OnRender (BOOL bClear)
{
	CScreen::SetClearColor (m_ClearColor.r, m_ClearColor.g, m_ClearColor.b);
	CScreen::Clear();

	// TODO[FIXME]: Display Object and Motion in SceneEffect using SceneObject
	if (m_bUsingSceneObject)
	{
		CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
		CSceneObject * pObject = pApp->GetSceneObject();
		pObject->OnRender (bClear);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	const TEnvironmentData * c_pEnvironmentData;
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	pMapManagerAccessor->CMapManagerAccessor::GetEnvironmentData (&c_pEnvironmentData);
	pMapManagerAccessor->CMapManager::SetEnvironmentDataPtr (c_pEnvironmentData);

	pMapManagerAccessor->BeginEnvironment();
	///////////////////////////////////////////////////////////////////////////////////////////////

	if (!m_bUsingSceneObject && m_bShowCharacter)
	{
		RenderBackGroundCharacter (0.0f, 0.0f, 0.0f);
	}
	RenderBackGroundTile();

	if (m_bShowCursor)
	{
		SetDiffuseColor (0.0f, 0.0f, 0.0f);
		RenderLine3d (-1000, 0, 0, 1000, 0, 0);
		RenderLine3d (0, -1000, 0, 0, 1000, 0);
		RenderLine3d (0, 0, -1000, 0, 0, 1000);
	}

	STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0xffffffff);
	for (TEffectInstanceAccessorList::iterator itorInstance = m_EffectInstanceAccessorList.begin(); itorInstance != m_EffectInstanceAccessorList.end();)
	{
		CEffectInstanceAccessor * pEffectInstanceAccessor = *itorInstance;
		pEffectInstanceAccessor->Render();
		++itorInstance;
	}
	STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);

	if (m_bShowCursor)
	{
		RenderEffectPosition();
		RenderPositionGraph();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	pMapManagerAccessor->EndEnvironment();
	///////////////////////////////////////////////////////////////////////////////////////////////

	if (m_bShowBoundingSphere)
	{
		RenderBoundingSphere();
	}

	/////////////////////////////////////////////
	int iCurrentTime = timeGetTime();

	// draw time; iCurrentTime - iLastTime;
	int ElapsedTime = iCurrentTime - m_iStartTime;
	if (ElapsedTime < 100)
	{
		if (m_iMinTime > ElapsedTime)
		{
			m_iMinTime = ElapsedTime;
		}
		if (m_iMaxTime < ElapsedTime)
		{
			m_iMaxTime = ElapsedTime;
		}
	}

	m_iStartTime = iCurrentTime;
	/////////////////////////////////////////////
}

void CSceneEffect::SetCursorVisibility (BOOL bShow)
{
	m_bShowCursor = bShow;
}

void CSceneEffect::SetCharacterVisibility (BOOL bShow)
{
	m_bShowCharacter = bShow;
}

void CSceneEffect::SetBoundingSphereVisibility (BOOL bShow)
{
	m_bShowBoundingSphere = bShow;
}

void CSceneEffect::UsingSceneObject (BOOL bUsing)
{
	m_bUsingSceneObject = bUsing;
}

void CSceneEffect::RenderPositionGraph()
{
	CEffectAccessor::TEffectElement * pElement;
	if (!m_pEffectAccessor->GetElement (m_dwSelectedIndex, &pElement))
	{
		return;
	}

	CEffectElementBaseAccessor * pBase = pElement->pBase;

	CPickingArrows PickingArrows;
	TEffectPosition * pPosition;
	TEffectPosition * pNextPosition;

	DWORD i;
	DWORD iPositionCount = pBase->GetPositionCount();
	for (i = 0; i < iPositionCount; ++i)
	{
		if (!pElement->pBase->GetValuePosition (i, &pPosition))
		{
			continue;
		}

		STATEMANAGER.SaveRenderState (D3DRS_ZENABLE, false);
		if (i == m_dwGrippedPositionIndex)
		{
			STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0xffffff00);
		}
		else
		{
			STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0xffffffff);
		}

		D3DXVECTOR3& pos = pPosition->m_vecPosition;
		RenderLine3d (pos.x, pos.y, 0.0f, pos.x, pos.y, pos.z);
		RenderCircle3d (pos.x, pos.y, pos.z, 5.0f, 10);
		RenderCircle2d (pos.x, pos.y, pos.z, 5.0f, 10);
		RenderSphere (NULL, pos.x, pos.y, pos.z, 4.0f);

		if (i == m_dwGrippedPositionIndex)
		{
			if (pPosition->m_iMovingType == MOVING_TYPE_BEZIER_CURVE)
			{
				STATEMANAGER.SetRenderState (D3DRS_TEXTUREFACTOR, 0xff00ffff);
				RenderSphere (NULL,
							  pos.x + pPosition->m_vecControlPoint.x,
							  pos.y + pPosition->m_vecControlPoint.y,
							  pos.z + pPosition->m_vecControlPoint.z,
							  3.0f);
			}

			STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);
			PickingArrows.SetCenterPosition (pPosition->m_vecPosition);
			PickingArrows.Render();

			if (pPosition->m_iMovingType == MOVING_TYPE_BEZIER_CURVE)
			{
				// picking arrow for control point
				PickingArrows.SetScale (0.6f);
				PickingArrows.SetCenterPosition (pPosition->m_vecPosition + pPosition->m_vecControlPoint);
				PickingArrows.Render();
			}

		}
		else
		{
			STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);
		}
		STATEMANAGER.RestoreRenderState (D3DRS_ZENABLE);
	}

	if (iPositionCount)
		for (i = 0; i < iPositionCount - 1; ++i)
		{
			if (!pElement->pBase->GetValuePosition (i, &pPosition))
			{
				continue;
			}
			if (!pElement->pBase->GetValuePosition (i + 1, &pNextPosition))
			{
				continue;
			}

			STATEMANAGER.SaveRenderState (D3DRS_ZENABLE, FALSE);

			SetDiffuseColor (1.0f, 1.0f, 1.0f, 0.5f);
			if (MOVING_TYPE_DIRECT == pPosition->m_iMovingType)
			{
				RenderLine3d (pPosition->m_vecPosition.x, pPosition->m_vecPosition.y, pPosition->m_vecPosition.z,
							  pNextPosition->m_vecPosition.x, pNextPosition->m_vecPosition.y, pNextPosition->m_vecPosition.z);
			}
			else
			{
				// MOVING_TYPE_BEZIER_CURVE

				int i;
				D3DXVECTOR3 lastPos = pPosition->m_vecPosition;
				D3DXVECTOR3 pos;
				for (i = 1; i <= 10; i++)
				{
					pElement->pBase->GetPosition (pPosition->m_fTime * (10 - i) / 10.0f + pNextPosition->m_fTime * i / 10.0f, pos);
					RenderLine3d (lastPos.x, lastPos.y, lastPos.z, pos.x, pos.y, pos.z);
					lastPos = pos;
				}

				SetDiffuseColor (1.0f, 0.0f, 0.0f, 0.5f);
				RenderLine3d (
					pPosition->m_vecPosition.x + pPosition->m_vecControlPoint.x,
					pPosition->m_vecPosition.y + pPosition->m_vecControlPoint.y,
					pPosition->m_vecPosition.z + pPosition->m_vecControlPoint.z,
					pPosition->m_vecPosition.x, pPosition->m_vecPosition.y, pPosition->m_vecPosition.z);
				RenderLine3d (
					pPosition->m_vecPosition.x + pPosition->m_vecControlPoint.x,
					pPosition->m_vecPosition.y + pPosition->m_vecControlPoint.y,
					pPosition->m_vecPosition.z + pPosition->m_vecControlPoint.z,
					pNextPosition->m_vecPosition.x, pNextPosition->m_vecPosition.y, pNextPosition->m_vecPosition.z);
			}
			STATEMANAGER.RestoreRenderState (D3DRS_ZENABLE);
		}
}

void CSceneEffect::RenderBoundingSphere()
{
	float fRadius = m_pEffectAccessor->GetBoundingSphereRadius();
	const D3DXVECTOR3 & c_rv3Pos = m_pEffectAccessor->GetBoundingSpherePosition();

	if (fRadius <= 0.0f)
	{
		return;
	}

	STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0xffff0000);
	RenderSphere (NULL, c_rv3Pos.x, c_rv3Pos.y, c_rv3Pos.z, fRadius, D3DFILL_WIREFRAME);
	STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);
}

void CSceneEffect::RenderEffectPosition()
{
	CEffectAccessor::TEffectElement * pElement;
	if (!m_pEffectAccessor->GetElement (m_dwSelectedIndex, &pElement))
	{
		return;
	}

	float fcurTime = timeGetTime() / 1000.0f;

	TStartTimeVector::iterator it;
	for (it = m_StartTimeVector.begin(); it != m_StartTimeVector.end(); ++it)
	{
		// (fcurTime-*it) : passed time
		D3DXVECTOR3 pos;
		pElement->pBase->GetPosition (fcurTime - *it, pos);
		STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0xcc00cccc);
		RenderSphere (NULL, pos.x, pos.y, pos.z, 3.0f);
		STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);
	}
}

void CSceneEffect::DeselectGrippedPosition()
{
	m_iGrippedPointType = POINT_TYPE_POSITION;
	m_dwGrippedPositionIndex = 0xffffffff;
	m_iGrippedDirection = -1;
}

DWORD CSceneEffect::GetSelectedPositionIndex()
{
	return m_dwGrippedPositionIndex;
}

void CSceneEffect::SetSelectedPositionIndex (DWORD dwNewIndex)
{
	m_dwGrippedPositionIndex = dwNewIndex;
}

void CSceneEffect::PickingPositionGraph()
{
	CEffectAccessor::TEffectElement * pElement;
	if (!m_pEffectAccessor->GetElement (m_dwSelectedIndex, &pElement))
	{
		return;
	}

	CEffectElementBaseAccessor * pBase = pElement->pBase;

	TEffectPosition * pPosition;

	if (m_dwGrippedPositionIndex != 0xffffffff && pBase->GetValuePosition (m_dwGrippedPositionIndex, &pPosition))
	{
		CPickingArrows PickArrow;

		PickArrow.SetCenterPosition (pPosition->m_vecPosition);
		int iPickedDirection = PickArrow.Picking();
		if (iPickedDirection != -1)
		{
			m_iGrippedDirection = iPickedDirection;
			m_vecGrippedValue = pPosition->m_vecPosition;
			m_vecGrippedPosition = ms_vecMousePosition;
			m_iGrippedPointType = POINT_TYPE_POSITION;
			return;
		}

		PickArrow.SetScale (0.6f);
		PickArrow.SetCenterPosition (pPosition->m_vecPosition + pPosition->m_vecControlPoint);
		iPickedDirection = PickArrow.Picking();
		if (iPickedDirection != -1)
		{
			m_iGrippedDirection = iPickedDirection;
			m_vecGrippedValue = pPosition->m_vecControlPoint;
			m_vecGrippedPosition = ms_vecMousePosition;
			m_iGrippedPointType = POINT_TYPE_CONTROL_POINT;
			return;
		}
	}

	std::vector<DWORD> GrippedIndexVector;

	for (DWORD i = 0; i < pBase->GetPositionCount(); ++i)
	{
		if (!pBase->GetValuePosition (i, &pPosition))
		{
			continue;
		}
		if (IntersectSphere (pPosition->m_vecPosition, 5, ms_vtPickRayOrig, ms_vtPickRayDir))
		{
			GrippedIndexVector.push_back (i);
		}
	}

	if (GrippedIndexVector.empty())
	{
		// TODO : check for control point
	}
	else
	{
		std::vector<DWORD>::iterator it;

		DWORD iNextSelection = 0xffffffff;

		if ((it = std::find (GrippedIndexVector.begin(), GrippedIndexVector.end(), m_dwGrippedPositionIndex)) != GrippedIndexVector.end())
		{
			++it;
		}

		if (it == GrippedIndexVector.end())
		{
			iNextSelection = GrippedIndexVector.front();
		}
		else
		{
			iNextSelection = *it;
		}

		if (iNextSelection != m_dwGrippedPositionIndex)
		{
			m_dwGrippedPositionIndex = iNextSelection;
			m_iGrippedPointType = POINT_TYPE_POSITION;

			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			pFrame->UpdateEffectControlBar();

			m_pEffectTranslationDialog->ShowWindow (TRUE);
			RefreshTranslationDialog();
		}
		else
		{
			// TODO : check for control point
			DeselectGrippedPosition();

			m_pEffectTranslationDialog->ShowWindow (FALSE);
		}
	}
}

void CSceneEffect::Play()
{
	float fcurTime = timeGetTime() / 1000.0f;
	m_StartTimeVector.push_back (fcurTime);

	CEffectInstanceAccessor * pEffectInstanceAccessor = m_EffectInstanceAccessorPool.Alloc();
	pEffectInstanceAccessor->Clear();
	pEffectInstanceAccessor->SetEffectDataAccessorPointer (m_pEffectAccessor);
	pEffectInstanceAccessor->SetPosition (D3DXVECTOR3 (0.0f, 0.0f, 0.0f));
	m_EffectInstanceAccessorList.push_back (pEffectInstanceAccessor);

	// TODO[FIXME]: Display Object and Motion in SceneEffect using SceneObject
	if (m_bUsingSceneObject)
	{
		CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
		CSceneObject * pObject = pApp->GetSceneObject();
		pObject->Play();
	}
}

void CSceneEffect::PlayLoop()
{
	m_isLoop = true;

	Play();
	m_dwLoopStartTimeIndex = m_StartTimeVector.size() - 1;
}

void CSceneEffect::StopLoop()
{
	m_isLoop = false;
}

void CSceneEffect::Stop()
{
	TEffectInstanceAccessorList::iterator itor = m_EffectInstanceAccessorList.begin();
	for (; itor != m_EffectInstanceAccessorList.end(); ++itor)
	{
		m_EffectInstanceAccessorPool.Free (*itor);
	}
	m_EffectInstanceAccessorList.clear();
	m_StartTimeVector.clear();

	m_isLoop = false;

	m_iMaxTime = 0;
	m_iMinTime = 999999;
}

DWORD CSceneEffect::GetStartTimeCount()
{
	return m_StartTimeVector.size();
}

bool CSceneEffect::GetStartTime (DWORD dwIndex, float* pTime)
{
	if (dwIndex >= m_StartTimeVector.size())
	{
		return false;
	}

	*pTime = m_fCurrentTime - m_StartTimeVector[dwIndex];

	return true;
}

void CSceneEffect::SelectEffectElement (DWORD dwIndex)
{
	m_dwSelectedIndex = dwIndex;
}

void CSceneEffect::SetEffectAccessor (CEffectAccessor * pData)
{
	m_pEffectAccessor = pData;
}

void CSceneEffect::UpdatePreviewInstance()
{
}

void CSceneEffect::Initialize()
{
	CResource * pResource = CResourceManager::Instance().GetTypeResourcePointer ("±¼¸²Ã¼:15.fnt");
	CGraphicText * pText = static_cast<CGraphicText*> (pResource);
	m_TextInstanceFrame.Destroy();
	m_TextInstanceFrame.SetTextPointer (pText);
	m_TextInstanceFrame.SetColor (1.0f, 1.0f, 1.0f);
	m_TextInstanceElapsedTime.Destroy();
	m_TextInstanceElapsedTime.SetTextPointer (pText);
	m_TextInstanceElapsedTime.SetColor (1.0f, 1.0f, 1.0f);
}

CSceneEffect::CSceneEffect()
{
	m_pEffectAccessor = NULL;
	m_dwSelectedIndex = 0;

	m_iGrippedPointType = POINT_TYPE_POSITION;
	m_dwGrippedPositionIndex = 0xffffffff;
	m_iGrippedDirection = -1;

	m_vecGrippedPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	m_vecGrippedValue = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

	m_isLoop = false;
	m_dwLoopStartTimeIndex = 0;

	m_ClearColor = D3DXCOLOR (0.4882f, 0.4882f, 0.4882f, 1.0f);

	m_bUsingSceneObject = FALSE;
	m_bShowCursor = TRUE;
	m_bShowCharacter = TRUE;
	m_bShowBoundingSphere = TRUE;

	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	m_pEffectTranslationDialog = new CEffectTranslationDialog (new CPositionChangeEventReceiver (this));
	m_pEffectTranslationDialog->Create (CEffectTranslationDialog::IDD, pApp->GetMainWnd());
	m_pEffectTranslationDialog->ShowWindow (FALSE);
	::SetWindowPos (m_pEffectTranslationDialog->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	m_iStartTime = 0;
	m_iMaxTime = 0;
	m_iMinTime = 999999;
}
CSceneEffect::~CSceneEffect()
{
	delete m_pEffectTranslationDialog;
	m_pEffectTranslationDialog = NULL;
}
