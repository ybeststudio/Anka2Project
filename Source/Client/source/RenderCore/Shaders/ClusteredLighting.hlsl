// Metin2 Modernized Graphics Engine - RC::ClusteredLighting
// Clustered Forward Light Culling with Log-Depth Grid & 128-bit Bitmask
// HLSL 2021 / Shader Model 6.6 Bindless

struct LocalPointLight
{
    float3 position;
    float radius;

    float3 color;
    float intensity;
};

struct ClusterAABB
{
    float4 minPoint;
    float4 maxPoint;
};

struct ClusteredLightingParams
{
    uint clusterGridX;
    uint clusterGridY;
    uint clusterGridZ;
    uint activeLightCount;

    float nearPlane;
    float farPlane;
    uint lightBufferSRV;
    uint clusterMaskBufferUAV;

    uint clusterMaskBufferSRV;
    uint clusterAABBBufferSRV;
    uint clusterAABBBufferUAV;
    uint pad;
};

ConstantBuffer<ClusteredLightingParams> g_ClusterParams : register(b3);

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

// Screen-space NDC point to View-space ray
float3 ScreenToView(float2 screenCoord, float depth)
{
    float2 ndc = screenCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 clip = float4(ndc, depth, 1.0f);
    float4 view = mul(clip, g_FrameGlobals.invProjMatrix);
    return view.xyz / max(view.w, 0.00001f);
}

// --------------------------------------------------------------------------------------
// 1. Build Cluster AABBs Compute Shader (16 x 9 x 24)
// --------------------------------------------------------------------------------------
[numthreads(16, 9, 1)]
void BuildClusterAABBs_CS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (dispatchThreadId.x >= g_ClusterParams.clusterGridX ||
        dispatchThreadId.y >= g_ClusterParams.clusterGridY ||
        dispatchThreadId.z >= g_ClusterParams.clusterGridZ)
        return;

    uint clusterIndex = dispatchThreadId.x +
                        dispatchThreadId.y * g_ClusterParams.clusterGridX +
                        dispatchThreadId.z * (g_ClusterParams.clusterGridX * g_ClusterParams.clusterGridY);

    float2 tileSize = float2(1.0f / float(g_ClusterParams.clusterGridX), 1.0f / float(g_ClusterParams.clusterGridY));
    float2 tileMin = float2(dispatchThreadId.xy) * tileSize;
    float2 tileMax = tileMin + tileSize;

    // Log-depth slice boundaries (view-space positive depth)
    float sliceNearNorm = float(dispatchThreadId.z) / float(g_ClusterParams.clusterGridZ);
    float sliceFarNorm = float(dispatchThreadId.z + 1) / float(g_ClusterParams.clusterGridZ);

    float zNear = g_ClusterParams.nearPlane * pow(max(g_ClusterParams.farPlane / g_ClusterParams.nearPlane, 1.0001f), sliceNearNorm);
    float zFar = g_ClusterParams.nearPlane * pow(max(g_ClusterParams.farPlane / g_ClusterParams.nearPlane, 1.0001f), sliceFarNorm);

    // Compute 4 corner rays in view space
    float3 p00 = ScreenToView(tileMin, 0.5f);
    float3 p10 = ScreenToView(float2(tileMax.x, tileMin.y), 0.5f);
    float3 p01 = ScreenToView(float2(tileMin.x, tileMax.y), 0.5f);
    float3 p11 = ScreenToView(tileMax, 0.5f);

    float3 ray00 = normalize(p00);
    float3 ray10 = normalize(p10);
    float3 ray01 = normalize(p01);
    float3 ray11 = normalize(p11);

    // Corner points at near and far planes
    float3 cNear[4] = { ray00 * zNear, ray10 * zNear, ray01 * zNear, ray11 * zNear };
    float3 cFar[4]  = { ray00 * zFar,  ray10 * zFar,  ray01 * zFar,  ray11 * zFar };

    float3 aabbMin = float3(1e9, 1e9, 1e9);
    float3 aabbMax = float3(-1e9, -1e9, -1e9);

    for (int i = 0; i < 4; ++i)
    {
        aabbMin = min(aabbMin, min(cNear[i], cFar[i]));
        aabbMax = max(aabbMax, max(cNear[i], cFar[i]));
    }

    RWStructuredBuffer<ClusterAABB> aabbBuffer = ResourceDescriptorHeap[g_ClusterParams.clusterAABBBufferUAV];
    aabbBuffer[clusterIndex].minPoint = float4(aabbMin, 0.0f);
    aabbBuffer[clusterIndex].maxPoint = float4(aabbMax, 0.0f);
}

// --------------------------------------------------------------------------------------
// 2. Light Culling Compute Shader
// --------------------------------------------------------------------------------------
bool TestSphereAABB(float3 center, float radius, float3 aabbMin, float3 aabbMax)
{
    float3 closest = clamp(center, aabbMin, aabbMax);
    float3 diff = closest - center;
    return dot(diff, diff) <= (radius * radius);
}

