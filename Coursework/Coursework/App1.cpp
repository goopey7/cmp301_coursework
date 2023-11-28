#include "App1.h"

App1::App1() {}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in,
				bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	textureMgr->loadTexture(L"stone", L"res/stone.jpg");
	textureMgr->loadTexture(L"grass", L"res/grass.jpg");
	textureMgr->loadTexture(L"island", L"res/islandHeight.png");
	textureMgr->loadTexture(L"islandHeight", L"res/islandHeight.png");
	textureMgr->loadTexture(L"islandNormal", L"res/normalAlt.png");

	// Create Mesh object and shader object
	islandMesh = new TesselatedPlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 3.f, 0.f, 0.f, 50.f, 50.f);
	islandShader = new IslandShader(renderer->getDevice(), hwnd);

	light = new Light;
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.f);
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (islandMesh)
	{
		delete islandMesh;
		islandMesh = 0;
	}
	if (islandShader)
	{
		delete islandShader;
		islandShader = 0;
	}
	if (light)
	{
		delete light;
		light = 0;
	}
}

bool App1::frame()
{
	bool result;
	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	light->setDirection(lightDir[0], lightDir[1], lightDir[2]);
	elapsedTime += timer->getTime();

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	//renderer->beginScene(0.f,0.f,0.f,1.f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and
	// Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	for (size_t i = 0; i < islandMesh->getQuadrants(); i++)
	{
		islandMesh->sendData(renderer->getDeviceContext(), i);
		islandShader->setShaderParameters(
			renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"stone"),
			textureMgr->getTexture(L"islandHeight"), textureMgr->getTexture(L"islandNormal"),
			elapsedTime, amp, 1.f, 1.f, light, edges, inside, texRes);

		islandShader->render(renderer->getDeviceContext(), islandMesh->getIndexCount());
	}

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::SliderFloat("Amplitude", &amp, 0.f, 100.f);
	ImGui::SliderFloat4("Edges", edges, 1.f, 64.f);
	ImGui::SliderFloat2("Inside", inside, 1.f, 64.f);

	ImGui::SliderFloat3("LightDir", lightDir, -1.f, 1.f);

	ImGui::SliderFloat("TextureRes", &texRes, 1.f, 500.f);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
