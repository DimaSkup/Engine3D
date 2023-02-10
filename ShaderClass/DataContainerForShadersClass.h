////////////////////////////////////////////////////////////////////
// Filename:    DataContainerForShadersClass.h
// Description: contains different data which used by different 
//              shaders for the rendering process
// 
// Created:     10.02.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>


//////////////////////////////////
// Class name: DataContainerForShadersClass
//////////////////////////////////
class DataContainerForShadersClass
{
public:
	DataContainerForShadersClass();

	void Update(DirectX::XMMATRIX* view, 
		DirectX::XMMATRIX* projection,
		DirectX::XMMATRIX* ortho);

	const DirectX::XMMATRIX & GetViewMatrix() const;
	const DirectX::XMMATRIX & GetProjectionMatrix() const;
	const DirectX::XMMATRIX & GetOrthoMatrix() const;

private:
	DirectX::XMMATRIX* pViewMatrix_ = nullptr;
	DirectX::XMMATRIX* pProjectionMatrix_ = nullptr;
	DirectX::XMMATRIX* pOrthoMatrix_ = nullptr;
};
