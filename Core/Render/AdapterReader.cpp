////////////////////////////////////////////////////////////////////
// Filename:      AdapterReader.cpp
// Description:
// Revising:      22.10.22
////////////////////////////////////////////////////////////////////
#include "AdapterReader.h"
#include "../Engine/log.h"


std::vector<AdapterData> AdapterReader::adaptersData_;

// returns a vector of available IDXGI adapters
std::vector<AdapterData> AdapterReader::GetAdapters()
{
	if (adaptersData_.size() > 0) // if it is already initialized
		return adaptersData_;


	// --- in another case we get the adapters data --- //
	HRESULT hr = S_OK;
	IDXGIFactory* pFactory = nullptr;	// a pointer to the DirectX graphics interface
	IDXGIAdapter* pAdapter = nullptr;	// a pointer to the adapter (video card) interface
	UINT index = 0;

	// Create a DXGI Factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(hr))
	{
		Log::Error("can't create the DXGI Factory");
		exit(-1);
	}

	// go through all the available graphics adapters
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
	{
		adaptersData_.push_back(AdapterData(pAdapter));
		index++;
	}

	return adaptersData_;
}

void AdapterReader::Shutdown()
{
	// go through each element of the array of adapters data
	// and release the adapters interface pointers
	for (AdapterData& data : adaptersData_)
	{
		if (data.pAdapter_)
		{ 
			data.pAdapter_->Release();
			data.pAdapter_ = nullptr;
		}
	}

	adaptersData_.clear();
}


// stores a pointer to IDXGI adapter and its description
AdapterData::AdapterData(IDXGIAdapter* pAdapter)
{
	pAdapter_ = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&description_);

	if (FAILED(hr))
	{
		Log::Error("failed to get description for IDXGIAdapter");
	}
}
