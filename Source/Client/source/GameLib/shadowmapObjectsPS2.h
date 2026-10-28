const char* shadowmapObjectsPS2 = R"(
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
int SHADOW_PCF_NUM_SAMPLES : register(c10); // 0..100?
float SpecularLight : register(c11);

sampler2D DiffuseTexture : register(s0);
sampler2D ShadowMap1 : register(s2); // big shadowmap

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTexture, input.UV);

    // Fetch and transform normal from normal map
    float3 NM = normalize((tex2D(DiffuseTexture, input.UV).rgb * 20.0f) - 1.0f);
    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float LightIntensity = saturate(dot(N, L));
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * 0.01f);

#ifdef USE_SHADOWS
    const float ShadowBias = 0.00002f;
    int cnt = 0;
    const float2 LightTexCoord = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float fCurrDepth = input.LightViewPosition1.z - ShadowBias;
    float SampledDepth = tex2D(ShadowMap1, LightTexCoord).r;

    if (saturate(LightTexCoord.x) == LightTexCoord.x
    && saturate(LightTexCoord.y) == LightTexCoord.y
    && saturate(fCurrDepth) == fCurrDepth)
    {
        float shadow = 0.0f;
        float2 texelSize = 1.0f / float2(5920.0f, 1080.0f);
        for (int i = -SHADOW_PCF_RADIUS; i <= SHADOW_PCF_RADIUS; ++i)
        {
            for (int j = -SHADOW_PCF_RADIUS; j <= SHADOW_PCF_RADIUS; ++j)
            {
                float2 offset = float2(i, j) * texelSize;
                float depth = tex2D(ShadowMap1, LightTexCoord + offset).r;
                shadow += (fCurrDepth - ShadowBias > depth ? 1.0f : 0.0f);
                float delta = fCurrDepth - SampledDepth - ShadowBias;
                if (delta > 0.0f)
                    cnt++;
            }
        }
        shadow /= SHADOW_PCF_NUM_SAMPLES * 1.3;

    // Inverse fade effect calculation
        float borderDist = 0.5f - max(abs(0.5f - LightTexCoord.x), abs(0.5f - LightTexCoord.y));
        float inverseFade = smoothstep(0.0f, 0.2f, borderDist);

        LightResult *= (1.0f - shadow * inverseFade);

        if (SpecularLight > 0.5f)  // Use c11 bool to control specular light
        {
            if (shadow < 0.25)
            {
            // Compute specular reflection based on alpha intensity
                float3 V = normalize(CameraPos - input.WorldPos.xyz);
                float3 R = reflect(-L, N);
                float SpecularIntensity = Color.a * pow(saturate(dot(V, R)), Shininess);

            // Apply inverse fade effect
                SpecularIntensity *= inverseFade;

                if (Color.a < 0.5f) // Increase specular light power by 10 only where alpha channel is present
                {
                    SpecularIntensity *= 6.0f;
                }

                LightResult += SpecularIntensity * MaterialSpecular * LightSpecular.rgb;
            }
            else
            {
			// Compute specular reflection based on alpha intensity
			// Amplify Phong Specular Reflection when light is coming from the correct side
		
                float alphaFactor = (Color.a != 1.0f) ? 6.0f : 0.45f;
                float shinessAlpha = (Color.a != 1.0f) ? Shininess : 7.00f;
		
                float3 V = normalize(CameraPos - input.WorldPos.xyz);
                float3 R = reflect(-L, N);
			// Check the dot product, amplify if the light is coming from the correct side
                float dotVR = dot(V, R);
                float SpecularIntensity;
			
                if (dotVR > 0)
                {
				// Light is coming from the correct side, calculate traditional specular intensity
                    SpecularIntensity = alphaFactor * Color.a * pow(saturate(dotVR), shinessAlpha * 10) / 10;
                }
                else
                {
				// Light is coming from the opposite side, reduce specular intensity
				// You can adjust the factor to reduce the intensity as desired
                    SpecularIntensity = alphaFactor * Color.a * pow(saturate(abs(dotVR)), shinessAlpha * 12) / 26;
                }
		
                LightResult += SpecularIntensity * MaterialSpecular * LightSpecular.rgb * alphaFactor;
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
