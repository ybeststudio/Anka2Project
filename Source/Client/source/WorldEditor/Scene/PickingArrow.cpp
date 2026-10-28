#include "StdAfx.h"
#include "PickingArrow.h"

int CPickingArrows::m_dwIndex = 0;
const int c_iArrowBoxesCount = 3;
const float c_fArrowBoxes[c_iArrowBoxesCount * 2][6] =
{
	{ 30.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f },
	{  0.0f, 30.0f,  0.0f, 0.0f, 1.0f, 0.0f },
	{  0.0f,  0.0f, 30.0f, 0.0f, 0.0f, 1.0f },
	{ 30.0f, 30.0f,  0.0f, 1.0f, 1.0f, 0.0f },
	{  0.0f, 30.0f, 30.0f, 0.0f, 1.0f, 1.0f },
	{ 30.0f,  0.0f, 30.0f, 1.0f, 0.0f, 1.0f },
};

void CPickingArrows::SetCenterPosition (const D3DXVECTOR3 & rVector)
{
	m_v3Position = rVector;
}

void CPickingArrows::SetScale (float fScale)
{
	m_fScale = fScale;
}

void CPickingArrows::SetArrowSets (int dwIndex)
{
	m_dwIndex = 3 * dwIndex;
}


void CPickingArrows::GetCenterPosition (D3DXVECTOR3 * pv3Position)
{
	*pv3Position = m_v3Position;
}

int CPickingArrows::Picking()
{
	m_iHoldingDirection = -1;

	//float fu, fv, ft;
	for (int i = 0; i < c_iArrowBoxesCount; ++i)
	{
		/*if (IntersectCube(&ms_matWorld,
							m_v3Position.x + c_fArrowBoxes[i][0] * m_fScale - 8.0f * m_fScale,
							m_v3Position.y + c_fArrowBoxes[i][1] * m_fScale + 8.0f * m_fScale,
							m_v3Position.z + c_fArrowBoxes[i][2] * m_fScale - 8.0f * m_fScale,
							m_v3Position.x + c_fArrowBoxes[i][0] * m_fScale + 8.0f * m_fScale,
							m_v3Position.y + c_fArrowBoxes[i][1] * m_fScale - 8.0f * m_fScale,
							m_v3Position.z + c_fArrowBoxes[i][2] * m_fScale + 8.0f * m_fScale,
							ms_vtPickRayOrig, ms_vtPickRayDir,
							&fu, &fv, &ft))*/
		if (IntersectSphere (
				D3DXVECTOR3 (
					m_v3Position.x + c_fArrowBoxes[i + m_dwIndex][0] * m_fScale,
					m_v3Position.y + c_fArrowBoxes[i + m_dwIndex][1] * m_fScale,
					m_v3Position.z + c_fArrowBoxes[i + m_dwIndex][2] * m_fScale
				),
				5 * m_fScale, ms_vtPickRayOrig, ms_vtPickRayDir))
		{
			m_iHoldingDirection = i + m_dwIndex;
			GetCursorPosition (&m_v3HoldingPosition.x, &m_v3HoldingPosition.y, &m_v3HoldingPosition.z);
			return i + m_dwIndex;
		}
	}

	return -1;
}

void CPickingArrows::Unpicking()
{
	m_iHoldingDirection = -1;
}

bool CPickingArrows::Move()
{
	if (-1 == m_iHoldingDirection)
	{
		return false;
	}

	D3DXVECTOR3 v3Position;
	GetCursorPosition (&v3Position.x, &v3Position.y, &v3Position.z);
	switch (m_iHoldingDirection)
	{
		case DIRECTION_X:
			m_v3Position.x += (v3Position.x - m_v3HoldingPosition.x);
			break;
		case DIRECTION_Y:
			m_v3Position.y += (v3Position.y - m_v3HoldingPosition.y);
			break;
		case DIRECTION_Z:
			m_v3Position.z += (v3Position.z - m_v3HoldingPosition.z);
			break;
		case DIRECTION_XY:
			m_v3Position.x += (v3Position.x - m_v3HoldingPosition.x);
			m_v3Position.y += (v3Position.y - m_v3HoldingPosition.y);
			break;
		case DIRECTION_YZ:
			m_v3Position.y += (v3Position.y - m_v3HoldingPosition.y);
			m_v3Position.z += (v3Position.z - m_v3HoldingPosition.z);
			break;
		case DIRECTION_ZX:
			m_v3Position.x += (v3Position.x - m_v3HoldingPosition.x);
			m_v3Position.z += (v3Position.z - m_v3HoldingPosition.z);
			break;
	}

	m_v3Position.x = min (m_v3Position.x, +200.0f);
	m_v3Position.y = min (m_v3Position.y, +200.0f);
	m_v3Position.z = min (m_v3Position.z, +300.0f);
	m_v3Position.x = max (m_v3Position.x, -200.0f);
	m_v3Position.y = max (m_v3Position.y, -200.0f);
	m_v3Position.z = max (m_v3Position.z, -300.0f);

	m_v3HoldingPosition = v3Position;

	return true;
}

void CPickingArrows::Render()
{
	for (DWORD j = 0; j < c_iArrowBoxesCount; ++j)
	{
		DWORD i = j + m_dwIndex;
		SetDiffuseColor (c_fArrowBoxes[j][3], c_fArrowBoxes[j][4], c_fArrowBoxes[j][5]);
		RenderLine3d (m_v3Position.x, m_v3Position.y, m_v3Position.z,
					  m_v3Position.x + c_fArrowBoxes[j][0] * m_fScale,
					  m_v3Position.y + c_fArrowBoxes[j][1] * m_fScale,
					  m_v3Position.z + c_fArrowBoxes[j][2] * m_fScale);
		SetDiffuseColor (c_fArrowBoxes[i][3], c_fArrowBoxes[i][4], c_fArrowBoxes[i][5]);
		STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, ((DWORD)D3DXCOLOR (c_fArrowBoxes[i][3], c_fArrowBoxes[i][4], c_fArrowBoxes[i][5], 1.0f)));
		RenderLine3d (m_v3Position.x, m_v3Position.y, m_v3Position.z,
					  m_v3Position.x + c_fArrowBoxes[i][0] * m_fScale,
					  m_v3Position.y + c_fArrowBoxes[i][1] * m_fScale,
					  m_v3Position.z + c_fArrowBoxes[i][2] * m_fScale);
		RenderSphere (NULL,
					  m_v3Position.x + c_fArrowBoxes[i][0]*m_fScale,
					  m_v3Position.y + c_fArrowBoxes[i][1]*m_fScale,
					  m_v3Position.z + c_fArrowBoxes[i][2]*m_fScale,
					  5 * m_fScale);
		STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);
	}
}

CPickingArrows::CPickingArrows()
{
	m_fScale = 1.0f;
}
CPickingArrows::~CPickingArrows()
{
}