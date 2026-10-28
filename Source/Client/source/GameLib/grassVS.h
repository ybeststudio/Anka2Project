const char* grassVertexShader = R"(
struct VS_INPUT
{
    float3 Position : POSITION;
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
float4 WindDir : register(c46);         // xy: horizontal wind direction
float4 WindParam : register(c47);       // x: tip amplitude, y: 1 / blade height
float4 WindTime : register(c48);        // x: time in seconds
float3 GrassPosition : register(c49);   // Character position

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    // Instance root in world space gives each tuft its own phase (the local vertices are shared,
    // so object-space sway moved every tuft in lockstep).
    float3 inst = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), Transform).xyz;

    // Root stays planted, tip moves most.
    float h = saturate(input.Position.z * WindParam.y);
    float bend = h * h;

    float T = WindTime.x;
    float phase = dot(inst.xy, float2(0.0021f, 0.0017f));
    // Low-frequency gust front travelling across the field + per-tuft sway + small flutter.
    float gust = 0.55f + 0.45f * sin(T * 0.35f - inst.x * 0.0006f) * sin(T * 0.17f - inst.y * 0.0004f + 1.3f);
    float sway = 0.65f * sin(T * 1.6f + phase * 6.2831f) + 0.35f * sin(T * 2.9f + phase * 11.3f);
    float flutter = 0.12f * sin(T * 7.0f + phase * 23.0f + input.Position.x * 0.05f);
    float2 off = WindDir.xy * WindParam.x * bend * (gust + 0.35f * sway + flutter);

    float4 worldPosition = mul(float4(input.Position, 1.0f), Transform);
    worldPosition.xy += off;
    worldPosition.z -= dot(off, off) * 0.5f * WindParam.y;   // keep blade length roughly constant

    // Character interaction: push blades away and flatten them smoothly instead of freezing.
    float2 d = worldPosition.xy - GrassPosition.xy;
    float dl = length(d) + 1e-3f;
    float k = saturate(1.0f - dl / 150.0f) * bend;
    worldPosition.xy += (d / dl) * 60.0f * k;
    worldPosition.z -= 25.0f * k;

    output.WorldPos = worldPosition;
    output.Position = mul(worldPosition, ViewProj);
    output.Normal = float3(0.0f, 0.0f, 1.0f);
    output.UV = input.UV;
    output.LightViewPosition1 = mul(worldPosition, LightViewProj1);
    output.LightViewPosition2 = mul(worldPosition, LightViewProj2);
    return output;
}
)";
