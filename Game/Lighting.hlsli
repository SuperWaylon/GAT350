#pragma once

#define LIGHT_DIRECTIONAL   0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2


struct Material
{
    float3 color;
    float2 tiling;
    float2 offset;
    float shininess;
    float3 padding;
};

struct Light
{
    float3 position;
    uint type;
    float3 direction;
    float range;
    float3 color;
    float intensitiy;
    float3 ambient;
    float innerCutoff;
    float outerCutoff;
    float3 padding;
};

float calculateAttenuation(float distance, float range)
{
    return pow(1 - saturate(distance / range), 2);
}

float3 getLightVector(Light light, float3 position, out float attenuation)
{
    float3 L;
    attenuation = 1.0f;
    
    switch (light.type)
    {
        case LIGHT_DIRECTIONAL:
            L = normalize(-light.direction);
            break;
        case LIGHT_POINT:
            {
                float3 toLight = light.position - position;
                float distance = length(toLight);
                attenuation = calculateAttenuation(distance, light.range);
                L = toLight / distance;
            }
            break;
        case LIGHT_SPOT:
            {
                float3 toLight = light.position - position;            
                float distance = length(toLight);
                L = toLight / distance;
                attenuation = calculateAttenuation(distance, light.range);
                attenuation = smoothstep(light.outerCutoff, light.innerCutoff, dot(-L, light.direction));
            }
            break;
    }
}

void calculateLighting(Light light, Material material, float3 position, float3 N, float3 V,
                       out float3 diffuse, out float3 specular)
{
    float attenuation;
    float3 L = getLightVector(light, position, attenuation);

    // light arriving at the surface
    float3 radiance = light.color * light.intensity * attenuation;

    diffuse = light.ambient + calculateDiffuse(radiance, N, L);
    specular = calculateSpecular(radiance, material, N, L, V);
}