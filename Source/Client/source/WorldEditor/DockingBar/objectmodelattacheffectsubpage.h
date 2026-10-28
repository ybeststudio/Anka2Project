#if !defined(AFX_OBJECTMODELATTACHEFFECTSUBPAGE_H__9B0C04ED_0E79_4FB2_9882_12078809C911__INCLUDED_)
#define AFX_OBJECTMODELATTACHEFFECTSUBPAGE_H__9B0C04ED_0E79_4FB2_9882_12078809C911__INCLUDED_

#pragma once

// objectmodelattacheffectsubpage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectModelAttachEffectSubPage dialog

class CObjectModelAttachEffectSubPage : public CDialog
{
		// Construction
	public:
		CObjectModelAttachEffectSubPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);
		void SetAttachingIndex (DWORD dwIndex);
		void UpdateUI();

		// Dialog Data
		//{{AFX_DATA(CObjectModelAttachEffectSubPage)
		enum { IDD = IDD_OBJECT_MODEL_ATTACH_EFFECT_PAGE };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectModelAttachEffectSubPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectModelAttachEffectSubPage)
		afx_msg void OnEffectPageLoadEffect();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()


	protected:
		DWORD m_dwAttachingIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTMODELATTACHEFFECTSUBPAGE_H__9B0C04ED_0E79_4FB2_9882_12078809C911__INCLUDED_)
