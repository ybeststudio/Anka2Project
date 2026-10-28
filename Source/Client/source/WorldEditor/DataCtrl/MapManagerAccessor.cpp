#include "StdAfx.h"
#include "MapManagerAccessor.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <il/il.h>
#include <il/ilu.h>
//#include <dxtlib.h>

////////////////////////////////
// Test Code
#include "../MainFrm.h"
#include "../DockingBar/MapTreeLoader.h"
#include "../../GameLib/Property.h"
#include "../../EterPack/EterPackManager.h"
#include "ShadowRenderHelper.h"
#include "MiniMapRenderHelper.h"
#include "MapAccessorOutdoor.h"
#include "MapAccessorTerrain.h"
#include "NonPlayerCharacterInfo.h"

////////////////////////////////

float CMapManagerAccessor::CHeightObserver::GetHeight (float fx, float fy)
{
	return m_pMap->GetHeight (fx, fy);
}

//////////////////////////////////////////////////////////////////////////
CMapManagerAccessor::CMapManagerAccessor() : m_pHeightObserver (NULL)
{
	m_v3EditingCursorPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

	Initialize();
}

CMapManagerAccessor::~CMapManagerAccessor()
{
	Destroy();
}

void CMapManagerAccessor::Initialize()
{
	Clear();
	__LoadMapInfoVector();
}

void CMapManagerAccessor::Destroy()
{
	if (m_pHeightObserver)
	{
		delete m_pHeightObserver;
		m_pHeightObserver = NULL;
	}
}

void CMapManagerAccessor::DestroyShadowTexture()
{
	if (!m_pMapAccessor)
	{
		return;
	}

	m_pMapAccessor->ReleaseCharacterShadowTexture();
}

void CMapManagerAccessor::RecreateShadowTexture()
{
	if (!m_pMapAccessor)
	{
		return;
	}

	m_pMapAccessor->CreateCharacterShadowTexture();
}

void CMapManagerAccessor::Clear()
{
	m_pMapAccessor = NULL;

	m_byBrushSize = 1;
	m_byBrushSizeY = 1;
	m_byBrushStrength = 30;

	m_ixEdit = 0;
	m_iyEdit = 0;
	m_bySubCellX = 0;
	m_bySubCellY = 0;
	m_wEditTerrainNumX = 0;
	m_wEditTerrainNumY = 0;

	m_bEditingInProgress = false;
	m_bHeightEditingOn = false;
	m_bTextureEditingOn = false;
	m_bWaterEditingOn = false;
	m_bAttrEditingOn = false;
	m_bMonsterAreaInfoEditingOn = false;
	m_bEraseTexture = false;
	m_bDrawOnlyOnBlankTile = false;
	m_bEraseWater = false;
	m_bEraseAttr = false;

	m_bySelectedAttrFlag = 0;

	m_lBrushWaterHeight = 0;

	m_bShowAllMonsterAreaInfo = false;

	//////////////////////////////////////////////////////////////////////////
	m_dwNewMonsterVID = 0;
	m_dwNewMonsterCount = 0;
	m_eNewMonsterDir = CMonsterAreaInfo::DIR_NORTH;

	m_pSelectedMonsterAreaInfoVector.clear();
	m_bSelectMonsterAreaInfoStart = false;
	m_dwSelectedMonsterAreaInfoIndex = 0;
	//////////////////////////////////////////////////////////////////////////

	InitializeEnvironmentData();
	CMapManager::Clear();
}

void CMapManagerAccessor::__LoadMapInfoVector()
{
	m_kVct_kMapInfo.clear();

	CMappedFile kFile;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get (kFile, "AtlasInfo.txt", &pData))
	{
		return;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind (kFile.Size(), pData);

	char szMapName[256];
	int x, y;
	int width, height;
	int iID;
	for (UINT uLineIndex = 0; uLineIndex < textFileLoader.GetLineCount(); ++uLineIndex)
	{
		const std::string& c_rstLine = textFileLoader.GetLineString (uLineIndex);
		sscanf (c_rstLine.c_str(), "%s %d %d %d %d %d",
				szMapName,
				&x, &y, &width, &height, &iID);

		if ('\0' == szMapName[0])
		{
			continue;
		}

		TMapInfo kMapInfo;
		kMapInfo.m_strName = szMapName;
		kMapInfo.m_dwBaseX = x;
		kMapInfo.m_dwBaseY = y;

		kMapInfo.m_dwSizeX = width;
		kMapInfo.m_dwSizeY = height;

		kMapInfo.m_dwEndX = kMapInfo.m_dwBaseX + kMapInfo.m_dwSizeX * CTerrainImpl::TERRAIN_XSIZE;
		kMapInfo.m_dwEndY = kMapInfo.m_dwBaseY + kMapInfo.m_dwSizeY * CTerrainImpl::TERRAIN_YSIZE;

		m_kVct_kMapInfo.push_back (kMapInfo);
		m_kMap_strMapName_iID.insert (make_pair (std::string (szMapName), iID));
	}

	return;
}

CMapBase* CMapManagerAccessor::AllocMap()
{
	CMapBase * pMap = new CMapOutdoorAccessor;

	m_pMapAccessor = (CMapOutdoorAccessor*) pMap;
	m_pMapAccessor->SetDrawShadow (true);			// 기본값이 false 라 라이트를 안먹으므로 .. 여기서 한다.
	m_pMapAccessor->SetDrawCharacterShadow (true);	// 기본값이 false 라 라이트를 안먹으므로 .. 여기서 한다.

	m_pHeightObserver = new CHeightObserver (m_pMapAccessor);
	return pMap;
}

void CMapManagerAccessor::UpdateEditing()
{
	/////////////////////////////////////////////
	// Move To SceneMap because it's code for editing
	if (m_bEditingInProgress)
	{
		if (m_bHeightEditingOn)
		{
			EditTerrain();

			// 고쳐야 함... 심리스 되면서 EditX, EditY 만으로는 해결 안됨..
			RefreshObjectHeight (m_v3EditingCursorPosition.x,
								 m_v3EditingCursorPosition.y,
								 float (m_byBrushSize) * CTerrainImpl::CELLSCALE);
		}
		else if (m_bTextureEditingOn)
		{
			EditTerrainTexture();
		}
		else if (m_bWaterEditingOn)
		{
			EditWater();
		}
		else if (m_bAttrEditingOn)
		{
			EditAttr();
		}
		else if (m_bMonsterAreaInfoEditingOn)
		{
			EditMonsterAreaInfo();
		}
	}
	else if (m_bSelectMonsterAreaInfoStart)
	{
		SelectMonsterAreaInfo();
	}
	/////////////////////////////////////////////
}

void CMapManagerAccessor::SetBrushShape (DWORD dwShape)
{
	m_dwBrushShape = dwShape;
}

void CMapManagerAccessor::SetBrushType (DWORD dwType)
{
	m_dwBrushType = dwType;
}

void CMapManagerAccessor::SetBrushSize (unsigned char ucBrushSize)
{
	if (ucBrushSize > m_byMAXBrushSize)
	{
		m_byBrushSize = m_byMAXBrushSize;
	}
	else
	{
		m_byBrushSize = ucBrushSize;
	}
}

void CMapManagerAccessor::SetBrushSizeY (BYTE byBrushSizeY)
{
	if (byBrushSizeY > m_byMAXBrushSize)
	{
		m_byBrushSizeY = m_byMAXBrushSize;
	}
	else
	{
		m_byBrushSizeY = byBrushSizeY;
	}
}

