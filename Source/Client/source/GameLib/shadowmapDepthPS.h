const char* shadowmapDepthPS = R"(
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
    if (Color.a < 0.33f)
        discard;
    return float4(input.DepthPosition.rrr, 1.0f);
}
)";

const char* shadowmapDepthTreePS = R"(
ps.2.0
def c0, 0.33, 0.3, 0.4, 1.0 
def c1, 0.0, 0.0, 0.0, 0.0  ; transparent
dcl_2d s0
dcl t0.xy
dcl t1
mov r3, t1.zzzz          ; r3 = depth value
mov r3.a, c0.a      ; r3 = (depth, depth, depth, 1.0) - for proper alpha blending
texld r0, t0, s0    ; r0 - textured value
sub r1.xyzw, r0.aaaa, c0.xxxx  ; r1 = (d,d,d,d) where d = r0.a - 0.33
cmp r2, r1, r3, c1  ; r2 = (r1>=0)? r3 : c1 - replacement for "discard"
mov oC0, r2
)";
