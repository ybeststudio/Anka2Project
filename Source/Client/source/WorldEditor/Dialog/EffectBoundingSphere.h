#if !defined(AFX_EFFECTBOUNDINGSPHERE_H__0657AEB6_B286_4BCB_980C_254043818736__INCLUDED_)
#define AFX_EFFECTBOUNDINGSPHERE_H__0657AEB6_B286_4BCB_980C_254043818736__INCLUDED_

#pragma once

// EffectBoundingSphere.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEffectBoundingSphere dialog

class CEffectBoundingSphere : public CDialog
{
		// Construction
	public:
		CEffectBoundingSphere (CWnd* pParent = NULL);  // standard constructor

		// Functions
		void RefreshInfo();
		void SetBoundingSphereData();

		// Dialog Data
		//{{AFX_DATA(CEffectBoundingSphere)
		enum { IDD = IDD_EFFECT_BOUNDING_SPHERE };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectBoundingSphere)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		void OnOK();
		void OnCancel();

		// Generated message map functions
		//{{AFX_MSG(CEffectBoundingSphere)
		virtual BOOL OnInitDialog();
		afx_msg void OnChangeEffectBoundingSphereRadius();
		afx_msg void OnChangeEffectBoundingSpherePosX();
		afx_msg void OnChangeEffectBoundingSpherePosY();
		afx_msg void OnChangeEffectBoundingSpherePosZ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		BOOL m_bCallBackEnable;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTBOUNDINGSPHERE_H__0657AEB6_B286_4BCB_980C_254043818736__INCLUDED_)
