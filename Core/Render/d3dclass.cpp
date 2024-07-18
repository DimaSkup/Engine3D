/////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
// Revising: 01.01.23
/////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

#include <sstream>
#include <bitset>
#include <iostream>

D3DClass* D3DClass::pInstance_ = nullptr;


D3DClass::D3DClass()
{
	if (D3DClass::pInstance_ == nullptr)
	{
		D3DClass::pInstance_ = this;
	}
	else
	{
		ASSERT_TRUE(false, "you can't create more than only one instance of this class");
	}

	Log::Debug(LOG_MACRO);

	rasterParamsNames_[static_cast<int>(FILL_MODE_SOLID)]     = "FILL_MODE_SOLID";
	rasterParamsNames_[static_cast<int>(FILL_MODE_WIREFRAME)] = "FILL_MODE_WIREFRAME";
	rasterParamsNames_[static_cast<int>(CULL_MODE_BACK)]      = "CULL_MODE_BACK";
	rasterParamsNames_[static_cast<int>(CULL_MODE_FRONT)]     = "CULL_MODE_FRONT";
	rasterParamsNames_[static_cast<int>(CULL_MODE_NONE)]      = "CULL_MODE_NONE";
}

D3DClass::~D3DClass()
{

}




// ----------------------------------------------------------------------------------- //
//                                                                                     //
//                                PUBLIC METHODS                                       //
//                                                                                     //
// ----------------------------------------------------------------------------------- //


// this function initializes Direct3D stuff
bool D3DClass::Initialize(HWND hwnd, 
	const int windowWidth, 
	const int windowHeight,
	const bool vsyncEnabled,
	const bool fullScreen,
	const bool enable4xMSAA,
	const float screenNear, 
	const float screenDepth)
{
	try
	{
		Log::Debug(LOG_MACRO);

		assert(windowWidth > 0);
		assert(windowHeight > 0);
		assert(screenNear >= 0.0f);
		assert(screenDepth > screenNear);

		//width_ = screenWidth;
		//height_ = screenHeight;
		vsyncEnabled_ = vsyncEnabled;        // define if VSYNC is enabled or not
		fullScreen_ = fullScreen;            // define if window is full screen or not
		enable4xMsaa_ = enable4xMSAA;        // use 4X MSAA?

		// initialize all the main parts of DirectX
		InitializeDirectX(hwnd, windowWidth, windowHeight, screenNear, screenDepth);

		Log::Print(LOG_MACRO, "is initialized successfully");
	}
	catch (EngineException& exception)
	{
		Log::Error(exception, true);
		return false;
	}

	return true;
}


// reset the screen state and release the allocated memory
void D3DClass::Shutdown()
{
	// set a windowed mode as active
	if (pSwapChain_)
		pSwapChain_->SetFullscreenState(FALSE, nullptr);

	// release all the blend states
	_RELEASE(prevBlendState_);

	_RELEASE(pTransparentBS_);
	_RELEASE(pMultiplyingBS_);
	_RELEASE(pSubtractingBS_);
	_RELEASE(pAddingBS_);

	_RELEASE(pNoRenderTargetWritesBS_);
	_RELEASE(pAlphaBSForSkyPlane_);
	_RELEASE(pAlphaDisableBS_);
	_RELEASE(pAlphaEnableBS_);
	
	// release all the rasterizer states
	if (!rasterizerStatesMap_.empty())
	{
		for (auto & elem : rasterizerStatesMap_)
		{
			_RELEASE(elem.second);     // release rasterizer states objects
		}
		rasterizerStatesMap_.clear();
	}

	// release all the depth / stencil states
	_RELEASE(pNoDoubleBlendDSS_);
	_RELEASE(pDrawReflectionDSS_);
	_RELEASE(pMarkMirrorDSS_);
	_RELEASE(pDepthDisabledStencilState_);
	_RELEASE(pDepthStencilView_);
	_RELEASE(pDepthStencilState_);
	_RELEASE(pDepthStencilBuffer_);


	_RELEASE(pRenderTargetView_);
	_RELEASE(pImmediateContext_);
	_RELEASE(pDevice_);
	_RELEASE(pSwapChain_);

	_DELETE_ARR(pBlendFactor_);

	return;
}


// memory allocation
void* D3DClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}
	
	Log::Error(LOG_MACRO, "can't allocate memory for the D3DClass object");
	throw std::bad_alloc{};
}

void D3DClass::operator delete(void* p)
{
	_aligned_free(p);
}

///////////////////////////////////////////////////////////



