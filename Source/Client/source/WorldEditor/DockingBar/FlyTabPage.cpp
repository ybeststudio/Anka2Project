// FlyTabPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "FlyTabPage.h"
#include "../MainFrm.h"
#include "../../GameLib/FlyingData.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

CFlyTabPage* CFlyTabPage::ms_pThis = 0;
/////////////////////////////////////////////////////////////////////////////
// CFlyTabPage dialog

void CFlyTabPage::CallBack()
{
	if (ms_pThis->m_ctrlList.GetCurSel() == -1)
	{
		return;
	}
	int iIndex = ms_pThis->m_ctrlList.GetCurSel();
	COLORREF Color;
	ms_pThis->m_ctrlTailColor.GetColor (&Color);

	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();
	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (iIndex);

	rfad.dwTailColor &= 0xff000000;
	rfad.dwTailColor |= GetRValue (Color) << 16;
	rfad.dwTailColor |= GetGValue (Color) << 8;
	rfad.dwTailColor |= GetBValue (Color);

	ms_pThis->Invalidate (FALSE);
}

CFlyTabPage::CFlyTabPage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CFlyTabPage::IDD, pParent)
{
	m_iSelectIndex = -1;
	ms_pThis = this;
	//{{AFX_DATA_INIT(CFlyTabPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFlyTabPage::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CFlyTabPage)
	DDX_Control (pDX, IDC_FLY_TAB_TAIL_ALPHA, m_ctrlTailAlpha);
	DDX_Control (pDX, IDC_FLY_TAB_TAIL_COLOR, m_ctrlTailColor);
	DDX_Control (pDX, IDC_FLY_LIST, m_ctrlList);
	//}}AFX_DATA_MAP
}

void CFlyTabPage::RebuildAttachData()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	m_ctrlList.ResetContent();

	for (int i = 0; i < pData->GetAttachDataCount(); i++)
	{
		char szBuffer[1024];
		sprintf (szBuffer, "%d : Effect", i);
		m_ctrlList.InsertString (i, szBuffer);
		m_ctrlList.SelectString (i, szBuffer);
	}

	m_iSelectIndex = -1;
	SelectAttachIndex (m_iSelectIndex);
	m_ctrlList.SetCurSel (-1);

}

void CFlyTabPage::UpdateUI()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_ACCEL_GRAVITY, pData->m_v3Accel.z);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_ACCEL_FRONT, -pData->m_v3Accel.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_ACCEL_SIDE, pData->m_v3Accel.x);

	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_ANGLE_GRAVITY, pData->m_v3AngVel.z);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_ANGLE_FRONT, pData->m_v3AngVel.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_ANGLE_SIDE, pData->m_v3AngVel.x);

	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_INIT_VEL, pData->m_fInitVel);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_CONE_ANGLE, pData->m_fConeAngle);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_ROLL_ANGLE, pData->m_fRollAngle);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_RANGE, pData->m_fRange);

	CheckDlgButton (IDC_FLY_TAB_HIT_ON_BACKGROUND, pData->m_bHitOnBackground);
	CheckDlgButton (IDC_FLY_TAB_HIT_ON_ANOTHER_MONSTER, pData->m_bHitOnAnotherMonster);
	SetDialogIntegerText (GetSafeHwnd(), IDC_FLY_TAB_PIERCE_COUNT, pData->m_iPierceCount);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_TAB_COLLISION_SPHERE_RADIUS, pData->m_fCollisionSphereRadius);

	CheckDlgButton (IDC_FLY_HOMING_FLAG, pData->m_bIsHoming);
	CheckDlgButton (IDC_FLY_TAB_SPREADING_CHECK, pData->m_bSpreading);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_HOMING_MAX_ANGLE, pData->m_fHomingMaxAngle);
	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_HOMING_START_TIME, pData->m_fHomingStartTime);

	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_BOMB_RANGE, pData->m_fBombRange);
	SetDlgItemText (IDC_FLY_BOMB_EFFECT, pData->m_strBombEffectName.c_str());

	SetDialogFloatText (GetSafeHwnd(), IDC_FLY_GRAVITY, pData->m_fGravity);

	RebuildAttachData();

	SelectAttachIndex (m_iSelectIndex);
}

void CFlyTabPage::Initialize()
{
}

