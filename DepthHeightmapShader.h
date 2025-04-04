#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthHeightmapShader : public BaseShader
{
private:
	struct ScaleBufferType
	{
		float scale;
		XMFLOAT3 padding;
	};
public:

	DepthHeightmapShader(ID3D11Device* device, HWND hwnd);
	~DepthHeightmapShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float scale);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* scaleBuffer;
};

