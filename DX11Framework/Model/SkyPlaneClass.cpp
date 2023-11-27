////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      SkyPlaneClass.cpp
// Description:   encapsulates everything related to the plane used
//                for rendering the clouds. It holds the geometry for
//                the sky plane, the two bitmaps textures for the 
//                clouds, and all the variables for the shader that
//                relate to how to draw the sky plane.
// 
// Created:       25.06.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "SkyPlaneClass.h"

SkyPlaneClass::SkyPlaneClass(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->SetModelInitializer(pModelInitializer);
	this->modelType_ = "sky_plane";
}

SkyPlaneClass::~SkyPlaneClass()
{
}



////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

// here we do all the setup for the sky plane. It takes as input the
// two cloud texture file names as well as the Direct3D device
bool SkyPlaneClass::Initialize(const std::string & filePath)
{
	// the sky plane parameters
	int textureRepeat = 4;        // determines how many times to repeat the texture over the sky plane. This is used to generate the UV coordinates
	int skyPlaneResolution = 10;  // is used for specifying how many quads that sky plane should be composed of in the X and Z direction, increasing this value makes it higher poly and smoother
	float skyPlaneWidth = 10.0f;  // the length of the plane
	float skyPlaneTop = 0.5f;     // the height of the curved sky plane
	float skyPlaneBottom = 0.0f;  // the base of the curved sky plane. The bottom four corners of the plane will be at skyPlaneBottom and the center of the plane will be at skyPlaneTop. All other points are interpolated between those two values 
	bool result = false;

	// arrays for vertices/indices data of the sky plane
	std::vector<VERTEX> verticesArr;
	std::vector<UINT> indicesArr;

	// settings the brightness is important for making clouds look realistic when using 
	// bitmaps that range just 0 to 255. The brightness value lowers how white clouds are
	// which allows you to give them more of a faded look just like real clouds have. 
	// The value range here from 0.0f to 1.0f. For instance if we set it to 0.65f so that
	// the clouds are at 65% brightness
	brightness_ = 0.65f;


	// the translation speed is how fast we translate the cloud textures over the sky plane.
	// Each cloud can be translated on both the X and Z axis. There are two textures so we
	// store the dual speed for both in a 4 float array
	translationSpeed_[0] = 0.00003f;    // first texture X translation speed increments
	translationSpeed_[1] = 0.0f;       // first texture Z translation speed increments
	translationSpeed_[2] = 0.000015f;   // second texture X translation speed increments 
	translationSpeed_[3] = 0.0f;       // second texture Z translation speed increments


	// we also store the current translation for the two textures and provide it to the 
	// pixel shader during rendering
	for (std::size_t i = 0; i < 4; i++)
	{
		textureTranslation_[i] = 0.0f;
	}

	// create the sky plane's geometry
	result = this->BuildSkyPlaneGeometry(this->pDevice_,
		skyPlaneResolution, 
		skyPlaneWidth, 
		skyPlaneTop, 
		skyPlaneBottom,
		textureRepeat);
	COM_ERROR_IF_FALSE(result, "can't generate data for a sky plane model");

	// load the vertex and index array with the sky plane array data
	result = this->FillSkyPlaneArrays(this->pDevice_,
		skyPlaneResolution,
		verticesArr, 
		indicesArr);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex/index buffer");

	// each sky plane has only one mesh so create it initialize with vertices/indices data
	this->InitializeOneMesh(verticesArr, indicesArr);

	// release the sky plane raw data array now that the vertex and index buffers
	// have been created and loaded; also release the vertices/indices arrays which had
	// final data of vertices/indices
	_DELETE_ARR(pSkyPlaneRawData_);
	verticesArr.clear();
	indicesArr.clear();

	return true;
}

///////////////////////////////////////////////////////////

void SkyPlaneClass::SetTextureByIndex(WCHAR* textureFilename, UINT index)
{
	// set a texture by particular index 
	this->GetTextureArrayObj()->SetTexture(textureFilename, index);

	return;
}

///////////////////////////////////////////////////////////

void SkyPlaneClass::Frame(float deltaTime)
{
	// the frame processing that we do for the sky plane is the cloud texture translation
	// which simulates movement of the clouds across the sky. The coordinates are translated
	// according to the speed given for that direction. Index x and y is for the X and Z on the
	// first cloud. Index z and w is for the X and Z on the second cloud. We also truncate the
	// values so they never go over 1.0f. Note that if you unlock the vsync the clouds will go
	// at a speed according to the new frame rate, to avoid that you should pass in the frame 
	// time and adjust the translation accordingly.


	// increment the translation values to simulate the moving clouds;
	for (std::size_t i = 0; i < 4; i++)
	{
		textureTranslation_[i] += (translationSpeed_[i] * deltaTime / 2);
	}
	
	// keep the values in the zero to one range
	for (std::size_t i = 0; i < 4; i++)
	{
		if (textureTranslation_[i] > 1.0f) { textureTranslation_[i] -= 1.0f; }
	}

	return;
}

///////////////////////////////////////////////////////////


const float SkyPlaneClass::GetBrightness() const _NOEXCEPT
{
	// returns a value of the current brightness value that we 
	// want applied to the clouds in the pixel shader
	return brightness_;
}

///////////////////////////////////////////////////////////

float* SkyPlaneClass::GetTranslationData() _NOEXCEPT
{
	// returns a pointer to the cloud translation data array
	return textureTranslation_;
}









////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

