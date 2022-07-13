////////////////////////////////////////////////////////////////////
// Filename:    bitmapclass.h
// Description: will be used to represent an individual 2D image
//              that needs to be rendered to the screen. For every 
//              2D image you have you will need a new BitmapClass obj.
//
// Revising:    27.05.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <DirectXMath.h>

#include "textureclass.h"



//////////////////////////////////
// Class name: BitmapClass
/////////////////////////////////
class BitmapClass
{
public:
	BitmapClass(void);
	BitmapClass(const BitmapClass& obj);
	~BitmapClass(void);

	bool Initialize(ID3D11Device* device,  int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext* deviceContext, 
		        int positionX, int positionY,
		        float textureTopX, float textureTopY,
		        float textureBottomX, float textureBottomY, int textureWrapTimes = 1);

	int GetIndexCount();
	int GetBitmapWidth(void);
	int GetBitmapHeight(void);
	ID3D11ShaderResourceView* GetTexture();


	// memory allocation
	void* operator new(size_t i)
	{
		void* ptr = _aligned_malloc(i, 16);
		if (!ptr)
		{
			Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
			return nullptr;
		}

		return ptr;
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers(void);
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext,   // because we use a dynamic buffers
		               int positionX, int positionY,
		               float textureTopX, float textureTopY,
		               float textureBottomX, float textureBottomY, int textureWrapTimes);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture(void);

private:
	// each bitmap image is still a polygon object that get rendered similar to 3D objects.
	// For 2D image we just need a position vector and texture coordinates
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

private:
	ID3D11Buffer* m_pVertexBuffer, *m_pIndexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_pTexture;

	// specific data only for 2D models rendering
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};
