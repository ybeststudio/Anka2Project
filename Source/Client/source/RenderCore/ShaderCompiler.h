#pragma once

#include "D3D12Common.h"
#include <dxc/inc/dxcapi.h>

namespace RenderCore
{
	class CShaderCompiler
	{
	public:
		static CShaderCompiler& Instance();

		bool Initialize();
		void Shutdown();

		bool CompileShaderFromFile(
			const std::wstring& filePath,
			const std::wstring& entryPoint,
			const std::wstring& targetProfile,
			ComPtr<IDxcBlob>& outShaderBlob,
			const std::vector<std::wstring>& defines = {}
		);

		bool CompileShaderFromSource(
			const std::string& source,
			const std::wstring& entryPoint,
			const std::wstring& targetProfile,
			ComPtr<IDxcBlob>& outShaderBlob,
			const std::vector<std::wstring>& defines = {}
		);

		// Pipeline Library Disk Caching
		bool InitializePipelineLibrary(const std::wstring& cacheFilePath);
		bool LoadGraphicsPipeline(const std::wstring& name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, ID3D12PipelineState** ppPSO);
		bool StoreGraphicsPipeline(const std::wstring& name, ID3D12PipelineState* pPSO);
		void SavePipelineLibraryToDisk();

	private:
		CShaderCompiler() = default;
		~CShaderCompiler();
		CShaderCompiler(const CShaderCompiler&) = delete;
		CShaderCompiler& operator=(const CShaderCompiler&) = delete;

	private:
		bool m_initialized = false;
		HMODULE m_dxcModule = nullptr;
		DxcCreateInstanceProc m_pfnDxcCreateInstance = nullptr;

		ComPtr<IDxcUtils> m_utils;
		ComPtr<IDxcCompiler3> m_compiler;
		ComPtr<IDxcIncludeHandler> m_includeHandler;

		// Pipeline Library
		ComPtr<ID3D12PipelineLibrary1> m_pipelineLibrary;
		std::wstring m_cacheFilePath;
		std::vector<uint8_t> m_libraryData;
	};
}
