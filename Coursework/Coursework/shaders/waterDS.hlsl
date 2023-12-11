static const float PI = 3.14159265359f;

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[26];
    matrix lightProjectionMatrix[26];
};

struct Wave
{
    float steepness;
    float length;
    float2 direction;
};

cbuffer WaterBuffer : register(b1)
{
    Wave waves[8];
    uint numWaves;
    float time;
    float gravity;
    float padding0;
}

cbuffer CamBuffer : register(b2)
{
    float3 cameraPos;
    float padding1;
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
    float3 worldNormal : TEXCOORD3;
    float3 viewVector : TEXCOORD4;
    float4 lightViewPos[26] : TEXCOORD5;
};

float3 gerstnerWave(Wave wave, float3 pos, inout float3 tangent, inout float3 binormal)
{
    // https://catlikecoding.com/unity/tutorials/flow/waves/
    float k = 2 * PI / wave.length;
    float c = sqrt(gravity / k);
    float2 d = normalize(wave.direction);
    float f = k * (dot(d, pos.xz) - time * c);
    float a = wave.steepness / k;

    tangent += normalize(float3(
        1 - d.x * d.x * (wave.steepness * sin(f)),
        d.x * (wave.steepness * cos(f)),
        -d.x * d.y * (wave.steepness * sin(f))
    ));

    binormal += normalize(float3(
        -d.x * d.y * (wave.steepness * sin(f)),
        d.y * (wave.steepness * cos(f)),
        1 - d.y * d.y * (wave.steepness * sin(f))
    ));

    return float3(
        d.x * (a * cos(f)),
        a * sin(f),
        d.y * (a * cos(f))
    );
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

    float3 tangent = float3(0.f, 0.f, 0.f);
    float3 binormal = float3(0.f, 0.f, 0.f);
    float3 p = vertexPosition.xyz;

    for (uint i = 0; i < numWaves; i++)
    {
        Wave wave = waves[i];
        p += gerstnerWave(wave, vertexPosition, tangent, binormal);
    }

    float3 normal = normalize(cross(binormal, tangent));
    vertexPosition = p;

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.worldPos = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.tex = texCoord;

    output.depthPos = output.position;

    output.worldNormal = mul(float4(normal, 1.f), worldMatrix).xyz;

    for (uint j = 0; j < 26; j++)
    {
        output.lightViewPos[j] = mul(float4(vertexPosition, 1.f), worldMatrix);
        output.lightViewPos[j] = mul(output.lightViewPos[j], lightViewMatrix[j]);
        output.lightViewPos[j] = mul(output.lightViewPos[j], lightProjectionMatrix[j]);
    }

    //output.viewVector = normalize(cameraPos - output.worldPos);

    return output;
}

