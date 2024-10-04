// ********************************************************************************
// Filename: d3dclass.cpp
// Revising: 01.01.23
// ********************************************************************************
#include "d3dclass.h"

#include "../Common/MemHelpers.h"
#include "../Common/Assert.h"
#include "../Engine/Log.h"
#include "../Engine/EngineException.h"
#include "AdapterReader.h"

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
		throw EngineException("you can't create more than only one instance of this class");
	}

	Log::Debug();
}

D3DClass::~D3DClass()
{
}


// ********************************************************************************
// 
//                           PUBLIC METHODS
// 
// ********************************************************************************
bool D3DClass::Initialize(
	HWND hwnd, 
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
		Log::Debug();

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

		renderStates_.InitAll(pDevice_);

		Log::Print("is initialized successfully");
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void D3DClass::Shutdown()
{
	// reset the screen state and release the allocated memory

	// set a windowed mode as active
	if (pSwapChain_)
		pSwapChain_->SetFullscreenState(FALSE, nullptr);


	// release all the depth stencil stuff
	SafeRelease(&pDepthStencilView_);
	SafeRelease(&pDepthStencilBuffer_);

	SafeRelease(&pRenderTargetView_);
	SafeRelease(&pImmediateContext_);
	SafeRelease(&pDevice_);
	SafeRelease(&pSwapChain_);
}

///////////////////////////////////////////////////////////

void* D3DClass::operator new(size_t i)
{
	// memory allocation

	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}
	
	Log::Error("can't allocate memory for the D3DClass object");
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

	const FLOAT bgColor[4]{ 0.4f, 0.6f, 0.8f, 1.0f };        // higly light blue background colour

	//const FLOAT bgColor[4]{ 1.0f, 1.0f, 1.0f, 1.0f };      // white
	//const FLOAT bgColor[4] { 0.2f, 0.4f, 0.6f, 1.0f };     // light blue background colour
	//const FLOAT bgColor[4]{ 230.0f / 255.0f, 185.0f / 255.0f, 170.0f / 255.0f };
	//const FLOAT bgColor[4]{ 46.0f/255.0f, 36.0f/255.0f, 34.0f/255.0f};
	//const FLOAT bgColor[4]{ 0.5f, 0.5f, 0.5f, 1.0f };      // grey background colour
	
	// clear the render target view with particular color
	pImmediateContext_->ClearRenderTargetView(pRenderTargetView_, bgColor);

	// clear the depth stencil view with 1.0f values
	pImmediateContext_->ClearDepthStencilView(
		pDepthStencilView_, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

///////////////////////////////////////////////////////////

void D3DClass::EndScene()
{
	// after all the rendering into the back buffer 
	// we need to present it on the screen

	// if vertical synchronization is enabled the first param will be set to 1
	// or in another case it will be set to 0 (no vsync)
	pSwapChain_->Present((UINT)vsyncEnabled_, 0);
}

///////////////////////////////////////////////////////////

void D3DClass::GetDeviceAndDeviceContext(ID3D11Device*& pDevice,
	ID3D11DeviceContext*& pDeviceContext)
{
	pDevice = pDevice_;
	pDeviceContext = pImmediateContext_;
}

///////////////////////////////////////////////////////////

void D3DClass::GetVideoCardInfo(std::string & cardName, int & memory)
{
	// this function returns us the information of the video card;
	// it stores the data into the input params

	Log::Debug();

	cardName = videoCardDescription_;
	memory = videoCardMemory_;
}

///////////////////////////////////////////////////////////

// enable/disable the Z-buffer
void D3DClass::TurnZBufferOn()
{
	pImmediateContext_->OMSetDepthStencilState(renderStates_.GetDSS(RenderStates::STATES::DEPTH_ENABLED), 1);
}

void D3DClass::TurnZBufferOff()
{
	pImmediateContext_->OMSetDepthStencilState(renderStates_.GetDSS(RenderStates::STATES::DEPTH_DISABLED), 1);	
}



// *********************************************************************************
// 
//              TURN ON/OFF RENDER STATES (RASTERIZER, BLENDING, etc.)
// 
// *********************************************************************************

void D3DClass::TurnOnBlending(const RenderStates::STATES state)
{
	renderStates_.SetBlendState(pImmediateContext_, state);
}

void D3DClass::TurnOffBlending()
{
	// turn off the alpha blending
	renderStates_.SetBlendState(pImmediateContext_, RenderStates::STATES::ALPHA_DISABLE);
}

///////////////////////////////////////////////////////////

void D3DClass::SetBackBufferRenderTarget()
{
	// bind the render target view and depth stencil buffer to the output render pipeline
	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);
}

///////////////////////////////////////////////////////////

void D3DClass::ResetViewport()
{
	// set the viewport
	pImmediateContext_->RSSetViewports(1, &viewport_);
}

///////////////////////////////////////////////////////////

void D3DClass::TurnOnRSfor2Drendering()
{
	// we call this function to set up a raster state 
	// for proper rendering of 2D elements / UI;
	// NOTE: we store a hash of the previous RS so later we can set it back

	using enum RenderStates::STATES;
	prevRasterStateHash_ = renderStates_.GetCurrentRSHash();
	renderStates_.SetRasterState(pImmediateContext_, { FILL_MODE_SOLID, CULL_MODE_BACK });
}

///////////////////////////////////////////////////////////

void D3DClass::TurnOffRSfor2Drendering()
{
	renderStates_.SetRasterStateByHash(pImmediateContext_, prevRasterStateHash_);
}





// ************************************************************************************
// 
//                             PRIVATE METHODS
// 
// ************************************************************************************

void D3DClass::InitializeDirectX(
	HWND hwnd,
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,
	const float farZ)
{
	try
	{
		Log::Debug();

		Assert::True((windowWidth & windowHeight), "wrong window dimensions");
		Assert::True(nearZ > 0, "near wnd plane must be > 0");
		Assert::True(farZ > nearZ, "far wnd plane must be > near plane");

		// enumerate adapters to get inforation about it
		EnumerateAdapters();

		// create the Direct3D 11 device and context
		InitializeDevice();

		// --- initialize all the main parts of DirectX --- //
		InitializeSwapChain(hwnd, windowWidth, windowHeight);
		InitializeRenderTargetView();

		InitializeDepthStencil(windowWidth, windowHeight);

		InitializeViewport(windowWidth, windowHeight);
		InitializeMatrices(windowWidth, windowHeight, nearZ, farZ);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		throw EngineException("can't initialize DirectX stuff");
	}
}

///////////////////////////////////////////////////////////

void D3DClass::EnumerateAdapters()
{
	// get data about the video card, user's screen, etc.

	// get an array of adapters
	adapters_ = AdapterReader::GetAdapters();

	// check if we have any available IDXGI adapter
	Assert::True(adapters_.size() > 1, "can't find any IDXGI adapter");

	// store the dedicated video card memory in megabytes
	const UINT bytesInMegabyte = 1024 * 1024;
	videoCardMemory_ = static_cast<int>(adapters_[displayAdapterIndex_].description_.DedicatedVideoMemory / bytesInMegabyte);

	// convert the name of the video card to a character array and store it
	videoCardDescription_ = StringHelper::ToString(adapters_[1].description_.Description);

	Log::Debug("Video card name: " + videoCardDescription_);
	Log::Debug("Video memory : " + std::to_string(videoCardMemory_) + " MB");
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
		adapters_[displayAdapterIndex_].pAdapter_,   // use a display adapter by this index
		D3D_DRIVER_TYPE_UNKNOWN,
		0,                                          // no software device
		createDeviceFlags,
		0, 0,                                       // default feature level array
		D3D11_SDK_VERSION,
		&pDevice_,
		&featureLevel,
		&pImmediateContext_);

	Assert::NotFailed(hr, "D3D11CreateDevice failed");
	Assert::True(featureLevel == D3D_FEATURE_LEVEL_11_0, "Direct3D Feature Level 11 unsupported");

	// now that we have a created device, we can check the quality level support for 4X MSAA.
	hr = pDevice_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality_);
	Assert::NotFailed(hr, "the quality level number must be > 0");
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
	sd.Windowed = !fullScreen_;				        // specity true to run in windowed mode or false for full-screen mode
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			    // discard the content of the back buffer after presenting
	sd.Flags = 0;


	sd.BufferDesc.RefreshRate.Numerator = (vsyncEnabled_) ? 60 : 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;

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
	Assert::NotFailed(hr, "can't get the interface of DXGI Device");

	IDXGIAdapter* pDxgiAdapter = nullptr;
	hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDxgiAdapter);
	Assert::NotFailed(hr, "can't get the interface of DXGI Adapter");

	// finally go the IDXGIFactory interface
	IDXGIFactory* pDxgiFactory = nullptr;
	hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDxgiFactory);
	Assert::NotFailed(hr, "can't get the interface of DXGI Factory");
		
	// Create the swap chain
	pDxgiFactory->CreateSwapChain(pDevice_, &sd, &pSwapChain_);
	Assert::NotFailed(hr, "can't create the swap chain");
	Assert::NotNullptr(pSwapChain_, "something went wrong during creation of the swap chain because pSwapChain == NULLPTR");

	// release our acquired COM interfaces (because we are done with them)
	SafeRelease(&pDxgiDevice);
	SafeRelease(&pDxgiAdapter);
	SafeRelease(&pDxgiFactory);

	// since we already don't need any adapters data we have the release memory from it
	AdapterReader::Shutdown();
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
		Assert::NotFailed(hr, "can't get a buffer from the swap chain");

		// create a render target view 
		if (pBackBuffer)
		{
			hr = pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView_);
			SafeRelease(&pBackBuffer);
			Assert::NotFailed(hr, "can't create a render target view");
		}
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		throw EngineException("can't initialize the render target view");
	}
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeDepthStencil(
	const UINT clientWidth, 
	const UINT clientHeight)
{
	// creates the depth stencil buffer, depth stencil view,

	try
	{
		InitializeDepthStencilTextureBuffer(clientWidth, clientHeight);
		InitializeDepthStencilView();

		// Set the depth stencil state.
		pImmediateContext_->OMSetDepthStencilState(renderStates_.GetDSS(RenderStates::STATES::DEPTH_ENABLED), 1);

		// bind together the render target view and the depth stencil view to the output merger stage
		pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		throw EngineException("can't initialize some of the depth/stencil elements");
	}
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeDepthStencilTextureBuffer(
	const UINT clientWidth,
	const UINT clientHeight)
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
	Assert::NotFailed(hr, "can't create the depth stencil buffer");
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
	const HRESULT hr = pDevice_->CreateDepthStencilView(
		pDepthStencilBuffer_,
		nullptr,                  // &depthStencilViewDesc, -- because we specified the type of our depth/stencil buffer, we specify null for this parameter
		&pDepthStencilView_);

	Assert::NotFailed(hr, "can't create a depth stencil view");
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
}

///////////////////////////////////////////////////////////

void D3DClass::InitializeMatrices(
	const UINT clientWidth, 
	const UINT clientHeight,
	const float nearZ, 
	const float farZ)
{
	// THIS FUNCTION initializes world and ortho matrices to it's default values
 
	worldMatrix_ = DirectX::XMMatrixIdentity();

	// Initialize the orthographic matrix for 2D rendering
	orthoMatrix_ = DirectX::XMMatrixOrthographicLH(
		static_cast<float>(clientWidth),
		static_cast<float>(clientHeight),
		nearZ,
		farZ);
}