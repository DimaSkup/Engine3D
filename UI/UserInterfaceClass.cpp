/////////////////////////////////////////////////////////////////////
// Filename:     UserInterfaceclass.cpp
// Description:  a functional for initialization, 
//               updating and rendering of the UI
// 
// Created:      25.05.23
/////////////////////////////////////////////////////////////////////
#include "UserInterfaceClass.h"


UserInterfaceClass::UserInterfaceClass()
{
	try
	{
		pFont1_ = new FontClass;                  // create the first font object
		pFpsString_ = new TextClass;              // create the fps text string
		pVideoStrings_ = new TextClass[2];        // create the text objects for the video strings
		pPositionStrings_ = new TextClass[6];     // create the text objects for the position strings
		pRenderCountStrings_ = new TextClass[3];  // create the text objects for the render count strings
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the UI elements");
	}
}

UserInterfaceClass::~UserInterfaceClass()
{
	_DELETE(pFont1_);
	_DELETE(pFpsString_);
	_DELETE_ARR(pVideoStrings_);
	_DELETE_ARR(pPositionStrings_);
	_DELETE_ARR(pRenderCountStrings_);
}




/////////////////////////////////////////////////////////////////////
//
//                        PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////

bool UserInterfaceClass::Initialize(D3DClass* pD3D, 
	const SETTINGS::settingsParams* systemParams,
	const DirectX::XMMATRIX & baseViewMatrix)
{
	bool result = false;

	// initialize the first font object
	result = pFont1_->Initialize(pD3D->GetDevice(), "data/ui/font01.txt", L"data/ui/font01.dds");
	COM_ERROR_IF_FALSE(result, "can't initialize the first font object");

	// initialize the fps text string
	result = pFpsString_->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(), 
									 systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
									 16, pFont1_, "Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize the fps text string");

	// initialize the video text strings
	result = this->InitializeVideoStrings(pD3D, systemParams);
	COM_ERROR_IF_FALSE(result, "can't initialize the video text strings");

	// initialize the position text strings
	result = this->InitializePositionStrings(pD3D, systemParams);
	COM_ERROR_IF_FALSE(result, "can't initialize the position text strings");
	
	// initialize the base view matrix
	baseViewMatrix_ = baseViewMatrix;

	return true;
}


bool UserInterfaceClass::Frame(ID3D11DeviceContext* pDeviceContext, 
	const SETTINGS::settingsParams* pSystemParams, 
	const SystemState* pSystemState,
	const DirectX::XMFLOAT3 & position,
	const DirectX::XMFLOAT3 & rotation)
{
	bool result = false;

	// update the fps string
	result = UpdateFpsString(pDeviceContext, pSystemState->fps);
	COM_ERROR_IF_FALSE(result, "can't update the fps string");

	// update the position strings
	result = UpdatePositionStrings(pDeviceContext, position, rotation);

	return true;
}


bool UserInterfaceClass::Render(D3DClass* pD3D, const XMMATRIX & worldMatrix, const XMMATRIX & orthoMatrix)
{
	// turn off the Z buffer and enable alpha blending to begin 2D rendering
	pD3D->TurnZBufferOff();
	pD3D->TurnOnAlphaBlending();

	// render the fps string
	pFpsString_->Render(pD3D->GetDeviceContext(), worldMatrix, baseViewMatrix_, orthoMatrix);

	// render the video card strings
	pVideoStrings_[0].Render(pD3D->GetDeviceContext(), worldMatrix, baseViewMatrix_, orthoMatrix);
	pVideoStrings_[1].Render(pD3D->GetDeviceContext(), worldMatrix, baseViewMatrix_, orthoMatrix);

	// render the position and rotation strings
	for (size_t i = 0; i < 6; i++)
	{
		pPositionStrings_[i].Render(pD3D->GetDeviceContext(), worldMatrix, baseViewMatrix_, orthoMatrix);
	}
	
	pD3D->TurnOffAlphaBlending();  // turn off alpha blending now that the text has been rendered
	pD3D->TurnZBufferOn();         // turn the Z buffer back on now that the 2D rendering has completed

	return true;
}






/////////////////////////////////////////////////////////////////////
//
//                        PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////


