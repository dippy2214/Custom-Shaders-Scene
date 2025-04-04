// Colour shader.h
// Simple shader example.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class PointDepthHeightShader : public BaseShader
{

public:
	struct PSBufferType
	{
		XMFLOAT3 lightPos;
		float pad;
	};

	struct ScaleBufferType
	{
		float scale;
		XMFLOAT3 padding;
	};

	PointDepthHeightShader(ID3D11Device* device, HWND hwnd);
	~PointDepthHeightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT3 pointLightPos, ID3D11ShaderResourceView* texture, float scale);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* scaleBuffer;
	ID3D11Buffer* psBuffer;

	ID3D11SamplerState* sampleState;

};