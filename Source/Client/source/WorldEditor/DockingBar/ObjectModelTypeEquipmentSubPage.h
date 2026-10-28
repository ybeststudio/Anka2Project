#if !defined(AFX_OBJECTMODELTYPEEQUIPMENTSUBPAGE_H__4BA23E6D_5FA4_4922_A587_93A74F3EC910__INCLUDED_)
#define AFX_OBJECTMODELTYPEEQUIPMENTSUBPAGE_H__4BA23E6D_5FA4_4922_A587_93A74F3EC910__INCLUDED_

#pragma once

// ObjectModelTypeEquipmentSubPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectModelTypeEquipmentSubPage dialog

class CObjectModelTypeEquipmentSubPage : public CDialog
{
		// Construction
	public:
		CObjectModelTypeEquipmentSubPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		void UpdateUI();

		// Dialog Data
		//{{AFX_DATA(CObjectModelTypeEquipmentSubPage)
		enum { IDD = IDD_OBJECT_MODEL_TYPE_EQUIPMENT_PAGE };
		CComboBox	m_ctrlEquipmentType;
		CListBox	m_ctrlEffectList;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectModelTypeEquipmentSubPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectModelTypeEquipmentSubPage)
		afx_msg void OnOK();
		afx_msg void OnCancel();
		afx_msg void OnLoadSound();
		afx_msg void OnClearOneEffect();
		afx_msg void OnLoadEffect();
		afx_msg void OnLoadIconImage();
		afx_msg void OnLoadDropModel();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTMODELTYPEEQUIPMENTSUBPAGE_H__4BA23E6D_5FA4_4922_A587_93A74F3EC910__INCLUDED_)
