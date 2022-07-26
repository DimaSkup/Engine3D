#include "character2d.h"
#include <iostream>


Character2D::Character2D(void)
{
	m_pBitmap = nullptr;
	m_moveDirection = MOVE_DIRECTION::DOWN;
	curActiveKey = 0;
}

Character2D::Character2D(const Character2D &obj)
{
}

Character2D::~Character2D(void)
{
}



// --------------------------------------------------------------------------------- //
//                              PUBLIC FUNCTIONS                                     // 
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
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the BitmapClass object");
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
	

	Window::Get()->GetInputManager()->AddInputListener(this);

	return true;
}

void Character2D::Shutdown(void)
{
	_SHUTDOWN(m_pBitmap);

	Log::Get()->Debug(THIS_FUNC_EMPTY);
}

bool Character2D::Render(ID3D11DeviceContext* deviceContext)
{
	bool result = false;

	/*
	if (m_moveDirection == 4)
		m_moveDirection = 0;
	else
		m_moveDirection++;
	*/
	HandleMovingForTexture();

	// try to render this 2D object
	result = m_pBitmap->Render(deviceContext, 
		                       m_curPosX, m_curPosY, 
		                       m_textureTopX, m_textureTopY, 
		                       m_textureBottomX, m_textureBottomY);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the 2D object");
		return false;
	}

	return true;
}

void Character2D::HandleMovingForTexture(void)
{
	static float frameNum = 0.0f;
	int up = 3, left = 2, down = 1, right = 0;
	if (frameNum >= 4.0f)
		frameNum = 0.0f;

	if (curActiveKey)
	{
		switch (m_moveDirection)
		{
		case MOVE_DIRECTION::UP:
			m_textureTopX = (0.0f * up) + (0.25f * (int)frameNum);
			m_textureTopY = 0.25f * up;
			m_textureBottomX = m_textureTopX + 0.25f;
			m_textureBottomY = m_textureTopY + 0.25f;
			frameNum += 0.01f;
			break;

		case MOVE_DIRECTION::RIGHT:
			m_textureTopX = (0.0f * right) + (0.25f * (int)frameNum);
			m_textureTopY = 0.25f * right;
			m_textureBottomX = m_textureTopX + 0.25f;
			m_textureBottomY = m_textureTopY + 0.25f;
			frameNum += 0.01f;

			break;

		case MOVE_DIRECTION::DOWN:
			m_textureTopX = (0.0f * down) + (0.25f * (int)frameNum);
			m_textureTopY = 0.25f * down;
			m_textureBottomX = m_textureTopX + 0.25f;
			m_textureBottomY = m_textureTopY + 0.25f;
			frameNum += 0.01f;
			break;

		case MOVE_DIRECTION::LEFT:
			m_textureTopX = (0.75f) + (0.25f * (int)frameNum);
			m_textureTopY = 0.25f * left;
			m_textureBottomX = m_textureTopX + 0.25f;
			m_textureBottomY = m_textureTopY + 0.25f;
			frameNum += 0.01f;

			break;
		}
	}

}


bool Character2D::KeyPressed(const KeyButtonEvent& arg)
{
	int stepSize = 5;

	if (arg.code == KEY_UP)
	{
		if (m_curPosY > 0)
		{
			m_curPosY -= stepSize;
		}

		m_moveDirection = MOVE_DIRECTION::UP;
		curActiveKey = 1;
	}
	else if (arg.code == KEY_RIGHT)
	{
		if ((m_curPosX + this->GetCharacterWidth()) < m_screenWidth)
		{
			m_curPosX += stepSize;
		}

		m_moveDirection = MOVE_DIRECTION::RIGHT;
		curActiveKey = 1;
	}
	else if (arg.code == KEY_DOWN)
	{
		if ((m_curPosY + this->GetCharacterHeight()) < m_screenHeight)
		{
			m_curPosY += stepSize;
		}

		m_moveDirection = MOVE_DIRECTION::DOWN;
		curActiveKey = 1;
	}
	else if (arg.code == KEY_LEFT)
	{
		if (m_curPosX > 0)
		{
			m_curPosX -= stepSize;
		}

		m_moveDirection = MOVE_DIRECTION::LEFT;
		curActiveKey = 1;
	}
	
	if (false)
		printf("%s() (%d): %s %d:%d\n", __FUNCTION__, __LINE__, "cur gamer pos at: ", m_curPosX, m_curPosY);
	

	return false;
}


bool Character2D::KeyReleased(const KeyButtonEvent& arg)
{
	curActiveKey = 0;
	return false;
}












void Character2D::SetCharacterPos(int nx, int ny)
{
	m_curPosX = nx;
	m_curPosY = ny;
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