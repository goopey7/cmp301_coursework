Texture2D heightMap : register(t0);
Texture2D normalMap : register(t1);

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;
	output.position = input.position;
	output.tex = input.tex;
	output.normal = input.normal;
    return output;
}