bool SkyPlaneClass::BuildSkyPlaneGeometry(ID3D11Device* pDevice,
	const int skyPlaneResolution,
	const float skyPlaneWidth,
	const float skyPlaneTop,
	const float skyPlaneBottom,
	const int textureRepeat)
{
	// InitializeSkyPlane is where we build the geometry for the sky plane. We first create an
	// array to hold the geometry and then we setup the increment values needed to build the 
	// sky plane in the for loop. Then we run the for loop and create the position and texture
	// coordinates for each vertex based on the increment values. This process builds the curved
	// plane that we will use to render the clouds onto.


	int index = 0;               // the index into the sky plane data array to add this coordinate
	float quadSize = 0.0f;       // the side of each quad on the sky plane
	float radius = 0.0f;         // the radius of the sky plane based on the width
	float constant = 0.0f;       // the height constant to increment by
	float textureDelta = 0.0f;   // the texture coordinate to increment by

	try
	{
		// create the array to hold the sky plane coordinates
		pSkyPlaneRawData_ = new VERTEX[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the sky plane data array");
	}


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
	for (UINT j = 0; j <= static_cast<UINT>(skyPlaneResolution); j++)
	{
		for (UINT i = 0; i <= static_cast<UINT>(skyPlaneResolution); i++)
		{
			// calculate the index into the sky plane data array to add this coordinate
			index = j * (skyPlaneResolution + 1) + i;

			// calculate the vertex coordinates (PAY ATTENTION TO THE ORDER of coordinates: X Z Y)
			pSkyPlaneRawData_[index].position.x = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			pSkyPlaneRawData_[index].position.z = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
			pSkyPlaneRawData_[index].position.y = skyPlaneTop - (constant *
				((pSkyPlaneRawData_[index].position.x * pSkyPlaneRawData_[index].position.x) +
				 (pSkyPlaneRawData_[index].position.z * pSkyPlaneRawData_[index].position.z)));

			// calculate the texture coordinates
			pSkyPlaneRawData_[index].texture.x = (float)i * textureDelta;
			pSkyPlaneRawData_[index].texture.y = (float)j * textureDelta;
		}
	}


	

	return true;
}

///////////////////////////////////////////////////////////

bool SkyPlaneClass::FillSkyPlaneArrays(ID3D11Device* pDevice,
	const int skyPlaneResolution,
	std::vector<VERTEX> & verticesArr,
	std::vector<UINT> & indicesArr)
{
	UINT index = 0;                // the index into the vertex array
	UINT index1 = 0;
	UINT index2 = 0;
	UINT index3 = 0;
	UINT index4 = 0;

	// calculate the number of vertices in the sky plane mesh
	UINT vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;
	UINT indexCount = vertexCount;

	// resize the vertices/indices arrays
	verticesArr.resize(vertexCount);
	indicesArr.resize(indexCount);

	/////////////////////////////////////////////

	// load the vertex and index array with the sky plane array data
	for (UINT j = 0; j < (UINT)skyPlaneResolution; j++)
	{
		for (UINT i = 0; i < (UINT)skyPlaneResolution; i++)
		{
			index1 = j * (skyPlaneResolution + 1) + i;
			index2 = j * (skyPlaneResolution + 1) + (i + 1);
			index3 = (j + 1) * (skyPlaneResolution + 1) + i;
			index4 = (j + 1) * (skyPlaneResolution + 1) + (i + 1);

			// TRIANGLE 1: upper left
			verticesArr[index].position = pSkyPlaneRawData_[index1].position;
			verticesArr[index].texture = pSkyPlaneRawData_[index1].texture;
			indicesArr[index] = index;
			index++;

			// TRIANGLE 1: upper right
			verticesArr[index].position = pSkyPlaneRawData_[index2].position;
			verticesArr[index].texture = pSkyPlaneRawData_[index2].texture;
			indicesArr[index] = index;
			index++;

			// TRIANGLE 1: bottom left
			verticesArr[index].position = pSkyPlaneRawData_[index3].position;
			verticesArr[index].texture = pSkyPlaneRawData_[index3].texture;
			indicesArr[index] = index;
			index++;

			/////////////////////////////////////////////

			// TRIANGLE 2: bottom left
			verticesArr[index].position = pSkyPlaneRawData_[index3].position;
			verticesArr[index].texture = pSkyPlaneRawData_[index3].texture;
			indicesArr[index] = index;
			index++;

			// TRIANGLE 2: upper right
			verticesArr[index].position = pSkyPlaneRawData_[index2].position;
			verticesArr[index].texture = pSkyPlaneRawData_[index2].texture;
			indicesArr[index] = index;
			index++;

			// TRIANGLE 2: bottom right
			verticesArr[index].position = pSkyPlaneRawData_[index4].position;
			verticesArr[index].texture = pSkyPlaneRawData_[index4].texture;
			indicesArr[index] = index;
			index++;
		}
	}

	return true;

} // end FillSkyPlaneArrays

///////////////////////////////////////////////////////////

bool SkyPlaneClass::LoadCloudTextures(ID3D11Device* pDevice,
	WCHAR* textureFilename1, 
	WCHAR* textureFilename2)
{
	// The LoadTextures loads the two cloud textures that will be used for rendering with

	assert((textureFilename1 != nullptr) && (textureFilename1 != L'\0'));
	assert((textureFilename2 != nullptr) && (textureFilename2 != L'\0'));

	// try to add cloud textures to the sky plane model
	try
	{
		this->GetTextureArrayObj()->AddTexture(textureFilename1);
		this->GetTextureArrayObj()->AddTexture(textureFilename2);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		return false;
	}

	return true;
}


