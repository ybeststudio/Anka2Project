// texturepropertydlg.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "texturepropertydlg.h"
#include "../DataCtrl/MapAccessorOutdoor.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

#define Prolog() \
	CWorldEditorApp * pApplication = (CWorldEditorApp *)AfxGetApp(); \
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor(); \
	\
	CTextureSet * pTextureSet = CTerrain::GetTextureSet(); \
	TTerrainTexture & c_rTerrainTexture = pTextureSet->GetTexture(m_lTexNum); \
	\
	UpdateData()

#define PostSet() \
	UpdateData(FALSE); \
	ResetTextures()

#define PostUpdate(iEditControl) \
	if (iEditControl != 0) \
	{ \
		CEdit * pEdit = (CEdit *) GetDlgItem(iEditControl); \
		int len = pEdit->GetWindowTextLength(); \
		pEdit->SetSel(len, len); \
	} \
	\
	PostSet()

/////////////////////////////////////////////////////////////////////////////
// CTexturePropertyDlg dialog
CTexturePropertyDlg::CTexturePropertyDlg (CWnd* pParent /*=NULL*/)
	: CDialog (CTexturePropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTexturePropertyDlg)
	m_iBegin = 0;
	m_iEnd = 0;
	m_iUOffset = 0;
	m_iUScale = 0;
	m_iVOffset = 0;
	m_iVScale = 0;
	m_fEditUOffset = 0.0f;
	m_fEditUScale = 0.0f;
	m_fEditVOffset = 0.0f;
	m_fEditVScale = 0.0f;
	m_fEditBegin = 0.0f;
	m_fEditEnd = 0.0f;
	//}}AFX_DATA_INIT
}


void CTexturePropertyDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CTexturePropertyDlg)
	DDX_Control (pDX, IDC_SLIDER_VSCALE, m_SliderVScale);
	DDX_Control (pDX, IDC_SLIDER_VOFFSET, m_SliderVOffset);
	DDX_Control (pDX, IDC_SLIDER_USCALE, m_SliderUScale);
	DDX_Control (pDX, IDC_SLIDER_UOFFSET, m_SliderUOffset);
	DDX_Control (pDX, IDC_SLIDER_END, m_SliderEnd);
	DDX_Control (pDX, IDC_SLIDER_BEGIN, m_SliderBegin);
	DDX_Slider (pDX, IDC_SLIDER_BEGIN, m_iBegin);
	DDX_Slider (pDX, IDC_SLIDER_END, m_iEnd);
	DDX_Slider (pDX, IDC_SLIDER_UOFFSET, m_iUOffset);
	DDX_Slider (pDX, IDC_SLIDER_USCALE, m_iUScale);
	DDX_Slider (pDX, IDC_SLIDER_VOFFSET, m_iVOffset);
	DDX_Slider (pDX, IDC_SLIDER_VSCALE, m_iVScale);
	DDX_Text (pDX, IDC_EDIT_UOFFSET, m_fEditUOffset);
	DDV_MinMaxFloat (pDX, m_fEditUOffset, 0.f, 1.f);
	DDX_Text (pDX, IDC_EDIT_USCALE, m_fEditUScale);
	DDV_MinMaxFloat (pDX, m_fEditUScale, 0.f, 256.f);
	DDX_Text (pDX, IDC_EDIT_VOFFSET, m_fEditVOffset);
	DDV_MinMaxFloat (pDX, m_fEditVOffset, 0.f, 1.f);
	DDX_Text (pDX, IDC_EDIT_VSCALE, m_fEditVScale);
	DDV_MinMaxFloat (pDX, m_fEditVScale, 0.f, 256.f);
	DDX_Text (pDX, IDC_EDIT_BEGIN, m_fEditBegin);
	DDV_MinMaxFloat (pDX, m_fEditBegin, 0.f, 1.e+020f);
	DDX_Text (pDX, IDC_EDIT_END, m_fEditEnd);
	DDV_MinMaxFloat (pDX, m_fEditEnd, 0.f, 1.e+020f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CTexturePropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CTexturePropertyDlg)
	ON_BN_CLICKED (IDC_CHECK1, OnCheck1)
	ON_WM_HSCROLL()
	ON_EN_CHANGE (IDC_EDIT_BEGIN, OnUpdateEditBegin)
	ON_EN_CHANGE (IDC_EDIT_END, OnUpdateEditEnd)
	ON_EN_CHANGE (IDC_EDIT_UOFFSET, OnUpdateEditUOffset)
	ON_EN_CHANGE (IDC_EDIT_USCALE, OnUpdateEditUScale)
	ON_EN_CHANGE (IDC_EDIT_VOFFSET, OnUpdateEditVOffset)
	ON_EN_CHANGE (IDC_EDIT_VSCALE, OnUpdateEditVScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTexturePropertyDlg message handlers

void CTexturePropertyDlg::OnOK()
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}

