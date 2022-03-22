/////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
// Here we initialize the Direct3D
/////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

// Empty constructor
D3DClass::D3DClass(void)
{
	Log::Get()->Debug(__FUNCTION__);

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
	m_videoCardMemory = 0;
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
	Log::Get()->Debug("%s:%s", __FUNCTION__, "the beginning");
	HRESULT hr = S_OK;

	// DXGI variables, etc
	IDXGIFactory* factory = nullptr;	// a pointer to the DirectX graphics interface
	IDXGIAdapter* adapter = nullptr;	// a pointer to the adapter (video card) interface
	IDXGIOutput*  output = nullptr;		// a pointer to interface of the display output adapter 
	DXGI_ADAPTER_DESC adapterDesc;		// contains description of the adapter (video card)
	DXGI_MODE_DESC* displayModeList = nullptr;	// a pointer to the list of display adapter modes
	UINT numModes = 0;					// a number of dispay modes
	int numerator = 0, denominator = 0;	// numerator and denominator of the display refresh rate
	UINT error = 0;						// info about errors of conterting of WCHAR line into simple char line
	size_t stringLength = 0;

	// define if VSYNC is enabled or not
	m_vsync_enabled = VSYNC_ENABLED;

	// Create DXGI Factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
	{
		Log::Get()->Error("%s:%s", __FUNCTION__, "can't create the DXGI Factory");
		return false;
	}

	// Enumerate adapters (video cards)
	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
	{
		Log::Get()->Error("%s%s", __FUNCTION__, "can't enumerate adapters (video cards)");
		return false;
	}

	// Enumerate ouput adapters (display adapters)
	hr = adapter->EnumOutputs(0, &output);
	if (FAILED(hr))
	{
		Log::Get()->Error("%s:%s", __FUNCTION__, "can't enumerate ouput adapters (display adapters)");
		return false;
	}

	// Get the number of display output modes which fit to the DXGI_FORMAT_R8G8B8A8_UNORM format
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;

	hr = output->GetDisplayModeList(format, flags, &numModes, nullptr);
	if (FAILED(hr))
	{
		Log::Get()->Error("%s:%s", __FUNCTION__, "can't get the number of display modes");
	}

	// allocate the memory for the display modes description list
	displayModeList = new(std::nothrow) DXGI_MODE_DESC[numModes];

	// initialize the display mode list with modes which fit to the DXGI_FORMAT_R8G8B8A8_UNORM format
	hr = output->GetDisplayModeList(format, flags, &numModes, displayModeList);
	if (FAILED(hr))
	{
		Log::Get()->Error("%s:%s", __FUNCTION__, "can't initialize the display mode list");
	}

	// look for a mode which has the necessary screen resolution and get its refresh rate 
	for (size_t i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == static_cast<UINT>)
	}





	return true;
}


// Set Screen State and release the allocated memory
void D3DClass::Shutdown(void)
{
	return;
}


// before rendering of each frame we need to set buffers
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	// clear the render target view with particular color

	return;
}

// after all the rendering into the back buffer we need to present it on the screen
void D3DClass::EndScene(void)
{
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
void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}


// this function return us the information of the video card:
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strncpy(cardName, m_videoCardDescription, 128);
	memory = m_videoCardMemory;

	return;
}













































