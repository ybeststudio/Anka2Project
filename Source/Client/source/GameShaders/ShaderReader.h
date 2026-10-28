#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9shader.h>
#include <fstream>

#include <vector>
#include <libloaderapi.h>
#include <errhandlingapi.h>
#include "../EterBase/Debug.h"

namespace DX
{
    inline std::vector<uint8_t> ReadBinaryFile(const std::string& filename)
    {
        // Open the file in binary mode
        std::ifstream inFile(filename, std::ios::binary);

        // Check if the file was opened successfully
        if (!inFile)
        {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        // Move the file pointer to the end to determine the file size
        inFile.seekg(0, std::ios::end);
        std::streamsize size = inFile.tellg();
        inFile.seekg(0, std::ios::beg); // Go back to the beginning

        // Check if the file size is valid
        if (size <= 0)
        {
            throw std::runtime_error("File is empty or invalid: " + filename);
        }

        // Resize the vector to hold the file data
        std::vector<uint8_t> buffer(size);

        // Read the data into the buffer
        if (!inFile.read(reinterpret_cast<char*>(buffer.data()), size))
        {
            throw std::runtime_error("Error reading from file: " + filename);
        }

        // Close the file
        inFile.close();

        return buffer; // Return the vector containing the binary data
    }
}

#ifdef DirectX9Ex
class ShaderReader
{
public:
	~ShaderReader() = default;
    ShaderReader(std::string Root)
    {
        Folder = Root;
    }

    inline void ReadShader(IDirect3DDevice9Ex* device, LPDIRECT3DVERTEXSHADER9& vs, LPDIRECT3DPIXELSHADER9& ps, std::string filename)
    {
        if (!ps)
            ps = ReadPrecompiledPS(device, filename + ".psc");
        
        if (!vs)
            vs = ReadPrecompiledVS(device, filename + ".vsc");
    }

    inline LPDIRECT3DVERTEXSHADER9 ReadPrecompiledVS(IDirect3DDevice9Ex* device, std::string filename)
    {
        LPDIRECT3DVERTEXSHADER9 shader = nullptr;
        auto shaderCode = DX::ReadBinaryFile(std::string(Folder + "/" + filename).c_str());

        auto VertexResult = device->CreateVertexShader(reinterpret_cast<const DWORD*>(shaderCode.data()), &shader);

        if (VertexResult != S_OK)
        {
            TraceError("Cannot load %s VS shader!", filename.c_str());
        }

        return shader;
    }

    inline LPDIRECT3DPIXELSHADER9 ReadPrecompiledPS(IDirect3DDevice9Ex* device, std::string filename)
    {
        LPDIRECT3DPIXELSHADER9 shader = nullptr;
        auto shaderCode = DX::ReadBinaryFile(std::string(Folder + "/" + filename).c_str());

        auto PixelResult = device->CreatePixelShader(reinterpret_cast<const DWORD*>(shaderCode.data()), &shader);

        if (PixelResult != S_OK)
        {
            TraceError("Cannot load %s PS shader!", filename.c_str());
        }

        return shader;
    }
protected:
    std::string Folder;

};
#else
class ShaderReader
{
public:
    ~ShaderReader() = default;
    ShaderReader(std::string Root)
    {
        Folder = Root;
    }

    inline void ReadShader(LPDIRECT3DDEVICE9 device, LPDIRECT3DVERTEXSHADER9& vs, LPDIRECT3DPIXELSHADER9& ps, std::string filename)
    {
        if (!ps)
            ps = ReadPrecompiledPS(device, filename + ".psc");

        if (!vs)
            vs = ReadPrecompiledVS(device, filename + ".vsc");
    }

    inline LPDIRECT3DVERTEXSHADER9 ReadPrecompiledVS(LPDIRECT3DDEVICE9 device, std::string filename)
    {
        LPDIRECT3DVERTEXSHADER9 shader = nullptr;
        auto shaderCode = DX::ReadBinaryFile(std::string(Folder + "/" + filename).c_str());

        auto VertexResult = device->CreateVertexShader(reinterpret_cast<const DWORD*>(shaderCode.data()), &shader);

        if (VertexResult != S_OK)
        {
            TraceError("Cannot load %s VS shader!", filename.c_str());
        }

        return shader;
    }

    inline LPDIRECT3DPIXELSHADER9 ReadPrecompiledPS(LPDIRECT3DDEVICE9 device, std::string filename)
    {
        LPDIRECT3DPIXELSHADER9 shader = nullptr;
        auto shaderCode = DX::ReadBinaryFile(std::string(Folder + "/" + filename).c_str());

        auto PixelResult = device->CreatePixelShader(reinterpret_cast<const DWORD*>(shaderCode.data()), &shader);

        if (PixelResult != S_OK)
        {
            TraceError("Cannot load %s PS shader!", filename.c_str());
        }

        return shader;
    }
protected:
    std::string Folder;

};
#endif
