/////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
// Revising: 01.01.23
/////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

// Empty constructor
D3DClass::D3DClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
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
	const int screenWidth, 
	const int screenHeight,
	const bool vsyncEnabled,
	const bool fullScreen,
	const float screenNear, 
	const float screenDepth)
{
	try
	{
		Log::Debug(THIS_FUNC_EMPTY);

		HRESULT hr = S_OK;
		bool result = false;

		this->vsyncEnabled_ = vsyncEnabled;  // define if VSYNC is enabled or not
		this->width_ = screenWidth;
		this->height_ = screenHeight;
		this->fullScreen_ = fullScreen;      // define if window is full screen or not

		// initialize all the main parts of DirectX
		result = InitializeDirectX(hwnd, screenNear, screenDepth);
		COM_ERROR_IF_FALSE(result, "can't initialize DirectX stuff");

		Log::Print(THIS_FUNC, "is initialized successfully");
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
} // Initialize()


// reset the screen state and release the allocated memory
void D3DClass::Shutdown(void)
{
	// set a windowed mode as active
	if (pSwapChain_)
		pSwapChain_->SetFullscreenState(FALSE, nullptr);

	_RELEASE(pAlphaEnableBlendingState_);
	_RELEASE(pAlphaDisableBlendingState_);
	_RELEASE(pAlphaBlendingStateForSkyPlane_);

	// release all the rasterizer states
	if (!rasterizerStatesMap_.empty())
	{
		for (auto & elem : rasterizerStatesMap_)
		{
			_RELEASE(elem.second);     // release rasterizer states objects
		}
		rasterizerStatesMap_.clear();
	}

	// depth / depth stencil
	_RELEASE(pDepthDisabledStencilState_);
	_RELEASE(pDepthStencilView_);
	_RELEASE(pDepthStencilState_);
	_RELEASE(pDepthStencilBuffer_);

	_RELEASE(pRenderTargetView_);
	_RELEASE(pDeviceContext_);
	_RELEASE(pDevice_);
	_RELEASE(pSwapChain_);

	return;
} // Shutdown()


// memory allocation
void* D3DClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}
	
	Log::Error(THIS_FUNC, "can't allocate memory for the D3DClass object");
	throw std::bad_alloc{};
}

void D3DClass::operator delete(void* p)
{
	_aligned_free(p);
}


// before rendering of each frame we need to set buffers
void D3DClass::BeginScene()
{
	const FLOAT bgColor[4] { 0.2f, 0.4f, 0.6f, 1.0f };
	
	// clear the render target view with particular color
	pDeviceContext_->ClearRenderTargetView(pRenderTargetView_, bgColor);

	// clear the depth stencil view with 1.0f values
	pDeviceContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}

// after all the rendering into the back buffer we need to present it on the screen
void D3DClass::EndScene(void)
{
	if (vsyncEnabled_)              // if vertical synchronization is enabled
	{
		pSwapChain_->Present(1, 0); // lock the refresh rate to necessary value
	}
	else
	{
		pSwapChain_->Present(0, 0); // present the back buffer as fast as possible
	}

	return;
}


// These two functions return us pointers to the device and device context respectively
ID3D11Device* D3DClass::GetDevice(void) const
{
	return pDevice_;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext(void) const
{
	return pDeviceContext_;
}

// These next three helper function initialize its parameters with references to
// the world matrix, projection matrix and the orthographic matrix respectively
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


// this function return us the information of the video card:
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	Log::Debug(THIS_FUNC_EMPTY);

	std::string videoCardNameStr{ "Video card name: " + (std::string)videoCardDescription_ };
	std::string videoMemoryStr{ "Video memory : " + std::to_string(videoCardMemory_) + " MB" };
	Log::Debug(THIS_FUNC, videoCardNameStr.c_str());
	Log::Debug(THIS_FUNC, videoMemoryStr.c_str());

	strncpy_s(cardName, 128, videoCardDescription_, 128);
	memory = videoCardMemory_;

	return;
}



