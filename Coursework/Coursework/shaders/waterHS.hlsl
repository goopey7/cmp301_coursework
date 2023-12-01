cbuffer TesValues : register(b0)
{
    float4 edges;
    float2 inside;
    float2 padding0;
};

struct InputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    output.edges[0] = clamp(edges[0], 1, 64);
    output.edges[1] = clamp(edges[1], 1, 64);
    output.edges[2] = clamp(edges[2], 1, 64);
    output.edges[3] = clamp(edges[3], 1, 64);

    output.inside[0] = clamp(inside[0], 1, 64);
    output.inside[1] = clamp(inside[1], 1, 64);

    return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    output.position = patch[pointId].position;
    output.tex = patch[pointId].tex;

    return output;
}
