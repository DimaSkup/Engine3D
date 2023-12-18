#pragma once

#include <memory>                                // for std::unique_ptr

#include "../Model/Vertex.h"
#include "../Model/VertexBufferInterface.h"      // for using a vertex buffer's functional
#include "../Model/VertexBuffer.h"
#include "../Model/IndexBuffer.h"                // for using an index buffer's functional
#include "../Model/textureclass.h"

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<VERTEX> & vertices,
		const std::vector<UINT> & indices,
		std::vector<std::unique_ptr<TextureClass>> & texturesArr);

	Mesh(const Mesh & mesh);
	~Mesh();

	Mesh & Mesh::operator=(const Mesh & mesh);

	void Draw(D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void SetTextureByIndex(const UINT index, const std::string & texturePath, aiTextureType type);
	void AddTexture(const std::string & texturePath);
	const std::vector<std::unique_ptr<TextureClass>> & GetTexturesArr() const;

	// GETTERS
	bool UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext, 
		const std::vector<VERTEX> & newVerticesArr);
	//VertexBuffer<VERTEX>* GetVertexBufferPtr() const;
	//IndexBuffer* GetIndexBufferPtr() const;
	const UINT GetVertexCount() const;
	const UINT GetIndexCount() const;

private:
	std::unique_ptr<VertexBufferInterface> pVertexBuffer_;     // for work with a model vertex buffer
	std::unique_ptr<IndexBuffer>           pIndexBuffer_;      // for work with a model index buffer						

	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext*  pDeviceContext_ = nullptr;

	std::vector<std::unique_ptr<TextureClass>> texturesArr_;
};