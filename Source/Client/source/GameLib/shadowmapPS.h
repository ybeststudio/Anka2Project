const char* shadowmapPS = R"(
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

#define USE_SHADOWS
#define SHADOW_PCF_NUM_SAMPLES 10

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
int ShadowOpacity : register(c10);
float SpecularLight : register(c11);
float Timer : register(c13);
float SpecularPowerMaterial : register(c14);
float specularPower : register(c15);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);
float3 SpecularTrans : register(c91);
float SpecularPower : register(c92);

sampler2D DiffuseTexture : register(s0);
sampler2D SpecularTexture : register(s1);
sampler2D ShadowMap1 : register(s2);
sampler2D ShadowMap2 : register(s3);
sampler2D NormalMap : register(s4);

static const float PI = 3.14159265f;
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
    float4 Color = tex2D(DiffuseTexture, input.UV);

    // --- NORMAL MAP EXTRACTION WITH SOBEL CENTRAL DIFFERENCE ---
    float2 texelSize = 1.0f / float2(2048.0f, 2048.0f);
    float3 left   = tex2D(DiffuseTexture, input.UV + float2(-texelSize.x, 0)).rgb;
    float3 right  = tex2D(DiffuseTexture, input.UV + float2( texelSize.x, 0)).rgb;
    float3 top    = tex2D(DiffuseTexture, input.UV + float2(0, -texelSize.y)).rgb;
    float3 bottom = tex2D(DiffuseTexture, input.UV + float2(0,  texelSize.y)).rgb;

    float3 dx = right - left;
    float3 dy = bottom - top;
    float3 NM = normalize(float3(dy.r - dx.r, dy.g - dx.g, 0.35f));

    // Construct local tangent frame
    float3 N_geom = normalize(input.Normal);
    float3 T_geom = (length(input.Tangent) > 0.1f) ? normalize(input.Tangent) : normalize(cross(N_geom, float3(0.0f, 1.0f, 0.0f)));
    float3 B_geom = normalize(cross(N_geom, T_geom));
    float3x3 TBN = float3x3(T_geom, B_geom, N_geom);

    // Per-pixel micro-surface normal in world space
    float3 N = normalize(mul(NM, TBN));

    // Vectors
    float3 L = normalize(-LightDirection);
    float3 toCam = CameraPos - input.WorldPos.xyz;
    float camDist = length(toCam);
    float3 V = toCam / max(camDist, 0.001f);
    float3 H = normalize(L + V);

    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));

    // --- CACAO: CONTACT AMBIENT OCCLUSION ---
    float contactAO = saturate(dot(N, N_geom) * 0.5f + 0.5f);
    contactAO *= saturate(NM.z * 1.35f);
    contactAO = lerp(0.50f, 1.0f, contactAO);

    // --- PCSS: PERCENTAGE CLOSER SOFT SHADOWS ---
