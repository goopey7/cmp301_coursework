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
		LightBase(size_t shadowMapIndex, size_t shadowMapCount) : shadowMapIndex(shadowMapIndex), shadowMapCount(shadowMapCount)
		{
			light.generateOrthoMatrix(200.f, 200.f, 0.1f, 100.f);
		}
		operator Light*() { return &light; }
		LightBufferType getConstBuffer()
		{
			LightBufferType lbType = LightBufferType {
				light.getDiffuseColour(),
				light.getDirection(),
				(uint32_t)type,
				light.getAmbientColour(),
				light.getPosition(),
				attenuation,
			};

			lbType.shadowMapStart = (uint32_t)shadowMapIndex;
			lbType.shadowMapEnd = (uint32_t)shadowMapIndex + (uint32_t)shadowMapCount;

			return lbType;
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
		size_t getShadowMapCount() const { return shadowMapCount; }
		size_t getShadowMapIndex() const { return shadowMapIndex; }
		XMMATRIX getPointLightViewMatrix(size_t index)
		{
			XMFLOAT3 d = pointLightDirections[index];
			light.setDirection(d.x, d.y, d.z);
			light.generateViewMatrix();
			return light.getViewMatrix();
		}
		void setColor(XMFLOAT4 color)
		{
			light.setDiffuseColour(color.x, color.y, color.z, color.w);
		}
  protected:
	  Light light;
	  LightType type;
	  float attenuation = FLT_MAX;
	  size_t shadowMapCount;
	  size_t shadowMapIndex;
	  XMFLOAT3 pointLightDirections[6] = {{1.f, 0.f, 0.f},  {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f},
										{0.f, -1.f, 0.f}, {0.f, 0.f, 1.f},	{0.f, 0.f, -1.f}};
};

