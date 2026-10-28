#include "StdAfx.h"
#include "Util.h"

char					g_szProgramPath[PROGRAM_PATH_LENGTH + 1];
char					g_szProgramWindowPath[PROGRAM_PATH_LENGTH + 1];

BOOL					g_isTileData = FALSE;
BOOL					g_isTileFile = TRUE;
BOOL					g_isCharacterData = FALSE;
BOOL					g_isCharacterFile = TRUE;
LPD3DXMESH				g_pSphereMesh;
LPD3DXMESH				g_pCylinderMesh;
CGraphicThing* 			g_pTileThing = NULL;
CTileInstance			g_TileInstance;
CGraphicThingInstance	g_CharacterInstance;

void CreateBitmapButton (CButton * pButton, int iBitmapID, CBitmap & rBitmap)
{
	rBitmap.DeleteObject();
	rBitmap.LoadBitmap (iBitmapID);
	pButton->SetBitmap (rBitmap);
}

void RepositioningWindow (CPageCtrl & rPageCtrl, int ixTemporarySpace, int iyTemporarySpace)
{
	CRect Rect;
	rPageCtrl.GetClientRect (Rect);
	rPageCtrl.MoveWindow (Rect.left + ixTemporarySpace, Rect.top + iyTemporarySpace, Rect.Width(), Rect.Height());
}

