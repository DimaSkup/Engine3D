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
#include "includes.h"
#include "log.h"

#include "textureclass.h"


//////////////////////////////////
// Class name: ModelClass
//////////////////////////////////
class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	
	bool Initialize(ID3D11Device*, WCHAR*);		// The function here handle initializing of the model's vertex and index buffers
	void Shutdown(void);
	void Render(ID3D11DeviceContext*);	// The Render() function puts the model geometry on the video card to prepare 
										// it for drawing by the color shader

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();	// also can pass its own texture resource to shaders that will draw this model
	 
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers(void);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);	// these functions are for loading and releasing the texture that will be used to render this model
	void ReleaseTexture();

private:
	struct VERTEX
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_vertexCount;
	int m_indexCount;

	TextureClass* m_texture;
};

