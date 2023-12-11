#pragma once
#include "LightBase.h"
class PointLight : public LightBase
{
  public:
	PointLight(ID3D11Device* device, int width, int height);
};