void D3DClass::BeginScene()
{
	// before rendering of each frame we need to set buffers

	//const FLOAT whiteColor[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
	const FLOAT bgColor[4] { 0.2f, 0.4f, 0.6f, 1.0f };  // light blue background colour
	//const FLOAT lightPurple[4]{ 230.0f / 255.0f, 185.0f / 255.0f, 170.0f / 255.0f };
	//const FLOAT darkPurple[4]{ 46.0f/255.0f, 36.0f/255.0f, 34.0f/255.0f};
	//const FLOAT greyBgColor[4]{ 0.5f, 0.5f, 0.5f, 1.0f };      // grey background colour
	//const FLOAT blackBgColor[4]{ 0, 0, 0.1f, 1 };
	
	// clear the render target view with particular color
	pImmediateContext_->ClearRenderTargetView(pRenderTargetView_, bgColor);

	// clear the depth stencil view with 1.0f values
	pImmediateContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}

// after all the rendering into the back buffer we need to present it on the screen
void D3DClass::EndScene(void)
{
	pSwapChain_->Present(0, 0); // present the back buffer as fast as possible

#if 0
	if (vsyncEnabled_)              // if vertical synchronization is enabled
	{
		pSwapChain_->Present(1, 0); // lock the refresh rate to necessary value
	}
	else
	{
		pSwapChain_->Present(0, 0); // present the back buffer as fast as possible
	}
#endif

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::GetDeviceAndDeviceContext(ID3D11Device*& pDevice,
	ID3D11DeviceContext*& pDeviceContext)
{
	pDevice = pDevice_;
	pDeviceContext = pImmediateContext_;
}

///////////////////////////////////////////////////////////

// these two helper functions returns a reference to world/ortho matrix respectively
const DirectX::XMMATRIX & D3DClass::GetWorldMatrix() const 
{
	return worldMatrix_;
}

const DirectX::XMMATRIX & D3DClass::GetOrthoMatrix() const
{
	return orthoMatrix_;
}

// These two helper functions initialize its parameters with references to
// the world matrix, and the orthographic matrix respectively
void D3DClass::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = worldMatrix_;
	return;
}

void D3DClass::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
{
	orthoMatrix = orthoMatrix_;
	return;
}

///////////////////////////////////////////////////////////


void D3DClass::GetVideoCardInfo(std::string & cardName, int & memory)
{
	// this function returns us the information of the video card;
	// it stores the data into the input params

	Log::Debug(LOG_MACRO);

	Log::Debug(LOG_MACRO, "Video card name: " + (std::string)videoCardDescription_);
	Log::Debug(LOG_MACRO, "Video memory : " + std::to_string(videoCardMemory_) + " MB");

	cardName = videoCardDescription_;
	memory = videoCardMemory_;

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::SetRenderState(D3DClass::RASTER_PARAMS rsParam)
{
	this->UpdateRasterStateParams(rsParam);

	// get a rasterizer state accroding to the updated params
	ID3D11RasterizerState* pRSState = GetRasterStateByHash(GetRSHash());

	// set a rasterizer state
	pImmediateContext_->RSSetState(pRSState);

}

// functions for enabling and disabling the Z buffer
void D3DClass::TurnZBufferOn(void)
{
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState_, 1);
	return;
}

void D3DClass::TurnZBufferOff(void)
{
	pImmediateContext_->OMSetDepthStencilState(pDepthDisabledStencilState_, 1);
	return;
}

// TurnOnAlphaBlending() allows us to turn on alpha blending by using OMSetBlendState()
// with our m_pAlphaEnableBlendingState blending state
void D3DClass::TurnOnAlphaBlending(void)
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn on the alpha blending
	pImmediateContext_->OMSetBlendState(pAlphaEnableBS_, NULL, 0xFFFFFFFF);

	return;
}

// TurnOnAlphaBlending() allows us to turn off alpha blending by using OMSetBlendState()
// with our m_pAlphaDisableBlendingState blending state
void D3DClass::TurnOffAlphaBlending(void)
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn off the alpha blending
	pImmediateContext_->OMSetBlendState(pAlphaDisableBS_, NULL, 0xFFFFFFFF);

	return;
}

void D3DClass::TurnOnAddingBS()
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn on the adding blending
	pImmediateContext_->OMSetBlendState(pAddingBS_, NULL, 0xFFFFFFFF);

	return;
}

void D3DClass::TurnOffAddingBS()
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn off the adding blending
	pImmediateContext_->OMSetBlendState(pAlphaDisableBS_, NULL, 0xFFFFFFFF);

	return;
}


void D3DClass::TurnOnSubtractingBS()
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn on the subtracting blending
	pImmediateContext_->OMSetBlendState(pSubtractingBS_, NULL, 0xFFFFFFFF);

	return;
}

void D3DClass::TurnOffSubtractingBS()
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn off the subtracting blending
	pImmediateContext_->OMSetBlendState(pAlphaDisableBS_, NULL, 0xFFFFFFFF);

	return;
}

void D3DClass::TurnOnMultiplyingBS()
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn on the subtracting blending
	pImmediateContext_->OMSetBlendState(pMultiplyingBS_, NULL, 0xFFFFFFFF);

	return;
}

