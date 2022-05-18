////////////////////////////////////////////////////////////////////
// Filename:      lightclass.h
// Description:   The purpose of this class is only to maintain
//                the direction and color lights
//
// Revising:      16.04.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"
#include "log.h"

//////////////////////////////////
// Class name: LightClass
//////////////////////////////////
class LightClass
{
public:
	LightClass(void);
	LightClass(const LightClass&);
	~LightClass(void);

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	D3DXVECTOR4 GetAmbientColor(void);
	D3DXVECTOR4 GetDiffuseColor(void);
	D3DXVECTOR3 GetDirection(void);
	D3DXVECTOR4 GetSpecularColor(void);
	float GetSpecularPower(void);

private:
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_specularColor;
	float       m_specularPower;
};
