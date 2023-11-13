Texture2D heightMap : register(t0);
Texture2D normalMap : register(t1);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
    float time;
    float amplitude;
    float frequency;
    float speed;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float getHeight(float2 uv)
{
    int width, height;
    heightMap.GetDimensions(width, height);

    int3 texCoord = int3(uv * int2(width, height), 0);

    return heightMap.Load(texCoord);
}

OutputType main(InputType input)
{
    OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    float4 pos = input.position;
    pos.y += getHeight(input.tex) * 20;
    output.position = mul(pos, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    int width, height;
    normalMap.GetDimensions(width, height);
    int3 texCoord = int3(input.tex * int2(width, height), 0);
    float3 normalMapSample = normalMap.Load(texCoord);

	// Remap from [0,1] to [-1,1] and normalize
    normalMapSample = normalize(normalMapSample * 2 - 1);

	// Blend with original normal
    float blendFactor = 0.5f;
    output.normal = normalize(normalMapSample * blendFactor + input.normal);

    output.normal = mul(output.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    return output;
}