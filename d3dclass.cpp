///////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterState = nullptr;
}

D3DClass::D3DClass(const D3DClass& another)
{
}

D3DClass::~D3DClass(void)
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd,
							bool fullScreen, float screenDepth, float screenNear)
{
	HRESULT hr;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	UINT createDeviceFlag = 0;

#ifdef _DEBUG
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Store the vsync setting
	m_vsync_enabled = vsync;


	// --------------------------------------------------------------------- //
	//           GET THE REFRESH RATE FROM THE VIDEO CARD/MONITOR            //
	// --------------------------------------------------------------------- //

	// Create a DirectX graphics interface factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize() :can't create a DXGI Factory");
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card)
	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create an adapter for the primary graphics interface");
		return false;
	}

	// Enumerate the primary adapter output (such as monitor)
	hr = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hr))
	{
		Log::Get()->Error("can't enumerate the primary adapter output");
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format 
	// for the adapter output (monitor)
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
											DXGI_ENUM_MODES_INTERLACED, 
											&numModes, nullptr);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't get the number of modes");
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination
	displayModeList = new(std::nothrow) DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create a list to hold all the possible display modes");
		return false;
	}

	// Now fill the display mode list structures
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
											DXGI_ENUM_MODES_INTERLACED,
											&numModes,
											displayModeList);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't fill in the display mode list structures");
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height
	// When a match is found store the numerator and denominator of the refresh rate for 
	// that monitor
	for (int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == static_cast<unsigned int>(screenWidth) &&
			displayModeList[i].Height == static_cast<unsigned int>(screenHeight))
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
			//break;
		}
	}

	// Get the adapter (video card) description: 
	// (name of the video card and amount of memory on the video card)
	hr = adapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't get the adapter (video card) description");
		return false;
	}

	// Store the dedicated video card memory in megabytes
	m_videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128 - 1);
	if (error != 0)
	{
		Log::Get()->Error("D3DClass::Initialize(): can't convert the name of the video card to a character array");
		return false;
	}

	// Now we can release structures and interfaces used to get the information 
	_DELETE(displayModeList);		// Release the display mode list
	_RELEASE(adapterOutput);
	_RELEASE(adapter);
	_RELEASE(factory);



	// Now we have the refresh rate from the system we can start the DirectX initialization

	// ---------------------------------------------------------------------- //
	//                 INITIALIZE THE SWAP CHAIN DESCRIPTION                  //
	// ---------------------------------------------------------------------- //
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;					// Set to a single back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;	// Set the width and height of the back buffer
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// Set regular 32-bit surface to the back buffer

	// Set the refresh rate of the back buffer
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}


	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// Set the usage of the back buffer
	swapChainDesc.OutputWindow = hwnd;	// Set the handle for the window to render to
	swapChainDesc.SampleDesc.Count = 1;	// Turn multisampling off
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = (fullScreen) ? false : true;	// Set to full screen or windowed mode
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// Discard the back buffer contents after presenting
	swapChainDesc.Flags = 0;	// Don't set the advanced flags

	featureLevel = D3D_FEATURE_LEVEL_11_0;	// tell DirectX what version we plan to use

	
	// ---------------------------------------------------------------------- //
	//   CREATE THE SWAP CHAIN, DIRECT3D DEVICE, AND DIRECT3D DEVICE CONTEXT  //
	// ---------------------------------------------------------------------- //
	hr = D3D11CreateDeviceAndSwapChain(nullptr,
										D3D_DRIVER_TYPE_HARDWARE,			
										NULL, createDeviceFlag,
										&featureLevel, 1,
										D3D11_SDK_VERSION,
										&swapChainDesc,
										&m_swapChain,
										&m_device,
										nullptr,
										&m_deviceContext);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the swap chain, device and device context");
		return false;
	}


	// ---------------------------------------------------------------------- //
	//                  CREATION OF THE RENDER TARGET VIEW                    //
	// ---------------------------------------------------------------------- //

	// Get the pointer to the back buffer
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't get the buffer from the swap chain");
		return false;
	}

	// Create the render target view with the back buffer pointer
	hr = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the render target view");
		return false;
	}

	// Release pointer to the back buffer as we no longer need it
	_RELEASE(backBufferPtr);

	

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
	hr = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the depth stencil buffer");
		return false;
	}

	// Initialize the description of the stencil state
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

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
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the depth stencil state");
		return false;
	}

	// set the depth stencil state 
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


	// Initialize the depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer, 
											&depthStencilViewDesc, 
											&m_depthStencilView);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the depth stencil view");
		return false;
	}

	// Bind the render target view and depth stencil view to the output render pipeline
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


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
	hr = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(hr))
	{
		Log::Get()->Error("D3DClass::Initialize(): can't create the rasterizer state");
		return false;
	}

	// Now set the rasterizer state
	m_deviceContext->RSSetState(m_rasterState);


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
	m_deviceContext->RSSetViewports(1, &viewport);


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

	return true;
}


void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release 
	// the swap chain it will throw an exception
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	_RELEASE(m_rasterState);
	_RELEASE(m_depthStencilView);
	_RELEASE(m_depthStencilState);
	_RELEASE(m_depthStencilBuffer);
	_RELEASE(m_renderTargetView);
	_RELEASE(m_deviceContext);
	_RELEASE(m_device);
	_RELEASE(m_swapChain);

	return;
}

// at the beginning of each frame 
// this function initializes the buffers so they are blank and ready to be draw to
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	FLOAT color[4];
	D3DXCOLOR clearColor = (red, green, blue, alpha);

	// Setup the color to clear the buffer to
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);

	// Clear the depth buffer
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

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
		m_swapChain->Present(1, 0);
	}
	else 
	{
		// Present as fast as possible
		m_swapChain->Present(0, 0);
	}

	return;
}

// These next functions simple get pointer to the Direct3D device and device context
ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
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
