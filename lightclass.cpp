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
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}

D3DXVECTOR4 LightClass::GetAmbientColor(void)
{
	return m_ambientColor;
}

D3DXVECTOR4 LightClass::GetDiffuseColor(void)
{
	return m_diffuseColor;
}

D3DXVECTOR3 LightClass::GetDirection(void)
{
	return m_direction;
}

D3DXVECTOR4 LightClass::GetSpecularColor(void)
{
	return m_specularColor;
}

float LightClass::GetSpecularPower(void)
{
	return m_specularPower;
}