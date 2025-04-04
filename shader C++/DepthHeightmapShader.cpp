#include "DepthHeightmapShader.h"

DepthHeightmapShader::DepthHeightmapShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depthHeightmap_vs.cso", L"depth_ps.cso");
}

DepthHeightmapShader::~DepthHeightmapShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthHeightmapShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC ScaleBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	ScaleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ScaleBufferDesc.ByteWidth = sizeof(ScaleBufferType);
	ScaleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ScaleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ScaleBufferDesc.MiscFlags = 0;
	ScaleBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&ScaleBufferDesc, NULL, &scaleBuffer);

}

void DepthHeightmapShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, float scale)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	ScaleBufferType* dataPtr2;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->Map(scaleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr2 = (ScaleBufferType*)mappedResource.pData;
	dataPtr2->scale = scale;
	dataPtr2->padding = XMFLOAT3(0, 0, 0);
	deviceContext->Unmap(scaleBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &scaleBuffer);

	deviceContext->VSSetShaderResources(0, 1, &texture);
}