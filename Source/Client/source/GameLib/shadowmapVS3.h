const char* shadowmapVS3 = R"(
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
	float4 LightViewPosition : TEXCOORD2;
	float4 WorldPos : TEXCOORD3;
};

float4x4 Transform : register(c20);
float4x4 ViewProj : register(c4);
float4x4 LightViewProj : register(c8);

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;
	output.WorldPos = mul(float4(input.Position, 1.0f), Transform);
	output.LightViewPosition = mul(output.WorldPos, LightViewProj);
	output.Position = mul(output.WorldPos, ViewProj);
	output.Normal = mul(input.Normal, (float3x3)Transform);
	output.Normal = normalize(output.Normal);
	output.UV = input.UV;
	return output;
}
)";
