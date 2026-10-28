#include "StdAfx.h"

// Test Code
#include "MapAccessorOutdoor.h"
#include "../WorldEditor.h"
#include "../MainFrm.h"
#include "../WorldEditorDoc.h"
#include "../WorldEditorView.h"
#include "../../EterLib/Camera.h"

#include "MapAccessorTerrain.h"

void CMapOutdoorAccessor::BackupObject()
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CUndoBuffer * pUndoBuffer = pDocument->GetUndoBuffer();

	pUndoBuffer->Backup (new CObjectUndoData (this));
}

void CMapOutdoorAccessor::BackupObjectCurrent()
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CUndoBuffer * pUndoBuffer = pDocument->GetUndoBuffer();

	pUndoBuffer->BackupCurrent (new CObjectUndoData (this));
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CMapOutdoorAccessor::CObjectUndoData::Backup()
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		if (!m_pOwner->GetAreaAccessor (i, &m_pbackupArea[i]))
		{
			m_pbackupArea[i] = NULL;
			continue;
		}

		CAreaAccessor * pAreaAccessor = m_pbackupArea[i];
		CAreaAccessor::TObjectDataVector & rObjectDataVector = m_backupObjectDataVector[i];

		DWORD dwObjectCount = pAreaAccessor->GetObjectDataCount();

		rObjectDataVector.clear();
		rObjectDataVector.resize (dwObjectCount);

		for (DWORD j = 0; j < dwObjectCount; ++j)
		{
			const CAreaAccessor::TObjectData * pObjectData;

			if (!pAreaAccessor->CArea::GetObjectDataPointer (j, &pObjectData))
			{
				continue;
			}

			rObjectDataVector[j] = *pObjectData;
		}
	}
}

void CMapOutdoorAccessor::CObjectUndoData::Restore()
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		if (!m_pbackupArea[i])
		{
			continue;
		}

		CAreaAccessor * pAreaAccessor = m_pbackupArea[i];
		CAreaAccessor::TObjectDataVector & rObjectDataVector = m_backupObjectDataVector[i];

		pAreaAccessor->Clear();
		for (DWORD j = 0; j < rObjectDataVector.size(); ++j)
		{
			pAreaAccessor->AddObject (&rObjectDataVector[j]);
		}
	}
}

void CMapOutdoorAccessor::CObjectUndoData::BackupStatement()
{
	TOutdoorMapCoordinate OutdoorMapCoordinate = m_pOwner->GetCurCoordinate();
	m_sCenterCoordX = OutdoorMapCoordinate.m_sTerrainCoordX;
	m_sCenterCoordY = OutdoorMapCoordinate.m_sTerrainCoordY;

	D3DXVECTOR3 v3PickingPoint;
	int iPickX, iPickY;
	unsigned char ucSubCellX, ucSubCellY;
	unsigned short usEditTerrainX, usEditTerrainY;
	m_pOwner->GetPickingCoordinate (&v3PickingPoint, &iPickX, &iPickY, &ucSubCellX, &ucSubCellY, &usEditTerrainX, &usEditTerrainY);
	m_fCameraX = v3PickingPoint.x;
	m_fCameraY = v3PickingPoint.y;
}

void CMapOutdoorAccessor::CObjectUndoData::RestoreStatement()
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorView * pView = (CWorldEditorView*)pFrame->GetActiveView();

	D3DXVECTOR3 v3Target = CCameraManager::Instance().GetCurrentCamera()->GetTarget();
	pView->UpdateTargetPosition (m_fCameraX - v3Target.x, m_fCameraY - v3Target.y);
	m_pOwner->SetTerrainModified();
	m_pOwner->Update (m_fCameraX, m_fCameraY, 0.0f);
}

CMapOutdoorAccessor::CObjectUndoData::CObjectUndoData (CMapOutdoorAccessor * pOutdoorAccessor)
	: m_pOwner (pOutdoorAccessor)
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		m_pbackupArea[i] = NULL;
		m_backupObjectDataVector[i].clear();
	}
}

