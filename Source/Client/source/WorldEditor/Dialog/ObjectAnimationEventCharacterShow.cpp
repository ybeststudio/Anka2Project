// ObjectAnimationEventCharacterShow.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventCharacterShow.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventCharacterShow dialog

CObjectAnimationEventCharacterShow::CObjectAnimationEventCharacterShow (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventCharacterShow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventCharacterShow)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventCharacterShow::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventCharacterShow)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventCharacterShow, CDialog)
	//{{AFX_MSG_MAP(CObjectAnimationEventCharacterShow)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventCharacterShow normal functions

BOOL CObjectAnimationEventCharacterShow::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventCharacterShow::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}

void CObjectAnimationEventCharacterShow::Close()
{
}

void CObjectAnimationEventCharacterShow::GetData (CRaceMotionDataAccessor::TMotionEventData * pData)
{
}

void CObjectAnimationEventCharacterShow::SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData)
{
	const CRaceMotionDataAccessor::TMotionCharacterShowEventData * c_pCharacterShowEventData = (const CRaceMotionDataAccessor::TMotionCharacterShowEventData*)c_pData;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventCharacterShow message handlers
