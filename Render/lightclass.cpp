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
	m_ambientColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = DirectX::XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}

DirectX::XMFLOAT4 LightClass::GetAmbientColor(void)
{
	return m_ambientColor;
}

DirectX::XMFLOAT4 LightClass::GetDiffuseColor(void)
{
	return m_diffuseColor;
}

DirectX::XMFLOAT3 LightClass::GetDirection(void)
{
	return m_direction;
}

DirectX::XMFLOAT4 LightClass::GetSpecularColor(void)
{
	return m_specularColor;
}

float LightClass::GetSpecularPower(void)
{
	return m_specularPower;
}