////////////////////////////////////////////////////////////////////
// Filename:    bitmapclass.cpp
// Revising:    27.05.22
////////////////////////////////////////////////////////////////////
#include "bitmapclass.h"


// ---------------------------------------------------------------------------------- //
//                    CREATION OF THE VERTEX AND PIXEL SHADERS                        //
// ---------------------------------------------------------------------------------- //


BitmapClass::BitmapClass(void)
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTexture = nullptr;
}

BitmapClass::BitmapClass(const BitmapClass& obj)
{
}

BitmapClass::~BitmapClass(void)
{
}


/////////////////////////////////////////////////////////////////////
//
//                   PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////

// 
bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight,
	                         WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result = false;

	// in this function both the screen size and image size are stored. These will be
	// required for generating exact vertex locations during rendering.

	// store the screen size
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// store the size in pixels that this bitmap should be rendered at
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// the previous rendering location is first initialized to negative one. These variables
	// are responsible for location where 2D model was drawn
	m_previousPosX = -1;
	m_previousPosY = -1;


	// initialize the vertex and index buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize buffers for a 2D model");
		return false;
	}

	// load the texture for this model
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't load the texture for a 2D model");
		return false;
	}
	
	return true;
}




/////////////////////////////////////////////////////////////////////
//
//                   PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////