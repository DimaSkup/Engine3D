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
void ZoneClass::HandleMovementInput(const KeyboardEvent& kbe, const MouseEvent& me, float deltaTime)
{
	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	pCamera_->SetFrameTime(deltaTime);



	// after the frame time update the position movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	pCamera_->HandleKeyboardEvents(kbe);
	pCamera_->HandleMouseEvents(me);

}



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
