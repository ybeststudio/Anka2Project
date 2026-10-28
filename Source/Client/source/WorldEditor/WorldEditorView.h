// WorldEditorView.h : interface of the CWorldEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDEDITORVIEW_H__933ED50D_E0FE_4468_A7D2_52CB5D428A27__INCLUDED_)
#define AFX_WORLDEDITORVIEW_H__933ED50D_E0FE_4468_A7D2_52CB5D428A27__INCLUDED_

#pragma once

#include "ViewportManager.h"
#include "Dialog/DlgPerspective.h"

class CWorldEditorView : public CView
{
	protected: // create from serialization only
		CWorldEditorView();
		DECLARE_DYNCREATE (CWorldEditorView)

		// Attributes
	public:
		CWorldEditorDoc* GetDocument();
		void ChangeActiveMode();

		// Operations
	public:
		void Initialize();
		void Process();

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CWorldEditorView)
	public:
		virtual void OnDraw (CDC* pDC); // overridden to draw this view
		virtual BOOL PreCreateWindow (CREATESTRUCT& cs);
	protected:
		virtual BOOL OnPreparePrinting (CPrintInfo* pInfo);
		virtual void OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo);
		virtual void OnEndPrinting (CDC* pDC, CPrintInfo* pInfo);
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CWorldEditorView();
		#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump (CDumpContext& dc) const;
		#endif

	public:
		void UpdateTargetPosition (float fx, float fy);

		void TogglePerspectiveDialog();
		BOOL isShowingPerspectiveDialog();

		void ToggleInfoBoxVisibleState();
		BOOL IsInfoBoxVisible();

		void Lock();
		void Unlock();
		bool IsLocked();

	protected:
		void GetMouseLocalPosition (POINT point, float* px, float* py);

		void UpdateCursorPosition (int ix, int iy);
		void UpdateScreenStatus();
		void UpdateFrustum();
		void UpdateCameraPosition();

		void RunKeyDown (int iChar);
		void RunKeyUp (int iChar);
		void RunMouseMove (LONG x, LONG y);
		void RunLButtonDown (UINT nFlags, CPoint point);
		void RunLButtonUp();
		void RunRButtonDown();
		void RunRButtonUp();
		BOOL RunMouseWheel (short zDelta);

		// Generated message map functions
	protected:
		//{{AFX_MSG(CWorldEditorView)
		afx_msg void OnMouseMove (UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnMButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnMButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg BOOL OnMouseWheel (UINT nFlags, short zDelta, CPoint pt);
		afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnSize (UINT nType, int cx, int cy);
		afx_msg BOOL OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CScreen m_Screen;
		CViewportManager m_ViewportManager;

		bool m_bRenderUI;
		bool m_isControlKey;
		bool m_bInitialized;
		bool m_isLocked;

		CDlgPerspective* m_pDlgPerspective;
};

#ifndef _DEBUG  // debug version in WorldEditorView.cpp
inline CWorldEditorDoc* CWorldEditorView::GetDocument()
{
	return (CWorldEditorDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDEDITORVIEW_H__933ED50D_E0FE_4468_A7D2_52CB5D428A27__INCLUDED_)
