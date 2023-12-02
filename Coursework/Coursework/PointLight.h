#pragma once
#include "DXF.h"
class PointLight
{
  public:
	PointLight();
	void setPosition(XMFLOAT3 pos) { light.setPosition(pos.x, pos.y, pos.z); }
	XMFLOAT3 getPosition() const { return light.getPosition(); }

	operator Light*() { return &light; }
  private:
	Light light;
	float attenuation = 100.0f;
};
