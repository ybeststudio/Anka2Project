// Test.cpp : implementation file
//

#include "StdAfx.h"
#include "EffectUI.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

const char c_szBlendTypeCount = 13;
const char c_szBlendTypeName[c_szBlendTypeCount][32] =
{
	"Zero             ",
	"One              ",
	"Src Color        ",
	"InvSrc Color     ",
	"Src Alpha        ",
	"InvSrc Alpha     ",
	"Dest Alpha       ",
	"InvDest Alpha    ",
	"Dest Color       ",
	"InvDest Color    ",
	"Src Alpha Sat    ",
	"Both Src Alpha   ",
	"Both InvSrc Alpha",
};

/////////////////////////////////////////////////////////////////////////////
// CBlendTypeComboBox

CBlendTypeComboBox::CBlendTypeComboBox()
{
}
CBlendTypeComboBox::~CBlendTypeComboBox()
{
}

BEGIN_MESSAGE_MAP (CBlendTypeComboBox, CComboBox)
	//{{AFX_MSG_MAP(CBlendTypeComboBox)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlendTypeComboBox normal functions

BOOL CBlendTypeComboBox::Create()
{
	for (DWORD i = 0; i < c_szBlendTypeCount; ++i)
	{
		InsertString (i, c_szBlendTypeName[i]);
	}
	SelectString (0, c_szBlendTypeName[0]);

	return TRUE;
}

void CBlendTypeComboBox::SelectBlendType (DWORD dwIndex)
{
	dwIndex -= 1;

	if (dwIndex >= c_szBlendTypeCount)
	{
		return;
	}

	SelectString (0, c_szBlendTypeName[dwIndex]);
}

int CBlendTypeComboBox::GetBlendType() const
{
	return GetCurSel() + 1;
}

/////////////////////////////////////////////////////////////////////////////
// CBlendTypeComboBox message handlers


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


const char c_szOperationTypeCount = 26;
const char c_szOperationTypeName[c_szOperationTypeCount][32] =
{
	"Disable",
	"SelectArg1",
	"SelectArg2",
	"Modulate",
	"Modulate 2x",
	"Modulate 4x",
	"Add",
	"Add Signed",
	"Add Signed 2x",
	"Subtract",
	"Add Smooth",
	"Blend Diffuse Alpha",
	"Blend Texture Alpha",
	"Blend Factor Alpha",
	"Blend Texture Alpha PM",
	"Blend Current Alpha",
	"PreModulate",
	"Modulate Alpha & Add Color",
	"Modulate Color & Add Alpha",
	"Modulate InvAlpha & Add Color",
	"Modulate InvColor & Add Alpha",
	"Bump EnvMap",
	"Bump EnvMap Luminance",
	"Dot Product3",
	"Multiply Add",
	"Lerp",
};

/////////////////////////////////////////////////////////////////////////////
// COperationTypeComboBox

COperationTypeComboBox::COperationTypeComboBox()
{
}
COperationTypeComboBox::~COperationTypeComboBox()
{
}

BEGIN_MESSAGE_MAP (COperationTypeComboBox, CComboBox)
	//{{AFX_MSG_MAP(COperationTypeComboBox)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COperationTypeComboBox normal functions

BOOL COperationTypeComboBox::Create()
{
	for (DWORD i = 0; i < c_szOperationTypeCount; ++i)
	{
		InsertString (i, c_szOperationTypeName[i]);
	}
	SelectString (0, c_szOperationTypeName[0]);

	return TRUE;
}

void COperationTypeComboBox::SelectOperationType (DWORD dwIndex)
{
	dwIndex -= 1;

	if (dwIndex >= c_szOperationTypeCount)
	{
		return;
	}

	SelectString (0, c_szOperationTypeName[dwIndex]);
}

int COperationTypeComboBox::GetOperationType() const
{
	return GetCurSel() + 1;
}

/////////////////////////////////////////////////////////////////////////////
// CBlendTypeComboBox message handlers
