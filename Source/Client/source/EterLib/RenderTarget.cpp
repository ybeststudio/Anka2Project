#include "StdAfx.h"

#ifdef ENABLE_RENDER_TARGET
#include "RenderTarget.h"
#include "../EterLib/Camera.h"
#include "../EterLib/RenderTargetManager.h"
#include "../EterPythonLib/PythonGraphic.h"

#include "../EterBase/Timer.h"
#include "../EterBase/CRC32.h"

#include "../GameLib/GameType.h"
#include "../GameLib/MapType.h"
#include "../GameLib/ItemData.h"
#include "../GameLib/ItemManager.h"
#include "../GameLib/ActorInstance.h"
#include "../UserInterface/InstanceBase.h"
#include "ResourceManager.h"

CRenderTarget::~CRenderTarget() {}

CRenderTarget::CRenderTarget(const DWORD width, const DWORD height) :
	m_pModel(nullptr),
	m_background(nullptr),
	m_modelRotation(0),
	m_visible(false),
	m_pmodelRotationActive(true),
	m_v3Eye(0.0f, 0.0f, 0.0f),
	m_v3Target(0.0f, 0.0f, 0.0f),
	m_v3Up(0.0f, 0.0f, 1.0f),
	m_v3AngularVelocity(0.0f, 0.0f, 0.0f),
	m_bScale(1.0),
	m_bShowModel(true)
{
	//auto pTex = new CGraphicRenderTargetTexture;
	auto pTex = std::make_unique<CGraphicRenderTargetTexture>();
	if (!pTex->Create(width, height, D3DFMT_X8R8G8B8, D3DFMT_D16))
	{
		//delete pTex;
		TraceError("CRenderTarget::CRenderTarget: Could not create CGraphicRenderTargetTexture %dx%d", width, height);
		throw std::runtime_error("CRenderTarget::CRenderTarget: Could not create CGraphicRenderTargetTexture");
	}

	f_zoom = 0.0f;
	m_renderTargetTexture = std::move(pTex);
}

void CRenderTarget::RenderTexture() const
{
	m_renderTargetTexture->Render();
}

void CRenderTarget::SetRenderingRect(RECT* rect) const
{
	m_renderTargetTexture->SetRenderingRect(rect);
}

void CRenderTarget::CreateTextures() const
{
	m_renderTargetTexture->CreateTextures();
}

void CRenderTarget::ReleaseTextures() const
{
	m_renderTargetTexture->ReleaseTextures();
}

void CRenderTarget::ResetModel()
{
	if (m_pModel)
	{
		m_pModel->ClearWikiAffect();
		m_pModel.reset();
	}
}

void CRenderTarget::SetScale(float fScale)
{
	m_bScale = fScale;
	if (IsReady())
		SelectModel(m_pModel->GetRace(), true);
}
void CRenderTarget::SelectModel(const int index, bool isFromScale)
{
	if (m_pModel && !isFromScale && m_pModel->GetRace() == index)
		return;

	if (isFromScale && m_pModel)
		m_pModel.reset();

	CInstanceBase::SCreateData kCreateData{};
	kCreateData.m_bType = index > 8 ? CActorInstance::TYPE_NPC : CActorInstance::TYPE_PC;
	kCreateData.m_dwRace = index;
	auto model = std::make_unique<CInstanceBase>();
	if (!model->Create(kCreateData))
	{
		ResetModel();
		return;
	}
	m_pModel = std::move(model);
	m_pModel->NEW_SetPixelPosition(TPixelPosition(0, 0, 0));
	m_pModel->SetAlwaysRender(true);
	m_pModel->SetRotation(0.0f);

	float fTargetHeight = 110.0f;

	m_v3Eye.x = 0.0f;
	m_v3Eye.y = -1000.0f;
	m_v3Eye.z = 600.0f;

	m_v3Target.x = 0.0f;
	m_v3Target.y = 0.0f;
	m_v3Target.z = 95.0f;

	if (index > (NRaceData::JOB_MAX_NUM * 2))
	{
		const auto ptrGraphicInstance = m_pModel->GetGraphicThingInstancePtr();
		if (ptrGraphicInstance) {
			const float fRaceHeight = (ptrGraphicInstance->GetHeight() / m_bScale);
			fTargetHeight = fRaceHeight / 2.0f;
			m_v3Eye.y = -(fRaceHeight * 8.9f);
			m_v3Eye.z = 0.0f;
			m_v3Target.z = fRaceHeight / 2.0f;
		}
	}

	auto& camera_manager = CCameraManager::instance();
	camera_manager.SetCurrentCamera(CCameraManager::DEFAULT_ORTHO_CAMERA);
	camera_manager.GetCurrentCamera()->SetTargetHeight(fTargetHeight);
	camera_manager.ResetToPreviousCamera();
	m_modelRotation = 0.0f;
}

