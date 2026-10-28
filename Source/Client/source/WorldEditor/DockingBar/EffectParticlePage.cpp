// EffectParticlePage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectParticlePage.h"
#include "../Dialog/ColorAnimationDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/*class CTimeEventFloatAccessor : public CEffectTimeEventGraph::IValueAccessor
{
public:
	CTimeEventFloatAccessor(){}
	~CTimeEventFloatAccessor(){}

	TTimeEventTableFloat * m_pTimeEventLifeTime;

	void SetTablePointer(TTimeEventTableFloat * pTimeEventLifeTime)
	{
		m_pTimeEventLifeTime = pTimeEventLifeTime;
	}

	DWORD GetValueCount()
	{
		return m_pTimeEventLifeTime->size();
	}

	bool GetTime(DWORD dwIndex, float * pfTime)
	{
		if (dwIndex >= m_pTimeEventLifeTime->size())
			return false;

		*pfTime = m_pTimeEventLifeTime->at(dwIndex).m_fTime;

		return true;
	}

	bool GetValue(DWORD dwIndex, float * pfValue)
	{
		if (dwIndex >= m_pTimeEventLifeTime->size())
			return false;

		*pfValue = m_pTimeEventLifeTime->at(dwIndex).m_Value;

		return true;
	}

	void SetTime(DWORD dwIndex, float fTime)
	{
		if (dwIndex >= m_pTimeEventLifeTime->size())
			return;

		m_pTimeEventLifeTime->at(dwIndex).m_fTime = fTime;
	}
	void SetValue(DWORD dwIndex, float fValue)
	{
		if (dwIndex >= m_pTimeEventLifeTime->size())
			return;

		m_pTimeEventLifeTime->at(dwIndex).m_Value = fValue;
	}

	void Insert(float fTime, float fValue)
	{
		InsertItemTimeEventFloat(m_pTimeEventLifeTime, fTime, fValue);
	}
	void Delete(DWORD dwIndex)
	{
		DeleteVectorItem<TTimeEventTypeFloat>(m_pTimeEventLifeTime, dwIndex);
	}
};*/


CTimeEventFloatAccessor EmittingSize;
CTimeEventFloatAccessor EmittingAngularVelocity;
CTimeEventFloatAccessor EmittingDirectionX;
CTimeEventFloatAccessor EmittingDirectionY;
CTimeEventFloatAccessor EmittingDirectionZ;
CTimeEventFloatAccessor EmittingVelocity;
CTimeEventFloatAccessor EmissionCountAccessor;
CTimeEventFloatAccessor LifeTimeAccessor;
CTimeEventFloatAccessor SizeXAccessor;
CTimeEventFloatAccessor SizeYAccessor;

CTimeEventFloatAccessor ScaleXAccessor;
CTimeEventFloatAccessor ScaleYAccessor;
CTimeEventFloatAccessor ColorRedAccessor;
CTimeEventFloatAccessor ColorGreenAccessor;
CTimeEventFloatAccessor ColorBlueAccessor;
CTimeEventFloatAccessor AlphaAccessor;
CTimeEventFloatAccessor RotationAccessor;
CTimeEventFloatAccessor GravityAccessor;
CTimeEventFloatAccessor AirResistanceAccessor;

CEffectParticlePage* CEffectParticlePage::ms_pThis = NULL;

/////////////////////////////////////////////////////////////////////////////
// CEffectParticlePage dialog

CEffectParticlePage::CEffectParticlePage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CEffectParticlePage::IDD, pParent)
{
	ms_pThis = this;

	m_pColorAnimationDlg = NULL;

	//{{AFX_DATA_INIT(CEffectParticlePage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEffectParticlePage::~CEffectParticlePage()
{
	if (m_pColorAnimationDlg)
	{
		delete m_pColorAnimationDlg;
	}
}

void CEffectParticlePage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CEffectParticlePage)
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_OPERATION_TYPE, m_ctrlOperationType);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_PREVIEW, m_ctrlPreview);
	DDX_Control (pDX, IDD_EFFECT_PARTICLE_EMITTER_ADVANCED_TYPE, m_ctrlAdvancedType);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_BLEND_DEST, m_ctrlBlendDestType);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_BLEND_SRC, m_ctrlBlendSrcType);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_ROTATION_SPEED, m_ctrlParticleRotationSpeed);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_ROTATION_TYPE, m_ctrlRotationType);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_BILLBOARD_TYPE, m_ctrlBillboardType);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_TEXTURE_ANIMATION_DELAY, m_ctrlTexAniDelay);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_TEXTURE_ANIMATION_TYPE, m_ctrlTexAniType);
	DDX_Control (pDX, IDC_EFFECT_PARTICLE_TEXTURE_LIST, m_ctrlTextureList);
	DDX_Control (pDX, IDD_EFFECT_PARTICLE_EMITTER_SHAPE, m_ctrlEmitterShape);
	DDX_Control (pDX, IDD_EFFECT_PARTICLE_PARTICLE_GRAPH_LIST, m_ctrlParticleGraphList);
	DDX_Control (pDX, IDD_EFFECT_PARTICLE_PARTICLE_GRAPH, m_ctrlParticleGraph);
	DDX_Control (pDX, IDD_EFFECT_PARTICLE_EMITTER_GRAPH_LIST, m_ctrlEmitterGraphList);
	DDX_Control (pDX, IDD_EFFECT_PARTICLE_EMITTER_GRAPH, m_ctrlEmitterGraph);
	//}}AFX_DATA_MAP

	m_ctrlTexAniDelay.SetRangeMin (0);
	m_ctrlTexAniDelay.SetRangeMax (500);
	m_ctrlParticleRotationSpeed.SetRangeMin (0);
	m_ctrlParticleRotationSpeed.SetRangeMax (500);
}

