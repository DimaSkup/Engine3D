/////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
// Here we initialize the Direct3D
/////////////////////////////////////////////////////////////////////
#include "d3dclass.h"
#include <iostream>

// Empty constructor
D3DClass::D3DClass(void)
{
	Log::Get()->Debug(THIS_FUNC, "");

	m_pSwapChain = nullptr;
	m_pDevice = nullptr;
	m_pDeviceContext = nullptr;
	m_pRenderTargetView = nullptr;

	m_pDepthStencilBuffer = nullptr;
	m_pDepthStencilState = nullptr;
	m_pDepthDisabledStencilState = nullptr;
	m_pDepthStencilView = nullptr;
	m_pRasterState = nullptr;

	m_vsync_enabled = false;
	m_videoCardDescription[0] = '\0';
	m_videoCardMemory = 0;

	m_pAlphaEnableBlendingState = nullptr;
	m_pAlphaDisableBlendingState = nullptr;
}


// Copy constructor
D3DClass::D3DClass(const D3DClass& another)
{
}

// Desctructor
D3DClass::~D3DClass(void)
{
}


// this function initializes Direct3D
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool VSYNC_ENABLED,
	HWND hwnd, bool FULL_SCREEN,
	float screenNear, float screenDepth)
{
	Log::Get()->Debug(THIS_FUNC, "the beginning");
	HRESULT hr = S_OK;

	// DXGI variables, etc
	IDXGIFactory* factory = nullptr;	// a pointer to the DirectX graphics interface
	IDXGIAdapter* adapter = nullptr;	// a pointer to the adapter (video card) interface
	IDXGIOutput*  output = nullptr;		// a pointer to interface of the display output adapter 
	DXGI_ADAPTER_DESC adapterDesc;		// contains description of the adapter (video card)
	DXGI_MODE_DESC* displayModeList = nullptr;	// a pointer to the list of display adapter modes
	UINT numModes = 0;							// a number of dispay modes
	UINT numerator = 0, denominator = 0;		// numerator and denominator of the display refresh rate
	UINT error = 0;								// info about errors of conterting of WCHAR line into simple char line
	size_t stringLength = 0;

	// DirectX stuff
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL  featureLevel;
	UINT createDeviceFlags = 0;
	ID3D11Texture2D* pBackBuffer = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc; // depth stencil description for disabling the stencil 
	

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// define if VSYNC is enabled or not
	m_vsync_enabled = VSYNC_ENABLED;

	// Create DXGI Factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the DXGI Factory");
		return false;
	}

	// Enumerate adapters (video cards)
	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't enumerate adapters (video cards)");
		return false;
	}

	// Enumerate ouput adapters (display adapters)
	hr = adapter->EnumOutputs(0, &output);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't enumerate ouput adapters (display adapters)");
		return false;
	}

	// Get the number of display output modes which fit to the DXGI_FORMAT_R8G8B8A8_UNORM format
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;

	hr = output->GetDisplayModeList(format, flags, &numModes, nullptr);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't get the number of display modes");
	}

	// allocate the memory for the display modes description list
	displayModeList = new(std::nothrow) DXGI_MODE_DESC[numModes];

	// initialize the display mode list with modes which fit to the DXGI_FORMAT_R8G8B8A8_UNORM format
	hr = output->GetDisplayModeList(format, flags, &numModes, displayModeList);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the display mode list");
	}

	// look for a mode which has the necessary screen resolution and get its refresh rate 
	for (size_t i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == static_cast<UINT>(screenWidth) &&
			displayModeList[i].Height == static_cast<UINT>(screenHeight))
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// get description of the adapter (video card)
	hr = adapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't get description of the adapter (video card)");
		return false;
	}

	// get the video card memory amount in megabytes
	m_videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// get the video card name
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		Log::Get()->Error(THIS_FUNC, "can't convert the video card description from WCHAR type into char line");
		return false;
	}


	// clear the memory from DXGI variables
	_DELETE(displayModeList);
	_RELEASE(output);
	_RELEASE(adapter);
	_RELEASE(factory);

	// check the data
	Log::Get()->Debug("video card memory      = %d MB", m_videoCardMemory);
	Log::Get()->Debug("video card name        = %s", m_videoCardDescription);
	Log::Get()->Debug("video card refreshRate = %d:%d", numerator, denominator);


	// ------------------------------------------------------------------------------ //
	//             CREATE THE SWAP CHAIN, DEVICE AND DEVICE CONTEXT                   //
	// ------------------------------------------------------------------------------ //

	// Initialize the swap chain description
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// Setup the swap chain description
	swapChainDesc.BufferCount = 1;					// we have only one back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;	// set the resolution of the back buffer
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// use a simple 32-bit surface 
	swapChainDesc.SampleDesc.Count = 1;				// setup of the multisampling
	swapChainDesc.SampleDesc.Quality = 0;

	if (m_vsync_enabled)	// if we use a vertical synchronization
	{
		Log::Get()->Debug(THIS_FUNC, "VSYNC enabled mode");
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;		// set the refresh rate of the back buffer
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else					// we don't use a vertical synchronization
	{
		Log::Get()->Debug(THIS_FUNC, "VSYNC disabled mode");
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;		// set the refresh rate of the back buffer
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// use the back buffer as the render target output
	swapChainDesc.OutputWindow = hwnd;								// set the current window
	swapChainDesc.Windowed = !FULL_SCREEN;							// set or not a full screen mode;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// a rasterizer method to render an image on a surface
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// how to scale an image to fit it to the screen resolution
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			// discard the content of the back buffer after presenting
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;	// tell DirectX which feature level we want to use

											// Create the swap chain, device and device context
	hr = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_pSwapChain,
		&m_pDevice,
		nullptr,
		&m_pDeviceContext);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the swap chain");
		return false;
	}


	// ------------------------------------------------------------------------- //
	//                   CREATE THE RENDER TARGET VIEW							 //
	// ------------------------------------------------------------------------- //

	// get the buffer from the swap chain which we will use as a 2D Texture
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (VOID**)&pBackBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't get a buffer from the swap chain");
		return false;
	}

	// create a render target view 
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	_RELEASE(pBackBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create a render target view");
		return false;
	}

	// ------------------------------------------------------------------------- //
	//             CREATE THE DEPTH STENCIL BUFFER,                              //
	//          DEPTH STENCIL STATE AND DEPTH STENCIL VIEW						 //
	// ------------------------------------------------------------------------- //

	// Intialize the depth stencil buffer description
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// Setup the depth stencil buffer description
	depthStencilBufferDesc.Width = screenWidth;
	depthStencilBufferDesc.Height = screenHeight;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24 bits for the depth and 8 bits for the stencil
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	// Create the depth stencil buffer
	hr = m_pDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &m_pDepthStencilBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the depth stencil buffer");
		return false;
	}

	// Initialize the depth stencil state description
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	// Setup the depth stencil state description
	depthStencilDesc.DepthEnable = true;	// enable depth testing
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// a part of the depth buffer to writing
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;	// a function to compare source depth data against exiting (destination) depth data

	depthStencilDesc.StencilEnable = true;	// enable stencil testing
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

																		// Create a depth stencil state
	hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create a depth stencil state");
		return false;
	}

	// Set the depth stencil state.
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	// Initialize the depth stencil view description
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// Setup the depth stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create a depth stencil view
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer,
		&depthStencilViewDesc,
		&m_pDepthStencilView);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create a depth stencil view");
		return false;
	}

	// bind together the render target view and the depth stencil view to the output merger stage
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


	// ---------------------------------------------------------------------------- //
	//                     CREATE THE RASTERIZER STATE                              //
	// ---------------------------------------------------------------------------- //

	// Initialize the rasterizer state description
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	// Setup the rasterizer state description
	rasterDesc.AntialiasedLineEnable = false;   // not use line anti-aliasing algorithm (is used if param MultisampleEnable = false)
	rasterDesc.CullMode = D3D11_CULL_BACK;      // not render triangles which are back facing
	rasterDesc.DepthBias = 0;                   // a depth bias magnitude which is added to pixel's depth
	rasterDesc.DepthBiasClamp = 0.0f;           // a maximum magnitude of pixel depth bias
	rasterDesc.DepthClipEnable = true;          // enable clipping which is based on distance
	rasterDesc.FillMode = D3D11_FILL_SOLID;     // a mode of filling primitives during rendering
	rasterDesc.FrontCounterClockwise = false;   // a triangle is front facing if its vertices are clockwise and back facing if its vertices are counter-clockwise
	rasterDesc.MultisampleEnable = false;       // use alpha line anti-aliasing algorithm
	rasterDesc.ScissorEnable = false;           // not use clipping for pixels which are around of the scissor quadrilateral
	rasterDesc.SlopeScaledDepthBias = 0.0f;     // scalar of pixel depth slope

												// create a rasterizer state
	hr = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create a raster state");
		return false;
	}

	// set the rasterizer state
	m_pDeviceContext->RSSetState(m_pRasterState);


	// ------------------------------------------------------------------------ //
	//               CREATE THE VIEWPORT                                        //
	// ------------------------------------------------------------------------ //

	// Initialize the viewport description
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	// Setup the viewport description
	viewport.Width = static_cast<FLOAT>(screenWidth);
	viewport.Height = static_cast<FLOAT>(screenHeight);
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	// Set the viewport
	m_pDeviceContext->RSSetViewports(1, &viewport);


	// ------------------------------------------------------------------------ //
	//                     INITIALIZE MATRICES                                  //
	// ------------------------------------------------------------------------ //

	// Initialize the world matrix 
	m_worldMatrix = DirectX::XMMatrixIdentity();

	// Initialize the projection matrix
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH((float)D3DX_PI / 4.0f,
		(float)screenWidth / (float)screenHeight,
		screenNear, screenDepth);

	// Initialize the orthographic matrix for 2D rendering
	m_orthoMatrix = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight,
		screenNear, screenDepth);



	// lear the second depth stencil state before setting the parameters
	ZeroMemory(&depthDisabledStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	// now create a second depth stencil state which turns off the Z buffer for 2D rendering.
	// The only difference is that DepthEnable is set to false, all other parameters are the same
	// as the other depth stencil state
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

	// create the state using the device
	hr = m_pDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_pDepthDisabledStencilState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the depth stencil state");
		return false;
	}


	// ------------------------------------------------------------------------ //
	//                     INITIALIZE BLEND STATES                              //
	// ------------------------------------------------------------------------ //

	D3D11_BLEND_DESC blendStateDescription;            // description for setting up the two new blend states
	// clear the blend state description
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	//blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;

	// create an alpha enabled blen state description

blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	
	// create the blend state using the description
	hr = m_pDevice->CreateBlendState(&blendStateDescription, &m_pAlphaDisableBlendingState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the alpha enabled blend state");
		//return false;
	}

	// modify the description to create an alpha disabled blend state description
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;

	// create the blend state using the desription
	hr = m_pDevice->CreateBlendState(&blendStateDescription, &m_pAlphaEnableBlendingState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the alpha disabled blend state");
		return false;
	}
	else
	{
		Log::Get()->Error(THIS_FUNC, "disabled is created");
	}

	if ((m_pAlphaDisableBlendingState != nullptr) && (m_pAlphaEnableBlendingState != nullptr))
	{
		Log::Get()->Error("NOT NULL");
	}
	else
	{
		Log::Get()->Error("NULL");
	}


	Log::Get()->Debug(THIS_FUNC, "Direct3D is initialized successfully");

	return true;
}


// Set Screen State and release the allocated memory
void D3DClass::Shutdown(void)
{
	// set a windowed mode as active
	if (m_pSwapChain)
		m_pSwapChain->SetFullscreenState(FALSE, nullptr);

	_RELEASE(m_pAlphaEnableBlendingState);
	_RELEASE(m_pAlphaDisableBlendingState);
	_RELEASE(m_pDepthDisabledStencilState);
	_RELEASE(m_pRasterState);
	_RELEASE(m_pDepthStencilView);
	_RELEASE(m_pDepthStencilState);
	_RELEASE(m_pDepthStencilBuffer);

	_RELEASE(m_pRenderTargetView);
	_RELEASE(m_pDeviceContext);
	_RELEASE(m_pDevice);
	_RELEASE(m_pSwapChain);

	return;
}