void CMapManagerAccessor::SetMaxBrushSize (unsigned char ucMaxBrushSize)
{
	m_byMAXBrushSize = ucMaxBrushSize;
}

void CMapManagerAccessor::SetBrushStrength (unsigned char ucBrushStrength)
{
	if (ucBrushStrength < 1)
	{
		ucBrushStrength = 1;
	}
	else if (ucBrushStrength > m_byMAXBrushStrength)
	{
		ucBrushStrength = m_byMAXBrushStrength;
	}
	m_byBrushStrength = ucBrushStrength;
}

void CMapManagerAccessor::SetBrushWaterHeight (long lBrushWaterHeight)
{
	m_lBrushWaterHeight = lBrushWaterHeight;
}

void CMapManagerAccessor::SetTextureBrushVector (std::vector<BYTE>& rTextureBrushNumberVector)
{
	if (rTextureBrushNumberVector.empty())
	{
		m_TextureBrushNumberVector.clear();
		return;
	}

	m_TextureBrushNumberVector.resize (rTextureBrushNumberVector.size());

	std::ranges::copy (rTextureBrushNumberVector, m_TextureBrushNumberVector.begin());
}

void CMapManagerAccessor::SetInitTextureBrushVector (std::vector<BYTE>& rTextureBrushNumberVector)
{
	if (rTextureBrushNumberVector.empty())
	{
		m_InitTextureBrushNumberVector.clear();
		return;
	}

	m_InitTextureBrushNumberVector.resize (rTextureBrushNumberVector.size());

	std::ranges::copy (rTextureBrushNumberVector, m_InitTextureBrushNumberVector.begin());
}

