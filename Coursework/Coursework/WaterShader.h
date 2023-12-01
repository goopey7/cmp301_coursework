#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaterShader : public BaseShader
{
  private:
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

  public:
	WaterShader(ID3D11Device* device, HWND hwnd);
	~WaterShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix,
							 const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float* edges, float* inside);

  private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds, const wchar_t* ps);

  private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* tesBuffer;
};
