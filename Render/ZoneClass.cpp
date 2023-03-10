////////////////////////////////////////////////////////////////////
// Filename::    ZoneClass.cpp
// Description:  impementation of the ZoneClass functional;
//
// Created:      10.03.23
////////////////////////////////////////////////////////////////////
#include "ZoneClass.h"

ZoneClass::ZoneClass()
{
	pCamera_ = new EditorCamera();
}

ZoneClass::ZoneClass(const ZoneClass& copy)
{
}

ZoneClass::~ZoneClass()
{
	_DELETE(pCamera_);
}


////////////////////////////////////////////////////////////////////
//
//                        PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

bool ZoneClass::Initialize(SETTINGS::settingsParams* settingsList)
{
	Log::Print("---------------- INITIALIZATION: THE CAMERA -----------------");
	Log::Debug(THIS_FUNC_EMPTY);

	float windowWidth = static_cast<float>(settingsList->WINDOW_WIDTH);
	float windowHeight = static_cast<float>(settingsList->WINDOW_HEIGHT);
	float aspectRatio = windowWidth / windowHeight;


	// set up the EditorCamera object
	pCamera_->SetPosition({ 0.0f, 0.0f, -3.0f });
	pCamera_->SetProjectionValues(settingsList->FOV_DEGREES, aspectRatio, settingsList->NEAR_Z, settingsList->FAR_Z);

	return true;
}




void ZoneClass::HandleMovementInput(const KeyboardEvent& kbe, float deltaTime)
{
	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pCamera_->SetFrameTime(deltaTime);

	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pCamera_->HandleKeyboardEvents(kbe);
}


void ZoneClass::HandleMovementInput(const MouseEvent& me, float deltaTime)
{
	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pCamera_->SetFrameTime(deltaTime);



	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pCamera_->HandleMouseEvents(me);

}



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
