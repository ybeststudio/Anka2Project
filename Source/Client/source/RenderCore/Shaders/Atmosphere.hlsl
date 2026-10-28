// Sébastien Hillaire (2020) Physically Based Sky & Atmosphere Model
// HLSL 2021 / Shader Model 6.6 Bindless

struct AtmosphereConstants
{
    float3 rayleighScattering;
    float rayleighScaleHeight;

    float3 mieScattering;
    float mieScaleHeight;

    float3 mieExtinction;
    float mieAssymetry;

    float3 ozoneAbsorption;
    float bottomRadius;

    float topRadius;
    float timeOfDay;
    float2 padding0;

    float3 sunDirection;
    float sunIntensity;

    float3 moonDirection;
    float moonIntensity;

    float3 groundAlbedo;
    float multipleScatteringFactor;
};

ConstantBuffer<AtmosphereConstants> g_Atmosphere : register(b2);

static const float PI = 3.14159265359f;

float RayIntersectSphere(float3 rayOrigin, float3 rayDir, float radius)
{
    float b = dot(rayOrigin, rayDir);
    float c = dot(rayOrigin, rayOrigin) - radius * radius;
    float d = b * b - c;
    if (d < 0.0) return -1.0;
    float s = sqrt(d);
    float t0 = -b - s;
    float t1 = -b + s;
    if (t0 > 0.0) return t0;
    if (t1 > 0.0) return t1;
    return -1.0;
}

float3 ComputeSampleExtinction(float height)
{
    float rayleighDensity = exp(-height / g_Atmosphere.rayleighScaleHeight);
    float mieDensity = exp(-height / g_Atmosphere.mieScaleHeight);
    float ozoneDensity = max(0.0, 1.0 - abs(height - 25000.0) / 15000.0);

    return g_Atmosphere.rayleighScattering * rayleighDensity +
           g_Atmosphere.mieExtinction * mieDensity +
           g_Atmosphere.ozoneAbsorption * ozoneDensity;
}

[numthreads(8, 8, 1)]
void ComputeTransmittanceCS(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= 256 || id.y >= 64) return;

    float u = (float(id.x) + 0.5) / 256.0;
    float v = (float(id.y) + 0.5) / 64.0;

    float r = lerp(g_Atmosphere.bottomRadius, g_Atmosphere.topRadius, v * v);
    float mu = u * 2.0 - 1.0;

    float3 rayOrigin = float3(0.0, r, 0.0);
    float3 rayDir = float3(sqrt(max(0.0, 1.0 - mu * mu)), mu, 0.0);

    float t = RayIntersectSphere(rayOrigin, rayDir, g_Atmosphere.topRadius);
    if (t < 0.0) return;

    const int STEP_COUNT = 40;
    float dt = t / float(STEP_COUNT);
    float3 opticalDepth = float3(0.0, 0.0, 0.0);

    for (int i = 0; i < STEP_COUNT; ++i)
    {
        float tCurrent = (float(i) + 0.5) * dt;
        float3 pos = rayOrigin + rayDir * tCurrent;
        float height = length(pos) - g_Atmosphere.bottomRadius;

        opticalDepth += ComputeSampleExtinction(height) * dt;
    }

    float3 transmittance = exp(-opticalDepth);
    RWTexture2D<float4> outTex = ResourceDescriptorHeap[g_Atmosphere.padding0.x];
    outTex[id.xy] = float4(transmittance, 1.0);
}

[numthreads(8, 8, 1)]
void ComputeSkyViewCS(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= 192 || id.y >= 108) return;

    float u = (float(id.x) + 0.5) / 192.0;
    float v = (float(id.y) + 0.5) / 108.0;

    float azimuth = (u * 2.0 - 1.0) * PI;
    float elevation = (v * 2.0 - 1.0) * (PI * 0.5);

    float3 rayDir = float3(
        cos(elevation) * sin(azimuth),
        sin(elevation),
        cos(elevation) * cos(azimuth)
    );

    float3 rayOrigin = float3(0.0, g_Atmosphere.bottomRadius + 10.0, 0.0);
    float t = RayIntersectSphere(rayOrigin, rayDir, g_Atmosphere.topRadius);

    float3 radiance = float3(0.0, 0.0, 0.0);
    if (t > 0.0)
    {
        const int STEP_COUNT = 32;
        float dt = t / float(STEP_COUNT);
        float3 opticalDepth = float3(0.0, 0.0, 0.0);

        for (int i = 0; i < STEP_COUNT; ++i)
        {
            float tSample = (float(i) + 0.5) * dt;
            float3 samplePos = rayOrigin + rayDir * tSample;
            float height = length(samplePos) - g_Atmosphere.bottomRadius;

            float rayleighDensity = exp(-height / g_Atmosphere.rayleighScaleHeight);
            float mieDensity = exp(-height / g_Atmosphere.mieScaleHeight);

            float3 extinction = ComputeSampleExtinction(height);
            opticalDepth += extinction * dt;
            float3 transmittance = exp(-opticalDepth);

            // Sun light
            float cosTheta = dot(rayDir, g_Atmosphere.sunDirection);
            float rayleighPhase = (3.0 / (16.0 * PI)) * (1.0 + cosTheta * cosTheta);
            float g = g_Atmosphere.mieAssymetry;
            float miePhase = (3.0 / (8.0 * PI)) * ((1.0 - g * g) * (1.0 + cosTheta * cosTheta)) /
                             ((2.0 + g * g) * pow(max(0.001, 1.0 + g * g - 2.0 * g * cosTheta), 1.5));

            float3 scattering = g_Atmosphere.rayleighScattering * rayleighDensity * rayleighPhase +
                                g_Atmosphere.mieScattering * mieDensity * miePhase;

            radiance += scattering * transmittance * g_Atmosphere.sunIntensity * dt;
        }
    }

    RWTexture2D<float4> outSky = ResourceDescriptorHeap[g_Atmosphere.padding0.y];
    outSky[id.xy] = float4(radiance, 1.0);
}

struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VSOutput FullscreenVS(uint vertexID : SV_VertexID)
{
    VSOutput output;
    output.uv = float2((vertexID << 1) & 2, vertexID & 2);
    output.pos = float4(output.uv * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0); // Reverse-Z: far is 0.0
    return output;
}

float4 RenderSkyPS(VSOutput input) : SV_Target
{
    Texture2D<float4> skyViewLUT = ResourceDescriptorHeap[g_Atmosphere.padding0.y];
    SamplerState linearClamp = SamplerDescriptorHeap[1];

    float3 skyColor = skyViewLUT.Sample(linearClamp, input.uv).rgb;
    return float4(skyColor, 1.0);
}
