Texture2D textureColor : register(t0);
Texture2D textureNormal : register(t1);
SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    return textureColor.Sample(sampler0, input.tex);
}
