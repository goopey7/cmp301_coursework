Texture2D texture0 : register(t0);
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
    float3 normal : NORMAL;
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
    if (input.height <= 0.5f)
    {
        discard;
    }
    float4 textureColor;
    float4 lightColor;

	// Sample the texture. Calculate light intensity and color, return light*texture for final pixel color.
    textureColor = texture0.Sample(sampler0, input.tex);
    //lightColor = calculateLighting(-lightDirection, input.normal, diffuseColor);
    //return lightColor * textureColor;
	return textureColor;
}
