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
class ModelClass : public GraphicsComponent, public ModelData
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

	// The Render() function puts the model geometry on the video card to prepare 
	// and renders the model using some particular related shader
	virtual void Render(ID3D11DeviceContext* pDeviceContext);	

	bool AddTexture(ID3D11Device* pDevice, WCHAR* textureName);   // add a new texture at the end of the textures list
	bool SetTexture(ID3D11Device* pDevice, WCHAR* textureName, UINT index);  // set a new texture by some particular index

	// common getters 
	std::string GetPathToDefaultModelsDir() const;
	ID3D11ShaderResourceView* const* GetTextureResourcesArray();       // returns a pointer to the array of textures

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);

protected:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);
	bool LoadModel(std::string modelName);
	

protected:
	VertexBuffer<VERTEX> vertexBuffer_;     // for work with a model vertex buffer
	IndexBuffer          indexBuffer_;      // for work with a model index buffer
	TextureArrayClass    texturesList_;     // for work with multiple textures

private:
	ModelData            modelData_;
	std::string          defaultModelsDirPath_{ "internal/" };
};

