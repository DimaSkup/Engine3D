////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////
#include "lightclass.h"

LightClass::LightClass(void)
{
}

LightClass::LightClass(const LightClass& another)
{
}

LightClass::~LightClass(void)
{
}

// ---------------------------------------------------------------------------------- //
//
//                              PUBLIC METHODS
//
// ---------------------------------------------------------------------------------- //
void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	ambientColor_ = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor_ = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	direction_ = DirectX::XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	specularColor_ = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecularPower(float power)
{
	specularPower_ = power;
	return;
}




const DirectX::XMFLOAT4 & LightClass::GetAmbientColor(void) const
{
	return ambientColor_;
}

const DirectX::XMFLOAT4 & LightClass::GetDiffuseColor(void) const
{
	return diffuseColor_;
}

const DirectX::XMFLOAT3 & LightClass::GetDirection(void) const
{
	return direction_;
}

const DirectX::XMFLOAT4 & LightClass::GetSpecularColor(void) const
{
	return specularColor_;
}

float LightClass::GetSpecularPower(void) const
{
	return specularPower_;
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