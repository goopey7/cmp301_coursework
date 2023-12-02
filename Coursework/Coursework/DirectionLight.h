#pragma once
#include "LightBase.h"
class DirectionLight : public LightBase
{
  public:
	DirectionLight();
	void setDirection(XMFLOAT3 dir) { light.setDirection(dir.x, dir.y, dir.z); }
	XMFLOAT3 getDirection() const { return light.getDirection(); }
};
