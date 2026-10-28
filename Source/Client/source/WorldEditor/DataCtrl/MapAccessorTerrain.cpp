#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapAccessorTerrain.h"
#include "MapAccessorOutdoor.h"
#include <fcntl.h>
//#include <dxtlib.h>
#include <il/il.h>
#include <il/ilu.h>

//////////////////////////////////////////////////////////////////////////
// CTerrainAccessor
//////////////////////////////////////////////////////////////////////////
void CTerrainAccessor::RAW_ResetTextures (const BYTE * pbyTileMap)
{
	if (pbyTileMap)
	{
		for (long ly = 0; ly < TILEMAP_RAW_YSIZE; ++ly)
		{
			for (long lx = 0; lx < TILEMAP_RAW_XSIZE; ++lx)
			{
				long lOffset = ly * TILEMAP_RAW_XSIZE + lx;
				BYTE byOldRawtileNum = m_abyTileMap[lOffset];
				BYTE byNewRawtileNum = pbyTileMap[lOffset];
				if (byOldRawtileNum != byNewRawtileNum)
				{
					RecalculateTile (lx, ly, byNewRawtileNum);
				}
			}
		}
	}
	else
	{
		for (long y = 0; y < TILEMAP_RAW_YSIZE; ++y)
			for (long x = 0; x < TILEMAP_RAW_XSIZE; ++x)
			{
				RecalculateTile (x, y, m_abyTileMap[y * TILEMAP_RAW_XSIZE + x]);
			}
	}
}

void CTerrainAccessor::DrawHeightBrush (DWORD dwBrushShape,
										DWORD dwBrushType,
										long lCellX,
										long lCellY,
										BYTE byBrushSize,
										BYTE byBrushStrength)
{
	switch (dwBrushType)
	{
		case BRUSH_TYPE_UP:
			UpTerrain (dwBrushShape, lCellX, lCellY, byBrushSize, byBrushStrength);
			break;

		case BRUSH_TYPE_DOWN:
			DownTerrain (dwBrushShape, lCellX, lCellY, byBrushSize, byBrushStrength);
			break;

		case BRUSH_TYPE_PLATEAU:
			FlatTerrain (dwBrushShape, lCellX, lCellY, byBrushSize, byBrushStrength);
			break;

		case BRUSH_TYPE_NOISE:
			NoiseTerrain (dwBrushShape, lCellX, lCellY, byBrushSize, byBrushStrength);
			break;

		case BRUSH_TYPE_SMOOTH:
			SmoothTerrain (dwBrushShape, lCellX, lCellY, byBrushSize, byBrushStrength);
			break;

		default:
			return;
	}
	RAW_UpdateAttrSplat();
}

void CTerrainAccessor::DrawTextureBrush (DWORD dwBrushShape,
										 const std::vector<BYTE>& rVectorTextureNum,
										 long lCellX,
										 long lCellY,
										 BYTE bySubCellX,
										 BYTE bySubCellY,
										 BYTE byBrushSize,
										 bool bErase,
										 bool bDrawOnlyOnBlankTile)
{
	long cx, cy;
	long i, j;
	long x2, y2;
	float dist;
	BYTE origtilenum, newtilenum;

	long Left, Top;

	/* Center location */
	cx = lCellX * HEIGHT_TILE_XRATIO + bySubCellX;
	cy = lCellY * HEIGHT_TILE_YRATIO + bySubCellY;

	/* Move to upper left */
	Left = cx - byBrushSize * HEIGHT_TILE_XRATIO;
	Top = cy - byBrushSize * HEIGHT_TILE_YRATIO;

	BYTE byTextureMax = rVectorTextureNum.size();

	if (0 == byTextureMax)
	{
		return;
	}

	if (BRUSH_SHAPE_CIRCLE == dwBrushShape)
	{
		for (j = 0; j < max (2 * byBrushSize * HEIGHT_TILE_YRATIO, 1); j++)
		{
			for (i = 0; i < max (2 * byBrushSize * HEIGHT_TILE_XRATIO, 1); i++)
			{
				x2 = Left + i;
				y2 = Top + j;
				if (x2 < -1 || x2 >= TILEMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= TILEMAP_RAW_YSIZE - 1)
				{
					continue;
				}
				float xf = (float) (x2) + 0.5f;
				float yf = (float) (y2) + 0.5f;

				dist = sqrtf ((xf - (float)cx) * (xf - (float)cx) + (yf - (float)cy) * (yf - (float)cy));

				if (dist < max (byBrushSize * HEIGHT_TILE_XRATIO, 1))
				{
					origtilenum = m_abyTileMap[ (y2 + 1) * TILEMAP_RAW_XSIZE + (x2 + 1)];

					if (bDrawOnlyOnBlankTile)
					{
						if (0 == origtilenum)
						{
							newtilenum = rVectorTextureNum[random_range (0, byTextureMax - 1)];
							RecalculateTile (x2 + 1, y2 + 1, newtilenum);
						}
					}
					else
					{
						if (bErase)
						{
							bool bFoundNErased = false;
							for (DWORD dwi = 0; dwi < rVectorTextureNum.size(); ++dwi)
							{
								if (origtilenum == rVectorTextureNum[dwi])
								{
									bFoundNErased = true;
									newtilenum = 0;
								}
							}
							if (!bFoundNErased)
							{
								newtilenum = origtilenum;
							}
						}
						else
						{
							newtilenum = rVectorTextureNum[random_range (0, byTextureMax - 1)];
						}
						RecalculateTile (x2 + 1, y2 + 1, newtilenum);
					}
				}
			}
		}
	}
	else if (BRUSH_SHAPE_SQUARE == dwBrushShape)
	{
		for (j = 0; j < max (2 * byBrushSize * HEIGHT_TILE_YRATIO, 1); j++)
		{
			for (i = 0; i < max (2 * byBrushSize * HEIGHT_TILE_XRATIO, 1); i++)
			{
				x2 = Left + i;
				y2 = Top + j;
				if (x2 < -1 || x2 >= TILEMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= TILEMAP_RAW_YSIZE - 1)
				{
					continue;
				}

				origtilenum = m_abyTileMap[ (y2 + 1) * TILEMAP_RAW_XSIZE + (x2 + 1)];

				if (bDrawOnlyOnBlankTile)
				{
					if (0 == origtilenum)
					{
						newtilenum = rVectorTextureNum[random_range (0, byTextureMax - 1)];
						RecalculateTile (x2 + 1, y2 + 1, newtilenum);
					}
				}
				else
				{
					if (bErase)
					{
						bool bFoundNErased = false;
						for (DWORD dwi = 0; dwi < rVectorTextureNum.size(); ++dwi)
						{
							if (origtilenum == rVectorTextureNum[dwi])
							{
								bFoundNErased = true;
								newtilenum = 0;
							}
						}
						if (!bFoundNErased)
						{
							newtilenum = origtilenum;
						}
					}
					else
					{
						newtilenum = rVectorTextureNum[random_range (0, byTextureMax - 1)];
					}
					RecalculateTile (x2 + 1, y2 + 1, newtilenum);
				}
			}
		}
	}
}