void D3DClass::SetRenderState(D3DClass::RASTER_PARAMS rsParam)
{
	this->UpdateRasterStateParams(rsParam);

	// get a rasterizer state accroding to the updated params
	ID3D11RasterizerState* pRSState = GetRasterStateByHash(GetRSHash());

	// set a rasterizer state
	pDeviceContext_->RSSetState(pRSState);

}

// functions for enabling and disabling the Z buffer
void D3DClass::TurnZBufferOn(void)
{
	pDeviceContext_->OMSetDepthStencilState(pDepthStencilState_, 1);
	return;
}

void D3DClass::TurnZBufferOff(void)
{
	pDeviceContext_->OMSetDepthStencilState(pDepthDisabledStencilState_, 1);
	return;
}




// TurnOnAlphaBlending() allows us to turn on alpha blending by using OMSetBlendState()
// with our m_pAlphaEnableBlendingState blending state
void D3DClass::TurnOnAlphaBlending(void)
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn on the alpha blending
	pDeviceContext_->OMSetBlendState(pAlphaEnableBlendingState_, NULL, 0xFFFFFFFF);

	return;
}

// TurnOnAlphaBlending() allows us to turn off alpha blending by using OMSetBlendState()
// with our m_pAlphaDisableBlendingState blending state
void D3DClass::TurnOffAlphaBlending(void)
{
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn off the alpha blending
	pDeviceContext_->OMSetBlendState(pAlphaDisableBlendingState_, NULL, 0xFFFFFFFF);

	return;
}


// a function for enabling the additive blending that the sky plane clouds will require
void D3DClass::TurnOnAlphaBlendingForSkyPlane()
{
	// setup the blend factor 
	float blendFactor[4] = { 0.0f };

	// turn on the alpha blending 
	pDeviceContext_->OMSetBlendState(pAlphaBlendingStateForSkyPlane_, blendFactor, 0xFFFFFFFF);

	return;
}


// these two helpers are used in the render to texture functional
void D3DClass::SetBackBufferRenderTarget()
{
	// bind the render target view and depth stencil buffer to the output render pipeline
	pDeviceContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);

	return;
}

void D3DClass::ResetViewport()
{
	// set the viewport
	pDeviceContext_->RSSetViewports(1, &viewport_);
	
	return;
}






// ----------------------------------------------------------------------------------- //
//                                                                                     //
//                                PRIVATE METHODS                                      //
//                                                                                     //
// ----------------------------------------------------------------------------------- //

