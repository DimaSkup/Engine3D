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

	void GetWorldMatrix(D3DXMATRIX& worldMatrix);
	void GetProjectionMatrix(D3DXMATRIX& projectionMatrix);
	void GetOrthoMatrix(D3DXMATRIX& orthoMatrix);

	void GetVideoCardInfo(char* cardName, int& memorySize);

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


	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;
};