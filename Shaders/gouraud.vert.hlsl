cbuffer Uniforms : register(b0, space1)
{
    float4x4 model;
    float4x4 viewProjection;
    float4 cameraPosition;   
    float4 lightPosition;    
    float4 lightColor;       
    float4 ambientColor;     
    float4 material;         
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
    
    float3 radiance = lightColor.rgb * lightColor.a;

    float3 N = normalize(mul((float3x3)model, input.normal));

    float3 L = normalize(lightPosition.xyz - worldPosition);
    
    float NdotL = saturate(dot(N, L));

    float3 diffuse = radiance * NdotL;

    output.diffuse = ambientColor.rgb + diffuse;

    output.specular = float3(0.0f, 0.0f, 0.0f);

    if (NdotL > 0.0f)
    {
        float3 R = reflect(-L, N);

        float3 V = normalize(cameraPosition.xyz - worldPosition);

        float RdotV = saturate(dot(R, V));

        output.specular = pow(RdotV, material.x) * radiance;
    }

    return output;
}