bool CMapManagerAccessor::InitBaseTexture (const char* c_szMapName)
{
	if (m_InitTextureBrushNumberVector.empty())
	{
		return false;
	}

	std::string strMapFolder;

	if (nullptr == c_szMapName)
	{
		if ("" == m_pMapAccessor->GetName())
		{
			LogBox ("[ERROR] InitBaseTexture: Save failed. No file name provided.");
			return false;
		}

		strMapFolder = m_pMapAccessor->GetName();
	}
	else
	{
		strMapFolder = c_szMapName;
	}

	struct _stat sb;

	if (0 != _stat (strMapFolder.c_str(), &sb))
	{
		LogBoxf ("[ERROR] InitBaseTexture: Save failed: Directory does not exist. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	if (! (sb.st_mode & _S_IFDIR))
	{
		LogBoxf ("[ERROR] InitBaseTexture: Save failed: A file with the same name as directory exists. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	if (!m_pMapAccessor->RAW_InitBaseTexture (m_InitTextureBrushNumberVector))
	{
		LogBoxf ("[ERROR] InitBaseTexture: Failed to initialize default texture. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	LogBoxf ("[SUCCESS] InitBaseTexture: Default texture initialization complete. Dir: [%s].", strMapFolder.c_str());

	return true;
}

void CMapManagerAccessor::SetMaxBrushStrength (unsigned char ucMaxBrushStrength)
{
	m_byMAXBrushStrength = ucMaxBrushStrength;
	m_pMapAccessor->SetMaxBrushStrength (m_byMAXBrushStrength);
}

void CMapManagerAccessor::EditTerrain()
{
	m_pMapAccessor->DrawHeightBrush (m_dwBrushShape, m_dwBrushType, m_wEditTerrainNumX, m_wEditTerrainNumY, m_ixEdit, m_iyEdit, m_byBrushSize, m_byBrushStrength);
}

void CMapManagerAccessor::EditingStart()
{
	m_bEditingInProgress = true;
}

void CMapManagerAccessor::EditingEnd()
{
	m_bEditingInProgress = false;
}

void CMapManagerAccessor::EditTerrainTexture()
{
	m_pMapAccessor->DrawTextureBrush (m_dwBrushShape, m_TextureBrushNumberVector, m_wEditTerrainNumX, m_wEditTerrainNumY, m_ixEdit, m_iyEdit, m_bySubCellX, m_bySubCellY,
									  m_byBrushSize, m_bEraseTexture, m_bDrawOnlyOnBlankTile);
}

void CMapManagerAccessor::EditWater()
{
	m_pMapAccessor->DrawWaterBrush (m_dwBrushShape, m_wEditTerrainNumX, m_wEditTerrainNumY, m_ixEdit, m_iyEdit, m_byBrushSize, m_lBrushWaterHeight, m_bEraseWater);
}

void CMapManagerAccessor::PreviewEditWater()
{
	m_pMapAccessor->PreviewWaterBrush (m_dwBrushShape, m_wEditTerrainNumX, m_wEditTerrainNumY, m_ixEdit, m_iyEdit, m_byBrushSize, m_lBrushWaterHeight, m_bEraseWater);
}

void CMapManagerAccessor::EditAttr()
{
	m_pMapAccessor->DrawAttrBrush (m_dwBrushShape, m_bySelectedAttrFlag, m_wEditTerrainNumX, m_wEditTerrainNumY, m_ixEdit, m_iyEdit, m_bySubCellX, m_bySubCellY, m_byBrushSize,
								   m_bEraseAttr);
}

void CMapManagerAccessor::EditMonsterAreaInfo()
{
	long lOriginX, lOriginY, lSizeX, lSizeY;
	lOriginX = (m_wEditTerrainNumX * CTerrainImpl::XSIZE + m_ixEdit) * CTerrainImpl::CELLSCALE_IN_METER + m_bySubCellX * CTerrainImpl::HALF_CELLSCALE_IN_METER;
	lOriginY = (m_wEditTerrainNumY * CTerrainImpl::YSIZE + m_iyEdit) * CTerrainImpl::CELLSCALE_IN_METER + m_bySubCellY * CTerrainImpl::HALF_CELLSCALE_IN_METER;
	lSizeX = m_byBrushSize * CTerrainImpl::CELLSCALE_IN_METER;
	lSizeY = m_byBrushSizeY * CTerrainImpl::CELLSCALE_IN_METER;

	CMonsterAreaInfo * pMonsterAreaInfo = AddNewMonsterAreaInfo (lOriginX, lOriginY, lSizeX, lSizeY, m_eNewMonsterAreaInfoType, m_dwNewMonsterVID, m_dwNewMonsterCount,
																 m_eNewMonsterDir);

	if (!pMonsterAreaInfo)
	{
		return;
	}

	CNonPlayerCharacterInfo & rNPCInfo = CNonPlayerCharacterInfo::Instance();
	switch (pMonsterAreaInfo->GetMonsterAreaInfoType())
	{
		case CMonsterAreaInfo::MONSTERAREAINFOTYPE_MONSTER :
		{
			std::string strMonsterName = rNPCInfo.GetNameByVID (pMonsterAreaInfo->GetMonsterVID());
			pMonsterAreaInfo->SetMonsterName (strMonsterName);
		}
		break;
		case CMonsterAreaInfo::MONSTERAREAINFOTYPE_GROUP :
		{
			std::string strMonsterGroupName = rNPCInfo.GetNPCGroupNameByGroupID (pMonsterAreaInfo->GetMonsterGroupID());
			const CNonPlayerCharacterInfo::SNPCGroup * pNPCGroup = rNPCInfo.GetGroup (pMonsterAreaInfo->GetMonsterGroupID());
			std::string strMonsterGroupLeaderName = rNPCInfo.GetNameByVID (pNPCGroup->m_dwLeaderID);
			pMonsterAreaInfo->SetMonsterGroupName (strMonsterGroupName);
			pMonsterAreaInfo->SetMonsterGroupLeaderName (strMonsterGroupLeaderName);
			pMonsterAreaInfo->SetMonsterGroupFollowerCount (rNPCInfo.GetNPCGroupFollowerCountByGroupID (pMonsterAreaInfo->GetMonsterGroupID()));
		}
		break;
	}

	EditingEnd();
}

void CMapManagerAccessor::UpdateHeightFieldEditingPt (D3DXVECTOR3 * v3IntersectPt)
{
	m_pMapAccessor->GetPickingCoordinate (v3IntersectPt, &m_ixEdit, &m_iyEdit, &m_bySubCellX, &m_bySubCellY, &m_wEditTerrainNumX, &m_wEditTerrainNumY);
}

CMapManagerAccessor::CHeightObserver* CMapManagerAccessor::GetHeightObserverPointer()
{
	return m_pHeightObserver;
}

void CMapManagerAccessor::InsertObject (float fx, float fy, float fz, int iRotation, DWORD dwCRC)
{
	Tracef ("Deprecated : CMapManagerAccessor::InsertObject (x,y,z,rotation,crc)\n");
	InsertObject (fx, fy, fz, 0, 0, iRotation, 0, dwCRC);
}

void CMapManagerAccessor::InsertObject (float fx, float fy, float fz, float fYaw, float fPitch, float fRoll, DWORD dwScale, DWORD dwCRC)
{
	WORD wIndex = GetTerrainNum (fx, fy);

	CTerrainAccessor * pTerrainAccessor;
	if (!GetTerrain (wIndex, &pTerrainAccessor))
	{
		return;
	}

	CAreaAccessor * pAreaAccessor;
	if (!GetArea (wIndex, &pAreaAccessor))
	{
		return;
	}

	CProperty * pProperty;
	if (!CPropertyManager::Instance().Get (dwCRC, &pProperty))
	{
		return;
	}

	const char* c_szPropertyType;
	if (!pProperty->GetString ("PropertyType", &c_szPropertyType))
	{
		return;
	}

	CArea::TObjectData ObjectData;
	ObjectData.Position.x = fx;
	ObjectData.Position.y = fy;

	ObjectData.Position.z = pTerrainAccessor->GetHeight ((int)fx, - (int)fy);
	ObjectData.dwCRC = dwCRC;
	while (fYaw   >= 360.0f)
	{
		fYaw -= 360.0f;
	}
	while (fYaw   <    0.0f)
	{
		fYaw += 360.0f;
	}
	while (fPitch >= 360.0f)
	{
		fPitch -= 360.0f;
	}
	while (fPitch <    0.0f)
	{
		fPitch += 360.0f;
	}
	while (fRoll  >= 360.0f)
	{
		fRoll -= 360.0f;
	}
	while (fRoll  <    0.0f)
	{
		fRoll += 360.0f;
	}
	ObjectData.m_fYaw = fYaw;
	ObjectData.m_fPitch = fPitch;
	ObjectData.m_fRoll = fRoll;
	ObjectData.m_fHeightBias = fz;
	ObjectData.dwRange = 0;
	ObjectData.fMaxVolumeAreaPercentage = 0.0f;
	ZeroMemory (ObjectData.abyPortalID, sizeof (ObjectData.abyPortalID));

	if (prt::PROPERTY_TYPE_AMBIENCE == prt::GetPropertyType (c_szPropertyType))
	{
		ObjectData.dwRange = dwScale;
		ObjectData.fMaxVolumeAreaPercentage = 0.0f;

		const char* c_szPercentage;
		if (pProperty->GetString ("MaxVolumeAreaPercentage", &c_szPercentage))
		{
			ObjectData.fMaxVolumeAreaPercentage = atof (c_szPercentage);
		}
	}

	pAreaAccessor->AddObject (&ObjectData);
}

void CMapManagerAccessor::RefreshObjectHeight (float fx, float fy, float fHalfSize)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pAreaAccessor;
		if (!GetArea (i, &pAreaAccessor))
		{
			continue;
		}

		DWORD dwObjectCount = pAreaAccessor->GetObjectDataCount();
		for (DWORD j = 0; j < dwObjectCount; ++j)
		{
			CArea::TObjectData * pObjectData;
			if (!pAreaAccessor->GetObjectPointer (j, &pObjectData))
			{
				continue;
			}

			// Dungeon Block 은 높이를 수정하지 않는다.
			CProperty * pProperty;
			if (CPropertyManager::Instance().Get (pObjectData->dwCRC, &pProperty))
			{
				const char* c_szPropertyType;
				if (pProperty->GetString ("PropertyType", &c_szPropertyType))
				{
					if (prt::PROPERTY_TYPE_DUNGEON_BLOCK == prt::GetPropertyType (c_szPropertyType))
					{
						continue;
					}
				}
			}

			float fxDistance = fabs (pObjectData->Position.x - fx);
			float fyDistance = fabs (pObjectData->Position.y - fy);
			if (fxDistance < fHalfSize && fyDistance < fHalfSize)
			{
				pObjectData->Position.z = m_pMapAccessor->GetTerrainHeight (pObjectData->Position.x, -pObjectData->Position.y);
				pAreaAccessor->UpdateObject (j, pObjectData);
			}
		}
	}
}

bool CMapManagerAccessor::AddTerrainTexture (const char* pFilename)
{
	CTextureSet * pTextureSet = CTerrain::GetTextureSet();

	if (!pTextureSet->AddTexture (pFilename, 4.0f, 4.0f, 0.0f, 0.0f, true, 0, 0, 1.0f / (float) (CTerrainImpl::PATCH_XSIZE * CTerrainImpl::CELLSCALE)))
	{
		return false;
	}

	ResetTerrainTexture();
	return true;
}

bool CMapManagerAccessor::RemoveTerrainTexture (long lTexNum)
{
	CTextureSet * pTextureSet = CTerrain::GetTextureSet();

	if (!pTextureSet->RemoveTexture (lTexNum))
	{
		LogBox ("[ERROR] RemoveTerrainTexture: Cannot delete texture from the texture set.");
		return false;
	}

	ResetTerrainTexture();
	return true;
}

void CMapManagerAccessor::ResetTerrainTexture()
{
	m_pMapAccessor->ResetTextures();
}

void CMapManagerAccessor::RenderSelectedObject()
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->RenderSelectedObject();
	}
}

void CMapManagerAccessor::CancelSelect()
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->CancelSelect();
	}
}

BOOL CMapManagerAccessor::IsSelected()
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		if (pArea->IsSelected())
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CMapManagerAccessor::SelectObject (float fxStart, float fyStart, float fxEnd, float fyEnd)
{
	BOOL bNeedChange = FALSE;

	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		bNeedChange = pArea->SelectObject (fxStart, fyStart, fxEnd, fyEnd);
	}

	return bNeedChange;
}

void CMapManagerAccessor::DeleteSelectedObject()
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->DeleteSelectedObject();
	}
}

void CMapManagerAccessor::MoveSelectedObject (float fx, float fy)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->MoveSelectedObject (fx, fy);
	}
}

void CMapManagerAccessor::MoveSelectedObjectHeight (float fHeight)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->MoveSelectedObjectHeight (fHeight);
	}
}

