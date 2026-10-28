// ObjectAnimationEventGraph.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventGraph.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventGraph

CObjectAnimationEventGraph::CObjectAnimationEventGraph()
{
	m_dwcurHitDataIndex = 0;

	m_pOverNode = NULL;
	m_pSelectedNode = NULL;
	m_pPickedNode = NULL;
	m_pMotionData = NULL;

	m_WindowRect.top = 0;
	m_WindowRect.bottom = 0;
	m_WindowRect.left = 0;
	m_WindowRect.right = 0;
	m_fDuration = 1.0f;
	m_isFrameStep = FALSE;
	m_dwcurFrame = 0;

	for (DWORD i = 0; i < COMBO_NODE_MAX_NUM; ++i)
	{
		m_ComboNode[i].isOverriding = FALSE;
		m_ComboNode[i].isPicking = FALSE;
		m_ComboNode[i].fPosition = 0.0f;
	}
	for (DWORD j = 0; j < ATTACKING_NODE_MAX_NUM; ++j)
	{
		m_AttackingNode[j].isOverriding = FALSE;
		m_AttackingNode[j].isPicking = FALSE;
		m_AttackingNode[j].fPosition = 0.0f;
	}
	for (DWORD k = 0; k < LOOP_NODE_MAX_NUM; ++k)
	{
		m_LoopNode[k].isOverriding = FALSE;
		m_LoopNode[k].isPicking = FALSE;
		m_LoopNode[k].fPosition = 0.0f;
	}
}

CObjectAnimationEventGraph::~CObjectAnimationEventGraph()
{
}

BEGIN_MESSAGE_MAP (CObjectAnimationEventGraph, CStatic)
	//{{AFX_MSG_MAP(CObjectAnimationEventGraph)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventGraph normal functions

