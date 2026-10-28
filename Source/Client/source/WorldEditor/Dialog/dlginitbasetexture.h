#if !defined(AFX_DLGINITBASETEXTURE_H__AAF1EC86_71C5_4693_A4C3_F419336C3FE3__INCLUDED_)
#define AFX_DLGINITBASETEXTURE_H__AAF1EC86_71C5_4693_A4C3_F419336C3FE3__INCLUDED_

#pragma once

// dlginitbasetexture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInitBaseTexture dialog

class CDlgInitBaseTexture : public CDialog
{
		// Construction
	public:
		CDlgInitBaseTexture (CWnd* pParent = NULL);  // standard constructor

		// Dialog Data
		//{{AFX_DATA(CDlgInitBaseTexture)
		enum { IDD = IDD_DIALOG_INITBASETEXTURE };
		CListCtrl	m_ListTexture;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDlgInitBaseTexture)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		void LoadList();
		void InitTexture();

		std::vector<BYTE> m_TextureNumberVector;

		// Generated message map functions
		//{{AFX_MSG(CDlgInitBaseTexture)
		virtual void OnOK();
		virtual BOOL OnInitDialog();
		afx_msg void OnItemchangedListTexture (NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINITBASETEXTURE_H__AAF1EC86_71C5_4693_A4C3_F419336C3FE3__INCLUDED_)