void CMapManagerAccessor::AddSelectedAmbienceScale (int iAddScale)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->AddSelectedAmbienceScale (iAddScale);
	}
}

void CMapManagerAccessor::AddSelectedAmbienceMaxVolumeAreaPercentage (float fPercentage)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->AddSelectedAmbienceMaxVolumeAreaPercentage (fPercentage);
	}
}

void CMapManagerAccessor::AddSelectedObjectRotation (float fYaw, float fPitch, float fRoll)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->AddSelectedObjectRotation (fYaw, fPitch, fRoll);
	}
}

void CMapManagerAccessor::SetSelectedObjectPortalNumber (int iID)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->SetSelectedObjectPortalNumber (iID);
	}
}

void CMapManagerAccessor::DelSelectedObjectPortalNumber (int iID)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->DelSelectedObjectPortalNumber (iID);
	}
}

void CMapManagerAccessor::CollectPortalNumber (std::set<int>* pkSet_iPortalNumber)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->CollectPortalNumber (pkSet_iPortalNumber);
	}
}

void CMapManagerAccessor::EnablePortal (BOOL bFlag)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CAreaAccessor * pArea;
		if (!GetArea (i, &pArea))
		{
			continue;
		}

		pArea->EnablePortal (bFlag);
	}
}

int CMapManagerAccessor::GetPickedPickedObjectIndex()
{
	CAreaAccessor * pAreaAccessor;
	if (!m_pMapAccessor->GetAreaAccessor (4, &pAreaAccessor))
	{
		return -1;
	}

	return pAreaAccessor->GetPickedObjectIndex();
}

BOOL CMapManagerAccessor::IsSelectedObject (int iIndex)
{
	CAreaAccessor * pAreaAccessor;
	if (!m_pMapAccessor->GetAreaAccessor (4, &pAreaAccessor))
	{
		return -1;
	}

	return pAreaAccessor->IsSelectedObject (iIndex);
}

int CMapManagerAccessor::GetSelectedObjectCount()
{
	CAreaAccessor * pAreaAccessor;
	if (!m_pMapAccessor->GetAreaAccessor (4, &pAreaAccessor))
	{
		return 0;
	}

	return pAreaAccessor->GetSelectedObjectCount();
}

void CMapManagerAccessor::SelectObject (int iIndex)
{
	CAreaAccessor * pAreaAccessor;
	if (!m_pMapAccessor->GetAreaAccessor (4, &pAreaAccessor))
	{
		return;
	}

	pAreaAccessor->SelectObject (iIndex);
}

BOOL CMapManagerAccessor::Picking()
{
	CAreaAccessor * pAreaAccessor;
	if (!m_pMapAccessor->GetAreaAccessor (4, &pAreaAccessor))
	{
		return FALSE;
	}

	return pAreaAccessor->Picking();
}

void CMapManagerAccessor::LoadProperty (CPropertyTreeControler * pTreeControler)
{
	CTreeLoader TreeLoader;

	TreeLoader.SetPropertyManager (&m_PropertyManager);
	TreeLoader.SetTreeControler (pTreeControler);
	TreeLoader.Create ("*.*", "Property");

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->UpdateMapControlBar();
}

void CMapManagerAccessor::SaveCollisionData (const char* c_szFileName)
{
	short sTerrainCountX, sTerrainCountY;
	m_pMapAccessor->GetTerrainCount (&sTerrainCountX, &sTerrainCountY);

	FILE * File = fopen (c_szFileName, "wb");
	if (!File)
	{
		return;
	}
	DWORD dwFourCC = MAKEFOURCC ('M', '2', 'C', 'D');
	fwrite (&dwFourCC, 1, sizeof (DWORD), File);
	fwrite (&sTerrainCountX, 1, sizeof (short), File);
	fwrite (&sTerrainCountY, 1, sizeof (short), File);

	CAreaAccessor::OpenCollisionDataCountMapLog();

	char szFileName[64 + 1];
	for (short x = 0; x < sTerrainCountX; ++x)
		for (short y = 0; y < sTerrainCountY; ++y)
		{
			DWORD dwID = DWORD (x) * 1000L + DWORD (y);
			_snprintf (szFileName, 64, "%s\\%06u\\AreaData.txt", m_pMapAccessor->GetName().c_str(), dwID);

			CAreaAccessor AreaAccessor;
			AreaAccessor.SaveCollisionData (szFileName, File);
		}

	CAreaAccessor::CloseCollisionDataCountMapLog();

	fclose (File);

	FILE * FileRead = fopen (c_szFileName, "rb");

	if (FileRead)
	{
		WORD sTerrainCountXRead, sTerrainCountYRead;

		fread (&dwFourCC, sizeof (DWORD), 1, FileRead);
		fread (&sTerrainCountXRead, sizeof (WORD), 1, FileRead);
		fread (&sTerrainCountYRead, sizeof (WORD), 1, FileRead);

		for (DWORD x = 0; x < sTerrainCountXRead; ++x)
			for (DWORD y = 0; y < sTerrainCountYRead; ++y)
			{
				DWORD dwCount;
				fread (&dwCount, sizeof (DWORD), 1, File);

				if (dwCount)
				{
					Tracef (" >>> %d, %d : %d\n", x, y, dwCount);
				}

				for (DWORD j = 0; j < dwCount; ++j)
				{
					BYTE byType;
					fread (&byType, sizeof (BYTE), 1, File);

					switch (byType)
					{
						case COLLISION_TYPE_PLANE:
						{
							TPlaneData PlaneData;
							fread (&PlaneData, sizeof (TPlaneData), 1, FileRead);
						}
						break;

						case COLLISION_TYPE_SPHERE:
						{
							TSphereData SphereData;
							fread (&SphereData, sizeof (TSphereData), 1, FileRead);
						}
						break;

						case COLLISION_TYPE_CYLINDER:
						{
							TCylinderData CylinderData;
							fread (&CylinderData, sizeof (TCylinderData), 1, FileRead);
						}
						break;
					}
				}
			}
	}
}

void CMapManagerAccessor::SetSelectedAttrFlag (BYTE bFlag)
{
	m_bySelectedAttrFlag = bFlag;
	m_pMapAccessor->ResetAttrSplats();
}

void CMapManagerAccessor::GetEditArea (int* iEditX, int* iEditY, unsigned char* wSubEditX, unsigned char* wSubEditY, unsigned short* wTerrainNumX,
									   unsigned short* wTerrainNumY)
{
	*iEditX = m_ixEdit;
	*iEditY = m_iyEdit;
	*wSubEditX = m_bySubCellX;
	*wSubEditY = m_bySubCellY;
	*wTerrainNumX = m_wEditTerrainNumX;
	*wTerrainNumY = m_wEditTerrainNumY;
}