void CTerrainAccessor::DrawAttrBrush (DWORD dwBrushShape,
									  BYTE byAttrFlag,
									  long lCellX,
									  long lCellY,
									  BYTE bySubCellX,
									  BYTE bySubCellY,
									  BYTE byBrushSize,
									  bool bErase)
{
	long cx, cy;
	long i, j;
	long x2, y2;
	float dist;

	long Left, Top;

	float fAttrHeightRatio = ((float)ATTRMAP_XSIZE) / ((float)XSIZE);
	float fAttrTileRatio = ((float)ATTRMAP_XSIZE) / ((float)TILEMAP_XSIZE);

	/* Center location */
	cx = lCellX * fAttrHeightRatio + bySubCellX * fAttrTileRatio;
	cy = lCellY * fAttrHeightRatio + bySubCellY * fAttrTileRatio;

	/* Move to upper left */
	Left = cx - byBrushSize * fAttrHeightRatio;
	Top = cy - byBrushSize * fAttrHeightRatio;

	if (bErase)
	{
		if (BRUSH_SHAPE_CIRCLE == dwBrushShape)
		{
			for (j = 0; j < max (2 * byBrushSize * fAttrHeightRatio, 1); ++j)
			{
				for (i = 0; i < max (2 * byBrushSize * fAttrHeightRatio, 1); ++i)
				{
					x2 = Left + i;
					y2 = Top + j;
					if (x2 < 0 || x2 >= ATTRMAP_XSIZE || y2 < 0 || y2 >= ATTRMAP_YSIZE)
					{
						continue;
					}
					float xf = (float) (Left + i) + 0.5f;
					float yf = (float) (Top + j) + 0.5f;

					dist = sqrtf ((xf - (float)cx) * (xf - (float)cx) + (yf - (float)cy) * (yf - (float)cy));

					if (dist < max (byBrushSize * fAttrHeightRatio, 1))
					{
						m_abyAttrMap[y2 * ATTRMAP_XSIZE + x2] &= ~ (byAttrFlag);
					}
				}
			}
		}
		else if (BRUSH_SHAPE_SQUARE == dwBrushShape)
		{
			for (j = 0; j < max (2 * byBrushSize * fAttrHeightRatio, 1); ++j)
			{
				for (i = 0; i < max (2 * byBrushSize * fAttrHeightRatio, 1); ++i)
				{
					x2 = Left + i;
					y2 = Top + j;
					if (x2 < 0 || x2 >= ATTRMAP_XSIZE || y2 < 0 || y2 >= ATTRMAP_XSIZE)
					{
						continue;
					}
					m_abyAttrMap[y2 * ATTRMAP_XSIZE + x2] &= ~ (byAttrFlag);
				}
			}
		}
	}
	else
	{
		if (BRUSH_SHAPE_CIRCLE == dwBrushShape)
		{
			for (j = 0; j < max (2 * byBrushSize * fAttrHeightRatio, 1); ++j)
			{
				for (i = 0; i < max (2 * byBrushSize * fAttrHeightRatio, 1); ++i)
				{
					x2 = Left + i;
					y2 = Top + j;
					if (x2 < 0 || x2 >= ATTRMAP_XSIZE || y2 < 0 || y2 >= ATTRMAP_XSIZE)
					{
						continue;
					}
					float xf = (float) (Left + i) + 0.5f;
					float yf = (float) (Top + j) + 0.5f;

					dist = sqrtf ((xf - (float)cx) * (xf - (float)cx) + (yf - (float)cy) * (yf - (float)cy));

					if (dist < max (byBrushSize * fAttrHeightRatio, 1))
					{
						m_abyAttrMap[y2 * ATTRMAP_XSIZE + x2] |= (byAttrFlag);
					}
				}
			}
		}
		else if (BRUSH_SHAPE_SQUARE == dwBrushShape)
		{
			for (j = 0; j < max (2 * byBrushSize * fAttrHeightRatio, 1); ++j)
			{
				for (i = 0; i < max (2 * byBrushSize * fAttrHeightRatio, 1); ++i)
				{
					x2 = Left + i;
					y2 = Top + j;
					if (x2 < 0 || x2 >= ATTRMAP_XSIZE || y2 < 0 || y2 >= ATTRMAP_XSIZE)
					{
						continue;
					}
					m_abyAttrMap[y2 * ATTRMAP_XSIZE + x2] |= (byAttrFlag);
				}
			}
		}
	}
	RAW_NotifyAttrModified();
}

void CTerrainAccessor::DrawWaterBrush (DWORD dwBrushShape,
									   long lCellX,
									   long lCellY,
									   BYTE byBrushSize,
									   WORD wWaterHeight,
									   bool bErase)
{
	long cx, cy;
	long i, j;
	long x2, y2;
	float dist;
	long offset;

	long Left, Top;

	/* Center location */
	cx = lCellX;
	cy = lCellY;

	/* Move to upper left */
	Left = lCellX - byBrushSize;
	Top = lCellY - byBrushSize;

	BYTE byWaterID;

	if (MAX_WATER_NUM <= m_byNumWater)
	{
		//		Tracef("더이상 물을 추가할 수 없습니다.\n");
		//		return;
	}
	else
	{
		BYTE byi;
		for (byi = 0; byi < MAX_WATER_NUM; byi++)
		{
			if (-1 == m_lWaterHeight[byi])
			{
				break;
			}
		}
		byWaterID = byi;
		m_lWaterHeight[byi] = wWaterHeight;
		++m_byNumWater;
	}

	if (bErase)
	{
		if (BRUSH_SHAPE_CIRCLE == dwBrushShape)
		{
			for (j = 0; j < 2 * byBrushSize; ++j)
			{
				for (i = 0; i < 2 * byBrushSize; ++i)
				{
					x2 = Left + i;
					y2 = Top + j;
					if (x2 < 0 || x2 >= WATERMAP_XSIZE || y2 < 0 || y2 >= WATERMAP_YSIZE)
					{
						continue;
					}
					float xf = (float) (Left + i) + 0.5f;
					float yf = (float) (Top + j) + 0.5f;

					dist = sqrtf ((xf - (float)cx) * (xf - (float)cx) + (yf - (float)cy) * (yf - (float)cy));

					if (dist < byBrushSize)
					{
						offset = y2 * WATERMAP_XSIZE + x2;
						if (0xFF != m_abyWaterMap[offset])
						{
							m_abyWaterMap[offset] = 0xFF;
						}

						BYTE byPatchNumX = x2 / PATCH_XSIZE;
						BYTE byPatchNumY = y2 / PATCH_YSIZE;
						m_TerrainPatchList[byPatchNumY * PATCH_XCOUNT + byPatchNumX].NeedUpdate (true);

						DWORD dwRatio = ATTRMAP_XSIZE / WATERMAP_XSIZE;
						m_abyAttrMap[y2 * dwRatio * ATTRMAP_XSIZE + x2 * dwRatio] &= ~ (ATTRIBUTE_WATER);
						m_abyAttrMap[y2 * dwRatio * ATTRMAP_XSIZE + x2 * dwRatio + 1] &= ~ (ATTRIBUTE_WATER);
						m_abyAttrMap[ (y2 * dwRatio + 1) * ATTRMAP_XSIZE + x2 * dwRatio] &= ~ (ATTRIBUTE_WATER);
						m_abyAttrMap[ (y2 * dwRatio + 1) * ATTRMAP_XSIZE + x2 * dwRatio + 1] &= ~ (ATTRIBUTE_WATER);
					}
				}
			}
		}
		else if (BRUSH_SHAPE_SQUARE == dwBrushShape)
		{
			for (j = 0; j < 2 * byBrushSize; ++j)
			{
				for (i = 0; i < 2 * byBrushSize; ++i)
				{
					x2 = Left + i;
					y2 = Top + j;
					if (x2 < 0 || x2 >= WATERMAP_XSIZE || y2 < 0 || y2 >= WATERMAP_YSIZE)
					{
						continue;
					}
					offset = y2 * WATERMAP_XSIZE + x2;
					if (0xFF != m_abyWaterMap[offset])
					{
						m_abyWaterMap[offset] = 0xFF;
					}

					BYTE byPatchNumX = x2 / PATCH_XSIZE;
					BYTE byPatchNumY = y2 / PATCH_YSIZE;
					m_TerrainPatchList[byPatchNumY * PATCH_XCOUNT + byPatchNumX].NeedUpdate (true);

					DWORD dwRatio = ATTRMAP_XSIZE / WATERMAP_XSIZE;
					m_abyAttrMap[y2 * dwRatio * ATTRMAP_XSIZE + x2 * dwRatio] &= ~ (ATTRIBUTE_WATER);
					m_abyAttrMap[y2 * dwRatio * ATTRMAP_XSIZE + x2 * dwRatio + 1] &= ~ (ATTRIBUTE_WATER);
					m_abyAttrMap[ (y2 * dwRatio + 1) * ATTRMAP_XSIZE + x2 * dwRatio] &= ~ (ATTRIBUTE_WATER);
					m_abyAttrMap[ (y2 * dwRatio + 1) * ATTRMAP_XSIZE + x2 * dwRatio + 1] &= ~ (ATTRIBUTE_WATER);
				}
			}
		}
	}
	else
	{
		if (BRUSH_SHAPE_CIRCLE == dwBrushShape)
		{
			for (j = 0; j < 2 * byBrushSize; ++j)
			{
				for (i = 0; i < 2 * byBrushSize; ++i)
				{
					x2 = Left + i;
					y2 = Top + j;
					if (x2 < 0 || x2 >= WATERMAP_XSIZE || y2 < 0 || y2 >= WATERMAP_YSIZE)
					{
						continue;
					}
					float xf = (float) (Left + i) + 0.5f;
					float yf = (float) (Top + j) + 0.5f;

					dist = sqrtf ((xf - (float)cx) * (xf - (float)cx) + (yf - (float)cy) * (yf - (float)cy));

					if (dist < byBrushSize)
					{
						m_abyWaterMap[y2 * WATERMAP_XSIZE + x2] = byWaterID;

						BYTE byPatchNumX = x2 / PATCH_XSIZE;
						BYTE byPatchNumY = y2 / PATCH_YSIZE;
						m_TerrainPatchList[byPatchNumY * PATCH_XCOUNT + byPatchNumX].NeedUpdate (true);

						DWORD dwRatio = ATTRMAP_XSIZE / WATERMAP_XSIZE;
						m_abyAttrMap[y2 * dwRatio * ATTRMAP_XSIZE + x2 * dwRatio] |= (ATTRIBUTE_WATER);
						m_abyAttrMap[y2 * dwRatio * ATTRMAP_XSIZE + x2 * dwRatio + 1] |= (ATTRIBUTE_WATER);
						m_abyAttrMap[ (y2 * dwRatio + 1) * ATTRMAP_XSIZE + x2 * dwRatio] |= (ATTRIBUTE_WATER);
						m_abyAttrMap[ (y2 * dwRatio + 1) * ATTRMAP_XSIZE + x2 * dwRatio + 1] |= (ATTRIBUTE_WATER);
					}
				}
			}
		}
		else if (BRUSH_SHAPE_SQUARE == dwBrushShape)
		{
			for (j = 0; j < 2 * byBrushSize; ++j)
			{
				for (i = 0; i < 2 * byBrushSize; ++i)
				{
					x2 = Left + i;
					y2 = Top + j;
					if (x2 < 0 || x2 >= WATERMAP_XSIZE || y2 < 0 || y2 >= WATERMAP_YSIZE)
					{
						continue;
					}
					m_abyWaterMap[y2 * WATERMAP_XSIZE + x2] = byWaterID;

					BYTE byPatchNumX = x2 / PATCH_XSIZE;
					BYTE byPatchNumY = y2 / PATCH_YSIZE;
					m_TerrainPatchList[byPatchNumY * PATCH_XCOUNT + byPatchNumX].NeedUpdate (true);

					DWORD dwRatio = ATTRMAP_XSIZE / WATERMAP_XSIZE;
					m_abyAttrMap[y2 * dwRatio * ATTRMAP_XSIZE + x2 * dwRatio] |= (ATTRIBUTE_WATER);
					m_abyAttrMap[y2 * dwRatio * ATTRMAP_XSIZE + x2 * dwRatio + 1] |= (ATTRIBUTE_WATER);
					m_abyAttrMap[ (y2 * dwRatio + 1) * ATTRMAP_XSIZE + x2 * dwRatio] |= (ATTRIBUTE_WATER);
					m_abyAttrMap[ (y2 * dwRatio + 1) * ATTRMAP_XSIZE + x2 * dwRatio + 1] |= (ATTRIBUTE_WATER);
				}
			}
		}
	}

	RAW_UpdateAttrSplat();
}

