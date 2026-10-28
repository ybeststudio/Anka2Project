const char* jtx_waterPixelShader = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
    float4 ScreenPos : TEXCOORD5;
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
float ShadowOpacity : register(c10);
float SpecularLight : register(c11);
float3 OrangeTint : register(c12);
float waterOpacity : register(c72);
float time : register(c73);
// x: reflection target valid, y: wave normal maps bound, z: foam bound, w: shadow map bound
float4 Features : register(c74);

sampler2D ShadowMap1 : register(s1);
sampler2D ShadowMap2 : register(s2);
sampler2D reflectionTexture : register(s3);
sampler2D normalMap1 : register(s4);
sampler2D normalMap2 : register(s5);
sampler2D foamTexture : register(s6); // Foam texture

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

// --- Atmospheric fog (shared by terrain / water / grass) ---
float4 AtmoSunDir : register(c95);    // xyz: direction toward the sun, w: in-scatter exponent
float4 AtmoSunColor : register(c96);  // rgb: sun in-scatter colour, a: max fog opacity
float4 AtmoHeight : register(c97);    // x: height falloff (1/units), y: fog base height

float3 ApplyAtmosphere(float3 col, float3 wpos, float3 camPos, float4 fogColor, float2 fogRange)
{
    if (fogRange.y >= 800000.0f || fogRange.y <= fogRange.x)
        return col;

    float3 v = wpos - camPos;
    float d = length(v);
    float3 dir = v / max(d, 1e-3f);

    // Aerial-perspective curve: thickens early and reaches full fog at the authored far distance.
    float lin = saturate((d - fogRange.x) / (fogRange.y - fogRange.x));
    float distF = (1.0f - exp(-3.0f * lin)) * 1.0524f;

    // Fog hugs the ground: points high above the base (hill tops, peaks) stay clearer.
    float hF = exp(-max(wpos.z - AtmoHeight.y, 0.0f) * AtmoHeight.x);
    float maxOpacity = AtmoSunColor.a > 0.0f ? AtmoSunColor.a : 1.0f;
    float f = saturate(distF * lerp(0.55f, 1.0f, hF)) * maxOpacity;

    // Looking toward the sun the haze glows warm instead of being one flat colour.
    float sun = pow(saturate(dot(dir, AtmoSunDir.xyz)), max(AtmoSunDir.w, 1.0f));
    float3 fogCol = fogColor.rgb + AtmoSunColor.rgb * sun;
    return lerp(col, fogCol, f);
}

float4 main(PS_INPUT input) : COLOR0
{
    // Perspective divide per pixel (w > 0 for every rasterised pixel).
    float2 screenPos = input.ScreenPos.xy / input.ScreenPos.w;
    float2 uv = 0.5 * (1.0 - screenPos);

    float2 foamUV;
    foamUV.x = 0.01 + 0.98 * (input.WorldPos.x * 0.001);
    foamUV.y = 0.01 + 0.98 * (input.WorldPos.y * 0.001);

    float scroll = time * 0.3 * 0.0001975f;
    float2 animatedUV1 = foamUV + float2(-scroll, 0.0); // Right to left
    float2 animatedUV2 = foamUV + float2(scroll, 0.0);  // Left to right
    float2 animatedUV3 = animatedUV1;

    // Wave normal: "whiteout" blend of the two tangent-space maps with z kept up. The old
    // normalize(n1 + n2) could point below the surface (and did whenever a sampler was NULL,
    // giving (-1,-1,-1)), which flipped fresnel to full reflection and rendered black.
    float3 n1 = tex2D(normalMap1, animatedUV1).rgb * 2.0 - 1.0;
    float3 n2 = tex2D(normalMap2, animatedUV2).rgb * 2.0 - 1.0;
    float2 waveSlope = (n1.xy + n2.xy) * Features.y;
    float3 blendedNormal = normalize(float3(waveSlope, max(abs(n1.z * n2.z), 0.25f) * Features.y + (1.0f - Features.y)));

    float waterDiffusePerturbation = smoothstep(0.0f, 1.0f, input.Diffuse.a) * (1.0f - 0.2f) + 0.2f;

    float perturbationScale = 0.01 * waterDiffusePerturbation;
    float perturbationScaleFoam = 0.4 * waterDiffusePerturbation;

    float2 perturbedUV = saturate(uv + (blendedNormal.xy * 0.5) * perturbationScale);
    float2 perturbedUVFoam = animatedUV3 + (blendedNormal.xy * 0.5) * perturbationScaleFoam;

    float3 N = normalize(input.Normal);
    float3 N2 = blendedNormal;
    float3 L = normalize(-LightDirection + float3(0.0f, 0.0f, 1e-4f));
    float LightIntensity = saturate(dot(N, L));

    // Calculate specular lighting
    float3 viewDir = normalize(CameraPos - input.WorldPos.xyz);
    float3 reflectDir = reflect(-L, N2);
    float spec = pow(saturate(dot(viewDir, reflectDir)), Shininess * 7) / 20;
    float3 specular = spec * LightSpecular * SpecularLight;
    specular *= OrangeTint;

    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity + specular);

