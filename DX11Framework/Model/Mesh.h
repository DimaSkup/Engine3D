#pragma once

#include "../Model/Vertex.h"
#include "../Model/VertexBuffer.h"               // for using a vertex buffer's functional
#include "../Model/IndexBuffer.h"                // for using an index buffer's functional

//#include <assimp/Importer.hpp>
//#include <assimp/postprocess.h>
//#include <assimp/scene.h>

class Mesh
{
public:
	Mesh();
	Mesh(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<VERTEX> & vertices,
		const std::vector<UINT> & indices);

	Mesh(const Mesh & mesh);
	~Mesh();

	void Draw(D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// GETTERS
	bool UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext, 
		const std::vector<VERTEX> & newVerticesArr);
	//VertexBuffer<VERTEX>* GetVertexBufferPtr() const;
	//IndexBuffer* GetIndexBufferPtr() const;
	const UINT GetVertexCount() const;
	const UINT GetIndexCount() const;

private:
	VertexBuffer<VERTEX>* pVertexBuffer_ = nullptr;     // for work with a model vertex buffer
	IndexBuffer*          pIndexBuffer_ = nullptr;      // for work with a model index buffer						
	ID3D11DeviceContext*  pDeviceContext_ = nullptr;

};