void CTerrainAccessor::UpTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength)
{
	long cx, cy;
	long i, j;
	long x2, y2;
	long hgt, delta;
	long Left, Top;
	float dist;

	/* Center location */
	cx = x;
	cy = y;

	/* Move to upper left */
	Left = x - byBrushSize;
	Top = y - byBrushSize;

	for (j = 0; j < 2 * byBrushSize; ++j)
	{
		for (i = 0; i < 2 * byBrushSize; ++i)
		{
			x2 = Left + i;
			y2 = Top + j;

			if (x2 < -1 || x2 >= HEIGHTMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= HEIGHTMAP_RAW_YSIZE - 1)
			{
				continue;
			}

			dist = sqrtf (((float)x2 - (float)cx) * ((float)x2 - (float)cx) + ((float)y2 - (float)cy) * ((float)y2 - (float)cy));

			if (dist < byBrushSize)
			{
				delta = (long) (((float) byBrushSize * (float) byBrushSize - dist * dist) * (float) byBrushStrength / 16.0f);
				if (delta <= 0)
				{
					delta = 0;
				}

				hgt = (long)GetHeightMapValue (x2, y2);
				hgt += delta;

				if (hgt < 0)
				{
					hgt = 0;
				}
				if (hgt > 65535)
				{
					hgt = 65535;
				}
				TerrainPutHeightmap (x2, y2, hgt, false);
			}
		}
	}
}

void CTerrainAccessor::DownTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength)
{
	long cx, cy;
	long i, j;
	long x2, y2;
	long hgt, delta;
	long Left, Top;
	float dist;

	/* Center location */
	cx = x;
	cy = y;

	/* Move to upper left */
	Left = x - byBrushSize;
	Top = y - byBrushSize;

	for (j = 0; j < 2 * byBrushSize; j++)
	{
		for (i = 0; i < 2 * byBrushSize; i++)
		{
			x2 = Left + i;
			y2 = Top + j;

			if (x2 < -1 || x2 >= HEIGHTMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= HEIGHTMAP_RAW_YSIZE - 1)
			{
				continue;
			}

			dist = sqrtf (((float)x2 - (float)cx) * ((float)x2 - (float)cx) + ((float)y2 - (float)cy) * ((float)y2 - (float)cy));

			if (dist < byBrushSize)
			{
				delta = (long) (((float) byBrushSize * (float) byBrushSize - dist * dist) * (float) byBrushStrength / 16.0f);
				if (delta <= 0)
				{
					delta = 0;
				}

				hgt = (long)GetHeightMapValue (x2, y2);
				hgt -= delta;

				if (hgt < 0)
				{
					hgt = 0;
				}
				if (hgt > 65535)
				{
					hgt = 65535;
				}
				TerrainPutHeightmap (x2, y2, hgt, false);
				//				UpdateAttrMapFromHeightMap(x2, y2);
			}
		}
	}
}

void CTerrainAccessor::FlatTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength)
{
	long cx, cy;
	long i, j;
	long x2, y2;
	float dist;
	long TargetHeight, hgt, delta;
	long Left, Top;

	/* Center location */
	cx = x;
	cy = y;

	/* Move to upper left */
	Left = x - byBrushSize;
	Top = y - byBrushSize;

	if ((GetAsyncKeyState (VK_LSHIFT) & 0x80) == 0x80)
	{
		TargetHeight = 32767;
	}
	else
	{
		BYTE byMyTerrainNum;
		if (!m_pOwnerOutdoorMap->GetTerrainNumFromCoord (m_wX, m_wY, &byMyTerrainNum))
		{
			return;
		}

		CTerrain * pTerrain;
		if (y < 0)
		{
			if (x < 0)
			{
				if (!m_pOwnerOutdoorMap->GetTerrainPointer (byMyTerrainNum - 4, &pTerrain))
				{
					return;
				}
				TargetHeight = (long) pTerrain->WE_GetHeightMapValue (x + XSIZE, y + YSIZE);
			}
			else if (x > XSIZE)
			{
				if (!m_pOwnerOutdoorMap->GetTerrainPointer (byMyTerrainNum - 2, &pTerrain))
				{
					return;
				}
				TargetHeight = (long) pTerrain->WE_GetHeightMapValue (x - XSIZE, y + YSIZE);
			}
			else
			{
				if (!m_pOwnerOutdoorMap->GetTerrainPointer (byMyTerrainNum - 3, &pTerrain))
				{
					return;
				}
				TargetHeight = (long) pTerrain->WE_GetHeightMapValue (x, y + YSIZE);
			}
		}
		else if (y > YSIZE)
		{
			if (x < 0)
			{
				if (!m_pOwnerOutdoorMap->GetTerrainPointer (byMyTerrainNum + 2, &pTerrain))
				{
					return;
				}
				TargetHeight = (long) pTerrain->WE_GetHeightMapValue (x + XSIZE, y - YSIZE);
			}
			else if (x > XSIZE)
			{
				if (!m_pOwnerOutdoorMap->GetTerrainPointer (byMyTerrainNum + 4, &pTerrain))
				{
					return;
				}
				TargetHeight = (long) pTerrain->WE_GetHeightMapValue (x - XSIZE, y - YSIZE);
			}
			else
			{
				if (!m_pOwnerOutdoorMap->GetTerrainPointer (byMyTerrainNum + 3, &pTerrain))
				{
					return;
				}
				TargetHeight = (long) pTerrain->WE_GetHeightMapValue (x, y - YSIZE);
			}
		}
		else
		{
			if (x < 0)
			{
				if (!m_pOwnerOutdoorMap->GetTerrainPointer (byMyTerrainNum - 1, &pTerrain))
				{
					return;
				}
				TargetHeight = (long) pTerrain->WE_GetHeightMapValue (x + XSIZE, y);
			}
			else if (x > XSIZE)
			{
				if (!m_pOwnerOutdoorMap->GetTerrainPointer (byMyTerrainNum + 1, &pTerrain))
				{
					return;
				}
				TargetHeight = (long) pTerrain->WE_GetHeightMapValue (x - XSIZE, y);
			}
			else
			{
				TargetHeight = (long)WE_GetHeightMapValue (x, y);
			}
		}

		if (TargetHeight < 0)
		{
			TargetHeight = 0;
		}
		if (TargetHeight > 65535)
		{
			TargetHeight = 65535;
		}
	}

	if (BRUSH_SHAPE_CIRCLE == dwBrushShape)
	{
		for (j = 0; j < 2 * byBrushSize; j++)
		{
			for (i = 0; i < 2 * byBrushSize; i++)
			{
				x2 = Left + i;
				y2 = Top + j;

				if (x2 < -1 || x2 >= HEIGHTMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= HEIGHTMAP_RAW_YSIZE - 1)
				{
					continue;
				}

				dist = sqrtf (((float)x2 - (float)cx) * ((float)x2 - (float)cx) + ((float)y2 - (float)cy) * ((float)y2 - (float)cy));

				if (dist < byBrushSize)
				{
					hgt = (long)GetHeightMapValue (x2, y2);

					delta = (TargetHeight - hgt) * byBrushStrength / dynamic_cast<CMapOutdoorAccessor*> (m_pOwnerOutdoorMap)->GetMaxBrushStrength();
					hgt += delta;

					if (hgt < 0)
					{
						hgt = 0;
					}
					if (hgt > 65535)
					{
						hgt = 65535;
					}
					TerrainPutHeightmap (x2, y2, hgt);
					//					UpdateAttrMapFromHeightMap(x2, y2);
				}
			}
		}
	}
	else if (BRUSH_SHAPE_SQUARE == dwBrushShape)
	{
		for (j = 1; j < 2 * byBrushSize; j++)
		{
			for (i = 1; i < 2 * byBrushSize; i++)
			{
				x2 = Left + i;
				y2 = Top + j;

				if (x2 < -1 || x2 >= HEIGHTMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= HEIGHTMAP_RAW_YSIZE - 1)
				{
					continue;
				}

				hgt = (long)GetHeightMapValue (x2, y2);

				delta = (TargetHeight - hgt) * byBrushStrength / dynamic_cast<CMapOutdoorAccessor*> (m_pOwnerOutdoorMap)->GetMaxBrushStrength();
				hgt += delta;

				if (hgt < 0)
				{
					hgt = 0;
				}
				if (hgt > 65535)
				{
					hgt = 65535;
				}
				TerrainPutHeightmap (x2, y2, hgt);
				//				UpdateAttrMapFromHeightMap(x2, y2);
			}
		}
	}
}

