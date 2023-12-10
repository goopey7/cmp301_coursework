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
#include "TextureShader.h"
#include "WaterPPShader.h"
#include "UnderbellyShader.h"

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
	void sceneToTexturePass();
	void depthPass();
	void finalPass();
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

	// post processing
	RenderTexture* renderTexture;
	OrthoMesh* orthoMesh;
	TextureShader* textureShader;
	WaterPPShader* waterPPShader;

	float underwaterFreq = 120.f;
	float underwaterSpeed = 2.75f;
	float underwaterDisplacement = 0.004272f;
	XMFLOAT3 underwaterColor = { 0.1f, 0.275f, 1.f };

    float underwaterWeights[5] = { 0.38298f, 0.241732f, 0.060598f, 0.005977f, 0.000229f };
	float blurAmount = 3.f;
	XMFLOAT2 screenSize;

	//underwater objects
	PlaneMesh* islandUnderbellyMesh;
	UnderbellyShader* underbellyShader;
	XMVECTOR underbellyPos = { 0.f, .6f, -50.f };

	RenderTexture* finalRenderTexture;
	XMFLOAT4 clearColor = { 0.39f, 0.58, 0.92f, 1.f };

	// gui
	ImVec2 clientSize;

	// Setup the projection matrix.
	float fieldOfView = (float)XM_PI / 4.0f;
	float screenAspect = 1920.f / 1080.f;

	// Create the projection matrix for 3D rendering.
	XMMATRIX projectionMatrix;
};

#endif
