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
  protected:
	  Light light;
	  LightType type;
	  float attenuation = FLT_MAX;
};