BEGIN_MESSAGE_MAP (CEffectParticlePage, CPageCtrl)
	//{{AFX_MSG_MAP(CEffectParticlePage)
	ON_BN_CLICKED (IDD_EFFECT_PARTICLE_EMITTER_EMIT_FROM_EDGE, OnCheckEmitterFilledEnable)
	ON_BN_CLICKED (IDC_EFFECT_PARTICLE_INSERT_TEXTURE, OnInsertTexture)
	ON_BN_CLICKED (IDC_EFFECT_PARTICLE_CLEAR_ALL_TEXTURE, OnClearAllTexture)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_EFFECT_PARTICLE_TEXTURE_ANIMATION_RANDOM_START_FRAME, OnCheckTextureAnimationRandomStartFrame)
	ON_BN_CLICKED (IDC_EFFECT_PARTICLE_STRETCH_FLAG, OnCheckStretchFlag)
	ON_BN_CLICKED (IDC_EFFECT_PARTICLE_CLEAR_ONE_TEXTURE, OnClearOneTexture)
	ON_BN_CLICKED (IDD_EFFECT_PARTICLE_EMITTER_CYCLE_LOOP, OnCheckCycleLoopFlag)
	ON_BN_CLICKED (IDC_EFFECT_PARTICLE_ATTACH_FLAG, OnCheckAttachFlag)
	ON_BN_CLICKED (IDC_EFFECT_PARTICLE_COLOR_ANIMATION, OnColorAnimation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectParticlePage normal functions

BOOL CEffectParticlePage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CEffectParticlePage::IDD, pParent))
	{
		return FALSE;
	}

	if (!m_ctrlBlendSrcType.Create())
	{
		return FALSE;
	}
	if (!m_ctrlBlendDestType.Create())
	{
		return FALSE;
	}
	if (!m_ctrlOperationType.Create())
	{
		return FALSE;
	}

	m_ctrlEmitterGraph.Initialize (WINDOW_TIMER_ID_EMITTER_GRAPH);
	m_ctrlParticleGraph.Initialize (WINDOW_TIMER_ID_PARTICLE_GRAPH);

	m_ctrlEmitterShape.InsertString (CEmitterProperty::EMITTER_SHAPE_POINT, "Point");
	m_ctrlEmitterShape.InsertString (CEmitterProperty::EMITTER_SHAPE_ELLIPSE, "Ellipse");
	m_ctrlEmitterShape.InsertString (CEmitterProperty::EMITTER_SHAPE_SQUARE, "Square");
	m_ctrlEmitterShape.InsertString (CEmitterProperty::EMITTER_SHAPE_SPHERE, "Sphere");
	m_ctrlEmitterShape.SelectString (0, "Point");

	m_ctrlAdvancedType.InsertString (CEmitterProperty::EMITTER_ADVANCED_TYPE_FREE, "Free");
	m_ctrlAdvancedType.InsertString (CEmitterProperty::EMITTER_ADVANCED_TYPE_OUTER, "Outer");
	m_ctrlAdvancedType.InsertString (CEmitterProperty::EMITTER_ADVANCED_TYPE_INNER, "Inner");
	m_ctrlAdvancedType.SelectString (0, "Free");

	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_EMITTING_SIZE, "Emitting Size");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_EMITTING_ANGULAR_VELOCITY, "Emit AngularVel");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_DIRECTION_X, "Direction X");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_DIRECTION_Y, "Direction Y");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_DIRECTION_Z, "Direction Z");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_VELOCITY, "Velocity");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_EMISSION_COUNT, "Emission Count");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_LIFE_TIME, "Life Time");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_SIZE_X, "Size X");
	m_ctrlEmitterGraphList.InsertString (EMITTER_TIME_EVENT_GRAPH_SIZE_Y, "Size Y");
	m_ctrlEmitterGraphList.SelectString (0, "Emitting Size");

	m_ctrlTextureList.InsertString (0, "None");
	m_ctrlTextureList.SelectString (0, "None");

	m_ctrlTexAniType.InsertString (CParticleProperty::TEXTURE_ANIMATION_TYPE_NONE, "None");
	m_ctrlTexAniType.InsertString (CParticleProperty::TEXTURE_ANIMATION_TYPE_CW, "CW");
	m_ctrlTexAniType.InsertString (CParticleProperty::TEXTURE_ANIMATION_TYPE_CCW, "CCW");
	m_ctrlTexAniType.InsertString (CParticleProperty::TEXTURE_ANIMATION_TYPE_RANDOM_FRAME, "Random Frame");
	m_ctrlTexAniType.InsertString (CParticleProperty::TEXTURE_ANIMATION_TYPE_RANDOM_DIRECTION, "Random Direction");
	m_ctrlTexAniType.SelectString (0, "None");

	m_ctrlBillboardType.InsertString (BILLBOARD_TYPE_NONE,  "None Billboarding");
	m_ctrlBillboardType.InsertString (BILLBOARD_TYPE_ALL,   "All Axis Billboarding");
	m_ctrlBillboardType.InsertString (BILLBOARD_TYPE_Y,     "Only Y Axis Billboarding");
	m_ctrlBillboardType.InsertString (BILLBOARD_TYPE_LIE,   "Lie Billboarding");
	m_ctrlBillboardType.InsertString (BILLBOARD_TYPE_2FACE, "2 Face Billboarding");
	m_ctrlBillboardType.InsertString (BILLBOARD_TYPE_3FACE, "3 Face Billboarding");
	//m_ctrlBillboardType.InsertString(BILLBOARD_TYPE_RAY,   "Ray Effect");
	m_ctrlBillboardType.SelectString (0, "None Billboarding");

	m_ctrlRotationType.InsertString (CParticleProperty::ROTATION_TYPE_NONE, "None");
	m_ctrlRotationType.InsertString (CParticleProperty::ROTATION_TYPE_TIME_EVENT, "Time Event");
	m_ctrlRotationType.InsertString (CParticleProperty::ROTATION_TYPE_CW, "Rotation To CW");
	m_ctrlRotationType.InsertString (CParticleProperty::ROTATION_TYPE_CCW, "Rotation To CCW");
	m_ctrlRotationType.InsertString (CParticleProperty::ROTATION_TYPE_RANDOM_DIRECTION, "Random Direction");
	m_ctrlRotationType.SelectString (0, "None");

	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_SCALE_X, "Scale X");
	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_SCALE_Y, "Scale Y");
	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_ROTATION, "Rotation");
	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_GRAVITY, "Gravity");
	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_AIR_RESISTANCE, "Air Resistance");
	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_COLOR_RED, "Color Red");
	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_COLOR_GREEN, "Color Green");
	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_COLOR_BLUE, "Color Blue");
	m_ctrlParticleGraphList.InsertString (PARTICLE_TIME_EVENT_GRAPH_ALPHA, "Alpha");
	m_ctrlParticleGraphList.SelectString (0, "Scale X");

	m_pColorAnimationDlg = new CColorAnimationDlg;
	m_pColorAnimationDlg->Create (this);
	m_pColorAnimationDlg->ShowWindow (FALSE);

	return TRUE;
}