/*
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


	// ------------------------------------------------------------------------ //
	// CREATION OF THE DEPTH BUFFER, DEPTH STENCIL STATE AND DEPTH STENCIL VIEW //
	// ------------------------------------------------------------------------ //

	// Initialize the depth-stencil buffer description
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// Set up the depth-stencil buffer description
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

	// Create a depth-stencil buffer
	hr = m_pDevice->CreateTexture2D(&depthBufferDesc, nullptr, &m_pDepthStencilBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create a depth-stencil buffer");
		return false;
	}


	// Initialize the depth-stencil state description
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	// Set up the depth-stencil state description
	depthStencilDesc.DepthEnable = true;	// enable depth testing
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// a portion of the buffer for depth testing
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;	// a function to compare source depth data against existing (destination) depth data

	depthStencilDesc.StencilEnable = true;	// enable stencil testing
	depthStencilDesc.StencilReadMask = 0xFF;	// a portion of the buffer for reading stencil data
	depthStencilDesc.StencilWriteMask = 0xFF;	// a portion of the buffer for writing stencil data

	// Set operations if pixel is front facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// not change values in the buffer if stencil testing is failed
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;	// increment values in the buffer if stencil testing is failed
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	// not change values in the buffer if stencil testring and depth testing both passed
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// a function to compare source stencil values agains existing (destination) stencil values

	// Set operations if pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// not change values in the buffer if stencil testing is failed
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;	// descement values in the buffer if stencil testing is failed
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	// not change values in the buffer if stencil testring and depth testing both passed
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// a function to compare source stencil values agains existing (destination) stencil values

	// Create a depth stencil state
	hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create a depth stencil state");
		return false;
	}

	// Set the depth stencil state
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);	// the second parameter which = 1 means that we have only one stencil state


	// Initialize the depth stencil view description
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// Set up the depth stencil view descritpion
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create a depth stencil view
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create a depth stencil view");
		return false;
	}

	// Bind together the render target view and the depth stencil view to the render pipeline
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


	// ---------------------------------------------------------------------- //
	//                   CREATE A RASTERIZER STATE                            //
	// ---------------------------------------------------------------------- //
	// Initialize the raster description
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	
	// Setup the raster description which will define how and what polygons will be drawn
	rasterDesc.AntialiasedLineEnable = false;	// not use line anti-aliasing	(this param is used if MultisampleEnable = false)
	rasterDesc.CullMode = D3D11_CULL_BACK;		// identify special facing of triangles which won't be drawn
	rasterDesc.DepthBias = 0;					// a depth bias which is added to pixel's depth
	rasterDesc.DepthBiasClamp = 0.0f;			// a maximum depth bias of pixel
	rasterDesc.DepthClipEnable = true;			// enable clipping which is based on distance
	rasterDesc.FillMode = D3D11_FILL_SOLID;		// a filling mode which is used during rendering
	rasterDesc.FrontCounterClockwise = false;	// polygon is front facing if its vertices are clockwise and back facing if its vertices are counter-clockwise
	rasterDesc.MultisampleEnable = false;		// use alpha line anti-aliasing algorithm
	rasterDesc.ScissorEnable = false;			// enable pixels catching which are around of a scissor quadrilateral
	rasterDesc.SlopeScaledDepthBias = 0;		// a scalar of current pixel's slope
	
	// Create the rasterizer state
	hr = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the rasterizer state");
		return false;
	}

	// Set the rasterizer state
	m_pDeviceContext->RSSetState(m_pRasterState);



	// ---------------------------------------------------------------------- //
	//                      CREATE THE VIEWPORT                               //
	// ---------------------------------------------------------------------- //

	// Initialize the viewport params
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	// Set the viewport
	m_pDeviceContext->RSSetViewports(1, &viewport);


	// -------------------------------------------------------------------- //
	//                CREATE MATRICES                                       //
	// -------------------------------------------------------------------- //

	// Create the projection matrix
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, (float)D3DX_PI / 4.0f, 
								(float)screenWidth / (float)screenHeight,
								screenNear, screenDepth);

	// Create the world matrix to the identity matrix
	D3DXMatrixIdentity(&m_worldMatrix);

	// Create the orhographic projection to 2D rendering
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight,
						screenNear, screenDepth);

	Log::Get()->Debug("D3DClass::Initialize(): the Direct3D is successfully initialized");
	return true;
}


void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release 
	// the swap chain it will throw an exception
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(FALSE, nullptr);
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
// this function initialized buffers so they are ready to be drawn to
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	D3DXCOLOR clearColor = { red, green, blue, alpha };

	// initialize the render target view with the particular colour
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);

	// initialize the depth stencil view
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

// tells the swap chain to present our 3D scene after all the rendering into it
void D3DClass::EndScene(void)
{
	// present the back buffer
	if (m_vsync_enabled)
	{
		m_pSwapChain->Present(1, 0);	// lock to screen refresh rate 
	}
	else
	{
		m_pSwapChain->Present(0, 0);	// present as fast as possible
	}
}

// These next functions just give us pointers to the device and device context respectively
ID3D11Device* D3DClass::GetDevice(void)
{
	return m_pDevice;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext(void)
{
	return m_pDeviceContext;
}


// The next three helper functions give us references to
// the world matrix, projection matrix and the orthographic matrix respectively
void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

// return by reference the amount of the video card memory
// and the pointer to the video card description
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strncpy(cardName, m_videoCardDescription, 128);
	memory = m_videoCardMemory;
	return;
}


*/