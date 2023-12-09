Texture2D shadowMapTexture : register(t0);
SamplerState shadowSampler : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float height : COLOR;
	float3 worldPos : TEXCOORD1;
    float4 depthPos : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
    float3 worldNormal : TEXCOORD4;
    float3 viewVector : TEXCOORD5;
};

struct Light
{
    float4 diffuseColor;
    float3 lightDirection;
    uint type;
    float4 ambientColor;
    float3 lightPosition;
    float attenuation;
};

cbuffer LightBuffer : register(b0)
{
    Light lights[8];
    uint numLights;
    float3 padding0;
};

float4 calculateSpecularLighting(float3 lightDirection, float3 normal, float3 viewVector)
{
    float4 specularColor = float4(1.f, 1.f, 1.f, 1.f);
    float specularPower = 32.f;

    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColor * specularIntensity);
}

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 color = saturate(diffuse * intensity);
    return color;
}

bool hasDepthDataInMap(float2 uv)
{
    //return uv.x >= 0.f && uv.x <= 1.f && uv.y >= 0.f && uv.y <= 1.f;
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    float depthValue = sMap.Sample(shadowSampler, uv).r;
    float lightDepthValue = min(lightViewPosition.z / lightViewPosition.w, 1.f);
    lightDepthValue -= bias;

    if (lightDepthValue <= depthValue)
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
    //return float4(input.worldNormal, 1.f);
    float4 waterColor = float4(0.f, 0.f, 1.f, 1.f);
    float4 pointLightColor = float4(0.f, 0.f, 0.f, 1.f);
    float4 dirLightColor = float4(0.f, 0.f, 0.f, 1.f);

    float shadowMapBias = 0.005f;
    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);
    for (uint i = 0; i < numLights; i++)
    {
        Light light = lights[i];
        if (light.type == 0) // directional lights
        {
            if (hasDepthDataInMap(pTexCoord))
            {
                if (!isInShadow(shadowMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
                {
                    dirLightColor += calculateSpecularLighting(-light.lightDirection, input.worldNormal, input.viewVector);
                    dirLightColor += calculateLighting(-light.lightDirection, input.worldNormal, light.diffuseColor);
                }
            }
            else  // not in shadowmap, but we still want lighting
            {
                dirLightColor += calculateSpecularLighting(-light.lightDirection, input.worldNormal, input.viewVector);
                dirLightColor += calculateLighting(-light.lightDirection, input.worldNormal, light.diffuseColor);
            }
        }
        else // point lights
        {
            // divide by 2 because I want attenuation to represent the max distance from the center of the light
            float distance = length(light.lightPosition - input.worldPos) / 2.f;

            // gradual quadratic falloff depending on distance from the light
            float att = pow(saturate(1.0 - pow(distance / 5.f, 2)), 2);
            pointLightColor += att * calculateLighting(normalize(light.lightPosition - input.worldPos), input.worldNormal, light.diffuseColor);
        }
    }
    return (lights[0].ambientColor + (dirLightColor + pointLightColor)) * waterColor;
}