CMapOutdoorAccessor::CObjectUndoData::~CObjectUndoData()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////

CMapOutdoorAccessor::CTerrainUndoData::CTerrainUndoDataSet::CTerrainUndoDataSet()
	: m_pRawHeightMap (NULL),
	  m_pbyTileMap (NULL),
	  m_pNormalMap (NULL)
{
	m_sCoordX = m_sCoordY = 0;
}

CMapOutdoorAccessor::CTerrainUndoData::CTerrainUndoDataSet::~CTerrainUndoDataSet()
{
	DeleteMaps();
}

void CMapOutdoorAccessor::CTerrainUndoData::CTerrainUndoDataSet::DeleteMaps()
{
	if (m_pRawHeightMap)
	{
		delete[] m_pRawHeightMap;
		m_pRawHeightMap = NULL;
	}
	if (m_pbyTileMap)
	{
		delete[] m_pbyTileMap;
		m_pbyTileMap = NULL;
	}
	if (m_pNormalMap)
	{
		delete[] m_pNormalMap;
		m_pNormalMap = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void CMapOutdoorAccessor::BackupTerrain()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->isHeightEditing() && !pMapManagerAccessor->isTextureEditing())
	{
		return;
	}

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CUndoBuffer * pUndoBuffer = pDocument->GetUndoBuffer();

	CTerrainUndoData * pTerrainUndoData = new CTerrainUndoData (this);
	pTerrainUndoData->AddTerrainUndoDataSets();
	pUndoBuffer->Backup (pTerrainUndoData);
}

void CMapOutdoorAccessor::BackupTerrainCurrent()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->isHeightEditing() && !pMapManagerAccessor->isTextureEditing())
	{
		return;
	}

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CUndoBuffer * pUndoBuffer = pDocument->GetUndoBuffer();

	CTerrainUndoData * pTerrainUndoData = new CTerrainUndoData (this);
	pTerrainUndoData->AddTerrainUndoDataSets();
	pUndoBuffer->BackupCurrent (pTerrainUndoData);
}

void CMapOutdoorAccessor::CTerrainUndoData::AddTerrainUndoDataSets()
{
	D3DXVECTOR3 v3PickingPoint;
	int iPickX, iPickY;
	unsigned char ucSubCellX, ucSubCellY;
	WORD usEditTerrainX, usEditTerrainY;
	m_pOwner->GetPickingCoordinate (&v3PickingPoint, &iPickX, &iPickY, &ucSubCellX, &ucSubCellY, &usEditTerrainX, &usEditTerrainY);

	TOutdoorMapCoordinate aOutdoorMapCoordinate = m_pOwner->GetCurCoordinate();

	const unsigned char ucEditTerrainNum = (usEditTerrainY - aOutdoorMapCoordinate.m_sTerrainCoordY + 1) * 3 + (usEditTerrainX - aOutdoorMapCoordinate.m_sTerrainCoordX + 1);

	Clear();

	for (unsigned char i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CTerrain * pTerrain;
		if (m_pOwner->GetTerrainPointer (i, &pTerrain))
		{
			AddTerrainUndoDataSet (i);
		}
	}
}

