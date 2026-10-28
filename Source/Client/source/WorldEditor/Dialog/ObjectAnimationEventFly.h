#if !defined(AFX_OBJECTANIMATIONEVENTFLY_H__62219444_D886_4AE7_82FB_99FD9C72BF8B__INCLUDED_)
#define AFX_OBJECTANIMATIONEVENTFLY_H__62219444_D886_4AE7_82FB_99FD9C72BF8B__INCLUDED_

#pragma once

// ObjectAnimationEventFly.h : header file
//

#include "ObjectAnimationEventBase.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventFly dialog
class CObjectAnimationEvent;

class CObjectAnimationEventFly : public CObjectAnimationEventBase
{
		// Construction
	public:
		CObjectAnimationEventFly (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationEventFly)
		enum { IDD = IDD_OBJECT_ANIMATION_EVENT_FLY };
		CComboBox	m_ctrlBone;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationEventFly)
	protected:
		virtual void OnOK();
		virtual void OnCancel();
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	public:
		BOOL canClose();
		void Close();

		void GetData (CRaceMotionDataAccessor::TMotionEventData * pData);
		void SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData);

	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectAnimationEventFly)
		afx_msg void OnObjectAnimationEventFlyLoad();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		CString m_strFlyFileName;

		BOOL m_isAttachingEnable;
		D3DXVECTOR3 m_v3FlyPosition;
		std::string m_strAttachingBoneName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONEVENTFLY_H__62219444_D886_4AE7_82FB_99FD9C72BF8B__INCLUDED_)
