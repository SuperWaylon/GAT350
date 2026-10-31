#include "lighting.hlsli"

cbuffer Uniforms : register(b0, space1)
{
    float4x4 model;
    float4x4 viewProjection;
    float3   cameraPosition;   
    float    shininess;
    Light    light;
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
    
    float3 radiance = light.color * light.intensity;
    
    float3 N = normalize(mul((float3x3)model, input.normal));
    
    float attenuation;
    float3 L = getLightVector(light, worldPosition, attenuation);
    
    float NdotL = saturate(dot(N, L));
    float3 diffuse = radiance * NdotL * attenuation;
    
    output.diffuse = light.ambient + diffuse;
    
    output.specular = float3(0.0f, 0.0f, 0.0f);
    if (NdotL > 0.0f)
    {
        float3 V = normalize(cameraPosition - worldPosition);
        float3 H = normalize(L + V);
        float NdotH = saturate(dot(N, H));

        output.specular = pow(NdotH, shininess) * radiance * attenuation;
    }

    return output;
}