// before rendering of each frame we need to set buffers
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	// clear the render target view with particular color
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, D3DXCOLOR(red, green, blue, alpha));

	// clear the depth stencil view with 1.0f values
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

// after all the rendering into the back buffer we need to present it on the screen
void D3DClass::EndScene(void)
{
	if (m_vsync_enabled) // if vertical synchronization is enabled
	{
		m_pSwapChain->Present(1, 0); // lock the refresh rate to necessary value
	}
	else
	{
		m_pSwapChain->Present(0, 0); // present the back buffer as fast as possible
	}

	return;
}


// These two functions return us pointers to the device and device context respectively
ID3D11Device* D3DClass::GetDevice(void)
{
	return m_pDevice;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext(void)
{
	return m_pDeviceContext;
}

// These next three helper function initialize its parameters with references to
// the world matrix, projection matrix and the orthographic matrix respectively
void D3DClass::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}


// this function return us the information of the video card:
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strncpy_s(cardName, 128, m_videoCardDescription, 128);
	//(cardName, m_videoCardDescription, 128);
	memory = m_videoCardMemory;

	return;
}


// functions for enabling and disabling the Z buffer
void D3DClass::TurnZBufferOn(void)
{
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
	return;
}

void D3DClass::TurnZBufferOff(void)
{
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthDisabledStencilState, 1);
	return;
}


// TurnOnAlphaBlending() allows us to turn on alpha blending by using OMSetBlendState()
// with our m_pAlphaEnableBlendingState blending state
void D3DClass::TurnOnAlphaBlending(void)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn on the alpha blending
	m_pDeviceContext->OMSetBlendState(m_pAlphaEnableBlendingState, blendFactor, 0xFFFFFFFF);

	return;
}

// TurnOnAlphaBlending() allows us to turn off alpha blending by using OMSetBlendState()
// with our m_pAlphaDisableBlendingState blending state
void D3DClass::TurnOffAlphaBlending(void)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// turn off the alpha blending
	m_pDeviceContext->OMSetBlendState(m_pAlphaDisableBlendingState, blendFactor, 0xFFFFFFFF);

	return;
}