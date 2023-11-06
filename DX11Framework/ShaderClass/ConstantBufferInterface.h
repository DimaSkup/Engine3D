////////////////////////////////////////////////////////////////////
// Filename:     ConstantBufferInterface.h
// Description:  an interface for shaders' constant buffers functional
//
// Created:      04.09.23
////////////////////////////////////////////////////////////////////
#pragma once



//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"

#include <DirectXMath.h>


//////////////////////////////////
// Class name: ConstantBufferInterface
//////////////////////////////////
template<class T>
class ConstantBufferInterface
{
public:
	virtual ~ConstantBufferInterface() {};

	// initialize a constant buffer
	virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) { return S_FALSE; }

	virtual bool ApplyChanges() { return false; }          // update the constant buffer data

	T data;                                      // here is placed data for a HLSL constant buffer

	// GETTERS
	virtual ID3D11Buffer* Get() const { return nullptr; };                 // return a pointer to the constant buffer
	virtual ID3D11Buffer* const* GetAddressOf() const { return nullptr; }; // return a double pointer to the constant buffer
};
