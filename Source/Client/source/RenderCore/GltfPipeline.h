#pragma once

#include "D3D12Common.h"
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <meshoptimizer.h>

namespace RenderCore
{
	struct GltfVertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT4 tangent;
		XMFLOAT2 uv;
		uint32_t boneIndices[4];
		float boneWeights[4];
	};

	struct GltfSubmesh
	{
		std::string name;
		std::vector<GltfVertex> vertices;
		std::vector<uint32_t> indices;

		// LOD levels generated via meshoptimizer
		std::vector<std::vector<uint32_t>> lodIndices;

		uint32_t materialIndex = 0;
		bool isTwoSided = false;
		bool hasDiffuseTransmission = false;
		float transmissionFactor = 0.0f;
	};

	struct GltfModel
	{
		std::string filePath;
		std::vector<GltfSubmesh> submeshes;
		XMFLOAT3 boundingBoxMin = XMFLOAT3(1e9f, 1e9f, 1e9f);
		XMFLOAT3 boundingBoxMax = XMFLOAT3(-1e9f, -1e9f, -1e9f);
		float boundingSphereRadius = 0.0f;
	};

	class CGltfPipeline
	{
	public:
		static CGltfPipeline& Instance();

		bool LoadModel(const std::string& filePath, GltfModel& outModel);
		void OptimizeMesh(GltfSubmesh& submesh, float lodSimplificationFactor = 0.5f);

	private:
		CGltfPipeline() = default;
		~CGltfPipeline() = default;
	};
}
