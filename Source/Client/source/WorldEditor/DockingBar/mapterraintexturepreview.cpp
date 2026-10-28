// mapterraintexturepreview.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "mapterraintexturepreview.h"
#include "../DataCtrl/MapAccessorTerrain.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

#include <il/ilu.h>

/////////////////////////////////////////////////////////////////////////////
// CMapTerrainTexturePreview

CMapTerrainTexturePreview::CMapTerrainTexturePreview()
{
	m_ilImage = 0;
}

CMapTerrainTexturePreview::~CMapTerrainTexturePreview()
{
	if (m_ilImage)
	{
		ilDeleteImages (1, &m_ilImage);
	}

	m_ilImage = 0;
}

BEGIN_MESSAGE_MAP (CMapTerrainTexturePreview, CWnd)
	//{{AFX_MSG_MAP(CMapTerrainTexturePreview)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapTerrainTexturePreview message handlers

BOOL CMapTerrainTexturePreview::Create()
{
	CRect rect;
	GetClientRect (&rect);
	if (!m_DIB.Create (NULL, rect.Width(), rect.Height()))
	{
		return FALSE;
	}

	ilGenImages (1, &m_ilImage);
	ilBindImage (m_ilImage);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMapTerrainTexturePreview message handlers

void CMapTerrainTexturePreview::UpdatePreview (int nTexNum)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	CTextureSet * pTextureSet = CTerrain::GetTextureSet();

	if (nTexNum == 0)
	{
		ilClearImage();
	}
	else
	{
		CGraphicImageInstance & rImageInstance = pTextureSet->GetTexture (nTexNum).ImageInstance;
		CGraphicImage * pImage = rImageInstance.GetGraphicImagePointer();
		ilLoadImage ((const ILstring)pImage->GetFileName());
		iluScale (m_DIB.GetWidth(), m_DIB.GetHeight(), 1);
		Invalidate (TRUE);
	}
}

void CMapTerrainTexturePreview::OnPaint()
{
	CPaintDC dc (this); // device context for painting

	DWORD * pdwDst = (DWORD*)m_DIB.GetPointer();

	if (ilIsImage (m_ilImage))
	{
		DWORD * pdwSrc = (DWORD*)ilGetData();
		int iwidth = ilGetInteger (IL_IMAGE_WIDTH);
		int iheight = ilGetInteger (IL_IMAGE_HEIGHT);
		int iBPP = ilGetInteger (IL_IMAGE_BYTES_PER_PIXEL);

		for (int y = 0; y < iheight; ++y)
		{
			for (int x = 0; x < iwidth; ++x)
			{
				DWORD dwColor = pdwSrc[x + y * iwidth];
				BYTE byr = dwColor & 0xff;
				BYTE byg = (dwColor >> 8) & 0xff;
				BYTE byb = (dwColor >> 16) & 0xff;
				dwColor = DWORD (0xff << 24) | DWORD (byr << 16) | DWORD (byg << 8) | DWORD (byb);
				pdwDst[x + y * m_DIB.GetWidth()] = dwColor;
			}
		}
	}
	else
	{
		memset (pdwDst, 0, m_DIB.GetWidth()*m_DIB.GetHeight() * 4);
	}

	m_DIB.Put (dc.GetSafeHdc(), 0, 0);
}
