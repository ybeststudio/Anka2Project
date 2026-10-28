#if !defined(AFX_OBJECTANIMATIONEVENTATTACK_H__44579141_854D_4D4C_AE44_0279D931D317__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTATTACK_H__44579141_854D_4D4C_AE44_0279D931D317__INCLUDED_

#pragma once

// ObjectAnimationEventAttack.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventAttack dialog

class CObjectAnimationEventAttack : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventAttack (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventAttack)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_ATTACK };
		CComboBox	m_ctrlAttackType;
		CComboBox	m_ctrlHitType;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventAttack)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual void OnOK();
		virtual void OnCancel();
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		void Close();

		void GetData (CRaceMotionDataAccessor::TMotionEventData * pData);
		void SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData);

		// Generated message map functions
		//{{AFX_MSG(CObjectAnimationEventAttack)
		afx_msg void OnEnableHitProcess();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		D3DXVECTOR3 m_v3Position;
		float m_fRadius;

		BOOL m_isEnableHitProcess;
		DWORD m_dwHitType;
		DWORD m_dwAttackType;
		float m_fInvisibleType;
		float m_fExternalForce;
		float m_fDuration;
		int m_iHitLimitCount;

		std::map<DWORD, std::string> m_HitTypeNameMap;
		std::map<DWORD, std::string> m_AttackTypeNameMap;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTATTACK_H__44579141_854D_4D4C_AE44_0279D931D317__INCLUDED_)
