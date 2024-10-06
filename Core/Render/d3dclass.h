// ********************************************************************************
// Filename:     d3dclass.h
// Description:  here we initialize all the stuff which is reponsible
//               for work with DirectX; enumerate adapters, execute all
//               the primary initialization of devices, etc.
// Revising:     21.03.22
// ********************************************************************************
#pragma once

/////////////////////////////
// INCLUDES
/////////////////////////////


#include <d3dcommon.h>
#include <DirectXMath.h>

#include <memory>                     // for using std::unique_ptr
#include <map>
#include <string>

#include "AdapterReader.h"
#include "RenderStates.h"

class D3DClass
{
public:
	D3DClass();
	~D3DClass();

	// restrict a copying of this class instance
	D3DClass(const D3DClass& obj) = delete;
	D3DClass& operator=(const D3DClass& obj) = delete;
		

	bool Initialize(HWND hwnd, 
		const int screenWidth, 
		const int screenHeight, 
		const bool vsync, 
		const bool fullScreen, 
		const bool enable4xMSAA,
		const float screenNear, 
		const float screenDepth);

	void Shutdown(void);

	// execute some operations before each frame and after each frame
	void BeginScene();
	void EndScene(void);
	
	// getters
	inline static D3DClass* Get() { return pInstance_; }
	inline ID3D11Device* GetDevice() const { return pDevice_; }
	inline ID3D11DeviceContext* GetDeviceContext() const { return pImmediateContext_; }

	void GetDeviceAndDeviceContext(
		ID3D11Device*& pDevice,
		ID3D11DeviceContext*& pDeviceContext);

	void GetVideoCardInfo(std::string & cardName, int & memorySize);

	inline const DirectX::XMMATRIX& GetWorldMatrix() const { return worldMatrix_; }
	inline const DirectX::XMMATRIX& GetOrthoMatrix() const { return orthoMatrix_; }

	inline void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix) { worldMatrix = worldMatrix_; }
	inline void GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix) { orthoMatrix = orthoMatrix_; }

	inline RenderStates& GetRenderStates() { return renderStates_; }

	// set rasterizer states (RS)
	inline void SetRS(const RenderStates::STATES state) { renderStates_.SetRS(pImmediateContext_, state); }
	inline void SetRS(const std::vector<RenderStates::STATES>& states) { renderStates_.SetRS(pImmediateContext_, states); }

	// turn on/off 2D rendering
	// functions for turning the Z buffer on and off when rendering 2D images
	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnBlending(const RenderStates::STATES state);
	void TurnOffBlending();

	void TurnOnRSfor2Drendering();
	void TurnOffRSfor2Drendering();

	// turn on/off marking the pixels of the mirror on the stencil buffer.
	void TurnOnMarkMirrorOnStencil();
	void TurnOffMarkMirrorOnStencil();

	void SetBackBufferRenderTarget();
	void ResetViewport();

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);

private:

	void InitializeDirectX(HWND hwnd, 
		const UINT windowWidth,
		const UINT windowHeight, 
		const float nearZ, 
		const float farZ);

	void EnumerateAdapters(); // get data about the video card, user's screen, etc.
	void InitializeDevice();
	void InitializeSwapChain(HWND hwnd, const int width, const int height);
	void InitializeRenderTargetView();

	// initialize depth stencil parts
	void InitializeDepthStencil(const UINT clientWidth, const UINT clientHeight);
	void InitializeDepthStencilTextureBuffer(const UINT clientWidth, const UINT clientHeight);
	void InitializeDepthStencilView();

	void InitializeRasterizerState();
	void InitializeViewport(const UINT clientWidth, const UINT clientHeight);
	void InitializeMatrices(const UINT clientWidth, const UINT clientHeight, const float nearZ, const float farZ);

	
private:
	static D3DClass* pInstance_;

	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	IDXGISwapChain*			    pSwapChain_ = nullptr;        
	ID3D11Device*			    pDevice_ = nullptr;           // for creation of buffers, etc.
	ID3D11DeviceContext*	    pImmediateContext_ = nullptr;    // set different resource for rendering
	ID3D11RenderTargetView*     pRenderTargetView_ = nullptr; // where we are going to render our buffers
	D3D11_VIEWPORT              viewport_;

	RenderStates                renderStates_;

	uint8_t                     prevRasterStateHash_ = 0;
	
	// depth stencil stuff
	ID3D11Texture2D*			pDepthStencilBuffer_ = nullptr;
	ID3D11DepthStencilView*		pDepthStencilView_ = nullptr;

	std::string videoCardDescription_{ "" };
	int  videoCardMemory_ = 0;
	//int width_ = 800;                    // default screen width
	//int height_ = 600;                   // default screen height
	bool vsyncEnabled_ = false;
	bool fullScreen_ = false;
	bool enable4xMsaa_ = false;          // use 4X MSAA?
	UINT m4xMsaaQuality_ = 0;            // 4X MSAA quality level
	UINT displayAdapterIndex_ = 1;       // 0 - primary adapter, 1 - discrete adapter

	std::vector<AdapterData> adapters_;  // a vector of all the available IDXGI adapters

	//////////////////////////////////////////////////////////////////
	// THESE VARIABLES ARE USED FOR TURNING BETWEEN DIFFERENT
	// BLEND STATES AND DEPTH STENCIL STATES
	//////////////////////////////////////////////////////////////////

	ID3D11DepthStencilState* prevDepthStencilState_ = nullptr;  // previous depth stencil state
	UINT stencilRef_ = 0;                                       // previous stencil reference

};
