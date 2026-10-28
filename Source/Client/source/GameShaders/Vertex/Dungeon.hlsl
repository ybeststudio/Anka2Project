struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
    float2 UV2 : TEXCOORD1;
};

struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float2 UV2 : TEXCOORD1;
    float3 Normal : TEXCOORD2;
    float4 LightViewPosition1 : TEXCOORD3;
    float4 LightViewPosition2 : TEXCOORD4;
    float4 WorldPos : TEXCOORD5;
    float4 ScreenPos : TEXCOORD8;
};

float4x4 Transform : register(c20);
float4x4 ViewProj : register(c4);
float4x4 LightViewProj1 : register(c8);
float4x4 LightViewProj2 : register(c16);

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.WorldPos = mul(float4(input.Position, 1.0f), Transform);
    output.LightViewPosition1 = mul(output.WorldPos, LightViewProj1);
    output.LightViewPosition2 = mul(output.WorldPos, LightViewProj2);
    output.Position = mul(output.WorldPos, ViewProj);
    output.Normal = mul(input.Normal, (float3x3) Transform);
    output.Normal = normalize(output.Normal);
    output.UV = input.UV;
    output.UV2 = input.UV2;

    float4 worldPos = mul(float4(input.Position, 1.0f), Transform);
    float4 screenPos = mul(worldPos, ViewProj); 
    output.ScreenPos = screenPos / screenPos.w;
    return output;
}

