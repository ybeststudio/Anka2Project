// EffectTimeBarGraph.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectParticleTimeEventGraph.h"
#include "FloatEditDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

CFloatEditDialog* CEffectTimeEventGraph::ms_pFloatEditDialog = NULL;


/////////////////////////////////////////////////////////////////////////////
// CEffectTimeEventGraph

CEffectTimeEventGraph::CEffectTimeEventGraph()
{
	m_isLButtonDown = false;
	m_isMButtonDown = false;
	m_fMaxTime = 1.0f;
	m_fMaxValue = 1.0f;
	m_fStartValue = 0.0f;
	m_pAccessor = NULL;
	m_dwGrippedIndex = -1;
	m_dwSelectedIndex = POINT_NONE;
}

CEffectTimeEventGraph::~CEffectTimeEventGraph()
{
}


BEGIN_MESSAGE_MAP (CEffectTimeEventGraph, CStatic)
	//{{AFX_MSG_MAP(CEffectTimeEventGraph)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeEventGraph normal functions

void CEffectTimeEventGraph::Initialize (int iTimerID)
{
	SetTimer (iTimerID, 20, 0);

	CRect rc;
	GetClientRect (&rc);
	m_ixTemporary = 2;
	m_iyTemporary = 2;
	m_ixGridCount = 5;
	m_iyGridCount = 7;

	m_iWidth = rc.Width() - m_ixTemporary * 2;
	m_iHeight = rc.Height() - m_iyTemporary * 2;
	m_fxGridStep = float (m_iWidth) / float (m_ixGridCount);
	m_fyGridStep = float (m_iHeight) / float (m_iyGridCount);
}

void CEffectTimeEventGraph::Resizing (int iWidth, int iHeight)
{
	if (!::IsWindow (GetSafeHwnd()))
	{
		return;
	}

	CRect rc;
	GetClientRect (&rc);

	//	MoveWindow(rc.left, rc.top, iWidth, iHeight);
}

void CEffectTimeEventGraph::SetMaxTime (float fMaxTime)
{
	m_fMaxTime = fMaxTime;
}

void CEffectTimeEventGraph::SetMaxValue (float fMaxValue)
{
	m_fMaxValue = fMaxValue;
}

void CEffectTimeEventGraph::SetStartValue (float fStartValue)
{
	m_fStartValue = fStartValue;
}

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeEventGraph message handlers

void CEffectTimeEventGraph::OnMouseMove (UINT nFlags, CPoint point)
{
	int ix = point.x;
	int iy = point.y;

	/////
	m_dwSelectedIndex = POINT_NONE;

	for (DWORD i = 0; i < m_PointVector.size(); ++i)
	{
		TPoint & rPoint = m_PointVector[i];

		if (abs (ix - rPoint.ix) < 5)
			if (abs (iy - rPoint.iy) < 5)
			{
				m_dwSelectedIndex = i;
			}
	}

	// Move gripped point
	float fTime;
	float fValue;

	if (POINT_NONE != m_dwGrippedIndex)
		if (m_dwGrippedIndex < m_PointVector.size())
		{
			TPoint & rPoint = m_PointVector[m_dwGrippedIndex];

			if (m_dwGrippedIndex > 0)
			{
				if (m_PointVector[m_dwGrippedIndex - 1].ix + 1 > ix)
				{
					ix = m_PointVector[m_dwGrippedIndex - 1].ix + 1;
				}
			}
			if (m_dwGrippedIndex < m_PointVector.size() - 1)
			{
				if (m_PointVector[m_dwGrippedIndex + 1].ix - 1 < ix)
				{
					ix = m_PointVector[m_dwGrippedIndex + 1].ix - 1;
				}
			}

			ScreenToTime (ix, &fTime);
			ScreenToValue (iy, &fValue);

			fTime = fMAX (fTime, 0.0f);
			fValue = fMAX (fValue, m_fStartValue);
			fTime = fMIN (fTime, m_fMaxTime);
			fValue = fMIN (fValue, m_fStartValue + m_fMaxValue);

			m_pAccessor->SetTime (m_dwGrippedIndex, fTime);
			m_pAccessor->SetValue (m_dwGrippedIndex, fValue);
		}
	/////

	CStatic::OnMouseMove (nFlags, point);
}

