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
#include "../Model/ModelMediator.h"



//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass : public GraphicsComponent, ModelMath
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	virtual ~ModelClass();

	// initialization of the model's vertex and index buffers using some model data (a file with data) and textures
	virtual bool Initialize(ID3D11Device* pDevice) { return true; };
	bool Initialize(ID3D11Device* device, const std::string& modelId);
	bool Initialize(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId);

	//virtual bool Copy(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId);

	void Shutdown(void);

	// The Render() function puts the model geometry on the video card to prepare 
	// and renders the model using some particular related shader
	void Render(ID3D11DeviceContext* pDeviceContext);	


	bool AddTexture(ID3D11Device* pDevice, WCHAR* texture);
	//void SetRelatedShader(std::string shaderName);
	void SetModelType(const std::string& modelFilename);
	void SetID(const std::string& modelID);


	// common getters 
	std::string GetPathToDefaultModelsDir() const;
	int GetVertexCount() const;
	int GetIndexCount() const;
	ID3D11ShaderResourceView** GetTextureArray();       // returns a pointer to the array of textures
	//const std::string& GetRelatedShader() const;        // returns a name of the related shader which used for rendering of the model
	const DirectX::XMMATRIX & GetWorldMatrix();         // returns a model's world matrix
	const std::string & GetID();                        // returns an identifier of the model

	// modificators of the model
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float angleX, float angleY);
	void SetColor(float red, float green, float blue, float alpha);

	const DirectX::XMFLOAT3 & GetPosition() const;
	const DirectX::XMFLOAT3 & GetScale() const;
	const DirectX::XMFLOAT2 & GetRotation() const;
	const DirectX::XMFLOAT4 & GetColor() const;
	 
	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(size_t i);
	void operator delete(void* p);

protected:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

	bool LoadModel(std::string modelName);
	


protected:
	VertexBuffer<VERTEX> vertexBuffer_;     // for work with a model vertex buffer
	IndexBuffer          indexBuffer_;      // for work with a model index buffer
	TextureArrayClass    textureArray_;     // for work with multiple textures



	VERTEX* pModelData_ = nullptr;
	UINT* pIndicesData_ = nullptr;
	//ModelType* pModelType_ = nullptr;

	// model properties in the world
	DirectX::XMMATRIX modelWorldMatrix_;
	DirectX::XMFLOAT3 position_;        // position of the model in the world
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT2 radianAngle_;     // current angles of the model rotation (in radians)
	DirectX::XMFLOAT4 color_;           // color of the model

	// we need these variables because we use this data during model math calculations
	size_t vertexCount_ = 0;
	size_t indexCount_ = 0;

	std::string modelFilename_{ "" };
	std::string modelID_{ "" };
};

