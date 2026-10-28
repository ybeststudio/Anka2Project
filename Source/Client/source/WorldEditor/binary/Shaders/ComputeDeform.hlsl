// Metin2 Modernized Graphics Engine - RC::Deform
// GPU Compute Skinning and Sousa Vegetation Wind Sway
// HLSL 2021 / Shader Model 6.6 Bindless

struct SkinVertex
{
    float3 position;
    float3 normal;
    float4 tangent;
    float2 uv;
    uint4 boneIndices;
    float4 boneWeights;
};

struct DeformedVertex
{
    float3 position;
    float3 normal;
    float4 tangent;
    float2 uv;
};

struct SkinningParams
{
    uint vertexCount;
    uint inputVertexSRV;
    uint boneMatrixSRV;
    uint outputVertexUAV;
};

ConstantBuffer<SkinningParams> g_SkinParams : register(b3);

struct VegetationParams
{
    uint vertexCount;
    uint inputVertexSRV;
    uint outputVertexUAV;
    float time;

    float3 windDirection;
    float windSpeed;

    float windStrength;
    float branchAmplitude;
    float leafFrequency;
    uint pad;
};

ConstantBuffer<VegetationParams> g_VegParams : register(b3);

// --------------------------------------------------------------------------------------
// 1. Character & Monster Linear Blend Skinning Compute Shader
// --------------------------------------------------------------------------------------
[numthreads(64, 1, 1)]
void SkinningCS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint idx = dispatchThreadId.x;
    if (idx >= g_SkinParams.vertexCount)
        return;

    StructuredBuffer<SkinVertex> inVertices = ResourceDescriptorHeap[g_SkinParams.inputVertexSRV];
    StructuredBuffer<float4x4> boneMatrices = ResourceDescriptorHeap[g_SkinParams.boneMatrixSRV];
    RWStructuredBuffer<DeformedVertex> outVertices = ResourceDescriptorHeap[g_SkinParams.outputVertexUAV];

    SkinVertex v = inVertices[idx];

    // Compute blended bone matrix
    float4x4 skinMat = boneMatrices[v.boneIndices.x] * v.boneWeights.x +
                       boneMatrices[v.boneIndices.y] * v.boneWeights.y +
                       boneMatrices[v.boneIndices.z] * v.boneWeights.z +
                       boneMatrices[v.boneIndices.w] * v.boneWeights.w;

    DeformedVertex outV;
    outV.position = mul(float4(v.position, 1.0f), skinMat).xyz;
    outV.normal = normalize(mul(float4(v.normal, 0.0f), skinMat).xyz);
    outV.tangent = float4(normalize(mul(float4(v.tangent.xyz, 0.0f), skinMat).xyz), v.tangent.w);
    outV.uv = v.uv;

    outVertices[idx] = outV;
}

// --------------------------------------------------------------------------------------
// 2. Tiago Sousa Vegetation Wind Sway Compute Shader
// --------------------------------------------------------------------------------------
float SmoothCurve(float x)
{
    return x * x * (3.0f - 2.0f * x);
}

float TriangleWave(float x)
{
    return abs(frac(x + 0.5f) * 2.0f - 1.0f);
}

float SmoothTriangleWave(float x)
{
    return SmoothCurve(TriangleWave(x));
}

[numthreads(64, 1, 1)]
void VegetationSwayCS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint idx = dispatchThreadId.x;
    if (idx >= g_VegParams.vertexCount)
        return;

    StructuredBuffer<DeformedVertex> inVertices = ResourceDescriptorHeap[g_VegParams.inputVertexSRV];
    RWStructuredBuffer<DeformedVertex> outVertices = ResourceDescriptorHeap[g_VegParams.outputVertexUAV];

    DeformedVertex v = inVertices[idx];

    float3 worldPos = v.position;
    float3 windDir = normalize(g_VegParams.windDirection);
    float t = g_VegParams.time * g_VegParams.windSpeed;

    // Phase shift based on spatial coordinates
    float phase = dot(worldPos.xz, float2(0.1f, 0.1f));

    // 1. Main trunk bending (frequency 0.5 - 1.0 Hz, proportional to height)
    float trunkWave = sin(t * 0.7f + phase) * g_VegParams.windStrength;
    float heightFactor = saturate(v.position.y * 0.05f); // Assume base at y=0
    float3 trunkDisp = windDir * (trunkWave * heightFactor * heightFactor * 1.5f);

    // 2. Branch sway (frequency 1.5 - 3.0 Hz)
    float branchWave = SmoothTriangleWave(t * 1.8f + phase * 2.0f) * g_VegParams.branchAmplitude;
    float3 branchDisp = cross(windDir, float3(0.0f, 1.0f, 0.0f)) * (branchWave * heightFactor);

    // 3. Leaf flutter (high frequency oscillation)
    float leafWave = sin(t * g_VegParams.leafFrequency + dot(worldPos, float3(1.0f, 1.0f, 1.0f))) * 0.05f;
    float3 leafDisp = v.normal * leafWave;

    v.position += trunkDisp + branchDisp + leafDisp;

    outVertices[idx] = v;
}
