#if !defined(AFX_OBJECTMODELCOLLISIONSUBPAGE_H__580A5930_540C_413A_84AA_4B0B9450408F__INCLUDED_)
#define AFX_OBJECTMODELCOLLISIONSUBPAGE_H__580A5930_540C_413A_84AA_4B0B9450408F__INCLUDED_

#pragma once

// ObjectModelCollisionSubPage.h : header file
//

class CObjectModelPage;

/////////////////////////////////////////////////////////////////////////////
// CObjectModelCollisionSubPage dialog

class CObjectModelCollisionSubPage : public CDialog
{
		// Construction
	public:
		CObjectModelCollisionSubPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CObjectModelPage * pParent, const CRect & c_rRect);

		void SetAttachingIndex (DWORD dwIndex);
		void UpdateUI();

		// Dialog Data
		//{{AFX_DATA(CObjectModelCollisionSubPage)
		enum { IDD = IDD_OBJECT_MODEL_COLLISION_PAGE };
		CComboBox	m_ctrlSphereList;
		CSliderCtrl	m_ctrlSphereSize;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectModelCollisionSubPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectModelCollisionSubPage)
		afx_msg void OnCheckTypeBody();
		afx_msg void OnCheckTypeDefending();
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnOK();
		afx_msg void OnCancel();
		afx_msg void OnInsertCollisionData();
		afx_msg void OnDeleteOneCollisionData();
		afx_msg void OnCollisionSizeType1();
		afx_msg void OnCollisionSizeType2();
		afx_msg void OnCollisionSizeType3();
		afx_msg void OnCollisionSizeType4();
		afx_msg void OnCollisionSizeType5();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		BOOL GetCollisionDataPointer (DWORD dwAttachingIndex, NRaceData::TCollisionData** ppCollisionData);
		BOOL GetSphereDataPointer (DWORD dwAttachingIndex, DWORD dwSphereIndex, TSphereData** ppSphereData);

		void SetSphereSize (DWORD dwAttachingIndex, DWORD dwSphereIndex, int iSize);
		void SetSphereSizeType (DWORD dwAttachingIndex, DWORD dwSphereIndex, DWORD dwSizeType);

		void UpdateSphereData();

	protected:
		CObjectModelPage* m_pParent;

		DWORD m_dwAttachingIndex;
		DWORD m_dwSphereIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTMODELCOLLISIONSUBPAGE_H__580A5930_540C_413A_84AA_4B0B9450408F__INCLUDED_)