void CTerrainAccessor::NoiseTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength)
{
	long cx, cy;
	long i, j;
	long x2, y2;
	float dist;
	long hgt, delta;
	long Left, Top;

	/* Center location */
	cx = x;
	cy = y;

	/* Move to upper left */
	Left = x - byBrushSize;
	Top = y - byBrushSize;

	BYTE myTerrainNum;
	m_pOwnerOutdoorMap->GetTerrainNumFromCoord (m_wX, m_wY, &myTerrainNum);
	CTerrainAccessor * pTerrainAccessor = NULL;

	if (BRUSH_SHAPE_CIRCLE == dwBrushShape)
	{
		for (j = 0; j < 2 * byBrushSize; j++)
		{
			for (i = 0; i < 2 * byBrushSize; i++)
			{
				x2 = Left + i;
				y2 = Top + j;

				if (x2 < -1 || x2 >= HEIGHTMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= HEIGHTMAP_RAW_YSIZE - 1)
				{
					continue;
				}

				/* Find distance from center of brush */
				dist = sqrtf (((float) x2 - (float) cx) * ((float)x2 - (float)cx) + ((float)y2 - (float)cy) * ((float)y2 - (float)cy));

				if (dist < byBrushSize)
				{
					delta = (long) (random() % byBrushStrength - (byBrushStrength / 2));

					hgt = (long)GetHeightMapValue (x2, y2);
					hgt += delta;

					if (hgt < 0)
					{
						hgt = 0;
					}
					if (hgt > 65535)
					{
						hgt = 65535;
					}
					TerrainPutHeightmap (x2, y2, hgt);
				}
			}
		}
	}
	else if (BRUSH_SHAPE_SQUARE == dwBrushShape)
	{
		for (j = 1; j < 2 * byBrushSize; j++)
		{
			for (i = 1; i < 2 * byBrushSize; i++)
			{
				x2 = Left + i;
				y2 = Top + j;

				if (x2 < -1 || x2 >= HEIGHTMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= HEIGHTMAP_RAW_YSIZE - 1)
				{
					continue;
				}

				delta = (long) (random() % byBrushStrength - byBrushStrength / 2);

				hgt = (long)GetHeightMapValue (x2, y2);
				hgt += delta;

				if (hgt < 0)
				{
					hgt = 0;
				}
				if (hgt > 65535)
				{
					hgt = 65535;
				}
				TerrainPutHeightmap (x2, y2, hgt);
			}
		}
	}
}

void CTerrainAccessor::SmoothTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength)
{
	long cx, cy;
	long i, j;
	long x2, y2;
	float dist;
	long hgt, delta;
	long Left, Top;
	long xt, xb, xl, xr, yt, yb, yl, yr, zt, zb, zl, zr;

	/* Center location */
	cx = x;
	cy = y;

	/* Move to upper left */
	Left = x - byBrushSize;
	Top = y - byBrushSize;

	BYTE myTerrainNum;
	m_pOwnerOutdoorMap->GetTerrainNumFromCoord (m_wX, m_wY, &myTerrainNum);
	CTerrainAccessor * pTerrainAccessor = NULL;

	if (BRUSH_SHAPE_CIRCLE == dwBrushShape)
	{
		for (j = 0; j < 2 * byBrushSize; j++)
		{
			for (i = 0; i < 2 * byBrushSize; i++)
			{
				x2 = Left + i;
				y2 = Top + j;

				if (x2 < -1 || x2 >= HEIGHTMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= HEIGHTMAP_RAW_YSIZE - 1)
				{
					continue;
				}

				xt = xb = x2;
				yl = yr = y2;
				xl = x2 - 1;
				xr = x2 + 1;
				yt = y2 - 1;
				yb = y2 + 1;

				dist = sqrtf (((float)x2 - (float)cx) * ((float)x2 - (float)cx) + ((float)y2 - (float)cy) * ((float)y2 - (float)cy));

				if (dist < byBrushSize)
				{
					/* Find distance from center of brush */
					zt = (long)WE_GetHeightMapValue (xt, yt);
					zb = (long)WE_GetHeightMapValue (xb, yb);
					zl = (long)WE_GetHeightMapValue (xl, yl);
					zr = (long)WE_GetHeightMapValue (xr, yr);

					hgt = (long)GetHeightMapValue (x2, y2);

					delta = (zt + zb + zl + zr - 4 * hgt) / 4 * byBrushStrength / dynamic_cast<CMapOutdoorAccessor*> (m_pOwnerOutdoorMap)->GetMaxBrushStrength();

					hgt += delta;

					if (hgt < 0)
					{
						hgt = 0;
					}
					if (hgt > 65535)
					{
						hgt = 65535;
					}
					TerrainPutHeightmap (x2, y2, hgt);
				}
			}
		}
	}
	else if (BRUSH_SHAPE_SQUARE == dwBrushShape)
	{
		for (j = 1; j < 2 * byBrushSize; j++)
		{
			for (i = 1; i < 2 * byBrushSize; i++)
			{
				x2 = Left + i;
				y2 = Top + j;

				if (x2 < -1 || x2 >= HEIGHTMAP_RAW_XSIZE - 1 || y2 < -1 || y2 >= HEIGHTMAP_RAW_YSIZE - 1)
				{
					continue;
				}

				xt = xb = x2;
				yl = yr = y2;
				xl = x2 - 1;
				xr = x2 + 1;
				yt = y2 - 1;
				yb = y2 + 1;

				zt = (long)WE_GetHeightMapValue (xt, yt);
				zb = (long)WE_GetHeightMapValue (xb, yb);
				zl = (long)WE_GetHeightMapValue (xl, yl);
				zr = (long)WE_GetHeightMapValue (xr, yr);

				hgt = (long)GetHeightMapValue (x2, y2);

				delta = (zt + zb + zl + zr - 4 * hgt) / 4 * byBrushStrength / dynamic_cast<CMapOutdoorAccessor*> (m_pOwnerOutdoorMap)->GetMaxBrushStrength();

				hgt += delta;

				if (hgt < 0)
				{
					hgt = 0;
				}
				if (hgt > 65535)
				{
					hgt = 65535;
				}
				TerrainPutHeightmap (x2, y2, hgt);
			}
		}
	}
}

void CTerrainAccessor::RAW_RestoreMaps (const WORD * pHeightMap, const BYTE * pbyTileMap, const char* pNormalMap)
{
	memcpy (m_awRawHeightMap, pHeightMap, sizeof (WORD) * HEIGHTMAP_RAW_YSIZE * HEIGHTMAP_RAW_XSIZE);
	memcpy (m_acNormalMap, pNormalMap, sizeof (char) * NORMALMAP_YSIZE * NORMALMAP_XSIZE * 3);

	for (BYTE byPatchNumY = 0; byPatchNumY < PATCH_YCOUNT; ++byPatchNumY)
		for (BYTE byPatchNumX = 0; byPatchNumX < PATCH_XCOUNT; ++byPatchNumX)
		{
			m_TerrainPatchList[byPatchNumY * PATCH_XCOUNT + byPatchNumX].NeedUpdate (true);
		}
	RAW_ResetTextures (pbyTileMap);
}

