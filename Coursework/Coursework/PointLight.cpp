#include "PointLight.h"

PointLight::PointLight()
{ 
	type = LightType::Point;
	light.setAmbientColour(0.2f, 0.2f, 0.2f, 1.f); 
	light.setDiffuseColour(1.f, 1.f, 1.f, 1.f);
}