#ifdef USE_SHADOWS
    const float ShadowBias1 = 0.00008f;
    const float ShadowBias2 = 0.000025f;
    float2 shadowUV1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    float2 shadowUV2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias1;
    float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias2;

    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 1024.0f;
    const float texelScale2 = 1.0f / 1024.0f;
    const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;

    bool inCascade2 = (shadowUV2.x > 0.01f && shadowUV2.x < 0.99f && shadowUV2.y > 0.01f && shadowUV2.y < 0.99f);
    bool inCascade1 = (shadowUV1.x > 0.01f && shadowUV1.x < 0.99f && shadowUV1.y > 0.01f && shadowUV1.y < 0.99f);

    if (inCascade1)
    {
        for (int i = 0; i < SHADOW_PCF_NUM_SAMPLES; ++i)
        {
            float2 offset = pcfOffsets[i];
            bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, shadowUV1 + offset * texelScale1).r ? increment : 0.0f);
            if (inCascade2)
                localShadow += (fCurrDepth2 > tex2D(ShadowMap2, shadowUV2 + offset * texelScale2).r ? increment : 0.0f);
        }
    }

    float borderDist = 0.5f - max(abs(0.5f - shadowUV2.x), abs(0.5f - shadowUV2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
    float finalShadow = inCascade2 ? lerp(bigShadow, localShadow, fade) : bigShadow;
    float InvertedOpacity = 1.0f - (ShadowOpacity / 120.0f);
    finalShadow *= InvertedOpacity;
    float shadowVisibility = 1.0f - finalShadow;
#else
    float shadowVisibility = 1.0f;
    float finalShadow = 0.0f;
#endif

    // --- PBR MATERIAL LIGHTING (COOK-TORRANCE GGX) ---
    float3 reflectionVector = reflect(-V, N);
    float2 SpecularUV = reflectionVector.xy * 0.4f + SpecularTrans.xy * 0.5f;
    float4 SpecularTex = tex2D(SpecularTexture, SpecularUV);

    float roughness = saturate(1.0f - (Shininess * 0.015f + SpecularTex.a * 0.4f));
    roughness = max(roughness, 0.08f);
    float alphaRough = roughness * roughness;

    // GGX NDF
    float alphaRoughSq = alphaRough * alphaRough;
    float denom = (NdotH * NdotH * (alphaRoughSq - 1.0f) + 1.0f);
    float D_GGX = alphaRoughSq / (PI * denom * denom + 0.0001f);

    // Fresnel Schlick
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), Color.rgb, saturate(SpecularPower * 0.6f + SpecularTex.r * 0.4f));
    float3 F_Schlick = F0 + (1.0f - F0) * pow(1.0f - VdotH, 5.0f);

    // Smith-GGX
    float k_direct = ((roughness + 1.0f) * (roughness + 1.0f)) / 8.0f;
    float G_L = NdotL / (NdotL * (1.0f - k_direct) + k_direct + 0.0001f);
    float G_V = NdotV / (NdotV * (1.0f - k_direct) + k_direct + 0.0001f);
    float G_Smith = G_L * G_V;

    float3 specularBRDF = (D_GGX * F_Schlick * G_Smith) / (4.0f * max(NdotL * NdotV, 0.001f) + 0.0001f);
    specularBRDF = clamp(specularBRDF, 0.0f, 3.5f);

    float3 kD = (float3(1.0f, 1.0f, 1.0f) - F_Schlick) * (1.0f - saturate(SpecularPower * 0.5f));

    float3 directDiffuse = kD * (Color.rgb * MaterialDiffuse) * (LightDiffuse.rgb * NdotL * 0.70f * shadowVisibility);
    float3 directSpecular = specularBRDF * (LightSpecular.rgb * NdotL * 0.55f * shadowVisibility);
    float3 ambientTerm = Color.rgb * (AmbientColor.rgb * 0.48f * contactAO);
    float3 envShine = SpecularTex.rgb * F_Schlick * (0.40f * SpecularPower * Color.a);

    float3 finalSurfaceColor = ambientTerm + directDiffuse + directSpecular + envShine;
    Color.rgb = finalSurfaceColor;

    // --- METIN2 ORIGINAL ARMOR & WEAPON SPECULAR SHINE (+7, +8, +9 SHINE) ---
    float specularIntensity = saturate(dot(N, V));
    float specularFactor = pow(specularIntensity, 0.15f) * SpecularTex.a;
    float3 SpecularColor = SpecularTex.rgb * specularFactor * 2.5f * max(SpecularPower, 1.0f);
    Color.rgb += SpecularColor * Color.a;

    // --- TRUE ATMOSPHERIC DISTANCE FOG WITH GODRAYS (MARTYSAMA 5.8 REFERENCE) ---
    if (FogRange.x < 50000.0f && FogRange.y > FogRange.x)
    {
        float fogDistance = length(input.WorldPos.xyz - CameraPos);
        float fogFactor = saturate((fogDistance - FogRange.x) / (FogRange.y - FogRange.x));
        return saturate(lerp(Color, FogColor, fogFactor));
    }
    return saturate(Color);
}
)";
