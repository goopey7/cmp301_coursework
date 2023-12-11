#include "PointLight.h"

PointLight::PointLight(ID3D11Device* device, int width, int height)
{ 
	type = LightType::Point;
	light.setAmbientColour(0.2f, 0.2f, 0.2f, 1.f); 
	light.setDiffuseColour(1.f, 1.f, 1.f, 1.f);
	light.generateOrthoMatrix(200.f, 200.f, 0.1f, 100.f);
	for (int i=0; i<6; i++)
		shadowMaps.push_back(new ShadowMap(device, width, height));
}
