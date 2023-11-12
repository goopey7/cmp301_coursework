// Tessellation Hull Shader
// Prepares control points for tessellation

cbuffer TesValues : register(b0)
{
    float4 maxEdges;
    float2 maxInside;
    float2 padding0;
};

cbuffer CamBuffer : register(b1)
{
    float3 camPos;
    float padding1;
};

cbuffer MatrixBuffer : register(b2)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    float4 patchWorldPos = mul(float4(inputPatch[patchId].position, 1.f), worldMatrix);
    float4 cameraWorldPos = mul(float4(camPos, 1.f), worldMatrix);

    float dist = length(patchWorldPos - cameraWorldPos) * 10;

    // Set the tessellation factors for the three maxEdges of the triangle.
    output.edges[0] = clamp(maxEdges[0] - dist, 1, 64);
    output.edges[1] = clamp(maxEdges[1] - dist, 1, 64);
    output.edges[2] = clamp(maxEdges[2] - dist, 1, 64);
    output.edges[3] = clamp(maxEdges[3] - dist, 1, 64);

    // Set the tessellation factor for tessallating Inside the triangle.
    output.inside[0] = clamp(maxInside[0] - dist, 1, 64);
    output.inside[1] = clamp(maxInside[1] - dist, 1, 64);

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


    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input colour as the output colour.
    output.colour = patch[pointId].colour;

    return output;
}