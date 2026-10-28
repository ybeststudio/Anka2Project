struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float2 UV2 : TEXCOORD1;
    float3 Normal : TEXCOORD2;
    float4 WorldPos : TEXCOORD4;
    float3 Tangent : TEXCOORD5;
    float4 LightViewPosition1 : TEXCOORD6;
    float4 LightViewPosition2 : TEXCOORD7;
    float4 ScreenPos : TEXCOORD8;
    float4 Diffuse : COLOR0;
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
float enableSpecular : register(c13);
float terrainLightMultiply : register(c14);
float alphaOverride : register(c15);
float uv : register(c17);
float textureSpecularPower : register(c18);
float snowThreshold : register(c85);
float3 characterPosition : register(c86);
float puddleThreshold : register(c87);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);
float time : register(c73);
float3 PrevieousCharacterPosition : register(c93);
float isMoving : register(c94);
float isSnow : register(c95);

sampler2D DiffuseTexture : register(s0);
sampler2D SplatTexture : register(s1);
sampler2D NormalMap : register(s2);
sampler2D shadowMap1 : register(s3);
sampler2D SpecularMap : register(s4);
sampler2D normalMap1 : register(s5);
sampler2D shadowMap2 : register(s6);
sampler2D ReflectionTexture : register(s7);
sampler2D foamTexture : register(s8);
sampler2D snowTexture : register(s9);

