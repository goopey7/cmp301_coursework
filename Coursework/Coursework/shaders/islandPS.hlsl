Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D heightMap : register(t2);
Texture2DArray depthMapTextures : register(t3);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

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

cbuffer IslandBuffer : register(b1)
{
    float heightMultiplier;
    float texRes;
    float2 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float height : COLOR;
	float3 worldPos : TEXCOORD1;
    float4 depthPos : TEXCOORD2;
    float4 lightViewPos[7] : TEXCOORD3;
};

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 color = saturate(diffuse * intensity);
    return color;
}

float3 calculateNormal(float2 texCoord, float sampleOffset)
{
    // sample at mip level 0 for max detail
    // grabbing adjacent neighbor heights
    float up = heightMap.SampleLevel(sampler0, texCoord + float2(0, sampleOffset), 0).r;
    float down = heightMap.SampleLevel(sampler0, texCoord + float2(0, -sampleOffset), 0).r;
    float left = heightMap.SampleLevel(sampler0, texCoord + float2(-sampleOffset, 0), 0).r;
    float right = heightMap.SampleLevel(sampler0, texCoord + float2(sampleOffset, 0), 0).r;

    // 1.5 * sampleOffset seems to yield the best results, the normalization provides variation
    // basically the larger the y value is, the more grass there will be, smaller will be more stone
    return normalize(float3(left - right, 1.5f * sampleOffset, down - up));
}

bool hasDepthDataInMap(float2 uv)
{
    return uv.x >= 0.f && uv.x <= 1.f && uv.y >= 0.f && uv.y <= 1.f;
}

bool isInShadow(Texture2DArray sMap, uint index, float2 uv, float4 lightViewPosition, float bias)
{
    float depthValue = sMap.Sample(shadowSampler, float3(uv, index)).r;
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
    // discard the flat edges of the island heightmap
    if (input.height <= 0.6f)
    {
        discard;
    }

    float3 normal = calculateNormal(input.tex, 1.f / 1024.f);
    //return float4(normal, 1.f);

    // use grass for flat parts and stone for steep parts
    float4 grass = texture0.Sample(sampler0, input.tex * texRes);
    float4 stone = texture1.Sample(sampler0, input.tex * texRes);

    // the y value of the normal indicates how steep the terrain is
    float slope = abs(normal.y);

    if (slope < 0.4f)
    {
        slope = 0.f;
    }
    else if (slope < 0.6f)
    {
        // this is to prevent a hard transition between grass and stone
        // so if it's near the transition gradual interpolation happens
        slope = (slope - 0.4f) / 0.2f;
    }

    float4 terrainColor;
    terrainColor = lerp(stone, grass, slope);

    float4 pointLightColor = float4(0.f, 0.f, 0.f, 1.f);
    float4 dirLightColor = float4(0.f, 0.f, 0.f, 1.f);

    float shadowMapBias = 0.005f;
    
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
    return (lights[0].ambientColor + (dirLightColor + pointLightColor)) * terrainColor;
}
