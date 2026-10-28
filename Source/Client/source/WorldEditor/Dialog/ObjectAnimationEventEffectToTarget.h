#if !defined(AFX_OBJECTANIMATIONEVENTEFFECTTOTARGET_H__03A7F76C_9278_440A_B3CB_5955B7813DAC__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTEFFECTTOTARGET_H__03A7F76C_9278_440A_B3CB_5955B7813DAC__INCLUDED_

#pragma once

// ObjectAnimationEventEffectToTarget.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventEffectToTarget dialog

class CObjectAnimationEventEffectToTarget : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventEffectToTarget (CWnd* pParent = NULL);  // standard constructor

		// Operation
	public:
		BOOL Create (CWnd * pParent, const CRect & c_rRect);
		void Close();

		void GetData (CRaceMotionDataAccessor::TMotionEventData * pData);
		void SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventEffectToTarget)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventEffectToTarget)
	protected:
		virtual void OnOK();
		virtual void OnCancel();
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectAnimationEventEffectToTarget)
		afx_msg void OnLoadEffect();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		// Attributes
	protected:
		CString m_strEffectFileName;
		D3DXVECTOR3 m_v3EffectPosition;
		BOOL m_isFollowingEnable;
		BOOL m_isFishingEffectFlag;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTEFFECTTOTARGET_H__03A7F76C_9278_440A_B3CB_5955B7813DAC__INCLUDED_)
