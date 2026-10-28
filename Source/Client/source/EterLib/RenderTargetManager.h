#pragma once

#include <unordered_map>
#include <cstdint>

#include "../EterBase/Singleton.h"
#include <memory>
#include "RenderTarget.h"

class CRenderTargetManager : public CSingleton<CRenderTargetManager>
{
	public:
		CRenderTargetManager();
		virtual ~CRenderTargetManager();

		std::shared_ptr<CRenderTarget> GetRenderTarget(int index);
		bool CreateRenderTarget(int index, int width, int height);

		void CreateRenderTargetTextures();
		void ReleaseRenderTargetTextures();

		void RemoveRenderTarget(const int index);
		void Destroy();
		void DeformModels();
		void UpdateModels();
		void RenderBackgrounds();
		void RenderModels();

	protected:
		std::unordered_map<int, std::shared_ptr<CRenderTarget>> m_renderTargets;
};
