#pragma once

#include <memory>                                // for std::unique_ptr

#include "../GameObjects/Vertex.h"
#include "../GameObjects/VertexBuffer.h"
#include "../GameObjects/IndexBuffer.h"                // for using an index buffer's functional
#include "../GameObjects/textureclass.h"


class MeshObject final
{
private:
	struct MeshData
	{
		VertexBuffer<VERTEX> vertexBuffer_;     // for work with a model vertex buffer
		IndexBuffer  indexBuffer_;      // for work with a model index buffer						
		std::vector<TextureClass> texturesArr_;
		//DirectX::XMMATRIX transformMatrix_;
	};

public:
	MeshObject(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT> & indicesArr,
		std::vector<TextureClass> & texturesArr,
		const DirectX::XMMATRIX & transformMatrix,
		const bool isVertexBufferDynamic);

	MeshObject(const MeshObject & mesh);
	~MeshObject();

	

	////////////////////////////////////////////////

	void Draw(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Copy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const MeshObject & inMesh);

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
	MeshObject & MeshObject::operator=(const MeshObject & mesh);

private:
	MeshData data_;
};