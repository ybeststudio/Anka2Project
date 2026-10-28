#include "ComputeDeform.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"
#include "ShaderCompiler.h"

namespace RenderCore
{
	CComputeDeform& CComputeDeform::Instance()
	{
		static CComputeDeform instance;
		return instance;
	}

	CComputeDeform::~CComputeDeform()
	{
		Shutdown();
	}

	bool CComputeDeform::Initialize()
	{
		if (m_initialized)
			return true;

		if (!CreatePipelines())
			return false;

		m_initialized = true;
		return true;
	}

	void CComputeDeform::Shutdown()
	{
		m_vegetationSwayPSO.Reset();
		m_skinningPSO.Reset();
		m_initialized = false;
	}

	bool CComputeDeform::CreatePipelines()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pRootSig = CBindlessDescriptorHeap::Instance().GetGlobalRootSignature();
		auto& compiler = CShaderCompiler::Instance();

		std::wstring shaderPath = L"source/RenderCore/Shaders/ComputeDeform.hlsl";

		// 1. Skinning Compute PSO
		ComPtr<IDxcBlob> skinCSBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"SkinningCS", L"cs_6_6", skinCSBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { skinCSBlob->GetBufferPointer(), skinCSBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_skinningPSO)),
				"Skinning PSO oluşturulamadı");
		}

		// 2. Vegetation Sway Compute PSO
		ComPtr<IDxcBlob> vegCSBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"VegetationSwayCS", L"cs_6_6", vegCSBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { vegCSBlob->GetBufferPointer(), vegCSBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_vegetationSwayPSO)),
				"Vegetation Sway PSO oluşturulamadı");
		}

		return true;
	}

	void CComputeDeform::DispatchSkinning(
		ID3D12GraphicsCommandList7* pCmdList,
		uint32_t vertexCount,
		uint32_t inputVertexSRV,
		uint32_t boneMatrixSRV,
		uint32_t outputVertexUAV,
		ID3D12Resource* pOutputResource)
	{
		if (!m_skinningPSO || vertexCount == 0)
			return;

		auto& heap = CBindlessDescriptorHeap::Instance();
		heap.SetDescriptorHeaps(pCmdList);
		pCmdList->SetComputeRootSignature(heap.GetGlobalRootSignature());
		pCmdList->SetPipelineState(m_skinningPSO.Get());

		SkinningDispatchParams params = {};
		params.vertexCount = vertexCount;
		params.inputVertexSRV = inputVertexSRV;
		params.boneMatrixSRV = boneMatrixSRV;
		params.outputVertexUAV = outputVertexUAV;

		pCmdList->SetComputeRoot32BitConstants(0, sizeof(params) / 4, &params, 0);

		// Dispatch (64 threads per group)
		uint32_t threadGroups = (vertexCount + 63) / 64;
		pCmdList->Dispatch(threadGroups, 1, 1);

		// Enhanced Barrier: Sync compute write to Vertex Shader & Raytracing Acceleration Structure Build
		if (pOutputResource)
		{
			D3D12_BUFFER_BARRIER barrier = CD3DX12_BUFFER_BARRIER(
				D3D12_BARRIER_SYNC_COMPUTE_SHADING,
				D3D12_BARRIER_SYNC_VERTEX_SHADING | D3D12_BARRIER_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE | D3D12_BARRIER_SYNC_INDEX_INPUT,
				D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
				D3D12_BARRIER_ACCESS_VERTEX_BUFFER | D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ,
				pOutputResource);

			D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
			pCmdList->Barrier(1, &group);
		}
	}

	void CComputeDeform::DispatchVegetationSway(
		ID3D12GraphicsCommandList7* pCmdList,
		uint32_t vertexCount,
		uint32_t inputVertexSRV,
		uint32_t outputVertexUAV,
		ID3D12Resource* pOutputResource,
		float time,
		const XMFLOAT3& windDir,
		float windSpeed,
		float windStrength)
	{
		if (!m_vegetationSwayPSO || vertexCount == 0)
			return;

		auto& heap = CBindlessDescriptorHeap::Instance();
		heap.SetDescriptorHeaps(pCmdList);
		pCmdList->SetComputeRootSignature(heap.GetGlobalRootSignature());
		pCmdList->SetPipelineState(m_vegetationSwayPSO.Get());

		VegetationDispatchParams params = {};
		params.vertexCount = vertexCount;
		params.inputVertexSRV = inputVertexSRV;
		params.outputVertexUAV = outputVertexUAV;
		params.time = time;
		params.windDirection = windDir;
		params.windSpeed = windSpeed;
		params.windStrength = windStrength;
		params.branchAmplitude = 0.2f;
		params.leafFrequency = 4.0f;

		pCmdList->SetComputeRoot32BitConstants(0, sizeof(params) / 4, &params, 0);

		uint32_t threadGroups = (vertexCount + 63) / 64;
		pCmdList->Dispatch(threadGroups, 1, 1);

		if (pOutputResource)
		{
			D3D12_BUFFER_BARRIER barrier = CD3DX12_BUFFER_BARRIER(
				D3D12_BARRIER_SYNC_COMPUTE_SHADING,
				D3D12_BARRIER_SYNC_VERTEX_SHADING | D3D12_BARRIER_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE | D3D12_BARRIER_SYNC_INDEX_INPUT,
				D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
				D3D12_BARRIER_ACCESS_VERTEX_BUFFER | D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ,
				pOutputResource);

			D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
			pCmdList->Barrier(1, &group);
		}
	}
}
