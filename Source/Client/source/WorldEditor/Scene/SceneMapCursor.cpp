#include "StdAfx.h"
#include "SceneMapCursor.h"
#include "../../EffectLib/EffectManager.h"
#include "../../GameLib/Property.h"

#include <fstream>   // for [YOSUN_WORLDEDITOR_FIX_001]

void CCursorRenderer::Update()
{
	if (0 == m_dwCurrentCRC)
	{
		return;
	}

	const D3DXVECTOR3 & c_rv3CursorPosition = GetCursorPosition();

	// 이 경우도 상속으로 특화 시켜서 구현했어야 했는데.. - [levites]
	if (CURSOR_TYPE_TREE == m_iCursorType && m_pTreeCursor)
	{
		m_pTreeCursor->SetPosition (c_rv3CursorPosition.x,
									c_rv3CursorPosition.y,
									c_rv3CursorPosition.z + m_fObjectHeight);
	}
	else if (CURSOR_TYPE_BUILDING == m_iCursorType)
	{
		m_BuildingInstance.SetPosition (c_rv3CursorPosition.x, c_rv3CursorPosition.y, c_rv3CursorPosition.z + m_fObjectHeight);
		m_BuildingInstance.SetRotation (m_fYaw, m_fPitch, m_fRoll);
		m_BuildingInstance.Update();
		m_BuildingInstance.Transform();
		m_BuildingInstance.Deform();
	}
	else if (CURSOR_TYPE_EFFECT == m_iCursorType)
	{
		if (m_dwEffectInstanceIndex != 0xffffffff)
		{
			CEffectManager & rem = CEffectManager::Instance();
			rem.SelectEffectInstance (m_dwEffectInstanceIndex);
			D3DXMATRIX mat;
			D3DXMatrixRotationYawPitchRoll (&mat, D3DXToRadian (m_fYaw), D3DXToRadian (m_fPitch), D3DXToRadian (m_fRoll));
			mat._41 += c_rv3CursorPosition.x;
			mat._42 += c_rv3CursorPosition.y;
			mat._43 += c_rv3CursorPosition.z + GetObjectHeight();
			rem.SetEffectInstanceGlobalMatrix (mat);
		}
	}
	else if (CURSOR_TYPE_DUNGEON_BLOCK == m_iCursorType)
	{
		m_DungeonBlock.SetPosition (c_rv3CursorPosition.x, c_rv3CursorPosition.y, c_rv3CursorPosition.z + m_fObjectHeight);
		m_DungeonBlock.SetRotation (m_fYaw, m_fPitch, m_fRoll);
		m_DungeonBlock.Update();
		m_DungeonBlock.Transform();
		m_DungeonBlock.Deform();
	}
}

void CCursorRenderer::Render()
{
	if (0 == m_dwCurrentCRC)
	{
		return;
	}

	if (CURSOR_TYPE_TREE == m_iCursorType && m_pTreeCursor)
	{
		m_pTreeCursor->Render();
	}
	else if (CURSOR_TYPE_BUILDING == m_iCursorType)
	{
		m_BuildingInstance.Render();
	}
	else if (CURSOR_TYPE_EFFECT == m_iCursorType)
	{
		//  이펙트 매니저가 알아서 그려주시겠지 >_<!
	}
	else if (CURSOR_TYPE_AMBIENCE == m_iCursorType)
	{
		SetDiffuseColor (1.0f, 0.0f, 0.0f);

		const D3DXVECTOR3 & c_rv3CursorPosition = GetCursorPosition();
		float fz = c_rv3CursorPosition.z + m_fObjectHeight;

		RenderSphere (NULL, c_rv3CursorPosition.x, c_rv3CursorPosition.y, fz, m_dwAmbienceRange, D3DFILL_POINT);
		RenderSphere (NULL, c_rv3CursorPosition.x, c_rv3CursorPosition.y, fz, m_dwAmbienceRange / 10, D3DFILL_POINT);
		RenderCircle2d (c_rv3CursorPosition.x, c_rv3CursorPosition.y, fz, float (m_dwAmbienceRange));

		for (int i = 0; i < 4; ++i)
		{
			float fxAdd = cosf (float (i) * D3DX_PI / 4.0f) * float (m_dwAmbienceRange) / 2.0f;
			float fyAdd = sinf (float (i) * D3DX_PI / 4.0f) * float (m_dwAmbienceRange) / 2.0f;

			if (i % 2)
			{
				fxAdd /= 2.0f;
				fyAdd /= 2.0f;
			}

			RenderLine2d (c_rv3CursorPosition.x + fxAdd, c_rv3CursorPosition.y + fyAdd,
						  c_rv3CursorPosition.x - fxAdd, c_rv3CursorPosition.y - fyAdd, fz);
		}
	}
	else if (CURSOR_TYPE_DUNGEON_BLOCK == m_iCursorType)
	{
		m_DungeonBlock.Render();
	}
}