BOOL CTexturePropertyDlg::OnInitDialog()
{
	if (m_lTexNum == 0)	// 텍스춰가 선택되지 않음
	{
		CDialog::OnCancel();
		return FALSE;
	}

	CDialog::OnInitDialog();

	m_SliderUScale.SetRange (0, 65535);
	m_SliderVScale.SetRange (0, 65535);
	m_SliderUOffset.SetRange (0, 65535);
	m_SliderVOffset.SetRange (0, 65535);
	m_SliderBegin.SetRange (0, 65535);
	m_SliderEnd.SetRange (0, 65535);

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor)
	{
		return FALSE;
	}

	CTextureSet * pTextureSet = CTerrain::GetTextureSet();

	const TTerrainTexture & c_rTerrainTexture = pTextureSet->GetTexture (m_lTexNum);

	SetDlgItemText (IDC_STATIC, c_rTerrainTexture.stFilename.c_str());

	m_fUScale	= c_rTerrainTexture.UScale;
	m_fVScale	= c_rTerrainTexture.VScale;
	m_fUOffset	= c_rTerrainTexture.UOffset;
	m_fVOffset	= c_rTerrainTexture.VOffset;
	m_bSplst	= c_rTerrainTexture.bSplat;
	m_usBegin	= c_rTerrainTexture.Begin;
	m_usEnd		= c_rTerrainTexture.End;

	m_fHeightScale = pMapManagerAccessor->GetMapOutdoorRef().GetHeightScale();

	m_SliderUScale.SetPos ((int) m_fUScale * 255);
	m_SliderVScale.SetPos ((int) m_fVScale * 255);
	m_SliderUOffset.SetPos ((int) m_fVOffset * 65535);
	m_SliderVOffset.SetPos ((int) m_fVOffset * 65535);
	m_SliderBegin.SetPos (m_usBegin);
	m_SliderEnd.SetPos (m_usEnd);
	UpdateData();

	m_fEditUScale = m_fUScale;
	m_fEditVScale = m_fVScale;
	m_fEditUOffset = m_fUOffset;
	m_fEditVOffset = m_fVOffset;
	m_fEditBegin = (float) (m_usBegin) * m_fHeightScale / 100.0f;
	m_fEditEnd = (float) (m_usEnd) * m_fHeightScale / 100.0f;
	UpdateData (FALSE);

	if (m_bSplst)
	{
		CheckDlgButton (IDC_CHECK1, 1);
		m_SliderBegin.EnableWindow (true);
		m_SliderEnd.EnableWindow (true);
		GetDlgItem (IDC_EDIT_BEGIN)->EnableWindow (true);
		GetDlgItem (IDC_EDIT_END)->EnableWindow (true);
	}
	else
	{
		CheckDlgButton (IDC_CHECK1, 0);
		m_SliderBegin.EnableWindow (false);
		m_SliderEnd.EnableWindow (false);
		GetDlgItem (IDC_EDIT_BEGIN)->EnableWindow (false);
		GetDlgItem (IDC_EDIT_END)->EnableWindow (false);
	}

	AfxGetMainWnd()->PostMessage (WM_SYSKEYDOWN, 18, 0);
	AfxGetMainWnd()->PostMessage (WM_SYSKEYUP, 18, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTexturePropertyDlg::OnCheck1()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	CTextureSet * pTextureSet = CTerrain::GetTextureSet();

	TTerrainTexture & c_rTerrainTexture = pTextureSet->GetTexture (m_lTexNum);

	if (IsDlgButtonChecked (IDC_CHECK1))
	{
		m_SliderBegin.EnableWindow (true);
		m_SliderEnd.EnableWindow (true);
		GetDlgItem (IDC_EDIT_BEGIN)->EnableWindow (true);
		GetDlgItem (IDC_EDIT_END)->EnableWindow (true);
		c_rTerrainTexture.bSplat = true;
	}
	else
	{
		m_SliderBegin.EnableWindow (false);
		m_SliderEnd.EnableWindow (false);
		GetDlgItem (IDC_EDIT_BEGIN)->EnableWindow (false);
		GetDlgItem (IDC_EDIT_END)->EnableWindow (false);
		c_rTerrainTexture.bSplat = false;
	}

	ResetTextures();
}

