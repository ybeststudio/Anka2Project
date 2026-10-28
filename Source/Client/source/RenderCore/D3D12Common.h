#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <wrl/client.h>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <format>
#include <cassert>

// DirectX 12 Agility SDK
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h>
#include <d3dx12/d3dx12_barriers.h>
#include <d3dx12/d3dx12_check_feature_support.h>

// DirectXMath & StepTimer
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <StepTimer.h>

// D3D12 Memory Allocator
#if __has_include(<d3d12ma/D3D12MemAlloc.h>)
#include <d3d12ma/D3D12MemAlloc.h>
#elif __has_include(<D3D12MemAlloc.h>)
#include <D3D12MemAlloc.h>
#else
#include "../../extern/d3d12ma/D3D12MemAlloc.h"
#endif

namespace RenderCore
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;

	constexpr uint32_t FRAME_BUFFER_COUNT = 3;
	constexpr uint32_t MAX_BINDLESS_RESOURCES = 65536;
	constexpr uint32_t MAX_BINDLESS_SAMPLERS = 2048;

	// Froxel Volumetric Grid
	constexpr uint32_t FROXEL_GRID_X = 160;
	constexpr uint32_t FROXEL_GRID_Y = 90;
	constexpr uint32_t FROXEL_GRID_Z = 64;

	// Formats
	constexpr DXGI_FORMAT SCENE_COLOR_FORMAT = DXGI_FORMAT_R11G11B10_FLOAT;
	constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
	constexpr DXGI_FORMAT BACKBUFFER_FORMAT_SDR = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr DXGI_FORMAT BACKBUFFER_FORMAT_HDR = DXGI_FORMAT_R16G16B16A16_FLOAT;

	inline void ThrowIfFailed(HRESULT hr, const char* msg = "DirectX 12 Error")
	{
		if (FAILED(hr))
		{
			char buffer[512];
			sprintf_s(buffer, "%s [HRESULT: 0x%08X]", msg, static_cast<unsigned int>(hr));
			::MessageBoxA(nullptr, buffer, "RenderCore Fatal Error", MB_ICONERROR | MB_OK);
			throw std::runtime_error(buffer);
		}
	}

	struct alignas(256) FrameConstants
	{
		XMMATRIX viewMatrix;
		XMMATRIX projMatrix;
		XMMATRIX viewProjMatrix;
		XMMATRIX invViewMatrix;
		XMMATRIX invProjMatrix;
		XMMATRIX invViewProjMatrix;

		XMFLOAT3 cameraPosition;
		float nearPlane;

		XMFLOAT3 cameraDirection;
		float farPlane;

		XMFLOAT2 renderResolution;
		XMFLOAT2 invRenderResolution;

		float time;
		float deltaTime;
		uint32_t frameIndex;
		uint32_t sceneColorSRV;

		uint32_t sceneDepthSRV;
		uint32_t fogAccumulatedSRV;
		uint32_t tlasIndex;
		uint32_t padding;
	};
}
