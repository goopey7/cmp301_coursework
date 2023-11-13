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
	float amp = 0.f;
	float edges[4] = {4.f, 4.f, 4.f, 4.f};
	float inside[2] = {4.f, 4.f};
};

#endif
