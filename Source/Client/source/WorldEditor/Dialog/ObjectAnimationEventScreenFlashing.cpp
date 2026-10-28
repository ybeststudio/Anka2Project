// ObjectAnimationEventScreenFlashing.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventScreenFlashing.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventScreenFlashing dialog


CObjectAnimationEventScreenFlashing::CObjectAnimationEventScreenFlashing (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventScreenFlashing::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventScreenFlashing)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventScreenFlashing::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventScreenFlashing)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventScreenFlashing, CDialog)
	//{{AFX_MSG_MAP(CObjectAnimationEventScreenFlashing)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventScreenFlashing normal functions

BOOL CObjectAnimationEventScreenFlashing::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventScreenFlashing::IDD, pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}

void CObjectAnimationEventScreenFlashing::Close()
{
}

void CObjectAnimationEventScreenFlashing::GetData (CRaceMotionDataAccessor::TMotionEventData * pData)
{
}

void CObjectAnimationEventScreenFlashing::SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData)
{
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventScreenFlashing message handlers
