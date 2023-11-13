#include "VertexBufferInterface.h"

VertexBufferInterface::VertexBufferInterface(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	this->pDevice_ = pDevice;
	this->pDeviceContext_ = pDeviceContext;
}