bool CTerrainAccessor::SaveProperty (const std::string & c_rstrMapName)
{
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);
	sprintf (szFileName, "%s\\%06u\\AreaProperty.txt", c_rstrMapName.c_str(), ulID);
	FILE * File = fopen (szFileName, "w");

	if (!File)
	{
		return false;
	}

	fprintf (File, "ScriptType AreaProperty\n");
	fprintf (File, "\n");

	fprintf (File, "AreaName \"%s\"\n", m_strName.c_str());
	fprintf (File, "\n");

	fprintf (File, "NumWater %u\n", m_byNumWater);
	fprintf (File, "\n");

	fclose (File);

	return true;
}

bool CTerrainAccessor::NewHeightMap (const std::string & c_rstrMapName)
{
	WORD map[HEIGHTMAP_RAW_YSIZE * HEIGHTMAP_RAW_XSIZE];
	WORD * pmap;
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);
	sprintf (szFileName, "%s\\%06u\\height.raw", c_rstrMapName.c_str(), ulID);

	int x, y;
	pmap = map;
	for (y = 0; y < HEIGHTMAP_RAW_YSIZE; y++)
		for (x = 0; x < HEIGHTMAP_RAW_XSIZE; x++)
		{
			*pmap++ = 0x7fff;
		}
	FILE* fp;
	fp = fopen (szFileName, "wb");
	fwrite (map, sizeof (WORD), HEIGHTMAP_RAW_YSIZE * HEIGHTMAP_RAW_XSIZE, fp);
	fclose (fp);
	return true;
}

bool CTerrainAccessor::SaveHeightMap (const std::string & c_rstrMapName)
{
	//char szFileName[256];
	char szRawFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);

	FILE * fp;
	sprintf (szRawFileName, "%s\\%06u\\height.raw", c_rstrMapName.c_str(), ulID);
	fp = fopen (szRawFileName, "wb");
	fwrite (m_awRawHeightMap, sizeof (WORD), HEIGHTMAP_RAW_XSIZE * HEIGHTMAP_RAW_YSIZE, fp);
	fclose (fp);

	return true;
}

bool CTerrainAccessor::NewTileMap (const std::string & c_rstrMapName)
{
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);
	sprintf (szFileName, "%s\\%06u\\tile.raw", c_rstrMapName.c_str(), ulID);

	FILE *fp = fopen (szFileName, "wb");				/* open the file */
	if (!fp)
	{
		return false;
	}

	int size = TILEMAP_RAW_XSIZE * TILEMAP_RAW_YSIZE * sizeof (BYTE);
	BYTE * byTileMap = new BYTE[size];
	memset (byTileMap, 0, size);
	fwrite (byTileMap, size, 1, fp);
	delete [] byTileMap;

	fclose (fp);
	return true;
}

bool CTerrainAccessor::RAW_SaveTileMap (const std::string & c_rstrMapName)
{
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);
	sprintf (szFileName, "%s\\%06u\\tile.raw", c_rstrMapName.c_str(), ulID);

	FILE *fp = fopen (szFileName, "wb");

	if (!fp)
	{
		return false;
	}

	int size = TILEMAP_RAW_XSIZE * TILEMAP_RAW_YSIZE * sizeof (BYTE);	/* find size */
	fwrite (m_abyTileMap, size, 1, fp);			/* write size bytes of data from map ptr */

	fclose (fp);
	return true;
}

bool CTerrainAccessor::SaveAttrMap (const std::string & c_rstrMapName)
{
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);
	sprintf (szFileName, "%s\\%06u\\attr.atr", c_rstrMapName.c_str(), ulID);

	FILE *fp = fopen (szFileName, "wb");				/* open the file */

	if (!fp)
	{
		return false;
	}

	const WORD mapver = 2634;
	fwrite (&mapver, sizeof (WORD), 1, fp);		/* write the magic number */

	const WORD wWidth = ATTRMAP_XSIZE;
	const WORD wHeight = ATTRMAP_YSIZE;
	fwrite (&wWidth, sizeof (WORD), 1, fp);	/* write dimensions */
	fwrite (&wHeight, sizeof (WORD), 1, fp);

	int size = wWidth * wHeight * sizeof (BYTE);	/* find size */
	fwrite (m_abyAttrMap, size, 1, fp);			/* write size bytes of data from map ptr */
	fclose (fp);
	return true;
}

bool CTerrainAccessor::NewAttrMap (const std::string & c_rstrMapName)
{
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);
	sprintf (szFileName, "%s\\%06u\\attr.atr", c_rstrMapName.c_str(), ulID);

	FILE *fp = fopen (szFileName, "wb");				/* open the file */
	if (!fp)
	{
		return false;
	}

	const WORD mapver = 2634;
	fwrite (&mapver, sizeof (WORD), 1, fp);		/* write the magic number */

	const WORD wWidth = ATTRMAP_XSIZE;
	const WORD wHeight = ATTRMAP_YSIZE;
	fwrite (&wWidth, sizeof (WORD), 1, fp);	/* write dimensions */
	fwrite (&wHeight, sizeof (WORD), 1, fp);

	int size = wWidth * wHeight * sizeof (BYTE);	/* find size */
	BYTE * pbyAttrMap = new BYTE[size];			/* write size bytes of data from map ptr */
	memset (pbyAttrMap, 0, size);
	fwrite (pbyAttrMap, size, 1, fp);

	fclose (fp);
	return true;
}

void CTerrainAccessor::RecalculateWaterMap()
{
	BYTE byNumWaterFirst, byNumWaterSecond;
	WORD wWidth, wHeight;
	// Phase 1 : 같은 높이 물끼리 번호 통일한다.
	for (byNumWaterFirst = 0; byNumWaterFirst < m_byNumWater - 1; ++byNumWaterFirst)
	{
		if (-1 == m_lWaterHeight[byNumWaterFirst])
		{
			continue;
		}
		for (byNumWaterSecond = byNumWaterFirst + 1; byNumWaterSecond < m_byNumWater; ++byNumWaterSecond)
		{
			if (-1 == m_lWaterHeight[byNumWaterSecond])
			{
				continue;
			}
			if (m_lWaterHeight[byNumWaterSecond] == m_lWaterHeight[byNumWaterFirst])
			{
				for (wWidth = 0; wWidth < WATERMAP_YSIZE; ++wWidth)
				{
					for (wHeight = 0; wHeight < WATERMAP_XSIZE; ++wHeight)
					{
						if (byNumWaterSecond == m_abyWaterMap[wHeight * WATERMAP_YSIZE + wWidth])
						{
							m_abyWaterMap[wHeight * WATERMAP_XSIZE + wWidth] = byNumWaterFirst;
						}
					}
				}
				m_lWaterHeight[byNumWaterSecond] = -1;
			}
		}
	}

	// Phase 2 : 각 번호의 물 개수를 센다.
	DWORD dwNumWater[MAX_WATER_NUM];
	memset (dwNumWater, 0, sizeof (dwNumWater));

	for (wWidth = 0; wWidth < WATERMAP_YSIZE; ++wWidth)
	{
		for (wHeight = 0; wHeight < WATERMAP_XSIZE; ++wHeight)
		{
			BYTE byNumWater = m_abyWaterMap[wHeight * WATERMAP_XSIZE + wWidth];
			if (0xFF != byNumWater)
			{
				++dwNumWater[byNumWater];
			}
		}
	}

	// Phase 3 : 번호를 앞으로 밀어 붙이고 물개수 조정한다.
	BYTE byNumWaterAfterRecalculate = 0;

	for (byNumWaterFirst = 0; byNumWaterFirst < MAX_WATER_NUM - 1; ++byNumWaterFirst)
	{
		if (0 == dwNumWater[byNumWaterFirst])
		{
			bool bWaterFound = false;
			for (byNumWaterSecond = byNumWaterFirst + 1; byNumWaterSecond < MAX_WATER_NUM; ++byNumWaterSecond)
			{
				if (0 != dwNumWater[byNumWaterSecond])
				{
					bWaterFound = true;
					break;
				}
				else
				{
					m_lWaterHeight[byNumWaterSecond] = -1;
				}
			}
			if (!bWaterFound)
			{
				m_lWaterHeight[byNumWaterFirst] = -1;
				break;
			}
			for (WORD wWidth = 0; wWidth < WATERMAP_YSIZE; ++wWidth)
			{
				for (WORD wHeight = 0; wHeight < WATERMAP_XSIZE; ++wHeight)
				{
					if (byNumWaterSecond == m_abyWaterMap[wHeight * WATERMAP_XSIZE + wWidth])
					{
						m_abyWaterMap[wHeight * WATERMAP_XSIZE + wWidth] = byNumWaterFirst;
					}
				}
			}
			m_lWaterHeight[byNumWaterFirst] = m_lWaterHeight[byNumWaterSecond];
			m_lWaterHeight[byNumWaterSecond] = -1;
		}
		else
		{
			++byNumWaterAfterRecalculate;
		}
	}

	m_byNumWater = byNumWaterAfterRecalculate;
}

