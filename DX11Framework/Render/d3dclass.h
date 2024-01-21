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
#include <string>

class D3DClass
{
public:
	enum RASTER_PARAMS
	{
		FILL_MODE_SOLID = 1,
		FILL_MODE_WIREFRAME = 2,
		CULL_MODE_BACK = 3,
		CULL_MODE_FRONT = 4,
		CULL_MODE_NONE = 5,
	} RasterParams;

	std::map<int, std::string> rasterParamsNames_;
	

public:
	D3DClass();
	~D3DClass();

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

	const DirectX::XMMATRIX & GetWorldMatrix() const;
	const DirectX::XMMATRIX & GetOrthoMatrix() const;

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

	void TurnOnAddingBS();
	void TurnOffAddingBS();

	void TurnOnSubtractingBS();
	void TurnOffSubtractingBS();

	void TurnOnMultiplyingBS();
	void TurnOffMultiplyingBS();

	void TurnOnTransparentBS();
	void TurnOffTransparentBS();



	// turn on/off marking the pixels of the mirror on the stencil buffer.
	void TurnOnMarkMirrorOnStencil();
	void TurnOffMarkMirrorOnStencil();

	// a function for enabling the additive blending that the sky plane clouds will require
	void TurnOnAlphaBlendingForSkyPlane();

	void SetBackBufferRenderTarget();
	void ResetViewport();

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


private:  // restrict a copying of this class instance
	D3DClass(const D3DClass & obj);
	D3DClass & operator=(const D3DClass & obj);

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
	void InitializeMarkMirrorDSS();    // DSS -- depth stencil state
	void InitializeDrawReflectionDSS();
	void InitializeNoDoubleBlendDSS();
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
	D3D11_VIEWPORT              viewport_;

	
	// rasterizer state related stuff
	uint8_t rasterStateHash_{ 0b0000'0000 };  // fill mode wireframe | fill mode solid | cull mode back | cull mode front
	std::map<uint8_t, ID3D11RasterizerState*> rasterizerStatesMap_;   // a map of pointers to the rasterizer states pointer with different states

	 
	// depth stuff
	ID3D11Texture2D*			pDepthStencilBuffer_ = nullptr;
	ID3D11DepthStencilState*	pDepthStencilState_ = nullptr;
	ID3D11DepthStencilView*		pDepthStencilView_ = nullptr;
	ID3D11DepthStencilState*    pDepthDisabledStencilState_ = nullptr; // a depth stencil state for 2D drawing
	ID3D11DepthStencilState*    pMarkMirrorDSS_ = nullptr;
	ID3D11DepthStencilState*    pDrawReflectionDSS_ = nullptr;
	ID3D11DepthStencilState*    pNoDoubleBlendDSS_ = nullptr;

	//std::unique_ptr<ID3D11BlendState> pBlendState;
	ID3D11BlendState*           pAlphaEnableBS_ = nullptr;  // blending states
	ID3D11BlendState*           pAlphaDisableBS_ = nullptr;
	ID3D11BlendState*           pAlphaBSForSkyPlane_ = nullptr;
	ID3D11BlendState*           pNoRenderTargetWritesBS_ = nullptr;

	ID3D11BlendState*           pAddingBS_ = nullptr;
	ID3D11BlendState*           pSubtractingBS_ = nullptr;
	ID3D11BlendState*           pMultiplyingBS_ = nullptr;
	ID3D11BlendState*           pTransparentBS_ = nullptr;

	bool vsyncEnabled_ = false;
	char videoCardDescription_[128] = { '\0' };
	int  videoCardMemory_ = 0;
	int numerator_ = 0;   // the numerator of a screen refresh rate
	int denominator_ = 0; // the denomirator of a screen refresh rate
	int width_ = 800;  // default screen width
	int height_ = 600; // default screen height
	bool fullScreen_ = false;

	std::vector<AdapterData> adapters_;  // a vector of all the available IDXGI adapters

	//////////////////////////////////////////////////////////////////
	// THESE VARIABLES ARE USED FOR TURNING BETWEEN DIFFERENT
	// BLEND STATES AND DEPTH STENCIL STATES
	//////////////////////////////////////////////////////////////////
	ID3D11BlendState* prevBlendState_ = nullptr;   // previous blend state
	FLOAT* pBlendFactor_ = new FLOAT[4]{ 0.0f };   // previous blend factor
	UINT sampleMask_ = 0;                          // previous sample mask

	ID3D11DepthStencilState* prevDepthStencilState_ = nullptr;  // previous depth stencil state
	UINT stencilRef_ = 0;                                       // previous stencil reference

}; // D3DClass