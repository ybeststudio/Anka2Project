// FloatEditDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectParticleTimeEventGraph.h"
#include "FloatEditDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFloatEditDialog dialog


CFloatEditDialog::CFloatEditDialog (CWnd* pParent /*=NULL*/)
	: CDialog (CFloatEditDialog::IDD, pParent)
{
	m_pAccessor = NULL;

	//{{AFX_DATA_INIT(CFloatEditDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFloatEditDialog::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CFloatEditDialog)
	DDX_Control (pDX, IDC_FLOAT_EDIT_LIST_VALUE, m_ctrlListValue);
	DDX_Control (pDX, IDC_FLOAT_EDIT_LIST_TIME, m_ctrlListTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CFloatEditDialog, CDialog)
	//{{AFX_MSG_MAP(CFloatEditDialog)
	ON_WM_SIZE()
	ON_NOTIFY (LVN_ENDLABELEDIT, IDC_FLOAT_EDIT_LIST_TIME, OnEndlabeleditFloatEditListTime)
	ON_NOTIFY (NM_CLICK, IDC_FLOAT_EDIT_LIST_TIME, OnClickFloatEditListTime)
	ON_NOTIFY (LVN_ENDLABELEDIT, IDC_FLOAT_EDIT_LIST_VALUE, OnEndlabeleditFloatEditListValue)
	ON_NOTIFY (NM_DBLCLK, IDC_FLOAT_EDIT_LIST_TIME, OnDblclkFloatEditListTime)
	ON_NOTIFY (NM_CLICK, IDC_FLOAT_EDIT_LIST_VALUE, OnClickFloatEditListValue)
	ON_NOTIFY (NM_DBLCLK, IDC_FLOAT_EDIT_LIST_VALUE, OnDblclkFloatEditListValue)
	ON_NOTIFY (NM_RCLICK, IDC_FLOAT_EDIT_LIST_VALUE, OnRclickFloatEditListValue)
	ON_NOTIFY (NM_RCLICK, IDC_FLOAT_EDIT_LIST_TIME, OnRclickFloatEditListTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloatEditDialog message handlers

BOOL CFloatEditDialog::Create (CWnd* pParentWnd,  CEffectTimeEventGraph::IValueAccessor * pAccessor)
{
	// TODO: Add your specialized code here and/or call the base class

	if (!CDialog::Create (IDD, pParentWnd))
	{
		return FALSE;
	}

	CRect r, rc;
	GetClientRect (&r);
	rc = r;
	rc.right -= rc.Width() / 2;
	m_ctrlListTime.MoveWindow (&rc);

	rc = r;
	rc.left = rc.right - rc.Width() / 2;
	m_ctrlListValue.MoveWindow (&rc);

	m_ctrlListTime.GetClientRect (&rc);
	m_ctrlListTime.InsertColumn (0, "Time (0~1)", LVCFMT_LEFT, rc.Width());

	m_ctrlListValue.GetClientRect (&rc);
	m_ctrlListValue.InsertColumn (0, "Value", LVCFMT_LEFT, rc.Width());

	Update (pAccessor);

	return TRUE;
}

void CFloatEditDialog::OnSize (UINT nType, int cx, int cy)
{
	CDialog::OnSize (nType, cx, cy);

	if (m_ctrlListTime.GetSafeHwnd())
	{
		CRect r, rc;
		GetClientRect (&r);
		rc = r;
		rc.right -= rc.Width() / 2;
		m_ctrlListTime.MoveWindow (&rc);
		m_ctrlListTime.GetClientRect (&rc);
		m_ctrlListTime.SetColumnWidth (0, rc.Width());

		rc = r;
		rc.left = rc.right - rc.Width() / 2;
		m_ctrlListValue.MoveWindow (&rc);
		m_ctrlListValue.GetClientRect (&rc);
		m_ctrlListValue.SetColumnWidth (0, rc.Width());

	}
}

void CFloatEditDialog::Update (ValueAccessor * pAccessor)
{
	m_ctrlListTime.DeleteAllItems();
	m_ctrlListValue.DeleteAllItems();
	if (pAccessor)
	{
		DWORD i;
		m_pAccessor = pAccessor;

		for (i = 0; i < m_pAccessor->GetValueCount(); i++)
		{
			char str[128];

			float time;
			m_pAccessor->GetTime (i, &time);
			sprintf (str, "%.5f", time);
			m_ctrlListTime.InsertItem (i, str);

			float value;
			m_pAccessor->GetValue (i, &value);
			sprintf (str, "%.5f", value);
			m_ctrlListValue.InsertItem (i, str);

		}
	}
}

void CFloatEditDialog::OnEndlabeleditFloatEditListTime (NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	DWORD iIndex = pDispInfo->item.iItem;

	if (iIndex >= m_pAccessor->GetValueCount())
	{
		return;
	}

	if (!pDispInfo->item.pszText)
	{
		return;
	}

	//*pResult = TRUE;

	float newtime = atof (pDispInfo->item.pszText);
	float time;

	m_pAccessor->GetTime (iIndex, &time);
	if (time == newtime)
	{
		return;
	}

	float value;

	m_pAccessor->GetValue (iIndex, &value);

	m_pAccessor->Delete (iIndex);

	/*for(int i=0;i<m_pAccessor->GetValueCount();i++)
	{
		m_pAccessor->GetTime(i,&time);
		if (time>newtime)
		{
			m_pAccessor->Insert(newtime,value);
			return;
		}
	}*/
	m_pAccessor->Insert (newtime, value);
	Update (m_pAccessor);
	m_ctrlListTime.Invalidate();
	m_ctrlListValue.Invalidate();
}

void CFloatEditDialog::OnClickFloatEditListTime (NMHDR* pNMHDR, LRESULT* pResult)
{

	if (GetAsyncKeyState (VK_CONTROL) & 0x8000)
	{
		POSITION pos = m_ctrlListTime.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			DWORD dwCount = m_pAccessor->GetValueCount();
			if (dwCount)
			{
				dwCount--;

				float time, value;
				m_pAccessor->GetTime (dwCount, &time);
				m_pAccessor->GetValue (dwCount, &value);

				m_pAccessor->Insert (time, value);
			}
			else
			{
				m_pAccessor->Insert (0.0f, 0.0f);
			}
		}
		else
		{
			int nItem = m_ctrlListTime.GetNextSelectedItem (pos);
			if (nItem)
			{
				int nPrevItem = nItem - 1;
				float time1, time2;
				float value1, value2;

				m_pAccessor->GetTime (nPrevItem, &time1);
				m_pAccessor->GetValue (nPrevItem, &value1);

				m_pAccessor->GetTime (nItem, &time2);
				m_pAccessor->GetValue (nItem, &value2);

				m_pAccessor->Insert ((time1 + time2) / 2, (value1 + value2) / 2);
			}
			else
			{
				float time, value;
				m_pAccessor->GetTime (0, &time);
				m_pAccessor->GetValue (0, &value);

				m_pAccessor->Insert (time, value);
			}
		}
		Update (m_pAccessor);
		Invalidate();
	}

	*pResult = 0;
}

void CFloatEditDialog::OnEndlabeleditFloatEditListValue (NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	*pResult = 0;

	DWORD iIndex = pDispInfo->item.iItem;

	if (iIndex >= m_pAccessor->GetValueCount())
	{
		return;
	}

	if (!pDispInfo->item.pszText)
	{
		return;
	}

	float value;
	float newvalue;

	newvalue = atof (pDispInfo->item.pszText);

	m_pAccessor->GetValue (iIndex, &value);

	if (newvalue == value)
	{
		return;
	}

	*pResult = TRUE;

	m_pAccessor->SetValue (iIndex, newvalue);

	//m_ctrlListValue.Invalidate();

}

void CFloatEditDialog::OnDblclkFloatEditListTime (NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION pos = m_ctrlListTime.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_ctrlListTime.GetNextSelectedItem (pos);
		m_ctrlListTime.EditLabel (nItem);
	}

	*pResult = 0;
}

