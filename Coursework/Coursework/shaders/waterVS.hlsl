Texture2D heightMap : register(t0);
Texture2D normalMap : register(t1);

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

OutputType main(InputType input)
{
    OutputType output;
	output.position = input.position;
	output.tex = input.tex;
    return output;
}
