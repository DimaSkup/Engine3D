#pragma once

#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

class GraphicsComponent;


class ModelMediator
{
public:
	virtual void Render(ID3D11DeviceContext* pDeviceContext) = 0;
	virtual void SetRenderingShaderByName(const std::string & shaderName) = 0;
};


class GraphicsComponent
{
protected:
	ModelMediator* pMediator_ = nullptr;

public:

	// returns a pointer to the current related mediator to the model
	virtual ModelMediator* GetMediator() const
	{
		return this->pMediator_;
	}

	// changes the current related mediator to the model to another one
	void SetMediator(ModelMediator* pMediator)
	{
		this->pMediator_ = pMediator;
	}
};