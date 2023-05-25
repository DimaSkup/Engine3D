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

bool UserInterfaceClass::Initialize(D3DClass* pD3D, const SETTINGS::settingsParams* systemParams)
{
	bool result = false;
	char videoCard[128] = { '\0' };
	char videoString[144] = { '\0' };
	char memoryString[32] = { '\0' };
	int videoMemory = 0;

	// initialize the first font object
	result = pFont1_->Initialize(pD3D->GetDevice(), "../data/ui/font01.txt", L"../data/ui/font01.tga");
	COM_ERROR_IF_FALSE(result, "can't initialize the first font object");

	// initialize the fps text string
	result = pFpsString_->Initialize(pD3D->GetDevice(), pD3D->GetDeviceContext(), 
									 systemParams->WINDOW_WIDTH, systemParams->WINDOW_HEIGHT,
									 16, pFont1_, "Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);
	COM_ERROR_IF_FALSE(result, "can't initialize the fps text string");


	return true;
}


bool UserInterfaceClass::Frame(ID3D11DeviceContext* pDeviceContext, const SETTINGS::settingsParams* pSystemParams, const SystemState* pSystemState)
{
	bool result = false;

	// update the fps string
	result = UpdateFpsString(pDeviceContext, pSystemState->fps);
	COM_ERROR_IF_FALSE(result, "can't update the fps string");

	return true;
}


bool UserInterfaceClass::Render(D3DClass* pD3D, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix)
{
	return true;
}