struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float2 UV2 : TEXCOORD1;
    float3 Normal : TEXCOORD2;
    float4 LightViewPosition1 : TEXCOORD3;
    float4 LightViewPosition2 : TEXCOORD4;
    float4 WorldPos : TEXCOORD5;
    float3 Tangent : TEXCOORD6;
    float3 ScreenPos : TEXCOORD8;
};

#define USE_SHADOWS
#define SHADOW_PCF_RADIUS 2
#define SHADOW_PCF_NUM_SAMPLES 30 // 7

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
float specularPower : register(c15);
float time : register(c73);
float kingdomSelector : register(c88);
float isNight : register(c89);

sampler2D DiffuseTexture : register(s0);
sampler2D LightMap : register(s1);
sampler2D ShadowMap1 : register(s2); // big shadowmap
sampler2D ShadowMap2 : register(s3); // local (small) shadowmap
sampler2D normalMap1 : register(s5);
sampler2D normalMap2 : register(s6);
sampler2D ReflectionTexture : register(s7);
sampler2D foamTexture : register(s8);

float ComputePointLightIntensity(float3 position, float3 lightPosition, float radius)
{
    float3 toLightPosition = position - lightPosition;
    float distanceToLight = length(toLightPosition);
    return pow(saturate(1.0f - distanceToLight / radius), 2);
}

float random(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float smoothNoise(float2 uv)
{
    float2 i = floor(uv);
    float2 f = frac(uv);
    f = f * f * (3.0 - 2.0 * f);

    float a = random(i);
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));

    return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}

float isWithinPuddle(float2 uv, float3 normal)
{
    float noiseScale = 1.0; // Controls the size of the puddles
    float noiseValue = smoothNoise(uv * noiseScale);

    float puddlePresence = smoothstep(0.0 - 0.4, 0.0 + 0.4, noiseValue);

    float flatThreshold = 1e-10; 
    float flatnessFactor = 0.75;

    float smoothX = smoothstep(flatThreshold, flatThreshold + flatnessFactor, abs(normal.x));
    float smoothY = smoothstep(flatThreshold, flatThreshold + flatnessFactor, abs(normal.y));

    float isFlatSurface = 0.0f;
    if (smoothX < 1.0f && smoothY < 1.0f)
    {
        isFlatSurface = smoothstep(0.0, 1.0, 1.0f - max(smoothX, smoothY));
    }

    return puddlePresence * isFlatSurface;
}

static const float PI = 3.1416f;
static const float2 pcfOffsets[30] = {
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
    float2 reflection = 0.5 * (1.0 - input.ScreenPos.xy);

    float2 foamUV;
    foamUV.x = 0.01 + 0.98 * (input.WorldPos.x * 0.001);
    foamUV.y = 0.01 + 0.98 * (input.WorldPos.y * 0.001);

    float2 timeOffset1 = float2(-time * 0.3 * 0.0001975f, 0.0); // Right to left
    float2 animatedUV1 = foamUV + timeOffset1;

    float2 timeOffset2 = float2(time * 0.3 * 0.0001975f, 0.0); // Left to right
    float2 animatedUV2 = foamUV + timeOffset2;

    float3 normalMapSample1 = tex2D(normalMap1, animatedUV1).rgb;
    float3 normalMapSample2 = tex2D(normalMap2, animatedUV2).rgb;

    float3 blendedNormal = normalize((normalMapSample1 * 2.0 - 1.0) + (normalMapSample2 * 2.0 - 1.0));

    float perturbationScale = 0.4; // Adjust the scale as needed
    float2 perturbedUV = reflection + (blendedNormal.xy * 0.5) * perturbationScale;

    float4 Color = tex2D(DiffuseTexture, input.UV);
    float4 Color2 = tex2D(LightMap, input.UV2);
    float isPuddle = isWithinPuddle(input.UV, input.Normal);

    // Define texel size based on the texture dimensions
    float2 texelSize = 1.0f / float2(1024.0f, 1024.0f); // Consider making this dynamic based on actual texture size

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
    float3 NM = normalize(float3(dx.r - dy.r, dx.g - dy.g, 0.001f));

    // Adjust NM to [0, 1] range for storing in texture
    NM = NM * 0.5 + 0.5;

    // Combine with original normal map if required
    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float LightIntensity = saturate(dot(N, L));
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity);

    float3 pointLightColor = float3(1.0f, 0.0f, 0.0f);
    float3 newLightPos = float3(88117.734375, -72657.281250, 16987.082031);
    float pointLightIntensity = ComputePointLightIntensity(input.WorldPos.xyz, newLightPos, 2500.0f);
    float3 pointLightContribution = pointLightColor * pointLightIntensity * 6.0f;

    LightResult += pointLightContribution;
    
#ifdef USE_SHADOWS
    const float ShadowBias = 0.00002f;
    const float ShadowBias2 = 0.00004f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias2;
	 
    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 512.0f;
    const float texelScale2 = 1.0f / 512.0f;
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
	finalShadow *= InvertedOpacity;  // Here, if ShadowOpacity is 100 (full), InvertedOpacity will be 0, hence transparent.

	LightResult *= (1.0f - finalShadow);

    float3 V = normalize(CameraPos - input.WorldPos.xyz);

    float3 R = reflect(-L, N);
    float SpecularIntensity = Color.b * pow(saturate(dot(V, R)), 1.0f) * 2.0f;

    if(finalShadow < 0.3f)
        SpecularIntensity *= 5.5f;
    else
        SpecularIntensity *= 0.1f;
      
    if(!isPuddle)
    {
        LightResult += SpecularIntensity * MaterialSpecular * LightSpecular.rgb;     
    }
    

#endif

    float distanceToCamera = length(input.WorldPos.xyz - CameraPos);
    
    float reflectionFadeStart = 4500.0f;
    float reflectionFadeEnd = 100.0f;   
    float reflectionFactor = saturate((distanceToCamera - reflectionFadeEnd) / (reflectionFadeStart - reflectionFadeEnd));

    float4 reflectionColor = tex2D(ReflectionTexture, perturbedUV);
    Color.rgb = lerp(Color.rgb, reflectionColor.rgb * reflectionFactor, (isPuddle * reflectionFactor) * 0.8f);

    float2 perturbedUVFoam =  (blendedNormal.xy * 0.5) * perturbationScale;
    float4 foamColor = tex2D(foamTexture, perturbedUVFoam);
    Color.rgb = lerp(Color.rgb, foamColor.rgb, isPuddle / 6);

    Color.rgb *= LightResult * Color2.rgb * 1.5f;


    // Fog
    if (FogRange.y > FogRange.x && FogRange.y > 100.0f)
    {
        float distance = length(input.WorldPos.xyz - CameraPos);
        float fogFactor = saturate((distance - FogRange.x) / max(100.0f, (FogRange.y - FogRange.x)));
        return lerp(Color, FogColor, fogFactor);
    }
    return Color;
}

