cbuffer LightBuffer : register(b0)
{
    float4 diffuseColor;
    float3 lightDirection;
    float texRes;
    float4 ambientColor;
    float3 lightPosition;
    uint1 type;
    float attenuation;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 color = saturate(diffuse * intensity);
    return color;
}

float3 calculateNormal(float2 texCoord, float sampleOffset)
{
    return float3(0, 1, 0);
}

float4 main(InputType input) : SV_TARGET
{
    float3 normal = calculateNormal(input.tex, 1.f / 1024.f);
    //return float4(normal, 1.f);

    float4 waterColor = float4(0.1f, 0.1f, 0.8f, 1.f);
    float4 lightColor;
    if (type == 0) // direct light
    {
        lightColor = calculateLighting(-lightDirection, normal, diffuseColor) + ambientColor;
    }
    else // apply attenuation to point light
    {
        // divide by 2 because I prefer attenuation value to represent the max distance from the center of the light
        float distance = length(lightPosition - input.worldPos) / 2.f;

        // gradual quadratic falloff depending on distance from the light
        float att = pow(saturate(1.0 - pow(distance / attenuation, 2)), 2);
        lightColor = att * calculateLighting(normalize(lightPosition - input.worldPos), normal, diffuseColor);
        lightColor += ambientColor;
    }
    return lightColor * waterColor;
}