void D3DClass::TurnOffMultiplyingBS()
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn off the subtracting blending
	pImmediateContext_->OMSetBlendState(pAlphaDisableBS_, NULL, 0xFFFFFFFF);

	return;
}

void D3DClass::TurnOnTransparentBS()
{
	float blendFactor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

	// turn on the subtracting blending
	pImmediateContext_->OMSetBlendState(pTransparentBS_, blendFactor, 0xFFFFFFFF);

	return;
}

void D3DClass::TurnOffTransparentBS()
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn off the subtracting blending
	pImmediateContext_->OMSetBlendState(pAlphaDisableBS_, NULL, 0xFFFFFFFF);

	return;
}


///////////////////////////////////////////////////////////

void D3DClass::TurnOnMarkMirrorOnStencil()
{
	// turn on marking the pixels of the mirror on the stencil buffer.
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	

	// store the previous blend state and depth stencil state
	pImmediateContext_->OMGetBlendState(&prevBlendState_, pBlendFactor_, &sampleMask_);
	pImmediateContext_->OMGetDepthStencilState(&prevDepthStencilState_, &stencilRef_);

	// setup blend state and depth stencil state 
	pImmediateContext_->OMSetBlendState(pNoRenderTargetWritesBS_, blendFactor, -1);
	pImmediateContext_->OMSetDepthStencilState(pMarkMirrorDSS_, 1);

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::TurnOffMarkMirrorOnStencil()
{
	// turn off marking the pixels of the mirror on the stencil buffer.

	// restore the previous blend state and depth stencil state
	pImmediateContext_->OMSetBlendState(prevBlendState_, pBlendFactor_, sampleMask_);
	pImmediateContext_->OMSetDepthStencilState(prevDepthStencilState_, stencilRef_);

	return;
}

///////////////////////////////////////////////////////////


void D3DClass::TurnOnAlphaBlendingForSkyPlane()
{
	// a function for enabling the additive blending that the sky plane clouds will require

	// setup the blend factor 
	float blendFactor[4] = { 0.0f };

	// turn on the alpha blending 
	pImmediateContext_->OMSetBlendState(pAlphaBSForSkyPlane_, blendFactor, 0xFFFFFFFF);

	return;
}

///////////////////////////////////////////////////////////

// these two helpers are used in the render_to_texture functional
void D3DClass::SetBackBufferRenderTarget()
{
	// bind the render target view and depth stencil buffer to the output render pipeline
	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);

	return;
}

void D3DClass::ResetViewport()
{
	// set the viewport
	pImmediateContext_->RSSetViewports(1, &viewport_);
	
	return;
}






////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
//                                 PRIVATE METHODS                                        //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

