#include "StdAfx.h"
#include "../EterLib/StateManager.h"
#include "../EterLib/Camera.h"
#include "FlyingObjectManager.h"
#include "MapManager.h"
#include "MapOutdoor.h"
#include "../UserInterface/PythonBackground.h"
#include <sstream>
#include <vector>
#include <string>
#include <cmath> // include this header for fmax
#include "MapManager.h"
#include <random>
#include <unordered_set>
#include <chrono>
#include <algorithm> // Para std::transform

int radomzPosition = (rand() % 5) + 1;

float z = 0.025f;
float zMultiplier = 15.0f;
float tolerance = 0.001f; // adjust this value to your desired tolerance level

bool a = true;
D3DXVECTOR3 VECTOR3(0.0f, 0.0f, 1.0f);

#include "../EterPack/EterPackManager.h"
#include "shadowmapVS.h"
//#include "red_shadowmapPS.h"
#include "shadowmapPS.h"
#include "terrainShadowsVS.h"
//#include "red_terrainShadowsPS.h"
#include "terrainShadowsPS.h"
#include "shadowmapObjectsVS.h"
//#include "red_shadowmapObjectsPS.h"
#include "shadowmapObjectsPS.h"
#include "shadowmapWaterVS.h"
#include "shadowmapWaterPSceva.h"
#include "shadowmapTreeVS.h"
#include "shadowmapTreePS.h"
#include "shadowmapDepthVS.h"
#include "shadowmapDepthPS.h"
#include "grassPS.h"
#include "grassVS.h"
#include "../UserInterface/InstanceBase.h"
#include "shadowmapVS2.h"
#include "shadowmapPS2.h"
#include "terrainShadowsVS2.h"
#include "terrainShadowsPS2.h"
#include "shadowmapObjectsVS2.h"
#include "shadowmapObjectsPS2.h"
#include "shadowmapVS3.h"
#include "shadowmapPS3.h"
#include "terrainShadowsVS3.h"
#include "terrainShadowsPS3.h"
#include "shadowmapObjectsVS3.h"
#include "shadowmapObjectsPS3.h"
#include "shadowmapWaterVS3.h"
#include "shadowmapWaterPS3.h"
#include "shadowmapWaterVS2.h"
#include "shadowmapWaterPS2.h"

std::string readShaderFromEterPack(const char* filename)
{
	std::string shaderCode;

	// Example usage of readShaderFromEterPack function
	CMappedFile mappedFile;
	LPCVOID pData;

	if (CEterPackManager::Instance().Get(mappedFile, filename, &pData))
	{
		shaderCode.assign(static_cast<const char*>(pData), mappedFile.Size());
	}

	return shaderCode;
}

LPDIRECT3DVERTEXSHADER9 CMapOutdoor::LoadAndBuildVertexShader(const char* filename)
{
	LPD3DXBUFFER pShaderBuffer = nullptr;
	LPD3DXBUFFER pErrorBuffer = nullptr;

	HRESULT hr = D3DXCompileShader(
		filename,            // Shader code
		strlen(filename),
		nullptr,       // Optional defines
		nullptr,       // Optional include handler
		"main",        // Entry point for the vertex shader
		"vs_3_0",      // Target shader model version 
		0,             // Compilation flags (0 for default)
		&pShaderBuffer,
		&pErrorBuffer,
		nullptr);

	if (FAILED(hr))
		if (pErrorBuffer) {
			const char* errorMsg = static_cast<const char*>(pErrorBuffer->GetBufferPointer());
			//TraceError("Vertex shader compilation error %s", errorMsg);
			return nullptr;
		}

	LPDIRECT3DVERTEXSHADER9 shader = nullptr;
	hr = ms_lpd3dDevice->CreateVertexShader(
		reinterpret_cast<DWORD*>(pShaderBuffer->GetBufferPointer()),
		&shader
	);
	if (FAILED(hr)) TraceError("CreateVertexShader failed");

	if (pShaderBuffer)
		pShaderBuffer->Release();

	return shader;
}

LPDIRECT3DPIXELSHADER9 CMapOutdoor::LoadAndBuildPixelShader(const char* filename, bool HLSL)
{
	LPD3DXBUFFER pShaderBuffer = nullptr;
	LPD3DXBUFFER pErrorBuffer = nullptr;
	HRESULT hr;
	if (HLSL) {
		hr = D3DXCompileShader(
			filename,            // Shader code
			strlen(filename),
			nullptr,       // Optional defines
			nullptr,       // Optional include handler
			"main",        // Entry point for the vertex shader
			"ps_3_0",      // Target shader model version 
			0,             // Compilation flags (0 for default)
			&pShaderBuffer,
			&pErrorBuffer,
			nullptr
		);
	}
	else {
		hr = D3DXAssembleShader(
			filename,
			strlen(filename),
			nullptr,
			nullptr,
			0,
			&pShaderBuffer,
			&pErrorBuffer);
	}
	if (FAILED(hr))
	{
		if (pErrorBuffer) {
			const char* errorMsg = static_cast<const char*>(pErrorBuffer->GetBufferPointer());
			//TraceError("Pixel shader compilation error %s", errorMsg);
			return nullptr;
		}
	}

	LPDIRECT3DPIXELSHADER9 shader = nullptr;
	hr = ms_lpd3dDevice->CreatePixelShader(
		reinterpret_cast<DWORD*>(pShaderBuffer->GetBufferPointer()),
		&shader
	);
	if (FAILED(hr))
		//TraceError("CreatePixelShader failed");

	if (pShaderBuffer)
		pShaderBuffer->Release();

	return shader;
}

void CMapOutdoor::CreateShadowmapShaders()
{
	if (m_lpShadowMapVertexShaderDepth && m_lpShadowMapPixelShaderDepth &&
		m_lpShadowMapVertexShaderTerrain && m_lpShadowMapPixelShaderTerrain &&
		m_lpPixelShaderTerrainBase && m_lpVertexShaderTerrainBase &&
		m_lpShadowMapVertexShaderObjects && m_lpShadowMapPixelShaderObjects &&
		m_lpShadowMapVertexShaderMain && m_lpShadowMapPixelShaderMain &&
		m_lpVertexShaderGrass && m_lpPixelShaderGrass)
	{
		shadersReleased = false;
		return;
	}

	if (!m_lpShadowMapVertexShaderDepth)
		m_lpShadowMapVertexShaderDepth = LoadAndBuildVertexShader(shadowmapDepthVS);

	if (!m_lpShadowMapPixelShaderDepth)
		m_lpShadowMapPixelShaderDepth = LoadAndBuildPixelShader(shadowmapDepthPS);

	// Terrain
	if (!m_lpShadowMapVertexShaderTerrain)
		m_lpShadowMapVertexShaderTerrain = LoadAndBuildVertexShader(terrainShadowsVS);

	if (!m_lpShadowMapPixelShaderTerrain)
		m_lpShadowMapPixelShaderTerrain = LoadAndBuildPixelShader(terrainShadowsPS);

	if (!m_lpPixelShaderTerrainBase)
		m_lpPixelShaderTerrainBase = LoadAndBuildPixelShader(terrainBasePS);

	if (!m_lpVertexShaderTerrainBase)
		m_lpVertexShaderTerrainBase = LoadAndBuildVertexShader(terrainBaseVS);

	//objects
	if (!m_lpShadowMapVertexShaderObjects)
		m_lpShadowMapVertexShaderObjects = LoadAndBuildVertexShader(shadowmapObjectsVS);

	if (!m_lpShadowMapPixelShaderObjects)
		m_lpShadowMapPixelShaderObjects = LoadAndBuildPixelShader(shadowmapObjectsPS);

	// Main render stage shaders
	if (!m_lpShadowMapVertexShaderMain)
		m_lpShadowMapVertexShaderMain = LoadAndBuildVertexShader(shadowmapVS);
	if (!m_lpShadowMapPixelShaderMain)
		m_lpShadowMapPixelShaderMain = LoadAndBuildPixelShader(shadowmapPS);

	// Grass
	if (!m_lpVertexShaderGrass)
		m_lpVertexShaderGrass = LoadAndBuildVertexShader(grassVertexShader);
	if (!m_lpPixelShaderGrass)
		m_lpPixelShaderGrass = LoadAndBuildPixelShader(grassPixelShader);

	shadersReleased = false;
}


void CMapOutdoor::CreateWaterShaders()
{
	if (m_lpShadowMapVertexShaderWater && m_lpShadowMapPixelShaderWater)
	{
		shadersWaterReleased = false;
		return;
	}

	if (!m_lpShadowMapVertexShaderWater)
		m_lpShadowMapVertexShaderWater = LoadAndBuildVertexShader(jtx_waterVertexShader);


	if (!m_lpShadowMapPixelShaderWater)
		m_lpShadowMapPixelShaderWater = LoadAndBuildPixelShader(jtx_waterPixelShader);

	shadersWaterReleased = false;
}

void CMapOutdoor::CreateTreeShaders()
{
	if (m_lpShadowMapVertexShaderTree && m_lpShadowMapPixelShaderTree && m_lpShadowMapPixelShaderDepthTree)
	{
		shadersTreeReleased = false;
		return;
	}

	if (!m_lpShadowMapVertexShaderTree)
		m_lpShadowMapVertexShaderTree = LoadAndBuildVertexShader(shadowmapTreeVS);
	if (!m_lpShadowMapPixelShaderTree)
		m_lpShadowMapPixelShaderTree = LoadAndBuildPixelShader(shadowmapTreePS);
	if (!m_lpShadowMapPixelShaderDepthTree)
		m_lpShadowMapPixelShaderDepthTree = LoadAndBuildPixelShader(shadowmapDepthTreePS, false);

	shadersTreeReleased = false;
}

void CMapOutdoor::ReleaseShaders()
{

	if (m_lpShadowMapVertexShaderDepth)
	{
		m_lpShadowMapVertexShaderDepth->Release();
		m_lpShadowMapVertexShaderDepth = nullptr;
	}

	if (m_lpShadowMapPixelShaderDepth)
	{
		m_lpShadowMapPixelShaderDepth->Release();
		m_lpShadowMapPixelShaderDepth = nullptr;
	}

	if (m_lpShadowMapVertexShaderTerrain)
	{
		m_lpShadowMapVertexShaderTerrain->Release();
		m_lpShadowMapVertexShaderTerrain = nullptr;
	}

	if (m_lpShadowMapPixelShaderTerrain)
	{
		m_lpShadowMapPixelShaderTerrain->Release();
		m_lpShadowMapPixelShaderTerrain = nullptr;
	}

	if (m_lpPixelShaderTerrainBase)
	{
		m_lpPixelShaderTerrainBase->Release();
		m_lpPixelShaderTerrainBase = nullptr;
	}

	if (m_lpVertexShaderTerrainBase)
	{
		m_lpVertexShaderTerrainBase->Release();
		m_lpVertexShaderTerrainBase = nullptr;
	}

	if (m_lpShadowMapVertexShaderObjects)
	{
		m_lpShadowMapVertexShaderObjects->Release();
		m_lpShadowMapVertexShaderObjects = nullptr;
	}

	if (m_lpShadowMapPixelShaderObjects)
	{
		m_lpShadowMapPixelShaderObjects->Release();
		m_lpShadowMapPixelShaderObjects = nullptr;
	}

	if (m_lpShadowMapVertexShaderMain)
	{
		m_lpShadowMapVertexShaderMain->Release();
		m_lpShadowMapVertexShaderMain = nullptr;
	}

	if (m_lpShadowMapPixelShaderMain)
	{
		m_lpShadowMapPixelShaderMain->Release();
		m_lpShadowMapPixelShaderMain = nullptr;
	}

	shadersReleased = true;
}

