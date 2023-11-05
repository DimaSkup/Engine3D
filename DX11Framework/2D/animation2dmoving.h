////////////////////////////////////////////////////////////////////
// Filename:     animation2dmoving.h
// Description:  this class is responsible for the change 
//               of a character texture according to move direction, etc.
// Revising:     31.06.22
////////////////////////////////////////////////////////////////////
#pragma once

#include <DirectXMath.h>

//////////////////////////////////
// Class name: Animation2DMoving
//////////////////////////////////
class Animation2DMoving
{
public:
	// must be the same as the one in the Character2D class
	enum MOVE_DIRECTION
	{
		RIGHT, DOWN, LEFT, UP
	};

	Animation2DMoving(float animSpeed);
	Animation2DMoving(const Animation2DMoving& copy);
	~Animation2DMoving(void);

	// returns a structure with 4 float values
	// of texture coordinates in order: topLeftX, topLeftY, bottomLeftX, bottomLeftY
	DirectX::XMFLOAT4 GetTextureCoordinates(void);

	// according to move direction (up, right, down or left) we modify texture coordinates
	void CalculateTextureCoords(int moveDirection);

private:
	float m_texTopLeftX, m_texTopLeftY;         // upper left coordinates of the texture
	float m_texBottomRightX, m_texBottomRightY; // bottom right coordinates of the texture
	
	float m_animationSpeed;
};
