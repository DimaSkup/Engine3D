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

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd,
							bool fullScreen, float screenDepth, float screenNear)
{
	HRESULT hr;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, numerator, denominator, stringLength;
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
	float fieldOfView, screenAspect;

	// Store the vsync setting
	m_vsync_enabled = vsync;



	// ---- GET THE REFRESH RATE FROM THE VIDEO CARD/MONITOR ---- //
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

	// Enumerate the primary adapter output (window)
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
											&numModes, NULL);
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
											nullptr);
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
		}
	}

	// Get the adapter (video card) description

}

