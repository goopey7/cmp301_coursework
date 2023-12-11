#pragma once
#include "DXF.h"
#include "ShaderBuffers.h"

enum LightType
{
	Directional,
	Point,
	Spot
};

class LightBase
{
  public:
		operator Light*() { return &light; }
		LightBufferType getConstBuffer()
		{
			return LightBufferType {
				light.getDiffuseColour(),
				light.getDirection(),
				(uint32_t)type,
				light.getAmbientColour(),
				light.getPosition(),
				attenuation
			};
		}

		void generateViewMatrix()
		{
			light.generateViewMatrix();
		}
		XMMATRIX getViewMatrix()
		{
			return light.getViewMatrix();
		}
		XMMATRIX getOrthoMatrix()
		{
			return light.getOrthoMatrix();
		}
		LightType getType()
		{
			return type;
		}
		ShadowMap* getShadowMap(int index)
		{
			return shadowMaps[index];
		}
		void setDirection(XMFLOAT3 direction)
		{
			light.setDirection(direction.x, direction.y, direction.z);
		}
		void setPosition(XMFLOAT3 pos)
		{
			light.setPosition(pos.x, pos.y, pos.z);
		}
		XMFLOAT3 getPosition() const { return light.getPosition(); }
		XMFLOAT3 getDirection() const { return light.getDirection(); }
  protected:
	  Light light;
	  LightType type;
	  float attenuation = FLT_MAX;
	  std::vector<ShadowMap*> shadowMaps;
};
