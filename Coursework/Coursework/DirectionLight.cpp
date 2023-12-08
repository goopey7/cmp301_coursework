#include "DirectionLight.h"

DirectionLight::DirectionLight()
{
	type = LightType::Directional;
	light.setAmbientColour(0.2f, 0.2f, 0.2f, 1.f); 
	light.setDiffuseColour(1.f, 1.f, 1.f, 1.f);
	light.setPosition(0.f, 3.f, -10.f);
	light.setDirection(0.f, -0.7f, 0.7f);
	light.generateOrthoMatrix(200.f, 200.f, 0.1f, 100.f);
}