void CEffectTimeEventGraph::OnLButtonDown (UINT nFlags, CPoint point)
{
	m_isLButtonDown = true;
	m_LastPoint = point;

	/////
	if (POINT_NONE != m_dwSelectedIndex)
	{
		m_dwGrippedIndex = m_dwSelectedIndex;
	}
	else if (isCreatingMode())
	{
		float fTime;
		float fValue;
		ScreenToTime (point.x, &fTime);
		ScreenToValue (point.y, &fValue);

		if (m_pAccessor)
		{
			m_pAccessor->Insert (fTime, fValue);
		}
	}
	/////

	SetCapture();
	CStatic::OnLButtonDown (nFlags, point);
}

void CEffectTimeEventGraph::OnLButtonUp (UINT nFlags, CPoint point)
{
	m_isLButtonDown = false;
	m_LastPoint = point;

	/////
	m_dwGrippedIndex = -1;
	/////

	ReleaseCapture();
	CStatic::OnLButtonUp (nFlags, point);
}

void CEffectTimeEventGraph::OnMButtonDown (UINT nFlags, CPoint point)
{
	m_isMButtonDown = true;
	m_LastPoint = point;

	SetCapture();
	CStatic::OnMButtonDown (nFlags, point);
}
void CEffectTimeEventGraph::OnMButtonUp (UINT nFlags, CPoint point)
{
	m_isMButtonDown = false;
	m_LastPoint = point;

	ReleaseCapture();
	CStatic::OnMButtonUp (nFlags, point);
}

void CEffectTimeEventGraph::OnRButtonDown (UINT nFlags, CPoint point)
{
	/////
	if (isCreatingMode())
		if (POINT_NONE != m_dwSelectedIndex)
		{
			m_pAccessor->Delete (m_dwSelectedIndex);
			m_dwSelectedIndex = POINT_NONE;
		}
	/////

	SetCapture();
	CStatic::OnRButtonDown (nFlags, point);
}

void CEffectTimeEventGraph::OnRButtonUp (UINT nFlags, CPoint point)
{
	ReleaseCapture();
	CStatic::OnRButtonUp (nFlags, point);
}

bool CEffectTimeEventGraph::isCreatingMode()
{
	return (GetAsyncKeyState (VK_LCONTROL) & (SHORT) (1 << 15)) != 0 ? true : false;
}

void CEffectTimeEventGraph::SetAccessorPointer (IValueAccessor * pAccessor)
{
	m_pAccessor = pAccessor;
}

void CEffectTimeEventGraph::RenderGrid()
{
	m_Screen.SetDiffuseColor (0.8f, 0.8f, 0.8f);
	for (int x = 0; x < m_ixGridCount + 1; ++x)
	{
		float fxPos = m_ixTemporary + (x * m_fxGridStep);
		m_Screen.RenderLine2d (fxPos, m_iyTemporary,
							   fxPos, m_iyTemporary + (m_fyGridStep * m_iyGridCount));
	}
	for (int y = 0; y < m_iyGridCount + 1; ++y)
	{
		float fyPos = m_iyTemporary + (y * m_fyGridStep);
		m_Screen.RenderLine2d (m_ixTemporary, fyPos,
							   m_ixTemporary + (m_fxGridStep * m_ixGridCount), fyPos);
	}
}

