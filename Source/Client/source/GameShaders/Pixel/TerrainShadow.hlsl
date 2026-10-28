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
#define SHADOW_PCF_RADIUS 1
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
float Time : register(c12); // Add this to the constants at the bottom
float SpecularLight : register(c11);
float terrainLightMultiply : register(c14);

sampler2D DiffuseTexture : register(s0);
sampler2D ShadowMap1 : register(s3);   // big shadowmap
sampler2D ShadowMap2 : register(s10);   // local (small) shadowmap

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
    float4 Color = tex2D(DiffuseTexture, input.UV);

    // Fetch and transform normal from the diffuse map (pretending it's a normal map)
    float3 NM = normalize((tex2D(DiffuseTexture, input.UV).rgb * 2.0f) - 1.0f);
    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float OriginalLightIntensity = saturate(dot(N, L));
    float LightIntensity = 0.3f + 0.2f * OriginalLightIntensity;
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * 2.8f * terrainLightMultiply);



#ifdef USE_SHADOWS
    const float ShadowBias = 0.0005f;
    const float ShadowBias2 = 0.00004f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias2;
	 
    float bigShadow = 0.0f;
    float localShadow = 0.0f;	 
    const float texelScale1 = 1.0f / 2048.0f;
    const float texelScale2 = 1.0f / 1024.0f;
	 const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;
   
    for (int i = 0; i < SHADOW_PCF_NUM_SAMPLES; ++i) {		 		
            float2 offset = pcfOffsets[i];				
				bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + offset * texelScale1).r ? increment : 0.0f);
				localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + offset * texelScale2).r ? increment : 0.0f);      
    }

	 // Distance to the inner shadow border (0.0 .. 0.5)
    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
	 
	 /*
	 // Visualize map grid (for debug)	 
	 float2 pix = LightTexCoord1 * 2048.0f;
	 pix = pix - floor(pix);
	 pix = 0.5f - abs(pix - 0.5f);
 	 float gridVal = max(pix.x, pix.y);
	 gridVal = smoothstep(0.45f, 0.5f, gridVal);
	 */

    float finalShadow = lerp(bigShadow, localShadow, fade);//(shadow2 + (shadow1 * fade)) / 2.0f;
	 //finalShadow = bigShadow + gridVal * 0.8f;
	 float InvertedOpacity = 1.0f - (45.0f / 120.0f);

	finalShadow = lerp(bigShadow, localShadow, fade); 
	finalShadow *= InvertedOpacity;  // Here, if ShadowOpacity is 100 (full), InvertedOpacity will be 0, hence transparent.

	LightResult *= (1.0f - finalShadow);


    float shadowThreshold = 0.3f;
    float smoothShadowFactor = smoothstep(shadowThreshold, shadowThreshold - 0.1f, finalShadow);
    
    if (smoothShadowFactor > 0.0f)
    {
        // Compute view direction
        float3 V = normalize(CameraPos - input.WorldPos.xyz);
    
        // Compute reflection vector
        float3 R = reflect(-L, N);
    
        // Compute specular reflection based on alpha intensity, modulated by shadow factor
        float SpecularIntensity = pow(saturate(dot(V, R)), Shininess) * 0;
    
        // Scale specular intensity by smooth shadow factor for softer shadows
        SpecularIntensity *= smoothShadowFactor;
    
        // Accumulate specular lighting result, now softened by shadow influence
        LightResult += (SpecularIntensity * MaterialSpecular * LightSpecular.rgb) / 1.5f;
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