void CObjectAnimationEventGraph::Create()
{
	CGraphicText * pText = (CGraphicText*)CResourceManager::Instance().GetTypeResourcePointer ("±¼¸²Ã¼:11.fnt");

	for (DWORD i = 0; i < COMBO_NODE_MAX_NUM; ++i)
	{
		m_ComboNode[i].TextInstance.SetHorizonalAlign (CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		m_ComboNode[i].TextInstance.SetTextPointer (pText);
		m_ComboNode[i].TextInstance.SetColor (0.0f, 0.0f, 0.0f);
	}

	m_ComboNode[INPUT_START].TextInstance.SetValue ("INPUT START");
	m_ComboNode[NEXT_COMBO].TextInstance.SetValue ("NEXT COMBO!");
	m_ComboNode[INPUT_END].TextInstance.SetValue ("INPUT END");

	for (DWORD j = 0; j < LOOP_NODE_MAX_NUM; ++j)
	{
		CGraphicTextInstance & rTextInstance = m_LoopNode[j].TextInstance;
		rTextInstance.SetVerticalAlign (CGraphicTextInstance::VERTICAL_ALIGN_CENTER);
		rTextInstance.SetTextPointer (pText);
		rTextInstance.SetColor (0.0f, 0.0f, 0.0f);
	}
	m_LoopNode[LOOP_NODE_START].TextInstance.SetValue ("Loop Start");
	m_LoopNode[LOOP_NODE_END].TextInstance.SetValue ("Loop End");

	m_FrameIndexInstance.SetHorizonalAlign (CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
	m_FrameIndexInstance.SetVerticalAlign (CGraphicTextInstance::VERTICAL_ALIGN_CENTER);
	m_FrameIndexInstance.SetTextPointer (pText);
	m_FrameIndexInstance.SetColor (0.0f, 0.0f, 0.0f);
}

void CObjectAnimationEventGraph::Render()
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity (&matIdentity);
	STATEMANAGER.SaveTransform (D3DTS_PROJECTION, &matIdentity);
	STATEMANAGER.SaveTransform (D3DTS_VIEW, &matIdentity);
	STATEMANAGER.SetTransform (D3DTS_WORLD, &matIdentity);
	STATEMANAGER.SaveRenderState (D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SetRenderState (D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
	STATEMANAGER.SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_Screen.SetOrtho2D (1024.0f, 768.0f, 400.0f);
	m_Screen.SetClearColor (1.0f, 1.0f, 1.0f);
	m_Screen.Clear();
	m_Screen.Begin();

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Attacking Range
	if (m_pMotionData->isAttackingMotion())
	{
		RenderAttackingRange();
	}

	// Combo Data
	if (m_pMotionData->IsComboInputTimeData())
	{
		RenderComboData();
	}

	// Motion Event
	if (m_pSelectedNode)
	{
		RenderNodePicked (NODE_TYPE_LEFT, m_pSelectedNode->fPosition);
	}
	if (m_pPickedNode)
	{
		RenderNodePicked (NODE_TYPE_LEFT, m_pPickedNode->fPosition);
	}
	else if (m_pOverNode)
	{
		if (m_pOverNode != m_pSelectedNode)
		{
			RenderNodeOver (NODE_TYPE_LEFT, m_pOverNode->fPosition);
		}
	}

	for (auto itor = m_EventNodeList.begin(); itor != m_EventNodeList.end(); ++itor)
	{
		TEventNode * pEventNode = *itor;
		RenderNodeOutline (NODE_TYPE_LEFT, pEventNode->fPosition);
	}

	// Loop Start Time
	if (m_pMotionData->IsLoopMotion())
	{
		for (DWORD i = 0; i < LOOP_NODE_MAX_NUM; ++i)
		{
			TNodeInstance & rNode = m_LoopNode[i];

			float fPosition = rNode.fPosition;
			m_Screen.SetDiffuseColor (0.0f, 0.0f, 0.0f);
			m_Screen.RenderLine2d (3.0f, fPosition, 7.0f, fPosition);
			if (rNode.isOverriding || rNode.isPicking)
			{
				RenderNodeOver (NODE_TYPE_LEFT, fPosition);
			}

			rNode.TextInstance.SetPosition (10.0f, rNode.fPosition);
			rNode.TextInstance.Update();
			rNode.TextInstance.Render();
		}
	}

	if (m_isFrameStep)
	{
		static char szFrameText[32 + 1];
		_snprintf (szFrameText, 32, "%d", m_dwcurFrame);
		m_FrameIndexInstance.SetValue (szFrameText);
		m_FrameIndexInstance.Update();
		m_FrameIndexInstance.Render();
	}
	///////////////////////////////////////////////////////////////////////////////////////////////

	m_Screen.End();
	m_Screen.Show (&m_WindowRect, GetSafeHwnd());
	STATEMANAGER.RestoreTransform (D3DTS_VIEW);
	STATEMANAGER.RestoreTransform (D3DTS_PROJECTION);
	STATEMANAGER.RestoreRenderState (D3DRS_ALPHABLENDENABLE);
}

const float c_fNodeWidth = 35.0f;
const float c_fNodeHalfWidth = c_fNodeWidth / 2.0f;
const float c_fNodeHalfHeight = 5.0f;

void CObjectAnimationEventGraph::RenderNodeOutline (ENodeType eType, float fPosition)
{
	switch (eType)
	{
		case NODE_TYPE_LEFT:
			m_Screen.SetDiffuseColor (0.0f, 0.0f, 0.0f);
			m_Screen.RenderLine2d (3.0f, fPosition, 7.0f, fPosition);
			m_Screen.RenderBox2d (10.0f, fPosition - c_fNodeHalfHeight, 10.0f + c_fNodeWidth, fPosition + c_fNodeHalfHeight);
			break;
		case NODE_TYPE_RIGHT:
			m_Screen.SetDiffuseColor (0.0f, 0.0f, 0.0f);
			m_Screen.RenderLine2d (m_WindowRect.Width() - 7.0f, fPosition, m_WindowRect.Width() - 3.0f, fPosition);
			break;
	}
}
void CObjectAnimationEventGraph::RenderNodeOver (ENodeType eType, float fPosition)
{
	m_Screen.SetDiffuseColor (1.0f, 0.0f, 0.0f, 0.5f);
	switch (eType)
	{
		case NODE_TYPE_LEFT:
			m_Screen.RenderBar2d (10.0f, fPosition - c_fNodeHalfHeight, 10.0f + c_fNodeWidth, fPosition + c_fNodeHalfHeight);
			break;
		case NODE_TYPE_RIGHT:
			m_Screen.RenderBar2d (m_WindowRect.Width() - 40.0f - 10.0f, fPosition - 7.0f,
								  m_WindowRect.Width() - 10.0f, fPosition + 7.0f);
			break;
	}
}
void CObjectAnimationEventGraph::RenderNodePicked (ENodeType eType, float fPosition)
{
	m_Screen.SetDiffuseColor (0.5f, 0.0f, 0.0f);
	switch (eType)
	{
		case NODE_TYPE_LEFT:
			m_Screen.RenderBar2d (10.0f, fPosition - c_fNodeHalfHeight, 10.0f + c_fNodeWidth, fPosition + c_fNodeHalfHeight);
			break;
		case NODE_TYPE_RIGHT:
			break;
	}
}

BOOL CObjectAnimationEventGraph::isOverNode (ENodeType eType, float fPosition, const CPoint & c_rPoint)
{
	switch (eType)
	{
		case NODE_TYPE_LEFT:
			if (fabs (fPosition - float (c_rPoint.y)) <= c_fNodeHalfHeight)
				if (c_rPoint.x >= 10.0f)
					if (c_rPoint.x <= 10.0f + c_fNodeWidth)
					{
						return TRUE;
					}
			break;
		case NODE_TYPE_RIGHT:
			if (fabs (fPosition - float (c_rPoint.y)) <= c_fNodeHalfHeight)
				if (c_rPoint.x >= m_WindowRect.Width() - c_fNodeWidth - 10.0f)
					if (c_rPoint.x <= m_WindowRect.Width() - 10.0f)
					{
						return TRUE;
					}
			break;
	}

	return FALSE;
}

void CObjectAnimationEventGraph::RenderComboData()
{
	for (DWORD i = 0; i < COMBO_NODE_MAX_NUM; ++i)
	{
		TNodeInstance & rNode = m_ComboNode[i];

		if (rNode.isOverriding)
		{
			m_Screen.SetDiffuseColor (1.0f, 0.0f, 0.0f);
			RenderNodeOver (NODE_TYPE_RIGHT, rNode.fPosition);
		}
		m_Screen.SetDiffuseColor (0.0f, 0.0f, 0.0f);
		RenderNodeOutline (NODE_TYPE_RIGHT, rNode.fPosition);

		rNode.TextInstance.SetPosition (m_WindowRect.Width() - 30.0f, rNode.fPosition - 4.0f);
		rNode.TextInstance.Update();
		rNode.TextInstance.Render();
	}
}

void CObjectAnimationEventGraph::RenderAttackingRange()
{
	float fCenter = (m_WindowRect.Width()) / 2.0f;

	// Start
	for (DWORD i = 0; i < 2; ++i)
	{
		TNodeInstance & rNode = m_AttackingNode[i];
		if (rNode.isOverriding)
		{
			m_Screen.SetDiffuseColor (1.0f, 0.0f, 0.0f);
			RenderAttackingNode (fCenter, rNode.fPosition);
		}
		m_Screen.SetDiffuseColor (0.0f, 0.0f, 0.0f);
		m_Screen.RenderBox2d (fCenter - c_fNodeHalfWidth, rNode.fPosition - c_fNodeHalfHeight,
							  fCenter + c_fNodeHalfWidth, rNode.fPosition + c_fNodeHalfHeight);
	}

	m_Screen.SetDiffuseColor (1.0f, 0.0f, 0.0f, 0.2f);
	m_Screen.RenderBar2d (2.0f, m_AttackingNode[0].fPosition,
						  m_WindowRect.Width() - 2.0f, m_AttackingNode[1].fPosition);
}

void CObjectAnimationEventGraph::RenderAttackingNode (float fCenter, float fPosition)
{
	m_Screen.RenderBar2d (fCenter - c_fNodeHalfWidth, fPosition - c_fNodeHalfHeight,
						  fCenter + c_fNodeHalfWidth, fPosition + c_fNodeHalfHeight);
}

BOOL CObjectAnimationEventGraph::isOverridingAttacking (float fCenter, float fPosition, CPoint & c_rPoint)
{
	if (fabs (fPosition - float (c_rPoint.y)) <= 5.0f)
		if (c_rPoint.x >= fCenter - c_fNodeHalfWidth)
			if (c_rPoint.x <= fCenter + c_fNodeHalfWidth)
			{
				return TRUE;
			}

	return FALSE;
}

void CObjectAnimationEventGraph::SetMotionData (CRaceMotionDataAccessor * pMotionData)
{
	m_pMotionData = pMotionData;

	Refresh();

	//

	m_pMotionData->SetMotionDuration (m_fDuration);

	__UpdateAttackingNode();

	if (m_pMotionData->IsComboInputTimeData())
	{
		m_ComboNode[INPUT_START].fPosition = TimeToPosition (m_pMotionData->GetComboInputStartTime());
		m_ComboNode[NEXT_COMBO].fPosition = TimeToPosition (m_pMotionData->GetNextComboTime());
		m_ComboNode[INPUT_END].fPosition = TimeToPosition (m_pMotionData->GetComboInputEndTime());
	}
	if (m_pMotionData->IsLoopMotion())
	{
		m_LoopNode[LOOP_NODE_START].fPosition = TimeToPosition (m_pMotionData->GetLoopStartTime());
		m_LoopNode[LOOP_NODE_END].fPosition = TimeToPosition (m_pMotionData->GetLoopEndTime());
	}
}

void CObjectAnimationEventGraph::SetCurrentHitDataIndex (int iIndex)
{
	m_dwcurHitDataIndex = iIndex;
	__UpdateAttackingNode();
	Render();
}

void CObjectAnimationEventGraph::ToggleFrameStep()
{
	m_isFrameStep = 1 - m_isFrameStep;
}

void CObjectAnimationEventGraph::__UpdateAttackingNode()
{
	const NRaceData::THitData * c_pHitData = m_pMotionData->GetHitDataPtr (m_dwcurHitDataIndex);
	if (c_pHitData)
	{
		m_AttackingNode[START].fPosition = TimeToPosition (c_pHitData->fAttackStartTime);
		m_AttackingNode[END].fPosition = TimeToPosition (c_pHitData->fAttackEndTime);
	}
}

void CObjectAnimationEventGraph::ClearEventNode()
{
	for (TEventNodeListiterator itor = m_EventNodeList.begin(); itor != m_EventNodeList.end(); ++itor)
	{
		TEventNode * pEventNode = *itor;
		m_EventNodePool.Free (pEventNode);
	}
	m_EventNodeList.clear();
}

float CObjectAnimationEventGraph::PositionToTime (float fPosition)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	return fPosition / m_WindowRect.Height() * m_fDuration;
}
float CObjectAnimationEventGraph::TimeToPosition (float fTime)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	float fPosition = fTime / m_fDuration * m_WindowRect.Height();

	fPosition = fMAX (0.0f, fPosition);
	fPosition = fMIN (float (m_WindowRect.Height()), fPosition);

	return fPosition;
}

void CObjectAnimationEventGraph::Refresh()
{
	CheckDlgButton (IDC_OBJECT_ANIMATION_ATTACKING_ENABLE, m_pMotionData->isAttackingMotion());
	RefreshGraph();
}

void CObjectAnimationEventGraph::RefreshGraph()
{
	m_pOverNode = NULL;
	m_pSelectedNode = NULL;
	m_pPickedNode = NULL;
	ClearEventNode();

	/////

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	GetClientRect (&m_WindowRect);

	if (!m_pMotionData)
	{
		return;
	}

	if (!pSceneObject->isMotionData())
	{
		return;
	}

	m_fDuration = pSceneObject->GetMotionDuration();

	// Event Node
	for (DWORD i = 0; i < m_pMotionData->GetMotionEventDataCount(); ++i)
	{
		const CRaceMotionData::TMotionEventData * pMotionEventData;
		if (! ((const CRaceMotionData*)m_pMotionData)->GetMotionEventDataPointer (i, &pMotionEventData))
		{
			continue;
		}

		TEventNode * pEventNode = m_EventNodePool.Alloc();
		pEventNode->dwIndex = i;
		pEventNode->fPosition = TimeToPosition (pMotionEventData->fStartingTime);
		pEventNode->c_pEventData = pMotionEventData;
		m_EventNodeList.push_back (pEventNode);
	}
}

void CObjectAnimationEventGraph::MakeEventNode (float fPosition)
{
	CObjectAnimationEvent NewEvent;

	if (TRUE == NewEvent.DoModal())
	{
		DWORD dwIndex = m_pMotionData->MakeEvent (NewEvent.GetEventType());

		//GetEventNodeData(NewEvent, dwIndex);
		CRaceMotionDataAccessor::TMotionEventData * pData;
		if (m_pMotionData->GetMotionEventDataPointer (dwIndex, &pData))
		{
			NewEvent.GetMotionEventData (pData);
		}

		m_pMotionData->MovePosition (dwIndex, PositionToTime (fPosition));
		RefreshGraph();
	}
}

void CObjectAnimationEventGraph::DeleteEventNode (TEventNode * pEventNode)
{
	if (IDYES != MessageBox ("Are you sure you want to delete it?", NULL, MB_YESNO))
	{
		return;
	}

	m_pMotionData->DeleteEvent (pEventNode->dwIndex);
	RefreshGraph();
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventGraph message handlers

void CObjectAnimationEventGraph::OnMouseMove (UINT nFlags, CPoint point)
{
	int iMousePosY = point.y;
	if (m_isFrameStep)
	{
		const float c_fFrameTime = 1.0f / 30.0f;
		int iStepSize = int (float (m_WindowRect.Height()) * (c_fFrameTime / m_fDuration));

		iMousePosY = iMousePosY - (iMousePosY % iStepSize);

		m_dwcurFrame = iMousePosY / iStepSize;
		m_FrameIndexInstance.SetPosition (m_WindowRect.Width() / 2 + 20, m_dwcurFrame * iStepSize);
	}

	// Motion Event
	if (m_pPickedNode)
	{
		m_pPickedNode->fPosition = iMousePosY;

		m_pPickedNode->fPosition = fMAX (0.0f, m_pPickedNode->fPosition);
		m_pPickedNode->fPosition = fMIN (m_WindowRect.Height(), m_pPickedNode->fPosition);

		m_pMotionData->MovePosition (m_pPickedNode->dwIndex, PositionToTime (m_pPickedNode->fPosition));
	}
	else
	{
		m_pOverNode = nullptr;
		for (auto itor = m_EventNodeList.begin(); itor != m_EventNodeList.end(); ++itor)
		{
			TEventNode * pEventNode = *itor;
			if (isOverNode (NODE_TYPE_LEFT, pEventNode->fPosition, point))
			{
				m_pOverNode = pEventNode;
				break;
			}
		}
	}

	// Combo
	if (m_pMotionData->IsComboInputTimeData())
		for (int i = COMBO_NODE_MAX_NUM - 1; i >= 0; --i)
		{
			assert (i >= 0 && i < COMBO_NODE_MAX_NUM);
			TNodeInstance & rNode = m_ComboNode[i];

			if (rNode.isPicking)
			{
				rNode.fPosition = iMousePosY;
				rNode.fPosition = fMAX (0.0f, rNode.fPosition);
				rNode.fPosition = fMIN (m_WindowRect.Height(), rNode.fPosition);
				switch (i)
				{
					case INPUT_START:
						m_pMotionData->SetComboInputStartTime (PositionToTime (rNode.fPosition));
						break;
					case NEXT_COMBO:
						m_pMotionData->SetNextComboTime (PositionToTime (rNode.fPosition));
						break;
					case INPUT_END:
						m_pMotionData->SetComboInputEndTime (PositionToTime (rNode.fPosition));
						break;
				}
			}
			else
			{
				rNode.isOverriding = FALSE;
				if (isOverNode (NODE_TYPE_RIGHT, rNode.fPosition, point))
				{
					rNode.isOverriding = TRUE;
					break;
				}
			}
		}

	// Attacking Range
	if (m_pMotionData->isAttackingMotion())
		for (int i = 1; i >= 0; --i)
		{
			assert (i >= 0 && i < 2);
			TNodeInstance & rNode = m_AttackingNode[i];

			if (rNode.isPicking)
			{
				rNode.fPosition = iMousePosY;
				rNode.fPosition = fMAX (0.0f, rNode.fPosition);
				rNode.fPosition = fMIN (m_WindowRect.Height(), rNode.fPosition);
				if (START == i)
				{
					rNode.fPosition = fMIN (rNode.fPosition, m_AttackingNode[END].fPosition);
					m_pMotionData->SetAttackStartTime (m_dwcurHitDataIndex, PositionToTime (rNode.fPosition));
				}
				else
				{
					rNode.fPosition = fMAX (rNode.fPosition, m_AttackingNode[START].fPosition);
					m_pMotionData->SetAttackEndTime (m_dwcurHitDataIndex, PositionToTime (rNode.fPosition));
				}

				break;
			}
			else
			{
				rNode.isOverriding = FALSE;
				if (isOverridingAttacking (m_WindowRect.Width() / 2.0f, rNode.fPosition, point))
				{
					rNode.isOverriding = TRUE;
					break;
				}
			}
		}

	// Loop Start Time
	for (DWORD j = 0; j < LOOP_NODE_MAX_NUM; ++j)
	{
		TNodeInstance & rNode = m_LoopNode[j];
		if (rNode.isPicking)
		{
			rNode.fPosition = iMousePosY;
			rNode.fPosition = fMAX (0.0f, rNode.fPosition);
			rNode.fPosition = fMIN (m_WindowRect.Height(), rNode.fPosition);

			if (j == LOOP_NODE_START)
			{
				m_pMotionData->SetLoopStartTime (PositionToTime (rNode.fPosition));
			}
			else if (j == LOOP_NODE_END)
			{
				m_pMotionData->SetLoopEndTime (PositionToTime (rNode.fPosition));
			}
		}
		else
		{
			rNode.isOverriding = FALSE;
			if (isOverNode (NODE_TYPE_LEFT, rNode.fPosition, point))
			{
				rNode.isOverriding = TRUE;
			}
		}
	}

	Render();
	CStatic::OnMouseMove (nFlags, point);
}

void CObjectAnimationEventGraph::OnLButtonDown (UINT nFlags, CPoint point)
{
	BOOL isPickedNode = FALSE;

	// Motion Event
	m_pSelectedNode = nullptr;
	for (auto itor = m_EventNodeList.begin(); itor != m_EventNodeList.end(); ++itor)
	{
		TEventNode * pEventNode = *itor;
		if (isOverNode (NODE_TYPE_LEFT, pEventNode->fPosition, point))
		{
			m_pSelectedNode = pEventNode;
			m_pPickedNode = pEventNode;
			isPickedNode = TRUE;
			break;
		}
	}

	// Combo
	if (!isPickedNode)
		if (m_pMotionData->IsComboInputTimeData())
			for (DWORD i = 0; i < COMBO_NODE_MAX_NUM; ++i)
				if (m_ComboNode[i].isOverriding)
				{
					m_ComboNode[i].isPicking = TRUE;
					isPickedNode = TRUE;
					break;
				}

	// Attacking Range
	if (!isPickedNode)
		if (m_pMotionData->isAttackingMotion())
			for (DWORD i = 0; i < ATTACKING_NODE_MAX_NUM; ++i)
				if (m_AttackingNode[i].isOverriding)
				{
					m_AttackingNode[i].isPicking = TRUE;
					isPickedNode = TRUE;
					break;
				}

	// Loop
	if (!isPickedNode)
		if (m_pMotionData->IsLoopMotion())
			for (DWORD i = 0; i < LOOP_NODE_MAX_NUM; ++i)
			{
				TNodeInstance & rNode = m_LoopNode[i];
				if (rNode.isOverriding)
				{
					rNode.isPicking = TRUE;
					isPickedNode = TRUE;
					break;
				}
			}

	if (!m_pPickedNode)
		if (-32767 == GetAsyncKeyState (VK_LCONTROL))
		{
			MakeEventNode (point.y);
			Render();
			return;
		}

	Render();
	SetCapture();
	CStatic::OnLButtonDown (nFlags, point);
}

void CObjectAnimationEventGraph::OnLButtonUp (UINT nFlags, CPoint point)
{
	m_pOverNode = m_pPickedNode;
	m_pPickedNode = NULL;
	for (DWORD i = 0; i < COMBO_NODE_MAX_NUM; ++i)
	{
		m_ComboNode[i].isPicking = FALSE;
	}
	for (DWORD j = 0; j < ATTACKING_NODE_MAX_NUM; ++j)
	{
		m_AttackingNode[j].isPicking = FALSE;
	}
	for (DWORD k = 0; k < ATTACKING_NODE_MAX_NUM; ++k)
	{
		m_LoopNode[k].isPicking = FALSE;
	}

	Render();
	ReleaseCapture();
	CStatic::OnLButtonUp (nFlags, point);
}

void CObjectAnimationEventGraph::OnLButtonDblClk (UINT nFlags, CPoint point)
{
	if (m_pSelectedNode)
	{
		CObjectAnimationEvent EditEvent;
		EditEvent.SetData (m_pSelectedNode->c_pEventData);
		if (EditEvent.DoModal())
		{
			//GetEventNodeData(EditEvent, m_pSelectedNode->dwIndex);

			CRaceMotionDataAccessor::TMotionEventData * pData;
			if (m_pMotionData->GetMotionEventDataPointer (m_pSelectedNode->dwIndex, &pData))
			{
				EditEvent.GetMotionEventData (pData);
			}
		}
	}

	CStatic::OnLButtonDblClk (nFlags, point);
}

void CObjectAnimationEventGraph::OnRButtonDown (UINT nFlags, CPoint point)
{
	if (GetAsyncKeyState (VK_LCONTROL) & 0x8000)
	{
		for (auto itor = m_EventNodeList.begin(); itor != m_EventNodeList.end(); ++itor)
		{
			TEventNode * pEventNode = *itor;
			if (isOverNode (NODE_TYPE_LEFT, pEventNode->fPosition, point))
			{
				DeleteEventNode (pEventNode);

				m_pOverNode = nullptr;
				m_pPickedNode = nullptr;
				break;
			}
		}

		Render();
	}
	CStatic::OnRButtonDown (nFlags, point);
}

void CObjectAnimationEventGraph::OnPaint()
{
	CPaintDC dc (this); // device context for painting

	Render();
}
