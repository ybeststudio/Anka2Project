#if !defined(AFX_OBJECTANIMATIONEVENTEFFECT_H__B70B7DB5_54AB_4327_A40C_A06E1D3890DF__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTEFFECT_H__B70B7DB5_54AB_4327_A40C_A06E1D3890DF__INCLUDED_

#pragma once

// ObjectAnimationEventEffect.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventEffect dialog

class CObjectAnimationEventEffect : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventEffect (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventEffect)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_EFFECT };
		CComboBox	m_ctrlAttachingBone;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventEffect)
	protected:
		virtual void OnOK();
		virtual void OnCancel();
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	public:
		BOOL canClose();
		void Close();

		void GetData (CRaceMotionDataAccessor::TMotionEventData * pData);
		void SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData);

	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectAnimationEventEffect)
		afx_msg void OnLoadEffect();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CString m_strEffectFileName;

		BOOL m_isAttachingEnable;
		BOOL m_isFollowingEnable;
		BOOL m_isIndependentEnable;
		D3DXVECTOR3 m_v3EffectPosition;
		std::string m_strAttachingBoneName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTEFFECT_H__B70B7DB5_54AB_4327_A40C_A06E1D3890DF__INCLUDED_)