void D3DClass::InitializeDirectX(HWND hwnd,
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,
	const float farZ)
{
	try
	{
		Log::Debug(LOG_MACRO);

		bool result = false;

		assert(windowWidth > 0);
		assert(windowHeight > 0);

		// enumerate adapters to get inforation about it
		EnumerateAdapters();

		// create the Direct3D 11 device and context
		InitializeDevice();

		// --- initialize all the main parts of DirectX --- //
		InitializeSwapChain(hwnd, windowWidth, windowHeight);
		InitializeRenderTargetView();

		InitializeDepthStencil(windowWidth, windowHeight);
		InitializeRasterizerState();

		InitializeViewport(windowWidth, windowHeight);
		InitializeMatrices(windowWidth, windowHeight, nearZ, farZ);
		InitializeBlendStates();
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't initialize DirectX stuff");
	}

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::EnumerateAdapters()
{
	// get data about the video card, user's screen, etc.

	// get an array of adapters
	adapters_ = AdapterReader::GetAdapters();

	// check if we have any available IDXGI adapter
	const bool result = adapters_.size() > 1;
	ASSERT_TRUE(result, "can't find any IDXGI adapter");

	// store the dedicated video card memory in megabytes
	const UINT bytesInMegabyte = 1024 * 1024;
	videoCardMemory_ = static_cast<int>(adapters_[displayAdapterIndex_].description.DedicatedVideoMemory / bytesInMegabyte);

	// convert the name of the video card to a character array and store it
	videoCardDescription_ = StringHelper::ToString(this->adapters_[1].description.Description);
} 

///////////////////////////////////////////////////////////

void D3DClass::InitializeDevice()
{
	// THIS FUNCTION creates the Direct3D 11 device and context;
	// also it check the quality level support for 4X MSAA.

	D3D_FEATURE_LEVEL featureLevel;
	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


	HRESULT hr = D3D11CreateDevice(
		adapters_[displayAdapterIndex_].pAdapter,   // use a display adapter by this index
		D3D_DRIVER_TYPE_UNKNOWN,
		0,                                          // no software device
		createDeviceFlags,
		0, 0,                                       // default feature level array
		D3D11_SDK_VERSION,
		&pDevice_,
		&featureLevel,
		&pImmediateContext_);

	ASSERT_NOT_FAILED(hr, "D3D11CreateDevice failed");
	assert((featureLevel == D3D_FEATURE_LEVEL_11_0) && "Direct3D Feature Level 11 unsupported");

	// now that we have a created device, we can check the quality level support for 4X MSAA.
	hr = pDevice_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality_);
	ASSERT_NOT_FAILED(hr, "the quality level number must be > 0");

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeSwapChain(HWND hwnd, const int width, const int height)
{
	// THIS FUNCTION creates the swap chain
	HRESULT hr = S_OK;
	DXGI_SWAP_CHAIN_DESC sd { 0 };

	// Setup the swap chain description
	sd.BufferDesc.Width = width;	                        // desired back buffer width
	sd.BufferDesc.Height = height;                          // desired back buffer height
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	    // use a simple 32-bit surface 
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// a rasterizer method to render an image on a surface
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// how to scale an image to fit it to the screen resolution

	sd.BufferCount = 1;					                    // we have only one back buffer
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	    // use the back buffer as the render target output
	sd.OutputWindow = hwnd;								    // set the current window
	sd.Windowed = !this->fullScreen_;				        // specity true to run in windowed mode or false for full-screen mode
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			    // discard the content of the back buffer after presenting
	sd.Flags = 0;


	// if we use a vertical synchronization
	if (vsyncEnabled_)
	{
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}
	// we don't use a vertical synchronization
	else 
	{
		sd.BufferDesc.RefreshRate.Numerator = 0;		
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Use 4X MSAA?
	if (enable4xMsaa_)
	{
		sd.SampleDesc.Count = 4;

		// m4xMsaaQuality_ is returned via CheckMultisampleQualitylevels()
		sd.SampleDesc.Quality = m4xMsaaQuality_ -1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;				                    
		sd.SampleDesc.Quality = 0;
	}

	// for creation of the swap chain we have to use the IDXGIFactory instance that was
	// used to create the device
	IDXGIDevice* pDxgiDevice = nullptr;
	hr = pDevice_->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDxgiDevice);
	ASSERT_NOT_FAILED(hr, "can't get the interface of DXGI Device");

	IDXGIAdapter* pDxgiAdapter = nullptr;
	hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDxgiAdapter);
	ASSERT_NOT_FAILED(hr, "can't get the interface of DXGI Adapter");

	// finally go the IDXGIFactory interface
	IDXGIFactory* pDxgiFactory = nullptr;
	hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDxgiFactory);
	ASSERT_NOT_FAILED(hr, "can't get the interface of DXGI Factory");
		
	// Create the swap chain
	pDxgiFactory->CreateSwapChain(pDevice_, &sd, &pSwapChain_);
	ASSERT_NOT_FAILED(hr, "can't create the swap chain");
	ASSERT_NOT_NULLPTR(pSwapChain_, "something went wrong during creation of the swap chain because pSwapChain == NULLPTR");

	// release our acquired COM interfaces (because we are done with them)
	_RELEASE(pDxgiDevice);
	_RELEASE(pDxgiAdapter);
	_RELEASE(pDxgiFactory);

	// since we already don't need any adapters data we have the release memory from it
	AdapterReader::Shutdown();

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeRenderTargetView()
{
	// THIS FUNCTION creates and sets up the render target view to the back buffer;

	try
	{
		HRESULT hr = S_OK;
		ID3D11Texture2D* pBackBuffer = nullptr;

		// obtain a pointer to the swap chain's back buffer which we will use as a render target
		hr = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (VOID**)&pBackBuffer);
		ASSERT_NOT_FAILED(hr, "can't get a buffer from the swap chain");

		// create a render target view 
		hr = pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView_);
		_RELEASE(pBackBuffer);
		ASSERT_NOT_FAILED(hr, "can't create a render target view");
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't initialize the render target view");
	}

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeDepthStencil(const UINT clientWidth, const UINT clientHeight)
{
	// creates the depth stencil buffer, depth stencil state, depth stencil view,
	// and disabled stencil state which is necessary for 2D rendering

	try
	{
		// initialize all the necessary parts of depth stencil
		InitializeDepthStencilTextureBuffer(clientWidth, clientHeight);
		InitializeDepthStencilState();           // the depth stencil state with ENABLED depth
		InitializeDepthDisabledStencilState();   // the depth stencil state with DISABLED depth

												 // depth stencil states which are used for rendering reflections
		InitializeMarkMirrorDSS();               // DSS -- depth stencil state
		InitializeDrawReflectionDSS();
		InitializeNoDoubleBlendDSS();

		InitializeDepthStencilView();

		// Set the depth stencil state.
		pImmediateContext_->OMSetDepthStencilState(pDepthStencilState_, 1);

		// bind together the render target view and the depth stencil view to the output merger stage
		pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);

	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't initialize some of the depth/stencil elements");
	}

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeDepthStencilTextureBuffer(const UINT clientWidth, const UINT clientHeight)
{
	// THIS FUNCTION creates the depth/stencil texture

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;

	// describe our Depth/Stencil Buffer
	depthStencilBufferDesc.Width     = clientWidth;
	depthStencilBufferDesc.Height    = clientHeight;
	depthStencilBufferDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24 bits for the depth and 8 bits for the stencil
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;

	// Use 4X MSAA?
	if (enable4xMsaa_)
	{
		depthStencilBufferDesc.SampleDesc.Count = 4;
		depthStencilBufferDesc.SampleDesc.Quality = m4xMsaaQuality_ - 1;
	}
	// No MSAA
	else
	{
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
	}
	
	depthStencilBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags      = 0;

	// Create the depth/stencil buffer
	HRESULT hr = pDevice_->CreateTexture2D(&depthStencilBufferDesc, nullptr, &pDepthStencilBuffer_);
	ASSERT_NOT_FAILED(hr, "can't create the depth stencil buffer");

	return;
} 

