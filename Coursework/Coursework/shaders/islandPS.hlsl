Texture2D texture0 : register(t0);
Texture2D normalMap : register(t1);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float height : COLOR;
};

// Calculate lighting intensity based on direction and normal. Combine with light color.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 color = saturate(diffuse * intensity);
    return color;
}

float4 main(InputType input) : SV_TARGET
{
    // discard the black edges of the island
    if (input.height <= 0.5f)
    {
        discard;
    }

    float4 textureColor;
    float4 lightColor;

    float3 normal = normalMap.Sample(sampler0, input.tex).rgb;

    // normal map encoding is [0,1] but we're using [-1, 1]
    // so 2(norm) - 1 should do the trick
    normal = normalize(normal * 2 - 1);

	// Sample the texture. Calculate light intensity and color, return light*texture for final pixel color.
    textureColor = texture0.Sample(sampler0, input.tex);
    lightColor = calculateLighting(-lightDirection, normal, diffuseColor);
    return lightColor * textureColor;
	//return textureColor;
}
