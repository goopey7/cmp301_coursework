#include "BoatShader.h"
#include "ShaderBuffers.h"

BoatShader::BoatShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"boatVS.cso", L"boatPS.cso", L"depthPS.cso");
}

BoatShader::~BoatShader()
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

void BoatShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename, const wchar_t* dpsFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC waterBufferDesc;
	D3D11_BUFFER_DESC boatBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
	loadDepthPixelShader(dpsFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShadowMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup the description of the dynamic water constant buffer that is in the vertex shader.
	waterBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waterBufferDesc.ByteWidth = sizeof(WaterBufferType);
	waterBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waterBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waterBufferDesc.MiscFlags = 0;
	waterBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&waterBufferDesc, NULL, &waterBuffer);

	// Setup the description of the dynamic boat constant buffer that is in the vertex shader.
	boatBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	boatBufferDesc.ByteWidth = sizeof(BoatBufferType);
	boatBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	boatBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	boatBufferDesc.MiscFlags = 0;
	boatBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&boatBufferDesc, NULL, &boatBuffer);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER
	// or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType) * 8 + sizeof(uint32_t) * 4;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	HRESULT result = renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create light buffer");
	}

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

void BoatShader::setShaderParameters(
	ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, float time, float gravity, const std::vector<Wave>& waves, XMFLOAT3 boatPivot,
	LightManager* lm
	)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ShadowMatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (ShadowMatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld; // worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	for (size_t i = 0; i < lm->getLights().size(); i++)
	{
		auto light = lm->getLight(i);
		switch (light->getType())
		{
		case LightType::Directional:
			dataPtr->lightViews[light->getShadowMapIndex()] = XMMatrixTranspose(light->getViewMatrix());
			dataPtr->lightProjections[light->getShadowMapIndex()] = XMMatrixTranspose(light->getOrthoMatrix());
			break;
		case LightType::Point:
		{
			int dir = 0;
			for (int j = light->getShadowMapIndex();
				 j < light->getShadowMapIndex() + light->getShadowMapCount(); j++)
			{
				dataPtr->lightViews[j] = XMMatrixTranspose(light->getPointLightViewMatrix(dir));
				dataPtr->lightProjections[j] = XMMatrixTranspose(light->getOrthoMatrix());
				dir++;
			}
			break;
		}
		}
	}

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	WaterBufferType* waterData;
	result = deviceContext->Map(waterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	waterData = (WaterBufferType*)mappedResource.pData;
	waterData->time = time;
	waterData->gravity = gravity;
	waterData->numWaves = min(waves.size(), 8);
	std::copy(waves.begin(), waves.begin() + min(waves.size(), 8), waterData->waves);
	deviceContext->Unmap(waterBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &waterBuffer);

	BoatBufferType* boatData;
	result = deviceContext->Map(boatBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	boatData = (BoatBufferType*)mappedResource.pData;
	boatData->boatPivot = boatPivot;
	deviceContext->Unmap(boatBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &boatBuffer);

	std::vector<LightBufferType> ldata;
	for (auto& light : lm->getLights())
	{
		ldata.push_back(light->getConstBuffer());
	}
	LightsBufferType* lightsPtr;
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightsPtr = (LightsBufferType*)mappedResource.pData;
	std::move(ldata.begin(), ldata.begin() + min(ldata.size(), 8), lightsPtr->lights);
	lightsPtr->lightCount = min(ldata.size(), 8);
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	ID3D11ShaderResourceView* sMaps = lm->getDepthMapSRV();
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &sMaps);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}

void BoatShader::setDepthShaderParamters(ID3D11DeviceContext* deviceContext,
										  const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
										  const XMMATRIX& projectionMatrix, float time, float gravity, const std::vector<Wave>& waves, XMFLOAT3 boatPivot)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld; // worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	WaterBufferType* waterData;
	result = deviceContext->Map(waterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	waterData = (WaterBufferType*)mappedResource.pData;
	waterData->time = time;
	waterData->gravity = gravity;
	waterData->numWaves = min(waves.size(), 8);
	std::copy(waves.begin(), waves.begin() + min(waves.size(), 8), waterData->waves);
	deviceContext->Unmap(waterBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &waterBuffer);
}
