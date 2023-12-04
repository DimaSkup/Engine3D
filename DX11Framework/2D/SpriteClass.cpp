////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     SpriteClass.cpp
// Description:  will be used to represent an individual 2D image
//               that needs to be rendered to the screen. For every 
//               2D image you have you will need a new BitmapClass object;
//               It also includes a frame timer assist smoothly cycling 
//               through the textures that get mapped to the 2D square.
//
// Created:      14.08.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "SpriteClass.h"

#include <fstream>


SpriteClass::SpriteClass(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	try
	{
		this->SetModelInitializer(pModelInitializer);
		this->modelType_ = "2D_sprite";
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the SpriteClass members");
	}
	
}

SpriteClass::~SpriteClass()
{
	//std::string debugMsg{ "destroyment of the " + this->GetModelDataObj()->GetID() };
	//Log::Debug(THIS_FUNC, debugMsg.c_str());
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
////////////////////////////////////////////////////////////////////////////////////////////


bool SpriteClass::Initialize(const std::string & filePath)
{
	// initialize a 2D sprite 

	try
	{
		// since each 2D sprite is just a plane it has 4 vertices and 6 indices
		UINT vertexCount = 4;
		UINT indexCount = 6;

		// arrays for vertices/indices data
		std::vector<VERTEX> verticesArr(vertexCount);
		std::vector<UINT> indicesArr(indexCount);

		/////////////////////////////////////////////////////

		// setup the vertices positions
		verticesArr[0].position = {  1, -1,  0 };    // bottom right 
		verticesArr[1].position = { -1, -1,  0 };    // bottom left
		verticesArr[2].position = {  1,  1,  0 };    // upper right
		verticesArr[3].position = { -1,  1,  0 };    // upper left

		// setup the texture coords of each vertex
		verticesArr[0].texture = { 1, 1 };     
		verticesArr[1].texture = { 0, 1 };    
		verticesArr[2].texture = { 1, 0 };    
		verticesArr[3].texture = { 0, 0 };

		// setup the indices
		indicesArr.insert(indicesArr.begin(), { 0, 2, 1, 2, 3, 1 });

		/////////////////////////////////////////////////////

		// each 2D sprite has only one mesh so create it and fill in with data
		this->InitializeOneMesh(verticesArr, indicesArr);

	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't initialize a 2D sprite");

		return false;
	}
	
	return true;

} // end Initialize

///////////////////////////////////////////////////////////


void SpriteClass::SetupSprite(const POINT & renderAtPos,
	UINT screenWidth, UINT screenHeight,
	const std::string & spriteInfoDataFile)
{
	// ATTENTION: call this function after the Initialize() function

	// check input params
	COM_ERROR_IF_FALSE(spriteInfoDataFile.empty() == false, "the input name of the sprite's data file is empty");
	COM_ERROR_IF_ZERO(screenWidth, "input screen width is zero");
	COM_ERROR_IF_ZERO(screenHeight, "input screen height is zero");

	/////////////////////////////////////////////////////

	// initialize textures for this sprite
	bool result = LoadTextures(spriteInfoDataFile);
	COM_ERROR_IF_FALSE(result, "can't load textures for a 2D sprite");

	this->SetRenderLocation(renderAtPos.x, renderAtPos.y);
	this->SetScreenDimensions(screenWidth, screenHeight);

	// get the dimensions of the first texture and used that as the dimensions
	// of the 2D sprite images
	//bitmapWidth_ = pTexturesList_->GetTextureByIndex(0)->GetWidth();
	//bitmapHeight_ = pTexturesList_->GetTextureByIndex(0)->GetHeight();

	// set the starting texture in the cylce to be the first one in the list
	currentTexture_ = 0;

} // end SetupSprite

///////////////////////////////////////////////////////////

void SpriteClass::Render(D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// update the buffers if the position of the sprite has changed 
	// from its original position
	UpdateBuffers();

	Model::Render(topologyType);

	return;
}

///////////////////////////////////////////////////////////

void SpriteClass::Update(float frameTime)
{
	// This function takes in the frame time each frame. This will usually be around 
	// 16-17 ms if you are running your program at 60fps. Each frame we add this time to
	// the frameTime_ counter. If it reaches or passes the cycle time that was defined for
	// this sprite, then we change the sprite to use the next texture in the array. We then
	// reset the timer to start from zero again.


	// increment the grame time each frame
	frameTime_ += frameTime;

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

} // end Update




///////////////////////////////////////////////////////////////////////////////////////////
//                            PUBLIC SETTERS/LOADERS
///////////////////////////////////////////////////////////////////////////////////////////

void SpriteClass::SetRenderLocation(int posX, int posY)
{
	// store where the sprite should be rendered to
	renderX_ = posX;
	renderY_ = posY;

	return;
}

///////////////////////////////////////////////////////////

void SpriteClass::SetScreenDimensions(UINT width, UINT height)
{
	// a sprite will use width and height of the screen for the rendering purpose
	screenWidth_ = width;
	screenHeight_ = height;
}

///////////////////////////////////////////////////////////

#if 0
void SetSpriteDimensions(UINT width, UINT height)
{
	
}
#endif

///////////////////////////////////////////////////////////

bool SpriteClass::LoadTextures(const std::string & spriteInfoDataFile)
{
	// initialization of textures for this sprite;
	//
	// here we open the file and read in the number of textures it uses,
	// each dds/tga/etc. file used for each of the textures, and the speed
	// at which it should cycle through the textures


	char textureFilename[128]{ '\0' };   // fill in this array with null-terminated symbols
	std::ifstream fin;
	char input = ' ';
	bool result = false;

	// open the sprite info data file
	fin.open(spriteInfoDataFile);
	if (fin.fail())
	{
		std::string errorMsg{ "can't open the sprite info data file: " + (std::string)spriteInfoDataFile };
		Log::Error(THIS_FUNC, errorMsg.c_str());
		return false;
	}

	// read in the number of textures
	fin >> textureCount_;

	// read to star of the next line
	fin.get(input);

	// read in each texture file name
	for (UINT i = 0; i < textureCount_; i++)
	{
		UINT j = 0;

		// read in a texture file name
		fin.get(input);
		while (input != '\n')
		{
			textureFilename[j] = input;
			++j;
			fin.get(input);
		}

		// get a pointer to const WCHAR which contains texture filename
		std::wstring wstrTextureFilename{ StringHelper::StringToWide((std::string)textureFilename) };
		const WCHAR* wpszTextureFilename = wstrTextureFilename.c_str();

		// once we have the filename then load the texture in the texture array
		//pTexturesList_->AddTexture(wpszTextureFilename);
	}

	// read in the cycle time (sprite animation speed)
	fin >> cycleTime_;

	// close the file
	fin.close();

	return true;
} // end LoadTextures

///////////////////////////////////////////////////////////

ID3D11ShaderResourceView* const* SpriteClass::GetTextureResourceView()
{
	// returns the current texture for the sprite from the texture array
	//return this->pTexturesList_->GetTextureResourcesArray() + (currentTexture_);
	return nullptr;
}

///////////////////////////////////////////////////////////

UINT SpriteClass::GetSpriteWidth() const
{
	return bitmapWidth_;
}

///////////////////////////////////////////////////////////

UINT SpriteClass::GetSpriteHeight() const
{
	return bitmapHeight_;
}





////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PRIVATE FUNCTIONS
// 
////////////////////////////////////////////////////////////////////////////////////////////



void SpriteClass::UpdateBuffers()
{
	// if a sprite's position is changed we have to update its vertex buffer 
	// for rendering this sprite at the new position

	// if the position we are rendering this bitmap to hasn't changed 
	// then don't update the vertex buffer
	if ((prevPosX_ == renderX_) && (prevPosY_ == renderY_))
	{
		return;
	}

	SPRITE_RECT bitmapRect;

	// the rendering location has changed then store the new position and update the vertex buffer
	prevPosX_ = renderX_;
	prevPosY_ = renderY_;

	// an array for new vertices data 
	std::vector<VERTEX> verticesArr(4);

	// calculate the screen coordinates of the left/right/top/bottom side of the bitmap
	bitmapRect.left   = static_cast<float>((screenWidth_ / 2) * -1 + static_cast<float>(renderX_));
	bitmapRect.right  = static_cast<float>(bitmapRect.left + bitmapWidth_);
	bitmapRect.top    = static_cast<float>((screenHeight_ / 2) - static_cast<float>(renderY_));
	bitmapRect.bottom = static_cast<float>(bitmapRect.top - bitmapHeight_);

	/////////////////////////////////////////////////////

	// setup the vertices positions
	verticesArr[0].position = { bitmapRect.right, bitmapRect.bottom, 0.0f };  // bottom right 
	verticesArr[1].position = { bitmapRect.left,  bitmapRect.bottom, 0.0f };  // bottom left
	verticesArr[2].position = { bitmapRect.right, bitmapRect.top,    0.0f };  // top right
	verticesArr[3].position = { bitmapRect.left,  bitmapRect.top,    0.0f };  // top left

	 // setup the texture coords of each vertex
	verticesArr[0].texture = { 1, 1 };
	verticesArr[1].texture = { 0, 1 };
	verticesArr[2].texture = { 1, 0 };
	verticesArr[3].texture = { 0, 0 };

	/////////////////////////////////////////////////////

	// update the DYNAMIC vertex buffer
	this->meshes_[0]->UpdateVertexBuffer(this->pDeviceContext_, verticesArr);

	return;

} // end UpdateBuffers