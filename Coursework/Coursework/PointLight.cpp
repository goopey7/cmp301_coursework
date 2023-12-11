#include "PointLight.h"

PointLight::PointLight(size_t shadowMapIndex, size_t shadowMapCount) : LightBase(shadowMapIndex, shadowMapCount)
{ 
	type = LightType::Point;
	light.setAmbientColour(0.2f, 0.2f, 0.2f, 1.f); 
	light.setDiffuseColour(1.f, 1.f, 1.f, 1.f);
	light.generateOrthoMatrix(200.f, 200.f, 0.1f, 100.f);
}
