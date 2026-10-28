const char* terrainBasePS = R"(
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
float terrainLightMultiply : register(c14);
float alphaOverride : register(c15);
float isSnow : register(c16);
float uv : register(c17);
float textureSpecularPower : register(c18);
float isIceTerrain : register(c19);
float3 characterPosition : register(c86);
float puddleThreshold : register(c87);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);
float time : register(c73);
float3 PrevieousCharacterPosition : register(c93);
float isMoving : register(c94);

sampler2D DiffuseTexture : register(s0);
sampler2D SplatTexture : register(s1);
sampler2D NormalMap : register(s2);
sampler2D shadowMap1 : register(s3);
sampler2D SpecularMap : register(s4);
sampler2D normalMap1 : register(s5);
sampler2D normalMap2 : register(s6);
sampler2D ReflectionTexture : register(s7);
sampler2D foamTexture : register(s8);
sampler2D shadowMap2 : register(s9);   // cascade 2 (near, high detail)

// x: 1 / shadow map size, y: cascade 2 valid, z: shadow strength, w: shadows enabled
float4 ShadowParams : register(c20);

float ShadowPCF(sampler2D sm, float2 uv, float depth, float texel)
{
    float lit = 0.0f;
    [unroll] for (int y = -1; y <= 1; ++y)
    [unroll] for (int x = -1; x <= 1; ++x)
        lit += (depth <= tex2Dlod(sm, float4(uv + float2(x, y) * texel, 0, 0)).r) ? 1.0f : 0.0f;
    return lit * (1.0f / 9.0f);
}

static const float PI = 3.14159265f;

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float randomNoise(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898f, 78.233f))) * 43758.5453f);
}

