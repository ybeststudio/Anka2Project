// EffectTimeBarGraph.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectTimeBarGraph.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeBarGraph

CEffectTimeBarGraph::CEffectTimeBarGraph()
{
	m_isLButtonDown = false;
	m_isMButtonDown = false;
}

CEffectTimeBarGraph::~CEffectTimeBarGraph()
{
}


BEGIN_MESSAGE_MAP (CEffectTimeBarGraph, CStatic)
	//{{AFX_MSG_MAP(CEffectTimeBarGraph)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeBarGraph normal functions

void CEffectTimeBarGraph::Initialize()
{
	m_pGraphCtrl->Initialize();

	SetTimer (WINDOW_TIMER_ID_EFFECT_TIME_BAR, 20, 0);

	CRect rc;
	GetClientRect (&rc);
	m_pGraphCtrl->SetSize (rc.Width(), rc.Height());
}

void CEffectTimeBarGraph::SetGraphCtrl (CGraphCtrl * pGraphCtrl)
{
	m_pGraphCtrl = pGraphCtrl;
}

void CEffectTimeBarGraph::Resizing (int iWidth, int iHeight)
{
	if (!::IsWindow (GetSafeHwnd()))
	{
		return;
	}

	CRect rc;
	GetClientRect (&rc);

	//	MoveWindow(rc.left, rc.top, iWidth, iHeight);
}

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeBarGraph message handlers

void CEffectTimeBarGraph::OnMouseMove (UINT nFlags, CPoint point)
{
	m_pGraphCtrl->OnMouseMove (point.x, point.y);

	if (m_isMButtonDown)
	{
		int dx = m_LastPoint.x - point.x;
		int dy = m_LastPoint.y - point.y;

		m_pGraphCtrl->Move (float (dx) / 50.0f, float (dy) / 50.0f);

		m_LastPoint = point;
	}

	CStatic::OnMouseMove (nFlags, point);
}

void CEffectTimeBarGraph::OnLButtonDown (UINT nFlags, CPoint point)
{
	m_isLButtonDown = true;
	m_LastPoint = point;

	m_pGraphCtrl->OnLButtonDown (point.x, point.y);

	SetCapture();
	CStatic::OnLButtonDown (nFlags, point);
}

void CEffectTimeBarGraph::OnLButtonUp (UINT nFlags, CPoint point)
{
	m_isLButtonDown = false;
	m_LastPoint = point;

	m_pGraphCtrl->OnLButtonUp (point.x, point.y);

	ReleaseCapture();
	CStatic::OnLButtonUp (nFlags, point);
}

void CEffectTimeBarGraph::OnMButtonDown (UINT nFlags, CPoint point)
{
	m_isMButtonDown = true;
	m_LastPoint = point;

	SetCapture();
	CStatic::OnMButtonDown (nFlags, point);
}
void CEffectTimeBarGraph::OnMButtonUp (UINT nFlags, CPoint point)
{
	m_isMButtonDown = false;
	m_LastPoint = point;

	ReleaseCapture();
	CStatic::OnMButtonUp (nFlags, point);
}

void CEffectTimeBarGraph::OnRButtonDown (UINT nFlags, CPoint point)
{
	m_pGraphCtrl->OnRButtonDown (point.x, point.y);

	SetCapture();
	CStatic::OnRButtonDown (nFlags, point);
}

void CEffectTimeBarGraph::OnRButtonUp (UINT nFlags, CPoint point)
{
	m_pGraphCtrl->OnRButtonUp (point.x, point.y);

	ReleaseCapture();
	CStatic::OnRButtonUp (nFlags, point);
}

void CEffectTimeBarGraph::OnTimer (UINT_PTR nIDEvent)
{
	if (IsWindowVisible())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CSceneEffect * pSceneEffect = pApplication->GetSceneEffect();
		CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

		CRect rc;
		GetClientRect (&rc);

		m_pGraphCtrl->Update();

		m_pGraphCtrl->GraphBegin();
		m_pGraphCtrl->Render();
		for (DWORD i = 0; i < pSceneEffect->GetStartTimeCount(); ++i)
		{
			float fTime;
			if (pSceneEffect->GetStartTime (i, &fTime))
			{
				m_pGraphCtrl->RenderTimeLine (fTime);
			}
		}
		m_pGraphCtrl->RenderEndLine (pEffectAccessor->GetLifeTime());
		m_pGraphCtrl->GraphEnd (&rc, GetSafeHwnd());
	}

	CStatic::OnTimer (nIDEvent);
}

void CEffectTimeBarGraph::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_pGraphCtrl->OnKeyDown (nChar, nRepCnt, nFlags);

	CStatic::OnKeyDown (nChar, nRepCnt, nFlags);
}
