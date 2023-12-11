#pragma once
#include "d3d.h"

using namespace DirectX;

class ShadowMapArray
{
  public:
	ShadowMapArray(ID3D11Device* device, int mWidth, int mHeight);
	~ShadowMapArray();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc, size_t index);
	ID3D11ShaderResourceView* getDepthMapArraySRV() { return mDepthMapArraySRV; };

  private:
	ID3D11Texture2D* depthMaps;
	ID3D11DepthStencilView* mDepthMapDSVs[7];
	ID3D11ShaderResourceView* mDepthMapArraySRV;
	D3D11_VIEWPORT viewport;
	ID3D11RenderTargetView* renderTargets[1];
};
