Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer WaterPPBuffer : register(b0)
{
    float time;
    float frequency;
    float speed;
    float displacement;
    float3 waterTint;
    float weight0;
    float weight1;
    float weight2;
    float weight3;
    float weight4;
    float blurAmount;
    float2 screenSize;
    float padding;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float2 uvDistort(float2 uvIn)
{
    float2 uv = uvIn;
    uv.x += sin((uv.y * frequency) + (speed * time)) * displacement;
    return uv;
}

float4 blur(float2 uv)
{
    float weights[5] = { weight0, weight1, weight2, weight3, weight4};
    float4 color = float4(0.f, 0.f, 0.f, 1.f);

    // I get weird results using the exact screen size
    float2 texelSize = float2(blurAmount / (screenSize.x + .001f), blurAmount / (screenSize.y + 0.001f));

    for (float i = 1.f; i < 5.f; i++)
    {
        color += texture0.Sample(sampler0, uv + float2(texelSize.x * -i, texelSize.y * -i)) * weights[i];
        color += texture0.Sample(sampler0, uv + float2(texelSize.x * i, texelSize.y * i)) * weights[i];
    }

    color += texture0.Sample(sampler0, uv) * weights[0];
    return color;
}

float4 main(InputType input) : SV_TARGET
{
    float2 uv = uvDistort(input.tex);
    float4 color = blur(uv);
    color.rgb *= waterTint;
    return color;
}
