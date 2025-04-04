// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	screenWidth1 = screenWidth;
	screenHeight1 = screenHeight;

	// Initalise scene variables.
	// Load texture.
	//textureMgr->loadTexture(L"diffuseMap", L"res/DefaultDiffuse.dds");
	textureMgr->loadTexture(L"heightMap", L"res/heightMap.jpeg");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");
	textureMgr->loadTexture(L"grass", L"res/grass.jpg");
	textureMgr->loadTexture(L"water", L"res/water.jpg");
	textureMgr->loadTexture(L"blankMap", L"res/black.jpg");
	textureMgr->loadTexture(L"wood", L"res/wood.png");

	// Create Mesh object and shader object
	mesh = new TessellationMeshQuad(renderer->getDevice(), renderer->getDeviceContext());
	manipulationShader = new ManipulationShader(renderer->getDevice(), hwnd);
	//lightShader = new LightShader(renderer->getDevice(), hwnd);

	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// Confirgure directional light
	light = new MyLight();
	light->light->setAmbientColour(0, 0, 0, 0);
	light->light->setDiffuseColour(0.6f, 0.8f, 0.8f, 1.0f);
	light->light->setDirection(-1.0f, -1.0f, 0.0f);
	light->light->setPosition(-10, 10, 0);
	light->light->setSpecularPower(0);
	light->type = XMFLOAT4(3,0,0,0);
	light->cone = 0;
	light->attenuation = XMFLOAT4(0.1, 0, 0, 0);
	lights[0] = light;
	
	light2 = new MyLight();
	light2->light->setAmbientColour(0, 0, 0, 1.0f);
	light2->light->setDiffuseColour(0.7f, 0.7f, 0.3f, 1.0f);
	light2->light->setDirection(0.0f, -1.0f, -1.0f);
	light2->light->setPosition(15.0f, 4.0f, 15.0f);
	light2->light->setSpecularPower(100.0f);
	light2->light->setSpecularColour(0.5f, 0.5f, 0.3f, 1.0f);
	light2->type = XMFLOAT4(1, 0, 0, 0);
	light2->cone = 20.0f;
	light2->attenuation = XMFLOAT4(0.4, 0.2, 0.0, 0.0);
	light2->light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 1.0f, 100.f);
	lights[1] = light2;

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	textureShader2 = new TextureShader(renderer->getDevice(), hwnd);
	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size

	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);

	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	bloomHighlightTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomHighlightShader = new BloomHighlightShader(renderer->getDevice(), hwnd);

	bloomFinalShader = new BloomShaderFinal(renderer->getDevice(), hwnd);

	directionalLightShadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	depthHeightmapShader = new DepthHeightmapShader(renderer->getDevice(), hwnd);

	debugOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth/4, screenHeight/4, -screenWidth/2.7, screenHeight/2.7);
	debugRenderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	screenWidth1 = screenWidth;
	screenHeight1 = screenHeight;

	shadowMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 20);

	cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());

	depthShader = new DepthShader(renderer->getDevice(), hwnd);

	for (int i = 0; i < 6; i++)
	{
		pointLightShadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	}

	waterMesh = new TessellationMeshQuad(renderer->getDevice(), renderer->getDeviceContext());

	model = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/models/57-lowpoly-tree-v1/LowPoly Tree v1/LowPoly_Tree_v1.obj");
	//campFire = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/models/cai34a6aetc0-lowpoly_logs/logs_low_poly.obj");
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
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
	depthPass();
	pointDepthPass();
	firstPass();

	if (PostProcess)
	{
		bloomHighlightPass();
		horizontalBlurPass();
		verticalBlurPass();
		finalPass();
	}
	

	return true;
}

