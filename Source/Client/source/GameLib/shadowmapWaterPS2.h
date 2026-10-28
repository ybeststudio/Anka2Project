const char* shadowmapWaterPS2 = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition : TEXCOORD2;
    float4 WorldPos : TEXCOORD3;
    float3 Tangent : TEXCOORD4;
};

#define USE_SHADOWS
#define SHADOW_PCF_RADIUS 2

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
int SHADOW_PCF_NUM_SAMPLES : register(c10);
float SpecularLight : register(c11); 

sampler2D DiffuseTexture : register(s0);
sampler2D ShadowMap : register(s1);

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTexture, input.UV);

    // Fetch and transform normal from normal map
    float3 NM = normalize((tex2D(DiffuseTexture, input.UV).rgb * 10.0f) - 1.0f);
    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float LightIntensity = saturate(dot(N, L));
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * 1.0f);

    #ifdef USE_SHADOWS
    const float ShadowBias = 0.00007f;
    int cnt = 0;
    const float2 LightTexCoord = clipSpaceToTextureSpace(input.LightViewPosition);
    const float fCurrDepth = input.LightViewPosition.z - ShadowBias;
    float SampledDepth = tex2D(ShadowMap, LightTexCoord).r;

    if (saturate(LightTexCoord.x) == LightTexCoord.x
        && saturate(LightTexCoord.y) == LightTexCoord.y
        && saturate(fCurrDepth) == fCurrDepth)
    {
        float shadow = 0.0f;
        float2 texelSize = 1.0f / float2(8920.0f, 8080.0f);
        for (int i = -SHADOW_PCF_RADIUS; i <= SHADOW_PCF_RADIUS; ++i)
        {
            for (int j = -SHADOW_PCF_RADIUS; j <= SHADOW_PCF_RADIUS; ++j)
            {
                float2 offset = float2(i, j) * texelSize;
                float depth = tex2D(ShadowMap, LightTexCoord + offset).r;
                shadow += (fCurrDepth - ShadowBias > depth ? 1.0f : 0.0f);
                float delta = fCurrDepth - SampledDepth - ShadowBias;
                if (delta > 0.0f)
                    cnt++;
            }
        }
        shadow /= SHADOW_PCF_NUM_SAMPLES/2;

        float borderDist = 0.5f - max(abs(0.5f - LightTexCoord.x), abs(0.5f - LightTexCoord.y));
        float fade = smoothstep(0.0f, 0.2f, borderDist);
        float shade = (1.0 - cnt * 0.10f * fade);

        LightResult *= (1.0f - shadow * fade);

        if (SpecularLight > 0.5f)  // Use c11 bool to control specular light
        {
            if (shadow < 0.3f)
            {
                // Compute specular reflection based on alpha intensity
                float3 V = normalize(CameraPos - input.WorldPos.xyz);
                float3 R = reflect(-L, N);
                float SpecularIntensity = Color.a * pow(saturate(dot(V, R)), Shininess);

                LightResult += SpecularIntensity * MaterialSpecular * LightSpecular.rgb;

                // Add Phong Specular Reflection only if alpha is greater than 0
                if (Color.a > 0.0f)
                {
                    if (Color.a < 1.0f)
                        LightResult += SpecularIntensity * MaterialSpecular * LightSpecular.rgb * 0.05f;
                }
            }
        }

    }
    #endif

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

)";
