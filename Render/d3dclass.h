/////////////////////////////////////////////////////////////////////
// Filename:     d3dclass.h
// Description:  here we initialize all the stuff which is reponsible
//               for work with DirectX; enumerate adapters, execute all
//               the primary initialization of devices, etc.
// Revising:     21.03.22
/////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////
// INCLUDES
/////////////////////////////
#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "../Engine/AdapterReader.h"

#include <d3dcommon.h>
#include <d3dx10math.h>
#include <DirectXMath.h>

#include <memory>                     // for using std::unique_ptr
#include <map>


class D3DClass
{
public:
	enum RASTER_PARAMS
	{
		FILL_MODE_SOLID,
		FILL_MODE_WIREFRAME,
		CULL_MODE_BACK,
		CULL_MODE_FRONT,
	};

public:
	D3DClass(void);
	D3DClass(const D3DClass& another) {};
	~D3DClass(void) {};

	bool Initialize(HWND hwnd, 
					const int screenWidth, 
					const int screenHeight, 
					const bool vsync, 
					const bool fullScreen, 
					const float screenNear, 
					const float screenDepth);
	void Shutdown(void);

	// execute some operations before each frame and after each frame
	void BeginScene();
	void EndScene(void);

	// getters
	ID3D11Device* GetDevice(void) const;
	ID3D11DeviceContext* GetDeviceContext(void) const;
	void GetVideoCardInfo(char* cardName, int& memorySize);

	void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix);
	void GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix);

	// rasterizer state manager
	void SetRenderState(D3DClass::RASTER_PARAMS rsParam);

	// turn on/off 2D rendering
	// functions for turning the Z buffer on and off when rendering 2D images
	void TurnZBufferOn();
	void TurnZBufferOff();

	// there are functions for turning on and off alpha blending
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


private:
	static D3DClass* pInstance_;

	bool EnumerateAdapters(); // get data about the video card, user's screen, etc.
	bool InitializeDirectX(HWND hwnd, const float nearZ, const float farZ);
	bool InitializeSwapChain(HWND hwnd, const int width, const int height);
	bool InitializeRenderTargetView();

	// initialize depth stencil parts
	bool InitializeDepthStencil();
	bool InitializeDepthStencilTextureBuffer();
	bool InitializeDepthStencilState();
	bool InitializeDepthDisabledStencilState();
	bool InitializeDepthStencilView();


	bool InitializeRasterizerState();
	bool InitializeViewport();
	bool InitializeMatrices(const float nearZ, const float farZ);
	bool InitializeBlendStates();

	// rasterizer manager helpers
	void UpdateRasterStateParams(D3DClass::RASTER_PARAMS rsParam);
	uint8_t GetRSHash() const;
	ID3D11RasterizerState* GetRasterStateByHash(uint8_t hash) const;
	void turnOnRasterParam(D3DClass::RASTER_PARAMS rsParam);
	void turnOffRasterParam(D3DClass::RASTER_PARAMS rsParam);


private:
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	IDXGISwapChain*			    pSwapChain_ = nullptr;        
	ID3D11Device*			    pDevice_ = nullptr;           // for creation of buffers, etc.
	ID3D11DeviceContext*	    pDeviceContext_ = nullptr;    // set different resource for rendering
	ID3D11RenderTargetView*     pRenderTargetView_ = nullptr; // where we are going to render our buffers

	
	// rasterizer state related stuff
	uint8_t rasterStateHash_{ 0b0000'0000 };  // fill mode wireframe | fill mode solid | cull mode back | cull model front
	std::map<uint8_t, ID3D11RasterizerState*> rasterizerStatesMap_;   // a map of pointers to the rasterizer states pointer with different states

	 
	// depth stuff
	ID3D11Texture2D*			pDepthStencilBuffer_ = nullptr;
	ID3D11DepthStencilState*	pDepthStencilState_ = nullptr;
	ID3D11DepthStencilView*		pDepthStencilView_ = nullptr;
	ID3D11DepthStencilState*    pDepthDisabledStencilState_ = nullptr; // a depth stencil state for 2D drawing
	std::unique_ptr<ID3D11BlendState> pBlendState;
	ID3D11BlendState*           pAlphaEnableBlendingState_ = nullptr;  // blending states
	ID3D11BlendState*           pAlphaDisableBlendingState_ = nullptr;

	bool vsyncEnabled_ = false;
	char videoCardDescription_[128] = { '\0' };
	int  videoCardMemory_ = 0;
	int numerator_ = 0;   // the numerator of a screen refresh rate
	int denominator_ = 0; // the denomirator of a screen refresh rate
	int width_ = 800;  // default screen width
	int height_ = 600; // default screen height
	bool fullScreen_ = false;

	std::vector<AdapterData> adapters_;  // a vector of all the available IDXGI adapters

	
}; // D3DClass