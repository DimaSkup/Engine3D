////////////////////////////////////////////////////////////////////
// Filename:      AdapterReader.h
// Description:
// Revising:      21.10.22
////////////////////////////////////////////////////////////////////
#pragma once


/////////////////////////////
// LINKING
/////////////////////////////
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

/////////////////////////////
// INCLUDES
/////////////////////////////
#include <d3d11.h>
//#include <d3dx11.h>
#include <dxgi.h>	// a DirectX graphic interface header
#include <vector>


class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter_ = nullptr;
	DXGI_ADAPTER_DESC description_;
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
	static void Shutdown(); 
private:
	static std::vector<AdapterData> adaptersData_;
};