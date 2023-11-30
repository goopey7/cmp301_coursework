#pragma once
#include "DXF.h"

class ColorShader : public BaseShader
{
public:
	ColorShader(ID3D11Device* device, HWND hwnd);
	~ColorShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);


private:
	ID3D11Buffer * matrixBuffer;
};
