#include "WaterShader.h"
#include "ShaderBuffers.h"

WaterShader::WaterShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"waterVS.cso", L"waterHS.cso", L"waterDS.cso", L"waterPS.cso", L"depthPS.cso");
}

WaterShader::~WaterShader()
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

void WaterShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
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
	matrixBufferDesc.ByteWidth = sizeof(ShadowMatrixBufferType);
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
	lightBufferDesc.ByteWidth = sizeof(LightBufferType) * 8 + sizeof(uint32_t) * 4;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
}

void WaterShader::initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds,
							  const wchar_t* ps, const wchar_t* dps)
{
	initShader(vs, ps);

	loadHullShader(hs);
	loadDomainShader(ds);
	loadDepthPixelShader(dps);
}

void WaterShader::setShaderParameters(
ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix,
							 const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
							 float* edges, float* inside, float time, float speed, float amp, float freq,
		 ID3D11ShaderResourceView* height,
	const std::vector<LightBase*>& lights, ID3D11ShaderResourceView* shadowMap
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
	dataPtr->lightView = XMMatrixTranspose(lights[0]->getViewMatrix());
	dataPtr->lightProjection = XMMatrixTranspose(lights[0]->getOrthoMatrix());
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	TimeBufferType* timeData;
	result = deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timeData = (TimeBufferType*)mappedResource.pData;
	timeData->time = time;
	timeData->amplitude = amp;
	timeData->frequency = freq;
	timeData->speed = speed;
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &timeBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &timeBuffer);

	TesType* tesData;
	result = deviceContext->Map(tesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tesData = (TesType*)mappedResource.pData;
	tesData->edges = static_cast<XMFLOAT4>(edges);
	tesData->inside = static_cast<XMFLOAT2>(inside);
	tesData->padding = {0.0f, 0.0f};
	deviceContext->Unmap(tesBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tesBuffer);

	// Set shader texture resources
	deviceContext->PSSetShaderResources(0, 1, &shadowMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->DSSetShaderResources(0, 1, &height);


	std::vector<LightBufferType> ldata;
	for (auto& light : lights)
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
}

void WaterShader::setDepthShaderParameters(ID3D11DeviceContext* deviceContext,
										   const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
										   const XMMATRIX& projectionMatrix, float* edges,
										   float* inside, float time, float speed, float amp,
										   float freq, ID3D11ShaderResourceView* height)
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

	TimeBufferType* timeData;
	result = deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timeData = (TimeBufferType*)mappedResource.pData;
	timeData->time = time;
	timeData->amplitude = amp;
	timeData->frequency = freq;
	timeData->speed = speed;
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &timeBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &timeBuffer);

	TesType* tesData;
	result = deviceContext->Map(tesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tesData = (TesType*)mappedResource.pData;
	tesData->edges = static_cast<XMFLOAT4>(edges);
	tesData->inside = static_cast<XMFLOAT2>(inside);
	tesData->padding = {0.0f, 0.0f};
	deviceContext->Unmap(tesBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tesBuffer);

	// Set shader texture resources
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->DSSetShaderResources(0, 1, &height);
}
