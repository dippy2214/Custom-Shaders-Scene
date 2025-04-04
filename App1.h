// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "ManipulationShader.h"
#include "TessellationMeshQuad.h"
#include "TextureShader.h"
#include "VerticalBlurShader.h"
#include "HorizontalBlurShader.h"
#include "bloomHighlightShader.h"
#include "BloomFinalShader.h"
#include "DepthShader.h"
#include "DepthHeightmapShader.h"
#include "PlaneMesh.h"
#include "PointDepthShader.h"
#include "pointDepthHeightShader.h"
#include "Model.h"
//#include "LightShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void firstPass();

	void bloomHighlightPass();
	void horizontalBlurPass();
	void verticalBlurPass();

	void depthPass();
	void pointDepthPass();

	void finalPass();
	void gui();

private:
	ManipulationShader* manipulationShader;
	//LightShader* lightShader;
	TessellationMeshQuad* mesh;
	MyLight* light;
	MyLight* light2;

	MyLight* lights[maxLights];

	float scale = 1.5f;

	TextureShader* textureShader;
	TextureShader* textureShader2;
	RenderTexture* renderTexture;
	
	OrthoMesh* orthoMesh;

	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	VerticalBlurShader* verticalBlurShader;
	HorizontalBlurShader* horizontalBlurShader;

	RenderTexture* bloomHighlightTexture;
	BloomHighlightShader* bloomHighlightShader;

	BloomShaderFinal* bloomFinalShader;

	ShadowMap* directionalLightShadowMap;
	DepthHeightmapShader* depthHeightmapShader;

	OrthoMesh* debugOrthoMesh;
	RenderTexture* debugRenderTexture;

	PlaneMesh* shadowMesh;


	int screenWidth1;
	int screenHeight1;

	XMMATRIX lightProjMatrices[maxLights];
	XMMATRIX lightViewMatrices[maxLights];

	CubeMesh* cube;

	DepthShader* depthShader;

	XMFLOAT3 lightPos;
	XMFLOAT3 lightDir;

	XMFLOAT3 pointLightPos;

	ShadowMap* pointLightShadowMaps[6];

	bool PostProcess = false;

	TessellationMeshQuad* waterMesh;

	Model* model;
	Model* campFire;
};

#endif