bool CRenderTarget::CreateBackground(const char *imgPath, const DWORD width, const DWORD height)
{
	if (m_background)
		return false;

	m_background = std::make_unique<CGraphicImageInstance>();

	const auto graphic_image = dynamic_cast<CGraphicImage*>(CResourceManager::instance().GetResourcePointer(imgPath));
	if (!graphic_image)
	{
		m_background.reset();
		return false;
	}

	m_background->SetImagePointer(graphic_image);
	m_background->SetScale(static_cast<float>(width) / graphic_image->GetWidth(), static_cast<float>(height) / graphic_image->GetHeight());
	return true;
}

void CRenderTarget::RenderBackground() const
{
	if (!m_background)
		return;

	auto& rectRender = *m_renderTargetTexture->GetRenderingRect();
	m_renderTargetTexture->SetRenderTarget();

	CGraphicRenderTargetTexture::Clear();
	CPythonGraphic::Instance().SetInterfaceRenderState();

	const auto width = static_cast<float>(rectRender.right - rectRender.left);
	const auto height = static_cast<float>(rectRender.bottom - rectRender.top);

	CPythonGraphic::Instance().SetViewport(0.0f, 0.0f, width, height);

	m_background->Render();
	m_renderTargetTexture->ResetRenderTarget();
}

void CRenderTarget::UpdateModel()
{
	if (!IsReady())
		return;

	if (m_pmodelRotationActive && m_bShowModel)
	{
		if (m_modelRotation < 360.0f)
			m_modelRotation += 1.0f / 60 * 60.0f;
		else
			m_modelRotation = 0.0f / 60 * 60.0f;

		m_pModel->SetRotation(m_modelRotation);
	}
	else
		m_pModel->SetRotation(0.0f);
	m_pModel->Transform();
	if (m_pmodelRotationActive)
		m_pModel->GetGraphicThingInstanceRef().RotationProcess();
}

void CRenderTarget::DeformModel()
{
	if (!IsReady())
		return;
	m_pModel->Deform();
}

void CRenderTarget::RenderModel()
{
	if (!IsReady())
		return;

	auto& python_graphic = CPythonGraphic::Instance();
	auto& camera_manager = CCameraManager::instance();
	auto& state_manager = CStateManager::Instance();
	auto& rectRender = *m_renderTargetTexture->GetRenderingRect();


	m_renderTargetTexture->SetRenderTarget();
	if (!m_background)
		m_renderTargetTexture->Clear();

	python_graphic.ClearDepthBuffer();

	const auto fov = python_graphic.GetFOV();
	const auto aspect = python_graphic.GetAspect();
	const auto near_y = python_graphic.GetNear();
	const auto far_y = python_graphic.GetFar();

	const auto width = static_cast<float>(rectRender.right - rectRender.left);
	const auto height = static_cast<float>(rectRender.bottom - rectRender.top);

	state_manager.SetRenderState(D3DRS_FOGENABLE, FALSE);

	python_graphic.SetViewport(0.0f, 0.0f, width, height);
	python_graphic.PushState();

	camera_manager.SetCurrentCamera(CCameraManager::DEFAULT_ORTHO_CAMERA);
	camera_manager.GetCurrentCamera()->SetViewParams(m_v3Eye, m_v3Target, m_v3Up);
	camera_manager.GetCurrentCamera()->RotateEyeAroundTarget(m_v3AngularVelocity.z, m_v3AngularVelocity.x);

	python_graphic.UpdateViewMatrix();
	python_graphic.SetPerspective(10.0f, width / height, 100.0f, 15000.0f);

	if (m_bShowModel)
	{
		m_pModel->Update();
		m_pModel->Render();
	}

	camera_manager.ResetToPreviousCamera();
	python_graphic.RestoreViewport();
	python_graphic.PopState();
	python_graphic.SetPerspective(fov, aspect, near_y, far_y);

	m_renderTargetTexture->ResetRenderTarget();

	state_manager.SetRenderState(D3DRS_FOGENABLE, FALSE);
}

void CRenderTarget::SetModelV3Eye(float x, float y, float z)
{
	m_v3Eye.x = x;
	m_v3Eye.y = y;
	m_v3Eye.z = z;
}

void CRenderTarget::SetModelV3Target(float x, float y, float z)
{
	m_v3Target.x = x;
	m_v3Target.y = y;
	m_v3Target.z = z;
}

void CRenderTarget::RotateEyeAroundTarget(float fPitchDegree, float fRollDegree)
{
	m_v3AngularVelocity.x = fRollDegree;
	m_v3AngularVelocity.z = fPitchDegree;
}

void CRenderTarget::Zoom(float fRatio)
{
	if (fRatio == 1.0f)
		return;

	D3DXVECTOR3 v3Temp = m_v3Eye - m_v3Target;
	v3Temp *= fRatio;
	m_v3Eye = v3Temp + m_v3Target;
}

void CRenderTarget::SetZoom(bool bZoom)
{
	if (!m_visible || !m_pModel)
		return;

	if (f_zoom == 5.0f && !bZoom)
		return;

	if (f_zoom == -5.0f && bZoom)
		return;

	if (!bZoom)
		f_zoom += 1.0f;
	else
		f_zoom -= 1.0f;
}
#endif
