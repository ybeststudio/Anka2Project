struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 InstanceRow0 : TEXCOORD1;
    float4 InstanceRow1 : TEXCOORD2;
    float4 InstanceRow2 : TEXCOORD3;
    float4 InstanceRow3 : TEXCOORD4;
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

float4x4 ViewProj : register(c4);
float4x4 LightViewProj1 : register(c8);
float4x4 LightViewProj2 : register(c16);
float3 WindDirection : register(c46);
float WindStrength : register(c47);
float Time : register(c48);
float3 GrassPosition : register(c49);

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 instanceMatrix;
    instanceMatrix[0] = input.InstanceRow0;
    instanceMatrix[1] = input.InstanceRow1;
    instanceMatrix[2] = input.InstanceRow2;
    instanceMatrix[3] = input.InstanceRow3;

    float sway = sin(input.Position.x * 0.1 + Time) * cos(input.Position.z * 0.1 + Time) * WindStrength;
    float3 displacedPosition = input.Position + WindDirection * sway;

    float4 worldPosition = mul(float4(displacedPosition, 1.0f), instanceMatrix);

    float3 grassPosToCharacter = worldPosition.xyz - GrassPosition;
    float distanceSquared = dot(grassPosToCharacter.xy, grassPosToCharacter.xy);
    if (distanceSquared < 150.0f * 150.0f)
    {
        worldPosition = mul(float4(input.Position, 1.0f), instanceMatrix);
    }
    if (distanceSquared < 90.0f * 90.0f)
    {
        worldPosition.z -= 25.0f;
    }

    output.WorldPos = worldPosition;
    output.Position = mul(worldPosition, ViewProj);
    output.Normal = float3(0.0f, 0.0f, 1.0f);
    output.UV = input.TexCoord;
    output.LightViewPosition1 = mul(worldPosition, LightViewProj1);
    output.LightViewPosition2 = mul(worldPosition, LightViewProj2);

    return output;
}