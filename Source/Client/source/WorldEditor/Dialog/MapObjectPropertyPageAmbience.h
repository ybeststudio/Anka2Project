#if !defined(AFX_MAPOBJECTPROPERTYPAGEAMBIENCE_H__00B8872E_44D3_4AC4_BADD_DC037A63F25B__INCLUDED_)
#define AFX_MAPOBJECTPROPERTYPAGEAMBIENCE_H__00B8872E_44D3_4AC4_BADD_DC037A63F25B__INCLUDED_

#pragma once

// MapObjectPropertyPageAmbience.h : header file
//

#include "MapObjectPropertyPageBase.h"

class CMapObjectProperty;

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageAmbience dialog

class CMapObjectPropertyPageAmbience : public CMapObjectPropertyPageBase
{
		// Construction
	public:
		CMapObjectPropertyPageAmbience (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CMapObjectProperty * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CMapObjectPropertyPageAmbience)
		enum { IDD = IDD_MAP_OBJECT_PROPERTY_PAGE_AMBIENCE };
		CComboBox	m_ctrlPlayType;
		CListBox	m_ctrlSoundFileList;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapObjectPropertyPageAmbience)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual void OnOK();
		virtual void OnCancel();
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		void OnUpdateUI (CProperty * pProperty);
		void OnUpdatePropertyData (const char* c_szPropertyName);
		void OnRender (HWND hWnd);
		bool OnSave (const char* c_szPathName, CProperty * pProperty);

		const char* __GetPlayTypeName();

		// Generated message map functions
		//{{AFX_MSG(CMapObjectPropertyPageAmbience)
		afx_msg void OnNewSoundFile();
		afx_msg void OnDeleteSoundFile();
		afx_msg void OnChangePlayType();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CScreen m_Screen;

		prt::TPropertyAmbience m_propertyAmbience;

		CMapObjectProperty* m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPOBJECTPROPERTYPAGEAMBIENCE_H__00B8872E_44D3_4AC4_BADD_DC037A63F25B__INCLUDED_)
