#pragma once
#include "LightBase.h"
class DirectionLight : public LightBase
{
  public:
	DirectionLight();
	void setDirection(XMFLOAT3 dir) { light.setDirection(dir.x, dir.y, dir.z); }
	void setPosition(XMFLOAT3 pos) { light.setPosition(pos.x, pos.y, pos.z); }
	XMFLOAT3 getDirection() const { return light.getDirection(); }
};
