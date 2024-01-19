////////////////////////////////////////////////////////////////////
// Filename:      EditorCamera.h
// Description:   is needed to calculate and maintain  the position of 
//                the ENGINE'S EDITOR camera. Handles different 
//                movement changes. The movement can also include
//                acceleration and deceleration to create a smooth
//                camera effect.
//
// Revising:      25.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <cmath>
#include <DirectXMath.h>

#include "../Engine/Log.h"
//#include "../Input/inputclass.h"
#include "../Keyboard/KeyboardEvent.h"
#include "../Mouse/MouseEvent.h"
#include "../Mouse/MouseClass.h"
#include "../Input/inputcodes.h"

#include "cameraclass.h"
#include <vector>

//////////////////////////////////
// Class name: EditorCamera
//////////////////////////////////
class EditorCamera : public CameraClass
{
public:
	EditorCamera(const float cameraSpeed, const float cameraSensitivity);
	~EditorCamera();

	void SetFrameTime(const float time);
	void HandleKeyboardEvents(const KeyboardEvent& kbe); // handles the camera changes accodring to the input from the keyboard
	void HandleMouseEvents(const MouseEvent& me);  // handles the changing of the camera rotation


	// memory allocation (we need it because we use DirectX::XM-objects)
	//void* operator new(size_t i);
	//void operator delete(void* p);

private:
	void HandlePosition();     // handles the changing of the camera position


private:
	float frameTime_ = 0.0f;   // the time passed since the last frame

	// moving state bytes which are set by the keyboard input events
	bool isForward_ = false;
	bool isBackward_ = false;
	bool isRight_ = false;
	bool isLeft_ = false;
	bool isUp_ = false;
	bool isDown_ = false;
};