void CCursorRenderer::RenderPieceLine (float fxStart, float fyStart, float fxEnd, float fyEnd, int iStep)
{
	float fxStep = (fxEnd - fxStart) / float (iStep);
	float fyStep = (fyEnd - fyStart) / float (iStep);

	for (int i = 0; i < iStep; ++i)
	{
		float fx = fxStart + i * fxStep;
		float fy = fyStart + i * fyStep;
		float fCurHeight = m_pHeightObserver->GetHeight (fx, fy) + 10.0f;
		float fNextHeight = m_pHeightObserver->GetHeight (fx + fxStep, fy + fyStep) + 10.0f;
		RenderLine3d (fx, -fy, fCurHeight, fx + fxStep, - (fy + fyStep), fNextHeight);
	}
}

void CCursorRenderer::RenderCursorSquare (float fxStart, float fyStart, float fxEnd, float fyEnd, int iStep)
{
	SetDiffuseOperation();
	SetDiffuseColor (0.0f, 1.0f, 0.0f);
	STATEMANAGER.SetTransform (D3DTS_WORLD, &GetIdentityMatrix());
	STATEMANAGER.SetRenderState (D3DRS_LIGHTING, FALSE);

	RenderPieceLine (fxStart, fyStart, fxEnd, fyStart, iStep);
	RenderPieceLine (fxStart, fyEnd, fxEnd, fyEnd, iStep);
	RenderPieceLine (fxStart, fyStart, fxStart, fyEnd, iStep);
	RenderPieceLine (fxEnd, fyStart, fxEnd, fyEnd, iStep);
}

void CCursorRenderer::RenderCursorCircle (float fx, float fy, float fz, float fRadius, int iStep)
{
	SetDiffuseOperation();
	SetDiffuseColor (0.0f, 1.0f, 0.0f);
	STATEMANAGER.SetTransform (D3DTS_WORLD, &GetIdentityMatrix());
	STATEMANAGER.SetRenderState (D3DRS_LIGHTING, FALSE);

	int count;
	float theta, delta;
	float x, y, z;
	std::vector<D3DXVECTOR3> pts;

	pts.clear();
	pts.resize (iStep);

	theta = 0.0;
	delta = 2 * D3DX_PI / float (iStep);

	for (count = 0; count < iStep; ++count)
	{
		x = fx + fRadius * cosf (theta);
		y = fy + fRadius * sinf (theta);
		z = fz + m_pHeightObserver->GetHeight (x, y) + 10.0f;

		pts[count] = D3DXVECTOR3 (x, -y, z);

		theta += delta;
	}

	for (count = 0; count < iStep - 1; ++count)
	{
		RenderLine3d (pts[count].x, pts[count].y, pts[count].z, pts[count + 1].x, pts[count + 1].y, pts[count + 1].z);
	}

	RenderLine3d (pts[iStep - 1].x, pts[iStep - 1].y, pts[iStep - 1].z, pts[0].x, pts[0].y, pts[0].z);
}

