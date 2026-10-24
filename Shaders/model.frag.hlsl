cbuffer Uniforms : register(b0, space3)
{
    float4 lightDirection;   // direction the light travels (xyz)
    float4 color;            // surface color
}

// must match the vertex shader's Output order, types and semantics
struct Input
{
    float3 normal   : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
};

struct Output
{
    float4 color : SV_Target0;
};

Output main(Input input)
{
    float3 n = normalize(input.normal);
    float3 l = normalize(-lightDirection.xyz);

    float diffuse = max(dot(n, l), 0.0f);
    float ambient = 0.15f;

    Output output;
    output.color = float4(color.rgb * (ambient + diffuse), color.a);

    return output;
}