void CMapOutdoorAccessor::CTerrainUndoData::AddTerrainUndoDataSet (BYTE byTerrainNum)
{
	if (byTerrainNum < 0 || byTerrainNum > AROUND_AREA_NUM - 1)
	{
		LogBoxf ("[ERROR] AddTerrainUndoDataSet: Terrain number is invalid and cannot be added to the undo buffer. Number: [%u].", byTerrainNum);
		return;
	}

	CTerrain * pTerrain;
	if (!m_pOwner->GetTerrainPointer (byTerrainNum, &pTerrain))
	{
		LogBoxf ("[ERROR] AddTerrainUndoDataSet: Cannot obtain pointer to terrain. Terrain number: [%u].", byTerrainNum);
		return;
	}

	CTerrainUndoDataSet * pTerrainUndoDataSet = new CTerrainUndoDataSet;
	unsigned short usCoordX, usCoordY;
	pTerrain->GetCoordinate (&usCoordX, &usCoordY);

	TOutdoorMapCoordinate aOutdoorMapCoordinate = m_pOwner->GetCurCoordinate();
	D3DXVECTOR3 v3Target = CCameraManager::Instance().GetCurrentCamera()->GetTarget();

	pTerrainUndoDataSet->m_sCenterCoordX = aOutdoorMapCoordinate.m_sTerrainCoordX;
	pTerrainUndoDataSet->m_sCenterCoordY = aOutdoorMapCoordinate.m_sTerrainCoordY;
	pTerrainUndoDataSet->m_fCameraX = v3Target.x;
	pTerrainUndoDataSet->m_fCameraY = v3Target.y;
	pTerrainUndoDataSet->m_sCoordX = usCoordX;
	pTerrainUndoDataSet->m_sCoordY = usCoordY;
	m_TerrainUndoDataSetPtrVector.push_back (pTerrainUndoDataSet);
}

void CMapOutdoorAccessor::CTerrainUndoData::Backup()
{
	for (unsigned long i = 0; i < m_TerrainUndoDataSetPtrVector.size(); ++i)
	{
		CTerrainUndoDataSet * pTerrainUndoDataSet = m_TerrainUndoDataSetPtrVector[i];

		pTerrainUndoDataSet->DeleteMaps();

		pTerrainUndoDataSet->m_pRawHeightMap = new WORD[CTerrain::HEIGHTMAP_RAW_XSIZE * CTerrain::HEIGHTMAP_RAW_YSIZE];
		// 		pTerrainUndoDataSet->m_pTileMap = new WORD[CTerrain::TILEMAP_XSIZE * CTerrain::TILEMAP_YSIZE];
		pTerrainUndoDataSet->m_pbyTileMap = new BYTE[CTerrain::TILEMAP_RAW_XSIZE * CTerrain::TILEMAP_RAW_YSIZE];
		pTerrainUndoDataSet->m_pNormalMap = new char[CTerrain::NORMALMAP_XSIZE * CTerrain::NORMALMAP_YSIZE * 3];

		const unsigned char byTerrainNum = (pTerrainUndoDataSet->m_sCoordY - pTerrainUndoDataSet->m_sCenterCoordY + 1) * 3 + (pTerrainUndoDataSet->m_sCoordX -
																															  pTerrainUndoDataSet->m_sCenterCoordX + 1);

		CTerrain * pTerrain;

		if (!m_pOwner->GetTerrainPointer (byTerrainNum, &pTerrain))
		{
			LogBoxf ("[ERROR] CTerrainUndoData::Backup: Cannot obtain pointer to terrain.\n"
					 "Target terrain coordinates: [%u, %u].\n"
					 "Current terrain coordinates: [%u, %u].",
					 pTerrainUndoDataSet->m_sCoordX,
					 pTerrainUndoDataSet->m_sCoordY,
					 pTerrainUndoDataSet->m_sCenterCoordX,
					 pTerrainUndoDataSet->m_sCenterCoordY);
			continue;
		}

		memcpy (pTerrainUndoDataSet->m_pRawHeightMap, pTerrain->GetHeightMap(), sizeof (unsigned short) * CTerrain::HEIGHTMAP_RAW_XSIZE * CTerrain::HEIGHTMAP_RAW_YSIZE);
		// 		memcpy(pTerrainUndoDataSet->m_pTileMap, pTerrain->GetTileMap(), sizeof(unsigned short) * CTerrain::TILEMAP_XSIZE * CTerrain::TILEMAP_YSIZE );
		memcpy (pTerrainUndoDataSet->m_pbyTileMap, pTerrain->RAW_GetTileMap(), sizeof (BYTE) * CTerrain::TILEMAP_RAW_XSIZE * CTerrain::TILEMAP_RAW_YSIZE);
		memcpy (pTerrainUndoDataSet->m_pNormalMap, pTerrain->GetNormalMap(), sizeof (char) * CTerrain::NORMALMAP_XSIZE * CTerrain::NORMALMAP_YSIZE * 3);
	}
}

