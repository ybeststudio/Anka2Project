struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
};

sampler2D GrassTexture : register(s0);
sampler2D ShadowMap1 : register(s1);
sampler2D ShadowMap2 : register(s2);

float3 CameraPosition : register(c4);
float4 LightDiffuse : register(c2);
float4 AmbientColor : register(c3);
float3 LightDirection : register(c1);
float4 FogColor : register(c5);
float2 FogRange : register(c6);
float3 characterPosition : register(c101);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);

static const float3 PointLightColor = float3(1.0f, 0.3f, 0.0f);
static const float PointLightRadius = 3000.0f;

static const float2 pcfOffsets[10] =
{
    float2(0.0000, 0.0000),
    float2(0.5000, 0.0000),
    float2(0.3536, 0.3536),
    float2(0.0000, 0.5000),
    float2(-0.3536, 0.3536),
    float2(-0.5000, 0.0000),
    float2(-0.3536, -0.3536),
    float2(0.0000, -0.5000),
    float2(0.3536, -0.3536),
    float2(1.0000, 0.0000)
};

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 grassColor = tex2D(GrassTexture, input.UV);
    if (grassColor.a < 0.35f)
    {
        discard;
    }

    float distanceToCharacter = length(characterPosition - input.WorldPos.xyz);
    const float ShadowBias = 0.0001f;
    float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias;
    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 1024.0f;
    const float texelScale2 = 1.0f / 1024.0f;
    const float increment = 1.0f / 10.0f;
    for (int i = 0; i < 10; ++i)
    {
        float2 offset = pcfOffsets[i];
        bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + offset * texelScale1).r ? increment : 0.0f);
        localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + offset * texelScale2).r ? increment : 0.0f);
    }
    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
    float finalShadow = lerp(bigShadow, localShadow, fade);
    float InvertedOpacity = 1.0f - (45.0f / 120.0f);
    finalShadow *= InvertedOpacity;

    float3 normalizedLightDir = normalize(LightDirection);
    float3 normalizedNormal = normalize(input.Normal);
    float facingLightFactor = saturate(dot(normalizedLightDir, normalizedNormal));

    float3 amb = max(AmbientColor.rgb, float3(0.35f, 0.35f, 0.38f));
    float3 diff = max(LightDiffuse.rgb, float3(0.65f, 0.65f, 0.65f));

    float3 LightResult = amb + (diff * facingLightFactor);
    float grassIntensity = lerp(0.75f, 1.0f, saturate(distanceToCharacter / 8000.0f));
    LightResult *= (1.0f - finalShadow * 0.45f) * grassIntensity;
    grassColor.rgb *= LightResult;

    if (FogRange.y > FogRange.x && FogRange.y > 100.0f)
    {
        float distance2 = length(input.WorldPos.xyz - CameraPosition);
        const float fogStart = FogRange.x;
        const float fogEnd = FogRange.y / 1.25f;
        if (fogEnd > fogStart + 10.0f)
        {
            float fogFactor = saturate((distance2 - fogStart) / max(100.0f, (fogEnd - fogStart)));
            grassColor.rgb = lerp(grassColor.rgb, FogColor.rgb, fogFactor);
        }
    }

    return grassColor;
}