void CTexturePropertyDlg::OnCancel()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	CTextureSet * pTextureSet = CTerrain::GetTextureSet();

	TTerrainTexture & c_rTerrainTexture = pTextureSet->GetTexture (m_lTexNum);

	c_rTerrainTexture.UScale = m_fUScale;
	c_rTerrainTexture.VScale = m_fVScale;
	c_rTerrainTexture.UOffset = m_fUOffset;
	c_rTerrainTexture.VOffset = m_fVOffset;
	c_rTerrainTexture.Begin = m_usBegin;
	c_rTerrainTexture.End = m_usEnd;

	c_rTerrainTexture.bSplat = m_bSplst;

	ResetTextures();
	CDialog::OnCancel();
}

void CTexturePropertyDlg::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->GetSafeHwnd() == m_SliderUScale.GetSafeHwnd())
	{
		SetUScale (m_SliderUScale.GetPos());
	}
	else if (pScrollBar->GetSafeHwnd() == m_SliderVScale.GetSafeHwnd())
	{
		SetVScale (m_SliderVScale.GetPos());
	}
	else if (pScrollBar->GetSafeHwnd() == m_SliderUOffset.GetSafeHwnd())
	{
		SetUOffset (m_SliderUOffset.GetPos());
	}
	else if (pScrollBar->GetSafeHwnd() == m_SliderVOffset.GetSafeHwnd())
	{
		SetVOffset (m_SliderVOffset.GetPos());
	}
	else if (pScrollBar->GetSafeHwnd() == m_SliderBegin.GetSafeHwnd())
	{
		SetBegin (m_SliderBegin.GetPos());
	}
	else if (pScrollBar->GetSafeHwnd() == m_SliderEnd.GetSafeHwnd())
	{
		SetEnd (m_SliderEnd.GetPos());
	}

	ResetTextures();
	CDialog::OnHScroll (nSBCode, nPos, pScrollBar);
}

void CTexturePropertyDlg::ResetTextures()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor)
	{
		return;
	}

	pMapManagerAccessor->ResetTerrainTexture();
}

//
// SetXX
//
void CTexturePropertyDlg::SetUScale (int iUScale)
{
	Prolog();

	c_rTerrainTexture.UScale = (float)iUScale / 256.0f;
	m_fEditUScale = c_rTerrainTexture.UScale;

	PostSet();
}

void CTexturePropertyDlg::SetVScale (int iVScale)
{
	Prolog();

	c_rTerrainTexture.VScale = (float)iVScale / 256.0f;
	m_fEditVScale = c_rTerrainTexture.VScale;

	PostSet();
}

