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
#include "../Engine/Settings.h"

//////////////////////////////////
// Class name: EditorCamera
//////////////////////////////////
class EditorCamera : public CameraClass
{
public:
	EditorCamera();
	EditorCamera(const EditorCamera& copy) {};   // we don't use the copy constructor and destructor in this class
	~EditorCamera() {};

	void SetFrameTime(float time);
	void HandleKeyboardEvents(const KeyboardEvent& kbe); // handles the camera changes accodring to the input from the keyboard
	void HandleMouseEvents(const MouseEvent& me);  // handles the changing of the camera rotation


	// memory allocation (we need it because we use DirectX::XM-objects)
	//void* operator new(size_t i);
	//void operator delete(void* p);



private:
	void HandlePosition(const BYTE* keyboardState);  // handles the changing of the camera position
private:
	DirectX::XMFLOAT3 moveCommand_;

	float movingSpeed_;              // a camera movement speed
	float rotationSpeed_;            // a camera turning speed
	float frameTime_;                // the time passed since the last frame

	// moving state bits which are set by the keyboard input events
	//bool isNowMovement_;           // are we moving now?
	bool isForward_ = false;
	bool isBackward_ = false;
	bool isRight_ = false;
	bool isLeft_ = false;
	bool isUp_ = false;
	bool isDown_ = false;

	bool isRotateUp_ = false;
	bool isRotateDown_ = false;
	bool isRotateLeft_ = false;
	bool isRotateRight_ = false;


};
