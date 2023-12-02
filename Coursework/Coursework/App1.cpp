#include "App1.h"

App1::App1() {}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in,
				bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	textureMgr->loadTexture(L"stone", L"res/stone.jpg");
	textureMgr->loadTexture(L"grass", L"res/grass.jpg");
	textureMgr->loadTexture(L"island", L"res/Wizard.tif");
	textureMgr->loadTexture(L"islandHeight", L"res/Wizard.tif");
	textureMgr->loadTexture(L"waterColor", L"res/Water_001_COLOR.jpg");
	textureMgr->loadTexture(L"waterNormal", L"res/Water_001_NORM.jpg");
	textureMgr->loadTexture(L"waterHeight", L"res/Water_001_DISP.png");

	// Create Mesh object and shader object
	islandMesh = new TesselatedPlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 32.f, 0.f, 0.f, 100.f, 100.f);
	islandShader = new IslandShader(renderer->getDevice(), hwnd);
	waterMesh = new TesselatedPlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 32.f, -250.f, -250.f, 500.f, 500.f);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	colorShader = new ColorShader(renderer->getDevice(), hwnd);
	//pointLightMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	PointLight* pointLight = new PointLight();
	DirectionLight* dirLight = new DirectionLight();

	lights.push_back(dirLight);
	lights.push_back(pointLight);
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

	if (waterMesh)
	{
		delete waterMesh;
		waterMesh = 0;
	}

	if (islandShader)
	{
		delete islandShader;
		islandShader = 0;
	}

	if (waterShader)
	{
		delete waterShader;
		waterShader = 0;
	}

	if (colorShader)
	{
		delete colorShader;
		colorShader = 0;
	}

	for (auto light : lights)
	{
		if (light)
		{
			delete light;
			light = 0;
		}
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

	((DirectionLight*)lights[0])->setDirection((XMFLOAT3)lightDir);
	((PointLight*)lights[1])->setPosition(pointLightPos);
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

	auto ctx = renderer->getDeviceContext();

	/*
	pointLightMesh->sendData(ctx);
	worldMatrix *= XMMatrixTranslation(pointLightPos.x, pointLightPos.y, pointLightPos.z);
	colorShader->setShaderParameters(ctx, worldMatrix, viewMatrix, projectionMatrix);
	colorShader->render(ctx, pointLightMesh->getIndexCount());
	*/

	/*
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(0.f, 0.6f, 0.f);
	for (size_t i = 0; i < waterMesh->getQuadrants(); i++)
	{
		waterMesh->sendData(ctx, i);

		waterShader->setShaderParameters(ctx, worldMatrix, viewMatrix, projectionMatrix,
			edges,
			inside,
			elapsedTime,
			waterSpeed,
			waterAmp, 
			waterFreq,
			textureMgr->getTexture(L"waterColor"),
			textureMgr->getTexture(L"waterNormal"),
			textureMgr->getTexture(L"waterHeight")
		);

		waterShader->render(ctx, waterMesh->getIndexCount());
	}
	*/

	worldMatrix = renderer->getWorldMatrix();
	for (size_t i = 0; i < islandMesh->getQuadrants(); i++)
	{
		islandMesh->sendData(ctx, i);
		islandShader->setShaderParameters(
			ctx, worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"stone"),
			textureMgr->getTexture(L"islandHeight"),
			lights, edges, inside, texRes, islandHeight
		);

		islandShader->render(ctx, islandMesh->getIndexCount());
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
	ImGui::SliderFloat4("Edges", edges, 1.f, 64.f);
	ImGui::SliderFloat2("Inside", inside, 1.f, 64.f);
	ImGui::SliderFloat("TextureRes", &texRes, 1.f, 500.f);
	ImGui::DragFloat3("CameraPos", (float*)&camera->getPosition(), -100.f, 100.f);

	ImGui::Begin("Lighting");
		ImGui::SliderFloat3("LightDir", lightDir, -1.f, 1.f);
		ImGui::SliderFloat3("PointLightPos", (float*)&pointLightPos, -100.f, 100.f);
	ImGui::End();

	ImGui::Begin("Island");
		ImGui::SliderFloat("IslandHeight", &islandHeight, 0.f, 100.f);
	ImGui::End();

	ImGui::Begin("Water");
		ImGui::SliderFloat("Speed", &waterSpeed, 0.f, 0.5f);
		ImGui::SliderFloat("Frequency", &waterFreq, 0.f, 10.f);
		ImGui::SliderFloat("WaterAmplitude", &waterAmp, 0.f, 10.f);
	ImGui::End();

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
