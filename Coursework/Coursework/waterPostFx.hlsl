Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer WaterPPBuffer : register(b0)
{
    float time;
    float frequency;
    float speed;
    float displacement;
    float3 waterTint;
    float padding0;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float2 uv = input.tex;
    uv.x += sin((uv.y * frequency) + (speed * time)) * displacement;

    float4 color = texture0.Sample(sampler0, uv);
    color.rgb *= waterTint;
    return color;
}
