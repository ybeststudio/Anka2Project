#if !defined(AFX_OBJECTANIMATIONEVENTSCREENWAVING_H__D17B8F49_A410_45A8_9E5B_E8DFD0D7C162__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTSCREENWAVING_H__D17B8F49_A410_45A8_9E5B_E8DFD0D7C162__INCLUDED_

#pragma once

// ObjectAnimationEventScreenWaving.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventScreenWaving dialog

class CObjectAnimationEventScreenWaving : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventScreenWaving (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventScreenWaving)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_SCREEN_WAVING };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventScreenWaving)
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
		//{{AFX_MSG(CObjectAnimationEventScreenWaving)
		// NOTE: the ClassWizard will add member functions here
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CString m_strDurationTime;
		CString m_strPower;
		CString m_strAffectingRange;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTSCREENWAVING_H__D17B8F49_A410_45A8_9E5B_E8DFD0D7C162__INCLUDED_)
