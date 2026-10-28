// GTColorDialogDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "GTColorDialogDlg.h"
#include "../Resource.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGTColorDialogDlg dialog

CGTColorDialogDlg::CGTColorDialogDlg (CWnd* pParent /*=NULL*/)
	: CDialog (CGTColorDialogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGTColorDialogDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon (IDR_MAINFRAME);
	m_crColor = RGB (255, 0, 0);
	m_bChangingHSV = FALSE;
	m_bChangingRGB = FALSE;
	m_bChangingHEX = FALSE;
}

void CGTColorDialogDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CGTColorDialogDlg)
	DDX_Control (pDX, IDC_EDITHEX, m_edtHex);
	DDX_Control (pDX, IDC_EDT_HSV_V, m_edt_HSV_V);
	DDX_Control (pDX, IDC_EDT_HSV_S, m_edt_HSV_S);
	DDX_Control (pDX, IDC_EDT_HSV_H, m_edt_HSV_H);
	DDX_Control (pDX, IDC_EDT_RGB_R, m_edt_RGB_R);
	DDX_Control (pDX, IDC_EDT_RGB_G, m_edt_RGB_G);
	DDX_Control (pDX, IDC_EDT_RGB_B, m_edt_RGB_B);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP (CGTColorDialogDlg, CDialog)
	//{{AFX_MSG_MAP(CGTColorDialogDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED (IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED (IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED (IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED (IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED (IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED (IDC_RADIO6, OnRadio6)
	ON_EN_CHANGE (IDC_EDT_HSV_H, OnChangeEdtHsvH)
	ON_EN_CHANGE (IDC_EDT_HSV_S, OnChangeEdtHsvS)
	ON_EN_CHANGE (IDC_EDT_HSV_V, OnChangeEdtHsvV)
	ON_EN_CHANGE (IDC_EDT_RGB_B, OnChangeEdtRgbB)
	ON_EN_CHANGE (IDC_EDT_RGB_G, OnChangeEdtRgbG)
	ON_EN_CHANGE (IDC_EDT_RGB_R, OnChangeEdtRgbR)
	ON_EN_CHANGE (IDC_EDITHEX, OnChangeEdithex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGTColorDialogDlg message handlers

BOOL CGTColorDialogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	/*
	ASSERT((IDD_ABOUTBOX & 0xFFF0) == IDD_ABOUTBOX);
	ASSERT(IDD_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDD_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDD_ABOUTBOX, strAboutMenu);
		}
	}
	*/

	// set parent so we get colorchanged event notices...
	m_wndWell.m_pParent = this;

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon (m_hIcon, TRUE);			// Set big icon
	SetIcon (m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect theRect;
	GetDlgItem (IDC_COLORPICKER)->GetWindowRect (&theRect);
	ScreenToClient (&theRect);
	m_wndBulleted.Create (WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE, theRect, this, IDC_COLORPICKER, 1);

	// TODO: Add extra initialization here
	theRect;
	GetDlgItem (IDC_COLORWELL)->GetWindowRect (&theRect);
	ScreenToClient (&theRect);
	m_wndWell.Create (WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE, theRect, this, IDC_COLORWELL, 1);

	// TODO: Add extra initialization here
	theRect;
	GetDlgItem (IDC_COLORSLIDER)->GetWindowRect (&theRect);
	ScreenToClient (&theRect);
	m_wndSlider.Create (WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE, theRect, this, IDC_COLORSLIDER, 1);

	m_wndBulleted.pNotifyWnd = &m_wndSlider;
	m_wndBulleted.pNotifyWellWnd = &m_wndWell;

	m_wndBulleted.SetDataValue (modes_hsv_hue | modes_reverse);
	m_wndSlider.SetDataValue (modes_hsv_hue | modes_reverse);
	m_wndSlider.pNotifyWnd = &m_wndBulleted;
	m_wndWell.pNotifyWnd = &m_wndBulleted;

	CheckRadioButton (IDC_RADIO1, IDC_RADIO6, IDC_RADIO4);
	m_wndBulleted.SetColor (m_crColor);

	// create a font with even metrix for the hex
	LOGFONT lFont;
	m_edtHex.GetFont()->GetLogFont (&lFont);
	lFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	m_hxFont.CreateFontIndirect (&lFont);
	m_edtHex.SetFont (&m_hxFont);

	m_edtHex.SetWindowText (ColorToString (m_crColor));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGTColorDialogDlg::OnSysCommand (UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand (nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGTColorDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc (this); // device context for painting

		SendMessage (WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics (SM_CXICON);
		int cyIcon = GetSystemMetrics (SM_CYICON);
		CRect rect;
		GetClientRect (&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon (x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGTColorDialogDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGTColorDialogDlg::OnRadio1()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue (modes_rgb_red | modes_reverse);
	m_wndSlider.SetDataValue (modes_rgb_red | modes_reverse);
	m_wndBulleted.SetColor (old_color);
}

void CGTColorDialogDlg::OnRadio2()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue (modes_rgb_green | modes_reverse);
	m_wndSlider.SetDataValue (modes_rgb_green | modes_reverse);
	m_wndBulleted.SetColor (old_color);
}

void CGTColorDialogDlg::OnRadio3()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue (modes_rgb_blue | modes_reverse);
	m_wndSlider.SetDataValue (modes_rgb_blue | modes_reverse);
	m_wndBulleted.SetColor (old_color);
}

void CGTColorDialogDlg::OnRadio4()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue (modes_hsv_hue | modes_reverse);
	m_wndSlider.SetDataValue (modes_hsv_hue | modes_reverse);
	m_wndBulleted.SetColor (old_color);
}

void CGTColorDialogDlg::OnRadio5()
{
	// TODO: Add your control notification handler code here
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	m_wndBulleted.SetDataValue (modes_hsv_sat | modes_reverse);
	m_wndSlider.SetDataValue (modes_hsv_sat | modes_reverse);
	m_wndBulleted.SetColor (old_color);
}

void CGTColorDialogDlg::OnRadio6()
{
	COLORREF	old_color;

	old_color = m_wndBulleted.GetColor();
	// TODO: Add your control notification handler code here
	m_wndBulleted.SetDataValue (modes_hsv_value | modes_reverse);
	m_wndSlider.SetDataValue (modes_hsv_value | modes_reverse);
	m_wndBulleted.SetColor (old_color);
}

COLORREF CGTColorDialogDlg::GetColor()
{
	return m_wndWell.m_crColor;
}

void CGTColorDialogDlg::SetColor (COLORREF crColor)
{
	m_crColor = crColor;
	m_wndBulleted.SetColor (crColor);
}

void CGTColorDialogDlg::OnOK()
{
	COLORREF crColor = GetColor();
	CString strMsg;
	strMsg.Format ("(R-%u,G-%u,B-%u)", GetRValue (crColor), GetGValue (crColor), GetBValue (crColor));
	AfxMessageBox (strMsg);

	CDialog::OnOK();
}

void CGTColorDialogDlg::ColorChanged()
{
	COLORREF crColor = GetColor();
	CString sMsg;

	if (!m_bChangingRGB)
	{
		m_bChangingRGB = TRUE;

		sMsg.Format ("%d", GetRValue (crColor));
		m_edt_RGB_R.SetWindowText (sMsg);

		sMsg.Format ("%d", GetGValue (crColor));
		m_edt_RGB_G.SetWindowText (sMsg);

		sMsg.Format ("%d", GetBValue (crColor));
		m_edt_RGB_B.SetWindowText (sMsg);

		m_bChangingRGB = FALSE;
	}

	double	hue, sat, val;

	GTDrawHelper::RGB2HSV (
		GetRValue (crColor) / (double) max_rgb_red,
		GetGValue (crColor) / (double) max_rgb_green,
		GetBValue (crColor) / (double) max_rgb_blue,
		&hue, &sat, &val);

	if (!m_bChangingHSV)
	{
		m_bChangingHSV = TRUE;
		sMsg.Format ("%f", hue);
		m_edt_HSV_H.SetWindowText (sMsg);

		sMsg.Format ("%d", int (sat * 100));
		m_edt_HSV_S.SetWindowText (sMsg);

		sMsg.Format ("%d", int (val * 100));
		m_edt_HSV_V.SetWindowText (sMsg);
		m_bChangingHSV = FALSE;
	}

	if (!m_bChangingHEX)
	{
		m_bChangingHEX = TRUE;
		m_edtHex.SetWindowText (ColorToString (crColor));
		m_bChangingHEX = FALSE;
	}

	m_pObserver->SendColor (crColor);
}

void CGTColorDialogDlg::SetObserver (IObserver * pObserver)
{
	m_pObserver = pObserver;
}

CString	CGTColorDialogDlg::ColorToString (COLORREF cr)
{
	CString sHtml;

	int Rc = GetRValue (cr);
	int Gc = GetGValue (cr);
	int Bc = GetBValue (cr);

	sHtml.Format ("#%.2x%.2x%.2x", Rc, Gc, Bc);
	sHtml.MakeUpper();

	return sHtml;
}

void CGTColorDialogDlg::GetHSV (double& hue, double& sat, double& val)
{
	CString sVal;

	m_edt_HSV_H.GetWindowText (sVal);
	hue = atof ((LPCTSTR)sVal);

	if (hue < 0)
	{
		hue = 0;
	}

	if (hue > 359)
	{
		hue = 359;
	}

	m_edt_HSV_S.GetWindowText (sVal);
	sat = atof ((LPCTSTR)sVal) / 100.0f;

	if (sat < 0)
	{
		sat = 0;
	}

	if (sat > 1)
	{
		sat = 1;
	}

	m_edt_HSV_V.GetWindowText (sVal);
	val = atof ((LPCTSTR)sVal) / 100.0f;

	if (val < 0)
	{
		val = 0;
	}

	if (val > 1)
	{
		val = 1;
	}
}

void CGTColorDialogDlg::HandleHSVEdit()
{
	// we may be doing it ourselves..
	if (m_bChangingHSV)
	{
		return;
	}

	double	hue, sat, val;
	GetHSV (hue, sat, val);
	COLORREF crColor = GTDrawHelper::HSV2RGB (hue, sat, val);

	m_bChangingHSV = TRUE;

	m_bChangingHSV = TRUE;
	m_wndBulleted.SetColor (crColor);
	m_bChangingHSV = FALSE;

}
void CGTColorDialogDlg::OnChangeEdtHsvH()
{
	HandleHSVEdit();
}

void CGTColorDialogDlg::OnChangeEdtHsvS()
{
	HandleHSVEdit();
}

void CGTColorDialogDlg::OnChangeEdtHsvV()
{
	HandleHSVEdit();
}

void CGTColorDialogDlg::HandleRGBEdit()
{
	// we may be doing it ourselves..
	if (m_bChangingRGB)
	{
		return;
	}

	CString sVal;
	m_edt_RGB_R.GetWindowText (sVal);
	int nR = atoi ((LPCTSTR)sVal);

	m_edt_RGB_G.GetWindowText (sVal);
	int nG = atoi ((LPCTSTR)sVal);

	m_edt_RGB_B.GetWindowText (sVal);
	int nB = atoi ((LPCTSTR)sVal);

	if (nR > 255)
	{
		nR = 255;
	}
	if (nR < 0)
	{
		nR = 0;
	}

	if (nG > 255)
	{
		nG = 255;
	}
	if (nG < 0)
	{
		nG = 0;
	}

	if (nB > 255)
	{
		nB = 255;
	}
	if (nB < 0)
	{
		nB = 0;
	}

	COLORREF crColor = RGB (nR, nG, nB);
	m_bChangingRGB = TRUE;
	m_wndBulleted.SetColor (crColor);
	m_bChangingRGB = FALSE;
}
void CGTColorDialogDlg::OnChangeEdtRgbB()
{
	HandleRGBEdit();
}

void CGTColorDialogDlg::OnChangeEdtRgbG()
{
	HandleRGBEdit();
}

void CGTColorDialogDlg::OnChangeEdtRgbR()
{
	HandleRGBEdit();
}

void CGTColorDialogDlg::OnChangeEdithex()
{
	if (m_bChangingHEX)
	{
		return;
	}

	CString sVal;
	m_edtHex.GetWindowText (sVal);

	COLORREF crColor = html2rgb ((LPSTR) (LPCTSTR)sVal);
	m_bChangingHEX = TRUE;
	m_wndBulleted.SetColor (crColor);
	m_bChangingHEX = FALSE;
}

int	CGTColorDialogDlg::hex (char* szhex)
{
	return (szhex[1] >= 'A' ? szhex[1] - 'A' + 10 : szhex[1] - '0') +
		   (szhex[0] >= 'A' ? szhex[0] - 'A' + 10 : szhex[0] - '0') * 16;
}

COLORREF CGTColorDialogDlg::html2rgb (char* szHTMLColorValue)
{
	// HTML color values are encoded #RRGGBB
	if (szHTMLColorValue[0] != '#')
	{
		return (COLORREF)0;
	}

	if (lstrlen (szHTMLColorValue) != 7)
	{
		return (COLORREF)0;
	}

	// Make upper case
	CharUpperBuff (szHTMLColorValue, 7);

	int red = hex (szHTMLColorValue + 1);
	int green = hex (szHTMLColorValue + 3);
	int blue = hex (szHTMLColorValue + 5);

	return RGB (red, green, blue);
}
