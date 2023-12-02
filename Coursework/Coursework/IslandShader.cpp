#include "IslandShader.h"
#include "ShaderBuffers.h"

IslandShader::IslandShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"islandVS.cso", L"islandHS.cso", L"islandDS.cso", L"islandPS.cso");
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
	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = 0;
	}

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
	D3D11_BUFFER_DESC timeBufferDesc;
	D3D11_BUFFER_DESC tesBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);

	tesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tesBufferDesc.ByteWidth = sizeof(TesType);
	tesBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tesBufferDesc.MiscFlags = 0;
	tesBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tesBufferDesc, NULL, &tesBuffer);

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

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER
	// or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
}

void IslandShader::initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds,
							  const wchar_t* ps)
{
	initShader(vs, ps);

	loadHullShader(hs);
	loadDomainShader(ds);
}

void IslandShader::setShaderParameters(
	ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture0, ID3D11ShaderResourceView* texture1,
	ID3D11ShaderResourceView* heightMap,
	Light* light, float* edges, float* inside, float texRes, float height)
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
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	TimeBufferType* timeData;
	result = deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timeData = (TimeBufferType*)mappedResource.pData;
	timeData->amplitude = height;
	timeData->time = 0.f;
	timeData->frequency = 0.f;
	timeData->speed = 0.f;
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &timeBuffer);

	TesType* tesData;
	result = deviceContext->Map(tesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tesData = (TesType*)mappedResource.pData;
	tesData->edges = static_cast<XMFLOAT4>(edges);
	tesData->inside = static_cast<XMFLOAT2>(inside);
	tesData->padding = {0.0f, 0.0f};
	deviceContext->Unmap(tesBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tesBuffer);

	// Additional
	//  Send light data to pixel shader

	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->lightType = 1;
	lightPtr->diffuse = light->getDiffuseColour();
	lightPtr->direction = light->getDirection();
	lightPtr->position = light->getPosition();
	lightPtr->texRes = texRes;
	lightPtr->ambient = light->getAmbientColour();
	lightPtr->attenuation = 10.f;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture0);
	deviceContext->PSSetShaderResources(1, 1, &texture1);
	deviceContext->PSSetShaderResources(2, 1, &heightMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	deviceContext->DSSetShaderResources(0, 1, &heightMap);
}
