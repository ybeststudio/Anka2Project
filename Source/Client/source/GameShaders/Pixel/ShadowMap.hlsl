struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 LightViewSpecular : TEXCOORD4;
    float4 WorldPos : TEXCOORD5;
    float4 ClipPos : TEXCOORD6;
};

#define USE_SHADOWS
#define SHADOW_PCF_RADIUS 2
#define SHADOW_PCF_NUM_SAMPLES 10

float3 MaterialDiffuse : register(c0);
float3 LightDirection : register(c1);
float4 LightDiffuse : register(c2);
float4 AmbientColor : register(c3);
float3 CameraPos : register(c4);
float4 FogColor : register(c5);
float2 FogRange : register(c6);
float3 MaterialSpecular : register(c7);
float3 LightSpecular : register(c9);
int ShadowOpacity : register(c10);
float SpecularLight : register(c11);
float SpecularPowerMaterial : register(c14);
float specularPower : register(c15);
float blendTransition : register(c83);
float SpecularTrans : register(c91);
float SpecularPower : register(c92);

sampler2D DiffuseTexture : register(s0);
sampler2D SpecularTexture : register(s1);
sampler2D ShadowMap1 : register(s2); // big shadowmap
sampler2D ShadowMap2 : register(s3); // local (small) shadowmap

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

float ComputePointLightIntensity(float3 position, float3 lightPosition, float radius)
{
    float3 toLightPosition = position - lightPosition;
    float distanceToLight = length(toLightPosition);
    return pow(saturate(1.0f - distanceToLight / radius), 2);
}

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTexture, input.UV);

    float3 viewDir = normalize(CameraPos - input.WorldPos.xyz);
    float3 worldNormal = normalize(input.Normal);
    if (dot(viewDir, worldNormal) < 0.0f)
        return Color;

    if (input.ClipPos.z < 0.0f || input.ClipPos.z > input.ClipPos.w)
        return Color;

    // Fetch and transform normal from normal map
    float2 texelSize = 1.0f / float2(2048.0f, 2048.0f);
    float3 left = tex2D(DiffuseTexture, input.UV + float2(-texelSize.x, 0)).rgb;
    float3 right = tex2D(DiffuseTexture, input.UV + float2(texelSize.x, 0)).rgb;
    float3 top = tex2D(DiffuseTexture, input.UV + float2(0, -texelSize.y)).rgb;
    float3 bottom = tex2D(DiffuseTexture, input.UV + float2(0, texelSize.y)).rgb;

    float3 dx = right - left;
    float3 dy = bottom - top;

    float3 NM;

    NM = normalize(float3(dy.r - dx.r, dy.g - dx.g, 0.35f));

    NM = NM * 0.5 + 0.5; // Adjust to [0, 1] range

    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float LightIntensity = saturate(dot(N, L));
    float3 LightResult = (AmbientColor.rgb * 2 + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * 0.6f);

#ifdef USE_SHADOWS
    const float ShadowBias = 0.000025f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias;
	 
    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 1024.0f;
    const float texelScale2 = 1.0f / 1024.0f;
    const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;
   
    bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[0] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[0] * texelScale2).r ? increment : 0.0f);
    bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[1] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[1] * texelScale2).r ? increment : 0.0f);
    bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[2] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[2] * texelScale2).r ? increment : 0.0f);
    bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[3] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[3] * texelScale2).r ? increment : 0.0f);
    bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[4] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[4] * texelScale2).r ? increment : 0.0f);
    bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[5] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[5] * texelScale2).r ? increment : 0.0f);
    bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[6] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[6] * texelScale2).r ? increment : 0.0f);

    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
	 
    float finalShadow = lerp(bigShadow, localShadow, fade); //(shadow2 + (shadow1 * fade)) / 2.0f;
    float distance = length(input.WorldPos.xyz - CameraPos);
    float shadowDistance = (distance < 8000.0f) ? 1.0f : lerp(1.7f, 1.0f, saturate((16000.0f - distance) / (16000.0f - 8000.0f)));
    float InvertedOpacity = 1.0f - ((50.0f * shadowDistance) / 120.0f);

    finalShadow = lerp(bigShadow, localShadow, fade);
    finalShadow *= InvertedOpacity;

    LightResult *= (1.0f - finalShadow);

    float specularShadowFactor = 1.0f - finalShadow; // Reduce specular in shadow
    
#endif

    if (Color.a < 1.0 && Color.a > 0.0f)
    {
        float3 V = normalize(CameraPos - input.WorldPos.xyz);
        float3 H = normalize(L + V);
        float3 reflectionVector = reflect(-V, N);
        float2 SpecularUV = reflectionVector.xy * 0.5;
        SpecularUV += SpecularTrans * 0.5;

        float4 SpecularTex = tex2D(SpecularTexture, SpecularUV);
        float specularIntensity = saturate(dot(N, V));
        float specularFactor = (pow(specularIntensity, 0.1f) * SpecularTex.a) * (SpecularPower - 0.33f);

        float3 SpecularColor = SpecularTex.rgb * specularFactor * 2.0f;
        Color.rgb += SpecularColor * Color.a;

        float specularLightTerm = pow(max(dot(N, H), 0.0f), 100.0f) * 6.0f;
        float3 warmTint = float3(1.0, 0.82, 0.55);
        float3 SpecularLightColor = MaterialSpecular * specularLightTerm * warmTint;
        if (finalShadow < 0.3f)
            Color.rgb += SpecularLightColor * Color.a;
    }



    Color.rgb *= LightResult;

    float distanceFactor = (distance < 8000.0f) ? 0.85f : lerp(0.85f, 2.0f, saturate((distance - 8000.0f) / (16000.0f - 8000.0f)));
    float4 FinalColor = Color * distanceFactor;
    if (FogRange.y > FogRange.x && FogRange.y > 100.0f)
    {
        float fogFactor = saturate((distance - FogRange.x) / max(100.0f, (FogRange.y - FogRange.x)));
        FinalColor = lerp(FinalColor, FogColor, fogFactor);
    }
    FinalColor.a = blendTransition;
    return FinalColor;
}