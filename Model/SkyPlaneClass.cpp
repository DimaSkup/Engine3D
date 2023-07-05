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
	try
	{
		pModel_ = new ModelClass();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the SkyPlaneClass elements");
	}
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
bool SkyPlaneClass::Initialize(ID3D11Device* pDevice)
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
	translationSpeed_[0] = 0.0003f;    // first texture X translation speed increments
	translationSpeed_[1] = 0.0f;       // first texture Z translation speed increments
	translationSpeed_[2] = 0.00015f;   // second texture X translation speed increments 
	translationSpeed_[3] = 0.0f;       // second texture Z translation speed increments


	// we also store the current translation for the two textures and provide it to the 
	// pixel shader during rendering
	for (std::size_t i = 0; i < textureTranslation_.size(); i++)
	{
		textureTranslation_[i] = 0.0f;
	}

	// create the sky plane
	result = InitializeSkyPlane(pDevice, skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat);
	COM_ERROR_IF_FALSE(result, "can't create the sky plane");

	/*
	
	// load the sky plane textures
	result = LoadTextures(pDevice, 
		pModel_->GetTextureByIndex(0)->GetName(),
		pModel_->GetTextureByIndex(1)->GetName());
	COM_ERROR_IF_FALSE(result, "can't load the sky plane textures");
	*/


	// setup the id of the model
	SetID(modelType_);

	return true;
}


// set a texture by particular index 
void SkyPlaneClass::SetTextureByIndex(WCHAR* textureFilename, UINT index)
{
	pModel_->SetTexture(textureFilename, index);

	return;
}

/*

// the Render function calls RenderBuffers to put the sky plane geometry on the
// graphics pipeline for drawing
void SkyPlaneClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	pModel_->Render(pDeviceContext);
}

*/


// the frame processing that we do for the sky plane is the cloud texture translation
// which simulates movement of the clouds across the sky. The coordinates are translated
// according to the speed given for that direction. Index x and y is for the X and Z on the
// first cloud. Index z and w is for the X and Z on the second cloud. We also truncate the
// values so they never go over 1.0f. Note that if you unlock the vsync the clouds will go
// at a speed according to the new frame rate, to avoid that you should pass in the frame 
// time and adjust the translation accordingly.
void SkyPlaneClass::Frame()
{
	// increment the translation values to simulate the moving clouds;
	for (std::size_t i = 0; i < textureTranslation_.size(); i++)
	{
		textureTranslation_[0] += translationSpeed_[0];
	}
	
	// keep the values in the zero to one range
	for (std::size_t i = 0; i < textureTranslation_.size(); i++)
	{
		if (textureTranslation_[i] > 1.0f) { textureTranslation_[i] -= 1.0f; }
	}

	return;
}


// returns a pointer to the current brightness value that we 
// want applied to the clouds in the pixel shader
float* SkyPlaneClass::GetPtrToBrightness()
{
	return &brightness_;
}

// returns a pointer to the cloud translation data array
std::vector<float>* SkyPlaneClass::GetPtrToTranslationData()
{
	return &textureTranslation_;
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
bool SkyPlaneClass::InitializeSkyPlane(ID3D11Device* pDevice, 
	int skyPlaneResolution,
	float skyPlaneWidth,
	float skyPlaneTop,
	float skyPlaneBottom,
	int textureRepeat)
{
	int index = 0;               // the index into the sky plane data array to add this coordinate
	float quadSize = 0.0f;       // the side of each quad on the sky plane
	float radius = 0.0f;         // the radius of the sky plane based on the width
	float constant = 0.0f;       // the height constant to increment by
	float textureDelta = 0.0f;   // the texture coordinate to increment by

	//float posX = 0.0f;
	//float posY = 0.0f;
	//float posZ = 0.0f;
	//float tu = 0.0f;
	//float tv = 0.0f;

	VERTEX* pVertices = nullptr;
	UINT* pIndices = nullptr;

	// calculate the number of vertices and indices for this sky plane
	UINT vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1);
	UINT indexCount = vertexCount;

	// allocate memory for vertices and indices data
	pModel_->AllocateVerticesAndIndicesArrays(vertexCount, indexCount);

	// get a pointer to the vertices and indices array to write into the directly
	pVertices = *(pModel_->GetAddressOfVerticesData());
	pIndices = *(pModel_->GetAddressOfIndicesData());


	// determine the size of each quad on the sky plane
	quadSize = skyPlaneWidth / static_cast<float>(skyPlaneResolution);

	// calculate the radius of the sky plane based on the width
	radius = skyPlaneWidth / 2.0f;

	// calculate the height constant to increment by
	constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	// calculate the texture coordinate to increment by
	textureDelta = static_cast<float>(textureRepeat) / static_cast<float>(skyPlaneResolution);

	// loop through the sky plane and build the coordinates based 
	// on the increment value given
	for (int j = 0; j <= skyPlaneResolution; j++)
	{
		for (int i = 0; i <= skyPlaneResolution; i++)
		{
			// calculate the index into the sky plane data array to add this coordinate
			index = j * (skyPlaneResolution + 1) + i;

			// calculate the vertex coordinates (pay attention to the order of coordinates: X Z Y)
			pVertices[index].position.x = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			pVertices[index].position.z = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			pVertices[index].position.y = skyPlaneTop - (constant * 
				((pVertices[index].position.x * pVertices[index].position.x) +
				 (pVertices[index].position.z * pVertices[index].position.z)));

			// calculate the texture coordinates
			pVertices[index].texture.x = (float)i * textureDelta;
			pVertices[index].texture.y = (float)j * textureDelta;

			// write index data into the indices array
			pIndices[index] = index;
		}
	}


	// initialize the vertex and index buffers with the model data
	bool result = pModel_->InitializeBuffers(pDevice, pVertices, pIndices, vertexCount, indexCount);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex/index buffer");

	// release the arrays now that the vertex and index buffers have been created and loaded
	pModel_->ClearModelData();

	return true;
}




// The LoadTextures loads the two cloud textures that will be used for rendering with
bool SkyPlaneClass::LoadCloudTextures(ID3D11Device* pDevice,
	WCHAR* textureFilename1, 
	WCHAR* textureFilename2)
{
	bool result = false;

	// add the first cloud texture object
	result = pModel_->AddTexture(textureFilename1);
	COM_ERROR_IF_FALSE(result, "can't add the 1st cloud texture");

	// add the second cloud texture object
	result = pModel_->AddTexture(textureFilename2);
	COM_ERROR_IF_FALSE(result, "can't add the 2nd cloud texture");


	return true;
}