void CEffectTimeEventGraph::RenderGraph()
{
	if (!m_pAccessor)
	{
		return;
	}

	DWORD dwCount = m_pAccessor->GetValueCount();
	m_PointVector.clear();
	m_PointVector.resize (dwCount);

	DWORD i;
	float fTime;
	float fValue;

	int ix, iy;
	int iBoxHalfSize = 3;

	m_Screen.SetDiffuseColor (1.0f, 0.0f, 0.0f);
	for (i = 0; i < dwCount; ++i)
	{
		m_pAccessor->GetTime (i, &fTime);
		m_pAccessor->GetValue (i, &fValue);
		TimeToScreen (fTime, &ix);
		ValueToScreen (fValue, &iy);

		m_Screen.RenderBox2d (ix - iBoxHalfSize, iy - iBoxHalfSize,
							  ix + iBoxHalfSize, iy + iBoxHalfSize);

		if (m_dwSelectedIndex == i || m_dwGrippedIndex == i)
		{
			m_Screen.RenderBar2d (ix - iBoxHalfSize, iy - iBoxHalfSize,
								  ix + iBoxHalfSize, iy + iBoxHalfSize);
		}

		m_PointVector[i].ix = ix;
		m_PointVector[i].iy = iy;
	}

	// Render Line
	if (!m_PointVector.empty())
	{
		m_Screen.SetDiffuseColor (0.0f, 0.0f, 0.0f);
		m_Screen.RenderLine2d (m_ixTemporary, m_PointVector[0].iy, m_PointVector[0].ix, m_PointVector[0].iy);
		for (i = 0; i < dwCount - 1; ++i)
		{
			ConnectPoint (m_PointVector[i], m_PointVector[i + 1]);
		}
		if (dwCount >= 1)
		{
			m_Screen.RenderLine2d (m_PointVector[dwCount - 1].ix, m_PointVector[dwCount - 1].iy, m_ixTemporary + m_iWidth, m_PointVector[dwCount - 1].iy);
		}
	}
}

void CEffectTimeEventGraph::ConnectPoint (TPoint & rLeft, TPoint & rRight)
{
	m_Screen.RenderLine2d (rLeft.ix, rLeft.iy, rRight.ix, rRight.iy);
}

void CEffectTimeEventGraph::OnTimer (UINT_PTR nIDEvent)
{
	if (IsWindowVisible())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CSceneEffect * pSceneEffect = pApplication->GetSceneEffect();
		CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

		CRect rc;
		GetClientRect (&rc);

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity (&matIdentity);
		STATEMANAGER.SaveTransform (D3DTS_PROJECTION, &matIdentity);
		STATEMANAGER.SaveTransform (D3DTS_VIEW, &matIdentity);
		STATEMANAGER.SaveTransform (D3DTS_WORLD, &matIdentity);
		m_Screen.SetOrtho2D (1024.0f, 768.0f, 400.0f);
		m_Screen.SetClearColor (1.0f, 1.0f, 1.0f);
		m_Screen.Clear();
		m_Screen.Begin();

		RenderGrid();
		RenderGraph();

		m_Screen.End();
		m_Screen.Show (&rc, GetSafeHwnd());
		STATEMANAGER.RestoreTransform (D3DTS_WORLD);
		STATEMANAGER.RestoreTransform (D3DTS_VIEW);
		STATEMANAGER.RestoreTransform (D3DTS_PROJECTION);
	}

	CStatic::OnTimer (nIDEvent);
}

void CEffectTimeEventGraph::TimeToScreen (float fTime, int* px)
{
	*px = (fTime / m_fMaxTime) * float (m_iWidth) + m_ixTemporary;
}

void CEffectTimeEventGraph::ScreenToTime (int ix, float* pfTime)
{
	*pfTime = (float (ix - m_ixTemporary) / float (m_iWidth)) * m_fMaxTime;
}

void CEffectTimeEventGraph::ValueToScreen (float fValue, int* piy)
{
	*piy = m_iHeight - ((fValue - m_fStartValue) / m_fMaxValue) * float (m_iHeight) + m_iyTemporary;
}

void CEffectTimeEventGraph::ScreenToValue (int iy, float* pfValue)
{
	*pfValue = (float (m_iHeight - (iy - m_iyTemporary)) / float (m_iHeight)) * m_fMaxValue + m_fStartValue;
}

void CEffectTimeEventGraph::OnLButtonDblClk (UINT nFlags, CPoint point)
{
	if (!m_pAccessor)
	{
		return;
	}
	if (!ms_pFloatEditDialog)
	{
		ms_pFloatEditDialog = new CFloatEditDialog;
		ms_pFloatEditDialog->Create (this);
	}

	ms_pFloatEditDialog->Update (m_pAccessor);

	ms_pFloatEditDialog->ShowWindow (SW_SHOW);

	CStatic::OnLButtonDblClk (nFlags, point);
}
