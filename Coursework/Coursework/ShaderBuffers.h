#pragma once

#include "DXF.h"
using namespace std;
using namespace DirectX;

struct LightBufferType
{
	XMFLOAT4 diffuse;
	XMFLOAT3 direction;
	float texRes;
	XMFLOAT4 ambient;
	XMFLOAT3 position;
	uint32_t lightType;
	float attenuation;
	XMFLOAT3 padding;
};

struct TimeBufferType
{
	float time;
	float amplitude;
	float frequency;
	float speed;
};

struct TesType
{
	XMFLOAT4 edges;
	XMFLOAT2 inside;
	XMFLOAT2 padding;
};
