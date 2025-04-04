#include "PointDepthHeightShader.h"

PointDepthHeightShader::PointDepthHeightShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"pointDepthHeight_vs.cso", L"pointDepth_ps.cso");
}

PointDepthHeightShader::~PointDepthHeightShader()
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
	PointDepthHeightShader::~PointDepthHeightShader();
}

void PointDepthHeightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC scaleBufferDesc;
	D3D11_BUFFER_DESC psBufferDesc;

	D3D11_SAMPLER_DESC samplerDesc;

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

	scaleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	scaleBufferDesc.ByteWidth = sizeof(ScaleBufferType);
	scaleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	scaleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	scaleBufferDesc.MiscFlags = 0;
	scaleBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&scaleBufferDesc, NULL, &scaleBuffer);

	psBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	psBufferDesc.ByteWidth = sizeof(PSBufferType);
	psBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	psBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	psBufferDesc.MiscFlags = 0;
	psBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&psBufferDesc, NULL, &psBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

}

void PointDepthHeightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT3 pointLightPos, ID3D11ShaderResourceView* texture, float scale)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	ScaleBufferType* dataPtr2;
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

	deviceContext->Map(scaleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr2 = (ScaleBufferType*)mappedResource.pData;
	dataPtr2->scale = scale;
	dataPtr2->padding = XMFLOAT3(0, 0, 0);
	deviceContext->Unmap(scaleBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &scaleBuffer);

	deviceContext->Map(psBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	psPtr = (PSBufferType*)mappedResource.pData;
	psPtr->lightPos = pointLightPos;
	psPtr->pad = 0;
	deviceContext->Unmap(psBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &psBuffer);

	deviceContext->VSSetShaderResources(0, 1, &texture);
	deviceContext->VSSetSamplers(0, 1, &sampleState);


}