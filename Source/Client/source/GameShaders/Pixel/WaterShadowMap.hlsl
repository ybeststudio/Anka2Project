struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
    float3 ScreenPos : TEXCOORD5;
    float4 Diffuse : COLOR0;
};

#define USE_SHADOWS
#define SHADOW_PCF_RADIUS 2
#define SHADOW_PCF_NUM_SAMPLES 7

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
float3 OrangeTint : register(c12);
float enableSpecular : register(c13);
float waterOpacity : register(c72);
float time : register(c73);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);

sampler2D ShadowMap1 : register(s1);
sampler2D ShadowMap2 : register(s2);
sampler2D reflectionTexture : register(s3);
sampler2D normalMap1 : register(s4);
sampler2D normalMap2 : register(s5);
sampler2D foamTexture : register(s6); // Foam texture

static const float4 waterColor = { 1.0, 1.0, 1.0, 1.0 };

static const float PI = 3.1416f;
static const float2 pcfOffsets[7] =
{
    float2(0.0, 0.0), // Center
    float2(1.0, 0.0), // 0 degrees
    float2(0.5, 0.87) * 0.6f, // 60 degrees
    float2(-0.5, 0.87), // 120 degrees
    float2(-1.0, 0.0) * 0.6f, // 180 degrees
    float2(-0.5, -0.87), // 240 degrees
    float2(0.5, -0.87) * 0.6f // 300 degrees
};

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = 0.5 * (1.0 - input.ScreenPos.xy);

    float2 foamUV;
    foamUV.x = 0.01 + 0.98 * (input.WorldPos.x * 0.001);
    foamUV.y = 0.01 + 0.98 * (input.WorldPos.y * 0.001);

    // Calculate UV offsets for both normal maps
    float2 timeOffset1 = float2(-time * 0.3 * 0.0003975f, 0.0); // Right to left
    float2 animatedUV1 = foamUV + timeOffset1;

    float2 timeOffset3 = float2(-time * 0.3 * 0.0003975f, 0.0); // Right to left
    float2 animatedUV3 = foamUV + timeOffset3;

    float2 timeOffset2 = float2(time * 0.3 * 0.0003975f, 0.0); // Left to right
    float2 animatedUV2 = foamUV + timeOffset2;

    // Sample both normal maps
    float3 normalMapSample1 = tex2D(normalMap1, animatedUV1).rgb;
    float3 normalMapSample2 = tex2D(normalMap2, animatedUV2).rgb;

    // Blend the normal maps
    float3 blendedNormal = normalize((normalMapSample1 * 2.0 - 1.0) + (normalMapSample2 * 2.0 - 1.0));

    float waterDiffusePerturbation = smoothstep(0.0f, 1.0f, input.Diffuse.a) * (1.0f - 0.2f) + 0.2f;

    float perturbationScale = 0.018 * waterDiffusePerturbation; // Adjust the scale as needed
    float perturbationScaleFoam = 0.4 * waterDiffusePerturbation; // Adjust the scale as needed


    float2 perturbedUV = uv + (blendedNormal.xy * 0.5) * perturbationScale;
    float2 perturbedUVFoam = animatedUV3 + (blendedNormal.xy * 0.5) * perturbationScaleFoam;

    float4 reflectionColor = tex2D(reflectionTexture, perturbedUV);

    // Sample foam texture
    float4 foamColor = tex2D(foamTexture, perturbedUVFoam);
    foamColor.a = 1.0f * waterDiffusePerturbation;

    float3 N = normalize(input.Normal); // Use the original normal for lighting calculation
    float3 N2 = normalize(blendedNormal); // Use the original normal for lighting calculation
    float3 L = -LightDirection;
    float LightIntensity = saturate(dot(N, L));

    // Calculate specular lighting
    float3 viewDir = normalize(CameraPos - input.WorldPos.xyz);
    float3 reflectDir = reflect(-L, N2);
    float spec = pow(saturate(dot(viewDir, reflectDir)), Shininess * 9) / 15;
    float3 specular = spec * LightSpecular * SpecularLight;
    specular *= OrangeTint; // Apply a tiny orange tint

    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity + specular * enableSpecular);

#ifdef USE_SHADOWS
    const float ShadowBias = 0.00004f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias;
    float SampledDepth1 = tex2D(ShadowMap1, LightTexCoord1).r;
    float SampledDepth2 = tex2D(ShadowMap2, LightTexCoord2).r;
    const float squareSize = 0.2f; // Half of the full size
   
    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 1024.0f;
    const float texelScale2 = 1.0f / 1024.0f;
    const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;
   
    for (int i = 0; i < SHADOW_PCF_NUM_SAMPLES; ++i)
    {
        float2 offset = pcfOffsets[i];
        bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + offset * texelScale1).r ? increment : 0.0f);
        localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + offset * texelScale2).r ? increment : 0.0f);
    }

    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);

    float finalShadow = lerp(bigShadow, localShadow, fade); //(shadow2 + (shadow1 * fade)) / 2.0f;
    float InvertedOpacity = 1.0f - (ShadowOpacity / 120.0f);

    finalShadow = lerp(bigShadow, localShadow, fade);
    finalShadow *= InvertedOpacity;



    LightResult *= (1.0f - finalShadow);
      
#endif

    reflectionColor.rgb *= LightResult;

    // Fog
    float fogFactor = 0.0f;
    if (FogRange.y > FogRange.x && FogRange.y > 100.0f)
    {
        float distance = length(input.WorldPos.xyz - CameraPos);
        fogFactor = saturate((distance - FogRange.x) / max(100.0f, (FogRange.y - FogRange.x)));
    }

    // Blend reflection with water color
    float reflectionBlend = 1.0f;
    float4 finalColor = lerp(fogFactor, reflectionColor, reflectionBlend);
    finalColor.rgb *= (1.7 + 2.1 * LightIntensity);
    finalColor.a = 0.7f;

    // Blend foam texture based on the transparency level (input.Diffuse.a)
    float foamBlendFactor = saturate(1.1f - input.Diffuse.a); // Inverse of transparency
    finalColor.rgb = lerp(finalColor.rgb, foamColor.rgb, foamColor.a * foamBlendFactor * waterOpacity);

    float waterDiffuse;
    if (input.Diffuse.a == 1.0f)
        waterDiffuse = 1.0f;
    else
        waterDiffuse = min(input.Diffuse.a * 2.0f, 1.0f);
  
    return finalColor * waterDiffuse;
}