void CEffectParticlePage::CallBack()
{
}

void CEffectParticlePage::UpdateUI()
{
	RefreshTextureList();
}

void CEffectParticlePage::SetData (DWORD dwIndex)
{
	m_dwElementIndex = dwIndex;

	SelectEmitterTimeEvent (0);
	SelectParticleTimeEvent (0);

	/////

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (m_dwElementIndex, &pElement))
	{
		return;
	}

	CParticleAccessor * pAccessor = pElement->pParticle;

	// Emitter Property
	SetDialogIntegerText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_MAX_PARTICLE, pAccessor->GetMaxEmissionCountReference());

	SetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_CYCLE_LENGTH, pAccessor->GetCycleLengthReference());
	CheckDlgButton (IDD_EFFECT_PARTICLE_EMITTER_CYCLE_LOOP, pAccessor->GetCycleLoopFlagReference());

	SetDialogIntegerText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_LOOP_COUNT, pAccessor->GetLoopCountReference());

	m_ctrlEmitterShape.SetCurSel (pAccessor->GetEmitterShapeReference());
	m_ctrlAdvancedType.SetCurSel (pAccessor->GetEmitterAdvancedTypeReference());
	SelectEmitterShape (pAccessor->GetEmitterShapeReference());
	SetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_X, pAccessor->GetEmittingDirectionReference().x);
	SetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_Y, pAccessor->GetEmittingDirectionReference().y);
	SetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_Z, pAccessor->GetEmittingDirectionReference().z);
	SetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_SIZE_X, pAccessor->GetEmittingSizeReference().x);
	SetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_SIZE_Y, pAccessor->GetEmittingSizeReference().y);
	SetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_SIZE_Z, pAccessor->GetEmittingSizeReference().z);
	SetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_RADIUS, pAccessor->GetEmittingRadiusReference());
	CheckDlgButton (IDD_EFFECT_PARTICLE_EMITTER_EMIT_FROM_EDGE, pAccessor->GetEmitFromEdgeFlagReference());

	//// Particle Property
	// Texture
	RefreshTextureList();

	// TexAniType
	CString strTexAniType;
	m_ctrlTexAniType.GetLBText (pAccessor->GetTexAniTypeReference(), strTexAniType);
	m_ctrlTexAniType.SelectString (0, strTexAniType);
	// TexAniDelay
	m_ctrlTexAniDelay.SetPos (int (pAccessor->GetTexAniDelayReference() * 1000.0f));
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_PARTICLE_TEXTURE_ANIMATION_DELAY_PRINT, pAccessor->GetTexAniDelayReference());
	// TexAniRandomStartFrame
	CheckDlgButton (IDC_EFFECT_PARTICLE_TEXTURE_ANIMATION_RANDOM_START_FRAME, pAccessor->GetTexAniRandomStartFrameFlagReference());

	// Billboard
	m_ctrlBillboardType.SetCurSel (pAccessor->GetBillboardTypeReference());

	// Stretch
	CheckDlgButton (IDC_EFFECT_PARTICLE_ATTACH_FLAG, pAccessor->GetAttachFlagReference());
	CheckDlgButton (IDC_EFFECT_PARTICLE_STRETCH_FLAG, pAccessor->GetStretchFlagReference());

	// Rotation
	m_ctrlRotationType.SetCurSel (pAccessor->GetRotationTypeReference());
	m_ctrlParticleRotationSpeed.SetPos (float (pAccessor->GetRotationSpeedReference()));
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_PARTICLE_ROTATION_SPEED_PRINT, float (pAccessor->GetRotationSpeedReference()));

	SetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_PARTICLE_ROTATION_RANDOM_BEGIN, pAccessor->GetRotationRandomStartBeginReference());
	SetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_PARTICLE_ROTATION_RANDOM_END, pAccessor->GetRotationRandomStartEndReference());

	// Acceleration
	/*SetDialogFloatText(GetSafeHwnd(), IDC_EFFECT_PARTICLE_ACCELERATION_GRAVITY, pAccessor->GetGravityReference());
	SetDialogFloatText(GetSafeHwnd(), IDC_EFFECT_PARTICLE_ACCELERATION_AIR_RESISTANCE, pAccessor->GetAirResistanceReference());
	*/

	// Blend
	m_ctrlBlendSrcType.SelectBlendType (pAccessor->GetSrcBlendTypeReference());
	m_ctrlBlendDestType.SelectBlendType (pAccessor->GetDestBlendTypeReference());
	m_ctrlOperationType.SelectOperationType (pAccessor->GetColorOperationTypeReference());
}