bool D3DClass::InitializeDirectX(HWND hwnd, const float nearZ, const float farZ)
{
	try
	{
		Log::Debug(THIS_FUNC_EMPTY);

		bool result = false;

		// enumerate adapters to get inforation about it
		result = this->EnumerateAdapters();
		COM_ERROR_IF_FALSE(result, "can't enumerate adapters");

		// --- initialize all the main parts of DirectX --- //
		result = this->InitializeSwapChain(hwnd, this->width_, this->height_);
		COM_ERROR_IF_FALSE(result, "can't initialize the swap chain");

		result = this->InitializeRenderTargetView();
		COM_ERROR_IF_FALSE(result, "can't initialize the render target view");
	
		result = this->InitializeDepthStencil();
		COM_ERROR_IF_FALSE(result, "can't initialize the depth stencil stuff");
		
		result = this->InitializeRasterizerState();
		COM_ERROR_IF_FALSE(result, "can't initialize the rasterizer state");

		result = this->InitializeViewport();
		COM_ERROR_IF_FALSE(result, "can't initialize the viewport");
		
		result = this->InitializeMatrices(nearZ, farZ);
		COM_ERROR_IF_FALSE(result, "can't initialize matrices");
		
		result = this->InitializeBlendStates();
		COM_ERROR_IF_FALSE(result, "can't initialize the blend states");

	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
} // InitializeDirectX()



  // get data about the video card, user's screen, etc.
bool D3DClass::EnumerateAdapters()
{
	//DXGI_ADAPTER_DESC adapterDesc;
	ULONGLONG stringLength;

	this->adapters_ = AdapterReader::GetAdapters();

	// check if we have any available IDXGI adapter
	bool result = this->adapters_.size() > 1;
	COM_ERROR_IF_FALSE(result, "can't find any IDXGI adapter");

	// store the dedicated video card memory in megabytes
	videoCardMemory_ = static_cast<int>(this->adapters_[0].description.DedicatedVideoMemory / 1024 / 1024);

	// convert the name of the video card to a character array and store it
	errno_t error = wcstombs_s(&stringLength, videoCardDescription_, 128, this->adapters_[0].description.Description, 128);
	COM_ERROR_IF_FALSE(error == 0, "can't conver the name of the video card");
	

	return true;
} // EnumerateAdapters()


// creates the swap chain, device and device context
bool D3DClass::InitializeSwapChain(HWND hwnd, const int width, const int height)
{
	HRESULT hr = S_OK;
	DXGI_SWAP_CHAIN_DESC swapChainDesc { 0 };
	D3D_FEATURE_LEVEL  featureLevel;
	UINT createDeviceFlags = 0;
	

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


	// ------------------------------------------------------------------------------ //
	//             CREATE THE SWAP CHAIN, DEVICE AND DEVICE CONTEXT                   //
	// ------------------------------------------------------------------------------ //

	// Setup the swap chain description
	swapChainDesc.BufferCount = 1;					// we have only one back buffer
	swapChainDesc.BufferDesc.Width = width;	// set the resolution of the back buffer
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// use a simple 32-bit surface 
	swapChainDesc.SampleDesc.Count = 1;				// setup of the multisampling
	swapChainDesc.SampleDesc.Quality = 0;


	if (vsyncEnabled_)	// if we use a vertical synchronization
	{
		//Log::Get()->Debug(THIS_FUNC, "VSYNC enabled mode");

		// set the refresh rate of the back buffer
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;//this->numerator_;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;// this->denominator_;
	}
	else // we don't use a vertical synchronization
	{
		//Log::Get()->Debug(THIS_FUNC, "VSYNC disabled mode");
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;		// set the refresh rate of the back buffer
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}


	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// use the back buffer as the render target output
	swapChainDesc.OutputWindow = hwnd;								// set the current window
	swapChainDesc.Windowed = !this->fullScreen_;				    // set full screen or windowed mode;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// a rasterizer method to render an image on a surface
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// how to scale an image to fit it to the screen resolution
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			// discard the content of the back buffer after presenting
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;	// tell DirectX which feature level we want to use
											// Create the swap chain, device and device context

	hr = D3D11CreateDeviceAndSwapChain(
		this->adapters_[0].pAdapter, // IDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN,     // the driver type is unknown.
		NULL,                        // for software driver type
		createDeviceFlags,           // flags for runtime layers
		&featureLevel,               // feature levels array
		1,                           // number of the feature levels in the array
		D3D11_SDK_VERSION,
		&swapChainDesc,              // swapChain description
		&pSwapChain_,                // a swapchaing address
		&pDevice_,                   // a device address
		nullptr,                     // supported feature level
		&pDeviceContext_);           // device context address

	COM_ERROR_IF_FAILED(hr, "can't create the swap chain, device and device context");

	return true;
} // InitializeSwapChain()


// creates and sets up the render target view
bool D3DClass::InitializeRenderTargetView()
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pBackBuffer = nullptr;

	// get the buffer from the swap chain which we will use as a 2D Texture
	hr = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (VOID**)&pBackBuffer);
	COM_ERROR_IF_FAILED(hr, "can't get a buffer from the swap chain");

	// create a render target view 
	hr = pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView_);
	_RELEASE(pBackBuffer);
	COM_ERROR_IF_FAILED(hr, "can't create a render target view");

	return true;
} // InitializeRenderTargetView()

// creates the depth stencil buffer, depth stencil state, depth stencil view,
// and disabled stencil state which is necessary for 2D rendering
bool D3DClass::InitializeDepthStencil()
{
	HRESULT hr = S_OK;

	// initialize all the necessary parts of depth stencil
	InitializeDepthStencilTextureBuffer();
	InitializeDepthStencilState();           // the depth stencil state with ENABLED depth
	InitializeDepthDisabledStencilState();   // the depth stencil state with DISABLED depth
	InitializeDepthStencilView();

	// Set the depth stencil state.
	pDeviceContext_->OMSetDepthStencilState(pDepthStencilState_, 1);

	// bind together the render target view and the depth stencil view to the output merger stage
	pDeviceContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);

	return true;
} // InitializeDepthStencil()


