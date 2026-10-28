#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	class CDepthBuffer
	{
	public:
		CDepthBuffer() = default;
		~CDepthBuffer();

		bool Initialize(uint32_t width, uint32_t height);
		void Shutdown();
		void Resize(uint32_t width, uint32_t height);

		void Clear(ID3D12GraphicsCommandList* pCmdList);

		// Enhanced Barrier transitions
		void TransitionToDepthWrite(ID3D12GraphicsCommandList7* pCmdList);
		void TransitionToDepthRead(ID3D12GraphicsCommandList7* pCmdList);

		D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetReadOnlyDSV() const;
		uint32_t GetSRVIndex() const { return m_srvIndex; }

		ID3D12Resource* GetResource() const { return m_depthResource.Get(); }
		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

		// Reverse-Z Infinite Projection Matrix
		static XMMATRIX CreateReverseZInfiniteProjection(float fovY, float aspect, float nearZ);

	private:
		bool CreateResources();

	private:
		uint32_t m_width = 0;
		uint32_t m_height = 0;

		ComPtr<D3D12MA::Allocation> m_allocation;
		ComPtr<ID3D12Resource> m_depthResource;

		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		uint32_t m_dsvDescriptorSize = 0;

		uint32_t m_srvIndex = 0;
	};
}