BEGIN_MESSAGE_MAP (CFlyTabPage, CPageCtrl)
	//{{AFX_MSG_MAP(CFlyTabPage)
	ON_BN_CLICKED (IDC_FLY_HOMING_FLAG, OnFlyHoming)
	ON_BN_CLICKED (IDC_FLY_TAB_SPREADING_CHECK, OnFlySpreading)
	ON_BN_CLICKED (IDC_FLY_TAB_NEW_EFFECT, OnFlyTabNewEffect)
	ON_BN_CLICKED (IDC_FLY_TAB_NEW_OBJECT, OnFlyTabNewObject)
	ON_BN_CLICKED (IDC_FLY_LOAD_BOMB_EFFECT, OnFlyLoadBombEffect)
	ON_BN_CLICKED (IDC_FLY_TYPE_LINE, OnFlyTypeLine)
	ON_BN_CLICKED (IDC_FLY_TYPE_MULTI_LINE, OnFlyTypeMultiLine)
	ON_BN_CLICKED (IDC_FLY_TYPE_SINE, OnFlyTypeSine)
	ON_BN_CLICKED (IDC_FLY_TYPE_EXP, OnFlyTypeExp)
	ON_BN_CLICKED (IDC_FLY_CLEAR_ONE, OnFlyClearOne)
	ON_BN_CLICKED (IDC_FLY_CLEAR_ALL, OnFlyClearAll)
	ON_BN_CLICKED (IDC_FLY_LOAD_ATTACH_FILE, OnFlyLoadAttachFile)
	ON_BN_CLICKED (IDC_FLY_DUPLICATE, OnFlyDuplicate)
	ON_BN_CLICKED (IDC_FLY_TAB_HAS_TAIL, OnFlyTabHasTail)
	ON_BN_CLICKED (IDC_FLY_TAB_TAIL_TRI_RADIO, OnFlyTabTailTriRadio)
	ON_BN_CLICKED (IDC_FLY_TAB_TAIL_RECT_RADIO, OnFlyTabTailRectRadio)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_FLY_SAVE_SCRIPT, OnFlySaveScript)
	ON_BN_CLICKED (IDC_FLY_LOAD_SCRIPT, OnFlyLoadScript)
	ON_BN_CLICKED (IDC_FLY_TAB_HIT_ON_ANOTHER_MONSTER, OnFlyTabHitOnAnotherMonster)
	ON_BN_CLICKED (IDC_FLY_TAB_HIT_ON_BACKGROUND, OnFlyTabHitOnBackground)
	ON_BN_CLICKED (IDC_FLY_TAB_MAINTAIN_PARALLEL, OnMaintainParallel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlyTabPage message handlers

BOOL CFlyTabPage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CFlyTabPage::IDD, pParent))
	{
		return FALSE;
	}
	if (!m_ctrlTailColor.Create (this))
	{
		return FALSE;
	}

	m_ctrlTailColor.pfnCallBack = CallBack;

	CreateBitmapButton ((CButton*)GetDlgItem (IDC_FLY_TAB_NEW_EFFECT), IDB_FLY_EFFECT, m_BitmapEffect);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_FLY_TAB_NEW_OBJECT), IDB_FLY_OBJECT, m_BitmapObject);

	CreateBitmapButton ((CButton*)GetDlgItem (IDC_FLY_TYPE_LINE), IDB_FLY_TYPE_LINE, m_BitmapTypeLine);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_FLY_TYPE_MULTI_LINE), IDB_FLY_TYPE_MULTI_LINE, m_BitmapTypeMultiLine);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_FLY_TYPE_SINE), IDB_FLY_TYPE_SINE, m_BitmapTypeSine);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_FLY_TYPE_EXP), IDB_FLY_TYPE_EXP, m_BitmapTypeExp);

	return TRUE;
}