void CEffectParticlePage::SelectEmitterShape (int iShape)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	pParticle->GetEmitterShapeReference() = iShape;

	switch (iShape)
	{
		case CEmitterProperty::EMITTER_SHAPE_POINT:
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_X)->EnableWindow (FALSE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_Y)->EnableWindow (FALSE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_Z)->EnableWindow (FALSE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_RADIUS)->EnableWindow (FALSE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_EMIT_FROM_EDGE)->EnableWindow (FALSE);
			break;
		case CEmitterProperty::EMITTER_SHAPE_ELLIPSE:
		case CEmitterProperty::EMITTER_SHAPE_SPHERE:
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_X)->EnableWindow (FALSE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_Y)->EnableWindow (FALSE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_Z)->EnableWindow (FALSE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_RADIUS)->EnableWindow (TRUE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_EMIT_FROM_EDGE)->EnableWindow (TRUE);
			break;
		case CEmitterProperty::EMITTER_SHAPE_SQUARE:
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_X)->EnableWindow (TRUE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_Y)->EnableWindow (TRUE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_SIZE_Z)->EnableWindow (TRUE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_RADIUS)->EnableWindow (FALSE);
			GetDlgItem (IDD_EFFECT_PARTICLE_EMITTER_EMIT_FROM_EDGE)->EnableWindow (TRUE);
			break;
	}
}

