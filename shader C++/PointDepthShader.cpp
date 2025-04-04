#include "PointDepthShader.h"

PointDepthShader::PointDepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"pointDepth_vs.cso", L"pointDepth_ps.cso");
}

PointDepthShader::~PointDepthShader()
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
	PointDepthShader::~PointDepthShader();
}

void PointDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC psBufferDesc;


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

	psBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	psBufferDesc.ByteWidth = sizeof(PSBufferType);
	psBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	psBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	psBufferDesc.MiscFlags = 0;
	psBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&psBufferDesc, NULL, &psBuffer);

}

void PointDepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT3 pointLightPos)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	PSBufferType* psPtr;

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

	deviceContext->Map(psBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	psPtr = (PSBufferType*)mappedResource.pData;
	psPtr->lightPos = pointLightPos;
	psPtr->pad = 0;
	deviceContext->Unmap(psBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &psBuffer);
}
