// lit.color.frag.hlsl
cbuffer Uniforms : register(b0, space3)
{
    float4 tint;
}

struct Input
{
    float4 color : TEXCOORD0;
};

struct Output
{
    float4 color : SV_Target0;
};

Output main(Input input)
{
    Output output;
    output.color = input.color * tint;
    return output;
}