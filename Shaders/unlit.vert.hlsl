cbuffer Uniforms : register(b0, space1)
{
    float4x4 mvp;
};

struct Input
{
    float3 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
};

struct Output
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};

Output main(Input input)
{
    Output output;
    output.texcoord = input.texcoord;
    output.position = mul(mvp, float4(input.position, 1.0f));
    return output;
}