// initialize the video text strings with initial data
bool UserInterfaceClass::InitializeVideoStrings(D3DClass* pD3D, const SETTINGS::settingsParams* systemParams)
{
	bool result = false;
	char videoCardName[128] = { '\0' };
	char videoString[144] = { '\0' };
	char memoryString[32] = { '\0' };
	char tempString[16] = { '\0' };
	int videoCardMemory = 0;

	// setup the video card strings
	pD3D->GetVideoCardInfo(videoCardName, videoCardMemory);
	strcpy_s(videoString, "Video Card: ");
	strcat_s(videoString, videoCardName);

	_itoa_s(videoCardMemory, tempString, 10);

	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, "MB");

	// initialize the video text strings
	result = pVideoStrings_[0].Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
		systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
		256, pFont1_, videoString, 10, 10, 1.0f, 1.0f, 1.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize a string with a video card name");

	result = pVideoStrings_[1].Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
		systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
		32, pFont1_, memoryString, 10, 30, 1.0f, 1.0f, 1.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize a string with a video card memory");


	return true;
}  // InitializeVideoStrings()

// initialize the position text string with initial data
bool UserInterfaceClass::InitializePositionStrings(D3DClass* pD3D, const SETTINGS::settingsParams* systemParams)
{
	bool result = false;
	int stringLength = 16;

	// init X position string
	result = pPositionStrings_[0].Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
		systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
		stringLength, pFont1_, "X: 0", 10, 310, 1.0f, 1.0f, 1.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize the text string with X position data");

	// init Y position string
	result = pPositionStrings_[1].Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
		systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
		stringLength, pFont1_, "Y: 0", 10, 330, 1.0f, 1.0f, 1.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize the text string with Y position data");

	// init Z position string
	result = pPositionStrings_[2].Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
		systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
		stringLength, pFont1_, "Z: 0", 10, 350, 1.0f, 1.0f, 1.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize the text string with Z position data");



	// init X rotation string
	result = pPositionStrings_[3].Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
		systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
		stringLength, pFont1_, "rX: 0", 10, 370, 1.0f, 1.0f, 1.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize the text string with X position data");

	// init Y rotation string
	result = pPositionStrings_[4].Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
		systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
		stringLength, pFont1_, "rY: 0", 10, 390, 1.0f, 1.0f, 1.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize the text string with Y position data");

	// init Z rotation string
	result = pPositionStrings_[5].Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(),
		systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
		stringLength, pFont1_, "rZ: 0", 10, 410, 1.0f, 1.0f, 1.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize the text string with Z position data");

	return true;
} // InitializePositionStrings()


// update the fps value to render it onto the screen
bool UserInterfaceClass::UpdateFpsString(ID3D11DeviceContext* pDeviceContext, int fps)
{
	std::string newFpsValue{ "Fps: " + std::to_string(fps) };

	pFpsString_->Update(pDeviceContext, newFpsValue, { 10.0f, 50.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });

	return true;
}

// update the position string to render it onto the screen
bool UserInterfaceClass::UpdatePositionStrings(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMFLOAT3 & position, 
	const DirectX::XMFLOAT3 & rotation)
{
	char tempString[16] = { '\0' };
	char finalString[16] = { '\0' };
	bool result = false;

	// update the position strings if the value has changed since the last frame
	if (static_cast<int>(position.x) != previousPosition_[0])
	{
		previousPosition_[0] = static_cast<int>(position.x);
		_itoa_s(previousPosition_[0], tempString, 10);
		strcpy_s(finalString, "X: ");
		strcat_s(finalString, tempString);
		result = pPositionStrings_[0].Update(pDeviceContext, finalString, { 10.0f, 100.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
		COM_ERROR_IF_FALSE(result, "can't update the text string with X position data");
	}

	if (static_cast<int>(position.x) != previousPosition_[0])
	{
		previousPosition_[0] = static_cast<int>(position.y);
		_itoa_s(previousPosition_[0], tempString, 10);
		strcpy_s(finalString, "Y: ");
		strcat_s(finalString, tempString);
		result = pPositionStrings_[0].Update(pDeviceContext, finalString, { 10.0f, 120.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
		COM_ERROR_IF_FALSE(result, "can't update the text string with X position data");
	}

	return true;
}