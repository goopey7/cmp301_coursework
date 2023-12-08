struct InputType
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD1;
    float3 worldNorm : TEXCOORD2;
};

struct Light
{
    float4 diffuseColor;
    float3 lightDirection;
    uint type;
    float4 ambientColor;
    float3 lightPosition;
    float attenuation;
};

cbuffer LightBuffer : register(b0)
{
    Light lights[8];
    uint numLights;
    float3 padding0;
};

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 color = saturate(diffuse * intensity);
    return color;
}

float4 main(InputType input) : SV_TARGET
{
    float3 norm = float3(0.f, 1.f, 0.f);
    //return float4(norm, 1.f);
    float4 waterColor = float4(0.f, 0.f, 1.f, 1.f);
    float4 pointLightColor = float4(0.f, 0.f, 0.f, 1.f);
    float4 dirLightColor = float4(0.f, 0.f, 0.f, 1.f);
    for (uint i = 0; i < numLights; i++)
    {
        Light light = lights[i];
        if (light.type == 0)
        {
            dirLightColor += calculateLighting(-light.lightDirection, norm, light.diffuseColor);
        }
        else
        {
            // divide by 2 because I want attenuation to represent the max distance from the center of the light
            float distance = length(light.lightPosition - input.worldPos) / 2.f;

            // gradual quadratic falloff depending on distance from the light
            float att = pow(saturate(1.0 - pow(distance / 5.f, 2)), 2);
            pointLightColor += att * calculateLighting(normalize(light.lightPosition - input.worldPos), norm, light.diffuseColor);
        }
    }
    return (lights[0].ambientColor + (dirLightColor + pointLightColor)) * waterColor;
}
