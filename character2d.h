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
#include "bitmapclass.h"
#include "inputlistener.h"
#include "window.h"

//////////////////////////////////
// Class name: Character2D
//////////////////////////////////
class Character2D : public InputListener
{
public:
	Character2D(void);
	Character2D(const Character2D &obj);
	~Character2D(void);

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext* deviceContext);

	// input handlers
	bool KeyPressed(const KeyButtonEvent& arg);
	bool KeyReleased(const KeyButtonEvent& arg);
	
	// setters
	void SetCharacterPos(int x, int y);

	// getters
	int GetCharacterWidth(void);
	int GetCharacterHeight(void);
	int GetIndexCount(void);
	ID3D11ShaderResourceView* GetTexture(void);
	//void SetCharacterSize(int width, int height);

private:
	void HandleMovingForTexture(void);

private:
	enum MOVE_DIRECTION
	{
		UP, RIGHT, DOWN, LEFT
	};

	BitmapClass* m_pBitmap;

	int m_moveDirection;
	int m_curPosX, m_curPosY;
	int m_width, m_height;
	int m_screenWidth, m_screenHeight;

	float m_textureTopX, m_textureTopY;
	float m_textureBottomX, m_textureBottomY;

	int curActiveKey;
};