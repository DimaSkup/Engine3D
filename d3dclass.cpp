///////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
// Here we initialize Direct3D
///////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_pSwapChain = nullptr;
	m_pDevice = nullptr;
	m_pDeviceContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilBuffer = nullptr;
	m_pDepthStencilState = nullptr;
	m_pDepthStencilView = nullptr;
	m_pRasterState = nullptr;

	m_vsync_enabled = false;
	m_videoCardDescription[0] = '/0';
	m_videoCardMemory = -1;
}

D3DClass::D3DClass(const D3DClass& another)
{
}

D3DClass::~D3DClass(void)
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, 
							bool vsync, HWND hwnd, bool fullScreen, 
							float screenNear, float screenDepth)
{
	HRESULT hr = S_OK;

	// DirectX graphics interface stuff
	IDXGIFactory* factory = nullptr;
	IDXGIAdapter* adapter = nullptr;
	IDXGIOutput* adapterOutput = nullptr;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_MODE_DESC* displayModesList = nullptr;
	size_t stringLength = 0;
	int numerator = 0, denominator = 0;
	UINT error = 0, numModes = 0;

	// Direct3D stuff
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D11Texture2D* pBackBuffer = nullptr;
	D3D11_VIEWPORT viewport;
	D3D_FEATURE_LEVEL featureLevel;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	UINT createDeviceFlag = 0;
#ifdef _DEBUG
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	


	
	// initialize a vsync flag
	m_vsync_enabled = vsync;


	// ---------------------------------------------------------------------- // 
	//        GETTING OF THE VIDEO CARD DESCRIPTION, MEMORY SIZE              //
	//        AND DISPLAY REFRESH RATE                                        //
	// ---------------------------------------------------------------------- //

	// Create DirectX graphics interface factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the DXGI Factory");
		return false;
	}

	// Get an adapter (video card) interface
	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't get the adapter (video card) interface");
		return false;
	}

	// Get an adapterOutput (display) interface
	hr = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't get the adapterOutput (display) interface");
		return false;
	}

	// Get a number of the display modes which fit the DXGI_FORMAT_R8G8B8A8_UNORM format
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;

	hr = adapterOutput->GetDisplayModeList(format, flags, &numModes, nullptr);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't get the number of display modes");
		return false;
	}

	// allocate the memory for this number of the display modes
	displayModesList = new(std::nothrow) DXGI_MODE_DESC[numModes];
	if (!displayModesList)
	{
		Log::Get()->Error("D3DClass::Initialize(): can't allocate the memory for the display modes descriptions list");
		return false;
	}

	// initialize the display modes list
	hr = adapterOutput->GetDisplayModeList(format, flags, &numModes, displayModesList);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't initialize the display modes list");
		return false;
	}

	// now we have the list which contains display modes description
	// we neen to find out a refresh rate which fits to our screenWidth and screenHeight
	for (int i = 0; i < numModes; i++)
	{
		if (displayModesList[i].Width == static_cast<unsigned int>(screenWidth) &&
			displayModesList[i].Height == static_cast<unsigned int>(screenHeight))
		{
			Log::Get()->Debug("D3DClass::Initialize(): display resolution: %dx%d", screenWidth, screenHeight);
			numerator = static_cast<int>(displayModesList[i].RefreshRate.Numerator);
			denominator = static_cast<int>(displayModesList[i].RefreshRate.Denominator);
			break;	// when we found some we break out of the cycle
		}
	}

	// Get the description of the adapter (video card)
	hr = adapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't get the description of the adapter (video card)");
		return false;
	}

	// Get the memory amount of the adapter (video card) in megabytes
	m_videoCardMemory = adapterDesc.DedicatedVideoMemory / 1024 / 1024;

	// Get the description of the adapter (video card) as a charater line
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		Log::Get()->Error("D3DClass::Initialize(): can't convert the video card description into a character line");
		return false;
	}

	// Check data
	Log::Get()->Debug("D3DClass::Initialize(): display refresh rate = %d:%d", numerator, denominator);
	Log::Get()->Debug("D3DClass::Initialize(): video card description: %s", m_videoCardDescription);
	Log::Get()->Debug("D3DClass::Initialize(): video card memory size: %d MB", m_videoCardMemory);

	// release the memory out of DXGI stuff
	_DELETE(displayModesList);
	_RELEASE(adapterOutput);
	_RELEASE(adapter);
	_RELEASE(factory);



	// ----------------------------------------------------------------- //
	//          CREATION OF THE SWAP CHAIN DESCRIPTION                   //
	// ----------------------------------------------------------------- //
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;	// the number of back buffers
	swapChainDesc.BufferDesc.Width = screenWidth;	// set the back buffer width and height
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// set regular 32-bit surface

	// set the refresh rate of the back buffer
	if (m_vsync_enabled)
	{
		Log::Get()->Debug("D3DClass::Initialize(): vsync enabled mode");
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		Log::Get()->Debug("D3DClass::Initialize(): vsync disabled mode");
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.Windowed = !fullScreen;	// set fullscreen or windowed mode
	swapChainDesc.OutputWindow = hwnd;		// set the window handler to render to
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // set usage of the back buffer
	swapChainDesc.SampleDesc.Count = 1;		// turn multisampling off
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// discard the content of the back buffer after the presenting
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// ---------------------------------------------------------------------- //
	//   CREATE THE SWAP CHAIN, DIRECT3D DEVICE AND DIRECT3D DEVICE CONTEXT   //
	// ---------------------------------------------------------------------- //
	hr = D3D11CreateDeviceAndSwapChain(nullptr, 
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										createDeviceFlag,
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
		Log::Get()->Error("D3DClass::Initialize(): can't create the swap chain");
		return false;
	}


	// ---------------------------------------------------------------------- //
	//                CREATION OF THE RENDER TARGET VIEW                      //
	// ---------------------------------------------------------------------- //

	// Get the pointer to the back buffer
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (VOID**)&pBackBuffer);
	if (FAILED(hr))
	{
		_RELEASE(pBackBuffer);
		Log::Get()->Error("D3DClass::Initialize(): can't get the back buffer from the swap chain");
		return false;
	}

	// Create the render target view using the pointer to the back buffer
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	_RELEASE(pBackBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the render target view");
		return false;
	}

	// ---------------------------------------------------------------------- //
	//      SET UP A DEPTH BUFFER DESCRIPTION, CREATE A DEPTH BUFFER,         //
	//      DEPTH STENCIL AND DEPTH STENCIL VIEW                              //
	// ---------------------------------------------------------------------- //

	// Initialize the description of the depth buffer
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// Set up the description of the depth buffer;
	// the stencil buffer can be used to achieve effects such as:
	// motion blur, volumetric shadows, etc.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description
	hr = m_pDevice->CreateTexture2D(&depthBufferDesc, nullptr, &m_pDepthStencilBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the depth stencil buffer");
		return false;
	}

	// Initialize the description of the stencil state
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	// Set up the description of the stencil state
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create a depth stencil state
	hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the depth stencil state");
		return false;
	}

	// set the depth stencil state 
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);


	// Initialize the depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// Set up the depth stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 
											&depthStencilViewDesc, 
											&m_pDepthStencilView);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the depth stencil view");
		return false;
	}

	// Bind the render target view and depth stencil view to the output render pipeline
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);


	// ---------------------------------------------------------------------- //
	//                   CREATE THE RASTERIZER STATE                          //
	// ---------------------------------------------------------------------- //


	// Setup the raster description which will determine how and what polygons will be drawn
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	
	// Create the rasterizer state from the description we just filled out
	hr = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the rasterizer state");
		return false;
	}

	// Now set the rasterizer state
	m_pDeviceContext->RSSetState(m_pRasterState);



	// ---------------------------------------------------------------------- //
	//                      CREATE THE VIEWPORT                               //
	// ---------------------------------------------------------------------- //

	// Setup the viewport for rendering
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport
	m_pDeviceContext->RSSetViewports(1, &viewport);


	// ---------------------------------------------------------------------- //
	//                      CREATE MATRICES                                   //
	// ---------------------------------------------------------------------- //

	// Create the projection matrix for 3D rendering
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, (float)D3DX_PI / 4.0f,
								(float)screenWidth / (float)screenHeight,
								screenNear, screenDepth);

	// Initialize the world matrix to the identity matrix
	D3DXMatrixIdentity(&m_worldMatrix);

	// Create an orthographic projection for 2D rendering (to render GUI/text/etc)
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight,
		screenNear, screenDepth);

	Log::Get()->Debug("D3DClass::Initialize(): Direct3D is successfully initialized");
	return true;
}


void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release 
	// the swap chain it will throw an exception
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(false, NULL);
	}

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

// at the beginning of each frame 
// this function initializes the buffers so they are blank and ready to be draw to
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	// Clear the back buffer
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, D3DXCOLOR(red, green, blue, alpha));

	// Clear the depth buffer
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

// tells the swap chain to display our 3D scene once all the drawing
// has completed at the end of each frame
void D3DClass::EndScene()
{
	
	// Present the back buffer to the screen since rendering is complete
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate
		m_pSwapChain->Present(1, 0);
	}
	else 
	{
		// Present as fast as possible
		m_pSwapChain->Present(0, 0);
	}

	return;
}

// These next functions simple get pointer to the Direct3D device and device context
ID3D11Device* D3DClass::GetDevice()
{
	return m_pDevice;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_pDeviceContext;
}

// The next three helper functions give copies of 
// the projection, world and orthographic matrices to calling functions
void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

// returns by reference the name of the video card and the amount of dedicated memory on the video card
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strncpy(cardName, m_videoCardDescription, 128);
	memory = m_videoCardMemory;
	return;
}
