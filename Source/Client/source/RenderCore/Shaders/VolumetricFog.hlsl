// Metin2 Modernized Graphics Engine - RC::VolumetricFog
// 160x90x64 Froxel Volumetric Fog with Henyey-Greenstein Phase & DXR Visibility
// HLSL 2021 / Shader Model 6.6 Bindless

#include "Atmosphere.hlsl"

struct VolumetricFogParams
{
    float3 fogColor;
    float fogDensity;

    float fogHeightFalloff;
    float fogStartHeight;
    float nearPlane;
    float farPlane;

    float3 sunLightColor;
    float sunLightIntensity;

    float3 sunDirection;
    float anisotropy;

    uint injectionUAV;
    uint accumulatedUAV;
    uint accumulatedSRV;
    uint depthSRV;
};

ConstantBuffer<VolumetricFogParams> g_FogParams : register(b3);

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

static const float FOG_PI = 3.14159265359f;

// Henyey-Greenstein Phase Function
float HenyeyGreensteinPhase(float cosTheta, float g)
{
    float g2 = g * g;
    float denom = 1.0 + g2 - 2.0 * g * cosTheta;
    return (1.0 - g2) / (4.0 * FOG_PI * pow(max(denom, 0.0001f), 1.5f));
}

// --------------------------------------------------------------------------------------
// 1. Light Injection Compute Shader (160 x 90 x 64)
// --------------------------------------------------------------------------------------
[numthreads(8, 8, 1)]
void InjectLightCS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (dispatchThreadId.x >= 160 || dispatchThreadId.y >= 90 || dispatchThreadId.z >= 64)
        return;

    // Normalized grid coords
    float2 uv = (float2(dispatchThreadId.xy) + 0.5f) / float2(160.0f, 90.0f);
    float zSlice = (float(dispatchThreadId.z) + 0.5f) / 64.0f;

    // Exponential slice distribution along depth
    float linearDepth = g_FogParams.nearPlane * pow(max(g_FogParams.farPlane / g_FogParams.nearPlane, 1.0001f), zSlice);

    // Screen to View & World Ray
    float2 ndc = uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 clipPos = float4(ndc, 0.5f, 1.0f);
    float4 viewPos = mul(clipPos, g_FrameGlobals.invProjMatrix);
    viewPos.xyz /= max(viewPos.w, 0.00001f);

    float3 rayDir = normalize(viewPos.xyz);
    float3 worldPos = g_FrameGlobals.cameraPosition + mul(float4(rayDir * linearDepth, 0.0f), g_FrameGlobals.invViewMatrix).xyz;

    // Exponential height falloff fog density
    float heightDiff = max(0.0f, worldPos.y - g_FogParams.fogStartHeight);
    float density = g_FogParams.fogDensity * exp(-heightDiff * g_FogParams.fogHeightFalloff);

    // Henyey-Greenstein Phase for Sun Light
    float3 sunDir = normalize(g_FogParams.sunDirection);
    float3 eyeToSample = normalize(worldPos - g_FrameGlobals.cameraPosition);
    float cosTheta = dot(eyeToSample, sunDir);
    float phase = HenyeyGreensteinPhase(cosTheta, g_FogParams.anisotropy);

    // Visibility / Shadow test
    float shadow = 1.0f;

#if defined(ENABLE_DXR_SHADOWS)
    if (g_FrameGlobals.tlasIndex != 0)
    {
        RayQuery<RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER> rq;
        RayDesc ray;
        ray.Origin = worldPos;
        ray.Direction = sunDir;
        ray.TMin = 0.1f;
        ray.TMax = 500.0f;

        rq.TraceRayInline(ResourceDescriptorHeap[g_FrameGlobals.tlasIndex], RAY_FLAG_NONE, 0xFF, ray);
        while (rq.Proceed()) {}

        if (rq.Status() == RAY_STATUS_COMMITTED_HIT)
        {
            shadow = 0.0f;
        }
    }
