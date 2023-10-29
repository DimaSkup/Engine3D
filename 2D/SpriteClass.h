////////////////////////////////////////////////////////////////////
// Filename:     SpriteClass.h
// Description:  will be used to represent an animated 2D image
//               that needs to be rendered to the screen. For every 
//               2D animated image you should have a separate SpriteClass object;
//
//               It also includes a frame timer assist smoothly cycling 
//               through the textures that get mapped to the 2D square.
//
// Created:      14.08.23
////////////////////////////////////////////////////////////////////
#pragma once



//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Plane.h"


//////////////////////////////////
// Class name: SpriteClass
//////////////////////////////////
class SpriteClass : public Plane
{
private:
	struct SPRITE_RECT
	{
		float left = 0.0f;
		float right = 0.0f;
		float top = 0.0f;
		float bottom = 0.0f;
	};

public:
	SpriteClass(ModelInitializerInterface* pModelInitializer);
	~SpriteClass();

	virtual bool Initialize(ID3D11Device* pDevice) override;

	// ATTENTION: call this function after the Initialize() function
	bool SetupSprite(const POINT & renderAtPos,
		UINT screenWidth, UINT screenHeight,
		const std::string & spriteInfoDataFile);

	virtual void Render(ID3D11DeviceContext* pDeviceContext,
		D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;
	
	// each frame we update the sprites data
	void Update(float frameTime);


	
	// sprite's SETTERS / LOADERS
	void SetRenderLocation(int posX, int posY);
	void SetScreenDimensions(UINT width, UINT height);
	void SetSpriteDimensions(UINT width, UINT height);
	bool LoadTextures(const std::string & spriteInfoDataFile);  // initialization textures for this sprite;

	// GETTERS
	ID3D11ShaderResourceView* const* GetTexture();   // returns the current texture for the sprite from the texture array
	UINT SpriteClass::GetSpriteWidth() const _NOEXCEPT;
	UINT SpriteClass::GetSpriteHeight() const _NOEXCEPT;

private:
	void UpdateBuffers(ID3D11DeviceContext*);


private:
	UINT screenWidth_ = 0;
	UINT screenHeight_ = 0;
	UINT bitmapWidth_ = 0;
	UINT bitmapHeight_ = 0;
	UINT renderX_ = 0;
	UINT renderY_ = 0;

	// initialize the previous rendering position to negative one
	UINT prevPosX_ = -1;
	UINT prevPosY_ = -1;

	UINT currentTexture_ = 0;
	UINT textureCount_ = 0;     // the number of all the textures for this sprite (if it is an animated sprite)

	float frameTime_ = 0.0f;    // the frame time will be used to control the sprite cycling speed
	float cycleTime_ = 0.0f;    


};