BOOL CFlyTabPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	if (pApp)
	{
		CSceneFly * pFly = pApp->GetSceneFly();
		if (pFly)
		{
			CFlyingData * pData = pFly->GetFlyingDataPointer();
			switch (LOWORD (wParam))
			{
				case IDC_FLY_LIST:
					m_iSelectIndex = m_ctrlList.GetCurSel();
					SelectAttachIndex (m_iSelectIndex);
					break;
				case IDC_FLY_RANGE:
					pData->m_fRange = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_RANGE);
					break;
				case IDC_FLY_INIT_VEL:
					pData->m_fInitVel = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_INIT_VEL);
					break;
				case IDC_FLY_CONE_ANGLE:
					pData->m_fConeAngle = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_CONE_ANGLE);
					break;
				case IDC_FLY_ROLL_ANGLE:
					pData->m_fRollAngle = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_ROLL_ANGLE);
					break;
				case IDC_FLY_GRAVITY:
					pData->m_fGravity = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_GRAVITY);
					break;
				case IDC_FLY_ANGLE_GRAVITY:
					pData->m_v3AngVel.z = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_ANGLE_GRAVITY);
					break;
				case IDC_FLY_ANGLE_FRONT:
					pData->m_v3AngVel.y = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_ANGLE_FRONT);
					break;
				case IDC_FLY_ANGLE_SIDE:
					pData->m_v3AngVel.x = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_ANGLE_SIDE);
					break;
				case IDC_FLY_TAB_PIERCE_COUNT:
					pData->m_iPierceCount = GetDialogIntegerText (GetSafeHwnd(), IDC_FLY_TAB_PIERCE_COUNT);
					break;
				case IDC_FLY_TAB_COLLISION_SPHERE_RADIUS:
					pData->m_fCollisionSphereRadius = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_TAB_COLLISION_SPHERE_RADIUS);
					break;
				case IDC_FLY_ACCEL_GRAVITY:
					pData->m_v3Accel.z = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_ACCEL_GRAVITY);
					break;
				case IDC_FLY_ACCEL_FRONT:
					pData->m_v3Accel.y = -GetDialogFloatText (GetSafeHwnd(), IDC_FLY_ACCEL_FRONT);
					break;
				case IDC_FLY_ACCEL_SIDE:
					pData->m_v3Accel.x = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_ACCEL_SIDE);
					break;
				case IDC_FLY_HOMING_MAX_ANGLE:
					pData->m_fHomingMaxAngle = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_HOMING_MAX_ANGLE);
					break;
				case IDC_FLY_HOMING_START_TIME:
					pData->m_fHomingStartTime = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_HOMING_START_TIME);
					break;
				case IDC_FLY_BOMB_RANGE:
					pData->m_fBombRange = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_BOMB_RANGE);
					break;
				case IDC_FLY_BOMB_EFFECT:
				{
					char szBuffer[256];
					GetDlgItemText (IDC_FLY_BOMB_EFFECT, szBuffer, 256);
					pData->SetBombEffect (szBuffer);
				}
				break;

				case IDC_FLY_ATTACH_FILE:
				{
					char szBuffer[256];
					GetDlgItemText (IDC_FLY_ATTACH_FILE, szBuffer, 256);
					CFlyingData::TFlyingAttachData & rfad =  pData->GetAttachDataReference (m_iSelectIndex);
					rfad.strFilename = szBuffer;
				}
				break;

				case IDC_FLY_TAB_TAIL_LENGTH:
				{
					CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
					rfad.fTailLength = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_TAB_TAIL_LENGTH);
				}
				break;

				case IDC_FLY_TAB_TAIL_SIZE:
				{
					CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
					rfad.fTailSize = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_TAB_TAIL_SIZE);
				}
				break;

				case IDC_FLY_VALUE1:
				{
					CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
					if (rfad.iFlyType == CFlyingData::FLY_ATTACH_TYPE_SINE || rfad.iFlyType == CFlyingData::FLY_ATTACH_TYPE_EXP || rfad.iFlyType == CFlyingData::FLY_ATTACH_TYPE_MULTI_LINE)
					{
						rfad.fRoll = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE1);
					}
				}
				break;
				case IDC_FLY_VALUE2:
				{
					CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
					if (rfad.iFlyType == CFlyingData::FLY_ATTACH_TYPE_SINE || rfad.iFlyType == CFlyingData::FLY_ATTACH_TYPE_EXP)
					{
						rfad.fPeriod = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE2);
					}
					else if (rfad.iFlyType == CFlyingData::FLY_ATTACH_TYPE_MULTI_LINE)
					{
						rfad.fDistance = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE2);
					}
				}
				break;
				case IDC_FLY_VALUE3:
				{
					CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
					if (rfad.iFlyType == CFlyingData::FLY_ATTACH_TYPE_SINE || rfad.iFlyType == CFlyingData::FLY_ATTACH_TYPE_EXP)
					{
						rfad.fAmplitude = GetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE3);
					}
				}
				break;

			}
		}
	}
	return CPageCtrl::OnCommand (wParam, lParam);
}

