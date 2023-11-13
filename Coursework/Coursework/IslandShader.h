#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class IslandShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
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

public:
	IslandShader(ID3D11Device* device, HWND hwnd);
	~IslandShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView * heightMap, ID3D11ShaderResourceView* normalMap, float time, float amp, float freq, float speed, Light* light, float* edges, float* inside);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds, const wchar_t* ps);


private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* tesBuffer;
};

