struct InputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float height : COLOR;
	float3 worldPos : TEXCOORD1;
    float4 depthPos : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
    float depthValue;
    depthValue = input.depthPos.z / input.depthPos.w;
    //return float4(depthValue, depthValue, depthValue, 1.0f);
    return float4(1.f, 1.f, 0.f, 1.0f);
}