void CMapOutdoorAccessor::CTerrainUndoData::Restore()
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorView * pView = (CWorldEditorView*)pFrame->GetActiveView();
	const TOutdoorMapCoordinate & c_rOutdoorMapCoordinate = m_pOwner->GetCurCoordinate();

	for (DWORD i = 0; i < m_TerrainUndoDataSetPtrVector.size(); ++i)
	{
		CTerrainUndoDataSet * pTerrainUndoDataSet = m_TerrainUndoDataSetPtrVector[i];

		D3DXVECTOR3 v3Target = CCameraManager::Instance().GetCurrentCamera()->GetTarget();
		//if (pTerrainUndoDataSet->m_fCameraX != v3Target.x || pTerrainUndoDataSet->m_fCameraY != v3Target.y)
		if (pTerrainUndoDataSet->m_sCenterCoordX != c_rOutdoorMapCoordinate.m_sTerrainCoordX || pTerrainUndoDataSet->m_sCenterCoordY != c_rOutdoorMapCoordinate.m_sTerrainCoordY)
		{
			pView->UpdateTargetPosition (pTerrainUndoDataSet->m_fCameraX - v3Target.x, pTerrainUndoDataSet->m_fCameraY - v3Target.y);
			m_pOwner->SetTerrainModified();
			m_pOwner->Update (pTerrainUndoDataSet->m_fCameraX, pTerrainUndoDataSet->m_fCameraY, 0.0f);
		}

		const unsigned char byTerrainNum = (pTerrainUndoDataSet->m_sCoordY - pTerrainUndoDataSet->m_sCenterCoordY + 1) * 3 + (pTerrainUndoDataSet->m_sCoordX -
																															  pTerrainUndoDataSet->m_sCenterCoordX + 1);

		CTerrain * pTerrain;

		if (!m_pOwner->GetTerrainPointer (byTerrainNum, &pTerrain))
		{
			LogBoxf ("[ERROR] CTerrainUndoData::Restore: Cannot obtain pointer to terrain.\n"
					 "Target terrain coordinates: [%u, %u].\n"
					 "Current center terrain coordinates: [%u, %u].",
					 pTerrainUndoDataSet->m_sCoordX, pTerrainUndoDataSet->m_sCoordY, pTerrainUndoDataSet->m_sCenterCoordX, pTerrainUndoDataSet->m_sCenterCoordY);
			continue;
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		CTerrainAccessor * pTerrainAccessor = (CTerrainAccessor*) pTerrain;

		// 		pTerrainAccessor->RestoreMaps(pTerrainUndoDataSet->m_pRawHeightMap, pTerrainUndoDataSet->m_pTileMap, pTerrainUndoDataSet->m_pNormalMap);
		pTerrainAccessor->RAW_RestoreMaps (pTerrainUndoDataSet->m_pRawHeightMap, pTerrainUndoDataSet->m_pbyTileMap, pTerrainUndoDataSet->m_pNormalMap);

		CArea * pArea;

		if (!m_pOwner->GetAreaPointer (byTerrainNum, &pArea))
		{
			LogBoxf ("[ERROR] CTerrainUndoData::Restore: Cannot obtain pointer to Area.\n"
					 "Target Area coordinates: [%u, %u].\n"
					 "Current center terrain coordinates: [%u, %u].",
					 pTerrainUndoDataSet->m_sCoordX, pTerrainUndoDataSet->m_sCoordY, pTerrainUndoDataSet->m_sCenterCoordX, pTerrainUndoDataSet->m_sCenterCoordY);
			continue;
		}

		CAreaAccessor * pAreaAccessor = (CAreaAccessor*) pArea;

		// FIXME : 꼭 최적화를.. 꼭.. 꼭.. -_-
		DWORD dwObjectCount = pArea->GetObjectDataCount();

		for (DWORD j = 0; j < dwObjectCount; ++j)
		{
			const CArea::TObjectData * c_pObjectData;

			if (!pArea->GetObjectDataPointer (j, &c_pObjectData))
			{
				continue;
			}

			// 			((CArea::TObjectData *) c_pObjectData)->Position.z = pTerrain->GetTerrainHeight(c_pObjectData->Position.x, -c_pObjectData->Position.y);
			((CArea::TObjectData*) c_pObjectData)->Position.z = pTerrain->GetHeight ((int)c_pObjectData->Position.x, (int) - c_pObjectData->Position.y);
			pAreaAccessor->UpdateObject (j, c_pObjectData);
		}
	}

	for (int j = 0; j < AROUND_AREA_NUM; ++j)
	{
		CTerrainAccessor * pTerrainAccessor;

		if (!m_pOwner->GetTerrainPointer (j, (CTerrain**) &pTerrainAccessor))
		{
			continue;
		}

		// 		pTerrainAccessor->UpdateSplats();
		pTerrainAccessor->RAW_GenerateSplat();
	}

	m_pOwner->SetTerrainModified();
}

