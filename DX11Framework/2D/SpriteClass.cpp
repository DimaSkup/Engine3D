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

#include <fstream>


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


bool SpriteClass::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// allocate memory for the sprite's vertices and indices
	this->GetModelDataObj()->AllocateVerticesAndIndicesArrays(6, 6);

	// each sprite has only one mesh
	meshes_.push_back(Mesh(pDevice,
		pDeviceContext,
		pModelData_->GetVertices(),   // currently we have no vertices data, later we will build vertices in UpdateBuffers() function
		pModelData_->GetIndices()));

	// setup the sprite's ID (later this value can be changed. For example to: "sprite(10)")
	pModelData_->SetID("sprite");
}

///////////////////////////////////////////////////////////


void SpriteClass::SetupSprite(const POINT & renderAtPos,
	UINT screenWidth, UINT screenHeight,
	const std::string & spriteInfoDataFile)
{
	// ATTENTION: call this function after the Initialize() function

	// check input params
	assert(spriteInfoDataFile.empty() != true);

	// initialize textures for this sprite
	bool result = LoadTextures(spriteInfoDataFile);
	COM_ERROR_IF_FALSE(result, "can't load textures for a 2D sprite: " + this->GetModelDataObj()->GetID());

	this->SetRenderLocation(renderAtPos.x, renderAtPos.y);
	this->SetScreenDimensions(screenWidth, screenHeight);

	// get the dimensions of the first texture and used that as the dimensions
	// of the 2D sprite images
	bitmapWidth_ = pTexturesList_->GetTextureByIndex(0)->GetWidth();
	bitmapHeight_ = pTexturesList_->GetTextureByIndex(0)->GetHeight();

	// set the starting texture in the cylce to be the first one in the list
	currentTexture_ = 0;

} // end SetupSprite

///////////////////////////////////////////////////////////

void SpriteClass::Render(ID3D11DeviceContext* pDeviceContext,
	D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// update the buffers if the position of the sprite has changed 
	// from its original position
	UpdateBuffers(pDeviceContext);

	Model::Render(pDeviceContext, topologyType);

	return;
}

///////////////////////////////////////////////////////////


//
// sprite's setters/loaders
//

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


void SetSpriteDimensions(UINT width, UINT height)
{
	
}

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
		std::wstring wstrTextureFilename{ StringConverter::StringToWide((std::string)textureFilename) };
		const WCHAR* wpszTextureFilename = wstrTextureFilename.c_str();

		// once we have the filename then load the texture in the texture array
		pTexturesList_->AddTexture(wpszTextureFilename);
	}

	// read in the cycle time (sprite animation speed)
	fin >> cycleTime_;

	// close the file
	fin.close();

	return true;
} // end LoadTextures

///////////////////////////////////////////////////////////

// returns the current texture for the sprite from the texture array
ID3D11ShaderResourceView* const* SpriteClass::GetTexture()
{
	ID3D11ShaderResourceView* pShaderResource = nullptr;

	return this->pTexturesList_->GetTextureResourcesArray() + (currentTexture_);
}




// This function takes in the frame time each frame. This will usually be around 
// 16-17 ms if you are running your program at 60fps. Each frame we add this time to
// the frameTime_ counter. If it reaches or passes the cycle time that was defined for
// this sprite, then we change the sprite to use the next texture in the array. We then
// reset the timer to start from zero again.
void SpriteClass::Update(float frameTime)
{
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
}




UINT SpriteClass::GetSpriteWidth() const _NOEXCEPT
{
	return bitmapWidth_;
}

UINT SpriteClass::GetSpriteHeight() const _NOEXCEPT
{
	return bitmapHeight_;
}





/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PRIVATE FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


// if a sprite's position is changed we have to update its vertex buffer 
// for rendering this sprite at the new position
void SpriteClass::UpdateBuffers(ID3D11DeviceContext* pDeviceContext)
{
	// if the position we are rendering this bitmap to hasn't changed 
	// then don't update the vertex buffer
	if ((prevPosX_ == renderX_) && (prevPosY_ == renderY_))
	{
		return;
	}

	SPRITE_RECT bitmapRect;

	// if the rendering location has changed then store the new position and update the vertex buffer
	prevPosX_ = renderX_;
	prevPosY_ = renderY_;

	// get a pointer to the vertices array so we can write data directly into it
	std::vector<VERTEX> &pVertices = this->GetModelDataObj()->GetVertices();

	// calculate the screen coordinates of the left/right/top/bottom side of the bitmap
	bitmapRect.left   = static_cast<float>((screenWidth_ / 2) * -1 + static_cast<float>(renderX_));
	bitmapRect.right  = static_cast<float>(bitmapRect.left + bitmapWidth_);
	bitmapRect.top    = static_cast<float>((screenHeight_ / 2) - static_cast<float>(renderY_));
	bitmapRect.bottom = static_cast<float>(bitmapRect.top - bitmapHeight_);

	// load the vertex array with data
	// First triangle
	pVertices[0].position = { bitmapRect.left, bitmapRect.top, 0.0f };  // top left
	pVertices[0].texture = { 0.0f, 0.0f };

	pVertices[1].position = { bitmapRect.right, bitmapRect.bottom, 0.0f };  // bottom right
	pVertices[1].texture = { 1.0f, 1.0f };

	pVertices[2].position = { bitmapRect.left, bitmapRect.bottom, 0.0f };  // bottom left
	pVertices[2].texture = { 0.0f, 1.0f };


	// Second triangle
	pVertices[3].position = pVertices[0].position;   // top left
	pVertices[3].texture = pVertices[0].texture;

	pVertices[4].position = { bitmapRect.right, bitmapRect.top, 0.0f };  // top right
	pVertices[4].texture = { 1.0f, 0.0f };

	pVertices[5].position = pVertices[1].position;  // bottom right
	pVertices[5].texture = pVertices[1].texture;

	// update the DYNAMIC vertex buffer
	this->meshes_[0].UpdateVertexBuffer(pDeviceContext, pVertices.data());

	return;
}