void CreateHighColorImageList (int iBitmapID, CImageList * pImageList)
{
	CBitmap Bitmap;

	HINSTANCE hInstance = ::AfxFindResourceHandle (MAKEINTRESOURCE (iBitmapID), RT_BITMAP);
	HANDLE hImage = ::LoadImage (hInstance, MAKEINTRESOURCE (iBitmapID), IMAGE_BITMAP, 0, 0, (LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	Bitmap.Attach (hImage);

	// Replace
	BITMAP bmInfo;
	const RGBTRIPLE	kBackgroundColor = {192, 192, 192};

	VERIFY (Bitmap.GetBitmap (&bmInfo));

	const UINT numPixels (bmInfo.bmHeight * bmInfo.bmWidth);
	DIBSECTION ds;

	VERIFY (Bitmap.GetObject (sizeof (DIBSECTION), &ds) == sizeof (DIBSECTION));

	RGBTRIPLE * pixels = reinterpret_cast<RGBTRIPLE*> (ds.dsBm.bmBits);
	VERIFY (pixels != NULL);

	const COLORREF buttonColor (::GetSysColor (COLOR_BTNFACE));
	const RGBTRIPLE userBackgroundColor = { GetBValue (buttonColor), GetGValue (buttonColor), GetRValue (buttonColor) };

	for (UINT i = 0; i < numPixels; ++i)
	{
		if (pixels[i].rgbtBlue == kBackgroundColor.rgbtBlue &&
		pixels[i].rgbtGreen == kBackgroundColor.rgbtGreen &&
		pixels[i].rgbtRed == kBackgroundColor.rgbtRed)
		{
			pixels[i] = userBackgroundColor;
		}
	}

	// Create
	pImageList->Create (16, 16, ILC_COLOR24, 5, 0);
	pImageList->Add (&Bitmap, RGB (0, 0, 0));
}

void SetDialogFloatText (HWND hWnd, int iID, float fData)
{
	char szString[64 + 1];
	_snprintf (szString, 64, "%.3f", fData);
	SetDlgItemText (hWnd, iID, szString);
}

void SetDialogIntegerText (HWND hWnd, int iID, int iData)
{
	char szString[64 + 1];
	_snprintf (szString, 64, "%i", iData);
	SetDlgItemText (hWnd, iID, szString);
}

float GetDialogFloatText (HWND hWnd, int iID)
{
	char szString[64 + 1];
	GetDlgItemText (hWnd, iID, szString, 64);
	return atof (szString);
}

int GetDialogIntegerText (HWND hWnd, int iID)
{
	char szString[64 + 1];
	GetDlgItemText (hWnd, iID, szString, 64);
	return atoi (szString);
}

void SelectComboBoxItem (CComboBox & rComboBox, DWORD dwIndex)
{
	CString strItem;
	rComboBox.GetLBText (dwIndex, strItem);
	rComboBox.SelectString (0, strItem);
}

const char* GetProgramExcutingPath (const char* c_szFullFileName)
{
	if (strlen (c_szFullFileName) < strlen (g_szProgramPath) + 1)
	{
		return c_szFullFileName;
	}

	for (DWORD i = 0; i < strlen (g_szProgramPath); ++i)
	{
		char src, dst;

		src = tolower (c_szFullFileName[i]);
		dst = tolower (g_szProgramPath[i]);

		if (src == '/')
		{
			src = '\\';
		}

		if (dst == '/')
		{
			dst = '\\';
		}

		if (src != dst)
		{
			return &c_szFullFileName[i];
		}
	}

	return &c_szFullFileName[strlen (g_szProgramPath) + 1];
}

void CreateUtilData()
{
	class CUtilScreen : public CScreen
	{
		public:
			CUtilScreen()
			{
				if (FAILED (D3DXCreateSphere (ms_lpd3dDevice, 1.0f, 8, 8, &g_pSphereMesh, NULL)))
				{
					LogBox ("CreateUtilData: Cannot create sphere primitive");
					return;
				}

				if (FAILED (D3DXCreateCylinder (ms_lpd3dDevice, 1.0f, 1.0f, 1.0f, 8, 8, &g_pCylinderMesh, NULL)))
				{
					LogBox ("CreateUtilData: Cannot create cylinder primitive");
					return;
				}
			}
	} UtilScreen;
}

void DestroyUtilData()
{
	SAFE_RELEASE (g_pSphereMesh);
	SAFE_RELEASE (g_pCylinderMesh);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

class CSetRenderingOption : public CScreen
{
	public:
		BOOL	m_isFilled;
		DWORD	m_dwVS;

		CSetRenderingOption (BOOL isFilled, const D3DXMATRIX & c_rmatWorld)
		{
			ms_lpd3dDevice->GetFVF (&m_dwVS);

			STATEMANAGER.SaveTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			STATEMANAGER.SaveTextureStageState (0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			STATEMANAGER.SaveTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			m_isFilled = isFilled;

			if (m_isFilled)
			{
				STATEMANAGER.SaveRenderState (D3DRS_ALPHABLENDENABLE, TRUE);
				STATEMANAGER.SaveRenderState (D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				STATEMANAGER.SaveRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
			else
			{
				STATEMANAGER.SetRenderState (D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			}

			STATEMANAGER.SetTransform (D3DTS_WORLD, &c_rmatWorld);

			STATEMANAGER.SetTexture (0, NULL);
			STATEMANAGER.SetTexture (1, NULL);
		}

		~CSetRenderingOption()
		{
			ms_lpd3dDevice->SetFVF (m_dwVS);

			STATEMANAGER.RestoreTextureStageState (0, D3DTSS_COLORARG1);
			STATEMANAGER.RestoreTextureStageState (0, D3DTSS_COLOROP);
			STATEMANAGER.RestoreTextureStageState (0, D3DTSS_ALPHAOP);

			if (m_isFilled)
			{
				STATEMANAGER.RestoreRenderState (D3DRS_ALPHABLENDENABLE);
				STATEMANAGER.RestoreRenderState (D3DRS_SRCBLEND);
				STATEMANAGER.RestoreRenderState (D3DRS_DESTBLEND);
			}
			else
			{
				STATEMANAGER.SetRenderState (D3DRS_FILLMODE, D3DFILL_SOLID);
			}
		}
};

void RenderSphere (const D3DXMATRIX * c_pmatWorld, float fx, float fy, float fz, float fRadius, BOOL isFilled /* = FALSE */)
{
	D3DXMATRIX matTranslation;
	D3DXMATRIX matScaling;

	D3DXMatrixTranslation (&matTranslation, fx, fy, fz);
	D3DXMatrixScaling (&matScaling, fRadius, fRadius, fRadius);

	D3DXMATRIX matWorld;
	matWorld = matScaling * matTranslation;

	if (c_pmatWorld)
	{
		matWorld *= *c_pmatWorld;
	}

	CSetRenderingOption SetRenderingOption (isFilled, matWorld);
	g_pSphereMesh->DrawSubset (0);
}

void RenderCylinder (const D3DXMATRIX * c_pmatWorld, float fx, float fy, float fz, float fRadius, float fLength, BOOL isFilled)
{
	D3DXMATRIX matTranslation;
	D3DXMATRIX matScaling;

	D3DXMatrixTranslation (&matTranslation, fx, fy, fz);
	D3DXMatrixScaling (&matScaling, fRadius, fRadius, fLength);

	D3DXMATRIX matWorld;
	matWorld = matScaling * matTranslation;

	if (c_pmatWorld)
	{
		matWorld *= *c_pmatWorld;
	}

	CSetRenderingOption SetRenderingOption (isFilled, matWorld);
	g_pCylinderMesh->DrawSubset (0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void RenderBackGroundCharacter (float fx, float fy, float fz)
{
	if (!g_isCharacterFile)
	{
		return;
	}

	if (!g_isCharacterData)
	{
		//CGraphicThing * pCharacter = (CGraphicThing *)CResourceManager::Instance().GetResourcePointer("D:\\Ymir Work\\pc\\warrior\\warrior_novice.GR2");
		//CGraphicThing * pMotion = (CGraphicThing *)CResourceManager::Instance().GetResourcePointer("D:\\Ymir Work\\pc\\warrior\\general\\wait.GR2");
		CGraphicThing * pCharacter = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer ("D:\\Ymir Work\\pc\\sura\\sura_novice.GR2");
		CGraphicThing * pMotion = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer ("D:\\Ymir Work\\pc\\sura\\onehand_sword\\wait.GR2");

		g_CharacterInstance.Clear();
		g_CharacterInstance.ReserveModelThing (1);
		g_CharacterInstance.ReserveModelInstance (1);
		g_CharacterInstance.RegisterModelThing (0, pCharacter);
		g_CharacterInstance.RegisterMotionThing (1, pMotion);

		if (pCharacter->IsEmpty())
		{
			g_isCharacterFile = FALSE;
			LogBoxf ("%s is not exist", pCharacter->GetFileName());
			return;
		}

		if (pMotion->IsEmpty())
		{
			g_isCharacterFile = FALSE;
			LogBoxf ("%s is not exist", pMotion->GetFileName());
			return;
		}

		g_CharacterInstance.SetModelInstance (0, 0, 0);
		g_CharacterInstance.SetMotion (1);
		g_isCharacterData = TRUE;
	}

	g_CharacterInstance.SetPosition (fx, fy, fz);
	g_CharacterInstance.SetRotation (45.0f);
	g_CharacterInstance.Update();
	g_CharacterInstance.Transform();
	g_CharacterInstance.Deform();
	g_CharacterInstance.Render();
}

void RenderBackGroundTile()
{
	if (!g_isTileFile)
	{
		return;
	}

	if (!g_isTileData)
	{
		if (g_pTileThing)
		{
			return;
		}

		g_pTileThing = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer ("background_stone.GR2");

		g_pTileThing->AddReference();
		if (g_pTileThing->IsEmpty())
		{
			LogBox ("background_stone.gr2 is not exist");
			return;
		}
		g_TileInstance.Clear();

		if (g_TileInstance.GetDeformableVertexCount() == false)
		{
			return;
		}
		CGraphicVertexBuffer m_kSharedDeformableVertexBuffer;
		m_kSharedDeformableVertexBuffer.Destroy();
		m_kSharedDeformableVertexBuffer.Create (
			g_TileInstance.GetDeformableVertexCount(),
			D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
			D3DUSAGE_WRITEONLY,
			D3DPOOL_DEFAULT);
		g_TileInstance.SetMainModelPointer (g_pTileThing->GetModelPointer (0), &m_kSharedDeformableVertexBuffer);
		//g_TileInstance.SetModelPointer(g_pTileThing->GetModelPointer(0));

		g_TileInstance.LoadTexture();
		g_isTileData = TRUE;
	}

	g_TileInstance.RenderTile();
}

void DestroyBackGroundData()
{
	if (g_isCharacterData)
	{
		g_CharacterInstance.Clear();
	}

	if (g_isTileData)
	{
		g_TileInstance.Clear();
		g_pTileThing->Release();
	}
}

void CTileInstance::LoadTexture()
{
	CGraphicImage * pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer ("D:\\Ymir Work\\terrainmaps/b/field/field 01.dds");
	m_ImageInstance.SetImagePointer (pImage);
}

void CTileInstance::RenderTile()
{
	LPDIRECT3DVERTEXBUFFER9 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();
	LPDIRECT3DINDEXBUFFER9 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();

	const CGrannyModel::TMeshNode * pMeshNode = m_pModel->GetMeshNodeList (CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_DIFFUSE_PNT);

	while (pMeshNode)
	{
		const CGrannyMesh * pMesh = pMeshNode->pMesh;
		int vtxMeshBasePos = pMesh->GetVertexBasePosition();

		STATEMANAGER.SetIndices (lpd3dIdxBuf, vtxMeshBasePos);
		STATEMANAGER.SetFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
		STATEMANAGER.SetStreamSource (0, lpd3dRigidPNTVtxBuf, sizeof (TPNTVertex));
		STATEMANAGER.SetTransform (D3DTS_WORLD, &CScreen::GetIdentityMatrix());

		const CGrannyMesh::TTriGroupNode* pTriGroupNode = pMesh->GetTriGroupNodeList (CGrannyMaterial::TYPE_DIFFUSE_PNT);

		while (pTriGroupNode)
		{
			ms_faceCount += pTriGroupNode->triCount;

			STATEMANAGER.SetTexture (0, m_ImageInstance.GetTexturePointer()->GetD3DTexture());
			STATEMANAGER.DrawIndexedPrimitive (D3DPT_TRIANGLELIST, 0, pMesh->GetVertexCount(), pTriGroupNode->idxPos, pTriGroupNode->triCount);

			pTriGroupNode = pTriGroupNode->pNextTriGroupNode;
		}

		pMeshNode = pMeshNode->pNextMeshNode;
	}
}


std::string RemoveStartString (const std::string & strOriginal, const std::string & strBegin)
{
	if (strOriginal.substr (0, strBegin.length()) == strBegin)
	{
		return strOriginal.substr (strBegin.length(), strOriginal.npos);
	}
	return strOriginal;
}