////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////
#include "lightclass.h"

LightClass::LightClass(void)
{
}

LightClass::~LightClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
}

// ---------------------------------------------------------------------------------- //
//
//                              PUBLIC METHODS
//
// ---------------------------------------------------------------------------------- //
void LightClass::SetAmbientColor(const float red, const float green, const float blue)
{
	ambientColor_ = DirectX::XMFLOAT3(red, green, blue);
	return;
}

void LightClass::SetAmbientColor(const DirectX::XMFLOAT3 & newColor)
{
	ambientColor_ = newColor;
	return;
}

void LightClass::SetDiffuseColor(const float red, const float green, const float blue)
{
	diffuseColor_ = DirectX::XMFLOAT3(red, green, blue);
	return;
}

void LightClass::SetDiffuseColor(const DirectX::XMFLOAT3 & newColor)
{
	diffuseColor_ = newColor;
	return;
}

void LightClass::SetDirection(const float x, const float y, const float z)
{
	direction_ = DirectX::XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(const float red, const float green, const float blue)
{
	specularColor_ = DirectX::XMFLOAT3(red, green, blue);
	return;
}

void LightClass::SetSpecularPower(const float power)
{
	specularPower_ = power;
	return;
}

// set position of the point light source
void LightClass::SetPosition(const float x, const float y, const float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;
	position_.w = 1.0f;

	return;
}

///////////////////////////////////////////////////////////

void LightClass::SetPosition(const DirectX::XMFLOAT3 & newPos)
{
	position_.x = newPos.x;
	position_.y = newPos.y;
	position_.z = newPos.z;
	position_.w = 1.0f;

	return;
}

///////////////////////////////////////////////////////////

void LightClass::AdjustPosition(const float x, const float y, const float z)
{
	position_.x += x;
	position_.y += y;
	position_.z += z;

	return;
}

///////////////////////////////////////////////////////////

void LightClass::AdjustPosition(const DirectX::XMFLOAT3 & adjustment)
{
	position_.x += adjustment.x;
	position_.y += adjustment.y;
	position_.z += adjustment.z;
}


///////////////////////////////////////////////////////////

void LightClass::AdjustPosX(const float x)
{
	position_.x = x;
	return;
}

///////////////////////////////////////////////////////////

void LightClass::AdjustPosY(const float y)
{
	position_.y = y;
	return;
}

///////////////////////////////////////////////////////////

void LightClass::AdjustPosZ(const float z)
{
	position_.z = z;
	return;
}

///////////////////////////////////////////////////////////


const DirectX::XMFLOAT3 & LightClass::GetAmbientColor() const
{
	return ambientColor_;
}

const DirectX::XMFLOAT3 & LightClass::GetDiffuseColor() const
{
	return diffuseColor_;
}

const DirectX::XMFLOAT3 & LightClass::GetDirection() const
{
	return direction_;
}

const DirectX::XMFLOAT3 & LightClass::GetSpecularColor() const
{
	return specularColor_;
}

float LightClass::GetSpecularPower() const
{
	return specularPower_;
}

// get the position of a point light source
const DirectX::XMFLOAT4 & LightClass::GetPosition() const
{
	return position_;
}





// memory allocation
void* LightClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	COM_ERROR_IF_FALSE(ptr, "can't allocate the memory for object");

	return ptr;
}

void LightClass::operator delete(void* p)
{
	_aligned_free(p);
}