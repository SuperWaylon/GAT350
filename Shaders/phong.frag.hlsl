Texture2D<float4> u_texture : register(t0, space2);
SamplerState u_sampler : register(s0, space2);

cbuffer Uniforms : register(b0, space3)
{
    float4 cameraPosition;   
    float4 lightPosition;    
    float4 lightColor;       
    float4 ambientColor;     
    float4 material;         
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
    
    float3 L = normalize(lightPosition.xyz - input.worldPosition);
    
    float3 V = normalize(cameraPosition.xyz - input.worldPosition);
    
    float3 radiance = lightColor.rgb * lightColor.a;
    
    float3 ambient = ambientColor.rgb;

    float NdotL = saturate(dot(N, L));
    float3 diffuse = radiance * NdotL;

    float3 specular = float3(0.0f, 0.0f, 0.0f);
    if (NdotL > 0.0f)
    {
        float shininess = material.x;
        float specularIntensity;

        if (material.y > 0.5f)
        {
            float3 H = normalize(L + V);
            specularIntensity = pow(saturate(dot(N, H)), shininess);
        }
        else
        {
            float3 R = reflect(-L, N);
            specularIntensity = pow(saturate(dot(R, V)), shininess);
        }

        specular = radiance * specularIntensity;
    }
    Output output;
    output.color = float4(albedo.rgb * (ambient + diffuse) + specular, albedo.a);

    return output;
}
