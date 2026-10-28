// ObjectAnimationEventWarp.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventWarp.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventWarp dialog


CObjectAnimationEventWarp::CObjectAnimationEventWarp (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventWarp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventWarp)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventWarp::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventWarp)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventWarp, CDialog)
	//{{AFX_MSG_MAP(CObjectAnimationEventWarp)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventWarp normal functions

BOOL CObjectAnimationEventWarp::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventWarp::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}

void CObjectAnimationEventWarp::Close()
{
}

void CObjectAnimationEventWarp::GetData (CRaceMotionData::TMotionEventData * pMotionEventData)
{
}

void CObjectAnimationEventWarp::SetData (const CRaceMotionData::TMotionEventData * c_pMotionEventData)
{
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventWarp message handlers