bool CTerrainAccessor::SaveWaterMap (const std::string & c_rstrMapName)
{
	RecalculateWaterMap();
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);
	sprintf (szFileName, "%s\\%06u\\water.wtr", c_rstrMapName.c_str(), ulID);

	FILE *fp;
	int size;

	fp = fopen (szFileName, "wb");

	if (!fp)
	{
		return false;
	}

	const WORD mapver = 5426;
	fwrite (&mapver, sizeof (WORD), 1, fp);

	WORD wSizeX = WATERMAP_XSIZE, wSizeY = WATERMAP_YSIZE;
	fwrite (&wSizeX, sizeof (WORD), 1, fp);
	fwrite (&wSizeY, sizeof (WORD), 1, fp);
	fwrite (&m_byNumWater, sizeof (BYTE), 1, fp);

	size = WATERMAP_XSIZE * WATERMAP_YSIZE * sizeof (BYTE);
	fwrite (m_abyWaterMap, size, 1, fp);

	if (m_byNumWater > 0)
	{
		size = m_byNumWater * sizeof (long);
		fwrite (m_lWaterHeight, size, 1, fp);
	}

	fclose (fp);
	return true;
}

static HFILE gs_fileout = NULL;

void WriteDTXnFile (DWORD count, void* buffer)
{
	if (!gs_fileout)
	{
		TraceError ("WriteDTXnFile: no file handle");
		return;
	}

	_write (gs_fileout, buffer, count);
}

void ReadDTXnFile (DWORD count, void* buffer)
{
	return;
}

bool CTerrainAccessor::SaveShadowFromD3DTexture9 (const std::string & c_rstrMapName, LPDIRECT3DTEXTURE9 lpShadowTexture)
{
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);

	// 그림자 관련 파일들을 시작
	sprintf (szFileName, "%s\\%06u\\shadowmap.tga", c_rstrMapName.c_str(), ulID);
	DeleteFile (szFileName);

	sprintf (szFileName, "%s\\%06u\\shadowmap.raw", c_rstrMapName.c_str(), ulID);
	DeleteFile (szFileName);

	sprintf (szFileName, "%s\\%06u\\shadowmap.bmp", c_rstrMapName.c_str(), ulID);
	DeleteFile (szFileName);

	// BMP로 저장
	sprintf (szFileName, "%s\\%06u\\shadowmap.bmp", c_rstrMapName.c_str(), ulID);
	D3DXSaveTextureToFile (szFileName, D3DXIFF_BMP, lpShadowTexture, NULL);

	{
		//////////////////////////////////////////////////////////////////////////
		// 사이즈 줄여 BMP로 변환
		ilInit();
		ilEnable (IL_FILE_OVERWRITE);

		ILuint image;
		ilGenImages (1, &image);
		ilBindImage (image);

		ilLoadImage (szFileName);
		iluScale (256, 256, 1);

		// 버그가 있는지.. RAW 데이터로 저장하면 용량이 너무 커져서
		// 저장 했다 다시 읽는다.. -_- 이러면 됨..
		ilSaveImage (szFileName);
		ilLoadImage (szFileName);

		ilConvertImage (IL_RGBA, IL_BYTE);
		iluFlipImage();	// BMP는 이미지가 거꾸로 되어 있다.

		// Raw Data (Shadow Map) 저장
		ILubyte * pRawData = ilGetData();

		#ifdef SHADOWMAP_TO_TGA
		sprintf (szFileName, "%s\\%06u\\shadowmap.tga", c_rstrMapName.c_str(), ulID);
		#else
		sprintf (szFileName, "%s\\%06u\\shadowmap.raw", c_rstrMapName.c_str(), ulID);
		#endif
		FILE * fp = fopen (szFileName, "w");

		if (fp)
		{
			#ifdef SHADOWMAP_TO_TGA
			TGA_HEADER header;

			memset (&header, 0, sizeof (TGA_HEADER));

			header.imgType		= 2;
			header.width		= ilGetInteger (IL_IMAGE_WIDTH);
			header.height		= ilGetInteger (IL_IMAGE_WIDTH);
			header.colorBits	= 16;
			header.desc			= IMAGEDESC_TOPLEFT;

			fwrite (&header, sizeof (TGA_HEADER), 1, fp);
			#endif
			BYTE * pbData = (BYTE*) pRawData;

			for (int h = 0; h < ilGetInteger (IL_IMAGE_HEIGHT); ++h)
			{
				for (int w = 0; w < ilGetInteger (IL_IMAGE_WIDTH); ++w)
				{
					BYTE r = * (pbData++);
					BYTE g = * (pbData++);
					BYTE b = * (pbData++);
					BYTE a = * (pbData++);

					#ifdef SHADOWMAP_TO_TGA
					// Targa = B5 G5 R5
					WORD wColor = ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3);
					#else
					// Raw565 = R5 G6 B5
					WORD wColor = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
					#endif
					fwrite (&wColor, sizeof (WORD), 1, fp);
				}
			}

			fclose (fp);
		}

		//////////////////////////////////////////////////////////////////////////
		// DDS (DXT1, Mipmap)로도 저장
		/*CompressionOptions DXT1Option;
		memset(&DXT1Option, 0, sizeof(DXT1Option));

		DXT1Option.bMipMapsInImage = false;
		DXT1Option.MIPFilterType = dMIPFilterBox;
		DXT1Option.MipMapType = dGenerateMipMaps;
		DXT1Option.TextureType = dTextureType2D;
		DXT1Option.TextureFormat = dDXT1;
		DXT1Option.bSwapRGB = true;

		char szDDSFileName[MAX_PATH + 1];
		_snprintf(szDDSFileName, MAX_PATH, "%s\\%06u\\shadowmap.dds", c_rstrMapName.c_str(), ulID);

		gs_fileout = _open(szDDSFileName, _O_WRONLY | _O_BINARY | _O_CREAT | _O_TRUNC,  _S_IWRITE);

		if (gs_fileout != -1)
		{
			nvDXTcompress((unsigned char *) pRawData,
							ilGetInteger(IL_IMAGE_WIDTH),
							ilGetInteger(IL_IMAGE_HEIGHT),
							ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL),
							&DXT1Option,
							4,
							0);
			_close(gs_fileout);
		}
		// End of DXT Routine
		*/
		ilDeleteImages (1, &image);
	}

	// BMP는 임시 파일이므로 지운다.
	sprintf (szFileName, "%s\\%06u\\shadowmap.bmp", c_rstrMapName.c_str(), ulID);
	DeleteFile (szFileName);

	return true;
}

bool CTerrainAccessor::SaveMiniMapFromD3DTexture9 (const std::string & c_rstrMapName, LPDIRECT3DTEXTURE9 lpMiniMapTexture)
{
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);

	// 그림자 관련 파일들을 시작
	sprintf (szFileName, "%s\\%06u\\minimap.bmp", c_rstrMapName.c_str(), ulID);
	DeleteFile (szFileName);
	D3DXSaveTextureToFile (szFileName, D3DXIFF_BMP, lpMiniMapTexture, NULL);

	{
		//////////////////////////////////////////////////////////////////////////
		// 사이즈 줄여 BMP로 변환
		ilInit();
		ilEnable (IL_FILE_OVERWRITE);

		ILuint image;
		ilGenImages (1, &image);
		ilBindImage (image);

		ilLoadImage (szFileName);

		ilConvertImage (IL_RGBA, IL_BYTE);
		iluFlipImage();	// BMP는 이미지가 거꾸로 되어 있다.

		// Raw Data (Shadow Map) 저장
		ILubyte * pRawData = ilGetData();


		char szDDSFileName[MAX_PATH + 1];
		_snprintf (szDDSFileName, MAX_PATH, "%s\\%06u\\minimap.dds", c_rstrMapName.c_str(), ulID);

		DeleteFile (szDDSFileName);
		D3DXSaveTextureToFile (szDDSFileName, D3DXIFF_DDS, lpMiniMapTexture, NULL);

		//////////////////////////////////////////////////////////////////////////
		// DDS (DXT1, Mipmap)로 저장
		/*CompressionOptions DXT1Option;

		DXT1Option.bMipMapsInImage = false;
		DXT1Option.MIPFilterType = dMIPFilterPoint;
		DXT1Option.MipMapType = dNoMipMaps;
		DXT1Option.TextureType = dTextureType2D;
		DXT1Option.TextureFormat = dDXT1;
		DXT1Option.bSwapRGB = true;

		char szDDSFileName[MAX_PATH + 1];
		_snprintf(szDDSFileName, MAX_PATH, "%s\\%06u\\minimap.dds", c_rstrMapName.c_str(), ulID);

		gs_fileout = _open(szDDSFileName, _O_WRONLY | _O_BINARY | _O_CREAT | _O_TRUNC,  _S_IWRITE);

		if (gs_fileout != -1)
		{
			nvDXTcompress((unsigned char *) pRawData,
							ilGetInteger(IL_IMAGE_WIDTH),
							ilGetInteger(IL_IMAGE_HEIGHT),
							ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL),
							&DXT1Option,
							4,
							0);
			_close(gs_fileout);
		}
		// End of DXT Routine*/

		ilDeleteImages (1, &image);
	}

	// BMP는 임시 파일이므로 지운다.
	sprintf (szFileName, "%s\\%06u\\minimap.bmp", c_rstrMapName.c_str(), ulID);
	DeleteFile (szFileName);

	return true;
}

