////////////////////////////////////////////////////////////////////
// Filename:     SpriteClass.h
// Description:  will be used to represent an individual 2D image
//               that needs to be rendered to the screen. For every 
//               2D image you have you will need a new BitmapClass object;
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

	bool Initialize(ID3D11Device* pDevice, 
		int screenWidth, int screenHeight,   // screen params
		int renderX, int renderY);           // render at this position

	virtual void Render(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;
	bool LoadTextures(char* spriteInfoDataFile);

	void Update(float frameTime);
	void SetRenderLocation(int posX, int posY);
	

private:
	void UpdateBuffers(ID3D11DeviceContext*);



private:
	//Plane* pPlane_ = nullptr;

	UINT screenWidth_ = 0;
	UINT screenHeight_ = 0;
	UINT bitmapWidth_ = 0;
	UINT bitmapHeight_ = 0;
	UINT renderX_ = 0;
	UINT renderY_ = 0;

	// initialize the previous rendering position to negative one
	UINT prevPosX_ = -1;
	UINT prevPosY_ = -1;


	float frameTime_ = 0.0f;    // the frame time will be used to control the sprite cycling speed
	float cycleTime_ = 0.0f;    

	UINT currentTexture_ = 0;
	UINT textureCount_ = 0;
};