void CEffectParticlePage::SelectEmitterTimeEvent (int iEvent)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	float fLifeTime = pParticle->GetCycleLengthReference();
	m_ctrlEmitterGraph.SetMaxTime (fLifeTime);

	CString strEmitterGraphName;
	m_ctrlEmitterGraphList.GetLBText (iEvent, strEmitterGraphName);
	m_ctrlEmitterGraphList.SelectString (0, strEmitterGraphName);

	switch (iEvent)
	{
		case EMITTER_TIME_EVENT_GRAPH_EMITTING_SIZE:
			EmittingSize.SetTablePointer (pParticle->GetEmitterTimeEventTableEmittingSize());
			m_ctrlEmitterGraph.SetAccessorPointer (&EmittingSize);
			m_ctrlEmitterGraph.SetMaxValue (3000.0f);
			m_ctrlEmitterGraph.SetStartValue (0.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_EMITTING_ANGULAR_VELOCITY:
			EmittingAngularVelocity.SetTablePointer (pParticle->GetEmitterTimeEventTableEmittingAngularVelocity());
			m_ctrlEmitterGraph.SetAccessorPointer (&EmittingAngularVelocity);
			m_ctrlEmitterGraph.SetMaxValue (60.0f);
			m_ctrlEmitterGraph.SetStartValue (-30.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_DIRECTION_X:
			EmittingDirectionX.SetTablePointer (pParticle->GetEmitterTimeEventTableEmittingDirectionX());
			m_ctrlEmitterGraph.SetAccessorPointer (&EmittingDirectionX);
			m_ctrlEmitterGraph.SetMaxValue (100.0f);
			m_ctrlEmitterGraph.SetStartValue (-50.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_DIRECTION_Y:
			EmittingDirectionY.SetTablePointer (pParticle->GetEmitterTimeEventTableEmittingDirectionY());
			m_ctrlEmitterGraph.SetAccessorPointer (&EmittingDirectionY);
			m_ctrlEmitterGraph.SetMaxValue (100.0f);
			m_ctrlEmitterGraph.SetStartValue (-50.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_DIRECTION_Z:
			EmittingDirectionZ.SetTablePointer (pParticle->GetEmitterTimeEventTableEmittingDirectionZ());
			m_ctrlEmitterGraph.SetAccessorPointer (&EmittingDirectionZ);
			m_ctrlEmitterGraph.SetMaxValue (100.0f);
			m_ctrlEmitterGraph.SetStartValue (-50.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_VELOCITY:
			EmittingVelocity.SetTablePointer (pParticle->GetEmitterTimeEventTableEmittingVelocity());
			m_ctrlEmitterGraph.SetAccessorPointer (&EmittingVelocity);
			m_ctrlEmitterGraph.SetMaxValue (100.0f);
			m_ctrlEmitterGraph.SetStartValue (0.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_EMISSION_COUNT:
			EmissionCountAccessor.SetTablePointer (pParticle->GetEmitterTimeEventTableEmissionCount());
			m_ctrlEmitterGraph.SetAccessorPointer (&EmissionCountAccessor);
			m_ctrlEmitterGraph.SetMaxValue (1000.0f);
			m_ctrlEmitterGraph.SetStartValue (0.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_LIFE_TIME:
			LifeTimeAccessor.SetTablePointer (pParticle->GetEmitterTimeEventTableLifeTime());
			m_ctrlEmitterGraph.SetAccessorPointer (&LifeTimeAccessor);
			m_ctrlEmitterGraph.SetMaxValue (2.5f);
			m_ctrlEmitterGraph.SetStartValue (0.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_SIZE_X:
			SizeXAccessor.SetTablePointer (pParticle->GetEmitterTimeEventTableSizeX());
			m_ctrlEmitterGraph.SetAccessorPointer (&SizeXAccessor);
			m_ctrlEmitterGraph.SetMaxValue (100.0f);
			m_ctrlEmitterGraph.SetStartValue (0.0f);
			break;
		case EMITTER_TIME_EVENT_GRAPH_SIZE_Y:
			SizeYAccessor.SetTablePointer (pParticle->GetEmitterTimeEventTableSizeY());
			m_ctrlEmitterGraph.SetAccessorPointer (&SizeYAccessor);
			m_ctrlEmitterGraph.SetMaxValue (100.0f);
			m_ctrlEmitterGraph.SetStartValue (0.0f);
			break;
	}
}

void CEffectParticlePage::SelectParticleTimeEvent (int iEvent)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (m_dwElementIndex, &pElement))
	{
		return;
	}

	float fLifeTime = 1.0f;
	m_ctrlParticleGraph.SetMaxTime (fLifeTime);

	CString strParticleGraphName;
	m_ctrlParticleGraphList.GetLBText (iEvent, strParticleGraphName);
	m_ctrlParticleGraphList.SelectString (0, strParticleGraphName);

	switch (iEvent)
	{
		case PARTICLE_TIME_EVENT_GRAPH_SCALE_X:
			ScaleXAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventScaleX());
			m_ctrlParticleGraph.SetAccessorPointer (&ScaleXAccessor);
			m_ctrlParticleGraph.SetMaxValue (1.0f);
			m_ctrlParticleGraph.SetStartValue (0.0f);
			break;
		case PARTICLE_TIME_EVENT_GRAPH_SCALE_Y:
			ScaleYAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventScaleY());
			m_ctrlParticleGraph.SetAccessorPointer (&ScaleYAccessor);
			m_ctrlParticleGraph.SetMaxValue (1.0f);
			m_ctrlParticleGraph.SetStartValue (0.0f);
			break;
		case PARTICLE_TIME_EVENT_GRAPH_COLOR_RED:
			ColorRedAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventColorRed());
			m_ctrlParticleGraph.SetAccessorPointer (&ColorRedAccessor);
			m_ctrlParticleGraph.SetMaxValue (1.0f);
			m_ctrlParticleGraph.SetStartValue (0.0f);
			break;
		case PARTICLE_TIME_EVENT_GRAPH_COLOR_GREEN:
			ColorGreenAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventColorGreen());
			m_ctrlParticleGraph.SetAccessorPointer (&ColorGreenAccessor);
			m_ctrlParticleGraph.SetMaxValue (1.0f);
			m_ctrlParticleGraph.SetStartValue (0.0f);
			break;
		case PARTICLE_TIME_EVENT_GRAPH_COLOR_BLUE:
			ColorBlueAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventColorBlue());
			m_ctrlParticleGraph.SetAccessorPointer (&ColorBlueAccessor);
			m_ctrlParticleGraph.SetMaxValue (1.0f);
			m_ctrlParticleGraph.SetStartValue (0.0f);
			break;
		case PARTICLE_TIME_EVENT_GRAPH_ALPHA:
			AlphaAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventAlpha());
			m_ctrlParticleGraph.SetAccessorPointer (&AlphaAccessor);
			m_ctrlParticleGraph.SetMaxValue (1.0f);
			m_ctrlParticleGraph.SetStartValue (0.0f);
			break;
		case PARTICLE_TIME_EVENT_GRAPH_ROTATION:
			RotationAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventRotation());
			m_ctrlParticleGraph.SetAccessorPointer (&RotationAccessor);
			m_ctrlParticleGraph.SetMaxValue (180.0f);
			m_ctrlParticleGraph.SetStartValue (-90.0f);
			break;
		case PARTICLE_TIME_EVENT_GRAPH_GRAVITY:
			GravityAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventGravity());
			m_ctrlParticleGraph.SetAccessorPointer (&GravityAccessor);
			m_ctrlParticleGraph.SetMaxValue (400.0f);
			m_ctrlParticleGraph.SetStartValue (0.0f);
			break;
		case PARTICLE_TIME_EVENT_GRAPH_AIR_RESISTANCE:
			AirResistanceAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventAirResistance());
			m_ctrlParticleGraph.SetAccessorPointer (&AirResistanceAccessor);
			m_ctrlParticleGraph.SetMaxValue (1.0f);
			m_ctrlParticleGraph.SetStartValue (0.0f);
			break;
	}
}

