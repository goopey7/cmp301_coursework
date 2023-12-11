#pragma once

#include "PointLight.h"
#include "DirectionLight.h"

class LightManager
{
public:
	LightManager(ID3D11Device* device) : device(device)
	{
		for (size_t i = 0; i < 7; i++)
		{
			shadowMaps.push_back(new ShadowMap(device, 1024, 1024));
		}
	}

	~LightManager()
	{ 
		for (auto light : lights) delete light;
		for (auto sm : shadowMaps) delete sm;
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
		shadowMaps[index]->BindDsvAndSetNullRenderTarget(deviceContext);
	}

	std::vector<ID3D11ShaderResourceView*> getDepthMapSRV(int index)
	{
		LightBase* light = lights[index];
		auto begin = light->getShadowMapIndex();
		auto end = begin + light->getShadowMapCount();

		std::vector<ID3D11ShaderResourceView*> srvs;
		for (int i = begin; i < end; i++)
		{
			srvs.push_back(shadowMaps[i]->getDepthMapSRV());
		}

		return srvs;
	}

	LightBase* getLight(size_t index) { return lights[index]; }
	const std::vector<LightBase*>& getLights() { return lights; }

	size_t getShadowMapCount() { return nextShadowMapIndex; }

  private:
	std::vector<LightBase*> lights;
	ID3D11Device* device;
	std::vector<ShadowMap*> shadowMaps;
	size_t nextShadowMapIndex = 0;
};
