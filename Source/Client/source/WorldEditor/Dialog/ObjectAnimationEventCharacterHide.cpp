// ObjectAnimationEventCharacterHide.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventCharacterHide.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventCharacterHide dialog


CObjectAnimationEventCharacterHide::CObjectAnimationEventCharacterHide (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventCharacterHide::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventCharacterHide)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventCharacterHide::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventCharacterHide)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventCharacterHide, CDialog)
	//{{AFX_MSG_MAP(CObjectAnimationEventCharacterHide)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventCharacterHide normal functions

BOOL CObjectAnimationEventCharacterHide::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventCharacterHide::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}

void CObjectAnimationEventCharacterHide::Close()
{
}

void CObjectAnimationEventCharacterHide::GetData (CRaceMotionDataAccessor::TMotionEventData * pData)
{
}

void CObjectAnimationEventCharacterHide::SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData)
{
	CRaceMotionDataAccessor::TMotionCharacterHideEventData * c_pCharacterHideEvent = (CRaceMotionDataAccessor::TMotionCharacterHideEventData*)c_pData;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventCharacterHide message handlers