void CEffectParticlePage::RefreshTextureList()
{
	m_ctrlTextureList.ResetContent();

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	DWORD dwTextuerCount = pParticle->GetTextureCount();
	if (dwTextuerCount > 0)
	{
		for (DWORD i = 0; i < pParticle->GetTextureCount(); ++i)
		{
			const char* c_szFileName;
			if (!pParticle->GetTextureName (i, &c_szFileName))
			{
				continue;
			}

			std::string strFileName;
			GetOnlyFileName (c_szFileName, strFileName);

			m_ctrlTextureList.InsertString (i, strFileName.c_str());
			m_ctrlTextureList.SelectString (0, strFileName.c_str());

			CGraphicImage * pImage;
			if (pParticle->GetImagePointer (i, &pImage))
			{
				m_ctrlPreview.UpdatePreview (pImage);
			}
		}
	}
	else
	{
		m_ctrlTextureList.InsertString (0, "None");
		m_ctrlTextureList.SelectString (0, "None");
		m_ctrlPreview.UpdatePreview (NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEffectParticlePage message handlers

BOOL CEffectParticlePage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
		switch (LOWORD (wParam))
		{
			case IDC_EFFECT_PARTICLE_TEXTURE_LIST:
				//RefreshTextureList();
			{
				CGraphicImage * pImage;
				int iIndex = m_ctrlTextureList.GetCurSel();
				if (iIndex == CB_ERR)
				{
					m_ctrlPreview.UpdatePreview (NULL);
				}
				else
				{
					if (pParticle->GetImagePointer (iIndex, &pImage))
					{
						m_ctrlPreview.UpdatePreview (pImage);
					}
					else
					{
						m_ctrlPreview.UpdatePreview (NULL);
					}
				}
			}
			break;
			case IDD_EFFECT_PARTICLE_EMITTER_MAX_PARTICLE:
				pParticle->GetMaxEmissionCountReference() = GetDialogIntegerText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_MAX_PARTICLE);
				break;
			case IDD_EFFECT_PARTICLE_EMITTER_CYCLE_LENGTH:
				pParticle->GetCycleLengthReference() = GetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_CYCLE_LENGTH);
				m_ctrlEmitterGraph.SetMaxTime (pParticle->GetCycleLengthReference());
				break;

			case IDD_EFFECT_PARTICLE_EMITTER_LOOP_COUNT:
				pParticle->GetLoopCountReference() = GetDialogIntegerText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_LOOP_COUNT);
				break;

			case IDD_EFFECT_PARTICLE_EMITTER_SHAPE:
				SelectEmitterShape (m_ctrlEmitterShape.GetCurSel());
				break;
			case IDD_EFFECT_PARTICLE_EMITTER_ADVANCED_TYPE:
				pParticle->GetEmitterAdvancedTypeReference() = m_ctrlAdvancedType.GetCurSel();
				break;
			case IDD_EFFECT_PARTICLE_EMITTER_GRAPH_LIST:
				SelectEmitterTimeEvent (m_ctrlEmitterGraphList.GetCurSel());
				break;
			case IDD_EFFECT_PARTICLE_PARTICLE_GRAPH_LIST:
				SelectParticleTimeEvent (m_ctrlParticleGraphList.GetCurSel());
				break;

			case IDD_EFFECT_PARTICLE_EMITTER_SIZE_X:
				pParticle->GetEmittingSizeReference().x = GetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_SIZE_X);
				break;
			case IDD_EFFECT_PARTICLE_EMITTER_SIZE_Y:
				pParticle->GetEmittingSizeReference().y = GetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_SIZE_Y);
				break;
			case IDD_EFFECT_PARTICLE_EMITTER_SIZE_Z:
				pParticle->GetEmittingSizeReference().z = GetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_SIZE_Z);
				break;
			case IDD_EFFECT_PARTICLE_EMITTER_RADIUS:
				pParticle->GetEmittingRadiusReference() = GetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_RADIUS);
				break;

			case IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_X:
				pParticle->GetEmittingDirectionReference().x = GetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_X);
				break;
			case IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_Y:
				pParticle->GetEmittingDirectionReference().y = GetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_Y);
				break;
			case IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_Z:
				pParticle->GetEmittingDirectionReference().z = GetDialogFloatText (GetSafeHwnd(), IDD_EFFECT_PARTICLE_EMITTER_DIRECTION_Z);
				break;

			case IDC_EFFECT_PARTICLE_TEXTURE_ANIMATION_TYPE:
				pParticle->GetTexAniTypeReference() = m_ctrlTexAniType.GetCurSel();
				break;
			case IDC_EFFECT_PARTICLE_BILLBOARD_TYPE:
				pParticle->GetBillboardTypeReference() = m_ctrlBillboardType.GetCurSel();
				break;
			case IDC_EFFECT_PARTICLE_ROTATION_TYPE:
				pParticle->GetRotationTypeReference() = m_ctrlRotationType.GetCurSel();
				break;

			case IDC_EFFECT_PARTICLE_ROTATION_RANDOM_BEGIN:
				pParticle->GetRotationRandomStartBeginReference() = GetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_PARTICLE_ROTATION_RANDOM_BEGIN);
				break;
			case IDC_EFFECT_PARTICLE_ROTATION_RANDOM_END:
				pParticle->GetRotationRandomStartEndReference() = GetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_PARTICLE_ROTATION_RANDOM_END);
				break;

			/*case IDC_EFFECT_PARTICLE_ACCELERATION_GRAVITY:
				pParticle->GetGravityReference() = GetDialogFloatText(GetSafeHwnd(), IDC_EFFECT_PARTICLE_ACCELERATION_GRAVITY);
				break;
			case IDC_EFFECT_PARTICLE_ACCELERATION_AIR_RESISTANCE:
				pParticle->GetAirResistanceReference() = GetDialogFloatText(GetSafeHwnd(), IDC_EFFECT_PARTICLE_ACCELERATION_AIR_RESISTANCE);
				break;
			*/
			case IDC_EFFECT_PARTICLE_BLEND_SRC:
				pParticle->GetSrcBlendTypeReference() = m_ctrlBlendSrcType.GetBlendType();
				break;
			case IDC_EFFECT_PARTICLE_BLEND_DEST:
				pParticle->GetDestBlendTypeReference() = m_ctrlBlendDestType.GetBlendType();
				break;
			case IDC_EFFECT_PARTICLE_OPERATION_TYPE:
				pParticle->GetColorOperationTypeReference() = m_ctrlOperationType.GetOperationType();
		}

	return CPageCtrl::OnCommand (wParam, lParam);
}

