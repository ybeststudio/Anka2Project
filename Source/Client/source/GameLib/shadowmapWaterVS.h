const char* jtx_waterVertexShader = R"(
// Water vertex stream is SWaterVertex (D3DFVF_XYZ | D3DFVF_DIFFUSE): no normal, no UV.
// Reading NORMAL/TEXCOORD0 here returned undefined data (normalize(0) = NaN on many drivers).
struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Diffuse : COLOR0;
};

struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 Diffuse : COLOR0;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
    float4 ScreenPos : TEXCOORD5;
};


float4x4 Transform : register(c20);
float4x4 ViewProj : register(c4);
float4x4 LightViewProj1 : register(c8);
float4x4 LightViewProj2 : register(c16);

// Parameters for the ripple effect
float Time : register(c30);
float RippleSpeed : register(c31);
float RippleHeight : register(c32);
float RippleFrequency : register(c33);
float3 RippleOrigin : register(c34);

// Noise function for more organic ripples
float noise(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

PS_INPUT main(VS_INPUT input)
{
    float noiseFactor = noise(input.Position.xy * 0.1) * 2000.0;

    float waterDiffuse;
    if(input.Diffuse.a > 0.7f)
        waterDiffuse = 1.0f;
    else
        waterDiffuse = min(input.Diffuse.a / 4.0f, 1.0f);

    float ripple1 = RippleHeight / 10 * sin(RippleFrequency - Time * RippleSpeed + noiseFactor) * waterDiffuse;
    float ripple2 = RippleHeight / 10 * sin((RippleFrequency * 0.75) - Time * (RippleSpeed * 1.2) + noiseFactor) * waterDiffuse;

    input.Position.z += ripple1 + ripple2 * 2;

    PS_INPUT output;
    output.WorldPos = mul(float4(input.Position, 1.0f), Transform);
    output.LightViewPosition1 = mul(output.WorldPos, LightViewProj1);
    output.LightViewPosition2 = mul(output.WorldPos, LightViewProj2);
    output.Position = mul(output.WorldPos, ViewProj);

    // Pass undivided clip position; the pixel shader divides by w. Dividing per vertex broke
    // perspective-correct interpolation and flipped the lookup for vertices behind the camera
    // (large patches at low camera pitch), which showed as black/warped reflection.
    output.ScreenPos = output.Position;

    output.Normal = float3(0.0f, 0.0f, 1.0f);
    output.UV = output.WorldPos.xy * 0.001f;
    output.Diffuse = input.Diffuse;
    return output;
}

)";