void CFlyTabPage::OnFlySpreading()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	pData->m_bSpreading = IsDlgButtonChecked (IDC_FLY_TAB_SPREADING_CHECK) ? true : false;
}

void CFlyTabPage::OnMaintainParallel()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	pData->m_bMaintainParallel = IsDlgButtonChecked (IDC_FLY_TAB_MAINTAIN_PARALLEL) ? true : false;
}

void CFlyTabPage::OnFlyHoming()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	pData->m_bIsHoming = IsDlgButtonChecked (IDC_FLY_HOMING_FLAG) ? true : false;
}

void CFlyTabPage::SelectAttachIndex (int iIndex)
{
	// Setting UI

	if (iIndex == -1)
	{
		// deselect
		GetDlgItem (IDC_FLY_ADDITIONAL)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TYPE_LINE)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TYPE_MULTI_LINE)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TYPE_SINE)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TYPE_EXP)->ShowWindow (SW_HIDE);

		GetDlgItem (IDC_FLY_ARG1)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_ARG2)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_ARG3)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_VALUE1)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_VALUE2)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_VALUE3)->ShowWindow (SW_HIDE);

		GetDlgItem (IDC_FLY_ATTACH_FILE)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_LOAD_ATTACH_FILE)->ShowWindow (SW_HIDE);

		GetDlgItem (IDC_FLY_TAB_TAIL_TRI_RADIO)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TAB_TAIL_RECT_RADIO)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TAB_TAIL_ALPHA)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TAB_TAIL_COLOR)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TAB_TAIL_LENGTH)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TAB_TAIL_SIZE)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_STATIC_FLY1)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_STATIC_FLY2)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_STATIC_FLY3)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_STATIC_FLY4)->ShowWindow (SW_HIDE);
		GetDlgItem (IDC_FLY_TAB_HAS_TAIL)->ShowWindow (SW_HIDE);

	}
	else
	{
		GetDlgItem (IDC_FLY_ADDITIONAL)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TYPE_LINE)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TYPE_MULTI_LINE)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TYPE_SINE)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TYPE_EXP)->ShowWindow (SW_SHOW);

		GetDlgItem (IDC_FLY_ATTACH_FILE)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_LOAD_ATTACH_FILE)->ShowWindow (SW_SHOW);

		GetDlgItem (IDC_FLY_TAB_TAIL_TRI_RADIO)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TAB_TAIL_RECT_RADIO)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TAB_TAIL_ALPHA)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TAB_TAIL_COLOR)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TAB_TAIL_LENGTH)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TAB_TAIL_SIZE)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_FLY_TAB_HAS_TAIL)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_STATIC_FLY1)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_STATIC_FLY2)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_STATIC_FLY3)->ShowWindow (SW_SHOW);
		GetDlgItem (IDC_STATIC_FLY4)->ShowWindow (SW_SHOW);


		CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
		CSceneFly * pFly = pApp->GetSceneFly();
		CFlyingData * pData = pFly->GetFlyingDataPointer();

		CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (iIndex);


		SetDlgItemText (IDC_FLY_ATTACH_FILE, rfad.strFilename.c_str());

		assert (rfad.iType == CFlyingData::FLY_ATTACH_EFFECT);

		CheckDlgButton (IDC_FLY_TYPE_LINE, FALSE);
		CheckDlgButton (IDC_FLY_TYPE_MULTI_LINE, FALSE);
		CheckDlgButton (IDC_FLY_TYPE_SINE, FALSE);
		CheckDlgButton (IDC_FLY_TYPE_EXP, FALSE);

		TailUIEnable (rfad.bHasTail);
		SetDialogFloatText (GetSafeHwnd(), IDC_FLY_TAB_TAIL_SIZE, rfad.fTailSize);
		SetDialogFloatText (GetSafeHwnd(), IDC_FLY_TAB_TAIL_LENGTH, rfad.fTailLength);
		m_ctrlTailAlpha.SetPos ((rfad.dwTailColor >> 24) * 100 / 255);
		m_ctrlTailColor.SetColor (RGB ((BYTE) (rfad.dwTailColor >> 16), (BYTE) (rfad.dwTailColor >> 8), (BYTE) (rfad.dwTailColor)));
		m_ctrlTailColor.Update();
		if (rfad.bRectShape)
		{
			CheckDlgButton (IDC_FLY_TAB_TAIL_TRI_RADIO, FALSE);
			CheckDlgButton (IDC_FLY_TAB_TAIL_RECT_RADIO, TRUE);
		}
		else
		{
			CheckDlgButton (IDC_FLY_TAB_TAIL_TRI_RADIO, TRUE);
			CheckDlgButton (IDC_FLY_TAB_TAIL_RECT_RADIO, FALSE);
		}

		switch (rfad.iFlyType)
		{
			case CFlyingData::FLY_ATTACH_TYPE_LINE:
				CheckDlgButton (IDC_FLY_TYPE_LINE, TRUE);
				GetDlgItem (IDC_FLY_ARG1)->ShowWindow (SW_HIDE);
				GetDlgItem (IDC_FLY_ARG2)->ShowWindow (SW_HIDE);
				GetDlgItem (IDC_FLY_ARG3)->ShowWindow (SW_HIDE);
				GetDlgItem (IDC_FLY_VALUE1)->ShowWindow (SW_HIDE);
				GetDlgItem (IDC_FLY_VALUE2)->ShowWindow (SW_HIDE);
				GetDlgItem (IDC_FLY_VALUE3)->ShowWindow (SW_HIDE);
				break;
			case CFlyingData::FLY_ATTACH_TYPE_MULTI_LINE:
				CheckDlgButton (IDC_FLY_TYPE_MULTI_LINE, TRUE);

				GetDlgItem (IDC_FLY_ARG1)->SetWindowText ("Axis Rotation");
				GetDlgItem (IDC_FLY_ARG2)->SetWindowText ("Distance");
				SetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE1, rfad.fRoll);
				SetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE2, rfad.fDistance);

				GetDlgItem (IDC_FLY_ARG1)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_ARG2)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_ARG3)->ShowWindow (SW_HIDE);
				GetDlgItem (IDC_FLY_VALUE1)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_VALUE2)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_VALUE3)->ShowWindow (SW_HIDE);
				break;
			case CFlyingData::FLY_ATTACH_TYPE_SINE:
				CheckDlgButton (IDC_FLY_TYPE_SINE, TRUE);

				GetDlgItem (IDC_FLY_ARG1)->SetWindowText ("Axis Rotation");
				GetDlgItem (IDC_FLY_ARG2)->SetWindowText ("Cycle");
				GetDlgItem (IDC_FLY_ARG3)->SetWindowText ("Amplitude");
				SetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE1, rfad.fRoll);
				SetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE2, rfad.fPeriod);
				SetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE3, rfad.fAmplitude);

				GetDlgItem (IDC_FLY_ARG1)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_ARG2)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_ARG3)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_VALUE1)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_VALUE2)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_VALUE3)->ShowWindow (SW_SHOW);
				break;
			case CFlyingData::FLY_ATTACH_TYPE_EXP:
				CheckDlgButton (IDC_FLY_TYPE_EXP, TRUE);

				GetDlgItem (IDC_FLY_ARG1)->SetWindowText ("Axis Rotation");
				GetDlgItem (IDC_FLY_ARG2)->SetWindowText ("Cycle");
				GetDlgItem (IDC_FLY_ARG3)->SetWindowText ("Amplitude");
				SetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE1, rfad.fRoll);
				SetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE2, rfad.fPeriod);
				SetDialogFloatText (GetSafeHwnd(), IDC_FLY_VALUE3, rfad.fAmplitude);

				GetDlgItem (IDC_FLY_ARG1)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_ARG2)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_ARG3)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_VALUE1)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_VALUE2)->ShowWindow (SW_SHOW);
				GetDlgItem (IDC_FLY_VALUE3)->ShowWindow (SW_SHOW);
				break;
			default:
				// do not happen!
				assert (!"CFlyTabPage::SelectAttachIndex NOT REACHED");
				break;
		}


	}
}

