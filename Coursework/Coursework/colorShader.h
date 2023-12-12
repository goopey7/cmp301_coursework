#pragma once
#include "DXF.h"
#include "LightManager.h"

class ColorShader : public BaseShader
{
public:
	ColorShader(ID3D11Device* device, HWND hwnd);
	~ColorShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, LightManager* lm, XMFLOAT4 color);
	void setDepthShaderParamters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps, const wchar_t* dps);


private:
	ID3D11Buffer * matrixBuffer;
	ID3D11Buffer * colorBuffer;
};
