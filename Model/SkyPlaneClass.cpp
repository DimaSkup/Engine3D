////////////////////////////////////////////////////////////////////
// Filename:      SkyPlaneClass.cpp
// Description:   encapsulates everything related to the plane used
//                for rendering the clouds. It holds the geometry for
//                the sky plane, the two bitmaps textures for the 
//                clouds, and all the variables for the shader that
//                relate to how to draw the sky plane.
// 
// Created:       25.06.23
////////////////////////////////////////////////////////////////////
#include "SkyPlaneClass.h"

SkyPlaneClass::SkyPlaneClass()
{
}

SkyPlaneClass::~SkyPlaneClass()
{
}



////////////////////////////////////////////////////////////////////
//
//                       PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// here we do all the setup for the sky plane. It takes as input the
// two cloud texture file names as well as the Direct3D device
bool SkyPlaneClass::Initialize(ID3D11Device* pDevice, WCHAR* textureFilename1, WCHAR* textureFilename2)
{
	// the sky plane parameters
	int textureRepeat = 4;        // determines how many times to repeat the texture over the sky plane. This is used to generate the UV coordinates
	int skyPlaneResolution = 10;  // is used for specifying how many quads that sky plane should be composed of in the X and Z direction, increasing this value makes it higher poly and smoother
	float skyPlaneWidth = 10.0f;  // the length of the plane
	float skyPlaneTop = 0.5f;     // the height of the curved sky plane
	float skyPlaneBottom = 0.0f;  // the base of the curved sky plane. The bottom four corners of the plane will be at skyPlaneBottom and the center of the plane will be at skyPlaneTop. All other points are interpolated between those two values 
	bool result = false;

	// settings the brightness is important for making clouds look realistic when using 
	// bitmaps that range just 0 to 255. The brightness value lowers how white clouds are
	// which allows you to give them more of a faded look just like real clouds have. 
	// The value range here from 0.0f to 1.0f. For instance if we set it to 0.65f so that
	// the clouds are at 65% brightness
	brightness_ = 0.65f;


	// the translation speed is how fast we translate the cloud textures over the sky plane.
	// Each cloud can be translated on both the X and Z axis. There are two textures so we
	// store the dual speed for both in a 4 float array
	translationSpeed_.x = 0.0003f;    // first texture X translation speed increments
	translationSpeed_.y = 0.0f;       // first texture Z translation speed increments
	translationSpeed_.z = 0.00015f;   // second texture X translation speed increments 
	translationSpeed_.w = 0.0f;       // second texture Z translation speed increments


	// we also store the current translation for the two texture and provide it to the 
	// pixel shader during rendering
	textureTranslation_ = { 0.0f, 0.0f, 0.0f, 0.0f };


	// create the sky plane
	result = InitializeSkyPlane(skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat);
	COM_ERROR_IF_FALSE(result, "can't create the sky plane");

	// create the vertex and index buffer for the sky plane
	result = InitializeBuffers(pDevice, skyPlaneResolution);
	COM_ERROR_IF_FALSE(result, "can't initialize buffers");

	// load the sky plane textures
	result = LoadTextures(pDevice, textureFilename1, textureFilename2);
	COM_ERROR_IF_FALSE(result, "can't load the sky plane textures");

	return true;
}


// the Render function calls RenderBuffers to put the sky plane geometry on the
// graphics pipeline for drawing
void SkyPlaneClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	// render the sky plane
	RenderBuffers(pDeviceContext);
}


// the frame processing that we do for the sky plane is the cloud texture translation
// which simulates movement of the clouds across the sky. The coordinates are translated
// according to the speed given for that direction. Index x and y is for the X and Z on the
// first cloud. Index z and w is for the X and Z on the second cloud. We also truncate the
// values so they never go over 1.0f. Note that if you unlock the vsync the clouds will go
// at a speed according to the new frame rate, to avoid that you should pass in the frame 
// time and adjust the translation accordingly.
void SkyPlaneClass::Frame()
{
	// increment the translation values to simulate the moving clouds
	textureTranslation_.x += translationSpeed_.x;   // 1st cloud X
	textureTranslation_.y += translationSpeed_.y;   // 1st cloud Z
	textureTranslation_.z += translationSpeed_.z;   // 2nd cloud X
	textureTranslation_.w += translationSpeed_.w;   // 2nd cloud Z

	// keep the values in the zero to one range
	if (textureTranslation_.x > 1.0f) { textureTranslation_.x -= 1.0f; }
	if (textureTranslation_.y > 1.0f) { textureTranslation_.y -= 1.0f; }
	if (textureTranslation_.z > 1.0f) { textureTranslation_.z -= 1.0f; }
	if (textureTranslation_.w > 1.0f) { textureTranslation_.w -= 1.0f; }

	return;
}


// returns the current brightness value that we want applied to the clouds in the pixel shader
float SkyPlaneClass::GetBrightness() const
{
	return brightness_;
}

// returns the texture translation value for the given index
float SkyPlaneClass::GetTranslation(UINT index) const
{
	return textureTranslation_[index];
}


////////////////////////////////////////////////////////////////////
//
//                       PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// InitializeSkyPlane is where we build the geometry for the sky plane. We first create an
// array to hold the geometry and then we setup the increment values needed to build the 
// sky plane in the for loop. Then we run the for loop and create the position and texture
// coordinates for each vertex based on the increment values. This process builds the curved
// plane that we will use to render the clouds onto.
bool SkyPlaneClass::InitializeSkyPlane(int skyPlaneResolution,
	float skyPlaneWidth,
	float skyPlaneTop,
	float skyPlaneBottom,
	int textureRepeat)
{
	int index = 0.0f;
	float quadSize = 0.0f;
	float radius = 0.0f;
	float constant = 0.0f;
	float textureDelta = 0.0f;

	float posX = 0.0f;
	float posY = 0.0f;
	float posZ = 0.0f;
	float tu = 0.0f;
	float tv = 0.0f;

	return true;
}