void App1::firstPass()
{
	if (PostProcess)
	{
		renderTexture->setRenderTarget(renderer->getDeviceContext());
		renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.3f, 0.3f, 0.3f, 1.0f);
	}
	else
	{
		renderer->beginScene(0.3f, 0.3f, 0.3f, 1.0f);

	}

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	//renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	ID3D11ShaderResourceView* pointmaps[6];
	for (int i = 0; i < 6; i++)
	{
		pointmaps[i] = pointLightShadowMaps[i]->getDepthMapSRV();
	}

	light2->light->setSpecularColour(0, 0, 0, 0);
	mesh->sendData(renderer->getDeviceContext());
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"heightMap"), lights, scale, camera->getPosition(), lightProjMatrices, lightViewMatrices, directionalLightShadowMap->getDepthMapSRV(), pointmaps);
	manipulationShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heightMap"), lights, camera->getPosition());
	//lightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	cube->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"));
	textureShader->render(renderer->getDeviceContext(), cube->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.1f, 0.6f, 0.1f);
	
	light2->light->setSpecularColour(0.5, 0.5, 0.5, 0.5);
	waterMesh->sendData(renderer->getDeviceContext());
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"heightMap"), lights, 0, camera->getPosition(), lightProjMatrices, lightViewMatrices, directionalLightShadowMap->getDepthMapSRV(), pointmaps);
	manipulationShader->render(renderer->getDeviceContext(), waterMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(5.f, 1.f, 5.f);
	scaleMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(90));
	scaleMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	
	worldMatrix = XMMatrixMultiply(scaleMatrix, worldMatrix);

	model->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"wood"));
	textureShader->render(renderer->getDeviceContext(), model->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();


	/*campFire->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"));
	textureShader->render(renderer->getDeviceContext(), model->getIndexCount());*/


	gui();
	if (PostProcess)
	{
		// Swap the buffers
		renderer->setBackBufferRenderTarget();
	}
	else
	{
		renderer->endScene();
	}
}

