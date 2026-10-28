#if !defined(AFX_OBJECTANIMATIONEVENTSCREENFLASHING_H__F77B8805_A2A8_480B_83D2_560E155EED74__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTSCREENFLASHING_H__F77B8805_A2A8_480B_83D2_560E155EED74__INCLUDED_

#pragma once

// ObjectAnimationEventScreenFlashing.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventScreenFlashing dialog

class CObjectAnimationEventScreenFlashing : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventScreenFlashing (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventScreenFlashing)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_SCREEN_FLASHING };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventScreenFlashing)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	public:
		void Close();

		void GetData (CRaceMotionDataAccessor::TMotionEventData * pData);
		void SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData);

	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectAnimationEventScreenFlashing)
		// NOTE: the ClassWizard will add member functions here
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTSCREENFLASHING_H__F77B8805_A2A8_480B_83D2_560E155EED74__INCLUDED_)
