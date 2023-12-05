#pragma once

#include "LightBase.h"

using namespace std;
using namespace DirectX;

class IslandShader : public BaseShader
{
private:

public:
	IslandShader(ID3D11Device* device, HWND hwnd);
	~IslandShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture0, ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView * heightMap, const std::vector<LightBase*>& lights, float* edges, float* inside, float texRes, float height);
	void setDepthShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix,
								  const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
								  ID3D11ShaderResourceView* heightMap,
								  float* edges,
								  float* inside, float height);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds, const wchar_t* ps, const wchar_t* dps);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* tesBuffer;
	ID3D11Buffer* texResBuffer;
};

