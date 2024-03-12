////////////////////////////////////////////////////////////////////
// Filename:      AdapterReader.cpp
// Description:
// Revising:      22.10.22
////////////////////////////////////////////////////////////////////
#include "AdapterReader.h"

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
		Log::Error(LOG_MACRO, "can't create the DXGI Factory");
		exit(-1);
	}

	// go through all the available graphics adapters
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
	{
		adaptersData_.push_back(AdapterData(pAdapter));
		index++;
	}

	return adaptersData_;

	/*
	// DXGI variables, etc
	IDXGIFactory* pFactory = nullptr;	// a pointer to the DirectX graphics interface
	IDXGIAdapter* pAdapter = nullptr;	// a pointer to the adapter (video card) interface
	IDXGIOutput*  pOutput = nullptr;		// a pointer to interface of the display output adapter 
	DXGI_ADAPTER_DESC adapterDesc;		// contains description of the adapter (video card)
	DXGI_MODE_DESC* displayModeList = nullptr;	// a pointer to the list of display adapter modes
	UINT numModes = 0;							// a number of dispay modes
	UINT numerator = 0, denominator = 0;		// numerator and denominator of the display refresh rate
	UINT error = 0;								// info about errors of conterting of WCHAR line into simple char line
	size_t stringLength = 0;


	// Create a DXGI Factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(hr))
	{
		Log::Error(LOG_MACRO, "can't create the DXGI Factory");
		exit(-1);
	}

	// Enumerate adapters (video cards)
	hr = pFactory->EnumAdapters(0, &pAdapter);
	if (FAILED(hr))
	{
		Log::Error(LOG_MACRO, "can't enumerate adapters (video cards)");
		exit(-1);
	}

	// Enumerate ouput adapters (display adapters)
	hr = pAdapter->EnumOutputs(0, &pOutput);
	COM_ERROR_IF_FAILED(hr, "can't enumerate ouput adapters (display adapters)");



	// Get the number of display output modes which fit to the DXGI_FORMAT_R8G8B8A8_UNORM format
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;

	hr = pOutput->GetDisplayModeList(format, flags, &numModes, nullptr);
	if (FAILED(hr))
	{
		Log::Error(LOG_MACRO, "can't get the number of display modes");
	}

	// allocate the memory for the display modes description list
	displayModeList = new DXGI_MODE_DESC[numModes];

	// initialize the display mode list with modes which fit to the DXGI_FORMAT_R8G8B8A8_UNORM format
	hr = pOutput->GetDisplayModeList(format, flags, &numModes, displayModeList);
	if (FAILED(hr))
	{
		Log::Get()->Error(LOG_MACRO, "can't initialize the display mode list");
	}

	// look for a mode which has the necessary screen resolution and get its refresh rate 
	for (size_t i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == static_cast<UINT>(this->width_) &&
			displayModeList[i].Height == static_cast<UINT>(this->height_))
		{
			this->numerator_ = displayModeList[i].RefreshRate.Numerator;
			this->denominator_ = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// get description of the adapter (video card)
	hr = pAdapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
	{
		Log::Get()->Error(LOG_MACRO, "can't get description of the adapter (video card)");
		exit(-1);
	}

	// get the video card memory amount in megabytes
	videoCardMemory_ = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// get the video card name
	error = wcstombs_s(&stringLength, videoCardDescription_, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		Log::Get()->Error(LOG_MACRO, "can't convert the video card description from WCHAR type into char line");
		exit(-1);
	}


	// clear the memory from DXGI variables
	_DELETE(displayModeList);
	_RELEASE(pOutput);
	_RELEASE(pAdapter);
	_RELEASE(pFactory);

	// check the data
	Log::Get()->Debug("video card memory      = %d MB", videoCardMemory_);
	Log::Get()->Debug("video card name        = %s", videoCardDescription_);
	Log::Get()->Debug("video card refreshRate = %d:%d", numerator, denominator);
	*/
}

void AdapterReader::Shutdown()
{
	// go through each element of the array of adapters data
	// and release the adapters interface pointers
	for (AdapterData & data : adaptersData_)
		_RELEASE(data.pAdapter);

	adaptersData_.clear();
}


// stores a pointer to IDXGI adapter and its description
AdapterData::AdapterData(IDXGIAdapter* pAdapter)
{
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&this->description);

	if (FAILED(hr))
	{
		Log::Error(LOG_MACRO, "failed to get description for IDXGIAdapter");
	}
}