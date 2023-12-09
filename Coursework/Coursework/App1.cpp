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
	islandMesh = new TesselatedPlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 2.f, 0.f, 0.f, 100.f, 100.f);
	islandShader = new IslandShader(renderer->getDevice(), hwnd);
	waterMesh = new TesselatedPlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 32.f, -250.f, -250.f, 500.f, 500.f);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	colorShader = new ColorShader(renderer->getDevice(), hwnd);
	shadowTestMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	PointLight* pointLight = new PointLight();
	DirectionLight* dirLight = new DirectionLight();
	dirLight->setDirection((XMFLOAT3)lightDir);
	dirLight->setPosition((XMFLOAT3)dirLightPos);

	lights.push_back(dirLight);
	lights.push_back(pointLight);

	shadowMap = new ShadowMap(renderer->getDevice(), 1024 * 5, 1024 * 5);
	camera->setPosition(0.f, 10.f, -10.f);

	waves.push_back(Wave());
	waves[0].direction = { 1.f, 1.f };
	waves[0].length = 10.f;
	waves[0].steepness = 0.25f;

	waves.push_back(Wave());
	waves[1].direction = { 0.66f, 0.6f };
	waves[1].length = 5.f;
	waves[1].steepness = 0.25f;

	waves.push_back(Wave());
	waves[2].direction = { 0.78, 1.3f };
	waves[2].length = 2.f;
	waves[2].steepness = 0.25f;
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

	update(timer->getTime());

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

void App1::update(float dt)
{
	((DirectionLight*)lights[0])->setDirection(lightDir);
	((DirectionLight*)lights[0])->setPosition(dirLightPos);
	((PointLight*)lights[1])->setPosition(pointLightPos);

	elapsedTime += timer->getTime();

	// Generate the view matrix based on the camera's position.
	camera->update();
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	lights[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = lights[0]->getViewMatrix();
	XMMATRIX lightProjMatrix = lights[0]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	auto ctx = renderer->getDeviceContext();

	// render test sphere
	worldMatrix *= XMMatrixTranslation(testMeshPos.x, testMeshPos.y, testMeshPos.z);
	shadowTestMesh->sendData(ctx);
	colorShader->setDepthShaderParamters(ctx, worldMatrix, lightViewMatrix, lightProjMatrix);
	colorShader->renderDepth(ctx, shadowTestMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	//renderer->beginScene(0.f,0.f,0.f,1.f);

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Get the world, view, projection, and ortho matrices from the camera and
	// Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	auto ctx = renderer->getDeviceContext();

	for (size_t i = 0; i < islandMesh->getQuadrants(); i++)
	{
		islandMesh->sendData(ctx, i);
		islandShader->setShaderParameters(
			ctx, worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"stone"), textureMgr->getTexture(L"islandHeight"),
			shadowMap->getDepthMapSRV(),
			lights, edges, inside, texRes, islandHeight
		);

		islandShader->render(ctx, islandMesh->getIndexCount());
	}

	// render test sphere
	worldMatrix *= XMMatrixTranslation(testMeshPos.x, testMeshPos.y, testMeshPos.z);
	shadowTestMesh->sendData(ctx);
	colorShader->setShaderParameters(ctx, worldMatrix, viewMatrix, projectionMatrix);
	colorShader->render(ctx, shadowTestMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(0.f, 0.6f, 0.f);
	for (size_t i = 0; i < waterMesh->getQuadrants(); i++)
	{
		waterMesh->sendData(ctx, i);

		waterShader->setShaderParameters(ctx, worldMatrix, viewMatrix, projectionMatrix,
			edges,
			inside,
			elapsedTime,
			waterGravity,
			waves,
			lights,
		    shadowMap->getDepthMapSRV(),
			camera->getPosition()
		);

		waterShader->render(ctx, waterMesh->getIndexCount());
	}
}

bool App1::render()
{
	depthPass();
	finalPass();

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
		ImGui::SliderFloat3("LightDir", (float*)&lightDir, -1.f, 1.f);
		ImGui::SliderFloat3("DirLightPos", (float*)&dirLightPos, -100.f, 100.f);
		ImGui::SliderFloat3("PointLightPos", (float*)&pointLightPos, -100.f, 100.f);
	ImGui::End();

	ImGui::Begin("Island");
		ImGui::SliderFloat("IslandHeight", &islandHeight, 0.f, 100.f);
	ImGui::End();

	ImGui::Begin("Water");
		ImGui::SliderFloat("Gravity", &waterGravity, 0.f, 20.f);
		if (ImGui::Button("Add Wave") && waves.size() < 8)
		{
			waves.push_back(Wave());
			waves[waves.size() - 1].direction = { 1.f, 1.f };
			waves[waves.size() - 1].length = 10.f;
			waves[waves.size() - 1].steepness = 0.6f;
		}

		ImGui::Text("Waves");
		ImGui::Text("Don't let the sum of steepness values exceed 1.0 if you do you will see "
					"looping artifacts");
		ImGui::Separator();

		for (size_t i = 0; i < waves.size(); i++)
		{
			Wave& wave = waves[i];
			char waveLetter = i + 65;
			ImGui::Text("Wave %c", waveLetter);
			std::string waveid = "Wave" + std::string(&waveLetter, 1);
			ImGui::SliderFloat((waveid + "Length").c_str(), &wave.length, 0.f, 20.f);
			ImGui::SliderFloat((waveid + "Steepness").c_str(), &wave.steepness, 0.f, 1.f);
			ImGui::SliderFloat2((waveid + "Direction").c_str(), (float*)&wave.direction, -1.f, 1.f);
			ImGui::Separator();
		}
	ImGui::End();

	ImGui::Begin("ShadowMap");
		ImGui::Image(shadowMap->getDepthMapSRV(), ImVec2(256, 256));
		ImGui::SliderFloat3("TestMeshPos", (float*)&testMeshPos, -100.f, 100.f);
	ImGui::End();

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
