#if !defined(AFX_MAPOBJECTPROPERTYPAGEBUILDING_H__7A9F05C7_54E4_4565_AAB6_C1D564191CBD__INCLUDED_)
#define AFX_MAPOBJECTPROPERTYPAGEBUILDING_H__7A9F05C7_54E4_4565_AAB6_C1D564191CBD__INCLUDED_

#pragma once

// MapObjectPropertyPageBuilding.h : header file
//

// Test Code
#include "MapObjectPropertyPageBase.h"

class CMapObjectProperty;

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageBuilding dialog

class CMapObjectPropertyPageBuilding : public CMapObjectPropertyPageBase
{
		// Construction
	public:
		CMapObjectPropertyPageBuilding (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CMapObjectProperty * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CMapObjectPropertyPageBuilding)
		enum { IDD = IDD_MAP_OBJECT_PROPERTY_PAGE_BUILDING };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapObjectPropertyPageBuilding)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual void OnOK();
		virtual void OnCancel();
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CMapObjectPropertyPageBuilding)
		afx_msg void OnLoadBuildingFile();
		afx_msg void OnCheckShadowFlag();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void OnUpdateUI (CProperty * pProperty);
		void OnUpdatePropertyData (const char* c_szPropertyName);
		bool OnSave (const char* c_szPathName, CProperty * pProperty);
		void OnRender (HWND hWnd);

	protected:
		CScreen m_Screen;

		prt::TPropertyBuilding m_propertyBuilding;

		CMapObjectProperty* m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPOBJECTPROPERTYPAGEBUILDING_H__7A9F05C7_54E4_4565_AAB6_C1D564191CBD__INCLUDED_)
