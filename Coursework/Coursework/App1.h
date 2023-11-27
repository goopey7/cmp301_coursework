// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "IslandShader.h"
#include "Light.h"
#include "TesselatedPlaneMesh.h"

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
	IslandShader* islandShader;
	float elapsedTime = 0.f;
	Light* light;
	float amp = 11.f;
	float edges[4] = {32.f, 32.f, 32.f, 32.f};
	float inside[2] = {32.f, 32.f};
	float lightDir[3] = {1.f, 0.8f, 0.25f};
};

#endif
