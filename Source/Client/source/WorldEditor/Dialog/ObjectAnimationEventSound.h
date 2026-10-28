#if !defined(AFX_OBJECTANIMATIONEVENTSOUND_H__57398C3E_5AD7_43EC_A52B_707FB84B617F__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTSOUND_H__57398C3E_5AD7_43EC_A52B_707FB84B617F__INCLUDED_

#pragma once

// ObjectAnimationEventSound.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventSound dialog

class CObjectAnimationEvent;

class CObjectAnimationEventSound : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventSound (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventSound)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_SOUND };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventSound)
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
		//{{AFX_MSG(CObjectAnimationEventSound)
		afx_msg void OnLoadSound();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CString m_strSoundFileName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTSOUND_H__57398C3E_5AD7_43EC_A52B_707FB84B617F__INCLUDED_)