static const float2 pcfOffsets[30] =
{
    float2(0.0000, 0.0000), float2(0.5000, 0.0000), float2(0.3536, 0.3536),
    float2(0.0000, 0.5000), float2(-0.3536, 0.3536), float2(-0.5000, 0.0000),
    float2(-0.3536, -0.3536), float2(-0.0000, -0.5000), float2(0.3536, -0.3536),
    float2(1.0000, 0.0000), float2(0.8660, 0.5000), float2(0.5000, 0.8660),
    float2(0.0000, 1.0000), float2(-0.5000, 0.8660), float2(-0.8660, 0.5000),
    float2(-1.0000, 0.0000), float2(-0.8660, -0.5000), float2(-0.5000, -0.8660),
    float2(-0.0000, -1.0000), float2(0.5000, -0.8660), float2(0.8660, -0.5000),
    float2(1.5000, 0.0000), float2(1.1491, 0.9642), float2(0.2605, 1.4772),
    float2(-0.7500, 1.2990), float2(-1.4095, 0.5130), float2(-1.4095, -0.5130),
    float2(-0.7500, -1.2990), float2(0.2605, -1.4772), float2(1.1491, -0.9642)
};

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
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
    float noiseScale = 1.0;
    float noiseValue = smoothNoise(uv * noiseScale);
    float puddleLocalThreshold = puddleThreshold;
    if (isSnow)
        puddleLocalThreshold = 0.3f;
    float puddlePresence = smoothstep(puddleLocalThreshold - 0.4, puddleLocalThreshold + 0.4, noiseValue);
    float flatThreshold = 1e-10;
    float flatnessFactor = 0.15;
    float smoothX = smoothstep(flatThreshold, flatThreshold + flatnessFactor, abs(normal.x));
    float smoothY = smoothstep(flatThreshold, flatThreshold + flatnessFactor, abs(normal.y));
    float isFlatSurface = 0.0f;
    if (smoothX < 1.0f && smoothY < 1.0f)
        isFlatSurface = smoothstep(0.0, 1.0, 1.0f - max(smoothX, smoothY));
    return puddlePresence * isFlatSurface;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 reflection = 0.5 * (1.0 - input.ScreenPos.xy);
    float2 foamUV;
    foamUV.x = 0.01 + 0.98 * (input.WorldPos.x * 0.001);
    foamUV.y = 0.01 + 0.98 * (input.WorldPos.y * 0.001);
    float timer = time;
    float isPuddle = isWithinPuddle(input.UV, input.Normal);
    if (!isPuddle)
        timer = 0.0f;
    float2 animatedUV1 = foamUV + float2(-timer * 0.2 * 0.0001975f, 0.0);
    float2 animatedUV2 = foamUV + float2(timer * 0.2 * 0.0001975f, 0.0);
    if (isSnow > 0.5f)
    {
        animatedUV1 = foamUV;
        animatedUV2 = foamUV;
    }
    float3 normalMapSample1 = tex2D(normalMap1, animatedUV1).rgb;
    float3 normalMapSample2 = tex2D(normalMap1, animatedUV2).rgb;
    float3 blendedNormal = normalize((normalMapSample1 * 2.0 - 1.0) + (normalMapSample2 * 2.0 - 1.0));
    float perturbationScale = (isSnow > 0.5f) ? 0.6 : 0.025;
    float2 perturbedUV = reflection + (blendedNormal.xy * 0.5) * perturbationScale;
    float flatness = 1.0f - saturate(abs(input.Normal.y));
    flatness = smoothstep(0.0, 1.0, flatness);
    float uvScale = (uv > 0.01f) ? uv : 1.0f;
    float4 baseColor = tex2D(DiffuseTexture, input.UV / uvScale);

    float4 Color = baseColor;
    if (isSnow > 0.5f)
    {
        float4 snowColor = tex2D(snowTexture, input.UV / uvScale);
        float snowBlendFactor = saturate(snowThreshold * 2.0f) * flatness;
        Color = lerp(baseColor, snowColor, snowBlendFactor);
    }

    float calculatedAlpha = tex2D(SplatTexture, input.UV2).a;
    Color.a = (alphaOverride > 0.5f) ? 1.0f : min(1.0f, calculatedAlpha * 1.5f);

    float3 baseNormal = normalize((tex2D(NormalMap, input.UV / uvScale).rgb * 2.0f) - 1.0f);
    float3 detailNormal = blendedNormal * 0.35f;
    float3 fullNormal = normalize(baseNormal + detailNormal);
    float snowFade = (isSnow > 0.5f) ? saturate(((snowThreshold * 3.0f) - 0.5f)) : 0.0f;
    snowFade = smoothstep(0.0, 1.0, snowFade);
    float3 flatNormal = float3(0.0f, 0.0f, 1.0f);
    float3 NM = normalize(lerp(fullNormal, flatNormal, snowFade));
    float3 L = -LightDirection;
    float3 V = normalize(CameraPos - input.WorldPos.xyz);
    float3 H = normalize(L + V);
    float NdotH = saturate(dot(NM, H));
    float diffuseLight = saturate(dot(NM, L));

    const float ShadowBias1 = 0.0005f;
    const float ShadowBias2 = 0.00004f;
    const float2 LightTexCoordBig = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoordLocal = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fDepthBig = input.LightViewPosition1.z - ShadowBias1;
    const float fDepthLocal = input.LightViewPosition2.z - ShadowBias2;
    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScaleBig = 1.0f / 2048.0f;
    const float texelScaleLocal = 1.0f / 1024.0f;
    const float increment = 1.0f / 30.0f;
    for (int i = 0; i < 30; ++i)
    {
        float2 offset = pcfOffsets[i];
        bigShadow += (fDepthBig > tex2D(shadowMap1, LightTexCoordBig + offset * texelScaleBig).r ? increment : 0.0f);
        localShadow += (fDepthLocal > tex2D(shadowMap2, LightTexCoordLocal + offset * texelScaleLocal).r ? increment : 0.0f);
    }
    float borderDist = 0.5f - max(abs(0.5f - LightTexCoordLocal.x), abs(0.5f - LightTexCoordLocal.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
    float finalShadow = lerp(bigShadow, localShadow, fade);
    float InvertedOpacity = 1.0f - (50.0f / 100.0f);
    finalShadow *= InvertedOpacity;

    float specSample = tex2D(SpecularMap, input.UV / uvScale).r;
    float roughness = lerp(0.1f, 0.9f, 1.0f - specSample);
    float minShininess = isStorm ? 50.0f : 150.0f;
    float maxShininess = isStorm ? 100.0f : 400.0f;
    float adjustedShininess = lerp(maxShininess, minShininess, roughness);
    float specularComponent = pow(max(NdotH, 0.001f), adjustedShininess);
    specularComponent *= (1.0f - roughness) * (isStorm ? 3.0f : 1.5f);
    float specFactor = (isSnow > 0.5f) ? lerp(2.0f, 1.0f, smoothstep(0.8f, 1.0f, snowThreshold)) : 1.0f;
    specularComponent *= terrainLightMultiply * enableSpecular * specFactor;
    float3 specularLight = specularComponent * specSample * min(textureSpecularPower, 4.0f) * 0.15f;
    specularLight *= (1.0f - finalShadow);

    float groundAmbient = 0.60f;
    float groundDiffuse = diffuseLight * 0.40f;
    float lightFactor = groundAmbient + groundDiffuse;
    lightFactor *= (1.0f - finalShadow * 0.45f);

    Color.rgb *= saturate(lightFactor);
    Color.rgb += specularLight;

    float maxPuddleOpacity = 0.42f;
    float puddleBlendFactor = saturate(isPuddle * maxPuddleOpacity);
    float4 reflectionColor = tex2D(ReflectionTexture, perturbedUV);
    reflectionColor.rgb *= 0.65f;
    float gray = dot(reflectionColor.rgb, float3(0.299f, 0.587f, 0.114f));
    reflectionColor.rgb = lerp(reflectionColor.rgb, gray.xxx, 0.65f);
    Color.rgb = lerp(Color.rgb, reflectionColor.rgb, puddleBlendFactor);

    float2 perturbedUVFoam = input.UV + (blendedNormal.xy * 0.5f) * perturbationScale;
    float4 foamColor = tex2D(foamTexture, perturbedUVFoam);
    if (isSnow > 0.5f)
        Color.rgb = lerp(Color.rgb, foamColor.rgb, saturate(isPuddle / 10.0f));
    else
        Color.rgb = lerp(Color.rgb, foamColor.rgb, saturate(isPuddle / 25.0f));

    float4 finalColor = Color;
    finalColor.a = Color.a;
    if (isSnow > 0.5f)
        finalColor.rgb = finalColor.rgb * 0.9f;

    return finalColor;
}

