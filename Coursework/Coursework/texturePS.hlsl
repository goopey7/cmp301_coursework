Texture2D texture0 : register(t0);
Texture2DArray depthMapTextures : register(t1);
SamplerState sampler0 : register(s0);

struct Light
{
    float4 diffuseColor;
    float3 lightDirection;
    uint type;
    float4 ambientColor;
    float3 lightPosition;
    float attenuation;
    uint shadowMapStart;
    uint shadowMapEnd;
    float2 padding;
};

cbuffer LightBuffer : register(b0)
{
    Light lights[8];
    uint numLights;
    float3 padding0;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float4 depthPos : TEXCOORD2;
    float4 lightViewPos[26] : TEXCOORD3;
};

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 color = saturate(diffuse * intensity);
    return color;
}

bool hasDepthDataInMap(float2 uv)
{
    return uv.x >= 0.f && uv.x <= 1.f && uv.y >= 0.f && uv.y <= 1.f;
}

bool isInShadow(Texture2DArray sMap, uint index, float2 uv, float4 lightViewPosition, float bias)
{
    float depthValue = sMap.Sample(sampler0, float3(uv, index)).r;
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
    float4 pointLightColor = float4(0.f, 0.f, 0.f, 1.f);
    float4 dirLightColor = float4(0.f, 0.f, 0.f, 1.f);

    float shadowMapBias = 0.005f;

    float4 textureColor = texture0.Sample(sampler0, input.tex);
    float3 normal = float3(0.f, 1.f, 0.f);

    for (uint i = 0; i < numLights; i++)
    {
        Light light = lights[i];
        if (light.type == 0) // directional light
        {
            float2 pTexCoord = getProjectiveCoords(input.lightViewPos[light.shadowMapStart]);
            if (hasDepthDataInMap(pTexCoord))
            {
                if (!isInShadow(depthMapTextures, light.shadowMapStart, pTexCoord, input.lightViewPos[light.shadowMapStart], shadowMapBias))
                {
                    dirLightColor += calculateLighting(-light.lightDirection, normal, light.diffuseColor);
                }
            }
            else
            {
                dirLightColor += calculateLighting(-light.lightDirection, normal, light.diffuseColor);
            }
        }
        else // point light
        {
            bool inShadow = false;
            for (uint j = light.shadowMapStart; j < light.shadowMapEnd; j++)
            {
                float2 pTexCoord = getProjectiveCoords(input.lightViewPos[j]);
                if (hasDepthDataInMap(pTexCoord))
                {
                    if (isInShadow(depthMapTextures, j, pTexCoord, input.lightViewPos[j], shadowMapBias))
                    {
                        inShadow = true;
                    }
                }
            }
            if (!inShadow)
            {
                // divide by 2 because I want attenuation to represent the max distance from the center of the light
                float distance = length(light.lightPosition - input.worldPos) / 2.f;

                // gradual quadratic falloff depending on distance from the light
                float att = pow(saturate(1.0 - pow(distance / 5.f, 2)), 2);
                pointLightColor += att * calculateLighting(normalize(light.lightPosition - input.worldPos), normal, light.diffuseColor);
            }
        }
    }
    return (lights[0].ambientColor + (dirLightColor + pointLightColor)) * textureColor;
    
}