void CMapOutdoor::ReleaseWaterShaders()
{
	if (m_lpShadowMapVertexShaderWater)
	{
		m_lpShadowMapVertexShaderWater->Release();
		m_lpShadowMapVertexShaderWater = nullptr;
	}

	if (m_lpShadowMapPixelShaderWater)
	{
		m_lpShadowMapPixelShaderWater->Release();
		m_lpShadowMapPixelShaderWater = nullptr;
	}

	shadersWaterReleased = true;
}

void CMapOutdoor::ReleaseTreeShaders()
{
	if (m_lpShadowMapVertexShaderTree)
	{
		m_lpShadowMapVertexShaderTree->Release();
		m_lpShadowMapVertexShaderTree = nullptr;
	}

	if (m_lpShadowMapPixelShaderTree)
	{
		m_lpShadowMapPixelShaderTree->Release();
		m_lpShadowMapPixelShaderTree = nullptr;
	}

	if (m_lpShadowMapPixelShaderDepthTree)
	{
		m_lpShadowMapPixelShaderDepthTree->Release();
		m_lpShadowMapPixelShaderDepthTree = nullptr;
	}

	shadersTreeReleased = true;
}

void CMapOutdoor::SetShadowTextureSize(WORD size)
{
	if (m_wShadowMapSize != size)
	{
		recreate = true;
		Tracenf("ShadowTextureSize changed %d -> %d", m_wShadowMapSize, size);
	}

	m_wShadowMapSize = size;
}

void CMapOutdoor::LoadDepthTerrainTexture(const char* textureName, IDirect3DTexture9** texture)
{
	if (!texture || *texture)
		return;

	const char* pRelPath = (_strnicmp(textureName, "d:/", 3) == 0 || _strnicmp(textureName, "d:\\", 3) == 0) ? (textureName + 3) : textureName;

	CMappedFile	mappedFile;
	LPCVOID		c_pvMap = nullptr;

	bool bFound = CEterPackManager::Instance().Get(mappedFile, textureName, &c_pvMap);
	if (!bFound && pRelPath != textureName)
	{
		bFound = CEterPackManager::Instance().Get(mappedFile, pRelPath, &c_pvMap);
	}

	if (bFound && c_pvMap && mappedFile.Size() > 0)
	{
		if (SUCCEEDED(D3DXCreateTextureFromFileInMemory(ms_lpd3dDevice, static_cast<const char*>(c_pvMap), mappedFile.Size(), texture)))
		{
			return;
		}
	}

	// Direct disk fallback ONLY if file actually exists on disk (prevent Windows I/O freeze)
	if (_access(pRelPath, 0) == 0)
	{
		if (SUCCEEDED(D3DXCreateTextureFromFileA(ms_lpd3dDevice, pRelPath, texture)))
			return;
	}
	if (pRelPath != textureName && _access(textureName, 0) == 0)
	{
		if (SUCCEEDED(D3DXCreateTextureFromFileA(ms_lpd3dDevice, textureName, texture)))
			return;
	}
}

bool loadTerrainTexture = true;

struct STerrainFxTexture { const char* szPath; IDirect3DTexture9** ppTexture; };

// Shared terrain/water/grass effect textures (normal, specular, foam, grass). Order matters
// for fallbacks: a later entry for an already-loaded slot is skipped.
static const std::vector<STerrainFxTexture>& GetTerrainFxTextureTable()
{
	Frustum& f = Frustum::Instance();
	static const std::vector<STerrainFxTexture> s_kTable = {
		{ "d:/ymir work/terrainmaps/temp/shadowhelper.png", &f.shadowTexture },
		{ "d:/ymir work/terrainmaps/temp/shadowhelperblack.png", &f.shadowTextureBlack },
		{ "d:/ymir work/terrainmaps/temp/spheremap.jpg", &f.specularTexture },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/tile_final_normal.dds", &f.normalMapTile01 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/dirt_normal.dds", &f.normalMapRacketDirt },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/leafy_grass_nor_gl_2k.dds", &f.normalMapGrass },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/stone_normal.dds", &f.normalMapStone },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/snow_steps_normal.dds", &f.normalMapSnow },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/beach_normal.dds", &f.normalMapBitch },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/field.dds", &f.normalMapField },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/sand01_normal.dds", &f.normalMapSand01 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/sand03_normal.dds", &f.normalMapSand03 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/dry_ground_rocks_nor_gl_1k.dds", &f.normalMapDryGroundRock },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/cobblestone_color_nor_gl_2k.dds", &f.normalMapCobblestoneTemple1 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/cobblestone_floor_13_nor_gl_2k.dds", &f.normalMapCobblestoneTemple2 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/aerial_grass_rock_nor_gl_1k.dds", &f.normalMapAerialGrass },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/snow_02_nor_gl_1k.dds", &f.normalMapSnow1 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/snow1.dds", &f.normalMapSnow2 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/snow2.dds", &f.normalMapSnow3 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/ice_0002_normal_directx_2k.dds", &f.normalMapSnowIce },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/ice1.dds", &f.normalMapSnowIce },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/forest_leaves_03_nor_gl_1k.dds", &f.forest_leaves_03_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/dirt_aerial_02_nor_gl_1k.dds", &f.dirt_aerial_02_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/mud_cracked_dry_riverbed_002_nor_gl_2k.dds", &f.mud_cracked_dry_riverbed_002_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/rocky_trail_02_nor_gl_1k.dds", &f.rocky_trail_02_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/rock_wall_02_nor_gl_2k.dds", &f.rock_wall_02_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/leafy_grass_nor_gl_1k.dds", &f.leafy_grass_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/mud_forest_nor_gl_2k.dds", &f.mud_forest_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/grass_map1_n.dds", &f.grass_map1 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/mossy_rock_nor_gl_1k.dds", &f.mossy_rock_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/brown_mud_leaves_01_nor_gl_1k.dds", &f.brown_mud_leaves_01_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/cobblestone_large_01_nor_gl_1k.dds", &f.cobblestone_large_01_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/cobblestone_05_nor_gl_1k.dds", &f.cobblestone_05_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/playground_sand_nor_gl_1k.dds", &f.playground_sand_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/monastery_stone_floor_nor_gl_2k.dds", &f.monastery_stone_floor_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/cobblestone_floor_01_nor_gl_2k.dds", &f.cobblestone_floor_01_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/cobblestone_floor_02_nor_gl_2k.dds", &f.cobblestone_floor_02_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/concrete_rock_path_nor_gl_2k.dds", &f.concrete_rock_path_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/dirt_floor_nor_gl_2k.dds", &f.dirt_floor_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/forest_floor_nor_gl_2k.dds", &f.forest_floor_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/forrest_ground_03_nor_gl_2k.dds", &f.forrest_ground_03_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/gray_rocks_nor_gl_2k.dds", &f.gray_rocks_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/grey_stone_path_nor_gl_2k.dds", &f.grey_stone_path_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/leaves_forest_ground_nor_gl_2k.dds", &f.leaves_forest_ground_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/rock_06_nor_gl_2k.dds", &f.rock_06_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/rock_boulder_cracked_nor_gl_2k.dds", &f.rock_boulder_cracked_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/rock_face_03_nor_gl_2k.dds", &f.rock_face_03_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/rustic_stone_wall_nor_gl_2k.dds", &f.rustic_stone_wall_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/ground_0019_normal_directx_2k.dds", &f.ground_0019_color_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/ground_0007_normal_directx_2k.dds", &f.ground_0007_color_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/sand_dunes_n.dds", &f.sand_dunes },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/sandstone_cracks_nor_gl_2k.dds", &f.sandstone_cracks_diff_4k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/dry_ground_01_nor_gl_2k.dds", &f.dry_ground_01_diff_4k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/ice_0002_normal_directx_2k.dds", &f.ice_0002_color_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/ground_0030_normal_directx_2k.dds", &f.ground_0030_color_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/ground_0031_normal_directx_2k.dds", &f.ground_0031_color_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/rock_0008_normal_directx_2k.dds", &f.rock_0008_color_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/grassy_cobblestone_nor_gl_2k.dds", &f.grassy_cobblestone_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/aerial_rocks_04_nor_gl_2k.dds", &f.aerial_rocks_04_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/coast_sand_rocks_02_nor_gl_2k.dds", &f.coast_sand_rocks_02_diff_1k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/brown_mud_03_nor_gl_2k.dds", &f.brown_mud_03_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/slate_driveway_nor_gl_2k.dds", &f.slate_driveway_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/cracked_red_ground_nor_gl_2k.dds", &f.cracked_red_ground_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/concrete_floor_painted_nor_gl_2k.dds", &f.concrete_floor_painted_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/burned_ground_01_nor_gl_2k.dds", &f.burned_ground_01_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/brown_mud_dry_nor_gl_2k.dds", &f.brown_mud_dry_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/brown_mud_nor_gl_2k.dds", &f.brown_mud_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/brown_mud_02_nor_gl_2k.dds", &f.brown_mud_02_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/lichen_rock_nor_gl_2k.dds", &f.lichen_rock_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/lava1_n.dds", &f.lava1 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/lava2_n.dds", &f.lava2 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/grass_2k_n.dds", &f.grass_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Gravel014_2K-JPG_NormalDX.dds", &f.Gravel014_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground033_2K-JPG_NormalDX.dds", &f.Ground033_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground035_2K-JPG_NormalDX.dds", &f.Ground035_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground044_2K-JPG_NormalDX.dds", &f.Ground044_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground049B_2K-JPG_NormalDX.dds", &f.Ground049B_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground050_2K-JPG_NormalDX.dds", &f.Ground050_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground051_2K-JPG_NormalDX.dds", &f.Ground051_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground054_2K-JPG_NormalDX.dds", &f.Ground054_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground059_2K-JPG_NormalDX.dds", &f.Ground059_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground071_2K-JPG_NormalDX.dds", &f.Ground071_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Rock029_2K-JPG_NormalDX.dds", &f.Rock029_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/ground013_2.dds", &f.ground013 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground019_2K-JPG_NormalDX.dds", &f.ground019 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground030_2K-JPG_NormalDX.dds", &f.ground030 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground037_2K-JPG_NormalDX.dds", &f.ground037 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground069_2K-JPG_NormalDX.dds", &f.ground069 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/PavingStones009_2K-JPG_NormalDX.dds", &f.pavingstones009 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Rock028_2K-JPG_NormalDX.dds", &f.rock028 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Rock051_2K-JPG_NormalDX.dds", &f.rock051 },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/forest_leaves_02_nor_gl_2k.dds", &f.forest_leaves_02_diffuse_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/forest_leaves_04_nor_gl_2k.dds", &f.forest_leaves_04_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/forrest_ground_01_nor_gl_2k.dds", &f.forrest_ground_01_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground018_2K-JPG_NormalDX.dds", &f.Ground018_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground023_2K-JPG_NormalDX.dds", &f.Ground023_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground024_2K-JPG_NormalDX.dds", &f.Ground024_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground038_2K-JPG_NormalDX.dds", &f.Ground038_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground040_2K-JPG_NormalDX.dds", &f.Ground040_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground042_2K-JPG_NormalDX.dds", &f.Ground042_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground067_2K-JPG_NormalDX.dds", &f.Ground067_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/Ground068_2K-JPG_NormalDX.dds", &f.Ground068_2K },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/empty.dds", &f.emptyNormalMap },
		{ "D:/ymir work/terrainmaps/temp/normalmaps/sand_02_nor_gl_2k.dds", &f.sand_02_diff_2k },
		{ "D:/ymir work/terrainmaps/temp/specular/ground_0007_roughness_2k.dds", &f.waterSandSpecularTexture },
		{ "D:/ymir work/terrainmaps/temp/specular/ground_0019_color_2k.dds", &f.waterLeavesSpecularTexture },
		{ "D:/ymir work/terrainmaps/temp/specular/ground_0030_color_specular.dds", &f.iceSpecularTexture },
		{ "D:/ymir work/terrainmaps/temp/specular/Ground050_2K-JPG_Roughness.dds", &f.sandSpecularTexture },
		{ "D:/ymir work/terrainmaps/temp/cobblestone_05_disp_2k2.jpg", &f.displacementTexture },
		{ "D:/ymir work/terrainmaps/temp/foam.jpg", &f.foam },
		{ "D:/ymir work/terrainmaps/temp/tree/pine_bark_nor_gl_2k.dds", &f.treeBarkNormal },
		{ "D:/ymir work/terrainmaps/temp/tree/pine_bark_rough_4k.jpg", &f.treeBarkSpecular },
		{ "D:/ymir work/terrainmaps/temp/tree/treewillowoakbarkmoss.dds", &f.treeBarkTexture },
		{ "D:/ymir work/terrainmaps/grass/grass_map1_1.dds", &f.grass_map1_1 },
		{ "D:/ymir work/terrainmaps/grass/grass_map1_2.dds", &f.grass_map1_2 },
		{ "D:/ymir work/terrainmaps/grass/grass_map1_3.dds", &f.grass_map1_3 },
		{ "D:/ymir work/terrainmaps/grass/grass_map1_4.dds", &f.grass_map1_4 },
		{ "D:/ymir work/terrainmaps/grass/grains1.dds", &f.grass_grain_1 },
		{ "D:/ymir work/terrainmaps/grass/grains2.dds", &f.grass_grain_2 },
		{ "D:/ymir work/terrainmaps/grass/flower1.dds", &f.grass_flower_1 },
		{ "D:/ymir work/terrainmaps/grass/flower2.dds", &f.grass_flower_2 },
	};
	return s_kTable;
}

