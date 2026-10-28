struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
    float2 UV_Transp : TEXCOORD5;
};

#define USE_SHADOWS
#define SHADOW_PCF_RADIUS 2
#define SHADOW_PCF_NUM_SAMPLES 30

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
float enableSpecular : register(c13);
float snowThreshold : register(c85);
float TransparencyEnabled : register(c104);
float3 CharacterPos : register(c105);

sampler2D DiffuseTexture : register(s0);
sampler2D ShadowMap1 : register(s2); // big shadowmap
sampler2D ShadowMap2 : register(s3); // local (small) shadowmap
sampler2D TranspTexture : register(s4);
sampler2D SnowTexture : register(s9);


static const float PI = 3.1416f;
static const float2 pcfOffsets[30] =
{
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
    float2(0.8660, 0.5000),
    float2(0.5000, 0.8660),
    float2(0.0000, 1.0000),
    float2(-0.5000, 0.8660),
    float2(-0.8660, 0.5000),
    float2(-1.0000, 0.0000),
    float2(-0.8660, -0.5000),
    float2(-0.5000, -0.8660),
    float2(-0.0000, -1.0000),
    float2(0.5000, -0.8660),
    float2(0.8660, -0.5000),
    float2(1.5000, 0.0000),
    float2(1.1491, 0.9642),
    float2(0.2605, 1.4772),
    float2(-0.7500, 1.2990),
    float2(-1.4095, 0.5130),
    float2(-1.4095, -0.5130),
    float2(-0.7500, -1.2990),
    float2(0.2605, -1.4772),
    float2(1.1491, -0.9642)
};

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTexture, input.UV);
   
    float3 snowTex = tex2D(SnowTexture, input.UV * 2.0f).rgb;
    float upness = saturate((dot(normalize(input.Normal), float3(0.0f, 0.0f, 1.0f)) - 0.1f) * 1.7f);
    upness = saturate(upness * snowThreshold * 0.4f);
    Color.rgb = lerp(Color.rgb, snowTex.rgb, upness);

    
    // Define texel size based on the texture dimensions
    float2 texelSize = 1.0f / float2(512.0f, 512.0f); // Consider making this dynamic based on actual texture size

    // Sample the surrounding pixels
    float3 tl = tex2D(DiffuseTexture, input.UV + float2(-texelSize.x, -texelSize.y)).rgb; // top-left
    float3 t = tex2D(DiffuseTexture, input.UV + float2(0, -texelSize.y)).rgb; // top
    float3 tr = tex2D(DiffuseTexture, input.UV + float2(texelSize.x, -texelSize.y)).rgb; // top-right
    float3 r = tex2D(DiffuseTexture, input.UV + float2(texelSize.x, 0)).rgb; // right
    float3 br = tex2D(DiffuseTexture, input.UV + float2(texelSize.x, texelSize.y)).rgb; // bottom-right
    float3 b = tex2D(DiffuseTexture, input.UV + float2(0, texelSize.y)).rgb; // bottom
    float3 bl = tex2D(DiffuseTexture, input.UV + float2(-texelSize.x, texelSize.y)).rgb; // bottom-left
    float3 l = tex2D(DiffuseTexture, input.UV + float2(-texelSize.x, 0)).rgb; // left

    // Apply Sobel filter to compute gradients in x and y directions
    float3 dx = tr + 2.0f * r + br - (tl + 2.0f * l + bl);
    float3 dy = bl + 2.0f * b + br - (tl + 2.0f * t + tr);

    // Compute the normal
    float3 NM = normalize(float3(dx.r - dy.r, dx.g - dy.g, 2.28f));
    float3 NM2 = normalize((tex2D(DiffuseTexture, input.UV).rgb * 10.0f) - 1.0f);

    // Adjust NM to [0, 1] range for storing in texture
    NM = NM * 0.5 + 0.5;

    // Combine with original normal map if required
    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float LightIntensity = saturate(dot(N, L));
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * 0.55f);


#ifdef USE_SHADOWS
    const float ShadowBias = 0.0002f;
    const float ShadowBias2 = 0.00004f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias2;
	 
    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 512.0f;
    const float texelScale2 = 1.0f / 1024.0f;
    const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;
   
    for (int i = 0; i < SHADOW_PCF_NUM_SAMPLES; ++i)
    {
        float2 offset = pcfOffsets[i];
        bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + offset * texelScale1).r ? increment : 0.0f);
        localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + offset * texelScale2).r ? increment : 0.0f);
    }

    float bigShadowBorderDist = 0.5f - max(abs(0.5f - LightTexCoord1.x), abs(0.5f - LightTexCoord1.y));
    float bigShadowEdgeFade = smoothstep(0.0f, 0.1f, bigShadowBorderDist);

    bigShadow *= bigShadowEdgeFade;

    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.15f, borderDist);
   
    float finalShadow = lerp(bigShadow, localShadow, fade);
    float distance = length(input.WorldPos.xyz - CameraPos);
    float InvertedOpacity = 1.0f - ((70.0f) / 120.0f);
    finalShadow *= InvertedOpacity;
    LightResult *= (1.0f - finalShadow);

    float3 V = normalize(CameraPos - input.WorldPos.xyz);

    float3 R = reflect(-L, N);
    float SpecularIntensity = Color.b * pow(saturate(dot(V, R)), 1.0f);
    
    float shadowFactor = saturate(smoothstep(0.1f, 0.5f, finalShadow));
    SpecularIntensity *= lerp(5.5f, 0.1f, shadowFactor);
    float distanceFactor = 0.75f;


    float3 lightResultColor = SpecularIntensity * distanceFactor * (LightSpecular / 1.6f) * 0.4f;
    LightResult += lightResultColor * enableSpecular;

#endif

    Color.rgb *= LightResult;

    // Fog effect
    float4 FinalColor = Color;
    if (FogRange.y > FogRange.x && FogRange.y > 100.0f)
    {
        float fogFactor = saturate((distance - FogRange.x) / max(100.0f, (FogRange.y - FogRange.x)));
        FinalColor = lerp(Color, FogColor, fogFactor);
    }

    if (TransparencyEnabled > 0.5f)
    {
        float3 cameraToCharacter = CharacterPos - CameraPos;
        float3 cameraToPixel = input.WorldPos.xyz - CameraPos;
        float projectionLength = dot(cameraToPixel, cameraToCharacter) / dot(cameraToCharacter, cameraToCharacter);
        projectionLength = saturate(projectionLength);
        float3 closestPoint = CameraPos + projectionLength * cameraToCharacter;
        float distanceToLine = length(input.WorldPos.xyz - closestPoint);
        float maxDistance = 800.0f;
        float transparencyFactor = smoothstep(0.0f, maxDistance, distanceToLine);
        transparencyFactor = max(transparencyFactor, 0.3f);
        FinalColor.a *= transparencyFactor;
    }
    return FinalColor;
}
