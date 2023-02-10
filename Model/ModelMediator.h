#pragma once

#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

class GraphicsComponent;


class ModelMediator
{
public:
	virtual void Render(ID3D11DeviceContext* pDeviceContext,
		std::string shaderName,
		GraphicsComponent* pModel) = 0;
};


class GraphicsComponent
{
protected:
	ModelMediator* pMediator_ = nullptr;

public:
	void SetMediator(ModelMediator* pMediator)
	{
		this->pMediator_ = pMediator;
	}
};