#pragma once

#include "DXF.h"
using namespace std;
using namespace DirectX;

struct ShadowMatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX lightView;
	XMMATRIX lightProjection;
};

struct LightBufferType
{
	XMFLOAT4 diffuse;
	XMFLOAT3 direction;
	uint32_t lightType;
	XMFLOAT4 ambient;
	XMFLOAT3 position;
	float attenuation;
};

struct LightsBufferType
{
	LightBufferType lights[8];
	uint32_t lightCount;
	XMUINT3 padding;
};

struct WaterBufferType
{
	float time;
	float steepness;
	float waveLength;
	float gravity;
	XMFLOAT2 direction;
	XMFLOAT2 padding;
};

struct TesType
{
	XMFLOAT4 edges;
	XMFLOAT2 inside;
	XMFLOAT2 padding;
};

struct TexResBufferType
{
	float texRes;
	XMFLOAT3 padding;
};
