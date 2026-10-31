#ifndef LIGHTING_HLSLI
#define LIGHTING_HLSLI

#define MAX_LIGHTS 4

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT       1
#define LIGHT_SPOT        2

struct Light
{
    float3 position;      
    int    type;          

    float3 direction;     
    float  intensity;     

    float3 color;         
    float  innerCutoff;   

    float  outerCutoff;   
    float  range;         
    float  padding0;
    float  padding1;
};

struct LightBlock
{
    float3 cameraPosition;   
    float  shininess;

    float3 ambient;          
    int    lightCount;       

    Light  lights[MAX_LIGHTS];
};

float3 getLightVector(Light lightSource, float3 worldPosition, out float attenuation)
{
    float3 L;
    attenuation = 1.0f;

    if (lightSource.type == LIGHT_DIRECTIONAL)
    {
        L = normalize(-lightSource.direction);
    }
    else
    {
        float3 toLight = lightSource.position - worldPosition;
        L = normalize(toLight);
        
        if (lightSource.range > 0.0f)
        {
            float falloff = saturate(1.0f - length(toLight) / lightSource.range);
            attenuation *= falloff * falloff;
        }

        if (lightSource.type == LIGHT_SPOT)
        {
            float theta = dot(-L, normalize(lightSource.direction));
            
            attenuation *= smoothstep(lightSource.outerCutoff, lightSource.innerCutoff, theta);
        }
    }

    return L;
}

void computeLight(Light lightSource, float3 worldPosition, float3 N, float3 V, float shininess, out float3 diffuse, out float3 specular)
{
    float attenuation;
    float3 L = getLightVector(lightSource, worldPosition, attenuation);
    
    float3 radiance = lightSource.color * lightSource.intensity;
    
    float NdotL = saturate(dot(N, L));
    diffuse = radiance * NdotL * attenuation;
    
    specular = float3(0.0f, 0.0f, 0.0f);
    if (NdotL > 0.0f)
    {
        float3 H = normalize(L + V);
        float NdotH = saturate(dot(N, H));

        specular = pow(NdotH, shininess) * radiance * attenuation;
    }
}

#endif
