#pragma once
#include "BaseMesh.h"

using namespace DirectX;

class TesselatedPlaneMesh : public BaseMesh
{
  public:
	TesselatedPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float x = 0.f, float z = 0.f, float width = 2.f, float length = 2.f);
	~TesselatedPlaneMesh();

	void sendData(
		ID3D11DeviceContext* deviceContext,
		D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST) override;

  private:
	void initBuffers(ID3D11Device* device) override;
	float x,z;
	float width, length;
};
