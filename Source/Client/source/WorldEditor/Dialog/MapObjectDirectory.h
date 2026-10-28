#if !defined(AFX_MAPOBJECTCREATEDIRECTORY_H__5AC43102_595A_4F7C_BB5F_D0D971B66B77__INCLUDED_)
#define AFX_MAPOBJECTCREATEDIRECTORY_H__5AC43102_595A_4F7C_BB5F_D0D971B66B77__INCLUDED_

#pragma once

// MapObjectCreateDirectory.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapObjectDirectory dialog

class CMapObjectDirectory : public CDialog
{
		// Construction
	public:
		CMapObjectDirectory (CWnd* pParent = NULL);  // standard constructor

		// Dialog Data
		//{{AFX_DATA(CMapObjectDirectory)
		enum { IDD = IDD_MAP_OBJECT_CREATE_DIRECTORY };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA

		// Operations
	public:
		const char* GetDirectoryName();

		// Overrides
	public:

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapObjectDirectory)
	protected:
		virtual void OnOK();
		virtual void OnCancel();
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CMapObjectDirectory)
		afx_msg void OnDirectoryAccept();
		afx_msg void OnDirectoryCancel();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		char m_szFileName[64 + 1];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPOBJECTCREATEDIRECTORY_H__5AC43102_595A_4F7C_BB5F_D0D971B66B77__INCLUDED_)
