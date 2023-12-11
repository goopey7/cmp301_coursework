#pragma once
#include "DXF.h"
#include "ShaderBuffers.h"

class BoatShader : public BaseShader
{
public:
	BoatShader(ID3D11Device* device, HWND hwnd);
	~BoatShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, float time, float gravity, const std::vector<Wave>& waves, XMFLOAT3 boatPivot);
	void setDepthShaderParamters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix,  float time, float gravity, const std::vector<Wave>& waves, XMFLOAT3 boatPivot);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps, const wchar_t* dps);


private:
	ID3D11Buffer * matrixBuffer;
	ID3D11Buffer* waterBuffer;
	ID3D11Buffer* boatBuffer;
	ID3D11SamplerState* sampleState;
};
