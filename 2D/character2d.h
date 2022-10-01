////////////////////////////////////////////////////////////////////
// Filename:     character2d.h
// Description:  this is a class for 2D character (control, rendering, etc.)
//
// 
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Input/inputclass.h"
#include "../Window/window.h"

#include "bitmapclass.h"
#include "animation2dmoving.h"

//////////////////////////////////
// Class name: Character2D
//////////////////////////////////
class Character2D
{
public:
	enum MOVE_DIRECTION
	{
		RIGHT, DOWN, LEFT, UP
	};

	Character2D(void);
	Character2D(const Character2D &obj);
	~Character2D(void);

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext* deviceContext);

	// setters
	void SetCharacterPos(int x, int y);

	// getters
	int GetCharacterWidth(void);
	int GetCharacterHeight(void);
	int GetIndexCount(void);
	ID3D11ShaderResourceView* GetTexture(void);

private:
	void HandleMovingForAnimation(void);

private:
	BitmapClass* m_pBitmap;
	InputClass* m_pInput;
	Animation2DMoving* m_p2DAnimMoving;

	//int m_moveDirection;       // current direction of the player
	float m_curPosX, m_curPosY;  // current position of the player
	int m_screenWidth, m_screenHeight;

	float m_textureTopX, m_textureTopY;
	float m_textureBottomX, m_textureBottomY;

	int curActiveKey;
};