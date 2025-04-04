#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

static const int maxLights = 8;


struct MyLight
{
public:
	Light* light = new Light;
	float cone = 0.0f;
	XMFLOAT4 attenuation = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT4 type = XMFLOAT4(0, 0, 0, 0);
};

class ManipulationShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[maxLights];
		XMMATRIX lightProjection[maxLights];
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[maxLights];
		XMFLOAT4 diffuse[maxLights];
		XMFLOAT4 position[maxLights];
		XMFLOAT4 specular[maxLights];
		XMFLOAT4 attenuation[maxLights];
		XMFLOAT4 lightDirection[maxLights];
		XMFLOAT4 lightType[maxLights];
	};

	struct VertexBufferType
	{
		float scale;
		XMFLOAT3 camPos;
	};

	struct TessBufferType
	{
		XMFLOAT4 tessfactors;
		XMFLOAT2 insides;
		XMFLOAT2 padding;
		XMFLOAT4 camPos;
	};


public:
	ManipulationShader(ID3D11Device* device, HWND hwnd);
	~ManipulationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture1, MyLight* lights[maxLights], float scale, XMFLOAT3 camPos, const XMMATRIX lightProjectionMatrix[maxLights], const XMMATRIX lightViewMatrix[maxLights], ID3D11ShaderResourceView* shadowmap, ID3D11ShaderResourceView* pointlightmaps[6]);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);


private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* tessBuffer;

};