#endif

    // Direct Sun in-scattering + ambient sky scattering
    float3 directLight = g_FogParams.sunLightColor * (g_FogParams.sunLightIntensity * phase * shadow);
    float3 ambientLight = g_FogParams.fogColor * 0.15f;
    float3 inScattered = (directLight + ambientLight) * density;

    float extinction = density;

    RWTexture3D<float4> outInjection = ResourceDescriptorHeap[g_FogParams.injectionUAV];
    outInjection[dispatchThreadId] = float4(inScattered, extinction);
}

// --------------------------------------------------------------------------------------
// 2. Volume Integration Compute Shader (Processes full depth column per thread)
// --------------------------------------------------------------------------------------
[numthreads(8, 8, 1)]
void AccumulateCS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (dispatchThreadId.x >= 160 || dispatchThreadId.y >= 90)
        return;

    Texture3D<float4> inInjection = ResourceDescriptorHeap[g_FogParams.injectionUAV];
    RWTexture3D<float4> outAccumulated = ResourceDescriptorHeap[g_FogParams.accumulatedUAV];

    float3 accumulatedScattering = float3(0.0f, 0.0f, 0.0f);
    float accumulatedTransmittance = 1.0f;
    float prevDepth = g_FogParams.nearPlane;

    for (uint z = 0; z < 64; ++z)
    {
        float zSlice = (float(z) + 1.0f) / 64.0f;
        float curDepth = g_FogParams.nearPlane * pow(max(g_FogParams.farPlane / g_FogParams.nearPlane, 1.0001f), zSlice);
        float stepLength = curDepth - prevDepth;
        prevDepth = curDepth;

        float4 sampleVal = inInjection[uint3(dispatchThreadId.xy, z)];
        float3 scattering = sampleVal.rgb;
        float extinction = max(sampleVal.a, 0.000001f);

        // Beer-Lambert law integration over slice
        float stepTransmittance = exp(-extinction * stepLength);
        float3 integratedScattering = (scattering - scattering * stepTransmittance) / extinction;

        accumulatedScattering += integratedScattering * accumulatedTransmittance;
        accumulatedTransmittance *= stepTransmittance;

        outAccumulated[uint3(dispatchThreadId.xy, z)] = float4(accumulatedScattering, accumulatedTransmittance);
    }
}

// --------------------------------------------------------------------------------------
// 3. Fullscreen Composite Pixel Shader (Blends into Scene Color Buffer)
// --------------------------------------------------------------------------------------
struct FogVSOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

FogVSOutput FogFullscreenVS(uint vertexID : SV_VertexID)
{
    FogVSOutput output;
    output.uv = float2((vertexID << 1) & 2, vertexID & 2);
    output.pos = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

float4 FogCompositePS(FogVSOutput input) : SV_Target
{
    Texture2D<float> depthTexture = ResourceDescriptorHeap[g_FogParams.depthSRV];
    Texture3D<float4> accumulatedFog = ResourceDescriptorHeap[g_FogParams.accumulatedSRV];

    SamplerState pointClamp = SamplerDescriptorHeap[2];
    SamplerState linearClamp = SamplerDescriptorHeap[1];

    // Read Reverse-Z depth (1.0 = near, 0.0 = far/infinite)
    float rawDepth = depthTexture.Sample(pointClamp, input.uv);

    // Linearize Reverse-Z depth
    float linearDepth = g_FogParams.nearPlane / max(rawDepth, 0.000001f);
    if (rawDepth <= 0.000001f)
    {
        linearDepth = g_FogParams.farPlane;
    }

    // Map linear depth to 3D froxel z-slice
    float zSlice = log(max(linearDepth / g_FogParams.nearPlane, 1.0f)) / log(max(g_FogParams.farPlane / g_FogParams.nearPlane, 1.0001f));
    zSlice = saturate(zSlice);

    float4 fog = accumulatedFog.SampleLevel(linearClamp, float3(input.uv, zSlice), 0.0f);

    // Return: RGB = In-scattered light, A = Transmittance
    // Blending with scene: Final = Scene * fog.a + fog.rgb (SrcBlend = ONE, DestBlend = SRC_ALPHA)
    return fog;
}
