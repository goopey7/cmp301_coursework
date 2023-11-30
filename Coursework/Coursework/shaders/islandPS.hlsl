Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D heightMap : register(t2);
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

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float height : COLOR;
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
    // sample at mip level 0 for max detail
    // grabbing adjacent neighbor heights
    float up = heightMap.SampleLevel(sampler0, texCoord + float2(0, sampleOffset), 0).r;
    float down = heightMap.SampleLevel(sampler0, texCoord + float2(0, -sampleOffset), 0).r;
    float left = heightMap.SampleLevel(sampler0, texCoord + float2(-sampleOffset, 0), 0).r;
    float right = heightMap.SampleLevel(sampler0, texCoord + float2(sampleOffset, 0), 0).r;

    // 2 * sampleOffset seems to yield the best results, the normalization provides variation
    // basically the larger the y value is, the more grass there will be, smaller will be more stone
    return normalize(float3(left - right, 1.5f * sampleOffset, down - up));
}

float4 main(InputType input) : SV_TARGET
{
    // discard the flat edges of the island heightmap
    if (input.height <= 0.6f)
    {
        discard;
    }

    float3 normal = calculateNormal(input.tex, 1.f / 1024.f);
    //return float4(normal, 1.f);

    // use grass for flat parts and stone for steep parts
    float4 grass = texture0.Sample(sampler0, input.tex * texRes);
    float4 stone = texture1.Sample(sampler0, input.tex * texRes);

    // the y value of the normal indicates how steep the terrain is
    float slope = abs(normal.y);

    if (slope < 0.4f)
    {
        slope = 0.f;
    }
    else if (slope < 0.6f)
    {
        // this is to prevent a hard transition between grass and stone
        // so if it's near the transition gradual interpolation happens
        slope = (slope - 0.4f) / 0.2f;
    }

    float4 terrainColor;
    terrainColor = lerp(stone, grass, slope);

    float4 lightColor;
    if (type == 0)
    {
        lightColor = calculateLighting(-lightDirection, normal, diffuseColor) + ambientColor;
    }
    else
    {
        // divide by 2 because I want attenuation to represent the max distance from the center of the light
        float distance = length(lightPosition - input.worldPos) / 2.f;

        // gradual quadratic falloff depending on distance from the light
        float att = pow(saturate(1.0 - pow(distance / attenuation, 2)), 2);
        lightColor = att * calculateLighting(normalize(lightPosition - input.worldPos), normal, diffuseColor);
        lightColor += ambientColor;
    }
    return lightColor * terrainColor;
}
