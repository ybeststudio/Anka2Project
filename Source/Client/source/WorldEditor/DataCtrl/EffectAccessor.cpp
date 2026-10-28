#include "StdAfx.h"
#include "../../EterPack/EterPackManager.h"

void CEffectElementBaseAccessor::SaveScript (int iBaseTab, FILE * File)
{
	PrintfTabs (File, iBaseTab + 0, "StartTime           %f\n", m_fStartTime);

	PrintfTabs (File, iBaseTab + 0, "List TimeEventPosition\n");
	PrintfTabs (File, iBaseTab + 0, "{\n");
	for (DWORD i = 0; i < m_TimeEventTablePosition.size(); ++i)
	{
		if (m_TimeEventTablePosition[i].m_iMovingType == MOVING_TYPE_BEZIER_CURVE)
		{
			PrintfTabs (File, iBaseTab + 1, "%f \"MOVING_TYPE_BEZIER_CURVE\" %f %f %f %f %f %f\n",
						m_TimeEventTablePosition[i].m_fTime,
						m_TimeEventTablePosition[i].m_vecPosition.x,
						m_TimeEventTablePosition[i].m_vecPosition.y,
						m_TimeEventTablePosition[i].m_vecPosition.z,
						m_TimeEventTablePosition[i].m_vecControlPoint.x,
						m_TimeEventTablePosition[i].m_vecControlPoint.y,
						m_TimeEventTablePosition[i].m_vecControlPoint.z
					   );
		}
		else
		{
			PrintfTabs (File, iBaseTab + 1, "%f \"MOVING_TYPE_DIRECT\" %f %f %f\n",
						m_TimeEventTablePosition[i].m_fTime,
						m_TimeEventTablePosition[i].m_vecPosition.x,
						m_TimeEventTablePosition[i].m_vecPosition.y,
						m_TimeEventTablePosition[i].m_vecPosition.z
					   );
		}
	}
	PrintfTabs (File, iBaseTab + 0, "}\n");
	PrintfTabs (File, iBaseTab + 0, "\n");
}

void CEffectElementBaseAccessor::SetStartTime (float fTime)
{
	m_fStartTime = fTime;
}

DWORD CEffectElementBaseAccessor::GetPositionCount()
{
	return m_TimeEventTablePosition.size();
}
bool CEffectElementBaseAccessor::GetTimePosition (DWORD dwIndex, float* pTime)
{
	if (dwIndex >= m_TimeEventTablePosition.size())
	{
		return false;
	}

	*pTime = m_TimeEventTablePosition[dwIndex].m_fTime;

	return true;
}
void CEffectElementBaseAccessor::SetTimePosition (DWORD dwIndex, float fTime)
{
	if (dwIndex >= m_TimeEventTablePosition.size())
	{
		return;
	}

	m_TimeEventTablePosition[dwIndex].m_fTime = fTime;
}
void CEffectElementBaseAccessor::DeletePosition (DWORD dwIndex)
{
	DeleteVectorItem<TEffectPosition> (&m_TimeEventTablePosition, dwIndex);
}
void CEffectElementBaseAccessor::InsertPosition (float fTime)
{
	TTimeEventTablePosition::iterator itor;
	itor = std::upper_bound (m_TimeEventTablePosition.begin(), m_TimeEventTablePosition.end(), fTime);

	TEffectPosition EffectPosition;
	EffectPosition.m_fTime = fTime;
	EffectPosition.m_iMovingType = MOVING_TYPE_DIRECT;
	EffectPosition.m_vecControlPoint = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	EffectPosition.m_vecPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

	if (!m_TimeEventTablePosition.empty())
	{
		GetPosition (fTime, EffectPosition.m_vecPosition);
	}

	m_TimeEventTablePosition.insert (itor, EffectPosition);
}

bool CEffectElementBaseAccessor::GetValuePosition (DWORD dwIndex, TEffectPosition** ppEffectPosition)
{
	if (dwIndex >= m_TimeEventTablePosition.size())
	{
		return false;
	}

	*ppEffectPosition = &m_TimeEventTablePosition[dwIndex];

	return true;
}

void CEffectElementBaseAccessor::SetValuePosition (DWORD dwIndex, const D3DXVECTOR3 & c_rVector)
{
	if (dwIndex >= m_TimeEventTablePosition.size())
	{
		return;
	}

	m_TimeEventTablePosition[dwIndex].m_vecPosition = c_rVector;
}

void CEffectElementBaseAccessor::SetValueControlPoint (DWORD dwIndex, const D3DXVECTOR3 & c_rVector)
{
	if (dwIndex >= m_TimeEventTablePosition.size())
	{
		return;
	}

	m_TimeEventTablePosition[dwIndex].m_vecControlPoint = c_rVector;
}

//////////////////////////////////////////////////////////////////////////

