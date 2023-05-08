/////////////////////////////////////////////////////////////////////
// Filename:     modelclass.h
// Description:  the ModelClass is responsible for encapsulating 
//               the geometry for 3DModels, converting model data,
//               texturing;
//
// Revising:     09.01.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
//#include <d3dx10math.h>
#include <fstream>
#include <DirectXMath.h>
#include <memory>                // for using unique_ptr

#include "../Engine/macros.h"    // for some macros utils
#include "../Engine/Log.h"       // for using a logger
//#include "textureclass.h"      // for using a texture for models
#include "TextureArrayClass.h"   // for using multiple textures for models
#include "modelconverterclass.h" // for converting a model data from other types (obj, etc.) into our internal model type
#include "ModelLoader.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ModelMath.h"
#include "ModelData.h"
#include "../Model/ModelMediator.h"


//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass : public GraphicsComponent
{
public:
	ModelClass();
	ModelClass(const ModelClass & copy);
	virtual ~ModelClass();

	// initialization of the model's vertex and index buffers using some model data (a file with data) and textures
	virtual bool Initialize(ID3D11Device* pDevice) { return true; };
	bool Initialize(ID3D11Device* device, const std::string& modelId);
	bool InitializeCopy(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId);

	void Shutdown(void);
	void ClearModelData();  // release memory from the model vertices/indices data

	// The Render() function puts the model geometry on the video card to prepare and renders the model using some particular related shader
	virtual void Render(ID3D11DeviceContext* pDeviceContext);	

	bool AddTexture(ID3D11Device* pDevice, WCHAR* textureName);   // add a new texture at the end of the textures list
	bool SetTexture(ID3D11Device* pDevice, WCHAR* textureName, UINT index);  // set a new texture by some particular index

	bool InitializeBuffers(ID3D11Device* pDevice, VERTEX* pVerticesData, UINT* pIndicesData, UINT vertexCount, UINT indexCount);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// common getters 
	VERTEX* GetVerticesData();
	UINT* GetIndicesData();
	UINT GetVertexCount() const;
	UINT GetIndexCount() const;
	VERTEX** GetAddressOfVerticesData();
	UINT** GetAddressOfIndicesData();

	std::string GetPathToDefaultModelsDir() const;
	ID3D11ShaderResourceView* const* GetTextureResourcesArray();       // returns a pointer to the array of textures

	// common setters
	void SetID(const std::string& modelID);
	void SetVertexCount(UINT vertexCount);
	void SetIndexCount(UINT indexCount);

	// memory allocation
	void AllocateVerticesAndIndicesArray(UINT vertexCount, UINT indexCount);
	void WriteVertexDataByIndex(const VERTEX & vertexData);
	void WriteIndexDataByIndex(UINT index, UINT indexData);

	void* operator new(std::size_t count);                              // a replaceable allocation function
	void* operator new(std::size_t count, const std::nothrow_t & tag);  // a replaceable non-throwing allocation function
	void* operator new(std::size_t count, void* ptr);                   // a non-allocating placement allocation function
	void operator delete(void* p) noexcept;

protected:
	bool LoadModel(std::string modelName);
	

protected:
	VertexBuffer<VERTEX>* pVertexBuffer_ = nullptr;     // for work with a model vertex buffer
	IndexBuffer*          pIndexBuffer_ = nullptr;      // for work with a model index buffer
	TextureArrayClass*    pTexturesList_ = nullptr;     // for work with multiple textures

private:
	ModelData*            pData_ = nullptr;
	std::string           defaultModelsDirPath_{ "internal/" };
};

