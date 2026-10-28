#if !defined(AFX_EFFECTTEXTUREPAGE_H__4D0B5194_467C_487F_9480_3A41BDB4DB2B__INCLUDED_)
#define AFX_EFFECTTEXTUREPAGE_H__4D0B5194_467C_487F_9480_3A41BDB4DB2B__INCLUDED_

#pragma once

// EffectTexturePage.h : header file
//

//------------------------//
// Test Code
#include "EffectUI.h"
//------------------------//

/////////////////////////////////////////////////////////////////////////////
// CEffectTexturePage dialog

class CEffectTexturePage : public CPageCtrl
{
		// Construction
	public:
		CEffectTexturePage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent);

		void UpdateUI();

		void SetData (DWORD dwIndex);

		// Dialog Data
		//{{AFX_DATA(CEffectTexturePage)
		enum { IDD = IDD_EFFECT_TEXTURE };
		CBlendTypeComboBox	m_ctrlBlendDestType;
		CBlendTypeComboBox	m_ctrlBlendSrcType;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectTexturePage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CEffectTexturePage)
		// NOTE: the ClassWizard will add member functions here
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTEXTUREPAGE_H__4D0B5194_467C_487F_9480_3A41BDB4DB2B__INCLUDED_)
