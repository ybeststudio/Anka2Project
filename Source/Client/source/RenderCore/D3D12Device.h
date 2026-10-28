#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	struct DeviceCaps
	{
		D3D_FEATURE_LEVEL maxFeatureLevel = D3D_FEATURE_LEVEL_1_0_CORE;
		D3D_SHADER_MODEL highestShaderModel = D3D_SHADER_MODEL_5_1;
		D3D12_RESOURCE_BINDING_TIER resourceBindingTier = D3D12_RESOURCE_BINDING_TIER_1;
		D3D12_RAYTRACING_TIER raytracingTier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
		D3D12_MESH_SHADER_TIER meshShaderTier = D3D12_MESH_SHADER_TIER_NOT_SUPPORTED;
		bool enhancedBarriersSupported = false;
		bool waveOpsSupported = false;
		uint32_t waveLaneCountMin = 0;
		uint32_t waveLaneCountMax = 0;
	};

	class CD3D12Device
	{
	public:
		static CD3D12Device& Instance();

		bool Initialize(HWND hWnd, uint32_t width, uint32_t height, bool enableDebugLayer = false);
		void Shutdown();

		// GPU Execution & Monotonic Fence Synchronization
		uint64_t ExecuteCommandList(ID3D12GraphicsCommandList* pCmdList, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		uint64_t SignalFence(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		void WaitForFence(D3D12_COMMAND_LIST_TYPE type, uint64_t fenceValue);
		void FlushGPU();

		// Getters
		ID3D12Device10* GetDevice() const { return m_device.Get(); }
		IDXGIFactory6* GetFactory() const { return m_factory.Get(); }
		IDXGIAdapter4* GetAdapter() const { return m_adapter.Get(); }
		D3D12MA::Allocator* GetAllocator() const { return m_allocator.Get(); }

		ID3D12CommandQueue* GetDirectQueue() const { return m_directQueue.Get(); }
		ID3D12CommandQueue* GetComputeQueue() const { return m_computeQueue.Get(); }
		ID3D12CommandQueue* GetCopyQueue() const { return m_copyQueue.Get(); }

		const DeviceCaps& GetCaps() const { return m_caps; }
		const std::wstring& GetAdapterName() const { return m_adapterName; }
		bool IsInitialized() const { return m_initialized; }
		bool IsRaytracingTier1_1Supported() const { return m_caps.raytracingTier >= D3D12_RAYTRACING_TIER_1_1; }
		bool IsMeshShaderSupported() const { return m_caps.meshShaderTier >= D3D12_MESH_SHADER_TIER_1; }
		bool IsEnhancedBarriersSupported() const { return m_caps.enhancedBarriersSupported; }

	private:
		CD3D12Device() = default;
		~CD3D12Device();
		CD3D12Device(const CD3D12Device&) = delete;
		CD3D12Device& operator=(const CD3D12Device&) = delete;

		bool CreateFactoryAndAdapter(bool enableDebugLayer);
		bool QueryCapabilitiesAndCreateDevice();
		bool CreateCommandQueues();
		bool CreateMemoryAllocator();

	private:
		bool m_initialized = false;
		std::wstring m_adapterName;
		DeviceCaps m_caps;

		ComPtr<IDXGIFactory6> m_factory;
		ComPtr<IDXGIAdapter4> m_adapter;
		ComPtr<ID3D12Device10> m_device;

		// Command Queues
		ComPtr<ID3D12CommandQueue> m_directQueue;
		ComPtr<ID3D12CommandQueue> m_computeQueue;
		ComPtr<ID3D12CommandQueue> m_copyQueue;

		// Monotonic Fences
		ComPtr<ID3D12Fence> m_directFence;
		ComPtr<ID3D12Fence> m_computeFence;
		ComPtr<ID3D12Fence> m_copyFence;
		uint64_t m_directFenceValue = 0;
		uint64_t m_computeFenceValue = 0;
		uint64_t m_copyFenceValue = 0;
		HANDLE m_fenceEvent = nullptr;

		// D3D12MA Memory Allocator
		ComPtr<D3D12MA::Allocator> m_allocator;
	};
}