void CFloatEditDialog::OnClickFloatEditListValue (NMHDR* pNMHDR, LRESULT* pResult)
{
	if (GetAsyncKeyState (VK_CONTROL) & 0x8000)
	{
		POSITION pos = m_ctrlListValue.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
			DWORD dwCount = m_pAccessor->GetValueCount();
			if (dwCount)
			{
				dwCount--;

				float time, value;
				m_pAccessor->GetTime (dwCount, &time);
				m_pAccessor->GetValue (dwCount, &value);

				m_pAccessor->Insert (time, value);
			}
			else
			{
				m_pAccessor->Insert (0.0f, 0.0f);
			}
		}
		else
		{
			int nItem = m_ctrlListValue.GetNextSelectedItem (pos);
			if (nItem)
			{
				int nPrevItem = nItem - 1;
				float time1, time2;
				float value1, value2;

				m_pAccessor->GetTime (nPrevItem, &time1);
				m_pAccessor->GetValue (nPrevItem, &value1);

				m_pAccessor->GetTime (nItem, &time2);
				m_pAccessor->GetValue (nItem, &value2);

				m_pAccessor->Insert ((time1 + time2) / 2, (value1 + value2) / 2);
			}
			else
			{
				float time, value;
				m_pAccessor->GetTime (0, &time);
				m_pAccessor->GetValue (0, &value);

				m_pAccessor->Insert (time, value);
			}
		}
		Update (m_pAccessor);
		Invalidate();
	}

	*pResult = 0;
}

void CFloatEditDialog::OnDblclkFloatEditListValue (NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION pos = m_ctrlListValue.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_ctrlListValue.GetNextSelectedItem (pos);
		m_ctrlListValue.EditLabel (nItem);
	}

	*pResult = 0;
}

void CFloatEditDialog::OnRclickFloatEditListValue (NMHDR* pNMHDR, LRESULT* pResult)
{
	if (GetAsyncKeyState (VK_CONTROL) & 0x8000)
	{
		POSITION pos = m_ctrlListValue.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nItem = m_ctrlListValue.GetNextSelectedItem (pos);
			m_pAccessor->Delete (nItem);

			Update (m_pAccessor);
			Invalidate();
		}
	}

	*pResult = 0;
}

void CFloatEditDialog::OnRclickFloatEditListTime (NMHDR* pNMHDR, LRESULT* pResult)
{
	if (GetAsyncKeyState (VK_CONTROL) & 0x8000)
	{
		POSITION pos = m_ctrlListTime.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nItem = m_ctrlListTime.GetNextSelectedItem (pos);
			m_pAccessor->Delete (nItem);

			Update (m_pAccessor);
			Invalidate();
		}
	}

	*pResult = 0;
}
