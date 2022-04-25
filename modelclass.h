/////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
// The ModelClass is responsible for encapsulating 
// the geometry for 3DModels.
//
// Last revising: 29.03.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <fstream>

#include "includes.h"
#include "log.h"
#include "textureclass.h"
#include "modelconverterclass.h"


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

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();	// also can pass its own texture resource to shaders that will draw this model
	 
private:
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
	struct VERTEX
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_vertexCount;
	int m_indexCount;
	TextureClass* m_texture;
	ModelType* m_model;
};

