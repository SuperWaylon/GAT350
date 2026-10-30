cbuffer Uniforms : register(b0, space1)
{
    float4x4 model;
    float4x4 viewProjection;
};

struct Input
{
    float3 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
    float3 normal   : TEXCOORD2;
};

struct Output
{
    float4 position      : SV_Position;
    float3 worldPosition : TEXCOORD0;   
    float3 normal        : TEXCOORD1;   
    float2 texcoord      : TEXCOORD2;
};

Output main(Input input)
{
    Output output;
    
    float3 worldPosition = mul(model, float4(input.position, 1.0f)).xyz;
    
    output.position = mul(viewProjection, float4(worldPosition, 1.0f));

    output.worldPosition = worldPosition;
    
    output.normal = mul((float3x3)model, input.normal);

    output.texcoord = input.texcoord;

    return output;
}
