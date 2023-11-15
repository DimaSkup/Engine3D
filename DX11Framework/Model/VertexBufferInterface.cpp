///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     VertexBufferInterface.cpp
// Description:  this file contains implementation for some common functions
//
// Revising:     13.12.22
///////////////////////////////////////////////////////////////////////////////////////////////
#include "VertexBufferInterface.h"



VertexBufferInterface::VertexBufferInterface(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	// check input params
	COM_ERROR_IF_FALSE(pDevice, "pDevice == nullptr");
	COM_ERROR_IF_FALSE(pDeviceContext, "pDeviceContext == nullptr");

	this->pDevice_ = pDevice;
	this->pDeviceContext_ = pDeviceContext;
}

///////////////////////////////////////////////////////////

VertexBufferInterface::VertexBufferInterface(const VertexBufferInterface & anotherBuffer)
{
	// copy the origin buffer into the current one ( using the virtual operator= )
	*this = anotherBuffer;
}

///////////////////////////////////////////////////////////

VertexBufferInterface::~VertexBufferInterface()
{
	_RELEASE(pBuffer_);

	pDevice_ = nullptr;
	pDeviceContext_ = nullptr;
}