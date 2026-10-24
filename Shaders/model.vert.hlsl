cbuffer Uniforms : register(b0, space1)
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
}

// matches the pipeline attributes in Main.cpp (locations 0, 1, 2)
// and the layout of Model::Vertex (position, texcoord, normal)
struct Input
{
    float3 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
    float3 normal   : TEXCOORD2;
};

struct Output
{
    float4 position : SV_Position;
    float3 normal   : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
};

Output main(Input input)
{
    Output output;

    float4 worldPosition = mul(model, float4(input.position, 1.0f));
    output.position = mul(projection, mul(view, worldPosition));

    // rotate the normal into world space (fine as long as the model has no non-uniform scale)
    output.normal = mul((float3x3)model, input.normal);
    output.texcoord = input.texcoord;

    return output;
}
