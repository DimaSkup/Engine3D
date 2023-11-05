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
#include <d3dx11.h>
#include <dxgi.h>	// a DirectX graphic interface header
#include <vector>

#include "Log.h"

class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter = nullptr;
	DXGI_ADAPTER_DESC description;
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
private:
	static std::vector<AdapterData> adapters;
};