#if !defined(AFX_MAPOBJECTPROPERTYPAGEEFFECT_H__3C31F9B8_D921_4A26_9261_271B0FC110C4__INCLUDED_)
#define AFX_MAPOBJECTPROPERTYPAGEEFFECT_H__3C31F9B8_D921_4A26_9261_271B0FC110C4__INCLUDED_

#pragma once

// MapObjectPropertyPageEffect.h : header file
//
#include "MapObjectPropertyPageBase.h"
class CMapObjectProperty;

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageEffect dialog

class CMapObjectPropertyPageEffect : public CMapObjectPropertyPageBase
{
		// Construction
	public:
		CMapObjectPropertyPageEffect (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CMapObjectProperty * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CMapObjectPropertyPageEffect)
		enum { IDD = IDD_MAP_OBJECT_PROPERTY_PAGE_EFFECT };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapObjectPropertyPageEffect)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual void OnOK();
		virtual void OnCancel();
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CMapObjectPropertyPageEffect)
		afx_msg void OnLoadEffectFile();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void OnUpdateUI (CProperty * pProperty);
		void OnUpdatePropertyData (const char* c_szPropertyName);
		bool OnSave (const char* c_szPathName, CProperty * pProperty);
		void OnRender (HWND hWnd);

		CScreen m_Screen;

		prt::TPropertyEffect m_propertyEffect;

		CMapObjectProperty* m_pParent;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPOBJECTPROPERTYPAGEEFFECT_H__3C31F9B8_D921_4A26_9261_271B0FC110C4__INCLUDED_)