void CEffectParticlePage::OnCheckEmitterFilledEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (m_dwElementIndex, &pElement))
	{
		return;
	}

	pElement->pParticle->GetEmitFromEdgeFlagReference() = IsDlgButtonChecked (IDD_EFFECT_PARTICLE_EMITTER_EMIT_FROM_EDGE);
}

/////

void CEffectParticlePage::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	pParticle->GetTexAniDelayReference() = float (m_ctrlTexAniDelay.GetPos()) / 1000.0f;
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_PARTICLE_TEXTURE_ANIMATION_DELAY_PRINT, pParticle->GetTexAniDelayReference());

	pParticle->GetRotationSpeedReference() = float (m_ctrlParticleRotationSpeed.GetPos());
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_PARTICLE_ROTATION_SPEED_PRINT, pParticle->GetRotationSpeedReference());

	CPageCtrl::OnHScroll (nSBCode, nPos, pScrollBar);
}

void CEffectParticlePage::OnInsertTexture()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Texture Files (*.tga, *.jpg)|*.tga;*.jpg|Sequence Files (*.ifl)|*.ifl|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

		CParticleAccessor * pParticle;
		if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
		{
			return;
		}

		if (0 == pParticle->GetTextureCount())
		{
			m_ctrlTextureList.ResetContent();
		}

		pParticle->InsertTexture (FileOpener.GetPathName());
		RefreshTextureList();
	}
}