static size_t s_nTerrainFxTexturesLoaded = 0;

static void LoadTerrainFxTextures(DWORD dwBudgetMs)
{
	const std::vector<STerrainFxTexture>& rkTable = GetTerrainFxTextureTable();
	const DWORD dwStart = ELTimer_GetMSec();
	while (s_nTerrainFxTexturesLoaded < rkTable.size())
	{
		const STerrainFxTexture& rkEntry = rkTable[s_nTerrainFxTexturesLoaded++];
		CMapOutdoor::LoadDepthTerrainTexture(rkEntry.szPath, rkEntry.ppTexture);
		if (dwBudgetMs && ELTimer_GetMSec() - dwStart >= dwBudgetMs)
			break;
	}
	if (s_nTerrainFxTexturesLoaded >= rkTable.size())
		loadTerrainTexture = false;
}

// Character-select idle preloading. Every resource here lives in a static pointer and is
// map-independent, so loading it before the warp makes "select -> in game" much shorter.
// CreateShadowmapShaders / CreateWaterShaders / CreateTreeShaders and the texture block in
// CreateCharacterShadowTexture skip whatever is already loaded.
bool CMapOutdoor::PreloadSharedGraphicsStep(int iGraphicsLevel, DWORD dwBudgetMs)
{
	if (!ms_lpd3dDevice || iGraphicsLevel <= 0)
		return true;

	const DWORD dwStart = ELTimer_GetMSec();
	if (loadTerrainTexture)
	{
		LoadTerrainFxTextures(dwBudgetMs);
		if (loadTerrainTexture)
			return false;
	}

	struct SShaderJob { LPDIRECT3DVERTEXSHADER9* ppVS; LPDIRECT3DPIXELSHADER9* ppPS; const char* szSource; bool bHLSL; int iMinLevel; };
	const SShaderJob s_akJobs[] =
	{
		{ &m_lpShadowMapVertexShaderDepth, nullptr, shadowmapDepthVS, true, 1 },
		{ nullptr, &m_lpShadowMapPixelShaderDepth, shadowmapDepthPS, true, 1 },
		{ &m_lpShadowMapVertexShaderTerrain, nullptr, terrainShadowsVS, true, 1 },
		{ nullptr, &m_lpShadowMapPixelShaderTerrain, terrainShadowsPS, true, 1 },
		{ nullptr, &m_lpPixelShaderTerrainBase, terrainBasePS, true, 1 },
		{ &m_lpVertexShaderTerrainBase, nullptr, terrainBaseVS, true, 1 },
		{ &m_lpShadowMapVertexShaderObjects, nullptr, shadowmapObjectsVS, true, 1 },
		{ nullptr, &m_lpShadowMapPixelShaderObjects, shadowmapObjectsPS, true, 1 },
		{ &m_lpShadowMapVertexShaderMain, nullptr, shadowmapVS, true, 1 },
		{ nullptr, &m_lpShadowMapPixelShaderMain, shadowmapPS, true, 1 },
		{ &m_lpVertexShaderGrass, nullptr, grassVertexShader, true, 1 },
		{ nullptr, &m_lpPixelShaderGrass, grassPixelShader, true, 1 },
		{ &m_lpShadowMapVertexShaderWater, nullptr, jtx_waterVertexShader, true, 1 },
		{ nullptr, &m_lpShadowMapPixelShaderWater, jtx_waterPixelShader, true, 1 },
		{ &m_lpShadowMapVertexShaderTree, nullptr, shadowmapTreeVS, true, 2 },
		{ nullptr, &m_lpShadowMapPixelShaderTree, shadowmapTreePS, true, 2 },
		{ nullptr, &m_lpShadowMapPixelShaderDepthTree, shadowmapDepthTreePS, false, 2 },
	};

	for (const SShaderJob& rkJob : s_akJobs)
	{
		if (iGraphicsLevel < rkJob.iMinLevel)
			continue;
		if (rkJob.ppVS && !*rkJob.ppVS)
			*rkJob.ppVS = LoadAndBuildVertexShader(rkJob.szSource);
		else if (rkJob.ppPS && !*rkJob.ppPS)
			*rkJob.ppPS = LoadAndBuildPixelShader(rkJob.szSource, rkJob.bHLSL);
		else
			continue;

		if (dwBudgetMs && ELTimer_GetMSec() - dwStart >= dwBudgetMs)
			return false;
	}
	return true;
}


void CMapOutdoor::CreateCharacterShadowTexture()
{
	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;

	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (IsLowTextureMemory())
		SetShadowTextureSize(128);

	m_ShadowMapViewport.X = 0;
	m_ShadowMapViewport.Y = 0;
	m_ShadowMapViewport.Width = m_wShadowMapSize;
	m_ShadowMapViewport.Height = m_wShadowMapSize;
	m_ShadowMapViewport.MinZ = 0.0f;
	m_ShadowMapViewport.MaxZ = 1.0f;

	m_ShadowMapViewportTree.X = 0;
	m_ShadowMapViewportTree.Y = 0;
	m_ShadowMapViewportTree.Width = m_wShadowMapSize;
	m_ShadowMapViewportTree.Height = m_wShadowMapSize;
	m_ShadowMapViewportTree.MinZ = 0.0f;
	m_ShadowMapViewportTree.MaxZ = 1.0f;

	if (m_lpCharacterShadowMapTexture && m_lpCharacterShadowMapDepthSurface &&
		m_lpCharacterShadowMapTexture4 && m_lpCharacterShadowMapTexture2 &&
		Frustum::Instance().m_lpWaterReflectionTexture && !recreate)
	{
		return;
	}

	ReleaseCharacterShadowTexture();

	if (Frustum::Instance().shadowType == 2)
	{
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture, NULL)))
		{
			if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture, NULL)))
				return;
		}
	}
	else
	{
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture, NULL)))
		{
			if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture, NULL)))
				return;
		}
	}

	if (FAILED(m_lpCharacterShadowMapTexture->GetSurfaceLevel(0, &m_lpCharacterShadowMapRenderTargetSurface)))
	{
		return;
	}

	if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture4, NULL)))
	{
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture4, NULL)))
			return;
	}

	if (FAILED(m_lpCharacterShadowMapTexture4->GetSurfaceLevel(0, &m_lpCharacterShadowMapRenderTargetSurface4)))
	{
		return;
	}

	if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture2, NULL)))
	{
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture2, NULL)))
			return;
	}

	if (FAILED(m_lpCharacterShadowMapTexture2->GetSurfaceLevel(0, &m_lpCharacterShadowMapRenderTargetSurface2)))
	{
		return;
	}

	if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_wShadowMapSize, m_wShadowMapSize, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpCharacterShadowMapDepthSurface, NULL)))
	{
		if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_wShadowMapSize, m_wShadowMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpCharacterShadowMapDepthSurface, NULL)))
		{
			if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_wShadowMapSize, m_wShadowMapSize, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpCharacterShadowMapDepthSurface, NULL)))
				return;
		}
	}

	if (loadTerrainTexture)
	{
		LoadTerrainFxTextures(0);
	}


	if (!Frustum::Instance().m_lpWaterReflectionTexture)
	{
		m_waterReflectionTexSize = 2048;

		if (FAILED(ms_lpd3dDevice->CreateTexture(m_waterReflectionTexSize, m_waterReflectionTexSize, 1, D3DUSAGE_RENDERTARGET,
			D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &Frustum::Instance().m_lpWaterReflectionTexture, NULL)))
		{
			//TraceError("CMapOutdoor Unable to create Water Reflection render target texture\n");
			return;
		}
	}

	if (!m_lpWaterReflectionDepthSurface)
	{
		if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_waterReflectionTexSize, m_waterReflectionTexSize, D3DFMT_D16,
			D3DMULTISAMPLE_NONE, 0, true, &m_lpWaterReflectionDepthSurface, NULL)))
		{
			//TraceError("CMapOutdoor Unable to create Water Reflection depth surface\n");
			return;
		}
	}

	if (!Frustum::Instance().m_lpPuddleReflectionTexture)
	{
		m_waterPuddleReflectionTexSize = 1024;

		if (FAILED(ms_lpd3dDevice->CreateTexture(m_waterPuddleReflectionTexSize, m_waterPuddleReflectionTexSize, 1, D3DUSAGE_RENDERTARGET,
			D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &Frustum::Instance().m_lpPuddleReflectionTexture, NULL)))
		{
			//TraceError("CMapOutdoor Unable to create Puddle Reflection render target texture\n");
			return;
		}
	}

	if (!m_lpPuddleReflectionDepthSurface)
	{
		if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_waterPuddleReflectionTexSize, m_waterPuddleReflectionTexSize, D3DFMT_D16,
			D3DMULTISAMPLE_NONE, 0, true, &m_lpPuddleReflectionDepthSurface, NULL)))
		{
			//TraceError("CMapOutdoor Unable to create Water Reflection depth surface\n");
			return;
		}
	}
}