// initialize a texture buffer for the depth stencil
bool D3DClass::InitializeDepthStencilTextureBuffer()
{
	/* OLD STYLE

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc { 0 };

	// describe our Depth/Stencil Buffer
	depthStencilBufferDesc.Width = this->width_;
	depthStencilBufferDesc.Height = this->height_;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24 bits for the depth and 8 bits for the stencil
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;
	*/

	CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->width_, this->height_);
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// Create the depth stencil buffer
	HRESULT hr = pDevice_->CreateTexture2D(&depthStencilTextureDesc, nullptr, &pDepthStencilBuffer_);
	COM_ERROR_IF_FAILED(hr, "can't create the depth stencil buffer");

	return true;
} // InitializeDepthStencilTextureBuffer()


// initializes the depth stencil state
bool D3DClass::InitializeDepthStencilState()
{
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


	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	// Create a depth stencil state
	HRESULT hr = pDevice_->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState_);
	COM_ERROR_IF_FAILED(hr, "can't create a depth stencil state");

	return true;
} // InitializeDepthStencilState()


// now create a second depth stencil state which turns off the Z buffer for 2D rendering.
// The only difference is that the DepthEnable parameter is set to false,
// all other parameters are the same
// as the another depth stencil state
// (DEPTH DISABLED STENCIL STATE IS NECESSARY FOR 2D RENDERING)
bool D3DClass::InitializeDepthDisabledStencilState()
{
	/* OLD STYLE

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc { 0 }; // depth stencil description for disabling the stencil


	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;

	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	*/


	CD3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc(D3D11_DEFAULT);
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.DepthEnable = false;

	// create the state using the device
	HRESULT hr = pDevice_->CreateDepthStencilState(&depthDisabledStencilDesc, &pDepthDisabledStencilState_);
	COM_ERROR_IF_FAILED(hr, "can't create the depth disabled stencil state");

	return true;
}


bool D3DClass::InitializeDepthStencilView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// Setup the depth stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create a depth stencil view
	HRESULT hr = pDevice_->CreateDepthStencilView(pDepthStencilBuffer_,
		&depthStencilViewDesc,
		&pDepthStencilView_);
	COM_ERROR_IF_FAILED(hr, "can't create a depth stencil view");

	return true;
}


// creates/sets up the rasterizer state
bool D3DClass::InitializeRasterizerState()
{
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


	// set up the rasterizer state description
	HRESULT hr = S_OK;
	ID3D11RasterizerState* pRasterState = nullptr;
	CD3D11_RASTERIZER_DESC pRasterDesc(D3D11_DEFAULT);    // all the values of description are default  
	//rasterDesc.FrontCounterClockwise = true;
	//rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	

	// 1. create a fill solid + cull back rasterizer state
	pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
	COM_ERROR_IF_FAILED(hr, "can't create a raster state: fill solid + cull back");

	this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
	this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
	rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});



	// 2. create a fill solid + cull front rasterizer state
	pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
	hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
	COM_ERROR_IF_FAILED(hr, "can't create a raster state: fill solid + cull front");

	rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
	this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
	this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_FRONT);
	rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});



	// 3. create a fill wireframe + cull back rasterizer state
	pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
	COM_ERROR_IF_FAILED(hr, "can't create a raster state: fill wireframe + cull back");

	rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
	this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_WIREFRAME);
	this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
	rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});



	// 4. create a fill wireframe + cull front rasterizer state
	pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
	hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
	COM_ERROR_IF_FAILED(hr, "can't create a raster state: fill wireframe + cull front");
	
	rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
	this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_WIREFRAME);
	this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_FRONT);
	rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});

	// 5. create a fill solid + cull none rasterizer state
	pRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	pRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	hr = pDevice_->CreateRasterizerState(&pRasterDesc, &pRasterState);
	COM_ERROR_IF_FAILED(hr, "can't create a raster state: fill solid + cull none");

	rasterStateHash_ &= 0;      // reset the rasterizer state hash for using it again
	this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
	this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_NONE);
	rasterizerStatesMap_.insert(std::pair<uint8_t, ID3D11RasterizerState*>{GetRSHash(), pRasterState});


	// AFTER ALL: reset the rasterizer state hash after initialization and set the default params
	rasterStateHash_ &= 0;
	this->turnOnRasterParam(RASTER_PARAMS::FILL_MODE_SOLID);
	this->turnOnRasterParam(RASTER_PARAMS::CULL_MODE_BACK);
	

	return true;
} // InitializeRasterizerState()

