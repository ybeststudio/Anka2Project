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
#define SHADOW_PCF_RADIUS 2
#define SHADOW_PCF_NUM_SAMPLES 7 // 7

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
int ShadowOpacity : register(c10); // 0..100?
float SpecularLight : register(c11);
float specularPower : register(c12);
float enableSpecular : register(c13);
float SpecularPowerMaterial : register(c14);
float enableArmorSpecular : register(c15);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);

sampler2D DiffuseTexture : register(s0);
sampler2D specularTexture : register(s1);
sampler2D ShadowMap1 : register(s2); // big shadowmap
sampler2D ShadowMap2 : register(s3); // local (small) shadowmap
sampler2D NormalMap : register(s4);

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

// Utility function to compute point light intensity
float ComputePointLightIntensity(float3 position, float3 lightPosition, float radius)
{
    float3 toLightPosition = position - lightPosition;
    float distanceToLight = length(toLightPosition);
    return pow(saturate(1.0f - distanceToLight / radius), 2);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTexture, input.UV);

    // Fetch and transform normal from normal map
    // New normal map generation
    float2 texelSize = 1.0f / float2(1024.0f, 1024.0f);
    float3 left = tex2D(DiffuseTexture, input.UV + float2(-texelSize.x, 0)).rgb;
    float3 right = tex2D(DiffuseTexture, input.UV + float2(texelSize.x, 0)).rgb;
    float3 top = tex2D(DiffuseTexture, input.UV + float2(0, -texelSize.y)).rgb;
    float3 bottom = tex2D(DiffuseTexture, input.UV + float2(0, texelSize.y)).rgb;

    float3 dx = right - left;
    float3 dy = bottom - top;

    float3 NM = normalize(float3(dy.r - dx.r, dy.g - dx.g, 0.008f));

    NM = NM * 0.5 + 0.5; // Adjust to [0, 1] range

    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float LightIntensity = saturate(dot(N, L));
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * 0.4f);

#ifdef USE_SHADOWS
    const float ShadowBias = 0.00008f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias;
     
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

     // Distance to the inner shadow border (0.0 .. 0.5)
    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);

    float finalShadow = lerp(bigShadow, localShadow, fade); //(shadow2 + (shadow1 * fade)) / 2.0f;
     //finalShadow = bigShadow + gridVal * 0.8f;
    float InvertedOpacity = 1.0f - (ShadowOpacity / 120.0f);

    finalShadow = lerp(bigShadow, localShadow, fade);
    finalShadow *= InvertedOpacity; // Here, if ShadowOpacity is 100 (full), InvertedOpacity will be 0, hence transparent.

    LightResult *= (1.0f - finalShadow);

    if (finalShadow < 0.25f)
    {
        // Compute specular reflection based on alpha intensity
        // Add Phong Specular Reflection only if alpha is greater than 0

        float alphaFactor = (Color.a != 1.0f) ? 2.0f : 2.1f;
        float shinessAlpha = (Color.a != 1.0f) ? Shininess : 2.00f;

        float3 V = normalize(CameraPos - input.WorldPos.xyz);
        float3 R = reflect(-L, N);
        float SpecularIntensity = alphaFactor * Color.a * pow(saturate(dot(V, R)), shinessAlpha);

        LightResult += SpecularIntensity * MaterialSpecular * LightSpecular.rgb * alphaFactor * (specularPower / 8.5f) * enableSpecular;
    }
    else
    {
        // Compute specular reflection based on alpha intensity
        // Amplify Phong Specular Reflection when light is coming from the correct side

        float alphaFactor = (Color.a != 1.0f) ? 2.0f : 1.15f;
        float shinessAlpha = (Color.a != 1.0f) ? Shininess : 7.00f;

        float3 V = normalize(CameraPos - input.WorldPos.xyz);
        float3 R = reflect(-L, N);
        // Check the dot product, amplify if the light is coming from the correct side
        float dotVR = dot(V, R);
        float SpecularIntensity;
        
        if (dotVR > 0)
        {
            // Light is coming from the correct side, calculate traditional specular intensity
            SpecularIntensity = alphaFactor * Color.a * pow(saturate(dotVR), shinessAlpha * 2) / 10 * enableSpecular;
        }
        else
        {
            // Light is coming from the opposite side, reduce specular intensity
            // You can adjust the factor to reduce the intensity as desired
            SpecularIntensity = alphaFactor * Color.a * pow(saturate(abs(dotVR)), shinessAlpha / 2) / 15 * enableSpecular;
        }
        
        LightResult += SpecularIntensity * MaterialSpecular * LightSpecular.rgb * alphaFactor;
    }
#endif

    Color.rgb *= LightResult;

    // Calculate additional point light contributions
    float3 pointLightColor = float3(1.0f, 0.6f, 0.0f); // Hardcoded orange color

    

    Color.rgb *= LightResult;

    // Fog
    if (FogRange.y > FogRange.x && FogRange.y > 100.0f)
    {
        float distance = length(input.WorldPos.xyz - CameraPos);
        float fogFactor = saturate((distance - FogRange.x) / max(100.0f, (FogRange.y - FogRange.x)));
        return lerp(Color, FogColor, fogFactor);
    }
    return Color;
}