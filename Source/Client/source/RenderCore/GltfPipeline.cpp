#include "GltfPipeline.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace RenderCore
{
	CGltfPipeline& CGltfPipeline::Instance()
	{
		static CGltfPipeline instance;
		return instance;
	}

	bool CGltfPipeline::LoadModel(const std::string& filePath, GltfModel& outModel)
	{
		std::filesystem::path path(filePath);
		if (!std::filesystem::exists(path))
		{
			return false;
		}

		outModel.filePath = filePath;
		outModel.submeshes.clear();

		fastgltf::Parser parser(fastgltf::Extensions::KHR_materials_transmission);
		auto data = fastgltf::GltfDataBuffer::FromPath(path);
		if (data.error() != fastgltf::Error::None)
		{
			return false;
		}

		fastgltf::Options options = fastgltf::Options::GenerateMeshIndices |
									fastgltf::Options::DecomposeNodeMatrices;

		auto asset = parser.loadGltf(data.get(), path.parent_path(), options);
		if (asset.error() != fastgltf::Error::None)
		{
			return false;
		}

		fastgltf::Asset& gltfAsset = asset.get();

		// Process meshes and primitives
		for (const auto& mesh : gltfAsset.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				GltfSubmesh submesh;
				submesh.name = mesh.name;

				// Material properties
				if (primitive.materialIndex.has_value())
				{
					submesh.materialIndex = static_cast<uint32_t>(primitive.materialIndex.value());
					const auto& material = gltfAsset.materials[submesh.materialIndex];
					submesh.isTwoSided = material.doubleSided;
				}

				// Find Position accessor
				auto* posIt = primitive.findAttribute("POSITION");
				if (posIt == primitive.attributes.end())
					continue;

				const auto& posAccessor = gltfAsset.accessors[posIt->accessorIndex];
				size_t vertexCount = posAccessor.count;
				submesh.vertices.resize(vertexCount);

				// Read Positions
				std::vector<fastgltf::math::fvec3> positions(vertexCount);
				fastgltf::copyFromAccessor<fastgltf::math::fvec3>(gltfAsset, posAccessor, positions.data());
				for (size_t idx = 0; idx < vertexCount; ++idx)
				{
					submesh.vertices[idx].position = XMFLOAT3(positions[idx].x(), positions[idx].y(), positions[idx].z());
					outModel.boundingBoxMin.x = std::min(outModel.boundingBoxMin.x, positions[idx].x());
					outModel.boundingBoxMin.y = std::min(outModel.boundingBoxMin.y, positions[idx].y());
					outModel.boundingBoxMin.z = std::min(outModel.boundingBoxMin.z, positions[idx].z());
					outModel.boundingBoxMax.x = std::max(outModel.boundingBoxMax.x, positions[idx].x());
					outModel.boundingBoxMax.y = std::max(outModel.boundingBoxMax.y, positions[idx].y());
					outModel.boundingBoxMax.z = std::max(outModel.boundingBoxMax.z, positions[idx].z());
				}

				// Read Normals
				auto* normIt = primitive.findAttribute("NORMAL");
				if (normIt != primitive.attributes.end())
				{
					const auto& normAccessor = gltfAsset.accessors[normIt->accessorIndex];
					std::vector<fastgltf::math::fvec3> normals(normAccessor.count);
					fastgltf::copyFromAccessor<fastgltf::math::fvec3>(gltfAsset, normAccessor, normals.data());
					for (size_t idx = 0; idx < normals.size(); ++idx)
						submesh.vertices[idx].normal = XMFLOAT3(normals[idx].x(), normals[idx].y(), normals[idx].z());
				}

				// Read Texcoords
				auto* texIt = primitive.findAttribute("TEXCOORD_0");
				if (texIt != primitive.attributes.end())
				{
					const auto& texAccessor = gltfAsset.accessors[texIt->accessorIndex];
					std::vector<fastgltf::math::fvec2> uvs(texAccessor.count);
					fastgltf::copyFromAccessor<fastgltf::math::fvec2>(gltfAsset, texAccessor, uvs.data());
					for (size_t idx = 0; idx < uvs.size(); ++idx)
						submesh.vertices[idx].uv = XMFLOAT2(uvs[idx].x(), uvs[idx].y());
				}

				// Read Tangents
				auto* tanIt = primitive.findAttribute("TANGENT");
				if (tanIt != primitive.attributes.end())
				{
					const auto& tanAccessor = gltfAsset.accessors[tanIt->accessorIndex];
					std::vector<fastgltf::math::fvec4> tangents(tanAccessor.count);
					fastgltf::copyFromAccessor<fastgltf::math::fvec4>(gltfAsset, tanAccessor, tangents.data());
					for (size_t idx = 0; idx < tangents.size(); ++idx)
						submesh.vertices[idx].tangent = XMFLOAT4(tangents[idx].x(), tangents[idx].y(), tangents[idx].z(), tangents[idx].w());
				}

				// Read Indices
				if (primitive.indicesAccessor.has_value())
				{
					size_t accIdx = *primitive.indicesAccessor;
					const auto& idxAccessor = gltfAsset.accessors[accIdx];
					submesh.indices.resize(idxAccessor.count);
					fastgltf::copyFromAccessor<std::uint32_t>(gltfAsset, idxAccessor, submesh.indices.data());
				}

				// Optimize mesh with meshoptimizer
				OptimizeMesh(submesh);

				outModel.submeshes.push_back(std::move(submesh));
			}
		}

		// Calculate model bounding sphere
		XMFLOAT3 center = XMFLOAT3(
			(outModel.boundingBoxMin.x + outModel.boundingBoxMax.x) * 0.5f,
			(outModel.boundingBoxMin.y + outModel.boundingBoxMax.y) * 0.5f,
			(outModel.boundingBoxMin.z + outModel.boundingBoxMax.z) * 0.5f);

		float maxDistSq = 0.0f;
		for (const auto& sub : outModel.submeshes)
		{
			for (const auto& v : sub.vertices)
			{
				float dx = v.position.x - center.x;
				float dy = v.position.y - center.y;
				float dz = v.position.z - center.z;
				float distSq = dx * dx + dy * dy + dz * dz;
				maxDistSq = std::max(maxDistSq, distSq);
			}
		}
		outModel.boundingSphereRadius = sqrtf(maxDistSq);

		return true;
	}

	void CGltfPipeline::OptimizeMesh(GltfSubmesh& submesh, float lodSimplificationFactor)
	{
		if (submesh.indices.empty() || submesh.vertices.empty())
			return;

		size_t vertexCount = submesh.vertices.size();
		size_t indexCount = submesh.indices.size();

		// 1. Vertex cache optimization
		meshopt_optimizeVertexCache(submesh.indices.data(), submesh.indices.data(), indexCount, vertexCount);

		// 2. Overdraw optimization
		meshopt_optimizeOverdraw(submesh.indices.data(), submesh.indices.data(), indexCount,
			&submesh.vertices[0].position.x, vertexCount, sizeof(GltfVertex), 1.05f);

		// 3. Vertex fetch optimization
		std::vector<GltfVertex> remapVertices(vertexCount);
		meshopt_optimizeVertexFetch(remapVertices.data(), submesh.indices.data(), indexCount,
			submesh.vertices.data(), vertexCount, sizeof(GltfVertex));
		submesh.vertices = std::move(remapVertices);

		// 4. Generate LODs using meshoptimizer simplifier
		submesh.lodIndices.clear();

		// LOD 1: 50% triangles
		size_t targetIndexCountLOD1 = static_cast<size_t>(indexCount * lodSimplificationFactor);
		std::vector<uint32_t> lod1(indexCount);
		float resultError1 = 0.0f;
		size_t lod1Count = meshopt_simplify(lod1.data(), submesh.indices.data(), indexCount,
			&submesh.vertices[0].position.x, vertexCount, sizeof(GltfVertex), targetIndexCountLOD1, 0.02f, 0, &resultError1);
		lod1.resize(lod1Count);
		submesh.lodIndices.push_back(std::move(lod1));

		// LOD 2: 25% triangles
		size_t targetIndexCountLOD2 = static_cast<size_t>(indexCount * (lodSimplificationFactor * 0.5f));
		std::vector<uint32_t> lod2(indexCount);
		float resultError2 = 0.0f;
		size_t lod2Count = meshopt_simplify(lod2.data(), submesh.indices.data(), indexCount,
			&submesh.vertices[0].position.x, vertexCount, sizeof(GltfVertex), targetIndexCountLOD2, 0.05f, 0, &resultError2);
		lod2.resize(lod2Count);
		submesh.lodIndices.push_back(std::move(lod2));
	}
}
