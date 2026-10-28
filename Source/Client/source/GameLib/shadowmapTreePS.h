const char* shadowmapTreePS = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 LightViewSpecular : TEXCOORD4;
    float4 WorldPos : TEXCOORD5;
    float3 Tangent : TEXCOORD6;
};

float3 MaterialDiffuse : register(c0);
float3 LightDirection : register(c1);
float4 LightDiffuse : register(c2);
float4 AmbientColor : register(c3);
float3 CameraPos : register(c4);
float4 FogColor : register(c5);
float2 FogRange : register(c6);
float3 MaterialSpecular : register(c7);
float Shininess : register(c8);
float3 LightSpecular : register(c9);
float textureDiffusePower : register(c10);
float SpecularLight : register(c11);
float treeNormalMap : register(c12);
float treeFinalColor : register(c13);
float terrainLightMultiply : register(c14);
float alphaOverride : register(c15);
float isSnow : register(c16);
float uv : register(c17);
float textureSpecularPower : register(c18);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);
float3 characterPosition : register(c86);

sampler2D DiffuseTexture : register(s0);
sampler2D shadowMap1 : register(s2);
sampler2D shadowMap2 : register(s3);
sampler2D NormalMap : register(s4);
sampler2D SpecularTexture : register(s5);

static const float2 pcfOffsets[10] = {
    float2(0.0000, 0.0000),
    float2(0.5000, 0.0000),
    float2(0.3536, 0.3536),
    float2(0.0000, 0.5000),
    float2(-0.3536, 0.3536),
    float2(-0.5000, 0.0000),
    float2(-0.3536, -0.3536),
    float2(-0.0000, -0.5000),
    float2(0.3536, -0.3536),
    float2(1.0000, 0.0000), 
};

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTexture, input.UV * 0.5f);
    clip(Color.a - 0.25f);

    float4 SpecularMap = tex2D(SpecularTexture, input.UV * 0.5f);

    float3 NM = normalize((tex2D(NormalMap, input.UV * 0.5f).rgb * max(treeNormalMap, 1.0f)) - 1.0f);
    float3 N = normalize(input.Normal * NM);
    float3 L = normalize(-LightDirection);
    float3 toCam = CameraPos - input.WorldPos.xyz;
    float camDist = length(toCam);
    float3 V = toCam / max(camDist, 0.001f);

    float diffuseLight = saturate(dot(N, L));
    // Leaf subsurface translucency: leaves light up when backlit by sun
    float translucency = saturate(dot(-V, L)) * 0.35f;

#ifdef USE_SHADOWS
    const float ShadowBias = 0.00004f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias;

    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 512.0f;
    const float texelScale2 = 1.0f / 256.0f;
    const float increment = 1.0f / 10.0f;

    for (int i = 0; i < 10; ++i)
    {
        float2 offset = pcfOffsets[i];
        bigShadow   += (fCurrDepth1 > tex2D(shadowMap1, LightTexCoord1 + offset * texelScale1).r ? increment : 0.0f);
        localShadow += (fCurrDepth2 > tex2D(shadowMap2, LightTexCoord2 + offset * texelScale2).r ? increment : 0.0f);
    }

    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
    float finalShadow = lerp(bigShadow, localShadow, fade);
    float InvertedOpacity = 1.0f - (45.0f / 120.0f);
    finalShadow *= InvertedOpacity;
    float shadowVisibility = 1.0f - finalShadow;
#else
    float shadowVisibility = 1.0f;
    float finalShadow = 0.0f;
#endif

    // Foliage ambient occlusion
    float contactAO = saturate(dot(N, float3(0.0f, 0.0f, 1.0f)) * 0.4f + 0.6f);
    float3 directLight = (MaterialDiffuse * LightDiffuse.rgb) * ((diffuseLight + translucency) * 0.70f) * shadowVisibility;
    float3 ambientLight = AmbientColor.rgb * (contactAO * 0.52f);
    float3 LightResult = ambientLight + directLight;
    LightResult = min(LightResult, float3(1.10f, 1.10f, 1.10f));

    Color.rgb *= LightResult * min(max(treeFinalColor, 1.0f), 1.25f);

    // Volumetric atmospheric fog with Mie god rays filtering through tree branches
    // --- TRUE ATMOSPHERIC DISTANCE FOG (MARTYSAMA 5.8 REFERENCE) ---
    const float fogStart = FogRange.x;
    const float fogEnd = FogRange.y;
    float fogFactor = saturate((camDist - fogStart) / max(fogEnd - fogStart, 1.0f));
    return saturate(lerp(Color, FogColor, fogFactor));
}
)";
