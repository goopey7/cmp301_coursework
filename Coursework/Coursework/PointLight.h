#pragma once
#include "LightBase.h"
class PointLight : public LightBase
{
  public:
	PointLight();
	void setPosition(XMFLOAT3 pos) { light.setPosition(pos.x, pos.y, pos.z); }
	XMFLOAT3 getPosition() const { return light.getPosition(); }
};
