const char* grassPixelShader = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2; 
    float4 LightViewPosition2 : TEXCOORD3; 
    float4 WorldPos : TEXCOORD4;
};

sampler2D GrassTexture : register(s0);
sampler2D ShadowMap1 : register(s1);   
sampler2D ShadowMap2 : register(s2);   

float3 CameraPosition : register(c4);
float4 LightDiffuse : register(c2);
float4 AmbientColor : register(c3);
float3 LightDirection : register(c1);
float4 FogColor : register(c5);
float2 FogRange : register(c6);
float3 characterPosition : register(c101);

static const float2 pcfOffsets[10] = {
    float2(0.0000, 0.0000),
    float2(0.5000, 0.0000),
    float2(0.3536, 0.3536),
    float2(0.0000, 0.5000),
    float2(-0.3536, 0.3536),
    float2(-0.5000, 0.0000),
    float2(-0.3536, -0.3536),
    float2(0.0000, -0.5000),
    float2(0.3536, -0.3536),
    float2(1.0000, 0.0000), 
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

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 grassColor = tex2D(GrassTexture, input.UV);

    float distance = length(CameraPosition - input.WorldPos);
    float alphaThreshold = lerp(0.1f, 0.25f, saturate(distance / 6000.0f));

    if (grassColor.a < alphaThreshold)
    {
        discard;
    }

    float distanceToCharacter = length(characterPosition - input.WorldPos);

    // Shadow calculation
    const float ShadowBias = 0.0001f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias;

    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 1024.0f;
    const float texelScale2 = 1.0f / 1024.0f;
    const float increment = 1.0f / 10.0f;

    for (int i = 0; i < 10; ++i) {
        float2 offset = pcfOffsets[i];
        bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + offset * texelScale1).r ? increment : 0.0f);
        localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + offset * texelScale2).r ? increment : 0.0f);
    }

    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
	 
    float finalShadow = lerp(bigShadow, localShadow, fade);

    float InvertedOpacity = 1.0f - (20.0f / 120.0f);
    finalShadow *= InvertedOpacity;

    float3 normalizedLightDir = (length(LightDirection) > 0.001f) ? normalize(LightDirection) : float3(0.0f, 0.0f, 1.0f);
    float3 normalizedNormal = (length(input.Normal) > 0.001f) ? normalize(input.Normal) : float3(0.0f, 0.0f, 1.0f);
    float facingLightFactor = saturate(dot(normalizedLightDir, normalizedNormal));

    float3 ambient = (length(AmbientColor.rgb) > 0.05f) ? AmbientColor.rgb : float3(0.55f, 0.55f, 0.55f);
    float3 diffuse = (length(LightDiffuse.rgb) > 0.05f) ? LightDiffuse.rgb : float3(0.85f, 0.85f, 0.85f);
    float3 LightResult = (ambient + (grassColor.rgb * diffuse));
    float grassIntensity = lerp(0.8f, 1.4f, saturate(distanceToCharacter / 12000.0f));
    LightResult *= (1.0f - finalShadow) * (grassIntensity + 0.3f * facingLightFactor);

    grassColor.rgb *= LightResult;

    // Same fog as the terrain underneath (was far/1.25, so grass greyed out before the ground).
    grassColor.rgb = ApplyAtmosphere(grassColor.rgb, input.WorldPos.xyz, CameraPosition, FogColor, FogRange);

    return grassColor;
}
)";
