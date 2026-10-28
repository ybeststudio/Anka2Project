// DlgPerspectiveSetting.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "DlgPerspective.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPerspective dialog


CDlgPerspective::CDlgPerspective (CWnd* pParent /*=NULL*/)
	: CDialog (CDlgPerspective::IDD, pParent)
{
	m_fFOV = 40.0f;
	m_fNearDistance = 100.0f;
	m_fFarDistance = 30000.0f;

	//{{AFX_DATA_INIT(CDlgPerspective)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgPerspective::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CDlgPerspective)
	DDX_Control (pDX, IDC_DIALOG_PERSPECTIVE_FAR_DISTANCE, m_ctrlFarDistance);
	DDX_Control (pDX, IDC_DIALOG_PERSPECTIVE_NEAR_DISTANCE, m_ctrlNearDistance);
	DDX_Control (pDX, IDC_DIALOG_PERSPECTIVE_FOV, m_ctrlFOV);
	//}}AFX_DATA_MAP

	m_ctrlFOV.SetRangeMin (1);
	m_ctrlFOV.SetRangeMax (900);
	m_ctrlNearDistance.SetRangeMin (1);
	m_ctrlNearDistance.SetRangeMax (100);
	m_ctrlFarDistance.SetRangeMin (100);
	m_ctrlFarDistance.SetRangeMax (500000);
}


BEGIN_MESSAGE_MAP (CDlgPerspective, CDialog)
	//{{AFX_MSG_MAP(CDlgPerspective)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPerspective normal functions

void CDlgPerspective::SetFOV (float fFOV)
{
	m_fFOV = fFOV;
	m_ctrlFOV.SetPos (int (fFOV * 10.0f));
	SetDialogFloatText (GetSafeHwnd(), IDC_DIALOG_PERSPECTIVE_FOV_PRINT, m_fFOV);
}
void CDlgPerspective::SetNearDistance (float fDistance)
{
	m_fNearDistance = fDistance;
	m_ctrlNearDistance.SetPos (int (fDistance));
	SetDialogIntegerText (GetSafeHwnd(), IDC_DIALOG_PERSPECTIVE_NEAR_DISTANCE_PRINT, m_fNearDistance);
}
void CDlgPerspective::SetFarDistance (float fDistance)
{
	m_fFarDistance = fDistance;
	m_ctrlFarDistance.SetPos (int (fDistance));
	SetDialogIntegerText (GetSafeHwnd(), IDC_DIALOG_PERSPECTIVE_FAR_DISTANCE_PRINT, m_fFarDistance);
}

float CDlgPerspective::GetFOV()
{
	return m_fFOV;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgPerspective message handlers

void CDlgPerspective::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_fFOV = float (m_ctrlFOV.GetPos()) / 10.0f;
	SetDialogFloatText (GetSafeHwnd(), IDC_DIALOG_PERSPECTIVE_FOV_PRINT, m_fFOV);
	CDialog::OnHScroll (nSBCode, nPos, pScrollBar);
}
