// ObjectAnimationEventScreenWaving.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventScreenWaving.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventScreenWaving dialog


CObjectAnimationEventScreenWaving::CObjectAnimationEventScreenWaving (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventScreenWaving::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventScreenWaving)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventScreenWaving::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventScreenWaving)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventScreenWaving, CDialog)
	//{{AFX_MSG_MAP(CObjectAnimationEventScreenWaving)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventScreenWaving normal functions

BOOL CObjectAnimationEventScreenWaving::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventScreenWaving::IDD, pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_DURATION_TIME, "0.1");
	SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_POWER, "100");
	SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_AFFECTING_RANGE, "0");

	return TRUE;
}

void CObjectAnimationEventScreenWaving::Close()
{
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_DURATION_TIME, m_strDurationTime);
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_POWER, m_strPower);
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_AFFECTING_RANGE, m_strAffectingRange);
}

void CObjectAnimationEventScreenWaving::GetData (CRaceMotionDataAccessor::TMotionEventData * pData)
{
	CRaceMotionDataAccessor::TScreenWavingEventData * pScreenWavingEventData = (CRaceMotionDataAccessor::TScreenWavingEventData*)pData;
	pScreenWavingEventData->fDurationTime = atof (m_strDurationTime);
	pScreenWavingEventData->iPower = atoi (m_strPower);
	pScreenWavingEventData->iAffectingRange = atoi (m_strAffectingRange);
}

void CObjectAnimationEventScreenWaving::SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData)
{
	const CRaceMotionDataAccessor::TScreenWavingEventData * c_pScreenWavingEventData = (const CRaceMotionDataAccessor::TScreenWavingEventData*)c_pData;
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_DURATION_TIME, c_pScreenWavingEventData->fDurationTime);
	SetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_POWER, c_pScreenWavingEventData->iPower);
	SetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_AFFECTING_RANGE, c_pScreenWavingEventData->iAffectingRange);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventScreenWaving message handlers