void CCursorRenderer::RenderCursorArea()
{
	SetDiffuseColor (0.0f, 1.0f, 0.0f, 1.0f);

	const D3DXVECTOR3 & c_rv3CursorPosition = GetCursorPosition();

	if (0 == m_dwCurrentCRC)
	{
		if (IsSelecting())
			// 마이너스 임시
			RenderCursorSquare (m_v3SelectStartPosition.x, -m_v3SelectStartPosition.y,
								c_rv3CursorPosition.x, -c_rv3CursorPosition.y);
	}
	else
	{
		switch (m_iObjectBrushType)
		{
			case BRUSH_TYPE_CIRCLE:
				RenderCursorCircle (c_rv3CursorPosition.x, c_rv3CursorPosition.y, c_rv3CursorPosition.z, float (m_iObjectAreaHalfSize));
				break;

			case BRUSH_TYPE_SQUARE:
				RenderCursorSquare (c_rv3CursorPosition.x - m_iObjectAreaHalfSize, c_rv3CursorPosition.y - m_iObjectAreaHalfSize,
									c_rv3CursorPosition.x + m_iObjectAreaHalfSize, c_rv3CursorPosition.y + m_iObjectAreaHalfSize);
				break;
		}
	}
}

DWORD CCursorRenderer::GetCurrentCRC()
{
	return m_dwCurrentCRC;
}

void CCursorRenderer::SetCursor (DWORD dwCRC)
{
	ClearCursor();
	m_dwCurrentCRC = dwCRC;

	if (m_dwCurrentCRC == 0)
	{
		return;
	}

	CProperty * pProperty;

	if (!CSingleton<CPropertyManager>::Instance().Get (dwCRC, &pProperty))
	{
		return;
	}

	const char* pszPropertyType;
	pProperty->GetString ("PropertyType", &pszPropertyType);

	int iLength = strlen (pszPropertyType) + 1;

	if (!strncmp ("Tree", pszPropertyType, iLength))
	{
		SetCursorTree (pProperty);
	}
	else if (!strncmp ("Building", pszPropertyType, iLength))
	{
		SetCursorBuilding (pProperty);
	}
	else if (!strncmp ("Effect", pszPropertyType, iLength))
	{
		SetCursorEffect (pProperty);
	}
	else if (!strncmp ("Ambience", pszPropertyType, iLength))
	{
		SetCursorAmbience (pProperty);
	}
	else if (!strncmp ("DungeonBlock", pszPropertyType, iLength))
	{
		SetCursorDungeonBlock (pProperty);
	}
}

void CCursorRenderer::RefreshCursor()
{
	CProperty * pProperty;
	if (!CSingleton<CPropertyManager>::Instance().Get (m_dwCurrentCRC, &pProperty))
	{
		return;
	}

	switch (m_iCursorType)
	{
		case CURSOR_TYPE_TREE:
			SetCursorTree (pProperty);
			break;
		case CURSOR_TYPE_BUILDING:
			SetCursorBuilding (pProperty);
			break;
		case CURSOR_TYPE_EFFECT:
			SetCursorEffect (pProperty);
			break;
		case CURSOR_TYPE_AMBIENCE:
			SetCursorAmbience (pProperty);
			break;
		case CURSOR_TYPE_DUNGEON_BLOCK:
			SetCursorDungeonBlock (pProperty);
			break;
	}
}

DWORD CCursorRenderer::GetCursorRotation()
{
	Tracef ("Deprecated : CCursorRenderer::GetCursorRotation()\n");
	return (DWORD) m_fRoll;
}

void CCursorRenderer::SetCursorRotation (DWORD dwRotation)
{
	Tracef ("Deprecated : CCursorRenderer::SetCursorRotation()\n");
	m_fRoll = dwRotation;
	m_BuildingInstance.SetRotation (dwRotation);
}

void CCursorRenderer::SetCursorYaw (float fYaw)
{
	while (fYaw >= 360.0f)
	{
		fYaw -= 360.0f;
	}
	while (fYaw <   0.0f)
	{
		fYaw += 360.0f;
	}
	m_fYaw = fYaw;
}

void CCursorRenderer::SetCursorPitch (float fPitch)
{
	while (fPitch >= 360.0f)
	{
		fPitch -= 360.0f;
	}
	while (fPitch <   0.0f)
	{
		fPitch += 360.0f;
	}
	m_fPitch = fPitch;
}

void CCursorRenderer::SetCursorRoll (float fRoll)
{
	while (fRoll >= 360.0f)
	{
		fRoll -= 360.0f;
	}
	while (fRoll <   0.0f)
	{
		fRoll += 360.0f;
	}
	m_fRoll = fRoll;
}