#ifdef USE_SHADOWS
    if (Features.w > 0.5f)
    {
        const float ShadowBias = 0.00004f;
        const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
        const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
        const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
        const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias;

        float bigShadow = 0.0f;
        float localShadow = 0.0f;
        const float texelScale1 = 1.0f / 500.0f;
        const float texelScale2 = 1.0f / 800.0f;
        const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;

        for (int i = 0; i < SHADOW_PCF_NUM_SAMPLES; ++i)
        {
            float2 offset = pcfOffsets[i];
            bigShadow += (fCurrDepth1 > tex2Dlod(ShadowMap1, float4(LightTexCoord1 + offset * texelScale1, 0, 0)).r ? increment : 0.0f);
            localShadow += (fCurrDepth2 > tex2Dlod(ShadowMap2, float4(LightTexCoord2 + offset * texelScale2, 0, 0)).r ? increment : 0.0f);
        }

        float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
        float fade = smoothstep(0.0f, 0.2f, borderDist);

        float finalShadow = lerp(bigShadow, localShadow, fade);
        finalShadow *= 1.0f - (ShadowOpacity / 120.0f);

        LightResult *= (1.0f - finalShadow);
    }
#endif

    // Reflection: the real target at water level 2, otherwise a sky/horizon approximation so
    // grazing angles (fresnel -> 1) never fall to black.
    float3 skyColor = lerp(FogColor.rgb, LightDiffuse.rgb, 0.25f);
    float3 reflectionColor = lerp(skyColor, tex2D(reflectionTexture, perturbedUV).rgb, Features.x);
    reflectionColor *= LightResult;


    // Natural aquatic base color (pristine crystal turquoise-blue)
    float lightLuminance = max(0.35f, dot(AmbientColor.rgb, float3(0.299f, 0.587f, 0.114f)));
    float3 deepWaterColor = float3(0.06f, 0.28f, 0.38f) * (lightLuminance * 1.2f) + float3(0.02f, 0.06f, 0.10f);
    float fresnel = saturate(0.20f + 0.80f * pow(1.0f - saturate(dot(viewDir, N2)), 3.0f));
    float3 combinedWaterColor = lerp(deepWaterColor, reflectionColor, fresnel);

    float4 baseColor;
    baseColor.rgb = combinedWaterColor + specular * 0.25f;
    baseColor.a = lerp(0.65f, 0.92f, fresnel);

    // Blend foam texture based on shoreline transparency level (input.Diffuse.a)
    float4 foamColor = tex2D(foamTexture, perturbedUVFoam);
    float foamAlpha = 1.4f * waterDiffusePerturbation;
    float foamBlendFactor = saturate(1.1f - input.Diffuse.a);
    baseColor.rgb = lerp(baseColor.rgb, foamColor.rgb, saturate(foamAlpha * foamBlendFactor * waterOpacity) * Features.z);

    float4 finalColor = baseColor;
    finalColor.rgb = ApplyAtmosphere(baseColor.rgb, input.WorldPos.xyz, CameraPos, FogColor, FogRange);

    // Shore fade goes into alpha only. Multiplying rgb as well (with SRCALPHA blending)
    // applied the fade twice and produced a dark rim along every shoreline.
    float waterDiffuse = (input.Diffuse.a >= 0.99f) ? 1.0f : min(input.Diffuse.a * 4.0f, 1.0f);
    finalColor.a = saturate(finalColor.a * waterDiffuse);

    return finalColor;
}
)";
