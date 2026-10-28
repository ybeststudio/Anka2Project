// MapArrangeHeightProgress.cpp : implementation file
//

#include "StdAfx.h"
#include "../worldeditor.h"
#include "MapArrangeHeightProgressDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapArrangeHeightProgress dialog


CMapArrangeHeightProgress::CMapArrangeHeightProgress (CWnd* pParent /*=NULL*/)
	: CDialog (CMapArrangeHeightProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapArrangeHeightProgress)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapArrangeHeightProgress::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapArrangeHeightProgress)
	DDX_Control (pDX, IDC_MAP_ARRANGE_HEIGHT_PROGRESS_NOTIFY, m_ctrlNotify);
	DDX_Control (pDX, IDC_MAP_ARRANGE_HEIGHT_PROGRESS, m_ctrlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapArrangeHeightProgress, CDialog)
	//{{AFX_MSG_MAP(CMapArrangeHeightProgress)
	ON_BN_CLICKED (IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapArrangeHeightProgress normal functions

void CMapArrangeHeightProgress::Init()
{
	m_bStopFlag = FALSE;
	m_ctrlProgress.SetPos (0);
	m_ctrlNotify.SetWindowText ("");
}

void CMapArrangeHeightProgress::SetProgress (int iPos)
{
	m_ctrlProgress.SetPos (iPos);
}

void CMapArrangeHeightProgress::AddLine (const char* c_szFmt, ...)
{
	va_list	args;
	char	szBuf[1024];

	va_start (args, c_szFmt);
	vsprintf (szBuf, c_szFmt, args);
	va_end (args);

	m_kVec_strLine.push_back (szBuf);

	std::string strLineAll;
	for (DWORD i = 0; i < m_kVec_strLine.size(); ++i)
	{
		strLineAll += m_kVec_strLine[i];
	}

	m_ctrlNotify.SetWindowText (strLineAll.c_str());
}

/////////////////////////////////////////////////////////////////////////////
// CMapArrangeHeightProgress message handlers

void CMapArrangeHeightProgress::OnOK()
{
}

void CMapArrangeHeightProgress::OnCancel()
{
	m_bStopFlag = TRUE;
}
