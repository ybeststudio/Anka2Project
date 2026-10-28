#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	constexpr uint32_t MAX_GPU_INSTANCES = 65536;
	constexpr uint32_t MAX_TERRAIN_CHUNKS = 1024;
	constexpr uint32_t HIZ_MIP_LEVELS = 8;

	struct GpuInstanceData
	{
		XMMATRIX worldMatrix;
		XMFLOAT4 boundingSphere; // xyz = center, w = radius
		uint32_t materialIndex;
		uint32_t meshIndex;
		uint32_t lodLevel;
		uint32_t flags; // 1 = CastShadow, 2 = TwoSided, etc.
	};

	struct IndirectDrawIndexedCommand
	{
		uint32_t objectIndex; // 32-bit root constant
		D3D12_DRAW_INDEXED_ARGUMENTS drawArgs;
	};

	struct TerrainChunkData
	{
		XMFLOAT2 minCoord;
		XMFLOAT2 maxCoord;
		float minHeight;
		float maxHeight;
		uint32_t lodLevel;
		float morphFactor;
	};

	struct GpuSceneConstants
	{
		uint32_t totalInstanceCount = 0;
		uint32_t totalTerrainChunks = 0;
		uint32_t instanceBufferSRV = 0;
		uint32_t indirectArgsUAV = 0;

		uint32_t hizDepthSRV = 0;
		uint32_t hizWidth = 0;
		uint32_t hizHeight = 0;
		uint32_t hizMipCount = 0;

		XMFLOAT3 cameraPos = XMFLOAT3(0, 0, 0);
		float cameraFov = 60.0f;

		uint32_t isPrepass = 0;
		uint32_t drawCountUAV = 0;
		uint32_t sourceMipIndex = 0;
		uint32_t targetMipUAV = 0;
	};

	class CGpuScene
	{
	public:
		static CGpuScene& Instance();

		bool Initialize(uint32_t screenWidth, uint32_t screenHeight);
		void Shutdown();
		void Resize(uint32_t screenWidth, uint32_t screenHeight);

		void ResetInstances();
		void AddInstance(const GpuInstanceData& instance);

		// Two-phase occlusion culling & Hi-Z pyramid generation
		void GenerateHiZPyramid(ID3D12GraphicsCommandList7* pCmdList, uint32_t depthSRV);
		void CullAndBuildIndirectDraws(ID3D12GraphicsCommandList7* pCmdList, bool isPrepass);

		// Execute indirect draws
		void ExecuteIndirect(ID3D12GraphicsCommandList7* pCmdList);

		ID3D12CommandSignature* GetCommandSignature() const { return m_commandSignature.Get(); }
		ID3D12Resource* GetIndirectArgsBuffer() const { return m_indirectArgsBuffer.Get(); }
		uint32_t GetInstanceBufferSRV() const { return m_instanceBufferSRV; }

	private:
		CGpuScene() = default;
		~CGpuScene();
		CGpuScene(const CGpuScene&) = delete;
		CGpuScene& operator=(const CGpuScene&) = delete;

		bool CreateBuffers();
		bool CreateHiZResources(uint32_t screenWidth, uint32_t screenHeight);
		bool CreatePipelines();

	private:
		bool m_initialized = false;
		uint32_t m_screenWidth = 0;
		uint32_t m_screenHeight = 0;
		uint32_t m_hizWidth = 0;
		uint32_t m_hizHeight = 0;

		GpuSceneConstants m_constants;
		std::vector<GpuInstanceData> m_instances;

		// Command Signature
		ComPtr<ID3D12CommandSignature> m_commandSignature;

		// GPU Instance Data Structured Buffer
		ComPtr<D3D12MA::Allocation> m_instanceUploadAlloc;
		ComPtr<ID3D12Resource> m_instanceUploadBuffer;
		GpuInstanceData* m_pInstanceUploadData = nullptr;
		uint32_t m_instanceBufferSRV = 0;

		// Indirect Arguments Buffer & Count Buffer
		ComPtr<D3D12MA::Allocation> m_indirectArgsAlloc;
		ComPtr<ID3D12Resource> m_indirectArgsBuffer;
		uint32_t m_indirectArgsSRV = 0;
		uint32_t m_indirectArgsUAV = 0;

		ComPtr<D3D12MA::Allocation> m_drawCountAlloc;
		ComPtr<ID3D12Resource> m_drawCountBuffer;
		uint32_t m_drawCountUAV = 0;

		// Hi-Z Depth Pyramid (Mips 0..7)
		ComPtr<D3D12MA::Allocation> m_hizTextureAlloc;
		ComPtr<ID3D12Resource> m_hizTexture;
		uint32_t m_hizSRV = 0;
		uint32_t m_hizMipUAVs[HIZ_MIP_LEVELS] = {};

		// Compute PSOs
		ComPtr<ID3D12PipelineState> m_hizGeneratePSO;
		ComPtr<ID3D12PipelineState> m_cullingPSO;
	};
}