[numthreads(16, 9, 1)]
void CullLights_CS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (dispatchThreadId.x >= g_ClusterParams.clusterGridX ||
        dispatchThreadId.y >= g_ClusterParams.clusterGridY ||
        dispatchThreadId.z >= g_ClusterParams.clusterGridZ)
        return;

    uint clusterIndex = dispatchThreadId.x +
                        dispatchThreadId.y * g_ClusterParams.clusterGridX +
                        dispatchThreadId.z * (g_ClusterParams.clusterGridX * g_ClusterParams.clusterGridY);

    StructuredBuffer<ClusterAABB> aabbBuffer = ResourceDescriptorHeap[g_ClusterParams.clusterAABBBufferSRV];
    StructuredBuffer<LocalPointLight> lightBuffer = ResourceDescriptorHeap[g_ClusterParams.lightBufferSRV];
    RWStructuredBuffer<uint4> maskBuffer = ResourceDescriptorHeap[g_ClusterParams.clusterMaskBufferUAV];

    float3 aabbMin = aabbBuffer[clusterIndex].minPoint.xyz;
    float3 aabbMax = aabbBuffer[clusterIndex].maxPoint.xyz;

    uint4 lightMask = uint4(0, 0, 0, 0);
    uint activeCount = min(g_ClusterParams.activeLightCount, 128u);

    for (uint i = 0; i < activeCount; ++i)
    {
        LocalPointLight light = lightBuffer[i];

        // Transform light position to view space
        float4 viewPos = mul(float4(light.position, 1.0f), g_FrameGlobals.viewMatrix);

        if (TestSphereAABB(viewPos.xyz, light.radius, aabbMin, aabbMax))
        {
            // Set bit in 128-bit mask
            uint word = i / 32;
            uint bit = i % 32;

            if (word == 0) lightMask.x |= (1u << bit);
            else if (word == 1) lightMask.y |= (1u << bit);
            else if (word == 2) lightMask.z |= (1u << bit);
            else if (word == 3) lightMask.w |= (1u << bit);
        }
    }

    maskBuffer[clusterIndex] = lightMask;
}

// --------------------------------------------------------------------------------------
// Forward Pass Clustered Lighting Evaluation Helper
// --------------------------------------------------------------------------------------
float3 EvaluateClusteredLighting(
    float3 worldPos,
    float3 normal,
    float3 viewDir,
    float linearDepth,
    float2 screenUV,
    float3 albedo,
    float roughness,
    float metallic)
{
    // Find cluster coordinate
    uint clusterX = uint(clamp(screenUV.x * float(g_ClusterParams.clusterGridX), 0.0f, float(g_ClusterParams.clusterGridX - 1)));
    uint clusterY = uint(clamp(screenUV.y * float(g_ClusterParams.clusterGridY), 0.0f, float(g_ClusterParams.clusterGridY - 1)));

    float depthNorm = log(max(linearDepth / g_ClusterParams.nearPlane, 1.0f)) /
                      log(max(g_ClusterParams.farPlane / g_ClusterParams.nearPlane, 1.0001f));
    uint clusterZ = uint(clamp(depthNorm * float(g_ClusterParams.clusterGridZ), 0.0f, float(g_ClusterParams.clusterGridZ - 1)));

    uint clusterIndex = clusterX +
                        clusterY * g_ClusterParams.clusterGridX +
                        clusterZ * (g_ClusterParams.clusterGridX * g_ClusterParams.clusterGridY);

    StructuredBuffer<uint4> maskBuffer = ResourceDescriptorHeap[g_ClusterParams.clusterMaskBufferSRV];
    StructuredBuffer<LocalPointLight> lightBuffer = ResourceDescriptorHeap[g_ClusterParams.lightBufferSRV];

    uint4 mask = maskBuffer[clusterIndex];
    float3 totalLighting = float3(0.0f, 0.0f, 0.0f);

    [unroll]
    for (uint wordIdx = 0; wordIdx < 4; ++wordIdx)
    {
        uint word = (wordIdx == 0) ? mask.x : ((wordIdx == 1) ? mask.y : ((wordIdx == 2) ? mask.z : mask.w));

        while (word != 0)
        {
            uint bit = firstbitlow(word);
            word &= ~(1u << bit);

            uint lightIndex = wordIdx * 32 + bit;
            LocalPointLight light = lightBuffer[lightIndex];

            float3 L = light.position - worldPos;
            float dist = length(L);

            if (dist < light.radius && dist > 0.001f)
            {
                L /= dist;
                float NdotL = max(0.0f, dot(normal, L));

                // Smooth windowed attenuation
                float atten = max(0.0f, 1.0f - (dist / light.radius));
                atten *= atten;

                float3 radiance = light.color * (light.intensity * atten);

                // Lambertian diffuse + specular
                float3 diffuse = albedo / 3.14159265f;
                float3 H = normalize(L + viewDir);
                float NdotH = max(0.0f, dot(normal, H));
                float spec = pow(NdotH, max(1.0f, (1.0f - roughness) * 64.0f));

                totalLighting += (diffuse * (1.0f - metallic) + spec * 0.04f) * radiance * NdotL;
            }
        }
    }

    return totalLighting;
}