void CCursorRenderer::SetCursorScale (DWORD dwScale)
{
	m_dwAmbienceRange = dwScale;
}

DWORD CCursorRenderer::GetSettingCursorCount()
{
	return m_CursorPositionVector.size();
}

bool CCursorRenderer::GetSettingCursorPosition (DWORD dwIndex, int* px, int* py)
{
	if (dwIndex >= m_CursorPositionVector.size())
	{
		return false;
	}

	*px = m_CursorPositionVector[dwIndex].ix;
	*py = m_CursorPositionVector[dwIndex].iy;
	return true;
}

void CCursorRenderer::SetCursorTree (CProperty * pProperty)
{
	const char* pszTreeName;
	const char* pszTreeSize;
	const char* pszTreeVariance;

	if (!pProperty->GetString ("TreeFile", &pszTreeName))
	{
		return;
	}

	if (!pProperty->GetString ("TreeSize", &pszTreeSize))
	{
		return;
	}

	if (!pProperty->GetString ("TreeVariance", &pszTreeVariance))
	{
		return;
	}

	float fTreeSize = atof (pszTreeSize);
	float fTreeVariance = atof (pszTreeVariance);

	/*
	------------------------------------------------------------
	 [YOSUN_WORLDEDITOR_FIX_001] Load .spt into a memory buffer
	 first, because CSpeedTreeWrapper::LoadTree() tries the
	 in-memory overload before the filename overload. Supplying
	 a nullptr/0 triggers SpeedTree's assert, so we give it
	 real data instead.
	------------------------------------------------------------
	*/

	std::ifstream file (pszTreeName, std::ios::binary);
	if (!file)
	{
		TraceError ("[WorldEditor] Failed to open spt '%s'", pszTreeName);
		return;
	}
	std::vector<char> vSpt{ std::istreambuf_iterator<char> (file), std::istreambuf_iterator<char>() };

	m_pTreeCursor = new CSpeedTreeWrapper;
	TraceError ("[WorldEditor] LoadTree() path = %s", pszTreeName);
	if (!m_pTreeCursor->LoadTree (pszTreeName, reinterpret_cast<const BYTE*> (vSpt.data()), vSpt.size()))
	{
		delete m_pTreeCursor;
		m_pTreeCursor = nullptr;
		return;
	}
	/* ------------------------------------------------------------ */

	m_CursorPositionVector.clear();
	m_CursorPositionVector.resize (1);

	m_CursorPositionVector[0].ix = 0;
	m_CursorPositionVector[0].iy = 0;

	m_iCursorType = CURSOR_TYPE_TREE;
}

void CCursorRenderer::SetCursorEffect (CProperty * pProperty)
{
	const char* c_szFileName;

	if (!pProperty->GetString ("EffectFile", &c_szFileName))
	{
		return;
	}

	CEffectManager & rem = CEffectManager::Instance();

	std::string strFileName;
	StringPath (c_szFileName, strFileName);
	DWORD dwCRC = GetCaseCRC32 (strFileName.c_str(), strFileName.length());

	CEffectData * pData;
	if (!rem.GetEffectData (dwCRC, &pData))
	{
		if (!rem.RegisterEffect (c_szFileName))
		{
			m_dwEffectInstanceIndex = 0xffffffff;
			return;
		}
	}

	m_dwEffectInstanceIndex = rem.GetEmptyIndex();
	rem.CreateEffectInstance (m_dwEffectInstanceIndex, dwCRC);

	m_CursorPositionVector.clear();
	m_CursorPositionVector.resize (1);

	m_CursorPositionVector[0].ix = 0;
	m_CursorPositionVector[0].iy = 0;
	SetCursorYaw (m_fYaw);
	SetCursorPitch (m_fPitch);
	SetCursorRoll (m_fRoll);

	m_iCursorType = CURSOR_TYPE_EFFECT;
}

void CCursorRenderer::SetCursorAmbience (CProperty * pProperty)
{
	m_CursorPositionVector.clear();
	m_CursorPositionVector.resize (1);

	m_CursorPositionVector[0].ix = 0;
	m_CursorPositionVector[0].iy = 0;

	m_iCursorType = CURSOR_TYPE_AMBIENCE;
}

