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
	EditorCamera();
	~EditorCamera();

	void HandleKeyboardEvents(const KeyboardEvent& kbe, const float deltaTime); // handles the camera changes accodring to the input from the keyboard
	void HandleMouseMovement(const int mouseX_delta, const int mouseY_delta, const float deltaTime);        // handles the changing of the camera rotation

private:
	void HandlePositionChange(const float deltaTime);     // handles the changing of the camera position
};
