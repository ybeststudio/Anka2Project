#if !defined(AFX_OBJECTANIMATIONEVENTWARP_H__C50162F9_AA0F_4997_A554_31728363D7BA__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTWARP_H__C50162F9_AA0F_4997_A554_31728363D7BA__INCLUDED_

#pragma once

// ObjectAnimationEventWarp.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventWarp dialog

class CObjectAnimationEventWarp : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventWarp (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventWarp)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_WARP };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventWarp)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	public:
		void Close();

		void GetData (CRaceMotionData::TMotionEventData * pMotionEventData);
		void SetData (const CRaceMotionData::TMotionEventData * c_pMotionEventData);

	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectAnimationEventWarp)
		// NOTE: the ClassWizard will add member functions here
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTWARP_H__C50162F9_AA0F_4997_A554_31728363D7BA__INCLUDED_)
