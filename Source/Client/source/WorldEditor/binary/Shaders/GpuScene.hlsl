// Metin2 Modernized Graphics Engine - RC::GpuScene
// GPU-Driven Scene Culling, Hi-Z Depth Pyramid & Indirect Draw Generation
// HLSL 2021 / Shader Model 6.6 Bindless

struct GpuInstanceData
{
    float4x4 worldMatrix;
    float4 boundingSphere; // xyz = center, w = radius
    uint materialIndex;
    uint meshIndex;
    uint lodLevel;
    uint flags;
};

struct IndirectDrawIndexedCommand
{
    uint objectIndex;
    uint indexCountPerInstance;
    uint instanceCount;
    uint startIndexLocation;
    int baseVertexLocation;
    uint startInstanceLocation;
};

struct GpuSceneParams
{
    uint totalInstanceCount;
    uint totalTerrainChunks;
    uint instanceBufferSRV;
    uint indirectArgsUAV;

    uint hizDepthSRV;
    uint hizWidth;
    uint hizHeight;
    uint hizMipCount;

    float3 cameraPos;
    float cameraFov;

    uint isPrepass;
    uint drawCountUAV;
    uint sourceMipIndex;
    uint targetMipUAV;
};

ConstantBuffer<GpuSceneParams> g_SceneParams : register(b3);

struct FrameGlobals
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 viewProjMatrix;
    float4x4 invViewMatrix;
    float4x4 invProjMatrix;
    float4x4 invViewProjMatrix;

    float3 cameraPosition;
    float nearPlane;

    float3 cameraDirection;
    float farPlane;

    float2 renderResolution;
    float2 invRenderResolution;

    float time;
    float deltaTime;
    uint frameIndex;
    uint sceneColorSRV;

    uint sceneDepthSRV;
    uint fogAccumulatedSRV;
    uint tlasIndex;
    uint padding;
};

ConstantBuffer<FrameGlobals> g_FrameGlobals : register(b1);

// --------------------------------------------------------------------------------------
// 1. Hi-Z Depth Pyramid Downsampling Compute Shader (Reverse-Z: Minimum depth is closest)
// --------------------------------------------------------------------------------------
[numthreads(8, 8, 1)]
void GenerateHiZMipCS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (dispatchThreadId.x >= g_SceneParams.hizWidth || dispatchThreadId.y >= g_SceneParams.hizHeight)
        return;

    Texture2D<float> srcTexture = ResourceDescriptorHeap[g_SceneParams.sourceMipIndex];
    RWTexture2D<float> dstTexture = ResourceDescriptorHeap[g_SceneParams.targetMipUAV];

    uint2 srcCoords = dispatchThreadId.xy * 2;

    float d0 = srcTexture[srcCoords + uint2(0, 0)];
    float d1 = srcTexture[srcCoords + uint2(1, 0)];
    float d2 = srcTexture[srcCoords + uint2(0, 1)];
    float d3 = srcTexture[srcCoords + uint2(1, 1)];

    // Reverse-Z: smaller values are farther away, larger values are closer to camera!
    // For conservative occlusion, we want the furthest depth in the 2x2 footprint, so min()
    float minDepth = min(min(d0, d1), min(d2, d3));

    dstTexture[dispatchThreadId.xy] = minDepth;
}

// --------------------------------------------------------------------------------------
// 2. Frustum & Hi-Z Occlusion Culling Compute Shader
// --------------------------------------------------------------------------------------
bool TestSphereFrustum(float3 center, float radius, float4x4 vp)
{
    // Extract planes from VP matrix
    float4 planes[6];
    planes[0] = vp._14_24_34_44 + vp._11_21_31_41; // Left
    planes[1] = vp._14_24_34_44 - vp._11_21_31_41; // Right
    planes[2] = vp._14_24_34_44 + vp._12_22_32_42; // Bottom
    planes[3] = vp._14_24_34_44 - vp._12_22_32_42; // Top
    planes[4] = vp._13_23_33_43;                    // Near (Reverse-Z)
    planes[5] = vp._14_24_34_44 - vp._13_23_33_43; // Far

    for (int i = 0; i < 6; ++i)
    {
        float length = sqrt(dot(planes[i].xyz, planes[i].xyz));
        planes[i] /= max(length, 0.00001f);

        if (dot(planes[i].xyz, center) + planes[i].w < -radius)
            return false;
    }
    return true;
}

[numthreads(64, 1, 1)]
void GpuCullCS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint instanceIdx = dispatchThreadId.x;
    if (instanceIdx >= g_SceneParams.totalInstanceCount)
        return;

    StructuredBuffer<GpuInstanceData> instances = ResourceDescriptorHeap[g_SceneParams.instanceBufferSRV];
    GpuInstanceData inst = instances[instanceIdx];

    // Transform bounding sphere center to world space
    float3 worldCenter = mul(float4(inst.boundingSphere.xyz, 1.0f), inst.worldMatrix).xyz;
    float radius = inst.boundingSphere.w;

    // 1. Frustum Culling
    if (!TestSphereFrustum(worldCenter, radius, g_FrameGlobals.viewProjMatrix))
        return;

    // 2. Conservative Hi-Z Occlusion Test
    bool isVisible = true;
    if (g_SceneParams.hizDepthSRV != 0)
    {
        float4 clipPos = mul(float4(worldCenter, 1.0f), g_FrameGlobals.viewProjMatrix);
        if (clipPos.w > 0.0001f)
        {
            float3 ndc = clipPos.xyz / clipPos.w;
            float2 uv = ndc.xy * float2(0.5f, -0.5f) + 0.5f;

            // Project sphere radius to screen space
            float screenRadius = radius / clipPos.w;
            float mip = clamp(log2(max(screenRadius * float(g_SceneParams.hizWidth), 1.0f)), 0.0f, float(g_SceneParams.hizMipCount - 1));

            Texture2D<float> hizTexture = ResourceDescriptorHeap[g_SceneParams.hizDepthSRV];
            SamplerState pointClamp = SamplerDescriptorHeap[2];

            float hizDepth = hizTexture.SampleLevel(pointClamp, uv, mip);

            // In Reverse-Z, if bounding sphere front is farther than Hi-Z depth, it is occluded!
            float sphereFrontDepth = (clipPos.z + radius) / clipPos.w; // Reverse-Z front is larger
            if (sphereFrontDepth < hizDepth)
            {
                isVisible = false;
            }
        }
    }

    if (isVisible)
    {
        // Append to Indirect Draw Command Buffer
        RWStructuredBuffer<uint> drawCountBuffer = ResourceDescriptorHeap[g_SceneParams.drawCountUAV];
        RWStructuredBuffer<IndirectDrawIndexedCommand> cmdBuffer = ResourceDescriptorHeap[g_SceneParams.indirectArgsUAV];

        uint cmdIndex;
        InterlockedAdd(drawCountBuffer[0], 1, cmdIndex);

        IndirectDrawIndexedCommand cmd;
        cmd.objectIndex = instanceIdx;
        cmd.indexCountPerInstance = 36; // Mesh index count
        cmd.instanceCount = 1;
        cmd.startIndexLocation = 0;
        cmd.baseVertexLocation = 0;
        cmd.startInstanceLocation = 0;

        cmdBuffer[cmdIndex] = cmd;
    }
}
