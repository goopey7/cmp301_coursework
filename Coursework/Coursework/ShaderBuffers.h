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

struct Wave
{
	float steepness;
	float length;
	XMFLOAT2 direction;
};

struct WaterBufferType
{
	Wave waves[8];
	uint32_t numWaves;
	float time;
	float gravity;
	float padding;
};

struct TesType
{
	XMFLOAT4 edges;
	XMFLOAT2 inside;
	XMFLOAT2 padding;
};

struct IslandBufferType
{
	float height;
	float texRes;
	XMFLOAT2 padding;
};

struct CameraBufferType
{
	XMFLOAT3 cameraPosition;
	float padding;
};

struct WaterPPBufferType
{
	float time;
	float frequency;
	float speed;
	float displacement;
	XMFLOAT3 waterTint;
	float padding;
};

