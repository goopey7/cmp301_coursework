static const float PI = 3.14159265359f;

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
    float amp;
    float waveLength;
    float speed;
}

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
    float height : COLOR;
	float3 worldPos : TEXCOORD1;
    float4 depthPos : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
    float3 worldNormal : TEXCOORD4;
};

float getHeight(float2 uv)
{
    int width, height;
    heightMap.GetDimensions(width, height);

    uv = frac(uv);
    uv -= floor(uv);

    int3 texCoord = int3(uv * int2(width, height), 0);

    return heightMap.Load(texCoord);
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

    //texCoord = 25.f * texCoord + float2(-time * speed, -time * speed);
    //float height = getHeight(texCoord) * amp;
    //vertexPosition.y += height;

    // https://catlikecoding.com/unity/tutorials/flow/waves/
    float k = 2 * PI / waveLength;
    float f = k * (vertexPosition.x - time * speed);
    vertexPosition.y = amp * sin(f);

    float3 tangent = normalize(float3(1, k * amp * cos(f), 0));
    float3 normal = float3(-tangent.y, tangent.x, 0);

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.worldPos = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.tex = texCoord;

    output.depthPos = output.position;

    output.worldNormal = normal;
    output.worldNormal = mul(float4(output.worldNormal, 1.f), worldMatrix).xyz;
    //output.worldTangent = float3(1, 0, 0);
    //output.worldBitangent = float3(0, 0, 1);

    //output.worldNormal = normalize(mul(float4(output.worldNormal, 1.f), worldMatrix));
    //output.worldTangent = normalize(mul(float4(output.worldTangent, 1.f), worldMatrix));
    //output.worldBitangent = normalize(mul(float4(output.worldBitangent, 1.f), worldMatrix));

    return output;
}

