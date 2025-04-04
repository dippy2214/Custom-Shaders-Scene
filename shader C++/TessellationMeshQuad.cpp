#include "TessellationMeshQuad.h"

TessellationMeshQuad::TessellationMeshQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	initBuffers(device);
}

// Release resources.
TessellationMeshQuad::~TessellationMeshQuad()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build triangle (with texture coordinates and normals).
void TessellationMeshQuad::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	int resolution = 20;

	int index, i, j;
	float positionX, positionZ, u, v, increment;

	vertexCount = (resolution - 1) * (resolution - 1) * 4;
	indexCount = (resolution - 1) * (resolution - 1) * 4;

	index = 0;
	// UV coords.
	u = 0;
	v = 0;
	increment = 1.0f / resolution;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];


	for (j = 0; j < (resolution - 1); j++)
	{
		for (i = 0; i < (resolution - 1); i++)
		{
			// lower left
			positionX = (float)(i);
			positionZ = (float)(j + 1);


			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u, v + increment);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;
			// Upper left.
			positionX = (float)(i);
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u + increment, v + increment);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Bottom right
			positionX = (float)(i + 1);
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u + increment, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			u += increment;

		}

		u = 0;
		v += increment;
	}

	//// Load the vertex array with data.
	//vertices[0].position = XMFLOAT3(-1.0f, 0.0f, -1.0f);  // Top.
	//vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	//vertices[0].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	//vertices[1].position = XMFLOAT3(-1.0f, 0.0f, 1.0f);  // Bottom left.
	//vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	//vertices[1].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	//vertices[2].position = XMFLOAT3(1.0f, 0.0f, 1.0f);  // Bottom right.
	//vertices[2].texture = XMFLOAT2(1.0f, 0.0f);
	//vertices[2].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	//vertices[3].position = XMFLOAT3(1.0f, 0.0f, -1.0f);  // Bottom right.
	//vertices[3].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[3].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	//// Load the index array with data.
	//indices[0] = 1;  // Top/
	//indices[1] = 0;  // Bottom left.
	//indices[2] = 2;  // Bottom right.
	//indices[3] = 3;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

// Override sendData() to change topology type. Control point patch list is required for tessellation.
void TessellationMeshQuad::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(top);
}
