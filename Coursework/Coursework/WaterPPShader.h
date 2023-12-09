#pragma once
#include "DXF.h"

class WaterPPShader : public BaseShader
{
public:
	WaterPPShader(ID3D11Device* device, HWND hwnd);
	~WaterPPShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, float timeElapsed, float frequency, float speed, float displacement, XMFLOAT3 waterTint);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps, const wchar_t* dps);


private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* waterPPBuffer;
	ID3D11SamplerState* sampleState;
};