void CMapOutdoorAccessor::CTerrainUndoData::BackupStatement()
{
	//	TOutdoorMapCoordinate OutdoorMapCoordinate = m_pOwner->GetCurCoordinate();
	//	m_sCenterCoordX = OutdoorMapCoordinate.m_sTerrainCoordX;
	//	m_sCenterCoordY = OutdoorMapCoordinate.m_sTerrainCoordY;
	//
	//	D3DXVECTOR3 v3PickingPoint;
	//	int iPickX, iPickY;
	//	unsigned char ucSubCellX, ucSubCellY;
	//	unsigned short usEditTerrainX, usEditTerrainY;
	//	m_pOwner->GetPickingPoint(&v3PickingPoint, &iPickX, &iPickY, &ucSubCellX, &ucSubCellY, &usEditTerrainX, &usEditTerrainY);
	D3DXVECTOR3 v3Target = CCameraManager::Instance().GetCurrentCamera()->GetTarget();
	m_fCameraX = v3Target.x;
	m_fCameraY = v3Target.y;
}

void CMapOutdoorAccessor::CTerrainUndoData::RestoreStatement()
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorView * pView = (CWorldEditorView*)pFrame->GetActiveView();

	D3DXVECTOR3 v3Target = CCameraManager::Instance().GetCurrentCamera()->GetTarget();
	pView->UpdateTargetPosition (m_fCameraX - v3Target.x, m_fCameraY - v3Target.y);
	m_pOwner->SetTerrainModified();
	m_pOwner->Update (m_fCameraX, m_fCameraY, 0.0f);
}

CMapOutdoorAccessor::CTerrainUndoData::CTerrainUndoData (CMapOutdoorAccessor * pOwner) : m_pOwner (pOwner)
{
	Clear();
}

CMapOutdoorAccessor::CTerrainUndoData::~CTerrainUndoData()
{
	Clear();
}

void CMapOutdoorAccessor::CTerrainUndoData::Clear()
{
	for (unsigned long i = 0; i < m_TerrainUndoDataSetPtrVector.size(); ++i)
	{
		CTerrainUndoDataSet * pTerrainUndoDataSet = m_TerrainUndoDataSetPtrVector[i];
		// 		pTerrainUndoDataSet->DeleteMaps();
		delete pTerrainUndoDataSet;
		pTerrainUndoDataSet = NULL;
	}
	m_TerrainUndoDataSetPtrVector.clear();
}
