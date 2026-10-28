//terrain
const char* terrainBaseVS = R"(
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Diffuse : COLOR0;
};

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

float4x4 ViewProj : register(c4);
float4x4 Transform : register(c20);
float4x4 texTransform : register(c24);
float4x4 texSplatTransform : register(c28);
float4x4 LightViewProj1 : register(c8);
float4x4 LightViewProj2 : register(c16);

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float4 WorldPos = mul(float4(input.Position, 1.0f), Transform);
    output.Position = mul(WorldPos, ViewProj);
    output.Normal = mul(input.Normal, (float3x3) Transform);
    output.Normal = normalize(output.Normal);
    output.UV = mul(WorldPos, texTransform).xy;
    output.UV2 = mul(WorldPos, texSplatTransform).xy;
    output.WorldPos = WorldPos;
    output.Tangent = float3(1.0, 0.0, 0.0);
    output.LightViewPosition1 = mul(output.WorldPos, LightViewProj1);
    output.LightViewPosition2 = mul(output.WorldPos, LightViewProj2);

    output.ScreenPos = output.Position;
    output.Diffuse = input.Diffuse;

    return output;
}

)";

const char* terrainShadowsVS = R"(
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
};

float4x4 Transform : register(c20);
float4x4 ViewProj : register(c4);
float4x4 LightViewProj1 : register(c8);
float4x4 LightViewProj2 : register(c16);

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.WorldPos = mul(float4(input.Position, 1.0f), Transform);
    //output.WorldPos.z += 0.35; // Offset along Z-axis before transforming
    output.LightViewPosition1 = mul(output.WorldPos, LightViewProj1);
    output.LightViewPosition2 = mul(output.WorldPos, LightViewProj2);
    output.Position = mul(output.WorldPos, ViewProj);
    output.Normal = mul(input.Normal, (float3x3) Transform);
    output.Normal = normalize(output.Normal);
    output.UV = input.UV;
    return output;
}

)";
