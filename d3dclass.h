/////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
/////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////
// LINKING
/////////////////////////////
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

/////////////////////////////
// INCLUDES
/////////////////////////////
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10math.h>

#include "includes.h"
#include "log.h"

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown(void);

	void BeginScene(float, float, float, float);
	void EndScene(void);

	ID3D11Device* GetDevice(void);
	ID3D11DeviceContext* GetDeviceContext(void);

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);
private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain* m_pSwapChain;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	ID3D11RenderTargetView* m_pRenderTargetView;

	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11RasterizerState* m_pRasterState;

	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;

};

































/*
///////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
///////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////
// LINKING
/////////////////////////////
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

/////////////////////////////
// INCLUDES
/////////////////////////////
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10math.h>

#include "includes.h"
#include "log.h"

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown(void);

	void BeginScene(float, float, float, float);
	void EndScene(void);


	ID3D11Device* GetDevice(void);
	ID3D11DeviceContext* GetDeviceContext(void);


	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;

	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView*m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;
};
*/