// EffectTimeBar.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectTimeBar.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


class CStartTimeAccssor : public CGraphCtrl::IValueAccessor
{
	public:
		CStartTimeAccssor() {}
		~CStartTimeAccssor() {}

		CEffectElementBaseAccessor* m_pBase;

		void SetBasePointer (CEffectElementBaseAccessor * pBase)
		{
			m_pBase = pBase;
		}

		DWORD GetValueCount()
		{
			return 1;
		}

		void Insert (float fTime, DWORD dwValue)
		{
			//m_pBase->InsertVisible(fTime);
		}
		void Delete (DWORD dwIndex)
		{
			//m_pBase->DeleteVisible(dwIndex);
		}

		bool GetTime (DWORD dwIndex, float* pTime)
		{
			*pTime = m_pBase->GetStartTime();
			return true;
		}
		bool GetValue (DWORD dwIndex, DWORD * pValue)
		{
			*pValue = 100;
			return true;
		}

		void SetTime (DWORD dwIndex, float fTime)
		{
			m_pBase->SetStartTime (fTime);
		}
		void SetValue (DWORD dwIndex, DWORD dwValue)
		{
		}
};

class CPositionAccssor : public CGraphCtrl::IValueAccessor
{
	public:
		CPositionAccssor() {}
		~CPositionAccssor() {}

		CEffectElementBaseAccessor* m_pBase;

		void SetBasePointer (CEffectElementBaseAccessor * pBase)
		{
			m_pBase = pBase;
		}

		DWORD GetValueCount()
		{
			return m_pBase->GetPositionCount();
		}

		void Insert (float fTime, DWORD dwValue)
		{
			m_pBase->InsertPosition (fTime);

		}
		void Delete (DWORD dwIndex)
		{
			m_pBase->DeletePosition (dwIndex);
		}

		bool GetTime (DWORD dwIndex, float* pTime)
		{
			return m_pBase->GetTimePosition (dwIndex, pTime);
		}
		bool GetValue (DWORD dwIndex, DWORD * pValue)
		{
			//*pValue = 100;
			//return true;
			TEffectPosition * pEffectPosition;

			if (m_pBase->GetValuePosition (dwIndex, &pEffectPosition))
			{
				*pValue = pEffectPosition->m_iMovingType * -100 + 200;
				return true;
			}
			return false;
		}

		void SetTime (DWORD dwIndex, float fTime)
		{
			m_pBase->SetTimePosition (dwIndex, fTime);
		}
		void SetValue (DWORD dwIndex, DWORD dwValue)
		{
			TEffectPosition * pEffectPosition;

			if (m_pBase->GetValuePosition (dwIndex, &pEffectPosition))
			{
				if (dwValue < 150)
				{
					dwValue = 1;
				}
				else
				{
					dwValue = 0;
				}

				pEffectPosition->m_iMovingType = dwValue;
			}
		}
};

CPositionAccssor PositionAccessor;
CStartTimeAccssor StartTimeAccessor;

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeBar dialog