void CCursorRenderer::SetCursorDungeonBlock (CProperty * pProperty)
{
	m_CursorPositionVector.clear();
	m_CursorPositionVector.resize (1);

	m_CursorPositionVector[0].ix = 0;
	m_CursorPositionVector[0].iy = 0;

	const char* pszDungeonBlockFileName;
	if (!pProperty->GetString ("DungeonBlockFile", &pszDungeonBlockFileName))
	{
		return;
	}

	m_DungeonBlock.Destroy();
	m_DungeonBlock.Load (pszDungeonBlockFileName);

	m_iCursorType = CURSOR_TYPE_DUNGEON_BLOCK;
}

void CCursorRenderer::SetCenterPosition (D3DXVECTOR3 & rv3Position)
{
	m_v3CenterPosition = rv3Position;
}

void CCursorRenderer::SetCursorPosition (D3DXVECTOR3 & rv3Position)
{
	m_v3CursorPosition = rv3Position;
}

void CCursorRenderer::SetObjectBrushType (int iType)
{
	m_iObjectBrushType = iType;
}

void CCursorRenderer::SetObjectAreaSize (int iSize)
{
	m_iObjectAreaHalfSize = iSize;
}

void CCursorRenderer::SetObjectAreaDensity (int iDensity)
{
	m_iObjectAreaDensity = iDensity;
}

void CCursorRenderer::SetObjectAreaRandom (int iRandom)
{
	m_iObjectAreaRandom = iRandom;
}

void CCursorRenderer::SetGridMode (BOOL byGridMode)
{
	m_isGridMode = byGridMode;
}

void CCursorRenderer::SetGridDistance (float fGridDistance)
{
	m_fGridDistance = fGridDistance;
}

void CCursorRenderer::SetObjectHeight (float fHeight)
{
	m_fObjectHeight = fHeight;

	m_fObjectHeight = fMAX (m_fObjectHeight, -5000.0f);
	m_fObjectHeight = fMIN (m_fObjectHeight, +5000.0f);
}

float CCursorRenderer::GetCursorYaw()
{
	return m_fYaw;
}

float CCursorRenderer::GetCursorPitch()
{
	return m_fPitch;
}

float CCursorRenderer::GetCursorRoll()
{
	return m_fRoll;
}

float CCursorRenderer::GetObjectHeight()
{
	return m_fObjectHeight;
}

DWORD CCursorRenderer::GetCursorScale()
{
	return m_dwAmbienceRange;
}

// Building
void CCursorRenderer::SetCursorBuilding (CProperty * pProperty)
{
	const char* c_szFileName;

	if (!pProperty->GetString ("BuildingFile", &c_szFileName))
	{
		return;
	}

	CGraphicThing * pThing = (CGraphicThing*) CResourceManager::Instance().GetResourcePointer (c_szFileName);

	if (!pThing)
	{
		return;
	}

	pThing->AddReference();

	int iModelCount = pThing->GetModelCount();

	if (iModelCount)
	{
		m_BuildingInstance.Clear();
		m_BuildingInstance.ReserveModelThing (iModelCount);
		m_BuildingInstance.ReserveModelInstance (iModelCount);
		m_BuildingInstance.RegisterModelThing (0, pThing);

		for (int i = 0; i < iModelCount; ++i)
		{
			m_BuildingInstance.SetModelInstance (i, 0, i);
		}
	}

	pThing->Release();

	m_CursorPositionVector.clear();
	m_CursorPositionVector.resize (1);

	m_CursorPositionVector[0].ix = 0;
	m_CursorPositionVector[0].iy = 0;

	m_iCursorType = CURSOR_TYPE_BUILDING;

	SetCursorYaw (m_fYaw);
	SetCursorPitch (m_fPitch);
	SetCursorRoll (m_fRoll);
}
// Building

