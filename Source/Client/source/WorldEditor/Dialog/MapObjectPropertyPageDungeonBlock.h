#if !defined(AFX_MAPOBJECTPROPERTYPAGEDUNGEONBLOCK_H__1A5ACF7D_0F80_4AD8_91C1_63A343F91ADF__INCLUDED_)
#define AFX_MAPOBJECTPROPERTYPAGEDUNGEONBLOCK_H__1A5ACF7D_0F80_4AD8_91C1_63A343F91ADF__INCLUDED_

#pragma once

// MapObjectPropertyPageDungeonBlock.h : header file
//

#include "MapObjectPropertyPageBase.h"

class CMapObjectProperty;

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageDungeonBlock dialog

class CMapObjectPropertyPageDungeonBlock : public CMapObjectPropertyPageBase
{
		// Construction
	public:
		CMapObjectPropertyPageDungeonBlock (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CMapObjectProperty * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CMapObjectPropertyPageDungeonBlock)
		enum { IDD = IDD_MAP_OBJECT_PROPERTY_PAGE_DUNGEON_BLOCK };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapObjectPropertyPageDungeonBlock)
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

		// Generated message map functions
		//{{AFX_MSG(CMapObjectPropertyPageDungeonBlock)
		afx_msg void OnLoadDungeonBlockFile();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CScreen m_Screen;

		prt::TPropertyDungeonBlock m_propertyDungeonBlock;

		CMapObjectProperty* m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPOBJECTPROPERTYPAGEDUNGEONBLOCK_H__1A5ACF7D_0F80_4AD8_91C1_63A343F91ADF__INCLUDED_)
