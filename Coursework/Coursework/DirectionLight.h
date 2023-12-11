#pragma once
#include "LightBase.h"
class DirectionLight : public LightBase
{
  public:
	DirectionLight(ID3D11Device* device, int width, int height);
};