void CLightAccessor::SaveScript (int iBaseTab, FILE* File)
{
	PrintfTabs (File, iBaseTab + 0, "Duration            %f\n", m_fDuration);
	PrintfTabs (File, iBaseTab + 0, "LoopFlag            %d\n", m_bLoopFlag);
	PrintfTabs (File, iBaseTab + 0, "LoopCount           %d\n", m_iLoopCount);
	PrintfTabs (File, iBaseTab + 0, "AmbientColor        %f %f %f %f\n", m_cAmbient.r, m_cAmbient.g, m_cAmbient.b, m_cAmbient.a);
	PrintfTabs (File, iBaseTab + 0, "DiffuseColor        %f %f %f %f\n", m_cDiffuse.r, m_cDiffuse.g, m_cDiffuse.b, m_cDiffuse.a);

	PrintfTabs (File, iBaseTab + 0, "MaxRange            %f\n", m_fMaxRange);

	PrintfTabs (File, iBaseTab + 0, "Attenuation0        %f\n", m_fAttenuation0);
	PrintfTabs (File, iBaseTab + 0, "Attenuation1        %f\n", m_fAttenuation1);
	PrintfTabs (File, iBaseTab + 0, "Attenuation2        %f\n", m_fAttenuation2);

	PrintfTabs (File, iBaseTab + 0, "List TimeEventRange\n");
	PrintfTabs (File, iBaseTab + 0, "{\n");
	for (DWORD i = 0; i < m_TimeEventTableRange.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 1, "%f %f\n", m_TimeEventTableRange[i].m_fTime, m_TimeEventTableRange[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 0, "}\n");
}

float& CLightAccessor::GetMaxRangeReference()
{
	return m_fMaxRange;
}

BOOL& CLightAccessor::GetLoopFlagReference()
{
	return m_bLoopFlag;
}

float& CLightAccessor::GetDurationReference()
{
	return m_fDuration;
}

TTimeEventTableFloat* CLightAccessor::GetEmitterTimeEventTableRange()
{
	return &m_TimeEventTableRange;
}

float& CLightAccessor::GetAttenuation0Reference()
{
	return m_fAttenuation0;
}

float& CLightAccessor::GetAttenuation1Reference()
{
	return m_fAttenuation1;
}

float& CLightAccessor::GetAttenuation2Reference()
{
	return m_fAttenuation2;
}

D3DXCOLOR&   CLightAccessor::GetAmbientColorReference()
{
	return m_cAmbient;
}

D3DXCOLOR& CLightAccessor::GetDiffuseColorReference()
{
	return m_cDiffuse;
}

void CLightAccessor::SetAmbientColor (float fr, float fg, float fb)
{
	m_cAmbient.r = fr;
	m_cAmbient.g = fg;
	m_cAmbient.b = fb;
}

void CLightAccessor::SetDiffuseColor (float fr, float fg, float fb)
{
	m_cDiffuse.r = fr;
	m_cDiffuse.g = fg;
	m_cDiffuse.b = fb;

}

void CLightAccessor::ClearAccessor()
{
	Clear();
}

CLightAccessor::CLightAccessor()
{
}

CLightAccessor::~CLightAccessor()
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void CParticleAccessor::SaveScript (int iBaseTab, FILE * File, const char* c_szGlobalPath)
{
	// Emitter Property
	PrintfTabs (File, iBaseTab + 0, "Group EmitterProperty\n");
	PrintfTabs (File, iBaseTab + 0, "{\n");
	PrintfTabs (File, iBaseTab + 1, "MaxEmissionCount        %d\n", m_EmitterProperty.m_dwMaxEmissionCount);
	PrintfTabs (File, iBaseTab + 0, "\n");

	PrintfTabs (File, iBaseTab + 1, "CycleLength             %f\n", m_EmitterProperty.m_fCycleLength);
	PrintfTabs (File, iBaseTab + 1, "CycleLoopEnable         %d\n", m_EmitterProperty.m_bCycleLoopFlag);
	PrintfTabs (File, iBaseTab + 1, "LoopCount               %d\n", m_EmitterProperty.m_iLoopCount);
	PrintfTabs (File, iBaseTab + 0, "\n");

	PrintfTabs (File, iBaseTab + 1, "EmitterShape            %d\n", m_EmitterProperty.m_byEmitterShape);
	PrintfTabs (File, iBaseTab + 1, "EmitterAdvancedType     %d\n", m_EmitterProperty.m_byEmitterAdvancedType);
	switch (m_EmitterProperty.m_byEmitterShape)
	{
		case CEmitterProperty::EMITTER_SHAPE_ELLIPSE:
		case CEmitterProperty::EMITTER_SHAPE_SPHERE:
			PrintfTabs (File, iBaseTab + 1, "EmittingRadius          %f\n", m_EmitterProperty.m_fEmittingRadius);
			break;
		case CEmitterProperty::EMITTER_SHAPE_SQUARE:
			PrintfTabs (File, iBaseTab + 1, "EmittingSize            %f %f %f\n", m_EmitterProperty.m_v3EmittingSize.x, m_EmitterProperty.m_v3EmittingSize.y,
						m_EmitterProperty.m_v3EmittingSize.z);
			break;
	}
	PrintfTabs (File, iBaseTab + 1, "EmitterEmitFromEdgeFlag  %d\n", m_EmitterProperty.m_bEmitFromEdgeFlag);

	PrintfTabs (File, iBaseTab + 1, "EmittingDirection       %f %f %f\n", m_EmitterProperty.m_v3EmittingDirection.x, m_EmitterProperty.m_v3EmittingDirection.y,
				m_EmitterProperty.m_v3EmittingDirection.z);
	PrintfTabs (File, iBaseTab + 0, "\n");

	DWORD i;
	// Emitting Size
	PrintfTabs (File, iBaseTab + 1, "List TimeEventEmittingSize\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventEmittingSize.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventEmittingSize[i].m_fTime, m_EmitterProperty.m_TimeEventEmittingSize[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");
	// Emitting Rotation
	PrintfTabs (File, iBaseTab + 1, "List TimeEventEmittingAngularVelocity\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventEmittingAngularVelocity.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventEmittingAngularVelocity[i].m_fTime, m_EmitterProperty.m_TimeEventEmittingAngularVelocity[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");
	// Direction X
	PrintfTabs (File, iBaseTab + 1, "List TimeEventEmittingDirectionX\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventEmittingDirectionX.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventEmittingDirectionX[i].m_fTime, m_EmitterProperty.m_TimeEventEmittingDirectionX[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");
	// Direction Y
	PrintfTabs (File, iBaseTab + 1, "List TimeEventEmittingDirectionY\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventEmittingDirectionY.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventEmittingDirectionY[i].m_fTime, m_EmitterProperty.m_TimeEventEmittingDirectionY[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");
	// Direction Z
	PrintfTabs (File, iBaseTab + 1, "List TimeEventEmittingDirectionZ\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventEmittingDirectionZ.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventEmittingDirectionZ[i].m_fTime, m_EmitterProperty.m_TimeEventEmittingDirectionZ[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");
	// Velocity
	PrintfTabs (File, iBaseTab + 1, "List TimeEventEmittingVelocity\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventEmittingVelocity.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventEmittingVelocity[i].m_fTime, m_EmitterProperty.m_TimeEventEmittingVelocity[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	// Emission Count Per Second
	PrintfTabs (File, iBaseTab + 1, "List TimeEventEmissionCountPerSecond\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventEmissionCountPerSecond.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventEmissionCountPerSecond[i].m_fTime, m_EmitterProperty.m_TimeEventEmissionCountPerSecond[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	// LifeTime
	PrintfTabs (File, iBaseTab + 1, "List TimeEventLifeTime\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventLifeTime.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventLifeTime[i].m_fTime, m_EmitterProperty.m_TimeEventLifeTime[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	// SizeX
	PrintfTabs (File, iBaseTab + 1, "List TimeEventSizeX\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventSizeX.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventSizeX[i].m_fTime, m_EmitterProperty.m_TimeEventSizeX[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	// SizeY
	PrintfTabs (File, iBaseTab + 1, "List TimeEventSizeY\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_EmitterProperty.m_TimeEventSizeY.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_EmitterProperty.m_TimeEventSizeY[i].m_fTime, m_EmitterProperty.m_TimeEventSizeY[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");
	PrintfTabs (File, iBaseTab + 0, "}\n");
	PrintfTabs (File, iBaseTab + 0, "\n");

	// Particle Property
	PrintfTabs (File, iBaseTab + 0, "Group ParticleProperty\n");
	PrintfTabs (File, iBaseTab + 0, "{\n");

	PrintfTabs (File, iBaseTab + 1, "SrcBlendType                 %d\n", m_ParticleProperty.m_bySrcBlendType);
	PrintfTabs (File, iBaseTab + 1, "DestBlendType                %d\n", m_ParticleProperty.m_byDestBlendType);
	PrintfTabs (File, iBaseTab + 1, "ColorOperationType           %d\n", m_ParticleProperty.m_byColorOperationType);

	PrintfTabs (File, iBaseTab + 1, "BillboardType                %d\n", m_ParticleProperty.m_byBillboardType);

	PrintfTabs (File, iBaseTab + 1, "RotationType                 %d\n", m_ParticleProperty.m_byRotationType);
	PrintfTabs (File, iBaseTab + 1, "RotationSpeed                %f\n", m_ParticleProperty.m_fRotationSpeed);
	PrintfTabs (File, iBaseTab + 1, "RotationRandomStartingBegin  %d\n", m_ParticleProperty.m_wRotationRandomStartingBegin);
	PrintfTabs (File, iBaseTab + 1, "RotationRandomStartingEnd    %d\n", m_ParticleProperty.m_wRotationRandomStartingEnd);
	PrintfTabs (File, iBaseTab + 0, "\n");

	PrintfTabs (File, iBaseTab + 1, "AttachEnable                 %d\n", m_ParticleProperty.m_bAttachFlag);
	PrintfTabs (File, iBaseTab + 1, "StretchEnable                %d\n", m_ParticleProperty.m_bStretchFlag);
	PrintfTabs (File, iBaseTab + 0, "\n");

	PrintfTabs (File, iBaseTab + 1, "TexAniType                   %d\n", m_ParticleProperty.m_byTexAniType);
	PrintfTabs (File, iBaseTab + 1, "TexAniDelay                  %f\n", m_ParticleProperty.m_fTexAniDelay);
	PrintfTabs (File, iBaseTab + 1, "TexAniRandomStartFrameEnable %d\n", m_ParticleProperty.m_bTexAniRandomStartFrameFlag);
	PrintfTabs (File, iBaseTab + 0, "\n");

	PrintfTabs (File, iBaseTab + 1, "List TimeEventGravity\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_ParticleProperty.m_TimeEventGravity.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventGravity[i].m_fTime, m_ParticleProperty.m_TimeEventGravity[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	PrintfTabs (File, iBaseTab + 1, "List TimeEventAirResistance\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_ParticleProperty.m_TimeEventAirResistance.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventAirResistance[i].m_fTime, m_ParticleProperty.m_TimeEventAirResistance[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	// List - ScaleX
	PrintfTabs (File, iBaseTab + 1, "List TimeEventScaleX\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_ParticleProperty.m_TimeEventScaleX.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventScaleX[i].m_fTime, m_ParticleProperty.m_TimeEventScaleX[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	// List - ScaleY
	PrintfTabs (File, iBaseTab + 1, "List TimeEventScaleY\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_ParticleProperty.m_TimeEventScaleY.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventScaleY[i].m_fTime, m_ParticleProperty.m_TimeEventScaleY[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	{
		//		// List - ColorRed
		//		PrintfTabs(File, iBaseTab+1, "List TimeEventColorRed\n");
		//		PrintfTabs(File, iBaseTab+1, "{\n");
		//		for (i = 0; i < m_ParticleProperty.m_TimeEventColorRed.size(); ++i)
		//		{
		//			PrintfTabs(File, iBaseTab+2, "%f %f\n", m_ParticleProperty.m_TimeEventColorRed[i].m_fTime, m_ParticleProperty.m_TimeEventColorRed[i].m_Value);
		//		}
		//		PrintfTabs(File, iBaseTab+1, "}\n");
		//
		//		// List - ColorGreen
		//		PrintfTabs(File, iBaseTab+1, "List TimeEventColorGreen\n");
		//		PrintfTabs(File, iBaseTab+1, "{\n");
		//		for (i = 0; i < m_ParticleProperty.m_TimeEventColorGreen.size(); ++i)
		//		{
		//			PrintfTabs(File, iBaseTab+2, "%f %f\n", m_ParticleProperty.m_TimeEventColorGreen[i].m_fTime, m_ParticleProperty.m_TimeEventColorGreen[i].m_Value);
		//		}
		//		PrintfTabs(File, iBaseTab+1, "}\n");
		//
		//		// List - ColorBlue
		//		PrintfTabs(File, iBaseTab+1, "List TimeEventColorBlue\n");
		//		PrintfTabs(File, iBaseTab+1, "{\n");
		//		for (i = 0; i < m_ParticleProperty.m_TimeEventColorBlue.size(); ++i)
		//		{
		//			PrintfTabs(File, iBaseTab+2, "%f %f\n", m_ParticleProperty.m_TimeEventColorBlue[i].m_fTime, m_ParticleProperty.m_TimeEventColorBlue[i].m_Value);
		//		}
		//		PrintfTabs(File, iBaseTab+1, "}\n");
		//
		//		// List - Alpha
		//		PrintfTabs(File, iBaseTab+1, "List TimeEventAlpha\n");
		//		PrintfTabs(File, iBaseTab+1, "{\n");
		//		for (i = 0; i < m_ParticleProperty.m_TimeEventAlpha.size(); ++i)
		//		{
		//			PrintfTabs(File, iBaseTab+2, "%f %f\n", m_ParticleProperty.m_TimeEventAlpha[i].m_fTime, m_ParticleProperty.m_TimeEventAlpha[i].m_Value);
		//		}
		//		PrintfTabs(File, iBaseTab+1, "}\n");

		// List - ColorRed
		PrintfTabs (File, iBaseTab + 1, "List TimeEventColorRed\n");
		PrintfTabs (File, iBaseTab + 1, "{\n");
		for (i = 0; i < m_ParticleProperty.m_TimeEventColorRed.size(); ++i)
		{
			PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventColorRed[i].m_fTime, m_ParticleProperty.m_TimeEventColorRed[i].m_Value);
		}
		PrintfTabs (File, iBaseTab + 1, "}\n");

		// List - ColorGreen
		PrintfTabs (File, iBaseTab + 1, "List TimeEventColorGreen\n");
		PrintfTabs (File, iBaseTab + 1, "{\n");
		for (i = 0; i < m_ParticleProperty.m_TimeEventColorGreen.size(); ++i)
		{
			PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventColorGreen[i].m_fTime, m_ParticleProperty.m_TimeEventColorGreen[i].m_Value);
		}
		PrintfTabs (File, iBaseTab + 1, "}\n");

		// List - ColorBlue
		PrintfTabs (File, iBaseTab + 1, "List TimeEventColorBlue\n");
		PrintfTabs (File, iBaseTab + 1, "{\n");
		for (i = 0; i < m_ParticleProperty.m_TimeEventColorBlue.size(); ++i)
		{
			PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventColorBlue[i].m_fTime, m_ParticleProperty.m_TimeEventColorBlue[i].m_Value);
		}
		PrintfTabs (File, iBaseTab + 1, "}\n");

		// List - Alpha
		PrintfTabs (File, iBaseTab + 1, "List TimeEventAlpha\n");
		PrintfTabs (File, iBaseTab + 1, "{\n");
		for (i = 0; i < m_ParticleProperty.m_TimeEventAlpha.size(); ++i)
		{
			PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventAlpha[i].m_fTime, m_ParticleProperty.m_TimeEventAlpha[i].m_Value);
		}
		PrintfTabs (File, iBaseTab + 1, "}\n");
	}

	// List - Rotation
	PrintfTabs (File, iBaseTab + 1, "List TimeEventRotation\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_ParticleProperty.m_TimeEventRotation.size(); ++i)
	{
		PrintfTabs (File, iBaseTab + 2, "%f %f\n", m_ParticleProperty.m_TimeEventRotation[i].m_fTime, m_ParticleProperty.m_TimeEventRotation[i].m_Value);
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	// List - TextureFiles
	PrintfTabs (File, iBaseTab + 1, "List TextureFiles\n");
	PrintfTabs (File, iBaseTab + 1, "{\n");
	for (i = 0; i < m_ParticleProperty.m_TextureNameVector.size(); ++i)
	{
		std::string strLocalFileName;
		if (GetLocalFileName (c_szGlobalPath, m_ParticleProperty.m_TextureNameVector[i].c_str(), &strLocalFileName))
		{
			PrintfTabs (File, iBaseTab + 2, "\"%s\"\n", strLocalFileName.c_str());
		}
		else
		{
			PrintfTabs (File, iBaseTab + 2, "\"%s\"\n", m_ParticleProperty.m_TextureNameVector[i].c_str());
		}
	}
	PrintfTabs (File, iBaseTab + 1, "}\n");

	PrintfTabs (File, iBaseTab + 0, "}\n");
}

// Emitter
DWORD& CParticleAccessor::GetMaxEmissionCountReference()
{
	return m_EmitterProperty.m_dwMaxEmissionCount;
}

float& CParticleAccessor::GetCycleLengthReference()
{
	return m_EmitterProperty.m_fCycleLength;
}
BOOL& CParticleAccessor::GetCycleLoopFlagReference()
{
	return m_EmitterProperty.m_bCycleLoopFlag;
}
int& CParticleAccessor::GetLoopCountReference()
{
	return m_EmitterProperty.m_iLoopCount;
}
BYTE& CParticleAccessor::GetEmitterShapeReference()
{
	return m_EmitterProperty.m_byEmitterShape;
}
BYTE& CParticleAccessor::GetEmitterAdvancedTypeReference()
{
	return m_EmitterProperty.m_byEmitterAdvancedType;
}
BOOL& CParticleAccessor::GetEmitFromEdgeFlagReference()
{
	return m_EmitterProperty.m_bEmitFromEdgeFlag;
}
D3DXVECTOR3& CParticleAccessor::GetEmittingSizeReference()
{
	return m_EmitterProperty.m_v3EmittingSize;
}
float& CParticleAccessor::GetEmittingRadiusReference()
{
	return m_EmitterProperty.m_fEmittingRadius;
}

D3DXVECTOR3& CParticleAccessor::GetEmittingDirectionReference()
{
	return m_EmitterProperty.m_v3EmittingDirection;
}

TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableEmittingSize()
{
	return & m_EmitterProperty.m_TimeEventEmittingSize;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableEmittingAngularVelocity()
{
	return & m_EmitterProperty.m_TimeEventEmittingAngularVelocity;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableEmittingDirectionX()
{
	return & m_EmitterProperty.m_TimeEventEmittingDirectionX;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableEmittingDirectionY()
{
	return & m_EmitterProperty.m_TimeEventEmittingDirectionY;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableEmittingDirectionZ()
{
	return & m_EmitterProperty.m_TimeEventEmittingDirectionZ;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableEmittingVelocity()
{
	return & m_EmitterProperty.m_TimeEventEmittingVelocity;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableEmissionCount()
{
	return & m_EmitterProperty.m_TimeEventEmissionCountPerSecond;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableLifeTime()
{
	return & m_EmitterProperty.m_TimeEventLifeTime;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableSizeX()
{
	return & m_EmitterProperty.m_TimeEventSizeX;
}
TTimeEventTableFloat* CParticleAccessor::GetEmitterTimeEventTableSizeY()
{
	return & m_EmitterProperty.m_TimeEventSizeY;
}

// Particle Property
BYTE& CParticleAccessor::GetSrcBlendTypeReference()
{
	return m_ParticleProperty.m_bySrcBlendType;
}

BYTE& CParticleAccessor::GetDestBlendTypeReference()
{
	return m_ParticleProperty.m_byDestBlendType;
}

BYTE& CParticleAccessor::GetColorOperationTypeReference()
{
	return m_ParticleProperty.m_byColorOperationType;
}

BYTE& CParticleAccessor::GetBillboardTypeReference()
{
	return m_ParticleProperty.m_byBillboardType;
}

BYTE& CParticleAccessor::GetTexAniTypeReference()
{
	return m_ParticleProperty.m_byTexAniType;
}

BOOL& CParticleAccessor::GetTexAniRandomStartFrameFlagReference()
{
	return m_ParticleProperty.m_bTexAniRandomStartFrameFlag;
}

float& CParticleAccessor::GetTexAniDelayReference()
{
	return m_ParticleProperty.m_fTexAniDelay;
}

BOOL& CParticleAccessor::GetStretchFlagReference()
{
	return m_ParticleProperty.m_bStretchFlag;
}

BOOL& CParticleAccessor::GetAttachFlagReference()
{
	return m_ParticleProperty.m_bAttachFlag;
}

BYTE& CParticleAccessor::GetRotationTypeReference()
{
	return m_ParticleProperty.m_byRotationType;
}

float& CParticleAccessor::GetRotationSpeedReference()
{
	return m_ParticleProperty.m_fRotationSpeed;
}

WORD& CParticleAccessor::GetRotationRandomStartBeginReference()
{
	return m_ParticleProperty.m_wRotationRandomStartingBegin;
}
WORD& CParticleAccessor::GetRotationRandomStartEndReference()
{
	return m_ParticleProperty.m_wRotationRandomStartingEnd;
}

TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventGravity()
{
	return & m_ParticleProperty.m_TimeEventGravity;
}

TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventAirResistance()
{
	return & m_ParticleProperty.m_TimeEventAirResistance;
}

TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventScaleX()
{
	return & m_ParticleProperty.m_TimeEventScaleX;
}
TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventScaleY()
{
	return & m_ParticleProperty.m_TimeEventScaleY;
}
TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventColorRed()
{
	return & m_ParticleProperty.m_TimeEventColorRed;
}
TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventColorGreen()
{
	return & m_ParticleProperty.m_TimeEventColorGreen;
}
TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventColorBlue()
{
	return & m_ParticleProperty.m_TimeEventColorBlue;
}
TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventAlpha()
{
	return & m_ParticleProperty.m_TimeEventAlpha;
}
TTimeEventTableFloat* CParticleAccessor::GetParticleTimeEventRotation()
{
	return & m_ParticleProperty.m_TimeEventRotation;
}

void CParticleAccessor::InsertTexture (const char* c_szFileName)
{
	std::string strExtension;
	GetFileExtension (c_szFileName, strlen (c_szFileName), &strExtension);
	stl_lowers (strExtension);

	if (0 == strExtension.compare ("ifl"))
	{
		LPCVOID pMotionData;
		CMappedFile File;

		if (CEterPackManager::Instance().Get (File, c_szFileName, &pMotionData))
		{
			CMemoryTextFileLoader textFileLoader;
			std::vector<std::string> stTokenVector;

			textFileLoader.Bind (File.Size(), pMotionData);

			std::string strPathName;
			GetOnlyPathName (c_szFileName, strPathName);

			std::string strTextureFileName;
			for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
			{
				const std::string & c_rstrFileName = textFileLoader.GetLineString (i);

				if (c_rstrFileName.empty())
				{
					continue;
				}

				strTextureFileName = strPathName;
				strTextureFileName += c_rstrFileName;

				m_ParticleProperty.InsertTexture (strTextureFileName.c_str());
			}
		}
	}
	else
	{
		m_ParticleProperty.InsertTexture (c_szFileName);
	}
}
void CParticleAccessor::ClearOneTexture (DWORD dwIndex)
{
	if (dwIndex < m_ParticleProperty.m_ImageVector.size())
	{
		DeleteVectorItem<CGraphicImage*> (&m_ParticleProperty.m_ImageVector, dwIndex);
	}
	if (dwIndex < m_ParticleProperty.m_TextureNameVector.size())
	{
		DeleteVectorItem<std::string> (&m_ParticleProperty.m_TextureNameVector, dwIndex);
	}
}
void CParticleAccessor::ClearAllTexture()
{
	m_ParticleProperty.m_ImageVector.clear();
	m_ParticleProperty.m_TextureNameVector.clear();
}

DWORD CParticleAccessor::GetTextureCount()
{
	return m_ParticleProperty.m_TextureNameVector.size();
}

BOOL CParticleAccessor::GetImagePointer (DWORD dwIndex, CGraphicImage** ppImage)
{
	if (dwIndex >= GetTextureCount())
	{
		return FALSE;
	}

	*ppImage = m_ParticleProperty.m_ImageVector[dwIndex];

	return TRUE;
}

BOOL CParticleAccessor::GetTextureName (DWORD dwIndex, const char** pszTextureName)
{
	if (dwIndex >= GetTextureCount())
	{
		return FALSE;
	}

	*pszTextureName = m_ParticleProperty.m_TextureNameVector[dwIndex].c_str();

	return TRUE;
}

void CParticleAccessor::SetDefaultData()
{
	//// Emitter
	// Emission
	m_EmitterProperty.m_dwMaxEmissionCount = 30;

	m_EmitterProperty.m_fCycleLength = 0.5f;
	m_EmitterProperty.m_bCycleLoopFlag = FALSE;

	TTimeEventTypeFloat EmittingSize;
	EmittingSize.m_fTime = 0.0f;
	EmittingSize.m_Value = 0.0f;
	m_EmitterProperty.m_TimeEventEmittingSize.push_back (EmittingSize);

	TTimeEventTypeFloat EmittingAngularVelocity;
	EmittingAngularVelocity.m_fTime = 0.0f;
	EmittingAngularVelocity.m_Value = 0.0f;
	m_EmitterProperty.m_TimeEventEmittingAngularVelocity.push_back (EmittingAngularVelocity);

	TTimeEventTypeFloat DirectionX;
	DirectionX.m_fTime = 0.0f;
	DirectionX.m_Value = 0.0f;
	m_EmitterProperty.m_TimeEventEmittingDirectionX.push_back (DirectionX);
	TTimeEventTypeFloat DirectionY;
	DirectionY.m_fTime = 0.0f;
	DirectionY.m_Value = 0.0f;
	m_EmitterProperty.m_TimeEventEmittingDirectionY.push_back (DirectionY);
	TTimeEventTypeFloat DirectionZ;
	DirectionZ.m_fTime = 0.0f;
	DirectionZ.m_Value = 0.0f;
	m_EmitterProperty.m_TimeEventEmittingDirectionZ.push_back (DirectionZ);
	TTimeEventTypeFloat Velocity;
	Velocity.m_fTime = 0.0f;
	Velocity.m_Value = 0.0f;
	m_EmitterProperty.m_TimeEventEmittingVelocity.push_back (Velocity);

	TTimeEventTypeFloat EmissionCount;
	EmissionCount.m_fTime = 0.0f;
	EmissionCount.m_Value = 20.0f;
	m_EmitterProperty.m_TimeEventEmissionCountPerSecond.push_back (EmissionCount);

	// Emitter Shape
	m_EmitterProperty.m_byEmitterShape = CEmitterProperty::EMITTER_SHAPE_POINT;
	m_EmitterProperty.m_byEmitterAdvancedType = CEmitterProperty::EMITTER_ADVANCED_TYPE_FREE;
	m_EmitterProperty.m_bEmitFromEdgeFlag = FALSE;
	m_EmitterProperty.m_v3EmittingSize.x = 100.0f;
	m_EmitterProperty.m_v3EmittingSize.y = 100.0f;
	m_EmitterProperty.m_v3EmittingSize.z = 100.0f;
	m_EmitterProperty.m_fEmittingRadius = 100.0f;

	// LifeTime
	TTimeEventTypeFloat LifeTime;
	LifeTime.m_fTime = 0.0f;
	LifeTime.m_Value = 0.3f;
	m_EmitterProperty.m_TimeEventLifeTime.push_back (LifeTime);

	// Size
	TTimeEventTypeFloat SizeX;
	SizeX.m_fTime = 0.0f;
	SizeX.m_Value = 64.0f;
	m_EmitterProperty.m_TimeEventSizeX.push_back (SizeX);
	TTimeEventTypeFloat SizeY;
	SizeY.m_fTime = 0.0f;
	SizeY.m_Value = 64.0f;
	m_EmitterProperty.m_TimeEventSizeY.push_back (SizeY);

	//// Particle
	// Scale
	TTimeEventTypeFloat TimeEventScaleX;
	TimeEventScaleX.m_fTime = 0.0f;
	TimeEventScaleX.m_Value = 1.0f;
	m_ParticleProperty.m_TimeEventScaleX.push_back (TimeEventScaleX);
	TTimeEventTypeFloat TimeEventScaleY;
	TimeEventScaleY.m_fTime = 0.0f;
	TimeEventScaleY.m_Value = 1.0f;
	m_ParticleProperty.m_TimeEventScaleY.push_back (TimeEventScaleY);

	// Color
	TTimeEventTypeFloat TimeEventColor;
	TimeEventColor.m_fTime = 0.0f;
	TimeEventColor.m_Value = 1.0f;
	m_ParticleProperty.m_TimeEventColorRed.push_back (TimeEventColor);
	TimeEventColor.m_fTime = 0.0f;
	TimeEventColor.m_Value = 1.0f;
	m_ParticleProperty.m_TimeEventColorGreen.push_back (TimeEventColor);
	TimeEventColor.m_fTime = 0.0f;
	TimeEventColor.m_Value = 1.0f;
	m_ParticleProperty.m_TimeEventColorBlue.push_back (TimeEventColor);

	TTimeEventTypeFloat TimeEventAlpha;
	TimeEventAlpha.m_fTime = 0.0f;
	TimeEventAlpha.m_Value = 1.0f;
	m_ParticleProperty.m_TimeEventAlpha.push_back (TimeEventAlpha);
	TimeEventAlpha.m_fTime = 1.0f;
	TimeEventAlpha.m_Value = 0.0f;
	m_ParticleProperty.m_TimeEventAlpha.push_back (TimeEventAlpha);

	// Rotation
	TTimeEventTypeFloat Rotation;
	Rotation.m_fTime = 0.0f;
	Rotation.m_Value = 0.0f;
	m_ParticleProperty.m_TimeEventRotation.push_back (Rotation);
	m_ParticleProperty.m_byRotationType = CParticleProperty::ROTATION_TYPE_NONE;
	m_ParticleProperty.m_wRotationRandomStartingBegin = 0;
	m_ParticleProperty.m_wRotationRandomStartingEnd = 0;
	m_ParticleProperty.m_fRotationSpeed = 0.0f;

	m_ParticleProperty.m_byBillboardType = BILLBOARD_TYPE_ALL;

	m_ParticleProperty.m_bStretchFlag = FALSE;

	m_ParticleProperty.m_byTexAniType = CParticleProperty::TEXTURE_ANIMATION_TYPE_NONE;
	m_ParticleProperty.m_fTexAniDelay = 0.03f;
	m_ParticleProperty.m_bTexAniRandomStartFrameFlag = FALSE;

	// Acceleration
	m_ParticleProperty.m_TimeEventGravity.clear();
	m_ParticleProperty.m_TimeEventAirResistance.clear();
}

void CParticleAccessor::ClearAccessor()
{
	Clear();
}

CParticleAccessor::CParticleAccessor()
{
}
CParticleAccessor::~CParticleAccessor()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CMeshAccessor::SaveScript (int iBaseTab, FILE * File, const char* c_szGlobalPath)
{
	std::string strLocalFileName;
	if (GetLocalFileName (c_szGlobalPath, m_strMeshFileName.c_str(), &strLocalFileName))
	{
		PrintfTabs (File, iBaseTab + 0, "MeshFileName               \t\"%s\"\n", strLocalFileName.c_str());
	}
	else
	{
		PrintfTabs (File, iBaseTab + 0, "MeshFileName               \t\"%s\"\n", m_strMeshFileName.c_str());
	}

	PrintfTabs (File, iBaseTab + 0, "MeshAnimationLoopEnable    \t%d\n", m_isMeshAnimationLoop);
	PrintfTabs (File, iBaseTab + 0, "MeshAnimationLoopCount     \t%d\n", m_iMeshAnimationLoopCount);

	PrintfTabs (File, iBaseTab + 0, "MeshAnimationFrameDelay    \t%f\n", m_fMeshAnimationFrameDelay);
	PrintfTabs (File, iBaseTab + 0, "\n");

	PrintfTabs (File, iBaseTab + 0, "MeshElementCount           \t%d\n", m_MeshDataVector.size());

	for (DWORD i = 0; i < m_MeshDataVector.size(); ++i)
	{
		TMeshData & rMeshData = m_MeshDataVector[i];

		PrintfTabs (File, iBaseTab + 0, "Group MeshElement%02d\n", i);
		PrintfTabs (File, iBaseTab + 0, "{\n");
		PrintfTabs (File, iBaseTab + 1, "BillboardType              \t%d\n", rMeshData.byBillboardType);
		PrintfTabs (File, iBaseTab + 1, "BlendingEnable             \t%d\n", rMeshData.bBlendingEnable);
		PrintfTabs (File, iBaseTab + 1, "BlendingSrcType            \t%d\n", rMeshData.byBlendingSrcType);
		PrintfTabs (File, iBaseTab + 1, "BlendingDestType           \t%d\n", rMeshData.byBlendingDestType);
		PrintfTabs (File, iBaseTab + 1, "\n");

		PrintfTabs (File, iBaseTab + 1, "TextureAnimationLoopEnable \t%d\n", rMeshData.bTextureAnimationLoopEnable);
		PrintfTabs (File, iBaseTab + 1, "TextureAnimationFrameDelay \t%f\n", rMeshData.fTextureAnimationFrameDelay);
		PrintfTabs (File, iBaseTab + 1, "TextureAnimationStartFrame \t%d\n", rMeshData.dwTextureAnimationStartFrame);
		PrintfTabs (File, iBaseTab + 1, "\n");

		PrintfTabs (File, iBaseTab + 1, "ColorOperationType         \t%d\n", rMeshData.byColorOperationType);
		PrintfTabs (File, iBaseTab + 1, "ColorFactor                \t%f %f %f %f\n", rMeshData.ColorFactor.r, rMeshData.ColorFactor.g, rMeshData.ColorFactor.b, 1.0f);
		PrintfTabs (File, iBaseTab + 1, "List TimeEventAlpha\n");
		PrintfTabs (File, iBaseTab + 1, "{\n");
		TTimeEventTableFloat::iterator it;
		for (it = rMeshData.TimeEventAlpha.begin(); it != rMeshData.TimeEventAlpha.end(); ++it)
		{
			PrintfTabs (File, iBaseTab + 2, "%f %f\n", it->m_fTime, it->m_Value);
		}
		PrintfTabs (File, iBaseTab + 1, "}\n");
		PrintfTabs (File, iBaseTab + 0, "}\n");
	}
}

void CMeshAccessor::SetMeshAnimationFlag (bool bFlag)
{
	m_isMeshAnimationLoop = bFlag;
}

void CMeshAccessor::SetMeshAnimationLoopCount (int iNewCount)
{
	m_iMeshAnimationLoopCount = iNewCount;
}

void CMeshAccessor::SetMeshAnimationFrameDelay (float fDelay)
{
	m_fMeshAnimationFrameDelay = fDelay;
}

void CMeshAccessor::SetMeshFileName (const char* c_szFileName)
{
	m_strMeshFileName = c_szFileName;

	LoadMeshInstance();
}

void CMeshAccessor::LoadMeshInstance()
{
	if (m_pEffectMesh)
	{
		m_pEffectMesh->Release();
		m_pEffectMesh = NULL;
	}

	if (m_strMeshFileName.empty())
	{
		return;
	}

	m_pEffectMesh = (CEffectMesh*) CResourceManager::Instance().GetResourcePointer (m_strMeshFileName.c_str());
	m_pEffectMesh->AddReference();

	ReserveMeshData (m_pEffectMesh->GetMeshCount());
}

DWORD CMeshAccessor::GetMeshElementCount()
{
	if (!m_pEffectMesh)
	{
		return 0;
	}

	return m_pEffectMesh->GetMeshCount();
}
BOOL CMeshAccessor::GetMeshElementDataPointer (DWORD dwIndex, CEffectMesh::TEffectMeshData** ppMeshElementData)
{
	if (!m_pEffectMesh)
	{
		return FALSE;
	}

	if (!m_pEffectMesh->GetMeshElementPointer (dwIndex, ppMeshElementData))
	{
		return FALSE;
	}

	return TRUE;
}

void CMeshAccessor::ClearAccessor()
{
	if (m_pEffectMesh)
	{
		m_pEffectMesh->Release();
	}
}

CMeshAccessor::CMeshAccessor()
{
	m_pEffectMesh = NULL;
}
CMeshAccessor::~CMeshAccessor()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

CEffectAccessor::~CEffectAccessor()
{
	Clear();
}

void CEffectAccessor::Clear()
{
	m_fBoundingSphereRadius = 0.0f;

	for (DWORD i = 0; i < m_ElementVector.size(); ++i)
	{
		switch (m_ElementVector[i].iType)
		{
			case EFFECT_ELEMENT_TYPE_MESH:
				m_ElementVector[i].pMesh->ClearAccessor();
				delete m_ElementVector[i].pMesh;
				break;

			case EFFECT_ELEMENT_TYPE_PARTICLE:
				m_ElementVector[i].pParticle->ClearAccessor();
				delete m_ElementVector[i].pParticle;
				break;
			case EFFECT_ELEMENT_TYPE_LIGHT:
				m_ElementVector[i].pLight->ClearAccessor();
				delete m_ElementVector[i].pLight;
				break;
		}
	}

	m_ParticleVector.clear();
	m_MeshVector.clear();
	m_LightVector.clear();

	m_ElementVector.clear();
}

CParticleSystemData* CEffectAccessor::AllocParticle()
{
	CEffectAccessor::TEffectElement EffectElement;
	memset (&EffectElement, 0, sizeof (TEffectElement));

	EffectElement.bVisible = TRUE;

	EffectElement.strName = "Particle";
	EffectElement.iType = CEffectAccessor::EFFECT_ELEMENT_TYPE_PARTICLE;

	EffectElement.pParticle = new CParticleAccessor;
	EffectElement.pBase = (CEffectElementBaseAccessor*)EffectElement.pParticle;

	EffectElement.pBase->InsertPosition (0.0f);
	EffectElement.pBase->SetValuePosition (0, D3DXVECTOR3 (0.0f, 0.0f, 0.0f));

	m_ElementVector.push_back (EffectElement);

	return EffectElement.pParticle;
}

CEffectMeshScript* CEffectAccessor::AllocMesh()
{
	CEffectAccessor::TEffectElement EffectElement;
	memset (&EffectElement, 0, sizeof (TEffectElement));

	EffectElement.bVisible = TRUE;

	EffectElement.strName = "Mesh";
	EffectElement.iType = CEffectAccessor::EFFECT_ELEMENT_TYPE_MESH;

	EffectElement.pMesh = new CMeshAccessor;
	EffectElement.pBase = (CEffectElementBaseAccessor*)EffectElement.pMesh;

	EffectElement.pBase->InsertPosition (0.0f);
	EffectElement.pBase->SetValuePosition (0, D3DXVECTOR3 (0.0f, 0.0f, 0.0f));

	m_ElementVector.push_back (EffectElement);

	return EffectElement.pMesh;
}

CLightData* CEffectAccessor::AllocLight()
{
	CEffectAccessor::TEffectElement EffectElement;
	memset (&EffectElement, 0, sizeof (TEffectElement));

	EffectElement.bVisible = TRUE;

	EffectElement.strName = "Light";
	EffectElement.iType = CEffectAccessor::EFFECT_ELEMENT_TYPE_LIGHT;

	EffectElement.pLight = new CLightAccessor;
	EffectElement.pBase = (CEffectElementBaseAccessor*)EffectElement.pLight;

	EffectElement.pBase->InsertPosition (0.0f);
	EffectElement.pBase->SetValuePosition (0, D3DXVECTOR3 (0.0f, 0.0f, 0.0f));

	m_ElementVector.push_back (EffectElement);

	return EffectElement.pLight;
}

void CEffectAccessor::SetVisible (DWORD dwIndex, BOOL bVisible)
{
	if (dwIndex >= m_ElementVector.size())
	{
		return;
	}

	CEffectAccessor::TEffectElement & rElement = m_ElementVector[dwIndex];
	rElement.bVisible = bVisible;
}

BOOL CEffectAccessor::GetVisible (DWORD dwIndex)
{
	if (dwIndex >= m_ElementVector.size())
	{
		return FALSE;
	}

	CEffectAccessor::TEffectElement & rElement = m_ElementVector[dwIndex];
	return rElement.bVisible;
}

float CEffectAccessor::GetLifeTime()
{
	return m_fLifeTime;
}

void CEffectAccessor::SetLifeTime (float fLifeTime)
{
	m_fLifeTime = fLifeTime;
}

void CEffectAccessor::SetBoundingSphereRadius (float fRadius)
{
	m_fBoundingSphereRadius = fRadius;
}

void CEffectAccessor::SetBoundingSpherePosition (const D3DXVECTOR3 & c_rv3Pos)
{
	m_v3BoundingSpherePosition = c_rv3Pos;
}

DWORD CEffectAccessor::GetElementCount()
{
	return m_ElementVector.size();
}
BOOL CEffectAccessor::GetElement (DWORD dwIndex, TEffectElement** ppElement)
{
	if (dwIndex >= m_ElementVector.size())
	{
		return FALSE;
	}

	*ppElement = &m_ElementVector[dwIndex];

	return TRUE;
}

BOOL CEffectAccessor::GetElementParticle (DWORD dwIndex, CParticleAccessor** ppParticleAccessor)
{
	if (dwIndex >= m_ElementVector.size())
	{
		return FALSE;
	}

	if (EFFECT_ELEMENT_TYPE_PARTICLE != m_ElementVector[dwIndex].iType)
	{
		return FALSE;
	}

	*ppParticleAccessor = m_ElementVector[dwIndex].pParticle;

	return TRUE;
}

void CEffectAccessor::DeleteElement (DWORD dwIndex)
{
	if (dwIndex >= m_ElementVector.size())
	{
		return;
	}

	CEffectAccessor::TEffectElement & rElement = m_ElementVector[dwIndex];
	if (rElement.pParticle)
	{
		delete rElement.pParticle;
		rElement.pParticle = NULL;
	}
	if (rElement.pMesh)
	{
		delete rElement.pMesh;
		rElement.pMesh = NULL;
	}
	if (rElement.pLight)
	{
		delete rElement.pLight;
		rElement.pLight = NULL;
	}

	DeleteVectorItem<TEffectElement> (&m_ElementVector, dwIndex);
}

void CEffectAccessor::SwapElement (DWORD dwIndex1, DWORD dwIndex2)
{
	CEffectAccessor::TEffectElement temp = m_ElementVector[dwIndex1];
	m_ElementVector[dwIndex1] = m_ElementVector[dwIndex2];
	m_ElementVector[dwIndex2] = temp;
}

void CEffectInstanceAccessor::SetEffectDataAccessorPointer (CEffectAccessor * pAccessor)
{
	CEffectAccessor::TEffectElement * pElement;
	for (DWORD i = 0; i < pAccessor->GetElementCount(); ++i)
	{
		if (!pAccessor->GetElement (i, &pElement))
		{
			continue;
		}

		if (!pElement->bVisible)
		{
			continue;
		}

		if (!pElement->pBase->isData())
		{
			continue;
		}

		switch (pElement->iType)
		{
			case CEffectAccessor::EFFECT_ELEMENT_TYPE_PARTICLE:
				__SetParticleData (pElement->pParticle);
				break;
			case CEffectAccessor::EFFECT_ELEMENT_TYPE_MESH:
				__SetMeshData (pElement->pMesh);
				break;
			case CEffectAccessor::EFFECT_ELEMENT_TYPE_LIGHT:
				__SetLightData (pElement->pLight);
				break;
		}
	}
}