// sets up the viewport 
bool D3DClass::InitializeViewport()
{
	// Setup the viewport
	viewport_.Width = static_cast<FLOAT>(this->width_);
	viewport_.Height = static_cast<FLOAT>(this->height_);
	viewport_.MaxDepth = 1.0f;
	viewport_.MinDepth = 0.0f;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;

	// Set the viewport
	pDeviceContext_->RSSetViewports(1, &viewport_);

	return true;
} // InitializeViewport()

// initialize world, projection, and ortho matrices to it's default values
bool D3DClass::InitializeMatrices(const float nearZ, const float farZ)
{
	// Initialize the world matrix 
	this->worldMatrix_ = DirectX::XMMatrixIdentity();

	// Initialize the orthographic matrix for 2D rendering
	this->orthoMatrix_ = DirectX::XMMatrixOrthographicLH(
		static_cast<float>(this->width_),
		static_cast<float>(this->height_),
		nearZ,
		farZ);

	return true;
} // InitializeMatrices()


// creates and sets up the blend states
bool D3DClass::InitializeBlendStates()
{
	HRESULT hr = S_OK;
	D3D11_BLEND_DESC blendDesc{ 0 };            // description for setting up the two new blend states	   
	D3D11_RENDER_TARGET_BLEND_DESC rtbd{ 0 };

	// create an alpha disabled blend state description
	rtbd.BlendEnable = FALSE;
	rtbd.SrcBlend       = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;  // was: D3D11_BLEND_ONE
	rtbd.DestBlend      = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA; 
	rtbd.BlendOp        = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha  = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha   = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL; // == 0x0F;

	blendDesc.RenderTarget[0] = rtbd;

	// create the blend state using the description
	hr = pDevice_->CreateBlendState(&blendDesc, &pAlphaDisableBlendingState_);
	COM_ERROR_IF_FAILED(hr, "can't create the alpha disabled blend state");



	// modify the description to create an alpha enabled blend state description
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	// create the blend state using the desription
	hr = pDevice_->CreateBlendState(&blendDesc, &pAlphaEnableBlendingState_);
	COM_ERROR_IF_FAILED(hr, "can't create the alpha enabled blend state");


	// setup the description for the additive blending that the sky plane clouds will require
	rtbd.BlendEnable = TRUE;
	rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL; // == 0x0F;

	blendDesc.RenderTarget[0] = rtbd;

	// create the blend state using the description
	hr = pDevice_->CreateBlendState(&blendDesc, &pAlphaBlendingStateForSkyPlane_);
	COM_ERROR_IF_FAILED(hr, "can't create the alpha blending state for sky plane");

	return true;
} // InitializeBlendStates()


// setup a rasterizer state params 
void D3DClass::UpdateRasterStateParams(D3DClass::RASTER_PARAMS rsParam)
{
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
			Log::Error(THIS_FUNC, "an unknown rasterizer state parameter");
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
		Log::Error(THIS_FUNC, errorMsg.c_str());  // print error message

		// print the hash
		int symbol = 0;
		for (int i = 7; i >= 0; i--)
		{
			symbol = (hash >> i) & 1;
			printf("%d ", symbol);
		}
		printf("\n");

		COM_ERROR_IF_FALSE(false, "wrong hash");  // throw an exception
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