void CFlyTabPage::OnFlyTabNewEffect()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();


	int idx = (pData->AttachFlyEffect (CFlyingData::FLY_ATTACH_TYPE_LINE, "", 0.0f, 0.0f, 0.0f));
	char szBuffer[1024];
	sprintf (szBuffer, "%d : Effect", idx);
	m_ctrlList.InsertString (idx, szBuffer);
	m_ctrlList.SelectString (idx, szBuffer);

	m_iSelectIndex = idx;
	SelectAttachIndex (idx);

	CheckDlgButton (IDC_FLY_TAB_NEW_EFFECT, FALSE);
}

void CFlyTabPage::OnFlyTabNewObject()
{
	CheckDlgButton (IDC_FLY_TAB_NEW_OBJECT, FALSE);
}

void CFlyTabPage::OnFlyLoadBombEffect()
{
	DWORD dwFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	const char* c_szFilter = "Metin 2 Effect Script (*.mse)|*.mse|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		pApplication->
		GetSceneFly()->
		GetFlyingDataPointer()->
		SetBombEffect (FileOpener.GetPathName());
		UpdateUI();
	}
}

void CFlyTabPage::OnFlyTypeLine()
{
	CheckDlgButton (IDC_FLY_TYPE_LINE, TRUE);
	CheckDlgButton (IDC_FLY_TYPE_MULTI_LINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_SINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_EXP, FALSE);

	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
	rfad.iFlyType = CFlyingData::FLY_ATTACH_TYPE_LINE;

	SelectAttachIndex (m_iSelectIndex);
}

