#pragma once

#include <string>

class GraphicsComponent;


class ModelMediator
{
public:
	virtual void Render(std::string shaderName, GraphicsComponent* pModel) = 0;
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