void CMapOutdoor::RenderGrass()
{
	D3DXVECTOR3 grassPosition = Frustum::Instance().characterPosition;
	D3DXMATRIX matShader;

	if (ms_lpd3dDevice == NULL) return;



	static const float radius = 8000.0f; // Sichtbarer Radius
	static const float cullingDistanceSquared = radius * radius;
	static const float flattenRadiusSquared = 100.0f * 100.0f; 
	static const size_t maxGrassInstances = 50000; // dichte 
	static const size_t maxVisibleGrass = 5000; // wv man sieht

	if (!Frustum::Instance().grassTexturesInitialized) {
		float width = 80.0f; // breite vom grass
		float height = 210.0f; // Höhe vom gras
		GrassVertex grassVertices[8] = {
			{ -width, 0.0f, 0.0f, 0.0f, 1.0f },
			{  width, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -width, 0.0f, height, 0.0f, 0.0f },
			{  width, 0.0f, height, 1.0f, 0.0f },
			{ 0.0f, -width, 0.0f, 0.0f, 1.0f },
			{ 0.0f,  width, 0.0f, 1.0f, 1.0f },
			{ 0.0f, -width, height, 0.0f, 0.0f },
			{ 0.0f,  width, height, 1.0f, 0.0f }
		};

		HRESULT hr = ms_lpd3dDevice->CreateVertexBuffer(8 * sizeof(GrassVertex), 0, GrassVertex::FVF, D3DPOOL_MANAGED, &m_pGrassVertexBuffer, NULL);
		if (FAILED(hr) || m_pGrassVertexBuffer == NULL) return;

		void* pVertices;
		hr = m_pGrassVertexBuffer->Lock(0, 8 * sizeof(GrassVertex), (void**)&pVertices, 0);
		if (SUCCEEDED(hr)) {
			memcpy(pVertices, grassVertices, 8 * sizeof(GrassVertex));
			m_pGrassVertexBuffer->Unlock();
		}
		else {
			m_pGrassVertexBuffer->Release();
			return;
		}

		WORD grassIndices[12] = {
			0, 1, 2,
			2, 1, 3,
			4, 5, 6,
			6, 5, 7
		};

		hr = ms_lpd3dDevice->CreateIndexBuffer(12 * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pGrassIndexBuffer, NULL);
		if (FAILED(hr) || m_pGrassIndexBuffer == NULL) {
			m_pGrassVertexBuffer->Release();
			return;
		}

		void* pIndices;
		hr = m_pGrassIndexBuffer->Lock(0, 12 * sizeof(WORD), (void**)&pIndices, 0);
		if (SUCCEEDED(hr)) {
			memcpy(pIndices, grassIndices, 12 * sizeof(WORD));
			m_pGrassIndexBuffer->Unlock();
		}
		else {
			m_pGrassVertexBuffer->Release();
			m_pGrassIndexBuffer->Release();
			return;
		}

		Frustum::Instance().grassTexturesInitialized = true;
	}

	size_t currentGrassCount = Frustum::Instance().grassMatrices.size();
	CInstanceBase instance;

	bool needToSpawnGrass = (currentGrassCount < maxVisibleGrass);

	// Berechne Distanz zum letzten Graspunkt
	D3DXVECTOR3 diff = grassPosition - Frustum::Instance().lastGrassPosition;
	float distanceMovedSquared = D3DXVec3LengthSq(&diff);

	bool moveTriggeredSpawn = (distanceMovedSquared > (radius * radius / 4.0f));

	// Bei Bewegung: letzten Graspunkt aktualisieren und Spawning erzwingen
	if (moveTriggeredSpawn) {
		Frustum::Instance().lastGrassPosition = grassPosition;
		needToSpawnGrass = true;
	}

	if (needToSpawnGrass) {
		size_t spawnCount = maxVisibleGrass * 10;

		for (size_t i = 0; i < spawnCount; ++i) {
			// Zufälliger Winkel im Kreis (0 - 2?)
			float angle = (float(rand()) / RAND_MAX) * 2.0f * 3.14159265f;

			float distance;
			if (moveTriggeredSpawn && currentGrassCount > 0) {
				distance = radius + (float(rand()) / RAND_MAX * radius); // Ring [r, 2r]
			} else {
				distance = radius * sqrtf(float(rand()) / RAND_MAX);     // Kreis [0, r]
			}


			// Umrechnung in X/Z-Koordinaten
			float offsetX = distance * cosf(angle);
			float offsetZ = distance * sinf(angle);

			float actualX = grassPosition.x + offsetX;
			float actualZ = grassPosition.y + offsetZ;

			// Zufälliger Skalierungsfaktor (z.?B. für optische Vielfalt)
			float randomValue = (rand() % 20 == 0) ? 0.6f + float(rand()) / RAND_MAX * 0.6f : 1.0f;

			// Prüfe, ob Position gültig für Gras
			if (instance.IsGrassPosition(actualX, actualZ)) {
				D3DXMATRIX matTranslation;
				D3DXMatrixIdentity(&matTranslation);

				float groundHeight = CFlyingManager::Instance().GetMapManagerPtr()->GetHeight(actualX, actualZ);

				// Transformation berechnen
				D3DXMatrixTranslation(&matTranslation, actualX, actualZ, groundHeight - 15.0f * randomValue);

				// Hinzufügen
				Frustum::Instance().grassMatrices.push_back(matTranslation);

				// Maximalanzahl beachten
				if (Frustum::Instance().grassMatrices.size() > maxGrassInstances) {
					Frustum::Instance().grassMatrices.pop_front();
				}
			}
		}
	}


	//TraceError("Grass Matrix: %d", Frustum::Instance().grassMatrices.size());

	STATEMANAGER.SetVertexShader(m_lpVertexShaderGrass);
	STATEMANAGER.SetPixelShader(m_lpPixelShaderGrass);

	if (m_lpCharacterShadowMapTexture)
		STATEMANAGER.SetTexture(1, m_lpCharacterShadowMapTexture);
	if (m_lpCharacterShadowMapTexture4)
		STATEMANAGER.SetTexture(2, m_lpCharacterShadowMapTexture4);

	D3DXMATRIX worldMat;
	ms_lpd3dDevice->GetTransform(D3DTS_WORLD, &worldMat);
	D3DXMatrixTranspose(&worldMat, &worldMat);
	ms_lpd3dDevice->SetVertexShaderConstantF(0, (const float*)(&worldMat), 4);

	ms_lpd3dDevice->SetVertexShaderConstantF(49, grassPosition, 1);
	ms_lpd3dDevice->SetPixelShaderConstantF(101, grassPosition, 1);

	D3DXMATRIX viewProjMat, viewMat, projMat;
	ms_lpd3dDevice->GetTransform(D3DTS_VIEW, &viewMat);
	ms_lpd3dDevice->GetTransform(D3DTS_PROJECTION, &projMat);
	D3DXMatrixMultiplyTranspose(&viewProjMat, &viewMat, &projMat);
	ms_lpd3dDevice->SetVertexShaderConstantF(4, (const float*)(&viewProjMat), 4);
	STATEMANAGER.SetPixelShaderConstant(1, &current_v3SunPosition, 1);

	D3DLIGHT9 light;
	STATEMANAGER.GetLight(0, &light);
	STATEMANAGER.SetPixelShaderConstant(2, &light.Diffuse, 1);
	STATEMANAGER.SetPixelShaderConstant(3, &light.Ambient, 1);

	ms_lpd3dDevice->SetVertexShaderConstantF(8, (const float*)(&m_MatLightViewProj), 4);
	STATEMANAGER.SetVertexShaderConstant(16, (const float*)(&m_MatLightViewProj2), 4);

	// Grass wind follows the tree wind (speedtree_wind option + weather). Time is accumulated
	// from frame deltas and wrapped so float precision never degrades with uptime.
	static double s_dGrassWindTime = 0.0;
	static float s_fGrassLastTime = CTimer::Instance().GetCurrentSecond();
	const float fGrassNow = CTimer::Instance().GetCurrentSecond();
	s_dGrassWindTime = fmod(s_dGrassWindTime + __min(__max(fGrassNow - s_fGrassLastTime, 0.0f), 0.1f), 3600.0);
	s_fGrassLastTime = fGrassNow;

	const float fWind = CSpeedTreeForestDirectX8::Instance().GetWindStrength();
	const float windDirection[4] = { 0.894f, 0.447f, 0.0f, 0.0f };				// horizontal unit vector
	const float windParams[4] = { fWind > 0.0f ? 18.0f + 40.0f * fWind : 0.0f, 1.0f / 210.0f, 0.0f, 0.0f };	// x: tip amplitude, y: 1/blade height
	const float windTime[4] = { (float)s_dGrassWindTime, 0.0f, 0.0f, 0.0f };
	ms_lpd3dDevice->SetVertexShaderConstantF(46, windDirection, 1);
	ms_lpd3dDevice->SetVertexShaderConstantF(47, windParams, 1);
	ms_lpd3dDevice->SetVertexShaderConstantF(48, windTime, 1);

	D3DXVECTOR3 cameraPos = CCameraManager::instance().GetCurrentCamera()->GetEye();
	STATEMANAGER.SetPixelShaderConstant(4, &cameraPos, 1);

	D3DXCOLOR fogColorX = mc_pEnvironmentData ? mc_pEnvironmentData->FogColor : D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	STATEMANAGER.SetPixelShaderConstant(5, (float*)&fogColorX, 1);

	float fogStart, fogEnd;
	STATEMANAGER.GetRenderState(D3DRS_FOGSTART, (DWORD*)&fogStart);
	STATEMANAGER.GetRenderState(D3DRS_FOGEND, (DWORD*)&fogEnd);
	const float fogRange[4] = { fogStart, fogEnd, 0.0f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(6, fogRange, 1);
	__SetAtmosphereShaderConstants();

	STATEMANAGER.SetPixelShaderConstant(86, &grassPosition, 1);

	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	ms_lpd3dDevice->SetStreamSource(0, m_pGrassVertexBuffer, 0, sizeof(GrassVertex));
	ms_lpd3dDevice->SetFVF(GrassVertex::FVF);
	ms_lpd3dDevice->SetIndices(m_pGrassIndexBuffer);

	std::vector<size_t> visibleGrassIndices;
	visibleGrassIndices.reserve(maxVisibleGrass);

	for (size_t i = 0; i < Frustum::Instance().grassMatrices.size(); ++i) {
		const D3DXMATRIX& matrix = Frustum::Instance().grassMatrices[i];
		D3DXVECTOR3 grassInstancePos(matrix._41, matrix._42, matrix._43);

		D3DXVECTOR3 diff = grassInstancePos - grassPosition;
		if (D3DXVec3LengthSq(&diff) <= cullingDistanceSquared) {
			visibleGrassIndices.push_back(i);
			if (visibleGrassIndices.size() >= maxVisibleGrass)
				break;
		}
	}

	for (size_t i : visibleGrassIndices) {
		if (i >= Frustum::Instance().grassMatrices.size()) continue;

		if (Frustum::Instance().grassType == 0)
		{
			if (i % 5 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_2);
			else if (i % 50 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_flower_1);
			else
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_1);
		}

		if (Frustum::Instance().grassType == 1)
		{
			if (i % 10 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_3);
			else
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_4);
		}

		if (Frustum::Instance().grassType == 2)
		{
			if (i % 200 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_flower_1);
			else if (i % 10 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_grain_2);
			else
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_grain_1);
		}

		if (Frustum::Instance().grassType == 3)
		{
			if (i % 50 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_flower_1);
			else if (i % 25 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_flower_2);
			else if (i % 10 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_grain_2);
			else
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_2);

		}

		D3DXMATRIX& matrix = Frustum::Instance().grassMatrices[i];
		D3DXMatrixTranspose(&matShader, &matrix);
		STATEMANAGER.SetVertexShaderConstant(20, &matShader, 4);
		ms_lpd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 4);
	}

	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);

	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTexture(2, nullptr);
	ms_lpd3dDevice->SetStreamSource(0, nullptr, 0, 0);
	ms_lpd3dDevice->SetIndices(nullptr);
	STATEMANAGER.SetVertexShader(nullptr);
	STATEMANAGER.SetPixelShader(nullptr);
}


