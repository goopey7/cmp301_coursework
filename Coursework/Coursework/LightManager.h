#pragma once

#include "PointLight.h"
#include "DirectionLight.h"
#include "ShadowMapArray.h"

class LightManager
{
public:
	LightManager(ID3D11Device* device) : device(device)
	{
		shadowMaps = new ShadowMapArray(device, 2048 * 2, 2048 * 2);
	}

	~LightManager()
	{ 
		for (auto light : lights) delete light;
		delete shadowMaps;
	}

	void addPointLight()
	{ 
		PointLight* light = new PointLight(nextShadowMapIndex, 6);
		lights.push_back(light);
		nextShadowMapIndex += 6;
	}

	void addDirLight()
	{
		DirectionLight* light = new DirectionLight(nextShadowMapIndex, 1);
		lights.push_back(light);
		nextShadowMapIndex++;
	}

	std::pair<size_t, size_t> getShadowMaps(int index)
	{ 
		size_t start = lights[index]->getShadowMapIndex();
		size_t end = start + lights[index]->getShadowMapCount();

		return {start, end};
	}

	void bindDsvAndSetNullRenderTarget(ID3D11DeviceContext* deviceContext, int index)
	{
		shadowMaps->BindDsvAndSetNullRenderTarget(deviceContext, index);
	}

	ID3D11ShaderResourceView* getDepthMapSRV()
	{
		return shadowMaps->getDepthMapArraySRV();
	}

	LightBase* getLight(size_t index) { return lights[index]; }
	const std::vector<LightBase*>& getLights() { return lights; }

	size_t getShadowMapCount() { return nextShadowMapIndex; }

  private:
	std::vector<LightBase*> lights;
	ID3D11Device* device;
	ShadowMapArray* shadowMaps;
	size_t nextShadowMapIndex = 0;
};