bool CMapManagerAccessor::NewMap (const char* c_szMapName)
{
	std::string strMapFolder;
	strMapFolder = c_szMapName;

	struct _stat sb;

	if (0 != _stat (strMapFolder.c_str(), &sb))
	{
		// Make The Directory
		if (!CreateDirectory (strMapFolder.c_str(), NULL))
		{
			LogBoxf ("[ERROR] NewMap: Save failed: Cannot create directory. Dir: [%s].", strMapFolder.c_str());
			return false;
		}
	}

	if (0 != _stat (strMapFolder.c_str(), &sb))
	{
		LogBoxf ("[ERROR] NewMap: Save failed: Disk error.", strMapFolder.c_str());
		return false;
	}

	if (! (sb.st_mode & _S_IFDIR))
	{
		LogBoxf ("[ERROR] NewMap: Save failed: A file with the same name as directory exists. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	// Map Property
	if (!SaveMapProperty (strMapFolder))
	{
		LogBoxf ("[ERROR] NewMap: Failed to save property file. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	// Map Setting
	if (!SaveMapSetting (strMapFolder))
	{
		LogBoxf ("[ERROR] NewMap: Failed to save setting file. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	return true;
}

bool CMapManagerAccessor::SaveMap (const char* c_szMapName)
{
	std::string strMapFolder;

	if (NULL == c_szMapName)
	{
		if ("" == m_pMapAccessor->GetName())
		{
			LogBox ("[ERROR] SaveMap: Save failed: No file name provided");
			return false;
		}

		strMapFolder = m_pMapAccessor->GetName();
	}
	else
	{
		strMapFolder = c_szMapName;
	}

	struct _stat sb;

	if (0 != _stat (strMapFolder.c_str(), &sb))
	{
		LogBoxf ("[ERROR] SaveMap: Save failed: Directory does not exist. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	if (! (sb.st_mode & _S_IFDIR))
	{
		LogBoxf ("[ERROR] SaveMap: Save failed: A file with the same name as directory exists. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	// Map Property
	if (!SaveMapProperty (strMapFolder))
	{
		LogBoxf ("[ERROR] SaveMap: Failed to save property file. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	// Map Setting
	if (!SaveMapSetting (strMapFolder))
	{
		LogBoxf ("[ERROR] SaveMap: Failed to save setting file. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	if (!SaveTerrains())
	{
		LogBoxf ("[ERROR] SaveMap: Failed to save terrain files. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	if (!SaveAreas())
	{
		LogBoxf ("[ERROR] SaveMap: Failed to save area files. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	if (!SaveMonsterAreaInfo())
	{
		LogBoxf ("[ERROR] SaveMap: Failed to save MonsterAreaInfo file. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	LogBoxf ("[SUCCESS] SaveMap: Save complete. Dir: [%s]", strMapFolder.c_str());
	return true;
}

bool CMapManagerAccessor::SaveMapProperty (const std::string & c_rstrFolder)
{
	return m_pMapAccessor->SaveProperty (c_rstrFolder);
}


bool CMapManagerAccessor::SaveMapSetting (const std::string & c_rstrFolder)
{
	return m_pMapAccessor->SaveSetting (c_rstrFolder);
}

bool CMapManagerAccessor::SaveTerrains()
{
	return m_pMapAccessor->SaveTerrains();
}

bool CMapManagerAccessor::SaveAreas()
{
	return m_pMapAccessor->SaveAreas();
}

bool CMapManagerAccessor::CreateNewOutdoorMap()
{
	if ("" == m_strNewMapName || 0 == m_wNewMapSizeX || 0 == m_wNewMapSizeY)
	{
		LogBoxf ("[ERROR] CreateNewOutdoorMap: Cannot create map. Name: [%s] Size: [%u, %u].", m_strNewMapName.c_str(), m_wNewMapSizeX, m_wNewMapSizeY);
		return false;
	}

	struct _stat sb;

	if (0 != _stat (m_strNewMapName.c_str(), &sb))
	{
		// Make The Directory
		if (!CreateDirectory (m_strNewMapName.c_str(), NULL))
		{
			LogBoxf ("[ERROR] CreateNewOutdoorMap: Save failed: Cannot create directory. Dir: [%s].", m_strNewMapName.c_str());
			return false;
		}

		if (0 != _stat (m_strNewMapName.c_str(), &sb))
		{
			LogBoxf ("[ERROR] CreateNewOutdoorMap: Save failed: Disk error. Dir: [%s].", m_strNewMapName.c_str());
			return false;
		}

		if (! (sb.st_mode & _S_IFDIR))
		{
			LogBoxf ("[ERROR] CreateNewOutdoorMap: Save failed: A file with the same name as directory exists. Dir: [%s].", m_strNewMapName.c_str());
			return false;
		}
	}
	else
	{
		char szWarningMessage[1024];
		sprintf (szWarningMessage, "Directory already exists. Dir: [%s].\n"
								   "If you overwrite, all overlapping parts of the map previously saved in [%s] will be deleted.\n"
								   "Do you want to overwrite it?",
				 m_strNewMapName.c_str(), m_strNewMapName.c_str());
		if (IDCANCEL == MessageBox (AfxGetMainWnd()->GetSafeHwnd(), szWarningMessage, "Overwrite Warning", MB_OKCANCEL))
		{
			return false;
		}
	}

	CMapOutdoorAccessor * pNewMap = new CMapOutdoorAccessor;

	pNewMap->SetType (CMapBase::MAPTYPE_OUTDOOR);
	pNewMap->SetTerrainCount (m_wNewMapSizeX, m_wNewMapSizeY);
	pNewMap->SetName (m_strNewMapName);

	if (!pNewMap->SaveProperty (m_strNewMapName))
	{
		LogBoxf ("[ERROR] CreateNewOutdoorMap: Cannot save the property. Dir: [%s].", m_strNewMapName.c_str());
		return false;
	}

	if (!pNewMap->SaveSetting (m_strNewMapName))
	{
		LogBoxf ("[ERROR] CreateNewOutdoorMap: Cannot save the setting. Dir: [%s].", m_strNewMapName.c_str());
		return false;
	}

	for (unsigned short usY = 0; usY < m_wNewMapSizeY; ++usY)
	{
		for (unsigned short usX = 0; usX < m_wNewMapSizeX; ++usX)
		{
			if (!pNewMap->CreateNewTerrainFiles (usX, usY))
			{
				return false;
			}
		}
	}

	LogBoxf ("[SUCCESS] CreateNewOutdoorMap: New Map has been created. Name: [%s].", m_strNewMapName.c_str());
	return true;
}

BOOL CMapManagerAccessor::GetEditArea (CAreaAccessor** ppAreaAccessor)
{
	unsigned char ucAreaNum = (m_wEditTerrainNumY - m_pMapAccessor->GetCurCoordinate().m_sTerrainCoordY + 1) * 3 +
							  (m_wEditTerrainNumX - m_pMapAccessor->GetCurCoordinate().m_sTerrainCoordX + 1);

	if (ucAreaNum < 0 || ucAreaNum > AROUND_AREA_NUM - 1)
	{
		return FALSE;
	}

	CArea * pArea;
	if (!m_pMapAccessor->GetAreaPointer (ucAreaNum, &pArea))
	{
		return FALSE;
	}

	*ppAreaAccessor = dynamic_cast<CAreaAccessor*> (pArea);

	return TRUE;
}

BOOL CMapManagerAccessor::GetArea (const unsigned char& c_ucAreaNum, CAreaAccessor** ppAreaAccessor)
{
	if (c_ucAreaNum < 0 || c_ucAreaNum > AROUND_AREA_NUM - 1)
	{
		return FALSE;
	}

	return m_pMapAccessor->GetAreaPointer (c_ucAreaNum, (CArea**) &*ppAreaAccessor);
}

BOOL CMapManagerAccessor::GetEditTerrain (CTerrainAccessor** ppTerrainAccessor)
{
	unsigned char ucTerrainNum = (m_wEditTerrainNumY - m_pMapAccessor->GetCurCoordinate().m_sTerrainCoordY + 1) * 3 +
								 (m_wEditTerrainNumX - m_pMapAccessor->GetCurCoordinate().m_sTerrainCoordX + 1);
	return GetTerrain (ucTerrainNum, ppTerrainAccessor);
}

BOOL CMapManagerAccessor::GetTerrain (const unsigned char& c_ucTerrainNum, CTerrainAccessor** ppTerrainAccessor)
{
	if (c_ucTerrainNum < 0 || c_ucTerrainNum > AROUND_AREA_NUM - 1)
	{
		return FALSE;
	}

	return m_pMapAccessor->GetTerrainPointer (c_ucTerrainNum, (CTerrain**) ppTerrainAccessor);
}

const unsigned char CMapManagerAccessor::GetTerrainNum (float fx, float fy)
{
	BYTE byTerrainNum;
	m_pMapAccessor->GetTerrainNum (fx, fy, &byTerrainNum);
	return byTerrainNum;
}

const unsigned char CMapManagerAccessor::GetEditTerrainNum()
{
	return (m_wEditTerrainNumY - m_pMapAccessor->GetCurCoordinate().m_sTerrainCoordY + 1) * 3 +
		   (m_wEditTerrainNumX - m_pMapAccessor->GetCurCoordinate().m_sTerrainCoordX + 1);
}

void CMapManagerAccessor::RefreshArea()
{
	CAreaAccessor * pArea;
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		if (!m_pMapAccessor->GetAreaPointer (i, (CArea**)&pArea))
		{
			continue;
		}

		pArea->RefreshArea();
	}
}

void CMapManagerAccessor::RenderAttr()
{
	m_pMapAccessor->RenderAccessorTerrain (CMapOutdoorAccessor::RENDER_ATTR, m_bySelectedAttrFlag);
}

bool CMapManagerAccessor::ResetToDefaultAttr()
{
	std::string strMapFolder = m_pMapAccessor->GetName();

	struct _stat sb;

	if (0 != _stat (strMapFolder.c_str(), &sb))
	{
		LogBoxf ("[ERROR] ResetToDefaultAttr: Directory does not exist. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	if (! (sb.st_mode & _S_IFDIR))
	{
		LogBoxf ("[ERROR] ResetToDefaultAttr: Save failed: A file with the same name as directory exists. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	if (!m_pMapAccessor->ResetToDefaultAttr())
	{
		LogBoxf ("[ERROR] ResetToDefaultAttr: Attribute map initialization failed. Dir: [%s].", strMapFolder.c_str());
		return false;
	}

	LogBoxf ("[SUCCESS] ResetToDefaultAttr: Attribute map initialization complete. Dir: [%s].", strMapFolder.c_str());
	return true;
}

void CMapManagerAccessor::RenderObjectCollision()
{
	if (m_pMapAccessor)
	{
		m_pMapAccessor->RenderObjectCollision();
	}
}

void CMapManagerAccessor::SetTerrainModified()
{
	if (m_pMapAccessor)
	{
		m_pMapAccessor->SetTerrainModified();
	}
}

void CMapManagerAccessor::SetWireframe (bool isWireframe)
{
	if (m_pMapAccessor)
	{
		m_pMapAccessor->SetWireframe (isWireframe);
	}
}

void CMapManagerAccessor::ToggleWireframe()
{
	if (m_pMapAccessor)
	{
		m_pMapAccessor->SetWireframe (!m_pMapAccessor->IsWireframe());
	}
}

void CMapManagerAccessor::SaveMiniMap()
{
	if (!IsMapReady())
	{
		return;
	}

	CTerrainAccessor * pTerrainAccessor;
	if (!m_pMapAccessor->GetTerrainPointer (4, (CTerrain**) &pTerrainAccessor))
	{
		return;
	}

	CMiniMapRenderHelper::Instance().CreateTextures();

	CMiniMapRenderHelper::Instance().SetMapOutdoorAccssorPointer (m_pMapAccessor);

	WORD wCoordX, wCoordY;
	pTerrainAccessor->GetCoordinate (&wCoordX, &wCoordY);
	CMiniMapRenderHelper::Instance().SetTargetTerrainCoord (wCoordX, wCoordY);

	if (CMiniMapRenderHelper::Instance().StartRendering())
	{
		m_pMapAccessor->RenderMiniMap();
	}
	else
	{
		Trace ("CSceneMap::SaveMiniMap() Rendering Failed...\n");
	}

	CMiniMapRenderHelper::Instance().EndRendering();

	pTerrainAccessor->SaveMiniMapFromD3DTexture9 (m_pMapAccessor->GetName(), CMiniMapRenderHelper::Instance().GetMiniMapTexture());

	CMiniMapRenderHelper::Instance().ReleaseTextures();
}

void CMapManagerAccessor::SaveAtlas()
{
	short sCountX, sCountY;
	m_pMapAccessor->GetTerrainCount (&sCountX, &sCountY);

	char szFileName[256];
	sprintf (szFileName, "%s_atlas.bmp", m_pMapAccessor->GetName().c_str());
	DeleteFile (szFileName);

	ilInit();
	ilEnable (IL_FILE_OVERWRITE);

	ILuint dstImage;
	ilGenImages (1, &dstImage);
	ilBindImage (dstImage);

	ilClearImage();

	ilConvertImage (IL_RGB, IL_BYTE);
	iluScale (256 * sCountX, 256 * sCountY, 1);

	ilSaveImage (szFileName);
	ilLoadImage (szFileName);
	ILint iImageDepth = ilGetInteger (IL_IMAGE_BYTES_PER_PIXEL);
	ILint iImageFormat = ilGetInteger (IL_IMAGE_FORMAT);
	ILint iImageType = ilGetInteger (IL_IMAGE_TYPE);

	for (short sY = 0; sY < sCountY; ++sY)
	{
		for (short sX = 0; sX < sCountX; ++sX)
		{
			DWORD wID = (DWORD) (sX) * 1000L + (DWORD) (sY);
			char szDDSFileName[MAX_PATH + 1];
			_snprintf (szDDSFileName, MAX_PATH, "%s\\%06u\\minimap.dds", m_pMapAccessor->GetName().c_str(), wID);

			CGraphicImageInstance aMiniMapImageInstance;
			aMiniMapImageInstance.SetImagePointer ((CGraphicImage*) CResourceManager::Instance().GetResourcePointer (szDDSFileName));
			if (aMiniMapImageInstance.IsEmpty())
			{
				continue;
			}
			sprintf (szFileName, "%s_minimap_%06u.bmp", m_pMapAccessor->GetName().c_str(), wID);
			D3DXSaveTextureToFile (szFileName, D3DXIFF_BMP, aMiniMapImageInstance.GetTexturePointer()->GetD3DTexture(), NULL);

			ILuint srcImage;
			ilGenImages (2, &srcImage);
			ilBindImage (srcImage);

			ilLoadImage (szFileName);
			iluFlipImage();	// BMP는 이미지가 거꾸로 되어 있다.

			ILubyte * pData = ilGetData();
			ILint iImageWidth = ilGetInteger (IL_IMAGE_WIDTH);
			ILint iImageHeight = ilGetInteger (IL_IMAGE_HEIGHT);

			ilBindImage (dstImage);
			ilSetPixels ((ILint) sX * 256, (ILint) sY * 256, 0, iImageWidth, iImageHeight, iImageDepth, iImageFormat, iImageType, pData);

			ilDeleteImages (2, &srcImage);
			DeleteFile (szFileName);
		}
	}

	sprintf (szFileName, "%s_atlas.bmp", m_pMapAccessor->GetName().c_str());
	iluFlipImage();	// BMP는 이미지가 거꾸로 되어 있다.
	ilSaveImage (szFileName);

	ilDeleteImages (1, &dstImage);

	sprintf (szFileName, "%s_MAI_atlas.bmp", m_pMapAccessor->GetName().c_str());
	DeleteFile (szFileName);

	ilGenImages (1, &dstImage);
	ilBindImage (dstImage);

	ilClearImage();

	ilConvertImage (IL_RGB, IL_BYTE);
	iluScale (1024 * sCountX, 1024 * sCountY, 1);

	ilSaveImage (szFileName);
	ilLoadImage (szFileName);
	iImageDepth = ilGetInteger (IL_IMAGE_BYTES_PER_PIXEL);
	iImageFormat = ilGetInteger (IL_IMAGE_FORMAT);
	iImageType = ilGetInteger (IL_IMAGE_TYPE);

	for (short iy = 0; iy < sCountY; ++iy)
	{
		for (short ix = 0; ix < sCountX; ++ix)
		{
			DWORD wID = (DWORD) (ix) * 1000L + (DWORD) (iy);
			sprintf (szFileName, "MAI%s\\%06u_minimap.bmp", m_pMapAccessor->GetName().c_str(), wID);

			ILuint srcImage;
			ilGenImages (2, &srcImage);
			ilBindImage (srcImage);

			ilLoadImage (szFileName);
			iluFlipImage();	// BMP는 이미지가 거꾸로 되어 있다.

			ILubyte * pData = ilGetData();
			ILint iImageWidth = ilGetInteger (IL_IMAGE_WIDTH);
			ILint iImageHeight = ilGetInteger (IL_IMAGE_HEIGHT);

			ilBindImage (dstImage);
			ilSetPixels ((ILint) ix * 1024, (ILint) iy * 1024, 0, iImageWidth, iImageHeight, iImageDepth, iImageFormat, iImageType, pData);

			ilDeleteImages (2, &srcImage);
		}
	}

	sprintf (szFileName, "%s_MAI_atlas.bmp", m_pMapAccessor->GetName().c_str());
	iluFlipImage();	// BMP는 이미지가 거꾸로 되어 있다.
	ilSaveImage (szFileName);

	ilDeleteImages (1, &dstImage);

	LogBoxf ("[SUCCESS] SaveAtlas: Save complete. Name: [%s].", szFileName);

}

void CMapManagerAccessor::UpdateTerrainShadowMap()
{
	if (!IsMapReady())
	{
		return;
	}

	CTerrainAccessor * pTerrainAccessor;
	if (!m_pMapAccessor->GetTerrainPointer (4, (CTerrain**) &pTerrainAccessor))
	{
		return;
	}

	CShadowRenderHelper::Instance().CreateTextures();

	CShadowRenderHelper::Instance().SetMapOutdoorAccssorPointer (m_pMapAccessor);

	WORD wCoordX, wCoordY;
	pTerrainAccessor->GetCoordinate (&wCoordX, &wCoordY);
	CShadowRenderHelper::Instance().SetTargetTerrainCoord (wCoordX, wCoordY);

	if (CShadowRenderHelper::Instance().StartRenderingPhase (1))
	{
		CSpeedTreeForestDirectX9::Instance().Render (Forest_RenderAll | Forest_RenderToShadow);
		m_pMapAccessor->RenderToShadowMap();
	}
	else
	{
		Trace ("CSceneMap::OnRenderToShadowMap() RenderingPhase 1 Failed...\n");
	}

	CShadowRenderHelper::Instance().EndRenderingPhase (1);

	if (CShadowRenderHelper::Instance().StartRenderingPhase (2))
	{
		m_pMapAccessor->RenderShadow();
	}
	else
	{
		Trace ("CSceneMap::OnRenderToShadowMap() RenderingPhase 2 Failed...\n");
	}

	CShadowRenderHelper::Instance().EndRenderingPhase (2);

	pTerrainAccessor->SaveShadowFromD3DTexture9 (m_pMapAccessor->GetName(), CShadowRenderHelper::Instance().GetShadowTexture());

	CShadowRenderHelper::Instance().ReleaseTextures();
}

void CMapManagerAccessor::ReloadTerrainShadowTexture()
{
	if (!IsMapReady())
	{
		return;
	}

	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CTerrainAccessor * pTerrainAccessor;
		if (!m_pMapAccessor->GetTerrainPointer (i, (CTerrain**) &pTerrainAccessor))
		{
			continue;
		}
		pTerrainAccessor->ReloadShadowTexture (m_pMapAccessor->GetName());
	}
}

bool CMapManagerAccessor::LoadMap (const std::string & c_rstrMapName)
{
	m_pMapAccessor->Clear();
	m_pMapAccessor->Leave();
	m_pMapAccessor->SetName (c_rstrMapName);
	m_pMapAccessor->LoadProperty();
	__RefreshMapID (c_rstrMapName.c_str());

	if (CMapBase::MAPTYPE_INDOOR == m_pMapAccessor->GetType())
	{
		LogBoxf ("[ERROR] LoadMap: Failed to load Indoor Map. Dir: [%s].", c_rstrMapName.c_str());
		return false;
	}
	else if (CMapBase::MAPTYPE_OUTDOOR == m_pMapAccessor->GetType())
	{
		if (!m_pMapAccessor->Load (0.0f, 0.0f, 0.0f))
		{
			return false;
		}

		const std::string& strEnvName = m_pMapAccessor->GetEnvironmentDataName();

		// Environment Data 로드.. (공포의 ymir work .............. ㅠ_ㅠ)
		if (0 < strEnvName.length())
		{
			this->LoadEnvironmentScript (("d:/ymir work/environment/" + strEnvName).c_str());
		}

		m_pMapAccessor->SetMonsterNames();
	}
	else
	{
		LogBoxf ("[ERROR] LoadMap: Invalid Map Type");
		return false;
	}

	m_pMapAccessor->Enter();
	__AddMapInfo();
	return true;
}

void CMapManagerAccessor::RemoveMonsterAreaInfoPtr (CMonsterAreaInfo * pMonsterAreaInfo)
{
	if (!IsMapReady())
	{
		return;
	}
	m_pMapAccessor->RemoveMonsterAreaInfoPtr (pMonsterAreaInfo);

	if (GetSelectedMonsterAreaInfo() == pMonsterAreaInfo)
	{
		m_pSelectedMonsterAreaInfoVector.clear();
		m_lOldOriginX = -1;
	}

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->UpdateMapControlBar();
}

DWORD CMapManagerAccessor::GetMonsterAreaInfoCount()
{
	if (!IsMapReady())
	{
		return 0;
	}
	return m_pMapAccessor->GetMonsterAreaInfoCount();
}

bool CMapManagerAccessor::SaveMonsterAreaInfo()
{
	if (!IsMapReady())
	{
		return false;
	}
	return m_pMapAccessor->SaveMonsterAreaInfo();
}

//////////////////////////////////////////////////////////////////////////
CMonsterAreaInfo* CMapManagerAccessor::AddNewMonsterAreaInfo (long lOriginX, long lOriginY, long lSizeX, long lSizeY,
															  CMonsterAreaInfo::EMonsterAreaInfoType eMonsterAreaInfoType,
															  DWORD dwVID, DWORD dwCount, CMonsterAreaInfo::EMonsterDir eNewMonsterDir)
{
	if (!IsMapReady())
	{
		return nullptr;
	}
	return m_pMapAccessor->AddNewMonsterAreaInfo (lOriginX, lOriginY, lSizeX, lSizeY, eMonsterAreaInfoType, dwVID, dwCount, eNewMonsterDir);
}

struct FMonsterAreaInfoSizeCompare
{
	bool operator() (CMonsterAreaInfo * plhs, CMonsterAreaInfo * prhs) const
	{
		long lSizeX, lSizeY;
		plhs->GetSize (&lSizeX, &lSizeY);
		long lLHSSize = min (lSizeX, lSizeY);
		prhs->GetSize (&lSizeX, &lSizeY);
		long lRHSSize = min (lSizeX, lSizeY);
		return lLHSSize < lRHSSize;
	}
};

void CMapManagerAccessor::SelectMonsterAreaInfo()
{
	long lOriginX, lOriginY;
	lOriginX = (m_wEditTerrainNumX * CTerrainImpl::XSIZE + m_ixEdit) * CTerrainImpl::CELLSCALE_IN_METER + m_bySubCellX * CTerrainImpl::HALF_CELLSCALE_IN_METER;
	lOriginY = (m_wEditTerrainNumY * CTerrainImpl::YSIZE + m_iyEdit) * CTerrainImpl::CELLSCALE_IN_METER + m_bySubCellY * CTerrainImpl::HALF_CELLSCALE_IN_METER;

	if (m_lOldOriginX == lOriginX && m_lOldOriginY == lOriginY)
	{
		return;
	}

	m_lOldOriginX = lOriginX;
	m_lOldOriginY = lOriginY;

	m_pSelectedMonsterAreaInfoVector.clear();

	DWORD dwMonsterAreaInfoCount = GetMonsterAreaInfoCount();
	for (DWORD dwMonsterAreaInfoIndex = 0; dwMonsterAreaInfoIndex < dwMonsterAreaInfoCount; ++dwMonsterAreaInfoIndex)
	{
		CMonsterAreaInfo * pMonsterAreaInfo;
		if (GetMonsterAreaInfoFromVectorIndex (dwMonsterAreaInfoIndex, &pMonsterAreaInfo))
		{
			long lLeft = pMonsterAreaInfo->GetLeft();
			long lRight = pMonsterAreaInfo->GetRight();
			long lTop = pMonsterAreaInfo->GetTop();
			long lBottom = pMonsterAreaInfo->GetBottom();

			if (lOriginX > lRight || lOriginX < lLeft || lOriginY > lBottom || lOriginY < lTop)
			{
				continue;
			}
			m_pSelectedMonsterAreaInfoVector.push_back (pMonsterAreaInfo);
		}
	}

	std::ranges::sort (m_pSelectedMonsterAreaInfoVector, FMonsterAreaInfoSizeCompare());

	m_dwSelectedMonsterAreaInfoIndex = 0;

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->UpdateMapControlBar();
}

void CMapManagerAccessor::SelectNextMonsterAreaInfo (bool bForward)
{
	if (bForward)
	{
		++m_dwSelectedMonsterAreaInfoIndex;
		if (m_dwSelectedMonsterAreaInfoIndex >= m_pSelectedMonsterAreaInfoVector.size())
		{
			m_dwSelectedMonsterAreaInfoIndex = 0;
		}
	}
	else
	{
		if (m_dwSelectedMonsterAreaInfoIndex > 0)
		{
			--m_dwSelectedMonsterAreaInfoIndex;
		}
		else
		{
			m_dwSelectedMonsterAreaInfoIndex = m_pSelectedMonsterAreaInfoVector.size() - 1;
		}
	}

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->UpdateMapControlBar();
}

CMonsterAreaInfo* CMapManagerAccessor::GetSelectedMonsterAreaInfo()
{
	if (m_pSelectedMonsterAreaInfoVector.empty())
	{
		return nullptr;
	}
	else if (1 == m_pSelectedMonsterAreaInfoVector.size())
	{
		return m_pSelectedMonsterAreaInfoVector[0];
	}
	else
	{
		return m_pSelectedMonsterAreaInfoVector[m_dwSelectedMonsterAreaInfoIndex];
	}
}

bool CMapManagerAccessor::GetMonsterAreaInfoFromVectorIndex (DWORD dwMonsterAreaInfoVectorIndex, CMonsterAreaInfo** ppMonsterAreaInfo)
{
	if (!IsMapReady())
	{
		return false;
	}
	return m_pMapAccessor->GetMonsterAreaInfoFromVectorIndex (dwMonsterAreaInfoVectorIndex, ppMonsterAreaInfo);
}

void CMapManagerAccessor::__AddMapInfo()
{
	if (!IsMapReady())
	{
		return;
	}

	auto MapInfoVectorIterator = std::ranges::find_if (m_kVct_kMapInfo, FFindMapName (m_pMapAccessor->GetName()));
	if (m_kVct_kMapInfo.end() != MapInfoVectorIterator)
	{
		UpdateMapInfo();
		return;
	}

	TMapInfo aMapInfo;

	DWORD dwBaseX, dwBaseY;
	short sCountX, sCountY;
	m_pMapAccessor->GetBaseXY (&dwBaseX, &dwBaseY);
	m_pMapAccessor->GetTerrainCount (&sCountX, &sCountY);

	aMapInfo.m_strName = m_pMapAccessor->GetName();
	aMapInfo.m_dwBaseX = dwBaseX;
	aMapInfo.m_dwBaseY = dwBaseY;
	aMapInfo.m_dwSizeX = (DWORD) sCountX;
	aMapInfo.m_dwSizeY = (DWORD) sCountY;

	m_kVct_kMapInfo.push_back (aMapInfo);
}

void CMapManagerAccessor::UpdateMapInfo()
{
	if (!IsMapReady())
	{
		return;
	}

	const std::string & c_rMapName = m_pMapAccessor->GetName();
	auto MapInfoVectorIterator = std::ranges::find_if (m_kVct_kMapInfo, FFindMapName (c_rMapName));
	if (m_kVct_kMapInfo.end() == MapInfoVectorIterator)
	{
		return;
	}

	TMapInfo & rMapInfo = *MapInfoVectorIterator;
	m_pMapAccessor->SetBaseXY (rMapInfo.m_dwBaseX, rMapInfo.m_dwBaseY);
	m_pMapAccessor->SetTerrainCount (rMapInfo.m_dwSizeX, rMapInfo.m_dwSizeY);
}

void CMapManagerAccessor::__RefreshMapID (const char* c_szMapName)
{
	std::string strMapName = c_szMapName;
	stl_lowers (strMapName);
	if (m_kMap_strMapName_iID.contains(strMapName))
	{
		int iID = m_kMap_strMapName_iID[strMapName];
		m_pMapAccessor->SetMapID (iID);
	}
}

CMapOutdoorAccessor* CMapManagerAccessor::GetMapOutdoorPtr()
{
	return m_pMapAccessor;
}

void CMapManagerAccessor::SetEditingCursorPosition (const D3DXVECTOR3 & c_rv3Position)
{
	m_v3EditingCursorPosition = c_rv3Position;
}

void CMapManagerAccessor::RenderGuildArea()
{
	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.RenderMarkedArea();
}
