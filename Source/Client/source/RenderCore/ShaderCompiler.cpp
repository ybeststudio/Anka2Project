#include "ShaderCompiler.h"
#include "D3D12Device.h"
#include <fstream>
#include <filesystem>

namespace RenderCore
{
	CShaderCompiler& CShaderCompiler::Instance()
	{
		static CShaderCompiler s_instance;
		return s_instance;
	}

	CShaderCompiler::~CShaderCompiler()
	{
		Shutdown();
	}

	bool CShaderCompiler::Initialize()
	{
		if (m_initialized)
			return true;

		m_dxcModule = LoadLibraryW(L"dxcompiler.dll");
		if (!m_dxcModule)
		{
			m_dxcModule = LoadLibraryExW(L"dxcompiler.dll", nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
		}
		if (!m_dxcModule)
		{
			::MessageBoxW(nullptr, L"dxcompiler.dll yüklenemedi. Lütfen uygulamanın kurulu olduğu dizinde dxcompiler.dll ve dxil.dll dosyalarının bulunduğundan emin olun.", L"DXC Hatası", MB_ICONERROR | MB_OK);
			return false;
		}

		m_pfnDxcCreateInstance = (DxcCreateInstanceProc)GetProcAddress(m_dxcModule, "DxcCreateInstance");
		if (!m_pfnDxcCreateInstance)
			return false;

		ThrowIfFailed(m_pfnDxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils)), "IDxcUtils oluşturulamadı");
		ThrowIfFailed(m_pfnDxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler)), "IDxcCompiler3 oluşturulamadı");
		ThrowIfFailed(m_utils->CreateDefaultIncludeHandler(&m_includeHandler), "IDxcIncludeHandler oluşturulamadı");

		m_initialized = true;
		return true;
	}

	void CShaderCompiler::Shutdown()
	{
		SavePipelineLibraryToDisk();

		m_pipelineLibrary.Reset();
		m_includeHandler.Reset();
		m_compiler.Reset();
		m_utils.Reset();

		if (m_dxcModule)
		{
			FreeLibrary(m_dxcModule);
			m_dxcModule = nullptr;
		}

		m_initialized = false;
	}

	bool CShaderCompiler::CompileShaderFromFile(
		const std::wstring& filePath,
		const std::wstring& entryPoint,
		const std::wstring& targetProfile,
		ComPtr<IDxcBlob>& outShaderBlob,
		const std::vector<std::wstring>& defines)
	{
		if (!m_initialized)
		{
			if (!Initialize())
				return false;
		}

		if (!m_utils || !m_compiler)
			return false;

		// Resolve shader path
		std::wstring resolvedPath = filePath;
		if (!std::filesystem::exists(resolvedPath))
		{
			std::filesystem::path p(filePath);
			std::wstring candidate = L"Shaders/" + p.filename().wstring();
			if (std::filesystem::exists(candidate))
			{
				resolvedPath = candidate;
			}
			else
			{
				candidate = L"source/RenderCore/Shaders/" + p.filename().wstring();
				if (std::filesystem::exists(candidate))
				{
					resolvedPath = candidate;
				}
				else
				{
					candidate = L"../source/RenderCore/Shaders/" + p.filename().wstring();
					if (std::filesystem::exists(candidate))
					{
						resolvedPath = candidate;
					}
				}
			}
		}

		uint32_t codePage = DXC_CP_UTF8;
		ComPtr<IDxcBlobEncoding> sourceBlob;
		HRESULT hr = m_utils->LoadFile(resolvedPath.c_str(), &codePage, &sourceBlob);
		if (FAILED(hr))
		{
			wchar_t buffer[512];
			swprintf_s(buffer, L"Shader dosyası okunamadı: %ls", resolvedPath.c_str());
			::MessageBoxW(nullptr, buffer, L"Shader Hatası", MB_ICONERROR | MB_OK);
			return false;
		}

		DxcBuffer sourceBuffer = {};
		sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
		sourceBuffer.Size = sourceBlob->GetBufferSize();
		sourceBuffer.Encoding = DXC_CP_UTF8;

		std::vector<LPCWSTR> arguments;
		arguments.push_back(resolvedPath.c_str());
		arguments.push_back(L"-E"); arguments.push_back(entryPoint.c_str());
		arguments.push_back(L"-T"); arguments.push_back(targetProfile.c_str());
		arguments.push_back(L"-HV"); arguments.push_back(L"2021"); // HLSL 2021
		arguments.push_back(L"-I"); arguments.push_back(L"Shaders");
		arguments.push_back(L"-I"); arguments.push_back(L"source/RenderCore/Shaders");
		arguments.push_back(L"-all_resources_bound");
		arguments.push_back(L"-enable-16bit-types");

#if defined(_DEBUG)
		arguments.push_back(L"-Zi");
		arguments.push_back(L"-Od");
#else
		arguments.push_back(L"-O3");
		arguments.push_back(L"-Qstrip_reflect");
#endif

		for (const auto& def : defines)
		{
			arguments.push_back(L"-D");
			arguments.push_back(def.c_str());
		}

		ComPtr<IDxcResult> compileResult;
		hr = m_compiler->Compile(
			&sourceBuffer,
			arguments.data(),
			static_cast<UINT32>(arguments.size()),
			m_includeHandler.Get(),
			IID_PPV_ARGS(&compileResult)
		);

		if (FAILED(hr))
			return false;

		ComPtr<IDxcBlobUtf8> errors;
		compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (errors && errors->GetStringLength() > 0)
		{
			OutputDebugStringA(errors->GetStringPointer());
		}

		HRESULT status;
		compileResult->GetStatus(&status);
		if (FAILED(status))
		{
			if (errors && errors->GetStringLength() > 0)
			{
				::MessageBoxA(nullptr, errors->GetStringPointer(), "HLSL Derleme Hatası (DXC)", MB_ICONERROR | MB_OK);
			}
			return false;
		}

		compileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&outShaderBlob), nullptr);
		return true;
	}

	bool CShaderCompiler::CompileShaderFromSource(
		const std::string& source,
		const std::wstring& entryPoint,
		const std::wstring& targetProfile,
		ComPtr<IDxcBlob>& outShaderBlob,
		const std::vector<std::wstring>& defines)
	{
		if (!m_initialized)
		{
			if (!Initialize())
				return false;
		}

		if (!m_utils || !m_compiler)
			return false;

		DxcBuffer sourceBuffer = {};
		sourceBuffer.Ptr = source.data();
		sourceBuffer.Size = source.size();
		sourceBuffer.Encoding = DXC_CP_UTF8;

		std::vector<LPCWSTR> arguments;
		arguments.push_back(L"inline_shader.hlsl");
		arguments.push_back(L"-E"); arguments.push_back(entryPoint.c_str());
		arguments.push_back(L"-T"); arguments.push_back(targetProfile.c_str());
		arguments.push_back(L"-HV"); arguments.push_back(L"2021");
		arguments.push_back(L"-all_resources_bound");
		arguments.push_back(L"-enable-16bit-types");

#if defined(_DEBUG)
		arguments.push_back(L"-Zi");
		arguments.push_back(L"-Od");
#else
		arguments.push_back(L"-O3");
#endif

		for (const auto& def : defines)
		{
			arguments.push_back(L"-D");
			arguments.push_back(def.c_str());
		}

		ComPtr<IDxcResult> compileResult;
		HRESULT hr = m_compiler->Compile(
			&sourceBuffer,
			arguments.data(),
			static_cast<UINT32>(arguments.size()),
			m_includeHandler.Get(),
			IID_PPV_ARGS(&compileResult)
		);

		if (FAILED(hr))
			return false;

		ComPtr<IDxcBlobUtf8> errors;
		compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);

		HRESULT status;
		compileResult->GetStatus(&status);
		if (FAILED(status))
		{
			if (errors && errors->GetStringLength() > 0)
			{
				::MessageBoxA(nullptr, errors->GetStringPointer(), "HLSL Derleme Hatası (DXC)", MB_ICONERROR | MB_OK);
			}
			return false;
		}

		compileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&outShaderBlob), nullptr);
		return true;
	}

	bool CShaderCompiler::InitializePipelineLibrary(const std::wstring& cacheFilePath)
	{
		m_cacheFilePath = cacheFilePath;
		auto pDevice = CD3D12Device::Instance().GetDevice();

		// Read cache from disk if available
		std::ifstream file(cacheFilePath, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);

			if (size > 0)
			{
				m_libraryData.resize(static_cast<size_t>(size));
				file.read(reinterpret_cast<char*>(m_libraryData.data()), size);

				HRESULT hr = pDevice->CreatePipelineLibrary(m_libraryData.data(), m_libraryData.size(), IID_PPV_ARGS(&m_pipelineLibrary));
				if (SUCCEEDED(hr))
				{
					return true;
				}
			}
		}

		// Fallback: Create new empty pipeline library
		HRESULT hr = pDevice->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(&m_pipelineLibrary));
		return SUCCEEDED(hr);
	}

	bool CShaderCompiler::LoadGraphicsPipeline(const std::wstring& name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, ID3D12PipelineState** ppPSO)
	{
		if (!m_pipelineLibrary)
			return false;

		HRESULT hr = m_pipelineLibrary->LoadGraphicsPipeline(name.c_str(), &desc, IID_PPV_ARGS(ppPSO));
		return SUCCEEDED(hr);
	}

	bool CShaderCompiler::StoreGraphicsPipeline(const std::wstring& name, ID3D12PipelineState* pPSO)
	{
		if (!m_pipelineLibrary)
			return false;

		HRESULT hr = m_pipelineLibrary->StorePipeline(name.c_str(), pPSO);
		return SUCCEEDED(hr);
	}

	void CShaderCompiler::SavePipelineLibraryToDisk()
	{
		if (!m_pipelineLibrary || m_cacheFilePath.empty())
			return;

		SIZE_T librarySize = m_pipelineLibrary->GetSerializedSize();
		if (librarySize == 0)
			return;

		std::vector<uint8_t> buffer(librarySize);
		if (SUCCEEDED(m_pipelineLibrary->Serialize(buffer.data(), buffer.size())))
		{
			std::ofstream file(m_cacheFilePath, std::ios::binary);
			if (file.is_open())
			{
				file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
			}
		}
	}
}
