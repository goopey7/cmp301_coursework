struct InputType
{
    float4 position : SV_POSITION;
    float4 depthPos : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
    float depthValue;
    depthValue = input.depthPos.z / input.depthPos.w;
    return float4(depthValue, depthValue, depthValue, 1.0f);
}