#include "IslandShader.h"
#include "ShaderBuffers.h"
#include <array>

IslandShader::IslandShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"islandVS.cso", L"islandHS.cso", L"islandDS.cso", L"islandPS.cso", L"depthPS.cso");
}

IslandShader::~IslandShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the matrix constant buffer.
	if (islandBuffer)
	{
		islandBuffer->Release();
		islandBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	// Release base shader components
	BaseShader::~BaseShader();
}

void IslandShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC tesBufferDesc;
	D3D11_BUFFER_DESC islandBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShadowMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	tesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tesBufferDesc.ByteWidth = sizeof(TesType);
	tesBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tesBufferDesc.MiscFlags = 0;
	tesBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tesBufferDesc, NULL, &tesBuffer);

	islandBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	islandBufferDesc.ByteWidth = sizeof(IslandBufferType);
	islandBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	islandBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	islandBufferDesc.MiscFlags = 0;
	islandBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&islandBufferDesc, NULL, &islandBuffer);

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

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

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
}

void IslandShader::initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds,
							  const wchar_t* ps, const wchar_t* dps)
{
	initShader(vs, ps);

	loadHullShader(hs);
	loadDomainShader(ds);
	loadDepthPixelShader(dps);
}

void IslandShader::setShaderParameters(
	ID3D11Device* device, ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture0, ID3D11ShaderResourceView* texture1,
	ID3D11ShaderResourceView* heightMap,
	LightManager* lm, float* edges, float* inside, float texRes, float height)
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
	dataPtr->lightView = XMMatrixTranspose(lm->getLight(1)->getViewMatrix());
	dataPtr->lightProjection = XMMatrixTranspose(lm->getLight(1)->getOrthoMatrix());
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	TesType* tesData;
	result = deviceContext->Map(tesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tesData = (TesType*)mappedResource.pData;
	tesData->edges = static_cast<XMFLOAT4>(edges);
	tesData->inside = static_cast<XMFLOAT2>(inside);
	tesData->padding = {0.0f, 0.0f};
	deviceContext->Unmap(tesBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tesBuffer);

	IslandBufferType* islandData;
	result = deviceContext->Map(islandBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	islandData = (IslandBufferType*)mappedResource.pData;
	islandData->texRes = texRes;
	islandData->height = height;
	deviceContext->Unmap(islandBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &islandBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &islandBuffer);

	// Additional
	// Send light data to pixel shader

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

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture0);
	deviceContext->PSSetShaderResources(1, 1, &texture1);
	deviceContext->PSSetShaderResources(2, 1, &heightMap);

	ID3D11ShaderResourceView* dmsrv = lm->getDepthMapSRV();
	deviceContext->PSSetShaderResources(3, 1, &dmsrv);

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);

	deviceContext->DSSetShaderResources(0, 1, &heightMap);
}

void IslandShader::setDepthShaderParameters(
	ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix,
	ID3D11ShaderResourceView* heightMap,
	float* edges, float* inside, float height)
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
	dataPtr->lightView = tproj;
	dataPtr->lightProjection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	TesType* tesData;
	result = deviceContext->Map(tesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tesData = (TesType*)mappedResource.pData;
	tesData->edges = static_cast<XMFLOAT4>(edges);
	tesData->inside = static_cast<XMFLOAT2>(inside);
	tesData->padding = {0.0f, 0.0f};
	deviceContext->Unmap(tesBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tesBuffer);

	deviceContext->DSSetShaderResources(0, 1, &heightMap);
}