bool CTerrainAccessor::ReloadShadowTexture (const std::string & c_rstrMapName)
{
	char szFileName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);
	sprintf (szFileName, "%s\\%06u\\shadowmap.dds", c_rstrMapName.c_str(), ulID);

	CGraphicImage * pImage = (CGraphicImage*) CResourceManager::Instance().GetResourcePointer (szFileName);
	m_ShadowGraphicImageInstance.ReloadImagePointer (pImage);

	if (!m_ShadowGraphicImageInstance.GetTexturePointer()->IsEmpty())
	{
		m_lpShadowTexture = m_ShadowGraphicImageInstance.GetTexturePointer()->GetD3DTexture();
	}
	else
	{
		m_lpShadowTexture = NULL;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CTerrainAccessor::RAW_LoadAndSaveTileMap (const char* rawtilename, const std::string & c_rstrMapName, const std::vector<BYTE>& c_rVectorBaseTexture)
{
	if (!RAW_LoadTileMap (rawtilename))
	{
		LogBoxf ("[ERROR] RAW_LoadAndSaveTileMap: Failed to load TileMap. Coordinates: [%d, %d].", m_wX, m_wY);
		return false;
	}

	BYTE byNumTexture = c_rVectorBaseTexture.size();
	for (int ix = 0; ix < TILEMAP_YSIZE; ++ix)
		for (int iy = 0; iy < TILEMAP_XSIZE; ++iy)
		{
			m_abyTileMap[iy * TILEMAP_XSIZE + ix] = c_rVectorBaseTexture[random_range (0, byNumTexture - 1)];
		}

	if (!RAW_SaveTileMap (c_rstrMapName))
	{
		LogBoxf ("[ERROR] RAW_LoadAndSaveTileMap: Failed to save TileMap. Coordinates: [%d, %d].", m_wX, m_wY);
		return false;
	}

	return true;
}
// Utility
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Attr

void CTerrainAccessor::RAW_DeallocateAttrSplats()
{
	TTerainSplat & rSplat = m_RAWAttrSplatPatch.Splats[0];
	if (m_lpAttrTexture)
	{
		ULONG ulRef;
		do
		{
			ulRef = m_lpAttrTexture->Release();
		}
		while (ulRef > 0);
	}
	rSplat.pd3dTexture = m_lpAttrTexture = NULL;
}

void CTerrainAccessor::RAW_AllocateAttrSplats()
{
	RAW_DeallocateAttrSplats();
	m_RAWAttrSplatPatch.m_bNeedsUpdate = true;
	m_RAWAttrSplatPatch.Splats[0].NeedsUpdate = true;
	RAW_GenerateAttrSplat();
}

void CTerrainAccessor::RAW_ResetAttrSplat()
{
	m_RAWAttrSplatPatch.m_bNeedsUpdate = true;
	m_RAWAttrSplatPatch.Splats[0].NeedsUpdate = true;
	RAW_GenerateAttrSplat();
}

void CTerrainAccessor::RAW_UpdateAttrSplat()
{
	RAW_GenerateAttrSplat();
}

void CTerrainAccessor::RAW_GenerateAttrSplat()
{
	if (!m_RAWAttrSplatPatch.m_bNeedsUpdate)
	{
		return;
	}

	m_RAWAttrSplatPatch.m_bNeedsUpdate = false;

	BYTE abyAlphaMap[ATTRMAP_XSIZE * ATTRMAP_YSIZE];

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	BYTE bySelectedAttrFlag = pMapManagerAccessor->GetSelectedAttrFlag();

	TTerainSplat & rAttrSplat = m_RAWAttrSplatPatch.Splats[0];
	if (rAttrSplat.NeedsUpdate)
	{
		rAttrSplat.NeedsUpdate = 0;

		// make alpha texture
		if (m_lpAttrTexture)
		{
			ULONG ulRef;
			do
			{
				ulRef = m_lpAttrTexture->Release();
			}
			while (ulRef > 0);
		}
		rAttrSplat.pd3dTexture = m_lpAttrTexture = NULL;

		BYTE * aptr = abyAlphaMap;

		for (long y = 0; y < ATTRMAP_YSIZE; ++y)
		{
			for (long x = 0; x < ATTRMAP_XSIZE; ++x)
			{
				if (isAttrOn (x, y, bySelectedAttrFlag))
				{
					*aptr = 0x60;
				}
				else
				{
					*aptr = 0x00;
				}

				++aptr;
			}
		}
		D3DLOCKED_RECT  d3dlr;

		HRESULT hr;
		do
		{
			hr = ms_lpd3dDevice->CreateTexture (ATTRMAP_XSIZE, ATTRMAP_YSIZE, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpAttrTexture, nullptr);
		}
		while (FAILED (hr));
		do
		{
			hr = m_lpAttrTexture->LockRect (0, &d3dlr, 0, 0);
		}
		while (FAILED (hr));
		PutImage32 (abyAlphaMap, (BYTE*) d3dlr.pBits, ATTRMAP_XSIZE, d3dlr.Pitch, ATTRMAP_XSIZE, ATTRMAP_YSIZE);
		do
		{
			hr = m_lpAttrTexture->UnlockRect (0);
		}
		while (FAILED (hr));
		rAttrSplat.pd3dTexture = m_lpAttrTexture;
	}
}

void CTerrainAccessor::RAW_NotifyAttrModified()
{
	m_RAWAttrSplatPatch.m_bNeedsUpdate = true;
	m_RAWAttrSplatPatch.Splats[0].NeedsUpdate = true;
}

bool CTerrainAccessor::RAW_LoadAndSaveDefaultAttrMap (const std::string & c_rstrMapName)
{
	char szAttrMapName[256];
	DWORD ulID = (DWORD) (m_wX) * 1000L + (DWORD) (m_wY);

	sprintf (szAttrMapName, "%s\\%06u\\attr.atr", c_rstrMapName.c_str(), ulID);

	LoadAttrMap (szAttrMapName);

	for (DWORD dwY = 0; dwY < ATTRMAP_YSIZE; ++dwY)
	{
		for (DWORD dwX = 0; dwX < ATTRMAP_XSIZE; ++dwX)
		{
			m_abyAttrMap[dwY * ATTRMAP_XSIZE + dwX] &= ~0xF0;
		}
	}

	SaveAttrMap (c_rstrMapName);
	return true;
}
// Attr
//////////////////////////////////////////////////////////////////////////

void CTerrainAccessor::RecalculateTile (long lX, long lY, BYTE byNewTileNum)
{
	if (lX < 0 || lY < 0 || lX >= TILEMAP_RAW_XSIZE || lY >= TILEMAP_RAW_YSIZE)
	{
		return;
	}

	DWORD dwTileOffset = lY * TILEMAP_RAW_XSIZE + lX;
	BYTE byOrigTilelNum = m_abyTileMap[dwTileOffset];
	m_abyTileMap[dwTileOffset] = byNewTileNum;

	m_TerrainSplatPatch.m_bNeedsUpdate = true;

	if (m_TerrainSplatPatch.TileCount[byOrigTilelNum] > 0)
	{
		--m_TerrainSplatPatch.TileCount[byOrigTilelNum];
	}

	++m_TerrainSplatPatch.TileCount[byNewTileNum];

	for (int inum = min (byOrigTilelNum, byNewTileNum); inum <= max (byOrigTilelNum, byNewTileNum); ++inum)
	{
		m_TerrainSplatPatch.Splats[inum].NeedsUpdate = 1;
	}

	long lPatchIndexX = min (max ((lX - 1) / PATCH_TILE_XSIZE, 0), PATCH_XCOUNT - 1);
	long lPatchIndexY = min (max ((lY - 1) / PATCH_TILE_YSIZE, 0), PATCH_YCOUNT - 1);
	long lPatchNum = lPatchIndexY * PATCH_XCOUNT + lPatchIndexX;

	if (m_TerrainSplatPatch.PatchTileCount[lPatchNum][byOrigTilelNum] > 0)
	{
		--m_TerrainSplatPatch.PatchTileCount[lPatchNum][byOrigTilelNum];
	}

	++m_TerrainSplatPatch.PatchTileCount[lPatchNum][byNewTileNum];

	if (0 == lY % PATCH_TILE_YSIZE && 0 != lY && (TILEMAP_RAW_YSIZE - 2) != lY)
	{
		++m_TerrainSplatPatch.PatchTileCount[min (PATCH_YCOUNT - 1, lPatchIndexY + 1) * PATCH_XCOUNT + lPatchIndexX][byNewTileNum];
		--m_TerrainSplatPatch.PatchTileCount[min (PATCH_YCOUNT - 1, lPatchIndexY + 1) * PATCH_XCOUNT + lPatchIndexX][byOrigTilelNum];
		if (0 == lX % PATCH_TILE_XSIZE && 0 != lX && (TILEMAP_RAW_YSIZE - 2) != lX)
		{
			++m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byOrigTilelNum];
			++m_TerrainSplatPatch.PatchTileCount[min (PATCH_YCOUNT - 1, lPatchIndexY + 1) * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[min (PATCH_YCOUNT - 1, lPatchIndexY + 1) * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byOrigTilelNum];
		}
		else if (1 == lX % PATCH_TILE_XSIZE && (TILEMAP_RAW_XSIZE - 1) != lX && 1 != lX)
		{
			++m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byOrigTilelNum];
			++m_TerrainSplatPatch.PatchTileCount[min (PATCH_YCOUNT - 1, lPatchIndexY + 1) * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[min (PATCH_YCOUNT - 1, lPatchIndexY + 1) * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byOrigTilelNum];
		}
	}
	else if (1 == lY % PATCH_TILE_YSIZE && (TILEMAP_RAW_YSIZE - 1) != lY && 1 != lY)
	{
		++m_TerrainSplatPatch.PatchTileCount[max (0, lPatchIndexY - 1) * PATCH_XCOUNT + lPatchIndexX][byNewTileNum];
		--m_TerrainSplatPatch.PatchTileCount[max (0, lPatchIndexY - 1) * PATCH_XCOUNT + lPatchIndexX][byOrigTilelNum];
		if (0 == lX % PATCH_TILE_XSIZE && 0 != lX && (TILEMAP_RAW_YSIZE - 2) != lX)
		{
			++m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byOrigTilelNum];
			++m_TerrainSplatPatch.PatchTileCount[max (0, lPatchIndexY - 1) * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[max (0, lPatchIndexY - 1) * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byOrigTilelNum];
		}
		else if (1 == lX % PATCH_TILE_XSIZE && (TILEMAP_RAW_XSIZE - 1) != lX && 1 != lX)
		{
			++m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byOrigTilelNum];
			++m_TerrainSplatPatch.PatchTileCount[max (0, lPatchIndexY - 1) * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[max (0, lPatchIndexY - 1) * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byOrigTilelNum];
		}
	}
	else
	{
		if (0 == lX % PATCH_TILE_XSIZE && 0 != lX && (TILEMAP_RAW_YSIZE - 2) != lX)
		{
			++m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + min (PATCH_XCOUNT - 1, lPatchIndexX + 1)][byOrigTilelNum];
		}
		else if (1 == lX % PATCH_TILE_XSIZE && (TILEMAP_RAW_XSIZE - 1) != lX && 1 != lX)
		{
			++m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byNewTileNum];
			--m_TerrainSplatPatch.PatchTileCount[lPatchIndexY * PATCH_XCOUNT + max (0, lPatchIndexX - 1)][byOrigTilelNum];
		}
	}
}

