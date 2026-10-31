#ifndef LIGHTING_HLSLI
#define LIGHTING_HLSLI

// light types, Light::type holds one of these numbers
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT       1
#define LIGHT_SPOT        2

// every vec3 is followed by a 4 byte value so each pair fills exactly one 16 byte register
// the c++ Light struct must have the same members in the same order (64 bytes total)
struct Light
{
    float3 position;      // world space position (point and spot)
    int    type;          // LIGHT_DIRECTIONAL, LIGHT_POINT or LIGHT_SPOT

    float3 direction;     // direction the light travels (directional and spot)
    float  intensity;     // scales the light color

    float3 color;         // light color
    float  innerCutoff;   // spot: cosine of the inner cone angle (fully lit inside)

    float3 ambient;       // ambient contribution, not affected by attenuation
    float  outerCutoff;   // spot: cosine of the outer cone angle (unlit outside), must be less than innerCutoff
};

// returns the unit vector from the surface toward the light
// attenuation is 1 when the surface is fully lit and 0 when it receives no light from this light
float3 getLightVector(Light lightSource, float3 worldPosition, out float attenuation)
{
    float3 L;
    attenuation = 1.0f;

    if (lightSource.type == LIGHT_DIRECTIONAL)
    {
        // direction is the way the light travels, so the vector toward the light is the negative
        // directional lights don't lose intensity with distance, attenuation stays 1
        L = normalize(-lightSource.direction);
    }
    else
    {
        // point and spot: from the surface toward the light position
        L = normalize(lightSource.position - worldPosition);

        if (lightSource.type == LIGHT_SPOT)
        {
            // -L points from the light toward the surface
            // theta is the cosine of the angle between the spotlight axis and that direction
            float theta = dot(-L, normalize(lightSource.direction));

            // cosines get smaller as the angle gets larger, so outer < inner
            // smoothstep gives 0 outside the outer cone, 1 inside the inner cone and a soft blend between
            attenuation = smoothstep(lightSource.outerCutoff, lightSource.innerCutoff, theta);
        }
    }

    return L;
}

#endif
