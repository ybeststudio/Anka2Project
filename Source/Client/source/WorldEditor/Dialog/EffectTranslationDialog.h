#if !defined(AFX_EFFECTTRANSLATIONDIALOG_H__2760C2B7_1F13_4C8E_95E2_C1C302DA902E__INCLUDED_)
#define AFX_EFFECTTRANSLATIONDIALOG_H__2760C2B7_1F13_4C8E_95E2_C1C302DA902E__INCLUDED_

#pragma once

// EffectTranslationDialog.h : header file
//

#include "../DataCtrl/EffectAccessor.h"

/////////////////////////////////////////////////////////////////////////////
// CEffectTranslationDialog dialog

class CEffectTranslationDialog : public CDialog
{
		// Event Reciever
	public:
		class IEventReciever
		{
			public:
				virtual void OnEvent() = 0;
		};

		// Construction
	public:
		CEffectTranslationDialog (IEventReciever * pEventReciever = NULL);  // standard constructor

		// Operation
	public:
		void GetPosition (float* pfx, float* pfy, float* pfz);
		void SetPosition (float fx, float fy, float fz);

		// Dialog Data
		//{{AFX_DATA(CEffectTranslationDialog)
		enum { IDD = IDD_EFFECT_TRANSLATION };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectTranslationDialog)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CEffectTranslationDialog)
		afx_msg void OnChangePosition();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		IEventReciever* m_pEventReciever;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTRANSLATIONDIALOG_H__2760C2B7_1F13_4C8E_95E2_C1C302DA902E__INCLUDED_)
