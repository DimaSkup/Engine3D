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
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../LogSystem/Log.h"


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

	DirectX::XMFLOAT4 GetAmbientColor(void);
	DirectX::XMFLOAT4 GetDiffuseColor(void);
	DirectX::XMFLOAT3 GetDirection(void);
	DirectX::XMFLOAT4 GetSpecularColor(void);
	float GetSpecularPower(void);

	// memory allocation
	void* operator new(size_t i)
	{
		void* ptr = _aligned_malloc(i, 16);
		if (!ptr)
		{
			Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
			return nullptr;
		}

		return ptr;
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

private:
	DirectX::XMFLOAT4 m_ambientColor;
	DirectX::XMFLOAT4 m_diffuseColor;
	DirectX::XMFLOAT3 m_direction;
	DirectX::XMFLOAT4 m_specularColor;
	float             m_specularPower;
};
