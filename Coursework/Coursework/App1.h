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

protected:
	bool render();
	void gui();

private:
	TesselatedPlaneMesh* islandMesh;
	TesselatedPlaneMesh* waterMesh;
	IslandShader* islandShader;
	WaterShader* waterShader;
	ColorShader* colorShader;

	std::vector<LightBase*> lights;
	std::vector<SphereMesh*> lightMeshes;

	// island variables
	float islandHeight = 60.f;
	float texRes = 15.f;

	// water variables
	float waterSpeed = 0.2f;
	float waterAmp = 0.6f;
	float waterFreq = 0.5f;

	// universal tesselation variables
	float edges[4] = {32.f, 32.f, 32.f, 32.f};
	float inside[2] = {32.f, 32.f};

	// light variables
	float lightDir[3] = {1.f, 0.8f, 0.25f};
	XMFLOAT3 pointLightPos = { 27.f, 7.f, 21.f };

	float elapsedTime = 0.f;
};

#endif
