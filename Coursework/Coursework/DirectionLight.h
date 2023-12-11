#pragma once
#include "LightBase.h"
class DirectionLight : public LightBase
{
  public:
	DirectionLight(size_t shadowMapIndex, size_t shadowMapCount);
};