void CFlyTabPage::OnFlyTypeMultiLine()
{
	CheckDlgButton (IDC_FLY_TYPE_LINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_MULTI_LINE, TRUE);
	CheckDlgButton (IDC_FLY_TYPE_SINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_EXP, FALSE);

	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
	rfad.iFlyType = CFlyingData::FLY_ATTACH_TYPE_MULTI_LINE;

	SelectAttachIndex (m_iSelectIndex);
}

void CFlyTabPage::OnFlyTypeSine()
{
	CheckDlgButton (IDC_FLY_TYPE_LINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_MULTI_LINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_SINE, TRUE);
	CheckDlgButton (IDC_FLY_TYPE_EXP, FALSE);

	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
	rfad.iFlyType = CFlyingData::FLY_ATTACH_TYPE_SINE;

	SelectAttachIndex (m_iSelectIndex);
}

void CFlyTabPage::OnFlyTypeExp()
{
	CheckDlgButton (IDC_FLY_TYPE_LINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_MULTI_LINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_SINE, FALSE);
	CheckDlgButton (IDC_FLY_TYPE_EXP, TRUE);

	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);
	rfad.iFlyType = CFlyingData::FLY_ATTACH_TYPE_EXP;

	SelectAttachIndex (m_iSelectIndex);
}

void CFlyTabPage::OnFlyClearOne()
{
	int iIndex = m_ctrlList.GetCurSel();
	if (iIndex == -1)
	{
		return;
	}

	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	pData->RemoveAttach (iIndex);

	RebuildAttachData();
}

void CFlyTabPage::OnFlyClearAll()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	pData->RemoveAllAttach();

	RebuildAttachData();
}

void CFlyTabPage::OnFlyLoadAttachFile()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();
	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_iSelectIndex);

	DWORD dwFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	const char* c_szFilter;

	switch (rfad.iType)
	{
		case CFlyingData::FLY_ATTACH_EFFECT:
			c_szFilter = "Metin 2 Effect Script (*.mse)|*.mse|All Files (*.*)|*.*|";
			break;
		case CFlyingData::FLY_ATTACH_OBJECT:
			c_szFilter = "Metin 2 Item Script (*.msm)|*.msm|All Files (*.*)|*.*|";
			break;
	}

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		rfad.strFilename = FileOpener.GetPathName();
		StringPath (rfad.strFilename);

		CEffectManager::Instance().RegisterEffect (rfad.strFilename.c_str(), true);

		SetDlgItemText (IDC_FLY_ATTACH_FILE, rfad.strFilename.c_str());
	}
}

void CFlyTabPage::OnFlyDuplicate()
{
	if (m_ctrlList.GetCurSel() == -1)
	{
		return;
	}

	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	int iIndex = m_ctrlList.GetCurSel();
	pData->DuplicateAttach (iIndex);
	RebuildAttachData();
}

void CFlyTabPage::OnFlyTabHasTail()
{
	if (m_ctrlList.GetCurSel() == -1)
	{
		return;
	}

	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	int iIndex = m_ctrlList.GetCurSel();
	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (iIndex);
	rfad.bHasTail = !rfad.bHasTail;

	TailUIEnable (rfad.bHasTail);
}