void CTerrainAccessor::Clear()
{
	memset (&m_abyAttrMap, 0, sizeof (ATTRMAP_YSIZE * ATTRMAP_XSIZE));
	memset (&m_RAWAttrSplatPatch, 0, sizeof (m_RAWAttrSplatPatch));
	CTerrain::Clear();
}

void CTerrainAccessor::TerrainPutHeightmap (long x, long y, WORD val, bool bRecursive)
{
	int iPos = (y + 1) * HEIGHTMAP_RAW_XSIZE + (x + 1);
	if (iPos >= 0)
		if (iPos < HEIGHTMAP_RAW_YSIZE * HEIGHTMAP_RAW_XSIZE)
		{
			m_awRawHeightMap[iPos] = val;
		}

	if (x >= 0 && y >= 0 && x < NORMALMAP_XSIZE && y < NORMALMAP_YSIZE)
	{
		CalculateNormal (x, y);
	}
	long x2 = x - 1;
	long y2 = y;
	if (x2 >= 0 && y2 >= 0 && x2 < NORMALMAP_XSIZE && y2 < NORMALMAP_YSIZE)
	{
		CalculateNormal (x2, y2);
	}
	x2 = x + 1;
	y2 = y;
	if (x2 >= 0 && y2 >= 0 && x2 < NORMALMAP_XSIZE && y2 < NORMALMAP_YSIZE)
	{
		CalculateNormal (x2, y2);
	}
	x2 = x;
	y2 = y - 1;
	if (x2 >= 0 && y2 >= 0 && x2 < NORMALMAP_XSIZE && y2 < NORMALMAP_YSIZE)
	{
		CalculateNormal (x2, y2);
	}
	x2 = x;
	y2 = y + 1;
	if (x2 >= 0 && y2 >= 0 && x2 < NORMALMAP_XSIZE && y2 < NORMALMAP_YSIZE)
	{
		CalculateNormal (x2, y2);
	}

	BYTE byPatchNumX, byPatchNumY;
	byPatchNumX = max (x, 0) / PATCH_XSIZE;
	byPatchNumY = max (y, 0) / PATCH_YSIZE;
	int iPatchPos = byPatchNumY * PATCH_XCOUNT + byPatchNumX;
	if (iPatchPos >= 0)
		if (iPatchPos < PATCH_XCOUNT * PATCH_YCOUNT)
		{
			m_TerrainPatchList[iPatchPos].NeedUpdate (true);
		}
	if (y % PATCH_YSIZE == 0)
	{
		if (x % PATCH_YSIZE == 0)
		{
			int iPatchPos = (byPatchNumY - 1) * PATCH_XCOUNT + (byPatchNumX - 1);
			if (iPatchPos >= 0)
				if (iPatchPos < PATCH_XCOUNT * PATCH_YCOUNT)
				{
					m_TerrainPatchList[iPatchPos].NeedUpdate (true);
				}
		}
		else
		{
			int iPatchPos = (byPatchNumY - 1) * PATCH_XCOUNT + byPatchNumX;
			if (iPatchPos >= 0)
				if (iPatchPos < PATCH_XCOUNT * PATCH_YCOUNT)
				{
					m_TerrainPatchList[iPatchPos].NeedUpdate (true);
				}
		}
	}
	else if (x % PATCH_YSIZE == 0)
	{
		int iPatchPos = byPatchNumY * PATCH_XCOUNT + (byPatchNumX - 1);
		if (iPatchPos >= 0)
			if (iPatchPos < PATCH_XCOUNT * PATCH_YCOUNT)
			{
				m_TerrainPatchList[iPatchPos].NeedUpdate (true);
			}
	}

	if (!bRecursive)
	{
		return;
	}

	bool bWrongPut = false;
	int i, j;

	BYTE byTerrainNum;
	if (!m_pOwnerOutdoorMap->GetTerrainNumFromCoord (m_wX, m_wY, &byTerrainNum))
	{
		Tracef ("CTerrainAccessor::TerrainPutHeightmap : Can't Get TerrainNum from Coord %d, %d", m_wX, m_wY);
		byTerrainNum = 4;
	}

	if ((x >= 0 && x < HEIGHTMAP_XSIZE && y >= 0 && y < HEIGHTMAP_YSIZE) && ((x <= 1 || x >= HEIGHTMAP_XSIZE - 2) || (y <= 1 || y >= HEIGHTMAP_YSIZE - 2)))
	{
		for (i = -1; i <= 1; i++)
		{
			for (j = -1; j <= 1; j++)
			{
				if (i == j && i == 0)
				{
					continue;
				}
				else
				{
					int nx, ny;
					nx = x - j * YSIZE;
					ny = y - i * XSIZE;
					if ((nx <= 0 || ny <= 0 || nx >= HEIGHTMAP_RAW_XSIZE - 3 || ny >= HEIGHTMAP_RAW_YSIZE - 3) &&
						nx >= -1 && nx < HEIGHTMAP_RAW_XSIZE - 1 && ny >= -1 && ny < HEIGHTMAP_RAW_YSIZE - 1)
					{
						CTerrain* pTerrain;
						if (!m_pOwnerOutdoorMap->GetTerrainPointer (byTerrainNum + i * 3 + j, &pTerrain))
						{
							bWrongPut = true;
						}
						else
						{
							((CTerrainAccessor*)pTerrain)->TerrainPutHeightmap (nx, ny, val, false);
						}
					}
				}
			}
		}
	}
	if (bWrongPut)
	{
		Tracef ("It may affect nearby maps and cause the height value to be generated incorrectly. TerrainNum(%d), x(%d), y(%d)\n", byTerrainNum, x, y);
	}
}

CTerrainAccessor::CTerrainAccessor() : m_lpAttrTexture (NULL)
{
	m_isDestroied = FALSE;
}

CTerrainAccessor::~CTerrainAccessor()
{
	assert (!m_isDestroied);
	RAW_DeallocateAttrSplats();
	Clear();
	m_isDestroied = TRUE;
}
