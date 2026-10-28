#if !defined(AFX_DLGPERSPECTIVESETTING_H__67EE8DBE_591D_4C38_B86A_F7E178CB0B1C__INCLUDED_)
#define AFX_DLGPERSPECTIVESETTING_H__67EE8DBE_591D_4C38_B86A_F7E178CB0B1C__INCLUDED_

#pragma once

// DlgPerspectiveSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPerspective dialog

class CDlgPerspective : public CDialog
{
		// Construction
	public:
		CDlgPerspective (CWnd* pParent = NULL);  // standard constructor

		// Dialog Data
		//{{AFX_DATA(CDlgPerspective)
		enum { IDD = IDD_DIALOG_PERSPECTIVE };
		CSliderCtrl	m_ctrlFarDistance;
		CSliderCtrl	m_ctrlNearDistance;
		CSliderCtrl	m_ctrlFOV;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDlgPerspective)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	public:
		void SetFOV (float fFOV);
		void SetNearDistance (float fDistance);
		void SetFarDistance (float fDistance);

		float GetFOV();

	protected:

		// Generated message map functions
		//{{AFX_MSG(CDlgPerspective)
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		float m_fFOV;
		float m_fNearDistance;
		float m_fFarDistance;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPERSPECTIVESETTING_H__67EE8DBE_591D_4C38_B86A_F7E178CB0B1C__INCLUDED_)
