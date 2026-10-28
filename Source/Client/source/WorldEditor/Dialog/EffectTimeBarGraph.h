#if !defined(AFX_EFFECTTIMEBARGRAPH_H__FEB08EC0_B265_4D09_9AC0_0D2C438B35EB__INCLUDED_)
#define AFX_EFFECTTIMEBARGRAPH_H__FEB08EC0_B265_4D09_9AC0_0D2C438B35EB__INCLUDED_

#pragma once

// EffectTimeBarGraph.h : header file
//

#include "GraphCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeBarGraph window

class CEffectTimeBarGraph : public CStatic
{
		// Construction
	public:
		CEffectTimeBarGraph();
		void Initialize();

		// Attributes
	public:

		// Operations
	public:
		void SetGraphCtrl (CGraphCtrl * m_pGraphCtrl);
		void Resizing (int iWidth, int iHeight);

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectTimeBarGraph)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CEffectTimeBarGraph();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CEffectTimeBarGraph)
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnMouseMove (UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnTimer (UINT_PTR nIDEvent);
		afx_msg void OnMButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnMButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	protected:
		CGraphCtrl* m_pGraphCtrl;

		// Mouse
		bool m_isLButtonDown;
		bool m_isMButtonDown;
		CPoint m_LastPoint;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTIMEBARGRAPH_H__FEB08EC0_B265_4D09_9AC0_0D2C438B35EB__INCLUDED_)
