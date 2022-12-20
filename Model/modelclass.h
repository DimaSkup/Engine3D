/////////////////////////////////////////////////////////////////////
// Filename:     modelclass.h
// Description:  the ModelClass is responsible for encapsulating 
//               the geometry for 3DModels, converting model data,
//               texturing;
//
// Revising:     24.10.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
//#include <d3dx10math.h>
#include <fstream>
#include <DirectXMath.h>

#include "../Engine/macros.h"    // for some macros utils
#include "../Engine/Log.h"       // for using a logger
#include "textureclass.h"        // for using a texture for models
#include "modelconverterclass.h" // for converting a model data from other types (obj, etc.) into our internal model type
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	
	bool Initialize(ID3D11Device* pDevice, const VERTEX* verticesData,  // initialize a model using only its vertices data (position, texture, normal)
					const int vertexCount,
					string modelName = "custom"); 

	bool Initialize(ID3D11Device* device, std::string modelName, WCHAR* textureName);		// The function here handles initializing of the model's vertex and index buffers using some model data and texture
	void Shutdown(void);
	void Render(ID3D11DeviceContext* pDeviceContext);	// The Render() function puts the model geometry on the video card to prepare 
										


	// getters 
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();	     // also can pass its own texture resource to shaders that will draw this model
	const DirectX::XMMATRIX & GetWorldMatrix();  // returns a model's world matrix

	// modificators of the model
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float angleX, float angleY);
	 
	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(size_t i);
	void operator delete(void* p);


protected:

	// functions for work with a vertex and index buffers
	bool InitializeBuffers(ID3D11Device* pDevice);

	bool LoadModel(std::string modelName);
	void ReleaseModel();

	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

	// functions to handle loading and releasing the texture data from the .dds file
	bool LoadTexture(ID3D11Device* pDevice, WCHAR* textureName);
	void ReleaseTexture();

protected:
	// internal representation of a model vertex structure
	struct ModelType
	{
		ModelType()
		{
			// by default we set a purple colour for each vertex
			cr = 1.0f;
			cg = 0.0f;
			cb = 1.0f;
			ca = 1.0f;  // alpha
		}

		float x, y, z;         // position coords
		float tu, tv;          // texture coords
		float nx, ny, nz;      // normals
		float cr, cg, cb, ca;  // colours (cr - red, cg - green, cb - blue, ca - alpha)
	};


	VertexBuffer<VERTEX> vertexBuffer_;     // for work with a model vertex buffer
	IndexBuffer          indexBuffer_;      // for work with a model index buffer
	TextureClass* pTexture_ = nullptr;      // for work with textures

	ModelConverterClass modelConverter;     // for converting models to different formats
	ModelType* pModelType_ = nullptr;

	int vertexCount_ = 0;
	int indexCount_ = 0;

	// model properties in the world
	DirectX::XMMATRIX modelWorldMatrix_;
	DirectX::XMFLOAT3 position_;        // position of the model
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT2 radianAngle_;     // current angles of the model rotation (in radians)
};

