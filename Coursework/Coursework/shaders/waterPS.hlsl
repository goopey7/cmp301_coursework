Texture2D diffuse : register(t0);
Texture2D normalMap : register(t1);
SamplerState sampler0 : register(s0);

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

cbuffer TimeBuffer : register(b1)
{
    float time;
    float amp;
    float freq;
    float speed;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 worldNormal : TEXCOORD2;
    float3 worldTangent : TEXCOORD3;
    float3 worldBitangent : TEXCOORD4;
};

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 color = saturate(diffuse * intensity);
    return color;
}

float3 calculateNormal(float2 texCoord, float3 worldNormal, float3 worldTangent, float3 worldBitangent)
{
    float3 normSample = 2.f * normalize(normalMap.Sample(sampler0, texCoord).xyz) - 1.f;
    return normalize(worldTangent * normSample.r + worldBitangent * normSample.g + worldNormal * normSample.b);
}

float4 main(InputType input) : SV_TARGET
{
    float2 texCoord = input.tex;
    //float2 texCoord = input.tex * 25.f;
    float3 normal = calculateNormal(texCoord, input.worldNormal, input.worldTangent, input.worldBitangent);
    //return float4(normal * 0.5f + 0.5f, 1.f);

    float4 waterColor = diffuse.Sample(sampler0, texCoord);
    float4 lightColor;
    if (type == 0) // direct light
    {
        lightColor = calculateLighting(-lightDirection, normal, diffuseColor) + ambientColor;
    }
    else // apply attenuation to point light
    {
        // divide by 2 because I prefer attenuation value to represent the max distance from the center of the light
        //float distance = length(lightPosition - input.worldPos) / 2.f;

        // gradual quadratic falloff depending on distance from the light
        //float att = pow(saturate(1.0 - pow(distance / attenuation, 2)), 2);
        lightColor = calculateLighting(normalize(lightPosition - input.worldPos), normal, diffuseColor);
        lightColor += ambientColor;
    }
    return lightColor * waterColor;
}