CEffectTimeBar::CEffectTimeBar (CWnd* pParent /*=NULL*/)
	: CDialog (CEffectTimeBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffectTimeBar)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffectTimeBar::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CEffectTimeBar)
	DDX_Control (pDX, IDC_EFFECT_TIME_BAR_GRAPH, m_EffectTimeGraph);
	DDX_Control (pDX, IDC_EFFECT_TIME_BAR_GRAPH_LIST, m_ctrlGraphList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CEffectTimeBar, CDialog)
	//{{AFX_MSG_MAP(CEffectTimeBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_PLAY, OnPlay)
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_ZOOM_IN_HORIZON, OnZoomInHorizon)
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_ZOOM_OUT_HORIZON, OnZoomOutHorizon)
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_PLAY_LOOP, OnPlayLoop)
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_MINIMIZE, OnMinimize)
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_MAXIMIZE, OnMaximize)
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_STOP, OnStop)
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_XYZ, OnEffectTimeBarXyz)
	ON_BN_CLICKED (IDC_EFFECT_TIME_BAR_XYYZZX, OnEffectTimeBarXyyzzx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeBar normal functions

void CEffectTimeBar::Initialize()
{
	m_EffectTimeGraph.SetGraphCtrl (&m_GraphCtrl);
	m_GraphCtrl.SetGraphType (CGraphCtrl::GRAPH_TYPE_BRIDGE);
	m_GraphCtrl.SetValueType (CGraphCtrl::VALUE_TYPE_CENTER);

	m_EffectTimeGraph.Initialize();
}

void CEffectTimeBar::SetData (DWORD dwSelectedIndex)
{
	m_dwSelectedIndex = dwSelectedIndex;

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (dwSelectedIndex, &pElement))
	{
		return;
	}

	int iCurSel = m_ctrlGraphList.GetCurSel();

	int iCount = m_ctrlGraphList.GetCount();
	for (int i = 0; i < iCount; ++i)
	{
		m_ctrlGraphList.DeleteString (0);
	}

	m_ctrlGraphList.InsertString (0, "StartTime");
	// FIXME not implemented! -- ipkn
	//m_ctrlGraphList.InsertString(2, "Visible");

	m_ctrlGraphList.InsertString (1, "Position");

	/*switch(pElement->iType)
	{
		case CEffectAccessor::EFFECT_ELEMENT_TYPE_PARTICLE:
			m_ctrlGraphList.InsertString(1, "Position");
			break;
		case CEffectAccessor::EFFECT_ELEMENT_TYPE_MESH:
			m_ctrlGraphList.InsertString(1, "Position");
			break;
		case CEffectAccessor::EFFECT_ELEMENT_TYPE_LIGHT:
			m_ctrlGraphList.InsertString(1, "Position");
			break;
	}*/

	m_ctrlGraphList.SelectString (0, "StartTime");
	if (iCurSel != -1 && iCurSel < m_ctrlGraphList.GetCount())
	{
		CString str;
		m_ctrlGraphList.GetLBText (iCurSel, str);
		m_ctrlGraphList.SelectString (iCurSel, str);
	}
	else
	{
		iCurSel = 0;
	}
	SelectGraph (dwSelectedIndex, iCurSel);

	// FIXME : 다음에 볼 때 알아 보기 힘든 코드
	// 현재 선택이 Position인 경우 timebar의 선택된 노드와 화면에 선택된 노드를 동일시 한다.
	if (iCurSel == 1)
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CSceneEffect* pSceneEffect = pApplication->GetSceneEffect();
		m_GraphCtrl.SetSelectedIndex (pSceneEffect->GetSelectedPositionIndex());
	}

	m_ctrlGraphList.Invalidate (TRUE);

	char szLifeTime[32 + 1];
	_snprintf (szLifeTime, 32, "%.2f", pEffectAccessor->GetLifeTime());
	SetDlgItemText (IDC_EFFECT_TIME_BAR_LIFE_TIME, szLifeTime);
}

void CEffectTimeBar::SelectGraph (DWORD dwSelectedIndex, DWORD dwGraphType)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (dwSelectedIndex, &pElement))
	{
		return;
	}

	switch (dwGraphType)
	{
		case 0:
			StartTimeAccessor.SetBasePointer (pElement->pBase);
			m_GraphCtrl.SetAccessorPointer (&StartTimeAccessor);
			m_GraphCtrl.SetGraphType (CGraphCtrl::GRAPH_TYPE_BOOLEAN);
			break;
		case 1:
			PositionAccessor.SetBasePointer (pElement->pBase);
			m_GraphCtrl.SetAccessorPointer (&PositionAccessor);
			m_GraphCtrl.SetGraphType (CGraphCtrl::GRAPH_TYPE_BLOCK);
			break;
			/*case 2:
				VisibleAccessor.SetBasePointer(pElement->pBase);
				m_GraphCtrl.SetAccessorPointer(&VisibleAccessor);
				break;*/
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeBar message handlers

void CEffectTimeBar::CreateBitmapButton (int iButtonID, int iBitmapID, CBitmap & rBitmap)
{
	CButton * pButton;
	pButton = (CButton*)GetDlgItem (iButtonID);
	rBitmap.DeleteObject();
	rBitmap.LoadBitmap (iBitmapID);
	pButton->SetBitmap (rBitmap);
}

int CEffectTimeBar::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate (lpCreateStruct) == -1)
	{
		return -1;
	}

	return 0;
}

void CEffectTimeBar::CreateButtons()
{
	CreateBitmapButton (IDC_EFFECT_TIME_BAR_PLAY, IDB_EFFECT_PLAY, m_BitmapPlay);
	CreateBitmapButton (IDC_EFFECT_TIME_BAR_PLAY_LOOP, IDB_EFFECT_PLAY_LOOP, m_BitmapPlayLoop);
	CreateBitmapButton (IDC_EFFECT_TIME_BAR_STOP, IDB_EFFECT_STOP, m_BitmapStop);

	CreateBitmapButton (IDC_EFFECT_TIME_BAR_ZOOM_IN, IDB_EFFECT_ZOOM_IN_HORIZON, m_BitmapZoomIn);
	CreateBitmapButton (IDC_EFFECT_TIME_BAR_ZOOM_OUT, IDB_EFFECT_ZOOM_OUT_HORIZON, m_BitmapZoomOut);
	CreateBitmapButton (IDC_EFFECT_TIME_BAR_MINIMIZE, IDB_EFFECT_MINIMIZE, m_BitmapMinimize);
	CreateBitmapButton (IDC_EFFECT_TIME_BAR_MAXIMIZE, IDB_EFFECT_MAXIMIZE, m_BitmapMaximize);

	CreateBitmapButton (IDC_EFFECT_TIME_BAR_XYZ, IDB_EFFECT_XYZ, m_BitmapXYZ);
	CreateBitmapButton (IDC_EFFECT_TIME_BAR_XYYZZX, IDB_EFFECT_XYYZZX, m_BitmapXYYZZX);
}

