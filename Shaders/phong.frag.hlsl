#include "lighting.hlsli"

Texture2D<float4> u_texture : register(t0, space2);
SamplerState u_sampler : register(s0, space2);

cbuffer Uniforms : register(b0, space3)
{
    LightBlock lighting;
};

struct Input
{
    float3 worldPosition : TEXCOORD0;
    float3 normal        : TEXCOORD1;
    float2 texcoord      : TEXCOORD2;
};

struct Output
{
    float4 color : SV_Target0;
};

Output main(Input input)
{
    float4 albedo = u_texture.Sample(u_sampler, input.texcoord);
    
    float3 N = normalize(input.normal);
    
    float3 V = normalize(lighting.cameraPosition - input.worldPosition);
    
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < lighting.lightCount && i < MAX_LIGHTS; i++)
    {
        float3 lightDiffuse;
        float3 lightSpecular;
        computeLight(lighting.lights[i], input.worldPosition, N, V, lighting.shininess, lightDiffuse, lightSpecular);

        diffuse += lightDiffuse;
        specular += lightSpecular;
    }
    
    Output output;
    output.color = float4(albedo.rgb * (lighting.ambient + diffuse) + specular, albedo.a);

    return output;
}