void CMapOutdoor::RenderGrassShadow()
{
	D3DXVECTOR3 grassPosition = Frustum::Instance().characterPosition;
	D3DXMATRIX matShader;

	if (ms_lpd3dDevice == NULL) return;

	static float radius = 1000.0f;
	static std::unordered_map<size_t, std::pair<float, std::chrono::steady_clock::time_point>> flattenedGrassData;

	if (!Frustum::Instance().grassTexturesInitialized) return;
	D3DXVECTOR3 diff = grassPosition - Frustum::Instance().lastGrassShadowPosition;
	float distanceMovedSquared = D3DXVec3LengthSq(&diff);
	float thresholdDistance = 1000.0f * 1000.0f;

	if (distanceMovedSquared > thresholdDistance) {
		Frustum::Instance().lastGrassShadowPosition = grassPosition;

		CInstanceBase instance;
		int numGrassInstances = 500;
		float boundaryRadius = radius + 500.0f;

		for (int i = 0; i < numGrassInstances; ++i) {
			float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.141245f;
			float distance = radius + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1000);

			float x = distance * cos(angle);
			float z = distance * sin(angle);

			float actualX = grassPosition.x + x;
			float actualZ = grassPosition.y + z;
			float randomValue = (rand() % 20 == 0)
				? 0.6f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (1.0f - 0.6f)
				: 1.0f;

			if (instance.IsGrassPosition(actualX, actualZ)) {
				D3DXMATRIX matTranslation;
				D3DXMatrixIdentity(&matTranslation);
				float fGroundHeight = CFlyingManager::Instance().GetMapManagerPtr()->GetHeight(actualX, actualZ);
				D3DXMatrixTranslation(&matTranslation, actualX, actualZ, fGroundHeight - 15.0f * randomValue);
				Frustum::Instance().grassShadowMatrices.push_back(matTranslation);
			}
		}
	}

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x08);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	ms_lpd3dDevice->SetStreamSource(0, m_pGrassVertexBuffer, 0, sizeof(GrassVertex));
	ms_lpd3dDevice->SetFVF(GrassVertex::FVF);
	ms_lpd3dDevice->SetIndices(m_pGrassIndexBuffer);

	const Frustum& frustum = Frustum::Instance();

	float cullingDistanceSquared = radius * radius;

	std::vector<size_t> visibleGrassIndices;

	for (size_t i = 0; i < Frustum::Instance().grassShadowMatrices.size(); ++i) {
		const D3DXMATRIX& mat = Frustum::Instance().grassShadowMatrices[i];

		D3DXVECTOR3 grassInstancePos(mat._41, mat._42, mat._43);
		D3DXVECTOR3 diff = grassInstancePos - grassPosition;
		float distanceSquared = D3DXVec3LengthSq(&diff);

		if (distanceSquared <= cullingDistanceSquared) {
			visibleGrassIndices.push_back(i);
		}
	}

	const size_t maxDequeSize = 3000;
	const size_t elementsToRemove = 500;

	if (Frustum::Instance().grassShadowMatrices.size() >= maxDequeSize) {
		Frustum::Instance().grassShadowMatrices.erase(Frustum::Instance().grassShadowMatrices.begin(), Frustum::Instance().grassShadowMatrices.begin() + elementsToRemove);
	}

	for (size_t i : visibleGrassIndices)
	{
		if (i >= Frustum::Instance().grassShadowMatrices.size())
			continue;

		D3DXMATRIX& matrix = Frustum::Instance().grassShadowMatrices[i];
		if (!matrix)
			continue;

		D3DXVECTOR3 grassInstancePos(matrix._41, matrix._42, matrix._43);
		Vector3d grassInstancePos3d(grassInstancePos.x, grassInstancePos.y, grassInstancePos.z);
		ViewState viewState = frustum.ViewVolumeTestEffects(grassInstancePos3d, 100.0f);

		if (viewState == VS_OUTSIDE)
			continue;

		if (Frustum::Instance().grassType == 0)
		{
			if (i % 5 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_2);
			else
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_1);
		}

		if (Frustum::Instance().grassType == 1)
		{
			if (i % 5 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_3);
			else
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_map1_4);
		}

		if (Frustum::Instance().grassType == 2)
		{
			if (i % 5 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_grain_2);
			else
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_grain_1);
		}

		if (Frustum::Instance().grassType == 3)
		{
			if (i % 5 == 0)
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_flower_1);
			else
				ms_lpd3dDevice->SetTexture(0, Frustum::Instance().grass_flower_2);
		}

		D3DXMATRIX matTranslate;
		D3DXMatrixTranslation(&matTranslate, Frustum::Instance().grassShadowMatrices[i]._41, Frustum::Instance().grassShadowMatrices[i]._42, Frustum::Instance().grassShadowMatrices[i]._43);
		ms_lpd3dDevice->SetTransform(D3DTS_WORLD, &matTranslate);
		ms_lpd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 4);
	}

	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
}

void CMapOutdoor::ReleaseCharacterShadowTexture()
{
	SAFE_RELEASE(m_lpCharacterShadowMapRenderTargetSurface);
	SAFE_RELEASE(m_lpCharacterShadowMapRenderTargetSurface2);
	SAFE_RELEASE(m_lpCharacterShadowMapRenderTargetSurface4);
	SAFE_RELEASE(m_lpCharacterShadowMapTexture);
	SAFE_RELEASE(m_lpCharacterShadowMapTexture2);
	SAFE_RELEASE(m_lpCharacterShadowMapTexture4);
	SAFE_RELEASE(m_lpCharacterShadowMapDepthSurface);
	SAFE_RELEASE(Frustum::Instance().m_lpWaterReflectionTexture);
	SAFE_RELEASE(Frustum::Instance().m_lpPuddleReflectionTexture);
	SAFE_RELEASE(Frustum::Instance().TextureReflectionMap);
	SAFE_RELEASE(m_lpWaterReflectionDepthSurface);
	SAFE_RELEASE(m_lpPuddleReflectionDepthSurface);
	SAFE_RELEASE(m_waterOcclusionQuery);
}

void CMapOutdoor::ReleaseReflectionTexture()
{
	SAFE_RELEASE(m_waterOcclusionQuery);
	SAFE_RELEASE(Frustum::Instance().m_lpWaterReflectionTexture);
	SAFE_RELEASE(m_lpWaterReflectionDepthSurface);
}

DWORD dwLightEnable = FALSE;

// Creates/releases the shader sets the current graphics level needs. Runs every frame at every
// level (it used to live only in SkyUtil, which is only called on the "low" shadow path, so
// switching Medium -> Ultra never created the tree-shadow shaders and Ultra looked like Medium).
void CMapOutdoor::UpdateShaderResidency()
{
	const Frustum& rkFrustum = Frustum::Instance();

	if (rkFrustum.graphicsLevel == 0)
	{
		if (!shadersReleased)
		{
			ReleaseShaders();
			ReleaseCharacterShadowTexture();
		}
	}

	// Water shaders are used at water level 1 (shader, no reflection) and 2 (reflection);
	// only level 0 releases them. The old "!= 2" rule would have dropped them at level 1.
	if ((rkFrustum.graphicsWaterLevel == 0 || rkFrustum.graphicsLevel == 0) && !shadersWaterReleased)
		ReleaseWaterShaders();

	if ((rkFrustum.shadowRender < 2 || rkFrustum.graphicsLevel == 0) && !shadersTreeReleased)
		ReleaseTreeShaders();

	if (rkFrustum.graphicsLevel > 0 && shadersReleased == true)
		CreateShadowmapShaders();

	// Missing shaders are (re)built when released or when the level changes -- not every frame,
	// so a shader that fails to compile does not stall each frame.
	static int s_iLastWaterLevel = -1;
	static int s_iLastShadowRender = -1;
	const bool bWaterLevelChanged = s_iLastWaterLevel != rkFrustum.graphicsWaterLevel;
	const bool bShadowRenderChanged = s_iLastShadowRender != rkFrustum.shadowRender;
	s_iLastWaterLevel = rkFrustum.graphicsWaterLevel;
	s_iLastShadowRender = rkFrustum.shadowRender;

	if (rkFrustum.graphicsLevel > 0 && rkFrustum.graphicsWaterLevel > 0 &&
		(shadersWaterReleased || (bWaterLevelChanged && (!m_lpShadowMapVertexShaderWater || !m_lpShadowMapPixelShaderWater))))
		CreateWaterShaders();

	if (rkFrustum.graphicsLevel > 0 && rkFrustum.shadowRender == 2 &&
		(shadersTreeReleased || (bShadowRenderChanged && !m_lpShadowMapPixelShaderDepthTree)))
		CreateTreeShaders();

	if (recreate && rkFrustum.graphicsLevel != 0)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}
}