void CEffectTimeBar::OnLButtonDown (UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown (nFlags, point);
}

void CEffectTimeBar::OnSize (UINT nType, int cx, int cy)
{
	CDialog::OnSize (nType, cx, cy);

	m_EffectTimeGraph.Resizing (cx, cy);
}

void CEffectTimeBar::OnPlay()
{
	Play();
}

void CEffectTimeBar::Play()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneEffect * pSceneEffect = pApplication->GetSceneEffect();

	pSceneEffect->Play();

	CheckDlgButton (IDC_EFFECT_TIME_BAR_PLAY, FALSE);
}

BOOL CEffectTimeBar::OnCommand (WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD (wParam))
	{
		case IDC_EFFECT_TIME_BAR_GRAPH_LIST:
			SelectGraph (m_dwSelectedIndex, m_ctrlGraphList.GetCurSel());
			break;
		case IDC_EFFECT_TIME_BAR_LIFE_TIME:
		{
			CString strLifeTime;
			GetDlgItemText (IDC_EFFECT_TIME_BAR_LIFE_TIME, strLifeTime);

			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

			pEffectAccessor->SetLifeTime (atof (strLifeTime));
		}
		break;
	}

	return CDialog::OnCommand (wParam, lParam);
}

void CEffectTimeBar::OnZoomInHorizon()
{
	m_GraphCtrl.ZoomInHorizon();

	CheckDlgButton (IDC_EFFECT_TIME_BAR_ZOOM_IN_HORIZON, FALSE);
}
void CEffectTimeBar::OnZoomOutHorizon()
{
	m_GraphCtrl.ZoomOutHorizon();

	CheckDlgButton (IDC_EFFECT_TIME_BAR_ZOOM_OUT_HORIZON, FALSE);
}

void CEffectTimeBar::OnPlayLoop()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneEffect * pSceneEffect = pApplication->GetSceneEffect();

	if (IsDlgButtonChecked (IDC_EFFECT_TIME_BAR_PLAY_LOOP))
	{
		pSceneEffect->PlayLoop();
	}
	else
	{
		pSceneEffect->StopLoop();
	}
}

void CEffectTimeBar::OnStop()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneEffect * pSceneEffect = pApplication->GetSceneEffect();

	pSceneEffect->Stop();
}

void CEffectTimeBar::OnMinimize()
{
	GetDlgItem (IDC_EFFECT_TIME_BAR_MAXIMIZE)->ShowWindow (TRUE);
	GetDlgItem (IDC_EFFECT_TIME_BAR_MINIMIZE)->ShowWindow (FALSE);

	CheckDlgButton (IDC_EFFECT_TIME_BAR_MINIMIZE, FALSE);
	CheckDlgButton (IDC_EFFECT_TIME_BAR_MAXIMIZE, FALSE);

	CRect Rect;
	GetWindowRect (&Rect);
	MoveWindow (Rect.left, Rect.top, Rect.Width(), Rect.Height() - 283);
}

void CEffectTimeBar::OnMaximize()
{
	GetDlgItem (IDC_EFFECT_TIME_BAR_MAXIMIZE)->ShowWindow (FALSE);
	GetDlgItem (IDC_EFFECT_TIME_BAR_MINIMIZE)->ShowWindow (TRUE);

	CheckDlgButton (IDC_EFFECT_TIME_BAR_MAXIMIZE, FALSE);
	CheckDlgButton (IDC_EFFECT_TIME_BAR_MAXIMIZE, FALSE);

	CRect Rect;
	GetWindowRect (&Rect);
	MoveWindow (Rect.left, Rect.top, Rect.Width(), Rect.Height() + 283);
}

void CEffectTimeBar::OnEffectTimeBarXyz()
{
	// TODO: Add your control notification handler code here
	CPickingArrows::SetArrowSets (0);
	CheckDlgButton (IDC_EFFECT_TIME_BAR_XYZ, TRUE);
	CheckDlgButton (IDC_EFFECT_TIME_BAR_XYYZZX, FALSE);
}

void CEffectTimeBar::OnEffectTimeBarXyyzzx()
{
	// TODO: Add your control notification handler code here
	CPickingArrows::SetArrowSets (1);
	CheckDlgButton (IDC_EFFECT_TIME_BAR_XYZ, FALSE);
	CheckDlgButton (IDC_EFFECT_TIME_BAR_XYYZZX, TRUE);
}
