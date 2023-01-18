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

#include "../Engine/macros.h"    // for some macros utils
#include "../Engine/Log.h"       // for using a logger
//#include "textureclass.h"      // for using a texture for models
#include "TextureArrayClass.h"   // for using multiple textures for models
#include "modelconverterclass.h" // for converting a model data from other types (obj, etc.) into our internal model type
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass
{
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
		float nx, ny, nz;      // normal
		float tx, ty, tz;      // tangent
		float bx, by, bz;      // binormal
		float cr, cg, cb, ca;  // colour (RGBA)
	};


	// the following two structures will be used for calsulation the tangen and binormal
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};


public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	
	// initialize a model using only its vertices data (position, texture, normal)
	bool Initialize(ID3D11Device* pDevice, const VERTEX* verticesData, const int vertexCount, string modelName = "custom"); 

	// initialization of the model's vertex and index buffers using some model data (a file with data) and textures
	bool Initialize(ID3D11Device* device, 
					std::string modelName, 
					WCHAR* texture1, 
					WCHAR* texture2, 
					WCHAR* texture3 = nullptr);		
	void Shutdown(void);
	void Render(ID3D11DeviceContext* pDeviceContext);	// The Render() function puts the model geometry on the video card to prepare 
										
	bool AddTextures(ID3D11Device* pDevice, 
					 WCHAR* texture1, 
					 WCHAR* texture2, 
					 WCHAR* texture3 = nullptr);

	// getters 
	int GetIndexCount();
	ID3D11ShaderResourceView** GetTextureArray();       // returns a pointer to the array of textures
	const DirectX::XMMATRIX & GetWorldMatrix();         // returns a model's world matrix

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
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

	bool LoadModel(std::string modelName);

	// function for calculating the tangent and binormal vectors for the model
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	VertexBuffer<VERTEX> vertexBuffer_;     // for work with a model vertex buffer
	IndexBuffer          indexBuffer_;      // for work with a model index buffer
	TextureArrayClass    textureArray_;     // for work with multiple textures

	ModelConverterClass modelConverter_;    // for converting models to different formats
	ModelType* pModelType_ = nullptr;

	// model properties in the world
	DirectX::XMMATRIX modelWorldMatrix_;
	DirectX::XMFLOAT3 position_;        // position of the model
	DirectX::XMFLOAT3 scale_;           // scale of the model
	DirectX::XMFLOAT2 radianAngle_;     // current angles of the model rotation (in radians)

	int vertexCount_ = 0;
	int indexCount_ = 0;
};