float smoothTerrainNoise(float2 uvCoords)
{
    float2 i = floor(uvCoords);
    float2 f = frac(uvCoords);
    f = f * f * (3.0f - 2.0f * f);

    float a = randomNoise(i);
    float b = randomNoise(i + float2(1.0f, 0.0f));
    float c = randomNoise(i + float2(0.0f, 1.0f));
    float d = randomNoise(i + float2(1.0f, 1.0f));

    return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
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

float4 main(PS_INPUT input) : SV_TARGET
{
    float safeUV = max(abs(uv), 0.001f);
    float2 baseUV = input.UV / safeUV;

    // --- STABLE HIGH-RESOLUTION DIFFUSE SAMPLE ---
    float4 Color = tex2D(DiffuseTexture, baseUV);
    float calculatedAlpha = tex2D(SplatTexture, input.UV2).a;
    Color.a = min(1.0f, calculatedAlpha * 1.5f);

    // --- ULTRA-STABLE 3D WORLD NORMAL MAPPING ---
    float3 N_geom = normalize(input.Normal);
    // Normal map yuklenemediyse (null sampler -> 0) duz normal kullan; aksi halde (-1,-1,-1) normal
    // ambient'i (contactAO) ve NdotL'yi sifirlar, zemin/daglar siyah gorunur.
    float3 nmSample = tex2D(NormalMap, baseUV).rgb;
    float3 rawNM = (dot(nmSample, nmSample) < 0.0004f) ? float3(0.0f, 0.0f, 1.0f) : nmSample * 2.0f - 1.0f;
    float nmLenSq = dot(rawNM, rawNM);
    float3 NM = (nmLenSq > 0.0001f) ? rawNM * rsqrt(nmLenSq) : float3(0.0f, 0.0f, 1.0f);

    float3 T_axis = (abs(N_geom.z) < 0.99f) ? normalize(cross(N_geom, float3(0.0f, 0.0f, 1.0f))) : float3(1.0f, 0.0f, 0.0f);
    float3 B_axis = normalize(cross(N_geom, T_axis));
    float3 N_world = normalize(T_axis * NM.x + B_axis * NM.y + N_geom * NM.z);

    // --- ORGANIC 3D PUDDLE MAPPING (FIXED WORLD SPACE, NO PLAYER TRACKING) ---
    float isPuddle = 0.0f;
    if (puddleThreshold < 1.4f)
    {
        float noiseScale = 0.85f;
        float puddleVal = smoothTerrainNoise(input.WorldPos.xy * 0.0005f * noiseScale);
        float puddlePresence = smoothstep(puddleThreshold - 0.25f, puddleThreshold + 0.25f, puddleVal);
        float flatness = smoothstep(0.80f, 0.98f, N_geom.z);
        isPuddle = puddlePresence * flatness;
    }

    // Puddle ripple animation
    if (isPuddle > 0.01f && time > 0.0f)
    {
        float2 waveUV1 = input.WorldPos.xy * 0.0012f + float2(time * 0.00006f, 0.0f);
        float2 waveUV2 = input.WorldPos.xy * 0.0012f - float2(0.0f, time * 0.00006f);
        float3 waveNormal = (tex2D(normalMap1, waveUV1).rgb * 2.0f - 1.0f) + (tex2D(normalMap2, waveUV2).rgb * 2.0f - 1.0f);
        N_world = normalize(N_world + waveNormal * (isPuddle * 0.20f));
    }

    // --- CONTACT AMBIENT OCCLUSION (STONE AND CREVICE DEPTH) ---
    float contactAO = saturate(NM.z * 1.25f) * saturate(N_world.z * 0.45f + 0.55f);

    // --- LIGHT & VIEW VECTORS ---
    float3 L = normalize(-LightDirection);
    float3 toCam = CameraPos - input.WorldPos.xyz;
    float camDist = length(toCam);
    float3 V_world = toCam / max(camDist, 0.001f);
    float3 H = normalize(L + V_world);

    float NdotL = saturate(dot(N_world, L));
    float NdotH = saturate(dot(N_world, H));
    float NdotV = saturate(dot(N_world, V_world));

    // --- CASCADE SHADOW MAPPING (3x3 PCF, slope-scaled bias) ---
    // Previously compiled out (USE_SHADOWS never defined) and s3 was never bound, so the
    // ground received no shadows at all.
    float shadowVisibility = 1.0f;
    if (ShadowParams.w > 0.5f)
    {
        float slope = 1.0f - saturate(dot(N_geom, L));
        float2 shadowUV1 = clipSpaceToTextureSpace(input.LightViewPosition1);
        float fCurrDepth1 = input.LightViewPosition1.z - (0.0002f + 0.0010f * slope);
        float vis = 1.0f;
        if (all(shadowUV1 > 0.005f) && all(shadowUV1 < 0.995f))
            vis = ShadowPCF(shadowMap1, shadowUV1, fCurrDepth1, ShadowParams.x);

        if (ShadowParams.y > 0.5f)
        {
            float2 shadowUV2 = clipSpaceToTextureSpace(input.LightViewPosition2);
            float fCurrDepth2 = input.LightViewPosition2.z - (0.00004f + 0.00015f * slope);
            if (all(shadowUV2 > 0.01f) && all(shadowUV2 < 0.99f))
            {
                float vis2 = ShadowPCF(shadowMap2, shadowUV2, fCurrDepth2, ShadowParams.x);
                float border = 0.5f - max(abs(0.5f - shadowUV2.x), abs(0.5f - shadowUV2.y));
                vis = lerp(vis, vis2, smoothstep(0.0f, 0.1f, border));
            }
        }
        shadowVisibility = lerp(1.0f, vis, ShadowParams.z);
    }

    // --- 2K-4K MATERIAL RESPONSE & SPECULAR SHINE ---
    float specularSample = tex2D(SpecularMap, baseUV).r;
    float roughness = saturate(1.0f - (Shininess * 0.03f + specularSample * 0.28f));
    roughness = clamp(roughness, 0.06f, 0.95f);
    float alphaRough = roughness * roughness;
    float denom = (NdotH * NdotH * (alphaRough * alphaRough - 1.0f) + 1.0f);
    float D = (alphaRough * alphaRough) / (PI * denom * denom + 0.0001f);
    float fresnel = 0.04f + 0.40f * pow(1.0f - NdotV, 3.5f);

    float3 specularColor = LightSpecular.rgb * (D * fresnel * specularSample * textureSpecularPower * 0.25f * shadowVisibility);

    // --- BALANCED DIRECT & AMBIENT ILLUMINATION (NO OVEREXPOSURE) ---
    float3 directDiffuse = (MaterialDiffuse * LightDiffuse.rgb) * (NdotL * shadowVisibility);
    float3 ambientDiffuse = AmbientColor.rgb * (contactAO * 0.52f);
    float3 surfaceLighting = ambientDiffuse + directDiffuse * 0.72f;
    surfaceLighting = min(surfaceLighting, float3(1.10f, 1.10f, 1.10f));

    Color.rgb = Color.rgb * surfaceLighting + specularColor;

    // --- PERSPECTIVE-CORRECT MIRROR REFLECTION UV (PLANAR MIRROR CAMERA COORDINATES) ---
    float invW = 1.0f / max(abs(input.ScreenPos.w), 0.0001f);
    float2 reflectionUV;
    reflectionUV.x = 0.5f * (1.0f - (input.ScreenPos.x * invW));
    reflectionUV.y = 0.5f * (1.0f - (input.ScreenPos.y * invW));
    reflectionUV = clamp(reflectionUV, 0.002f, 0.998f);

    // --- REAL ORGANIC WATER PUDDLES (ONLY DURING ACTIVE RAIN) ---
    if (isPuddle > 0.01f && puddleThreshold < 1.40f)
    {
        float2 perturbedPuddleUV = reflectionUV + NM.xy * 0.008f;
        perturbedPuddleUV = clamp(perturbedPuddleUV, 0.002f, 0.998f);
        float4 mirrorTex = tex2D(ReflectionTexture, perturbedPuddleUV);

        // Wet darkening: Stone/ground under water naturally darkens rather than turning into a flat sky-colored mirror
        Color.rgb *= lerp(1.0f, 0.50f, isPuddle);

        // Subtle specular water sheen & soft reflection (preserves original texture underneath, avoids radioactive cyan)
        float puddleFresnel = saturate(0.02f + 0.60f * pow(1.0f - NdotV, 4.0f));
        float puddleDistFade = saturate(1.0f - (camDist / 5000.0f));
        float puddleBlend = isPuddle * puddleFresnel * puddleDistFade * 0.25f;

        // Modulate reflection with environment lighting to look like real water, not pure sky cyan paint
        float3 naturalPuddleReflection = mirrorTex.rgb * saturate(AmbientColor.rgb * 0.8f + 0.25f);
        Color.rgb = lerp(Color.rgb, naturalPuddleReflection, puddleBlend);
    }

    // --- 2K-4K GLACIAL ICE REFLECTION & CRYSTAL SPECULAR (SOHAN DAĞI / BUZUL HARİTALARI) ---
    if (isIceTerrain > 0.5f)
    {
        float3 iceCrystalTint = float3(0.72f, 0.88f, 1.08f);
        float iceFresnel = 0.15f + 0.82f * pow(1.0f - NdotV, 3.2f);

        // Perturb reflection with 2K ice crack normal map
        float2 iceRefUV = reflectionUV + NM.xy * 0.022f;
        iceRefUV = clamp(iceRefUV, 0.002f, 0.998f);
        float4 iceMirror = tex2D(ReflectionTexture, iceRefUV);

        // Brilliant crystalline sunlight/moonlight reflection
        float iceCrystalShine = pow(NdotH, 52.0f) * 3.8f + pow(NdotH, 14.0f) * 1.1f;
        float3 iceSpecularHighlight = LightSpecular.rgb * (iceCrystalShine * shadowVisibility) * iceCrystalTint;

        // Composite 2K ice texture with glacier tint, planar reflection, and diamond shine
        float3 compositeIce = Color.rgb * iceCrystalTint;
        compositeIce = lerp(compositeIce, iceMirror.rgb * iceCrystalTint * 1.30f, iceFresnel * 0.75f);
        compositeIce += iceSpecularHighlight * 0.70f;

        Color.rgb = compositeIce;
    }

    // --- ZINDAN YANSIMASI (DUNGEON STONE & MARBLE REFLECTION) ---
    if (kingdomSelector > 0.5f || isNight > 0.5f)
    {
        float stonePolish = specularSample * (1.0f - roughness);
        float3 dungeonTorchReflection = AmbientColor.rgb * 0.70f + LightSpecular.rgb * pow(NdotH, 28.0f) * 0.55f;
        Color.rgb += dungeonTorchReflection * (stonePolish * 0.30f * fresnel);
    }

    // --- ATMOSPHERIC FOG ---
    Color.rgb = ApplyAtmosphere(Color.rgb, input.WorldPos.xyz, CameraPos, FogColor, FogRange);

    return saturate(Color);
}
)";

