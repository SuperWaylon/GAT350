#include "lighting.hlsli"

cbuffer Uniforms : register(b0, space1)
{
    float4x4   model;
    float4x4   viewProjection;
    LightBlock lighting;
};

struct Input
{
    float3 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
    float3 normal   : TEXCOORD2;
};

struct Output
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float3 diffuse  : TEXCOORD1;   
    float3 specular : TEXCOORD2;   
};

Output main(Input input)
{
    Output output;
    
    float3 worldPosition = mul(model, float4(input.position, 1.0f)).xyz;

    output.position = mul(viewProjection, float4(worldPosition, 1.0f));
    output.texcoord = input.texcoord;
    
    float3 N = normalize(mul((float3x3)model, input.normal));
    
    float3 V = normalize(lighting.cameraPosition - worldPosition);
    
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < lighting.lightCount && i < MAX_LIGHTS; i++)
    {
        float3 lightDiffuse;
        float3 lightSpecular;
        computeLight(lighting.lights[i], worldPosition, N, V, lighting.shininess, lightDiffuse, lightSpecular);

        diffuse += lightDiffuse;
        specular += lightSpecular;
    }
    
    output.diffuse = lighting.ambient + diffuse;
    output.specular = specular;

    return output;
}