void CEffectParticlePage::OnClearOneTexture()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	pParticle->ClearOneTexture (m_ctrlTextureList.GetCurSel());
	RefreshTextureList();
}

void CEffectParticlePage::OnClearAllTexture()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	pParticle->ClearAllTexture();
	RefreshTextureList();
}

void CEffectParticlePage::OnCheckCycleLoopFlag()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	pParticle->GetCycleLoopFlagReference() = IsDlgButtonChecked (IDD_EFFECT_PARTICLE_EMITTER_CYCLE_LOOP);
}

void CEffectParticlePage::OnCheckTextureAnimationRandomStartFrame()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	pParticle->GetTexAniRandomStartFrameFlagReference() = IsDlgButtonChecked (IDC_EFFECT_PARTICLE_TEXTURE_ANIMATION_RANDOM_START_FRAME);
}

void CEffectParticlePage::OnCheckStretchFlag()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	pParticle->GetStretchFlagReference() = IsDlgButtonChecked (IDC_EFFECT_PARTICLE_STRETCH_FLAG);
}

void CEffectParticlePage::OnCheckAttachFlag()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CParticleAccessor * pParticle;
	if (!pEffectAccessor->GetElementParticle (m_dwElementIndex, &pParticle))
	{
		return;
	}

	pParticle->GetAttachFlagReference() = IsDlgButtonChecked (IDC_EFFECT_PARTICLE_ATTACH_FLAG);
}

void CEffectParticlePage::OnColorAnimation()
{
	assert (m_pColorAnimationDlg);

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (m_dwElementIndex, &pElement))
	{
		return;
	}

	ColorRedAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventColorRed());
	ColorGreenAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventColorGreen());
	ColorBlueAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventColorBlue());
	AlphaAccessor.SetTablePointer (pElement->pParticle->GetParticleTimeEventAlpha());

	m_pColorAnimationDlg->SetRGBAAccessor (
		&ColorRedAccessor,
		&ColorGreenAccessor,
		&ColorBlueAccessor,
		&AlphaAccessor);
	m_pColorAnimationDlg->ShowWindow (SW_SHOW);
	m_pColorAnimationDlg->Invalidate();
}

