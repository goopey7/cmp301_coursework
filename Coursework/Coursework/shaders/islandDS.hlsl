Texture2D heightMap : register(t0);

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

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float height : COLOR;
};

float getHeight(float2 uv)
{
    int width, height;
    heightMap.GetDimensions(width, height);

    int3 texCoord = int3(uv * int2(width, height), 0);

    return heightMap.Load(texCoord);
}

float3 getNormal(float2 uv)
{
    float delta = 0.01f; // Adjust this value as needed for your specific height map

    // Calculate the height at the current UV coordinates
    float center = getHeight(uv);
    
    // Calculate the heights at neighboring texels
    float left = getHeight(uv - float2(delta, 0));
    float right = getHeight(uv + float2(delta, 0));
    float up = getHeight(uv + float2(0, delta));
    float down = getHeight(uv - float2(0, delta));

    // Calculate the partial derivatives of the height map in the x and y directions
    float3 dx = float3(delta, 0, right - left);
    float3 dy = float3(0, delta, up - down);

    // Calculate the cross product of the partial derivatives to obtain the normal
    float3 normal = normalize(cross(dx, dy));

    return normal;
}

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    OutputType output;
 
    float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);

    float2 uv1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 uv2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    float2 texCoord = lerp(uv1, uv2, uvwCoord.x);

    float height = getHeight(texCoord) * amplitude;
    vertexPosition.y += height;

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.tex = texCoord;
    output.height = height;

    float3 recalculatedNormal = getNormal(texCoord);

    output.normal = recalculatedNormal;

    return output;
}

