struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float4 DepthPosition : TEXCOORD1;
};

sampler2D DiffuseTexture : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTexture, input.UV);
    return float4(input.DepthPosition.rrr, 1.0f);
}