// ToolBar256.cpp : implementation file
//
// 게임개발자용 맵에디터
// 저작권자 (c) 2001 신용우 (grace77@hanimail.com)
//
// 이 프로그램의 모든 권리는 저작권자에게 있습니다.
// 저작권자의 동의없이 프로그램을 설치/사용/수정/배포할 수 없습니다.

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ToolBar256.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolBar256

CToolBar256::CToolBar256()
{
	m_nBitmapID = -1;
}

CToolBar256::~CToolBar256()
{
	::DeleteObject (m_hbmImageWell);
	m_hbmImageWell = NULL;
}


BEGIN_MESSAGE_MAP (CToolBar256, CToolBar)
	//{{AFX_MSG_MAP(CToolBar256)
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolBar256 message handlers
BOOL CToolBar256::LoadBitmap (int nResourceID)
{
	return LoadBitmap (MAKEINTRESOURCE (nResourceID));
}

BOOL CToolBar256::LoadBitmap (LPCTSTR lpszResourceName)
{
	ASSERT_VALID (this);
	ASSERT (lpszResourceName != NULL);

	// determine location of the bitmap in resource fork
	HINSTANCE hInstImageWell = AfxFindResourceHandle (lpszResourceName, RT_BITMAP);
	HRSRC hRsrcImageWell = ::FindResource (hInstImageWell, lpszResourceName, RT_BITMAP);
	if (hRsrcImageWell == NULL)
	{
		return FALSE;
	}

	// load the bitmap
	HBITMAP hbmImageWell;
	hbmImageWell = GetBitmap256 (hInstImageWell, hRsrcImageWell, FALSE);

	// tell common control toolbar about the new bitmap
	if (!AddReplaceBitmap (hbmImageWell))
	{
		return FALSE;
	}

	// remember the resource handles so the bitmap can be recolored if necessary
	m_hInstImageWell = hInstImageWell;
	m_hRsrcImageWell = hRsrcImageWell;
	return TRUE;
}


#define CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))
HBITMAP CToolBar256::GetBitmap256 (HINSTANCE hInst, HRSRC hRsrc, BOOL bMono)
{
	HGLOBAL hglb;
	if ((hglb = LoadResource (hInst, hRsrc)) == NULL)
	{
		return NULL;
	}

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource (hglb);
	if (lpBitmap == NULL)
	{
		return NULL;
	}

	// make copy of BITMAPINFOHEADER so we can modify the color table
	int nColorTableSize = 0;
	if (lpBitmap->biBitCount == 8)
	{
		nColorTableSize = 256;
	}
	else if (lpBitmap->biBitCount == 4)
	{
		nColorTableSize = 16;
	}
	else
	{
		ASSERT (FALSE);
	}

	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof (RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc (nSize);
	if (lpBitmapInfo == NULL)
	{
		return NULL;
	}
	memcpy (lpBitmapInfo, lpBitmap, nSize);

	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*) (((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);

	const DWORD mc = 0x00ff00ff;

	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		if (pColorTable[iColor] == mc)
		{
			pColorTable[iColor] = CLR_TO_RGBQUAD (::GetSysColor (COLOR_BTNFACE));
		}
	}

	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC (NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap (hDCScreen, nWidth, nHeight);

	if (hbm != NULL)
	{
		HDC hDCGlyphs = ::CreateCompatibleDC (hDCScreen);
		HBITMAP hbmOld = (HBITMAP)::SelectObject (hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE) (lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof (RGBQUAD);

		StretchDIBits (hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
					   lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject (hDCGlyphs, hbmOld);
		::DeleteDC (hDCGlyphs);
	}
	::ReleaseDC (NULL, hDCScreen);

	// free copy of bitmap info struct and resource itself
	::free (lpBitmapInfo);
	::FreeResource (hglb);

	return hbm;
}


void CToolBar256::OnSysColorChange()
{
	CToolBar::OnSysColorChange();

	// TODO: Add your message handler code here
	LoadBitmap (m_nBitmapID);
}

BOOL CToolBar256::AddReplaceBitmap (HBITMAP hbmImageWell)
{
	// need complete bitmap size to determine number of images
	BITMAP bitmap;
	VERIFY (::GetObject (hbmImageWell, sizeof (BITMAP), &bitmap));

	// add the bitmap to the common control toolbar
	BOOL bResult;
	if (m_hbmImageWell == NULL)
	{
		TBADDBITMAP addBitmap;
		addBitmap.hInst = NULL; // makes TBADDBITMAP::nID behave a HBITMAP
		addBitmap.nID = (UINT)hbmImageWell;
		bResult =  DefWindowProc (TB_ADDBITMAP,
		bitmap.bmWidth / m_sizeImage.cx, (LPARAM)&addBitmap) == 0;
	}
	else
	{
		TBREPLACEBITMAP replaceBitmap;
		replaceBitmap.hInstOld = NULL;
		replaceBitmap.nIDOld = (UINT)m_hbmImageWell;
		replaceBitmap.hInstNew = NULL;
		replaceBitmap.nIDNew = (UINT)hbmImageWell;
		replaceBitmap.nButtons = bitmap.bmWidth / m_sizeImage.cx;
		bResult = (BOOL)DefWindowProc (TB_REPLACEBITMAP, 0,
									   (LPARAM)&replaceBitmap);
	}

	// remove old bitmap, if present
	if (bResult)
	{
		::DeleteObject (m_hbmImageWell);
		m_hbmImageWell = hbmImageWell;
	}

	return bResult;
}