void CTexturePropertyDlg::SetUOffset (int iUOffset)
{
	Prolog();

	c_rTerrainTexture.UOffset = (float)iUOffset / 65535.0f;
	m_fEditUOffset = c_rTerrainTexture.UOffset;

	PostSet();
}

void CTexturePropertyDlg::SetVOffset (int iVOffset)
{
	Prolog();

	c_rTerrainTexture.VOffset = (float)iVOffset / 65535.0f;
	m_fEditVOffset = c_rTerrainTexture.VOffset;

	PostSet();
}

void CTexturePropertyDlg::SetBegin (int iBegin)
{
	Prolog();

	c_rTerrainTexture.Begin = iBegin;
	m_fEditBegin = (float) (c_rTerrainTexture.Begin) * m_fHeightScale / 100.0f;

	PostSet();
}

void CTexturePropertyDlg::SetEnd (int iEnd)
{
	Prolog();

	c_rTerrainTexture.End = iEnd;
	m_fEditEnd = (float) (c_rTerrainTexture.End) * m_fHeightScale / 100.0f;

	PostSet();
}

//
// OnUpdateXX
//
void CTexturePropertyDlg::OnUpdateEditBegin()
{
	Prolog();

	m_fEditBegin = fMINMAX (0.0f, m_fEditBegin, 65535.0f * m_fHeightScale / 100.0f);

	UpdateData (FALSE);
	c_rTerrainTexture.Begin = (unsigned short) (m_fEditBegin / m_fHeightScale * 100);
	m_SliderBegin.SetPos (c_rTerrainTexture.Begin);

	PostUpdate (IDC_EDIT_BEGIN);
}

void CTexturePropertyDlg::OnUpdateEditEnd()
{
	Prolog();

	m_fEditEnd = fMINMAX (0.0f, m_fEditEnd, 65535.0f * m_fHeightScale / 100.0f);

	UpdateData (FALSE);
	c_rTerrainTexture.End = (unsigned short) (m_fEditEnd / m_fHeightScale * 100);
	m_SliderEnd.SetPos (c_rTerrainTexture.End);

	PostUpdate (IDC_EDIT_END);
}

void CTexturePropertyDlg::OnUpdateEditUOffset()
{
	Prolog();

	m_fEditUOffset = fMINMAX (0.0f, m_fEditUOffset, 1.0f);

	UpdateData (FALSE);
	c_rTerrainTexture.UOffset = m_fEditUOffset;
	m_SliderUOffset.SetPos ((int) (m_fEditUOffset * 65535));

	PostUpdate (IDC_EDIT_UOFFSET);
}

void CTexturePropertyDlg::OnUpdateEditUScale()
{
	Prolog();

	m_fEditUScale = fMINMAX (0.0f, m_fEditUScale, 256.0f);

	UpdateData (FALSE);
	c_rTerrainTexture.UScale = m_fEditUScale;
	m_SliderUScale.SetPos ((int) (m_fEditUScale * 256));

	PostUpdate (IDC_EDIT_USCALE);
}

void CTexturePropertyDlg::OnUpdateEditVOffset()
{
	Prolog();

	m_fEditVOffset = fMINMAX (0.0f, m_fEditVOffset, 1.0f);

	UpdateData (FALSE);
	c_rTerrainTexture.VOffset = m_fEditVOffset;
	m_SliderVOffset.SetPos ((int) (m_fEditVOffset * 65535));

	PostUpdate (IDC_EDIT_VOFFSET);
}

void CTexturePropertyDlg::OnUpdateEditVScale()
{
	Prolog();

	m_fEditVScale = fMINMAX (0.0f, m_fEditVScale, 256.0f);

	UpdateData (FALSE);
	c_rTerrainTexture.VScale = m_fEditVScale;
	m_SliderVScale.SetPos ((int) (m_fEditVScale * 256));

	PostUpdate (IDC_EDIT_VSCALE);
}
