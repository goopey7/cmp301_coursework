#pragma once
#include "DXF.h"

class UnderbellyShader : public BaseShader
{
public:
	UnderbellyShader(ID3D11Device* device, HWND hwnd);
	~UnderbellyShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* height);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps, const wchar_t* dps);


private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
};
