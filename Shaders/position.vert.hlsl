cbuffer Uniforms : register(b0, space1)
{
    float4x4 model;
}

struct Input
{
    float3 position : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct Output
{
    float4 position : SV_Position;
    float4 color : TEXCOORD0;
    
};

Output main(Input input)
{
    Output output;
    output.position = mul(model, float4(input.position, 1.0f));
    output.color = input.color;

    return output;
}