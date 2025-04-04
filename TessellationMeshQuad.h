// The tessellator uses these points to generate new geometry but sub-division.
// Due to change in geometry type the sendData() function has been overridden.

#ifndef _TESSELLATIONMESHQUAD_H_
#define _TESSELLATIONMESHQUAD_H_

#include "..\include\BaseMesh.h"
class TessellationMeshQuad : public BaseMesh
{

public:
	TessellationMeshQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~TessellationMeshQuad();

	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST) override;

protected:
	void initBuffers(ID3D11Device* device);

};

#endif

