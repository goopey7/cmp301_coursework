Texture2D texture0 : register(t0);
Texture2D heightMap : register(t1);
SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 heightMapColor = heightMap.Sample(sampler0, input.tex);
    if (heightMapColor.r * 45.f < 0.6f)
    {
        discard;
    }
    return texture0.Sample(sampler0, input.tex);
}