void CMapOutdoor::SkyUtil()
{
	UpdateShaderResidency();

	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);
	D3DXVECTOR3 v3SunPosition = envData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction;
	v3SunPosition.z = v3SunPosition.z * 1.5f;

	Frustum::Instance().currentEnvironmentFace = envData->strSkyBoxFaceFileName[0].c_str();

	Frustum::Instance().waterSunDirection = { 0.0, 0.0, 0.0 };
	Frustum::Instance().characterSpecularLightPower = 0.8f;
	Frustum::Instance().isNightEnvironment = 0.0f;
	Frustum::Instance().isStormEnvironment = 0.0f;
	Frustum::Instance().AmbientLight = { 0.4f, 0.4f, 0.4f };
	isNoapteEnvironment = false;
	Frustum::Instance().treeNormalMap = 2.0f;
	Frustum::Instance().treeFinalColor = 65.0f;

	if (Frustum::Instance().currentEnvironmentFace.find("environment01_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.0f;
		Frustum::Instance().isNightEnvironment = 1.0f;
		Frustum::Instance().waterOpacity = 0.7f;
		isNoapteEnvironment = true;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment02_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 5.0f;
		Frustum::Instance().characterSpecularLightPower = 1.4f;
		Frustum::Instance().waterOpacity = 1.0f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment03_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 4.5f;
		Frustum::Instance().characterSpecularLightPower = 1.0f;
		Frustum::Instance().waterOpacity = 1.0f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment04_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.0f;
		Frustum::Instance().waterOpacity = 0.6f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment05_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 4.0f;
		Frustum::Instance().characterSpecularLightPower = 1.1f;
		Frustum::Instance().waterOpacity = 1.0f;
		Frustum::Instance().AmbientLight = { 0.03f, 0.03f, 0.03f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment06_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 8.0f;
		Frustum::Instance().characterSpecularLightPower = 1.6f;
		Frustum::Instance().waterOpacity = 1.0f;
		Frustum::Instance().AmbientLight = { 0.8f, 0.8f, 0.8f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment07_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.0f;
		Frustum::Instance().characterSpecularLightPower = 1.1f;
		Frustum::Instance().waterOpacity = 1.0f;
		Frustum::Instance().AmbientLight = { 0.03f, 0.03f, 0.03f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment08_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 4.0f;
		Frustum::Instance().characterSpecularLightPower = 1.1f;
		Frustum::Instance().waterOpacity = 0.8f;
		Frustum::Instance().treeNormalMap = 2.0f;
		Frustum::Instance().treeFinalColor = 25.0f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment09_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 11.5f;
		Frustum::Instance().characterSpecularLightPower = 1.1f;
		Frustum::Instance().waterOpacity = 0.7f;
		Frustum::Instance().AmbientLight = { 10.0f, 10.0f, 10.0f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment10_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.5f;
		Frustum::Instance().characterSpecularLightPower = 1.45f;
		Frustum::Instance().waterOpacity = 1.5f;
		Frustum::Instance().AmbientLight = { 0.8f, 0.8f, 0.8f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment11_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 11.5f;
		Frustum::Instance().waterSunDirection = { -0.020000, -0.967921, -0.215650 };
		Frustum::Instance().characterSpecularLightPower = 2.1f;
		Frustum::Instance().waterOpacity = 1.5f;
		Frustum::Instance().AmbientLight = { 0.03f, 0.03f, 0.03f };
		Frustum::Instance().treeNormalMap = 0.0f;
		Frustum::Instance().treeFinalColor = 155.0f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("rain") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.0f;
		Frustum::Instance().characterSpecularLightPower = 2.0f;
		Frustum::Instance().isNightEnvironment = 1.0f;
		Frustum::Instance().waterOpacity = 1.0f;
		isNoapteEnvironment = true;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("storm") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 10.0f;
		Frustum::Instance().characterSpecularLightPower = 2.0f;
		Frustum::Instance().isNightEnvironment = 1.0f;
		Frustum::Instance().isStormEnvironment = 1.0f;
		Frustum::Instance().waterOpacity = 1.0f;
		isNoapteEnvironment = true;
	}
	else
	{
		Frustum::Instance().terrainLightPowerEnvBased = 6.0f;
		Frustum::Instance().characterSpecularLightPower = 1.0f;
		Frustum::Instance().isNightEnvironment = 1.0f;
		Frustum::Instance().isStormEnvironment = 1.0f;
		Frustum::Instance().waterOpacity = 1.0f;
		isNoapteEnvironment = true;
	}

#if !defined(ANK_SYSTEM_GRAPHICS) || !defined(ANKIRA_WATER_REFLECTION)
	Frustum::Instance().waterOpacity = 0.7f;
#endif

#include <unordered_map>

	std::unordered_map<float, bool*> environmentFlags{
		{-0.921066f, &isPinkEnvironment},
		{ 0.128933f, &Frustum::Instance().isCerApus4Environment },
		{ -0.196598, &Frustum::Instance().isCerApus4EnvironmentAzrael },
		{ -0.002973, &isNoapteEnvironment },
		{ 0.998932f, &isEnvironment6 },
		{ -0.021067f, &isEnvironment7 },
		{ 0.968932f, &isEnvironment9 },
		{ 0.968932f, &isEnvironment10 },
		{ 0.252034f, &isEnvironment2 },
		{ 0.218933f, &isEnvironment3 },
		{ 0.008933f, &isEnvironment4 },
		{ 0.007122f, &isEnvironment5 },
		{ 0.969653f, &isEnvironment11 }
	};

	float epsilon = 0.00001f;

	for (auto& pair : environmentFlags) {
		*(pair.second) = std::abs(current_v3SunPosition.x - pair.first) < epsilon;
	}

	if (current_v3SunPosition.x > 2.0f || current_v3SunPosition.x < -2.0f)
		current_v3SunPosition.x = 0.0f;
	if (current_v3SunPosition.y > 2.0f || current_v3SunPosition.y < -2.0f)
		current_v3SunPosition.y = 0.0f;
	if (current_v3SunPosition.z > 2.0f || current_v3SunPosition.z < -2.0f)
		current_v3SunPosition.z = 0.0f;

	Frustum::Instance().treeLightDirection = current_v3SunPosition;
	//TraceError("Water Opacity: %f", Frustum::Instance().waterOpacity);

}

int shadowstep = 10;
bool CMapOutdoor::BeginRenderCharacterShadowToTexture()
{
	time_t curr_time;
	curr_time = time(NULL);
	tm* tm_local = localtime(&curr_time);


	if (tm_local->tm_sec == 9 || tm_local->tm_sec == 29 || tm_local->tm_sec == 49)
	{
		shadowstep = 0;
	}

	D3DXMATRIX matLightView, matLightProj;

	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCurrentCamera)
		return false;

	if (recreate)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}

	if (!m_lpCharacterShadowMapRenderTargetSurface || !m_lpCharacterShadowMapDepthSurface)
		return false;

	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);
	D3DXVECTOR3 v3SunPosition = envData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction;
	v3SunPosition.z = v3SunPosition.z * 1.5f;

	//Frustum::Instance().currentEnvironmentFace = envData->strSkyBoxFaceFileName[0].c_str();

	if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_02") != 0
		&& std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_boss") != 0)
	{

		if ((Frustum::Instance().selectRainFrustun == 16 || Frustum::Instance().selectRainFrustun == 25 || Frustum::Instance().selectRainFrustun == 8) && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "map_n_snowm_01") != 0)
		{
			if (shadowstep >= 0 && shadowstep <= 2 || shadowstep == 8)
			{
				if (tm_local->tm_sec == 10 || tm_local->tm_sec == 30 || tm_local->tm_sec == 50)
				{
					shadowstep += 1;

					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<> distr(0.15, 0.2);

					float random_num;

					if (tm_local->tm_sec == 10)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}
					if (tm_local->tm_sec == 30)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}
					if (tm_local->tm_sec == 50)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}

					//z = 0.0072f;
				}
			}
			if (tm_local->tm_sec == 12 || tm_local->tm_sec == 32 || tm_local->tm_sec == 52)
			{
				z = 0.015f;
			}
		}

	}

	D3DXVECTOR3 v3Target = pCurrentCamera->GetTarget();
	if (a)
	{
		current_v3SunPosition = v3SunPosition;
		a = false;
	}

	if (current_v3SunPosition.x > 2.0f || current_v3SunPosition.x < -2.0f)
		current_v3SunPosition.x = 0.0f;
	if (current_v3SunPosition.y > 2.0f || current_v3SunPosition.y < -2.0f)
		current_v3SunPosition.y = 0.0f;
	if (current_v3SunPosition.z > 2.0f || current_v3SunPosition.z < -2.0f)
		current_v3SunPosition.z = 0.0f;


	const auto vv = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXMatrixLookAtRH(&matLightView,
		&shadow_eye,
		&v3Target,
		&vv);

	SnapShadowSpaceMatrix(matLightView, 10000.0f);
	D3DXMatrixOrthoRH(&matLightProj, 10000.0f, 10000.0f, 1.0f, 300000.0f);

	STATEMANAGER.SaveTransform(D3DTS_VIEW, &matLightView);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matLightProj);
	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(((v3Target)-current_v3SunPosition * (200000.0f)), matLightView, matLightProj);


	dwLightEnable = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	bool bSuccess = true;

	// Backup Device Context
	m_lpBackupRenderTargetSurface = nullptr;
	m_lpBackupDepthSurface = nullptr;

#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpBackupRenderTargetSurface)))
#else
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpBackupRenderTargetSurface)))
#endif
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Render Target\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpBackupDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Depth Surface\n");
		bSuccess = false;
	}

