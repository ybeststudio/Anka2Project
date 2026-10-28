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

float4 main(PS_INPUT input) : SV_TARGET
{
    float uvScale = (uv > 0.01f) ? uv : 1.0f;
    float4 Color = tex2D(DiffuseTexture, input.UV / uvScale);

    float calculatedAlpha = tex2D(SplatTexture, input.UV2).a;
    Color.a = (alphaOverride > 0.5f) ? 1.0f : min(1.0f, calculatedAlpha * 1.5f);

    float3 L = -LightDirection;
    float diffuseLight = saturate(dot(input.Normal, L));
    float groundAmbient = 0.65f;
    float lightFactor = groundAmbient + diffuseLight * 0.35f;

    Color.rgb *= saturate(lightFactor);

    return Color;
}
