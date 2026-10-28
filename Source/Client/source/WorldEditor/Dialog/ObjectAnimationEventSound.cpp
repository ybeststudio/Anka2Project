// ObjectAnimationEventSound.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventSound.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventSound dialog


CObjectAnimationEventSound::CObjectAnimationEventSound (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventSound::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventSound)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventSound::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventSound)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventSound, CDialog)
	//{{AFX_MSG_MAP(CObjectAnimationEventSound)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_EVENT_SOUND_LOAD, OnLoadSound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventBase normal functions

BOOL CObjectAnimationEventSound::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventSound::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}

void CObjectAnimationEventSound::Close()
{
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_SOUND_FILE_NAME, m_strSoundFileName);
}

void CObjectAnimationEventSound::GetData (CRaceMotionDataAccessor::TMotionEventData * pData)
{
	CRaceMotionDataAccessor::TMotionSoundEventData * pSoundEventData = (CRaceMotionDataAccessor::TMotionSoundEventData*)pData;
	pSoundEventData->strSoundFileName = m_strSoundFileName;
}

void CObjectAnimationEventSound::SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData)
{
	const CRaceMotionDataAccessor::TMotionSoundEventData * c_pSoundEventData = (const CRaceMotionDataAccessor::TMotionSoundEventData*)c_pData;
	SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_SOUND_FILE_NAME, c_pSoundEventData->strSoundFileName.c_str());
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventSound message handlers

void CObjectAnimationEventSound::OnLoadSound()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Sound Files (*.wav)|*.wav|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_SOUND_FILE_NAME, FileOpener.GetPathName());
	}
}
