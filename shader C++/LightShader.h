#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

//static const int maxLights = 8;

//struct MyLight
//{
//public:
//	Light* light = new Light;
//	float cone = 0.0f;
//	XMFLOAT4 attenuation = XMFLOAT4(0, 0, 0, 0);
//	XMFLOAT4 type = XMFLOAT4(0, 0, 0, 0);
//};

class LightShader : public BaseShader
{
private:
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

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

public:
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, MyLight* lights[maxLights], XMFLOAT3 camPos);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
};