#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpCharacterShadowMapDepthSurface)))
	{
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(0, m_lpCharacterShadowMapRenderTargetSurface)))
	{
		if (m_lpBackupDepthSurface)
			ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
#else
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpCharacterShadowMapRenderTargetSurface, m_lpCharacterShadowMapDepthSurface)))
	{
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
#endif

	if (FAILED(ms_lpd3dDevice->GetViewport(&m_BackupViewport)))
	{
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetViewport(&m_ShadowMapViewport)))
	{
		bSuccess = false;
	}

	HRESULT hrClear = ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0);
	if (FAILED(hrClear))
	{
		ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0);
	}

	if (FAILED(ms_lpd3dDevice->SetVertexShader(m_lpShadowMapVertexShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass vertex shader\n");
		bSuccess = false;
	}
	if (FAILED(STATEMANAGER.ForcePixelShader(m_lpShadowMapPixelShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass pixel shader\n");
		bSuccess = false;
	}

	D3DXMatrixMultiplyTranspose(&m_MatLightViewProj, &matLightView, &matLightProj);
	ms_lpd3dDevice->SetVertexShaderConstantF(4, (float*)&m_MatLightViewProj, 4);

	return bSuccess;
}

void CMapOutdoor::EndRenderCharacterShadowToTexture()
{
	if (!m_lpBackupRenderTargetSurface && !m_lpBackupDepthSurface)
		return;

	ms_lpd3dDevice->SetViewport(&m_BackupViewport);

#ifdef DIRECTX9
	if (m_lpBackupDepthSurface)
		ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
	if (m_lpBackupRenderTargetSurface)
		ms_lpd3dDevice->SetRenderTarget(0, m_lpBackupRenderTargetSurface);
#else
	if (m_lpBackupRenderTargetSurface && m_lpBackupDepthSurface)
		ms_lpd3dDevice->SetRenderTarget(m_lpBackupRenderTargetSurface, m_lpBackupDepthSurface);
#endif

	SAFE_RELEASE(m_lpBackupRenderTargetSurface);
	SAFE_RELEASE(m_lpBackupDepthSurface);

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	// Restore Device Context
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	ms_lpd3dDevice->SetVertexShader(nullptr);
	STATEMANAGER.ForcePixelShader(nullptr);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);
}

bool CMapOutdoor::BeginRenderCharacterShadowToTexture2()
{
	time_t curr_time;
	curr_time = time(NULL);
	tm* tm_local = localtime(&curr_time);


	if (tm_local->tm_sec == 9 || tm_local->tm_sec == 29 || tm_local->tm_sec == 49)
	{
		shadowstep = 0;
	}

	D3DXMATRIX matLightView, matLightProj;

	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCurrentCamera)
		return false;

	if (recreate)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}

	if (!m_lpCharacterShadowMapRenderTargetSurface2 || !m_lpCharacterShadowMapDepthSurface)
		return false;

	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);

	D3DXVECTOR3 v3SunPosition = envData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction;
	v3SunPosition.z = v3SunPosition.z * 1.5f;

	D3DXVECTOR3 v3Target = pCurrentCamera->GetTarget();
	if (a)
	{
		current_v3SunPosition = v3SunPosition;
		a = false;
	}


	if (current_v3SunPosition.x > 2.0f || current_v3SunPosition.x < -2.0f)
		current_v3SunPosition.x = 0.0f;
	if (current_v3SunPosition.y > 2.0f || current_v3SunPosition.y < -2.0f)
		current_v3SunPosition.y = 0.0f;
	if (current_v3SunPosition.z > 2.0f || current_v3SunPosition.z < -2.0f)
		current_v3SunPosition.z = 0.0f;





	float distanceX = v3SunPosition.x - current_v3SunPosition.x;
	float distanceY = v3SunPosition.y - current_v3SunPosition.y;
	float distanceZ = v3SunPosition.z - current_v3SunPosition.z;

	if (std::abs(distanceX) > tolerance)
	{
		float stepX = min(std::abs(distanceX), z);
		current_v3SunPosition.x += stepX * (distanceX > 0 ? 1 : -1);
	}

	if (std::abs(distanceY) > tolerance)
	{
		float stepY = min(std::abs(distanceY), z);
		current_v3SunPosition.y += stepY * (distanceY > 0 ? 1 : -1);
	}

	if (std::abs(distanceZ) > tolerance)
	{
		float stepZ = min(std::abs(distanceZ), z * zMultiplier);
		current_v3SunPosition.z += stepZ * (distanceZ > 0 ? 1 : -1);
	}

	if (std::abs(current_v3SunPosition.x - v3SunPosition.x) <= tolerance)
		current_v3SunPosition.x = v3SunPosition.x;
	if (std::abs(current_v3SunPosition.y - v3SunPosition.y) <= tolerance)
		current_v3SunPosition.y = v3SunPosition.y;
	if (std::abs(current_v3SunPosition.z - v3SunPosition.z) <= tolerance)
		current_v3SunPosition.z = v3SunPosition.z;

	if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_02") != 0
		&& std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_boss") != 0)
	{

		if ((Frustum::Instance().selectRainFrustun == 16 || Frustum::Instance().selectRainFrustun == 25 || Frustum::Instance().selectRainFrustun == 8) && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "map_n_snowm_01") != 0)
		{
			if (shadowstep >= 0 && shadowstep <= 2 || shadowstep == 8)
			{
				if (tm_local->tm_sec == 10 || tm_local->tm_sec == 30 || tm_local->tm_sec == 50)
				{
					shadowstep += 1;
					if (tm_local->tm_sec == 10)
					{
						current_v3SunPosition.z = current_v3SunPosition.z - 0.065f;
						current_v3SunPosition.x = current_v3SunPosition.x - 0.065f;
					}
					if (tm_local->tm_sec == 30)
					{
						current_v3SunPosition.z = current_v3SunPosition.z - 0.065f;
						current_v3SunPosition.x = current_v3SunPosition.x - 0.065f;
					}
					if (tm_local->tm_sec == 50)
					{
						current_v3SunPosition.z = current_v3SunPosition.z - 0.065f;
						current_v3SunPosition.x = current_v3SunPosition.x - 0.065f;
					}
					z = 0.1f;
				}
			}
			if (tm_local->tm_sec == 12 || tm_local->tm_sec == 32 || tm_local->tm_sec == 52)
			{
				z = 0.015f;
			}
		}

	}

#include <unordered_map>

	const auto vv = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	const auto shadow_eye = (v3Target)-current_v3SunPosition * (200000.0f);
	D3DXMatrixLookAtRH(&m_matLightView2,
		&shadow_eye,
		&v3Target,
		&vv);

	//TraceError("2 %f, %f, %f, ", current_v3SunPosition.x, current_v3SunPosition.y, current_v3SunPosition.z);
	SnapShadowSpaceMatrix(matLightView, 25000.0f);
	D3DXMatrixOrthoRH(&matLightProj, 25000.0f, 25000.0f, 1.0f, 300000.0f);

	STATEMANAGER.SaveTransform(D3DTS_VIEW, &m_matLightView2);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matLightProj);


	dwLightEnable = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	bool bSuccess = true;

	// Backup Device Context
	m_lpBackupRenderTargetSurface = nullptr;
	m_lpBackupDepthSurface = nullptr;

#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpBackupRenderTargetSurface)))
#else
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpBackupRenderTargetSurface)))
#endif
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Render Target\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpBackupDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Depth Surface\n");
		bSuccess = false;
	}

#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpCharacterShadowMapDepthSurface)))
	{
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(0, m_lpCharacterShadowMapRenderTargetSurface2)))
	{
		if (m_lpBackupDepthSurface)
			ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
#else
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpCharacterShadowMapRenderTargetSurface, m_lpCharacterShadowMapDepthSurface)))
	{
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
#endif

	if (FAILED(ms_lpd3dDevice->GetViewport(&m_BackupViewport)))
	{
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetViewport(&m_ShadowMapViewport)))
	{
		bSuccess = false;
	}

	HRESULT hrClear = ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0);
	if (FAILED(hrClear))
	{
		ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0);
	}

	return bSuccess;
}

void CMapOutdoor::EndRenderCharacterShadowToTexture2()
{
	if (!m_lpBackupRenderTargetSurface && !m_lpBackupDepthSurface)
		return;

	ms_lpd3dDevice->SetViewport(&m_BackupViewport);

#ifdef DIRECTX9
	if (m_lpBackupDepthSurface)
		ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
	if (m_lpBackupRenderTargetSurface)
		ms_lpd3dDevice->SetRenderTarget(0, m_lpBackupRenderTargetSurface);
#else
	if (m_lpBackupRenderTargetSurface && m_lpBackupDepthSurface)
		ms_lpd3dDevice->SetRenderTarget(m_lpBackupRenderTargetSurface, m_lpBackupDepthSurface);
#endif

	SAFE_RELEASE(m_lpBackupRenderTargetSurface);
	SAFE_RELEASE(m_lpBackupDepthSurface);

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	// Restore Device Context
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	ms_lpd3dDevice->SetVertexShader(nullptr);
	STATEMANAGER.ForcePixelShader(nullptr);
}


float currentValue = 500.0f; // Starting value
float targetValue = 500.0f; // Target value
float stepSize = 10.0f; // Step size for each update, will be recalculated
float animationDuration = 2.0f; // Duration of the animation in seconds
float elapsedTime = 0.0f; // Time elapsed since the start of the animation

