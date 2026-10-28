#if !defined(AFX_OBJECTANIMATIONEVENTCHARACTERHIDE_H__0FAE249A_60CF_4B97_A255_FB3478DE9ACB__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTCHARACTERHIDE_H__0FAE249A_60CF_4B97_A255_FB3478DE9ACB__INCLUDED_

#pragma once

// ObjectAnimationEventCharacterHide.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventCharacterHide dialog

class CObjectAnimationEventCharacterHide : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventCharacterHide (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventCharacterHide)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_CHARACTER_HIDE };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventCharacterHide)
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
		//{{AFX_MSG(CObjectAnimationEventCharacterHide)
		// NOTE: the ClassWizard will add member functions here
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTCHARACTERHIDE_H__0FAE249A_60CF_4B97_A255_FB3478DE9ACB__INCLUDED_)
