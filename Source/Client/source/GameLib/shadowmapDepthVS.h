const char* shadowmapDepthVS = R"(
struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float4 DepthPosition : TEXCOORD1;
};

float4x4 Transform : register(c20);
float4x4 ViewProj : register(c4);

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = mul(input.Position, Transform);
    output.Position = mul(output.Position, ViewProj);
    output.DepthPosition = output.Position.zzzz;
    output.UV = input.UV;
    return output;
}
)";