bool CMapOutdoor::BeginRenderCharacterShadowToTexture3()
{
	int cur_sec = (int)(time(NULL) % 60);
	if (cur_sec == 9 || cur_sec == 29 || cur_sec == 49)
	{
		shadowstep = 0;
	}

	D3DXMATRIX matLightView, matLightProj;

	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCurrentCamera)
		return false;

	if (recreate)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}

	if (!m_lpCharacterShadowMapRenderTargetSurface || !m_lpCharacterShadowMapDepthSurface)
		return false;

	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);
	D3DXVECTOR3 v3SunPosition = envData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction;
	v3SunPosition.z = v3SunPosition.z * 1.5f;

	if (v3SunPosition != Frustum::instance().previousSunPosition)
	{
		Frustum::instance().isChangingSky = true;
		Frustum::instance().previousSunPosition = v3SunPosition; // Update the previous value.
	}
	else
	{
		Frustum::instance().isChangingSky = false;
	}

	if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_02") != 0
		&& std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_boss") != 0)
	{

		if ((Frustum::Instance().selectRainFrustun == 16 || Frustum::Instance().selectRainFrustun == 25 || Frustum::Instance().selectRainFrustun == 8) && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "map_n_snowm_01") != 0)
		{
			if (shadowstep >= 0 && shadowstep <= 2 || shadowstep == 8)
			{
				if (cur_sec == 10 || cur_sec == 30 || cur_sec == 50)
				{
					shadowstep += 1;

					float random_num = 0.15f + ((float)(rand() % 50) / 1000.0f);

					current_v3SunPosition.z = current_v3SunPosition.z - random_num;
					current_v3SunPosition.x = current_v3SunPosition.x - random_num;
					current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
				}
			}
			if (cur_sec == 12 || cur_sec == 32 || cur_sec == 52)
			{
				z = 0.015f;
			}
		}

	}

	D3DXVECTOR3 v3Target = pCurrentCamera->GetTarget();
	if (a)
	{
		current_v3SunPosition = v3SunPosition;
		a = false;
	}


	float curLenSq = current_v3SunPosition.x * current_v3SunPosition.x + current_v3SunPosition.y * current_v3SunPosition.y + current_v3SunPosition.z * current_v3SunPosition.z;
	if (curLenSq < 0.001f || fabsf(current_v3SunPosition.x) > 2.5f || fabsf(current_v3SunPosition.y) > 2.5f || fabsf(current_v3SunPosition.z) > 2.5f)
	{
		current_v3SunPosition = v3SunPosition;
		curLenSq = current_v3SunPosition.x * current_v3SunPosition.x + current_v3SunPosition.y * current_v3SunPosition.y + current_v3SunPosition.z * current_v3SunPosition.z;
		if (curLenSq < 0.001f)
			current_v3SunPosition = D3DXVECTOR3(0.577f, -0.577f, 0.577f);
	}

	float distanceX = v3SunPosition.x - current_v3SunPosition.x;
	float distanceY = v3SunPosition.y - current_v3SunPosition.y;
	float distanceZ = v3SunPosition.z - current_v3SunPosition.z;

	if (std::abs(distanceX) > tolerance)
	{
		float stepX = min(std::abs(distanceX), z);
		current_v3SunPosition.x += stepX * (distanceX > 0 ? 1 : -1);
	}

	if (std::abs(distanceY) > tolerance)
	{
		float stepY = min(std::abs(distanceY), z);
		current_v3SunPosition.y += stepY * (distanceY > 0 ? 1 : -1);
	}

	if (std::abs(distanceZ) > tolerance)
	{
		float stepZ = min(std::abs(distanceZ), z * zMultiplier);
		current_v3SunPosition.z += stepZ * (distanceZ > 0 ? 1 : -1);
	}

	if (std::abs(current_v3SunPosition.x - v3SunPosition.x) <= tolerance)
		current_v3SunPosition.x = v3SunPosition.x;
	if (std::abs(current_v3SunPosition.y - v3SunPosition.y) <= tolerance)
		current_v3SunPosition.y = v3SunPosition.y;
	if (std::abs(current_v3SunPosition.z - v3SunPosition.z) <= tolerance)
		current_v3SunPosition.z = v3SunPosition.z;

	D3DXVECTOR3 safeSun = current_v3SunPosition;
	if (fabsf(safeSun.x) < 0.0001f && fabsf(safeSun.y) < 0.0001f)
	{
		safeSun.x = 0.01f;
		safeSun.y = 0.01f;
	}
	D3DXVec3Normalize(&safeSun, &safeSun);

	D3DXVECTOR3 vv = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	if (fabsf(safeSun.x) < 0.01f && fabsf(safeSun.y) < 0.01f)
		vv = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	shadow_eye = v3Target - safeSun * 200000.0f;
	D3DXMatrixLookAtRH(&matLightView,
		&shadow_eye,
		&v3Target,
		&vv);

	float newTargetValue;
	if (Frustum::Instance().shadowType == 2) {
		newTargetValue = 35000.0f;
	}
	else {
		newTargetValue = 5000.0f;
	}

	// If the target value has changed, reset the animation
	if (newTargetValue != targetValue) {
		targetValue = newTargetValue;
		elapsedTime = 0.0f;
		float totalDistance = std::abs(targetValue - currentValue);
		stepSize = totalDistance / (animationDuration * 60); // assuming 60 updates per second
	}

	// Update currentValue towards targetValue
	if (elapsedTime < animationDuration) {
		float distance = targetValue - currentValue;
		if (std::abs(distance) > 0) {
			float step = stepSize * 0.03f * 60; // Correct step for deltaTime
			currentValue += step * (distance > 0 ? 1 : -1);
			elapsedTime += 0.03f;
		}
	}
	else {
		currentValue = targetValue; // Snap to target if duration has elapsed
	}

	// Use currentValue in your functions
	SnapShadowSpaceMatrix(matLightView, currentValue);
	D3DXMatrixOrthoRH(&matLightProj, currentValue, currentValue, 1.0f, 300000.0f);

	STATEMANAGER.SaveTransform(D3DTS_VIEW, &matLightView);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matLightProj);
	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(((v3Target)-current_v3SunPosition * (200000.0f)), matLightView, matLightProj);


	dwLightEnable = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	bool bSuccess = true;

	// Backup Device Context
	m_lpBackupRenderTargetSurface = nullptr;
	m_lpBackupDepthSurface = nullptr;

#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpBackupRenderTargetSurface)))
#else
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpBackupRenderTargetSurface)))
#endif
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Render Target\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpBackupDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Depth Surface\n");
		bSuccess = false;
	}

#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpCharacterShadowMapDepthSurface)))
	{
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(0, m_lpCharacterShadowMapRenderTargetSurface)))
	{
		if (m_lpBackupDepthSurface)
			ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
#else
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpCharacterShadowMapRenderTargetSurface, m_lpCharacterShadowMapDepthSurface)))
	{
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
#endif

	if (FAILED(ms_lpd3dDevice->GetViewport(&m_BackupViewport)))
	{
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetViewport(&m_ShadowMapViewportTree)))
	{
		bSuccess = false;
	}

	HRESULT hrClear = ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0);
	if (FAILED(hrClear))
	{
		ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0);
	}

	if (FAILED(ms_lpd3dDevice->SetVertexShader(m_lpShadowMapVertexShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass vertex shader\n");
		bSuccess = false;
	}
	if (FAILED(STATEMANAGER.ForcePixelShader(m_lpShadowMapPixelShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass pixel shader\n");
		bSuccess = false;
	}

	D3DXMatrixMultiplyTranspose(&m_MatLightViewProj, &matLightView, &matLightProj);
	ms_lpd3dDevice->SetVertexShaderConstantF(4, (float*)&m_MatLightViewProj, 4);

	return bSuccess;
}

void CMapOutdoor::EndRenderCharacterShadowToTexture3()
{
	if (!m_lpBackupRenderTargetSurface && !m_lpBackupDepthSurface)
		return;

	ms_lpd3dDevice->SetViewport(&m_BackupViewport);

#ifdef DIRECTX9
	if (m_lpBackupDepthSurface)
		ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
	if (m_lpBackupRenderTargetSurface)
		ms_lpd3dDevice->SetRenderTarget(0, m_lpBackupRenderTargetSurface);
#else
	if (m_lpBackupRenderTargetSurface && m_lpBackupDepthSurface)
		ms_lpd3dDevice->SetRenderTarget(m_lpBackupRenderTargetSurface, m_lpBackupDepthSurface);
#endif

	SAFE_RELEASE(m_lpBackupRenderTargetSurface);
	SAFE_RELEASE(m_lpBackupDepthSurface);

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	// Restore Device Context
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);
	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	ms_lpd3dDevice->SetVertexShader(nullptr);
	STATEMANAGER.ForcePixelShader(nullptr);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);
}

void CMapOutdoor::SnapShadowSpaceMatrix(D3DXMATRIX& mat, float spaceSize)
{
	float step = spaceSize / 128.0;
	mat._41 = floor(mat._41 / step) * step;
	mat._42 = floor(mat._42 / step) * step;
	mat._43 = floor(mat._43 / step) * step;
}

bool CMapOutdoor::BeginRenderCharacterShadowToTexture4()
{
	D3DXMATRIX matLightView, matLightProj;

	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCurrentCamera)
		return false;

	if (recreate)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}

	if (!m_lpCharacterShadowMapRenderTargetSurface4 || !m_lpCharacterShadowMapDepthSurface)
		return false;

	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);
	D3DXVECTOR3 v3Target = pCurrentCamera->GetTarget();



	D3DXVECTOR3 sunDir = current_v3SunPosition;
	if (fabsf(sunDir.x) < 0.0001f && fabsf(sunDir.y) < 0.0001f)
	{
		sunDir.x = 0.01f;
		sunDir.y = 0.01f;
	}
	D3DXVec3Normalize(&sunDir, &sunDir);

	D3DXVECTOR3 shadow_eye_local = v3Target - sunDir * 200000.0f;
	D3DXVECTOR3 vv(0.0f, 0.0f, 1.0f);
	if (fabsf(sunDir.x) < 0.01f && fabsf(sunDir.y) < 0.01f)
		vv = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtRH(&matLightView,
		&shadow_eye_local,
		&v3Target,
		&vv);


	SnapShadowSpaceMatrix(matLightView, 3000.0f);
	D3DXMatrixOrthoRH(&matLightProj, 3000.0f, 3000.0f, 1.0f, 300000.0f);



	STATEMANAGER.SaveTransform(D3DTS_VIEW, &matLightView);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matLightProj);
	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix2(((v3Target)-current_v3SunPosition * (200000.0f)), matLightView, matLightProj);

	dwLightEnable = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	bool bSuccess = true;

	// Backup Device Context
	m_lpBackupRenderTargetSurface = nullptr;
	m_lpBackupDepthSurface = nullptr;

#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpBackupRenderTargetSurface)))
#else
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpBackupRenderTargetSurface)))
#endif
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Render Target\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpBackupDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Depth Surface\n");
		bSuccess = false;
	}

#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpCharacterShadowMapDepthSurface)))
	{
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(0, m_lpCharacterShadowMapRenderTargetSurface4)))
	{
		if (m_lpBackupDepthSurface)
			ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
#else
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpCharacterShadowMapRenderTargetSurface, m_lpCharacterShadowMapDepthSurface)))
	{
		SAFE_RELEASE(m_lpBackupRenderTargetSurface);
		SAFE_RELEASE(m_lpBackupDepthSurface);
		return false;
	}
#endif

	if (FAILED(ms_lpd3dDevice->GetViewport(&m_BackupViewport)))
	{
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetViewport(&m_ShadowMapViewport)))
	{
		bSuccess = false;
	}

	HRESULT hrClear = ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0);
	if (FAILED(hrClear))
	{
		ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0);
	}

	if (FAILED(ms_lpd3dDevice->SetVertexShader(m_lpShadowMapVertexShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass vertex shader\n");
		bSuccess = false;
	}
	if (FAILED(STATEMANAGER.ForcePixelShader(m_lpShadowMapPixelShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass pixel shader\n");
		bSuccess = false;
	}

	D3DXMatrixMultiplyTranspose(&m_MatLightViewProj2, &matLightView, &matLightProj);
	ms_lpd3dDevice->SetVertexShaderConstantF(4, (float*)&m_MatLightViewProj2, 4);

	return bSuccess;
}

void CMapOutdoor::EndRenderCharacterShadowToTexture4()
{
	if (!m_lpBackupRenderTargetSurface && !m_lpBackupDepthSurface)
		return;

	ms_lpd3dDevice->SetViewport(&m_BackupViewport);

#ifdef DIRECTX9
	if (m_lpBackupDepthSurface)
		ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
	if (m_lpBackupRenderTargetSurface)
		ms_lpd3dDevice->SetRenderTarget(0, m_lpBackupRenderTargetSurface);
#else
	if (m_lpBackupRenderTargetSurface && m_lpBackupDepthSurface)
		ms_lpd3dDevice->SetRenderTarget(m_lpBackupRenderTargetSurface, m_lpBackupDepthSurface);
#endif

	SAFE_RELEASE(m_lpBackupRenderTargetSurface);
	SAFE_RELEASE(m_lpBackupDepthSurface);

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	// Restore Device Context
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	ms_lpd3dDevice->SetVertexShader(nullptr);
	STATEMANAGER.ForcePixelShader(nullptr);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);
}
