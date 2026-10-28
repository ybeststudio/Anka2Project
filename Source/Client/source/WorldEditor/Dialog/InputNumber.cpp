// InputNumber.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "InputNumber.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputNumber dialog


CInputNumber::CInputNumber (CWnd* pParent /*=NULL*/)
	: CDialog (CInputNumber::IDD, pParent)
{
	m_iNumber = 0;
	//{{AFX_DATA_INIT(CInputNumber)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInputNumber::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CInputNumber)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CInputNumber, CDialog)
	//{{AFX_MSG_MAP(CInputNumber)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputNumber normal functions

int CInputNumber::GetNumber()
{
	return m_iNumber;
}

/////////////////////////////////////////////////////////////////////////////
// CInputNumber message handlers

void CInputNumber::OnOK()
{
	m_iNumber = GetDialogIntegerText (GetSafeHwnd(), IDC_NUMBER);
	EndDialog (TRUE);
}

void CInputNumber::OnCancel()
{
	EndDialog (FALSE);
}

int CInputNumber::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate (lpCreateStruct) == -1)
	{
		return -1;
	}

	SetDialogIntegerText (GetSafeHwnd(), IDC_NUMBER, 0);
	::SetFocus (GetDlgItem (IDC_NUMBER)->GetSafeHwnd());

	return 0;
}
