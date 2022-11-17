#pragma once

#include "modelclass.h"

class Model2D : public ModelClass
{
public:
	Model2D() {};

	bool Initialize(ID3D11Device* pDevice, VERTEX_2D* vertices); // initialize a model using only its vertices data

	// memory allocation (we need it because we use DirectX::XM-objects)
	void* operator new(size_t i);
	void operator delete(void* p);

private:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void ReleaseModel() { return; }
};