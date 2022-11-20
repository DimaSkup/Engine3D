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

//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	//bool Initialize(ID3D11Device* pDevice, const VERTEX_2D* vertices); // initialize a 2D model using only its vertices data (position, colour)
	
	// initialize a model using only its vertices data (position, texture, normal)
	bool Initialize(ID3D11Device* pDevice, const VERTEX* verticesData,
					const int vertexCount,
					string modelName = "custom"); 
	bool Initialize(ID3D11Device* device, std::string modelName, WCHAR* textureName);		// The function here handle initializing of the model's vertex and index buffers using some model data and texture
	void Shutdown(void);
	void Render(ID3D11DeviceContext* pDeviceContext);	// The Render() function puts the model geometry on the video card to prepare 
										// it for drawing by the color shader

	// --- getters --- //
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();	// also can pass its own texture resource to shaders that will draw this model
	 
	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(size_t i);
	void operator delete(void* p);


protected:
	// functions for work with a vertex and index buffers
	bool InitializeBuffers(ID3D11Device* pDevice);
	//bool InitializeBuffersFor2D(ID3D11Device* pDevice);

	bool LoadModel(std::string modelName);
	void ReleaseModel();
	
	void ShutdownBuffers(void);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

	// functions to handle loading and unloading the texture data from the .dds file
	bool LoadTexture(ID3D11Device* pDevice, WCHAR* textureName);
	void ReleaseTexture();

protected:
	// internal representation of a model structure
	struct ModelType
	{
		float x, y, z;     // position coords
		float tu, tv;      // texture coords
		float nx, ny, nz;  // normals
		float cr, cg, cb;  // colours (cr - red, cg - green, cb - blue)
	};


	ID3D11Buffer* pVertexBuffer_ = nullptr; // a pointer to the vertex buffer
	ID3D11Buffer* pIndexBuffer_ = nullptr;  // a pointer to the index buffer
	TextureClass* pTexture_ = nullptr;      // a pointer to the TextureClass for work with textures

	ModelConverterClass modelConverter;     // for converting models to different formats

	int vertexCount_ = 0;
	int indexCount_ = 0;
	bool isModel3D_ = true;                 // defines if this model whether it is a 3D-model or a 2D-model

	ModelType* pModelType_ = nullptr;
};

