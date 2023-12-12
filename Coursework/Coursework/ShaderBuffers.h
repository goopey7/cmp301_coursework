#pragma once

#include "DXF.h"
#include "ShadowMapArray.h"
using namespace std;
using namespace DirectX;

struct ShadowMatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX lightViews[MAX_SHADOW_MAPS];
	XMMATRIX lightProjections[MAX_SHADOW_MAPS];
};

struct LightBufferType
{
	XMFLOAT4 diffuse;
	XMFLOAT3 direction;
	uint32_t lightType;
	XMFLOAT4 ambient;
	XMFLOAT3 position;
	float attenuation;
	uint32_t shadowMapStart;
	uint32_t shadowMapEnd;
	XMFLOAT2 padding;
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
	float weight0;
	float weight1;
	float weight2;
	float weight3;
	float weight4;
	float blurAmount;
	XMFLOAT2 screenSize;
	float padding;
};

struct BoatBufferType
{
	XMFLOAT3 boatPivot;
	float padding;
};

struct ColorBufferType
{
	XMFLOAT4 color;
};

