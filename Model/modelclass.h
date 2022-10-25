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


//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureName);		// The function here handle initializing of the model's vertex and index buffers
	void Shutdown(void);
	void Render(ID3D11DeviceContext*);	// The Render() function puts the model geometry on the video card to prepare 
										// it for drawing by the color shader

	// --- getters --- //
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();	// also can pass its own texture resource to shaders that will draw this model
	 
	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(size_t i);
	void operator delete(void* p);

private:
	// functions for work with a vertex and index buffers
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers(void);
	void RenderBuffers(ID3D11DeviceContext*);

	// functions to handle loading and unloading the texture data from the .dds file
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	// functions to handle loading and unloading the model data from the text file
	bool LoadModel(char* modelFilename);
	void ReleaseModel();

private:
	struct VERTEX   // representation of the model vertex's data
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

private:
	ID3D11Buffer* m_pVertexBuffer = nullptr; // a pointer to the vertex buffer
	ID3D11Buffer* m_pIndexBuffer = nullptr;  // a pointer to the index buffer
	TextureClass* m_texture = nullptr;       // a pointer to the TextureClass for work with textures
	ModelType* m_model = nullptr; 

	int m_vertexCount = 0;
	int m_indexCount = 0;
};

