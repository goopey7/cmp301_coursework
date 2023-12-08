#pragma once

#include "LightBase.h"
using namespace std;
using namespace DirectX;

class WaterShader : public BaseShader
{
  public:
	WaterShader(ID3D11Device* device, HWND hwnd);
	~WaterShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix,
							 const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
							 float* edges, float* inside, float time, float gravity, float steepness, float wavelength, XMFLOAT2 direction,
		ID3D11ShaderResourceView* height,
		const std::vector<LightBase*>& lights, ID3D11ShaderResourceView* shadowMap
		);

  private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds, const wchar_t* ps, const wchar_t* dps);

  private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* tesBuffer;
};
