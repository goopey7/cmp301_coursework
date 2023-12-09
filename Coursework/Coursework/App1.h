// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "IslandShader.h"
#include "WaterShader.h"
#include "colorShader.h"
#include "Light.h"
#include "TesselatedPlaneMesh.h"
#include "PointLight.h"
#include "DirectionLight.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();
	void update(float dt);

protected:
	bool render();
	void finalPass();
	void depthPass();
	void gui();

private:
	TesselatedPlaneMesh* islandMesh;
	TesselatedPlaneMesh* waterMesh;
	IslandShader* islandShader;
	WaterShader* waterShader;
	ColorShader* colorShader;
	SphereMesh* shadowTestMesh;

	ShadowMap* shadowMap;

	std::vector<LightBase*> lights;
	std::vector<SphereMesh*> lightMeshes;

	// island variables
	float islandHeight = 45.f;
	float texRes = 15.f;

	// water variables
	float waterGravity = 9.81f;
	float waterSteepness = 0.6f;
	float waterWaveLength = 10.f;
	XMFLOAT2 waterDirection = {1.f, 1.f};

	// universal tesselation variables
	float edges[4] = {32.f, 32.f, 32.f, 32.f};
	float inside[2] = {32.f, 32.f};

	// light variables
	XMFLOAT3 lightDir = {1.f, -0.3f, 0.25f};
	XMFLOAT3 dirLightPos = { 27.f, 50.f, 21.f };
	XMFLOAT3 pointLightPos = { 40.f, 7.f, 20.f };

	// shadow test variables
	XMFLOAT3 testMeshPos = { 27.f, 9.f, 21.f };

	float elapsedTime = 0.f;

	std::vector<Wave> waves;
};

#endif
