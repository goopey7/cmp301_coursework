#pragma once
#include "DXF.h"
#include "LightManager.h"

class TextureShader : public BaseShader
{
public:
	TextureShader(ID3D11Device* device, HWND hwnd);
	~TextureShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, LightManager* lm);
	void setDepthShaderParamters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps, const wchar_t* dps);


private:
	ID3D11Buffer * matrixBuffer;
	ID3D11Buffer * lightBuffer;
	ID3D11SamplerState* sampleState;
};
