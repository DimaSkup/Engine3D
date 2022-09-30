////////////////////////////////////////////////////////////////////
// Filename: animation2dmoving.cpp
// Revising: 31.07.22
////////////////////////////////////////////////////////////////////
#include "animation2dmoving.h"


// Constructor
Animation2DMoving::Animation2DMoving(float animSpeed)
	: m_animationSpeed(animSpeed) {}


// we don't use the copy constructor and destructor in this class
Animation2DMoving::Animation2DMoving(const Animation2DMoving& copy) {}
Animation2DMoving::~Animation2DMoving(void) {}



// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PUBLIC FUNCTIONS                                     // 
//                                                                                   //
// --------------------------------------------------------------------------------- //

// returns a structure with 4 float values
// of texture coordinates in order: topLeftX, topLeftY, bottomLeftX, bottomLeftY
DirectX::XMFLOAT4 Animation2DMoving::GetTextureCoordinates(void)
{
	return DirectX::XMFLOAT4{ m_texTopLeftX, m_texTopLeftY, m_texBottomRightX, m_texBottomRightY };
}

// according to move direction (up, right, down or left) we modify texture coordinates
void Animation2DMoving::CalculateTextureCoords(int moveDirection)
{
	static float frameNum = 0.0f;

	if (frameNum >= 4.0f)  // by default we have only four pictures for particular moving direction
		frameNum = 0.0f;

	m_texTopLeftX = (0.0f * moveDirection) + (0.25f * static_cast<int>(frameNum));
	m_texTopLeftY = 0.25f * moveDirection;
	m_texBottomRightX = m_texTopLeftX + 0.25f;
	m_texBottomRightY = m_texTopLeftY + 0.25f;

	frameNum += 0.01f;
}


// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PRIVATE FUNCTIONS                                    // 
//                                                                                   //
// --------------------------------------------------------------------------------- //