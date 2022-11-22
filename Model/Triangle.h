#pragma once


#include "modelclass.h"
#include <vector>

class Triangle : public ModelClass
{
public:
	bool Initialize(ID3D11Device* pDevice, string modelName, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });

private:
	std::vector<VERTEX> triangle;
};