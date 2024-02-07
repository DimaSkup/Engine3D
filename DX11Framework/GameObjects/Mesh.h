#pragma once

#include <memory>                                // for std::unique_ptr

#include "../GameObjects/Vertex.h"
#include "../GameObjects/VertexBuffer.h"
#include "../GameObjects/IndexBuffer.h"                // for using an index buffer's functional
#include "../GameObjects/textureclass.h"

class Mesh final
{
private:
	struct MeshData
	{
		VertexBuffer vertexBuffer_;     // for work with a model vertex buffer
		IndexBuffer  indexBuffer_;      // for work with a model index buffer						
		std::vector<TextureClass> texturesArr_;
		//DirectX::XMMATRIX transformMatrix_;
	};

public:
	Mesh(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT> & indicesArr,
		std::vector<TextureClass> & texturesArr,
		const DirectX::XMMATRIX & transformMatrix,
		const bool isVertexBufferDynamic);

	Mesh(const Mesh & mesh);
	~Mesh();

	

	////////////////////////////////////////////////

	void Draw(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Copy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const Mesh & inMesh);

	void SetTextureByIndex(const UINT index, const std::string & texturePath, aiTextureType type);
	void UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext, const std::vector<VERTEX> & newVerticesArr);


	////////////////////////////////////////////////
	// GETTERS
	////////////////////////////////////////////////
	const MeshData & GetMeshData() const;

	//const DirectX::XMMATRIX & GetTransformMatrix() const;
#if 0
	const std::vector<std::unique_ptr<TextureClass>> & GetTexturesArr() const;
	ID3D11Buffer* const* GetAddressOfVertexBuffer() const;
	const UINT* GetAddressOfVertexBufferStride() const;
	ID3D11Buffer* GetIndexBuffer() const;

	const UINT GetVertexCount() const;
	const UINT GetIndexCount() const;
#endif

private:
	// restrict an assignment operator
	Mesh & Mesh::operator=(const Mesh & mesh);

private:
	MeshData data_;
};