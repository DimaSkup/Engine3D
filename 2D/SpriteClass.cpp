////////////////////////////////////////////////////////////////////
// Filename:     SpriteClass.cpp
// Description:  will be used to represent an individual 2D image
//               that needs to be rendered to the screen. For every 
//               2D image you have you will need a new BitmapClass object;
//               It also includes a frame timer assist smoothly cycling 
//               through the textures that get mapped to the 2D square.
//
// Created:      14.08.23
////////////////////////////////////////////////////////////////////
#include "SpriteClass.h"


SpriteClass::SpriteClass(ModelInitializerInterface* pModelInitializer)
{
	try
	{
		this->SetModelInitializer(pModelInitializer);
		this->AllocateMemoryForElements();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the SpriteClass members");
	}
	
}

SpriteClass::~SpriteClass()
{
	//std::string debugMsg{ "destroyment of the " + this->GetModelDataObj()->GetID() };
	//Log::Debug(THIS_FUNC, debugMsg.c_str());
}




/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////

// initialization of the sprite
bool SpriteClass::Initialize(ID3D11Device* pDevice,
	int screenWidth, int screenHeight,   // screen params
	int renderX, int renderY)            // render at this position
{
	// check input params
	assert(screenWidth > 0);
	assert(screenHeight > 0);

	// try to initialize a new 2D plane
	try
	{
		bool result = this->InitializeCopyOf(Plane::pDefaultPlane_, pDevice, "sprite");
		COM_ERROR_IF_FALSE(result, "can't initialize a sprite");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't initialize the sprite");
	}

	// store the screen size
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// store where the sprite should be rendered to
	renderX_ = renderX;
	renderY_ = renderY;

	

	return true;
}



// This function takes in the frame time each frame. This will usually be around 
// 16-17 ms if you are running your program at 60fps. Each frame we add this time to
// the frameTime_ counter. If it reaches or passes the cycle time that was defined for
// this sprite, then we change the sprite to use the next texture in the array. We then
// reset the timer to start from zero again.
void SpriteClass::Update(float frameTime)
{
	// increment the grame time each frame
	frameTime += frameTime;

	// check if the frame time has reached the cycle time
	if (frameTime_ >= cycleTime_)
	{
		// if it has then reset the frame time and cycle to the next sprite in the texture array
		frameTime_ -= cycleTime_;

		currentTexture_++;

		// if we are at the last sprite texture then go back to the beginning of the texture
		// array to the first texture again
		if (currentTexture_ == textureCount_)
		{
			currentTexture_ = 0;
		}
	}

	return;
}