void CCursorRenderer::GetSelectArea (float* pfxStart, float* pfyStart, float* pfxEnd, float* pfyEnd)
{
	const D3DXVECTOR3 & c_rv3CursorPosition = GetCursorPosition();

	*pfxStart = fMIN (m_v3SelectStartPosition.x, c_rv3CursorPosition.x);
	*pfyStart = fMIN (m_v3SelectStartPosition.y, c_rv3CursorPosition.y);

	*pfxEnd = fMAX (m_v3SelectStartPosition.x, c_rv3CursorPosition.x);
	*pfyEnd = fMAX (m_v3SelectStartPosition.y, c_rv3CursorPosition.y);
}

bool CCursorRenderer::IsSelecting()
{
	if (0 != m_dwCurrentCRC)
	{
		return false;
	}

	return m_isSelecting;
}

void CCursorRenderer::SelectStart()
{
	if (0 != m_dwCurrentCRC)
	{
		return;
	}

	const D3DXVECTOR3 & c_rv3CursorPosition = GetCursorPosition();

	m_isSelecting = true;
	m_v3SelectStartPosition = c_rv3CursorPosition;
}

void CCursorRenderer::SelectEnd()
{
	m_isSelecting = false;
	m_v3SelectStartPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
}

bool CCursorRenderer::IsPicking()
{
	if (0 != m_dwCurrentCRC)
	{
		return false;
	}

	return m_isPicking;
}

void CCursorRenderer::PickStart()
{
	if (0 != m_dwCurrentCRC)
	{
		return;
	}

	m_isPicking = true;
	m_v3PickStartPosition = GetCursorPosition();
}

void CCursorRenderer::PickEnd()
{
	m_isPicking = false;
	m_v3PickStartPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
}

const D3DXVECTOR3& CCursorRenderer::GetPickedPosition()
{
	return m_v3PickStartPosition;
}

void CCursorRenderer::UpdatePickedPosition()
{
	m_v3PickStartPosition = GetCursorPosition();
}

void CCursorRenderer::SetHeightObserver (CMapManagerAccessor::CHeightObserver * pHeightObserver)
{
	m_pHeightObserver = pHeightObserver;
}

const D3DXVECTOR3& CCursorRenderer::GetCursorPosition()
{
	static D3DXVECTOR3 s_v3MousePosition;

	if (m_isGridMode)
	{
		s_v3MousePosition.x = m_v3CursorPosition.x - fmod (m_v3CursorPosition.x, m_fGridDistance);
		s_v3MousePosition.y = m_v3CursorPosition.y - fmod (m_v3CursorPosition.y, m_fGridDistance);
		s_v3MousePosition.z = m_v3CursorPosition.z;

		return s_v3MousePosition;
	}

	return m_v3CursorPosition;
}

void CCursorRenderer::ClearCursor()
{
	m_iCursorType = CURSOR_TYPE_NONE;
	m_dwCurrentCRC = 0;

	if (m_pTreeCursor)
	{
		delete m_pTreeCursor;
		m_pTreeCursor = NULL;
	}

	if (m_dwEffectInstanceIndex != 0xffffffff)
	{
		CEffectManager::Instance().DestroyEffectInstance (m_dwEffectInstanceIndex);
	}
	m_dwEffectInstanceIndex = 0xffffffff;
}

void CCursorRenderer::__Initialize()
{
	m_dwCurrentCRC = 0;
	m_dwEffectInstanceIndex = 0xffffffff;
	m_iCursorType = CURSOR_TYPE_NONE;
	m_fYaw = m_fPitch = m_fRoll = 0.0f;
	m_pHeightObserver = NULL;
	m_dwAmbienceRange = 0;

	m_v3CursorPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	m_v3CenterPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

	m_isSelecting = FALSE;
	m_v3SelectStartPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	m_isPicking = FALSE;
	m_v3PickStartPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

	m_iObjectBrushType = BRUSH_TYPE_CIRCLE;
	m_iObjectAreaHalfSize = 1000;
	m_iObjectAreaDensity = 9;
	m_iObjectAreaRandom = 0;

	m_isGridMode = TRUE;
	m_fGridDistance = 1000.0f;

	m_pTreeCursor = NULL;
}

CCursorRenderer::CCursorRenderer()
{
	__Initialize();
}

CCursorRenderer::~CCursorRenderer()
{
}