void App1::bloomHighlightPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();
	bloomHighlightTexture->setRenderTarget(renderer->getDeviceContext());
	bloomHighlightTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	bloomHighlightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, renderTexture->getShaderResourceView());
	bloomHighlightShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::horizontalBlurPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, bloomHighlightTexture->getShaderResourceView(), screenSizeX);
	horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::verticalBlurPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)verticalBlurTexture->getTextureHeight();
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSizeY);
	verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::depthPass()
{
	directionalLightShadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	light->light->generateViewMatrix();
	//light->light->generateProjectionMatrix(1.0f, 100.0f);
	light->light->generateOrthoMatrix((float)100, (float)100, 1.0f, 100.f);


	lightViewMatrices[0] = light->light->getViewMatrix();
	lightProjMatrices[0] = light->light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	shadowMesh->sendData(renderer->getDeviceContext());
	depthHeightmapShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrices[0], lightProjMatrices[0], textureMgr->getTexture(L"heightMap"), scale);
	depthHeightmapShader->render(renderer->getDeviceContext(), shadowMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);


	cube->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrices[0], lightProjMatrices[0]);
	depthShader->render(renderer->getDeviceContext(), cube->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(5.f, 1.f, 5.f);
	scaleMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(90));
	scaleMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);

	worldMatrix = XMMatrixMultiply(scaleMatrix, worldMatrix);

	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrices[0], lightProjMatrices[0]);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::pointDepthPass()
{
	for (int i = 0; i < 6; i++)
	{
		pointLightShadowMaps[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
		//renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.5f, 0.5f, 0.5f, 0.5f);


		XMFLOAT3 direction;
		switch (i)
		{
		case 0:
		{
			direction = XMFLOAT3(1, 0, 0);
			break;
		}
		case 1:
		{
			direction = XMFLOAT3(-1, 0, 0);
			break;
		}
		case 2:
		{
			direction = XMFLOAT3(0, 1, 0);
			break;
		}
		case 3:
		{
			direction = XMFLOAT3(0, -1, 0);
			break;
		}
		case 4:
		{
			direction = XMFLOAT3(0, 0, 1);
			break;
		}
		case 5:
		{
			direction = XMFLOAT3(0, 0, -1);
			break;
		}
		default:
		{
			OutputDebugStringW(L"fell out of switch case in point light depth pass");
		}
		}

		light2->light->setDirection(direction.x, direction.y, direction.z);
		light2->light->generateViewMatrix();
		light2->light->generateProjectionMatrix(1.0f, 100.0f);
		light2->light->setLookAt(light2->light->getDirection().x, light2->light->getDirection().y, light2->light->getDirection().z);
		
		lightViewMatrices[i+1] = light2->light->getViewMatrix();
		lightProjMatrices[i + 1] = light2->light->getProjectionMatrix();//XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), float(screenWidth1) / float(screenHeight1), 1.0f, 100.0f);
		XMMATRIX worldMatrix = renderer->getWorldMatrix();


		shadowMesh->sendData(renderer->getDeviceContext());
		depthHeightmapShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrices[i+1], lightProjMatrices[i+1],  textureMgr->getTexture(L"heightMap"), scale);
		depthHeightmapShader->render(renderer->getDeviceContext(), shadowMesh->getIndexCount());

		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
		XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);


		cube->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrices[i+1], lightProjMatrices[i+1]);
		depthShader->render(renderer->getDeviceContext(), cube->getIndexCount());

		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(5.f, 1.f, 5.f);
		scaleMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);
		XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(90));
		scaleMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);

		worldMatrix = XMMatrixMultiply(scaleMatrix, worldMatrix);

		model->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrices[i+1], lightProjMatrices[i+1]);
		depthShader->render(renderer->getDeviceContext(), model->getIndexCount());
			
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.3f, 0.3f, 0.3f, 1.0f);

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	bloomFinalShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, verticalBlurTexture->getShaderResourceView(), renderTexture->getShaderResourceView());
	bloomFinalShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	

	debugOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, bloomHighlightTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), debugOrthoMesh->getIndexCount());

	renderer->setZBuffer(true);
	// Render GUI

	// Present the rendered scene to the screen.
	renderer->endScene();
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

	ImGui::Checkbox("Post processing effect", &PostProcess);

	lightPos = light->light->getPosition();
	////ImGui::SliderFloat("lightpos x", &lightPos.x, -30, 30);
	////ImGui::SliderFloat("lightpos y", &lightPos.y, -30, 30);
	////ImGui::SliderFloat("lightpos z", &lightPos.z, -30, 30);
	//ImGui::SliderFloat3("directional light position", &lightPos.x, -30, 30);
	

	lightDir = light->light->getDirection();
	/*ImGui::SliderFloat("lightdir x", &lightDir.x, -30, 30);
	ImGui::SliderFloat("lightdir y", &lightDir.y, -30, 30);
	ImGui::SliderFloat("lightdir z", &lightDir.z, -30, 30);*/
	ImGui::SliderFloat3("directional light direction", &lightDir.x, -2, 2);

	if (lightDir.x > 0)
	{
		lightPos.x = -10;
	}
	else
	{
		lightPos.x = 30;
	}

	if (lightDir.z > 0)
	{
		lightPos.z = -10;
	}
	else
	{
		lightPos.z = 30;
	}

	light->light->setPosition(lightPos.x, lightPos.y, lightPos.z);
	light->light->setDirection(lightDir.x, lightDir.y, lightDir.z);


	ImGui::SliderFloat("Scale: ", &scale, 0.0f, 50.0f);

	pointLightPos = light2->light->getPosition();
	/*ImGui::SliderFloat("pointlightpos x", &pointLightPos.x, -30, 30);
	ImGui::SliderFloat("pointlightpos y", &pointLightPos.y, -30, 30);
	ImGui::SliderFloat("pointlightpos z", &pointLightPos.z, -30, 30);*/
	ImGui::SliderFloat3("point light position", &pointLightPos.x, -30, 30);
	light2->light->setPosition(pointLightPos.x, pointLightPos.y, pointLightPos.z);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

