#include "character2d.h"
#include <iostream>


Character2D::Character2D(void)
{
	m_pBitmap = nullptr;
	m_pInput = nullptr;

	curActiveKey = 0;
}

Character2D::Character2D(const Character2D &obj)
{
}

Character2D::~Character2D(void)
{
}



// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PUBLIC FUNCTIONS                                     // 
//                                                                                   //
// --------------------------------------------------------------------------------- //


bool Character2D::Initialize(ID3D11Device* device, 
	                         int screenWidth, int screenHeight, 
	                         WCHAR* textureFilename, 
	                         int bitmapWidth, int bitmapHeight)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);


	bool result = false;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ------------------------------ BITMAP -------------------------------------- //
	// create the bitmap object
	m_pBitmap = new BitmapClass();
	if (!m_pBitmap)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the bitmap class object");
		return false;
	}

	// initialize the bitmap object
	result = m_pBitmap->Initialize(device, screenWidth, screenHeight,
			                        textureFilename, bitmapWidth, bitmapHeight);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the BitmapClass object");
		return false;
	}

	// ------------------------------ INPUT ----------------------------------------- //
	m_pInput = new InputClass();
	if (!m_pInput)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the input class object");
		return false;
	}	

	Window::Get()->GetInputManager()->AddInputListener(m_pInput);


	// ------------------------------- 2D ANIMATION ---------------------------------//

	// create the 2D animation class object and set the character animation speed
	float animationSpeed = 0.01f;
	m_p2DAnimMoving = new(std::nothrow) Animation2DMoving(animationSpeed);
	if (!m_p2DAnimMoving)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the animation 2D moving object");
		return false;
	}

	return true;
}

void Character2D::Shutdown(void)
{
	_SHUTDOWN(m_pBitmap);
	_DELETE(m_pInput);

	Log::Get()->Debug(THIS_FUNC_EMPTY);
}



bool Character2D::Render(ID3D11DeviceContext* deviceContext)
{
	bool result = false;


	HandleMovingForAnimation();   // modify texture coordinates according to input to make animation
	DirectX::XMFLOAT4 texCoords = m_p2DAnimMoving->GetTextureCoordinates();

	// render this 2D object
	result = m_pBitmap->Render(deviceContext, 
		                       static_cast<int>(m_curPosX), static_cast<int>(m_curPosY), 
		                       texCoords.x, texCoords.y,  // upper left texture coordinates
		                       texCoords.z, texCoords.w); // bottom right texture coordinates
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the 2D object");
		return false;
	}

	return true;
}


// set the character position on the screen
void Character2D::SetCharacterPos(int nx, int ny)
{
	m_curPosX = static_cast<float>(nx);
	m_curPosY = static_cast<float>(ny);
}

int Character2D::GetCharacterWidth(void)
{
	return m_pBitmap->GetBitmapWidth();
}

int Character2D::GetCharacterHeight(void)
{
	return m_pBitmap->GetBitmapHeight();
}

int Character2D::GetIndexCount(void)
{
	return m_pBitmap->GetIndexCount();
}

ID3D11ShaderResourceView* Character2D::GetTexture(void)
{
	return m_pBitmap->GetTexture();
}



// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PRIVATE FUNCTIONS                                    // 
//                                                                                   //
// --------------------------------------------------------------------------------- //

// handles changing of the player position and its current
// texture coordinates in the moving animation
void Character2D::HandleMovingForAnimation(void)
{
	float stepSize = 0.3f;   // the moving speed of the player on the screen



	// according to the pressed arrow button we modify texture coordinates
	switch (m_pInput->GetActiveKeyCode())
	{
	case KEY_UP:               // if we move in the upper direction
		if (m_curPosY > 0)
		{
			m_curPosY -= stepSize;
		}
		m_p2DAnimMoving->CalculateTextureCoords(MOVE_DIRECTION::UP);
		break;

	case KEY_RIGHT:
		if ((m_curPosX + this->GetCharacterWidth()) < m_screenWidth)
		{
			m_curPosX += stepSize;
		}
		m_p2DAnimMoving->CalculateTextureCoords(MOVE_DIRECTION::RIGHT);
		break;

	case KEY_DOWN:
		if ((m_curPosY + this->GetCharacterHeight()) < m_screenHeight)
		{
			m_curPosY += stepSize;
		}
		m_p2DAnimMoving->CalculateTextureCoords(MOVE_DIRECTION::DOWN);
		break;

	case KEY_LEFT:
		if (m_curPosX > 0)
		{
			m_curPosX -= stepSize;
		}
		m_p2DAnimMoving->CalculateTextureCoords(MOVE_DIRECTION::LEFT);

		break;
	}

	if (true)
		printf("%s() (%d): %s %d:%d\n", __FUNCTION__, __LINE__, "cur gamer pos at: ", static_cast<int>(m_curPosX), static_cast<int>(m_curPosY));
} // HandleMovingForAnimation()
