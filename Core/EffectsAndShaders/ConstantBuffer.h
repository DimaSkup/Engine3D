// *********************************************************************************
// Filename:     ConstantBuffer.h
// Description:  this class is needed for easier using of 
//               constant buffers for HLSL shaders;
// *********************************************************************************
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>

#include "../Engine/Log.h"



//////////////////////////////////
// Class name: ConstantBuffer
//////////////////////////////////
template<class T>
class ConstantBuffer final
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	// initialize a constant buffer
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext); 

	void ApplyChanges(ID3D11DeviceContext* pDeviceContext);  // update the constant buffer data

	ID3D11Buffer* Get() const;                               // return a pointer to the constant buffer
	ID3D11Buffer* const* GetAddressOf() const;               // return a double pointer to the constant buffer

	T data;                                                  // here is placed data for a HLSL constant buffer

private:  // restrict a copying of this class instance
	ConstantBuffer(const ConstantBuffer<T> & rhs);
	ConstantBuffer & operator=(const ConstantBuffer & obj);

private:
	ID3D11Buffer* pBuffer_ = nullptr;
};

// *********************************************************************************

template<class T>
ConstantBuffer<T>::ConstantBuffer()
{
}

template<class T>
ConstantBuffer<T>::~ConstantBuffer() 
{ 
	_RELEASE(pBuffer_);
}

template<class T>
ID3D11Buffer* ConstantBuffer<T>::Get() const
{
	// return a pointer to the constant buffer
	return pBuffer_;
}

template<class T>
ID3D11Buffer* const* ConstantBuffer<T>::GetAddressOf() const
{
	// return a double pointer to the constant buffer
	return &pBuffer_;
}

// *********************************************************************************


template<class T>
HRESULT ConstantBuffer<T>::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	//
	// initialize a constant buffer
	//

	D3D11_BUFFER_DESC desc;
	HRESULT hr = S_OK;

	// if the constant buffer has already been initialized before
	if (pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	desc.ByteWidth           = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
	desc.Usage               = D3D11_USAGE_DYNAMIC;
	desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags           = 0;
	desc.StructureByteStride = 0;

	hr = pDevice->CreateBuffer(&desc, 0, &pBuffer_);
	ASSERT_NOT_FAILED(hr, "can't create a constant buffer");

	return hr;
}

template<class T>
void ConstantBuffer<T>::ApplyChanges(ID3D11DeviceContext* pDeviceContext)
{
	//
	// update the constant buffer data
	//

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HRESULT hr = pDeviceContext->Map(pBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ASSERT_NOT_FAILED(hr, "failed to Map the constant buffer");
	
	CopyMemory(mappedResource.pData, &data, sizeof(T));
	pDeviceContext->Unmap(pBuffer_, 0);
}