#include "WaterPPShader.h"
#include "ShaderBuffers.h"

WaterPPShader::WaterPPShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"simpleVS.cso", L"waterPostFx.cso", L"depthPS.cso");
}

WaterPPShader::~WaterPPShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release base shader components
	BaseShader::~BaseShader();
}

void WaterPPShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename, const wchar_t* dpsFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC waterPPBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
	loadDepthPixelShader(dpsFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	waterPPBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waterPPBufferDesc.ByteWidth = sizeof(WaterPPBufferType);
	waterPPBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waterPPBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waterPPBufferDesc.MiscFlags = 0;
	waterPPBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&waterPPBufferDesc, NULL, &waterPPBuffer);

	D3D11_SAMPLER_DESC samplerDesc;
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}

void WaterPPShader::setShaderParameters(ID3D11DeviceContext* deviceContext,
										const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
										const XMMATRIX& projectionMatrix,
										ID3D11ShaderResourceView* texture, float timeElapsed,
										float frequency, float speed, float displacement,
										XMFLOAT3 waterTint, float weights[5], float blurAmount,
										XMFLOAT2 screenSize)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	MatrixBufferType* dataPtr;
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld; // worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	WaterPPBufferType* waterDataPtr;
	result = deviceContext->Map(waterPPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	waterDataPtr = (WaterPPBufferType*)mappedResource.pData;
	waterDataPtr->time = timeElapsed;
	waterDataPtr->frequency = frequency;
	waterDataPtr->speed = speed;
	waterDataPtr->displacement = displacement;
	waterDataPtr->waterTint = waterTint;
	waterDataPtr->weight0 = weights[0];
	waterDataPtr->weight1 = weights[1];
	waterDataPtr->weight2 = weights[2];
	waterDataPtr->weight3 = weights[3];
	waterDataPtr->weight4 = weights[4];
	waterDataPtr->blurAmount = blurAmount;
	waterDataPtr->screenSize = screenSize;
	deviceContext->Unmap(waterPPBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &waterPPBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}
