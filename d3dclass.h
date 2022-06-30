/////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
// Here we define D3DClass
// last revising at 21.03.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"
#include "log.h"

#include <DirectXMath.h>

class D3DClass
{
public:
	D3DClass(void);
	D3DClass(const D3DClass& another);
	~D3DClass(void);

	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd,
		bool fullScreen, float screenNear, float screenDepth);
	void Shutdown(void);

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene(void);

	ID3D11Device* GetDevice(void);
	ID3D11DeviceContext* GetDeviceContext(void);

	void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix);
	void GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix);
	void GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix);

	void GetVideoCardInfo(char* cardName, int& memorySize);

	// functions for turning the Z buffer on and off when rendering 2D images
	void TurnZBufferOn();
	void TurnZBufferOff();

	// there are functions for turning on and off alpha blending
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();


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
	bool m_vsync_enabled;
	char m_videoCardDescription[128];
	int  m_videoCardMemory;


	IDXGISwapChain*			m_pSwapChain;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	ID3D11RenderTargetView* m_pRenderTargetView;

	ID3D11Texture2D*			m_pDepthStencilBuffer;
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11DepthStencilView*		m_pDepthStencilView;
	ID3D11RasterizerState*		m_pRasterState;


	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_orthoMatrix;

	ID3D11DepthStencilState* m_pDepthDisabledStencilState; // a depth stencil state for 2D drawing

	// blending states
	ID3D11BlendState* m_pAlphaEnableBlendingState;
	ID3D11BlendState* m_pAlphaDisableBlendingState;
};