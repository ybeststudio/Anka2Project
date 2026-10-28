#include "StdAfx.h"

#ifdef ENABLE_RENDER_TARGET
#include "RenderTargetManager.h"
#include "../EterBase/Stl.h"
#include <memory>

CRenderTargetManager::CRenderTargetManager() {}
CRenderTargetManager::~CRenderTargetManager()
{
	Destroy();
}
void CRenderTargetManager::Destroy()
{
	m_renderTargets.clear();
}
void CRenderTargetManager::DeformModels()
{
	for (const auto& elem : m_renderTargets)
		elem.second->DeformModel();
}
void CRenderTargetManager::UpdateModels()
{
	for (auto& elem : m_renderTargets)
		elem.second->UpdateModel();
}
void CRenderTargetManager::RenderBackgrounds()
{
	for (const auto& elem : m_renderTargets)
		elem.second->RenderBackground();
}
void CRenderTargetManager::RenderModels()
{
	for (const auto& elem : m_renderTargets)
		elem.second->RenderModel();
}
void CRenderTargetManager::CreateRenderTargetTextures()
{
	for (const auto& elem : m_renderTargets)
		elem.second->CreateTextures();
}
void CRenderTargetManager::ReleaseRenderTargetTextures()
{
	for (const auto& elem : m_renderTargets)
		elem.second->ReleaseTextures();
}
bool CRenderTargetManager::CreateRenderTarget(const int index, const int width, const int height)
{
	if (index < 1 || GetRenderTarget(index))
		return false;
	m_renderTargets.emplace(index, std::make_shared<CRenderTarget>(width, height));
	return true;
}
void CRenderTargetManager::RemoveRenderTarget(const int index)
{
	const auto it = m_renderTargets.find(index);
	if (it != m_renderTargets.end())
	{
		it->second->ResetModel();
		m_renderTargets.erase(index);
	}
}
std::shared_ptr<CRenderTarget> CRenderTargetManager::GetRenderTarget(const int index)
{
	const auto it = m_renderTargets.find(index);
	if (it != m_renderTargets.end())
		return it->second;
	return nullptr;
}
#endif
