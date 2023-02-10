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
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ModelMath.h"
#include "../Model/ModelMediator.h"



//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass : public GraphicsComponent
{
protected:
	// internal representation of a model vertex structure
	struct ModelType
	{
		ModelType()
		{
			x = y = z = 0.0f;
			tu = tv = 0.0f;
			nx = ny = nz = 0.0f;
			tx = ty = tz = 0.0f;
			bx = by = bz = 0.0f;
			cr = cg = cb = ca = 1.0f;  // by default we set a white colour for each vertex
		}

		float x, y, z;         // position coords
		float tu, tv;          // texture coords
		float nx, ny, nz;      // normal
		float cr, cg, cb, ca;  // colour (RGBA)
		float tx, ty, tz;      // tangent
		float bx, by, bz;      // binormal
		
	};




public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// initialization of the model's vertex and index buffers using some model data (a file with data) and textures
	virtual bool Initialize(ID3D11Device* device, const std::string& modelId);
	virtual bool Initialize(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId);

	void Shutdown(void);

	// The Render() function puts the model geometry on the video card to prepare 
	// and renders the model using some particular related shader
	void Render(ID3D11DeviceContext* pDeviceContext);	
										
	bool AddTexture(ID3D11Device* pDevice, WCHAR* texture);
	void SetRelatedShader(std::string shaderName);
	void SetModel(const std::string& modelFilename);
	void SetID(const std::string& modelID);


	// getters 
	int GetVertexCount() const;
	int GetIndexCount() const;
	ID3D11ShaderResourceView** GetTextureArray();       // returns a pointer to the array of textures
	const std::string& GetRelatedShader() const;              // returns a name of the related shader which used for rendering of the model
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

	// functions for work with a vertex and index buffers
	bool InitializeBuffers(ID3D11Device* pDevice);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

	bool LoadModel(std::string modelName);


	VertexBuffer<VERTEX> vertexBuffer_;     // for work with a model vertex buffer
	IndexBuffer          indexBuffer_;      // for work with a model index buffer
	TextureArrayClass    textureArray_;     // for work with multiple textures

	ModelConverterClass modelConverter_;    // for converting models to different formats
	ModelType* pModelType_ = nullptr;

	// model properties in the world
	DirectX::XMMATRIX modelWorldMatrix_;
	DirectX::XMFLOAT3 position_;        // position of the model in the world
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT2 radianAngle_;     // current angles of the model rotation (in radians)
	DirectX::XMFLOAT4 color_;           // color of the model

	// we need these variables because we use this data during model math calculations
	int vertexCount_ = 0;
	int indexCount_ = 0;

	std::string modelFilename_{ "" };
	std::string modelID_{ "" };
	std::string relatedShader_{ "colorShaderClass" };    // a name of the shader which renders this model
};