const char* terrainShadowsPS = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
    float3 Tangent : TEXCOORD5;
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
float Time : register(c12);
float SpecularLight : register(c11);
float terrainLightMultiply : register(c14);
float3 characterPosition : register(c86);

sampler2D DiffuseTexture : register(s0);
sampler2D ShadowMap1 : register(s1);
sampler2D ShadowMap2 : register(s2);

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

    float3 N = normalize(input.Normal);
    float3 L = normalize(-LightDirection);
    float3 toCam = CameraPos - input.WorldPos.xyz;
    float camDist = length(toCam);
    float3 V = toCam / max(camDist, 0.001f);

    float NdotL = saturate(dot(N, L));
    float contactAO = saturate(N.z * 0.45f + 0.55f);
    float3 LightResult = AmbientColor.rgb * (contactAO * 0.52f) + (MaterialDiffuse * LightDiffuse.rgb) * (NdotL * 0.72f);
    LightResult = min(LightResult, float3(1.10f, 1.10f, 1.10f));

#ifdef USE_SHADOWS
    const float ShadowBias = 0.0001f;
    const float ShadowBias2 = 0.00004f;
    float2 shadowUV1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    float2 shadowUV2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias2;

    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 1250.0f;
    const float texelScale2 = 1.0f / 600.0f;
    const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;

    bool inCascade2 = (shadowUV2.x > 0.01f && shadowUV2.x < 0.99f && shadowUV2.y > 0.01f && shadowUV2.y < 0.99f);
    bool inCascade1 = (shadowUV1.x > 0.01f && shadowUV1.x < 0.99f && shadowUV1.y > 0.01f && shadowUV1.y < 0.99f);

    if (inCascade1)
    {
        for (int i = 0; i < SHADOW_PCF_NUM_SAMPLES; ++i)
        {
            float2 offset = pcfOffsets[i];
            bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, shadowUV1 + offset * texelScale1).r ? increment : 0.0f);
            if (inCascade2)
                localShadow += (fCurrDepth2 > tex2D(ShadowMap2, shadowUV2 + offset * texelScale2).r ? increment : 0.0f);
        }
    }

    float borderDist = 0.5f - max(abs(0.5f - shadowUV2.x), abs(0.5f - shadowUV2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
    float finalShadow = inCascade2 ? lerp(bigShadow, localShadow, fade) : bigShadow;
    float InvertedOpacity = 1.0f - (35.0f / 120.0f);
    finalShadow *= InvertedOpacity;

    LightResult *= (1.0f - finalShadow);

    if (finalShadow < 0.35f)
    {
        float3 H = normalize(L + V);
        float NdotH = saturate(dot(N, H));
        float specPower = pow(NdotH, clamp(Shininess, 2.0f, 64.0f));
        LightResult += (specPower * MaterialSpecular * LightSpecular.rgb) * 0.75f;
    }
#endif

    Color.rgb *= LightResult;

    // Atmospheric fog
    if (FogRange.x < 50000.0f && FogRange.y > FogRange.x)
    {
        float fogFactor = saturate((camDist - FogRange.x) / (FogRange.y - FogRange.x));
        Color.rgb = lerp(Color.rgb, FogColor.rgb, fogFactor);
    }

    return saturate(Color);
}
)";