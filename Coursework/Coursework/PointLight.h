#pragma once
#include "LightBase.h"
class PointLight : public LightBase
{
  public:
	PointLight(size_t shadowMapCount, size_t shadowMapIndex);
};
