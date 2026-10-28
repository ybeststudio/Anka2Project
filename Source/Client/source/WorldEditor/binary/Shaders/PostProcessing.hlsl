// Metin2 Modernized Graphics Engine - RC::PostProcessing
// Linear HDR ACES Tonemapping, BT.2020/ST.2084 PQ HDR10, and Jimenez IGN Dithering
// HLSL 2021 / Shader Model 6.6 Bindless

struct PostProcessParams
{
    float exposure;
    float paperWhiteNits;
    float maxLuminanceNits;
    uint isHDR10;

    uint sceneColorSRV;
    float bloomIntensity;
    float bloomThreshold;
    uint pad;
};

ConstantBuffer<PostProcessParams> g_PostParams : register(b3);

struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VSOutput FullscreenVS(uint vertexID : SV_VertexID)
{
    VSOutput output;
    output.uv = float2((vertexID << 1) & 2, vertexID & 2);
    output.pos = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

// --------------------------------------------------------------------------------------
// Jimenez Interleaved Gradient Noise (IGN) Dithering
// --------------------------------------------------------------------------------------
float InterleavedGradientNoise(float2 pixelPos)
{
    return frac(52.9829189f * frac(dot(pixelPos, float2(0.06711056f, 0.00583715f))));
}

// --------------------------------------------------------------------------------------
// ACES Film Fit Tonemapping
// --------------------------------------------------------------------------------------
float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

// --------------------------------------------------------------------------------------
// HDR10 Color Transforms (BT.709/Rec.709 -> BT.2020 -> ST.2084 PQ)
// --------------------------------------------------------------------------------------
static const float3x3 REC709_TO_REC2020 =
{
    0.6274040f, 0.3292820f, 0.0433136f,
    0.0690970f, 0.9195400f, 0.0113612f,
    0.0163916f, 0.0880132f, 0.8955950f
};

float3 LinearToPQ(float3 color, float maxNits)
{
    // ST.2084 constants
    const float m1 = 2610.0f / 4096.0f / 4.0f;
    const float m2 = 2523.0f / 4096.0f * 128.0f;
    const float c1 = 3424.0f / 4096.0f;
    const float c2 = 2413.0f / 4096.0f * 32.0f;
    const float c3 = 2392.0f / 4096.0f * 32.0f;

    // Scale to normalized 10,000 nits reference
    float3 Y = saturate(color * (maxNits / 10000.0f));
    float3 Ym1 = pow(Y, m1);
    float3 num = c1 + c2 * Ym1;
    float3 den = 1.0f + c3 * Ym1;
    return pow(num / den, m2);
}

// --------------------------------------------------------------------------------------
// SDR Tonemap Pixel Shader (Rec.709 / sRGB Output)
// --------------------------------------------------------------------------------------
float4 TonemapSDR_PS(VSOutput input) : SV_Target
{
    Texture2D<float3> sceneTexture = ResourceDescriptorHeap[g_PostParams.sceneColorSRV];
    SamplerState linearClamp = SamplerDescriptorHeap[1];

    float3 hdrColor = sceneTexture.Sample(linearClamp, input.uv).rgb;

    // Apply exposure
    hdrColor *= g_PostParams.exposure;

    // ACES Film Tonemap
    float3 ldrColor = ACESFilm(hdrColor);

    // Jimenez IGN Dithering (prevents 8-bit banding on dark gradients and sky)
    float dither = (InterleavedGradientNoise(input.pos.xy) - 0.5f) / 255.0f;
    ldrColor = saturate(ldrColor + dither);

    return float4(ldrColor, 1.0f);
}

// --------------------------------------------------------------------------------------
// HDR10 Tonemap Pixel Shader (BT.2020 / ST.2084 PQ Output)
// --------------------------------------------------------------------------------------
float4 TonemapHDR10_PS(VSOutput input) : SV_Target
{
    Texture2D<float3> sceneTexture = ResourceDescriptorHeap[g_PostParams.sceneColorSRV];
    SamplerState linearClamp = SamplerDescriptorHeap[1];

    float3 hdrColor = sceneTexture.Sample(linearClamp, input.uv).rgb;

    // Apply exposure
    hdrColor *= g_PostParams.exposure;

    // Scale linear values to display paper-white nits
    float3 linearNits = hdrColor * (g_PostParams.paperWhiteNits / 80.0f);

    // Convert from Rec.709 to Rec.2020 color gamut
    float3 rec2020Color = mul(REC709_TO_REC2020, linearNits);

    // Apply ST.2084 PQ curve
    float3 pqColor = LinearToPQ(rec2020Color, g_PostParams.maxLuminanceNits);

    // Jimenez IGN Dithering for 10-bit HDR output
    float dither = (InterleavedGradientNoise(input.pos.xy) - 0.5f) / 1023.0f;
    pqColor = saturate(pqColor + dither);

    return float4(pqColor, 1.0f);
}