///////////////////////////////////////////////////////////

void D3DClass::InitializeDepthStencilView()
{
	// THIS FUNCTION creates a depth/stencil view

#if 0
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// Setup the depth stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
#endif

	// Create a depth stencil view
	const HRESULT hr = pDevice_->CreateDepthStencilView(pDepthStencilBuffer_,
		nullptr, // &depthStencilViewDesc, -- because we specified the type of our depth/stencil buffer, we specify null for this parameter
		&pDepthStencilView_);
	ASSERT_NOT_FAILED(hr, "can't create a depth stencil view");

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeDepthStencilState()
{
	// THIS FUNCTION initializes the depth ENABLED stencil state

	/* OLD STYLE

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc { 0 };

	// Setup the depth stencil state description
	depthStencilDesc.DepthEnable = true;	// enable depth testing
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;	// a part of the depth buffer to writing
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	    // a function to compare source depth data against exiting (destination) depth data

	depthStencilDesc.StencilEnable = true;	    // enable stencil testing
	depthStencilDesc.StencilWriteMask = 0xFF;	// a part of the stencil buffer to write
	depthStencilDesc.StencilReadMask = 0xFF;	// a part of the staneil buffer to read

	// set operations if pixel is front facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// not change buffer values if stencil testing is failed
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;	// increment the buffer values if depth testing is failed
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	// not change buffer values if stencil testing and depth testing are passed
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// a function to compare source stencil data against exiting (destination) stencil data

	// set operations if pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// not change buffer values if stencil testing is failed
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;	// decrement the buffer values if depth testing is failed
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	// not change buffer values if stencil testing and depth testing are passed
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// a function to compare source stencil data against exiting (destination) stencil data
	*/


	// setup the description of the depth ENABLED stencil state
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	// Create a depth stencil state
	HRESULT hr = pDevice_->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState_);
	ASSERT_NOT_FAILED(hr, "can't create a depth stencil state");

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeDepthDisabledStencilState()
{
	// this func creates a second depth stencil state which turns off the Z buffer for 
	// 2D rendering. The only difference between this one and the depth_enabled_stencil_state
	// is that the DepthEnable parameter is set to false,
	// all other parameters are the same as the another depth stencil state
	// (DEPTH DISABLED STENCIL STATE IS NECESSARY FOR 2D RENDERING)

	// setup the description of the depth DISABLED stencil state
	CD3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc(D3D11_DEFAULT);
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.DepthEnable = false;

	// create the depth stencil state
	HRESULT hr = pDevice_->CreateDepthStencilState(&depthDisabledStencilDesc, &pDepthDisabledStencilState_);
	ASSERT_NOT_FAILED(hr, "can't create the depth disabled stencil state");

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeMarkMirrorDSS()
{
	// DSS -- depth stencil state;
	//
	// this state is used to mark the position of a mirror on the stencil buffer, without
	// changing the depth buffer. We will pair this with a new BlendState 
	// (noRenderTargetWritesBS) which will disable writing any color information to the 
	// backbuffer, so that we will have the combined effect which will be used to write
	// only to the stencil.


	///////////////////////////////////////////////////////
	//  CREATE A MARK_MIRROR_DEPTH_STENCIL_STATE
	///////////////////////////////////////////////////////

	// setup the description of the mark_mirror_depth_stencil_state
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	// create a depth stencil state (DSS)
	HRESULT hr = pDevice_->CreateDepthStencilState(&depthStencilDesc, &pMarkMirrorDSS_);
	ASSERT_NOT_FAILED(hr, "can't create a mark_mirror_depth_stencil_state");

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeDrawReflectionDSS()
{
	// DSS -- depth stencil state;
	//
	// this state will be used to draw the geometry that should appear as a reflection in
	// mirror. We will set the stencil test up so that we will only render pixels if they
	// have been previously marked as part of the mirror by the MarkMirrorDSS.


	///////////////////////////////////////////////////////
	//  CREATE A DRAW_REFLECTION_DEPTH_STENCIL_STATE
	///////////////////////////////////////////////////////

	// setup the description of the draw_relfection_depth_stencil_state
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);

	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc   = D3D11_COMPARISON_EQUAL;
	depthStencilDesc.BackFace.StencilPassOp  = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc    = D3D11_COMPARISON_EQUAL;

	// create a depth stencil state (DSS)
	HRESULT hr = pDevice_->CreateDepthStencilState(&depthStencilDesc, &pDrawReflectionDSS_);
	ASSERT_NOT_FAILED(hr, "can't create a draw_reflection_depth_stencil_state");

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeNoDoubleBlendDSS()
{
	// DSS -- depth stencil state;
	//
	// this state will be used to draw our shadown. Because we are drawing our shadows as
	// partially transparent black using alpha-blending, if we were to simply draw the 
	// shadow geometry, we would have darker patches where multiple surfaces of the shadow
	// object are projected to the shadow plane, a condition known as shadow-acne. Instead,
	// we setup the stencil test to check that the current stencil value is equal to the 
	// reference value, and increment on passes. Thus, the first time a projected pixel is
	// drawn, it will pass the stencil test, increment the stencil value, and be rendered.
	// On subsequent draws, the pixel will fail the stencil test.


	///////////////////////////////////////////////////////
	//  CREATE A NO_DOUBLE_BLEND_DEPTH_STENCIL_STATE
	///////////////////////////////////////////////////////

	// setup the description of the no_double_blend_depth_stencil_state
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);

	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilFunc   = D3D11_COMPARISON_EQUAL;
	depthStencilDesc.BackFace.StencilPassOp  = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.BackFace.StencilFunc    = D3D11_COMPARISON_EQUAL;

	// create a depth stencil state (DSS)
	HRESULT hr = pDevice_->CreateDepthStencilState(&depthStencilDesc, &pNoDoubleBlendDSS_);
	ASSERT_NOT_FAILED(hr, "can't create a no_double_blend_depth_stencil_state");

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeRasterizerState()
{
	// THIS FUNCTION creates/sets up the rasterizer state

	/* OLD STYLE
		
	HRESULT hr = S_OK;
	D3D11_RASTERIZER_DESC rasterDesc;
	
	// Initialize the rasterizer state description
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	// Setup the rasterizer state description
	rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;   // D3D11_CULL_BACK -- not render triangles which are back facing
	rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;  // a mode of filling primitives during rendering
	rasterDesc.AntialiasedLineEnable = false;                 // not use line anti-aliasing algorithm (is used if param MultisampleEnable = false)
	rasterDesc.DepthBias = 0;                                 // a depth bias magnitude which is added to pixel's depth

	rasterDesc.DepthBiasClamp = 0.0f;                         // a maximum magnitude of pixel depth bias
	rasterDesc.DepthClipEnable = true;                        // enable clipping which is based on distance
	rasterDesc.FrontCounterClockwise = false;                 // a triangle is front facing if its vertices are clockwise and back facing if its vertices are counter-clockwise

	rasterDesc.MultisampleEnable = false;                     // use alpha line anti-aliasing algorithm
	rasterDesc.ScissorEnable = false;                         // not use clipping for pixels which are around of the scissor quadrilateral
	rasterDesc.SlopeScaledDepthBias = 0.0f;                   // scalar of pixel depth slope
	*/

	try
	{

		// set up the rasterizer state description
		HRESULT hr = S_OK;
		ID3D11RasterizerState* pRasterState = nullptr;
		CD3D11_RASTERIZER_DESC pRasterDesc(D3D11_DEFAULT);    // all the values of description are default  


		// 1. create a fill solid + cull back rasterizer state
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
		ASSERT_NOT_FAILED(hr, "can't create a raster state: fill solid + cull back");

		this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
		this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
		rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});

		////////////////////////////////////////////////

		// 2. create a fill solid + cull front rasterizer state
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
		ASSERT_NOT_FAILED(hr, "can't create a raster state: fill solid + cull front");

		rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
		this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
		this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_FRONT);
		rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});

		////////////////////////////////////////////////

		// 3. create a fill wireframe + cull back rasterizer state
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
		ASSERT_NOT_FAILED(hr, "can't create a raster state: fill wireframe + cull back");

		rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
		this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_WIREFRAME);
		this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
		rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});

		////////////////////////////////////////////////

		// 4. create a fill wireframe + cull front rasterizer state
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
		ASSERT_NOT_FAILED(hr, "can't create a raster state: fill wireframe + cull front");

		rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
		this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_WIREFRAME);
		this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_FRONT);
		rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});

		////////////////////////////////////////////////

		// 5. create a fill wireframe + cull none rasterizer state
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
		ASSERT_NOT_FAILED(hr, "can't create a raster state: fill solid + cull none");

		rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
		this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_WIREFRAME);
		this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_NONE);
		rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});

		////////////////////////////////////////////////

		// 5. create a fill solid + cull none rasterizer state
		pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
		ASSERT_NOT_FAILED(hr, "can't create a raster state: fill solid + cull none");

		rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
		this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
		this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_NONE);
		rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});

		////////////////////////////////////////////////

		// AFTER ALL: reset the rasterizer state hash after initialization and set the default params
		rasterStateHash_ &= 0;
		this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
		this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't initialize the rasterizer state");
	}
	
	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeViewport(const UINT clientWidth, const UINT clientHeight)
{
	// THIS FUNCTION sets up the viewport for the rendering pipeline

	// Setup the viewport
	viewport_.Width = static_cast<FLOAT>(clientWidth);
	viewport_.Height = static_cast<FLOAT>(clientHeight);
	viewport_.MaxDepth = 1.0f;
	viewport_.MinDepth = 0.0f;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;

	// Set the viewport
	pImmediateContext_->RSSetViewports(1, &viewport_);

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeMatrices(const UINT clientWidth, 
	const UINT clientHeight,
	const float nearZ, 
	const float farZ)
{
	// THIS FUNCTION initializes world and ortho matrices to it's default values

	// Initialize the world matrix 
	this->worldMatrix_ = DirectX::XMMatrixIdentity();

	// Initialize the orthographic matrix for 2D rendering
	this->orthoMatrix_ = DirectX::XMMatrixOrthographicLH(
		static_cast<float>(clientWidth),
		static_cast<float>(clientHeight),
		nearZ,
		farZ);

	return;
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeBlendStates()
{
	// THIS FUNCTION creates and sets up the blend states

	try
	{
		HRESULT hr = S_OK;
		D3D11_BLEND_DESC blendDesc{ 0 };            // description for setting up the two new blend states	   
		D3D11_RENDER_TARGET_BLEND_DESC rtbd{ 0 };


		///////////////////////////////////////////////////////
		//  CREATE A BLEND_STATE WITH DISABLED BLENDING
		///////////////////////////////////////////////////////

		// create an alpha disabled blend state description
		rtbd.BlendEnable = FALSE;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		// create the blend state using the description
		hr = pDevice_->CreateBlendState(&blendDesc, &pAlphaDisableBS_);
		ASSERT_NOT_FAILED(hr, "can't create the alpha disabled blend state");


		///////////////////////////////////////////////////////
		//  CREATE A BLEND_STATE WITH ENABLED BLENDING
		///////////////////////////////////////////////////////

		// modify the description to create an alpha enabled blend state description
		rtbd.BlendEnable = TRUE;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		// create the blend state using the desription
		hr = pDevice_->CreateBlendState(&blendDesc, &pAlphaEnableBS_);
		ASSERT_NOT_FAILED(hr, "can't create the alpha enabled blend state");


		///////////////////////////////////////////////////////
		//  CREATE A BLEND_STATE FOR SKY PLANE RENDERING
		///////////////////////////////////////////////////////

		// setup the description for the additive blending that the sky plane clouds will require
		rtbd.BlendEnable = TRUE;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL; // == 0x0F;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		// create the blend state using the description
		hr = pDevice_->CreateBlendState(&blendDesc, &pAlphaBSForSkyPlane_);
		ASSERT_NOT_FAILED(hr, "can't create the alpha blending state for sky plane");


		///////////////////////////////////////////////////////
		//  CREATE A BLEND_STATE FOR MIRROR RENDERING
		///////////////////////////////////////////////////////

		// create a description for the blend state
		rtbd.BlendEnable = FALSE;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = 0x00;  // none
											//	D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALPHA;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		// create the blend state using the description
		hr = pDevice_->CreateBlendState(&blendDesc, &pNoRenderTargetWritesBS_);
		ASSERT_NOT_FAILED(hr, "can't create a no_render_target_writes_blend_state");



		///////////////////////////////////////////////////////
		//  CREATE A BLEND_STATE FOR ADDING PIXELS RENDERING
		///////////////////////////////////////////////////////

		// create a description for the blend state
		rtbd.BlendEnable = TRUE;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		// create the blend state using the description
		hr = pDevice_->CreateBlendState(&blendDesc, &pAddingBS_);
		ASSERT_NOT_FAILED(hr, "can't create an adding blend state");

		///////////////////////////////////////////////////////
		//  CREATE A BLEND_STATE FOR SUBTRACTING PIXELS RENDERING
		///////////////////////////////////////////////////////

		// create a description for the blend state
		rtbd.BlendEnable = TRUE;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_SUBTRACT;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		// create the blend state using the description
		hr = pDevice_->CreateBlendState(&blendDesc, &pSubtractingBS_);
		ASSERT_NOT_FAILED(hr, "can't create a subtracting blend state");


		///////////////////////////////////////////////////////
		//  CREATE A BLEND_STATE FOR MULTIPYING PIXELS RENDERING
		///////////////////////////////////////////////////////

		// create a description for the blend state
		rtbd.BlendEnable = TRUE;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		// create the blend state using the description
		hr = pDevice_->CreateBlendState(&blendDesc, &pMultiplyingBS_);
		ASSERT_NOT_FAILED(hr, "can't create a multiplying blend state");

		///////////////////////////////////////////////////////
		//  CREATE A BLEND_STATE FOR TRANSPARENT PIXELS RENDERING
		///////////////////////////////////////////////////////

		// create a description for the blend state
		rtbd.BlendEnable = TRUE;
		//	rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		//	rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_BLEND_FACTOR;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_BLEND_FACTOR;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		// create the blend state using the description
		hr = pDevice_->CreateBlendState(&blendDesc, &pTransparentBS_);
		ASSERT_NOT_FAILED(hr, "can't create a transparent blend state");

	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't initialize the blend states");
	}
	
	return;

}



void D3DClass::UpdateRasterStateParams(D3DClass::RASTER_PARAMS rsParam)
{
	// setup the rasterizer state according to the params 

	switch (rsParam)
	{
		// switch between rasterizer culling modes
		case RASTER_PARAMS::CULL_MODE_NONE:
			turnOnRasterParam(RASTER_PARAMS::CULL_MODE_NONE);
			turnOffRasterParam(RASTER_PARAMS::CULL_MODE_FRONT);  // turn off both front and back culling modes
			turnOffRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
			break;
		case RASTER_PARAMS::CULL_MODE_BACK:
			turnOnRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
			turnOffRasterParam(RASTER_PARAMS::CULL_MODE_FRONT);
			turnOffRasterParam(RASTER_PARAMS::CULL_MODE_NONE);
			break;
		case RASTER_PARAMS::CULL_MODE_FRONT:
			turnOnRasterParam(RASTER_PARAMS::CULL_MODE_FRONT);
			turnOffRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
			turnOffRasterParam(RASTER_PARAMS::CULL_MODE_NONE);
			break;

		// switch between rasterizer fill modes
		case RASTER_PARAMS::FILL_MODE_SOLID:
			turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
			turnOffRasterParam(RASTER_PARAMS::FILL_MODE_WIREFRAME);
			break;
		case RASTER_PARAMS::FILL_MODE_WIREFRAME:
			turnOnRasterParam(RASTER_PARAMS::FILL_MODE_WIREFRAME);
			turnOffRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
			break;
		default:
			Log::Error(LOG_MACRO, "an unknown rasterizer state parameter");
	}
}


// returns a hash to the pointer of the current rasterizer state
uint8_t D3DClass::GetRSHash() const
{
	return rasterStateHash_;
}


// returns a pointer to some rasterizer state by hash
ID3D11RasterizerState* D3DClass::GetRasterStateByHash(uint8_t hash) const
{
	// check if we have such rasterizer state
	auto iterator = rasterizerStatesMap_.find(hash);

	// if we found a rasterizer state by the hash
	if (iterator != rasterizerStatesMap_.end())
	{
		return iterator->second;
	}
	// we didn't found any rasterizer state
	else
	{
		std::string errorMsg{ "there is no rasterizer state by this hash: "};
		Log::Error(LOG_MACRO, errorMsg.c_str());  // print error message

		std::stringstream hashStream;
		std::stringstream rasterParamsNamesStream;

		// print the hash
		int symbol = 0;
		for (int i = 7; i >= 0; i--)
		{
			// generate a string with the hash in binary view
			symbol = (hash >> i) & 1;
			hashStream << symbol << " ";

			// if the current symbol == 1 we get its shift in the hash (value of i)
			// and get a name of the rasterizer state parameter from the map
			if (symbol == 1)
				rasterParamsNamesStream << rasterParamsNames_.at(i) << "\n";
		}

		// print has in binary
		printf("%s\n\n", hashStream.str().c_str());

		// print what rasterizer params are wrong
		printf("which is responsible to such D3DClass::RASTER_PARAMS:\n");
		printf("%s\n\n", rasterParamsNamesStream.str().c_str());


		ASSERT_TRUE(false, "wrong hash");  // throw an exception
	}
}


void D3DClass::turnOnRasterParam(RASTER_PARAMS rsParam)
{
	rasterStateHash_ |= (1 << rsParam);
	return;
}

void D3DClass::turnOffRasterParam(RASTER_PARAMS rsParam)
{
	rasterStateHash_ &= ~(1 << rsParam);
}