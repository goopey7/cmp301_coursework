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
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float height : COLOR;
};

// Calculate lighting intensity based on direction and normal. Combine with light color.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 color = saturate(diffuse * intensity);
    return color + ambientColor;
}

float4 main(InputType input) : SV_TARGET
{
    // discard the black edges of the island
    if (input.height <= 0.5f)
    {
        discard;
    }

    // TODO calculate normals properly

    float4 grass = texture0.Sample(sampler0, input.tex * texRes);
    float4 stone = texture1.Sample(sampler0, input.tex * texRes);

    float slope = abs(input.normal.y);

    float4 terrainColor;
    terrainColor = lerp(stone, grass, slope);

    float4 lightColor = calculateLighting(-lightDirection, input.normal, diffuseColor);
    return lightColor * terrainColor;
}
