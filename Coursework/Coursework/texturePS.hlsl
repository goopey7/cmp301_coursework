Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, input.tex);
    color.rgb *= float3(1.f, 1.f, 1.f);
    return color;
}
