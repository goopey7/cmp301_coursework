#include "ShadowMapArray.h"

ShadowMapArray::ShadowMapArray(ID3D11Device* device, int mWidth, int mHeight)
{
	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 7;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// ID3D11Texture2D* depthMap = 0;
	device->CreateTexture2D(&texDesc, 0, &depthMaps);

	for (size_t i = 0; i < 7; i++)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		dsvDesc.Texture2DArray.ArraySize = 1;
		device->CreateDepthStencilView(depthMaps, &dsvDesc, &mDepthMapDSVs[i]);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = 7;
	device->CreateShaderResourceView(depthMaps, &srvDesc, &mDepthMapArraySRV);

	// Setup the viewport for rendering.
	viewport.Width = (float)mWidth;
	viewport.Height = (float)mHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// NULL render target
	renderTargets[1] = {0};
}

ShadowMapArray::~ShadowMapArray()
{
	//delete mDepthMapDSV;
	//delete mDepthMapSRV;
}

void ShadowMapArray::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc, size_t index)
{
	dc->RSSetViewports(1, &viewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	// ID3D11RenderTargetView* renderTargets[1] = { 0 };
	dc->OMSetRenderTargets(1, renderTargets, mDepthMapDSVs[index]);

	dc->ClearDepthStencilView(mDepthMapDSVs[index], D3D11_CLEAR_DEPTH, 1.0f, 0);
}