void CFlyTabPage::TailUIEnable (bool bEnable)
{
	if (m_ctrlList.GetCurSel() == -1)
	{
		return;
	}

	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	int iIndex = m_ctrlList.GetCurSel();
	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (iIndex);

	//m_ctrlTailColor.SetColor(rfad.dwTailColor);
	m_ctrlTailColor.SetColor (RGB ((BYTE) (rfad.dwTailColor >> 16), (BYTE) (rfad.dwTailColor >> 8), (BYTE) (rfad.dwTailColor)));
	m_ctrlTailColor.Update();

	BOOL Enable = bEnable ? TRUE : FALSE;
	CheckDlgButton (IDC_FLY_TAB_HAS_TAIL, Enable);
	GetDlgItem (IDC_FLY_TAB_TAIL_TRI_RADIO)->EnableWindow (Enable);
	GetDlgItem (IDC_FLY_TAB_TAIL_RECT_RADIO)->EnableWindow (Enable);
	GetDlgItem (IDC_FLY_TAB_TAIL_ALPHA)->EnableWindow (Enable);
	GetDlgItem (IDC_FLY_TAB_TAIL_COLOR)->EnableWindow (Enable);
	GetDlgItem (IDC_FLY_TAB_TAIL_LENGTH)->EnableWindow (Enable);
	GetDlgItem (IDC_FLY_TAB_TAIL_SIZE)->EnableWindow (Enable);
	GetDlgItem (IDC_STATIC_FLY1)->EnableWindow (Enable);
	GetDlgItem (IDC_STATIC_FLY2)->EnableWindow (Enable);
	GetDlgItem (IDC_STATIC_FLY3)->EnableWindow (Enable);
}

void CFlyTabPage::OnFlyTabTailTriRadio()
{
	if (m_ctrlList.GetCurSel() == -1)
	{
		return;
	}

	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	int iIndex = m_ctrlList.GetCurSel();
	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (iIndex);

	rfad.bRectShape = false;
}

void CFlyTabPage::OnFlyTabTailRectRadio()
{
	if (m_ctrlList.GetCurSel() == -1)
	{
		return;
	}

	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	int iIndex = m_ctrlList.GetCurSel();
	CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (iIndex);

	rfad.bRectShape = true;
}

void CFlyTabPage::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_ctrlList.GetCurSel() != -1)
		if (IDC_FLY_TAB_TAIL_ALPHA == m_iSliderIndex)
		{
			CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
			CSceneFly * pFly = pApp->GetSceneFly();
			CFlyingData * pData = pFly->GetFlyingDataPointer();
			CFlyingData::TFlyingAttachData & rfad = pData->GetAttachDataReference (m_ctrlList.GetCurSel());
			rfad.dwTailColor &= 0x00ffffff;
			rfad.dwTailColor |= (BYTE) (m_ctrlTailAlpha.GetPos() * 255 / 100) << 24;

			Invalidate (FALSE);
		}

	CPageCtrl::OnHScroll (nSBCode, nPos, pScrollBar);
}


BOOL CFlyTabPage::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	m_iSliderIndex = wParam;

	return CPageCtrl::OnNotify (wParam, lParam, pResult);
}

void CFlyTabPage::OnFlySaveScript()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	const char* c_szFilter = "Metin2 Fly Script Files (*.msf)|*.msf|All Files (*.*)|*.*|";

	CFileDialog FileOpener (FALSE, "Save", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
		CSceneFly * pFly = pApp->GetSceneFly();
		CFlyingData * pData = pFly->GetFlyingDataPointer();

		pData->SaveScriptFile (FileOpener.GetPathName());
	}
}

void CFlyTabPage::OnFlyLoadScript()
{
	DWORD dwFlag = OFN_HIDEREADONLY;
	const char* c_szFilter = "Metin2 Fly Script Files (*.msf)|*.msf|All Files (*.*)|*.*|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
		CSceneFly * pFly = pApp->GetSceneFly();
		CFlyingData * pData = pFly->GetFlyingDataPointer();
		pFly->Stop();

		/////

		pData->LoadScriptFile (FileOpener.GetPathName());
		UpdateUI();
	}
}

void CFlyTabPage::OnFlyTabHitOnAnotherMonster()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	pData->m_bHitOnAnotherMonster = !pData->m_bHitOnAnotherMonster;
}

void CFlyTabPage::OnFlyTabHitOnBackground()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*)AfxGetApp();
	CSceneFly * pFly = pApp->GetSceneFly();
	CFlyingData * pData = pFly->GetFlyingDataPointer();

	pData->m_bHitOnBackground = !pData->m_bHitOnBackground;
}
