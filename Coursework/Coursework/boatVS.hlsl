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

cbuffer BoatBuffer : register(b2)
{
    float3 boatPivot;
    float padding1;
}

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
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL;
    float4 lightViewPos[26] : TEXCOORD3;
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

OutputType main(InputType input)
{
    OutputType output;

    // TODO MAKE BOAT PIVOT A CONSTANT BUFFER
    float3 boatPivot = float3(0.f, 0.f, 0.f);
    float3 tangent = float3(0.f, 0.f, 0.f);
    float3 binormal = float3(0.f, 0.f, 0.f);
    float height = 0.f;
    for (uint i = 0; i < numWaves; i++)
    {
        Wave wave = waves[i];
        height += gerstnerWave(wave, boatPivot, tangent, binormal).y;
    }

    float3 normal = normalize(cross(binormal, tangent));

    input.position.y += height;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.worldPos = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;
    output.normal = input.normal;

    for (uint i = 0; i < 26; i++)
    {
        output.lightViewPos[i] = mul(float4(input.position.xyz, 1.f), worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
    }

    return output;
}