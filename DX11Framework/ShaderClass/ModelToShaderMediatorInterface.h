#pragma once

#include <d3d11.h>
#include "DataContainerForShaders.h"


class ModelToShaderComponent;


// the Mediator interface declares a method used by components to notify the mediator
// that the component (model) wants to be rendered onto the screen;
//
// the Mediator may react to these notifications and pass the execution to 
// other components;
class ModelToShaderMediatorInterface
{
public:
	virtual void Render(ID3D11DeviceContext* pDeviceContext,
		ModelToShaderComponent* pModel) = 0;

	inline DataContainerForShaders* GetDataContainerForShaders() const
	{
		return pDataContainerForShaders_;
	}

protected:
	// a data container which will be used by different shaders to get necessary data for rendering
	DataContainerForShaders*  pDataContainerForShaders_ = nullptr; 
};


// the ModelToShaderComponent (Base Component) provides the basic functionality of storing a mediator's instance
// inside component objects.
class ModelToShaderComponent
{
public:
	ModelToShaderComponent(ModelToShaderMediatorInterface* pMediator = nullptr)
		: pModelToShaderMediator_(pMediator)
	{
	}


	///////////////////////////////////////////////////////////

	void SetModelToShaderMediator(ModelToShaderMediatorInterface* pMediator)
	{
		assert(pMediator != nullptr);
		this->pModelToShaderMediator_ = pMediator;
	}

	virtual void SetRenderShaderName(const std::string & shaderName)
	{
		assert(shaderName.empty() != true);
		renderShader_ = shaderName;
	}
	
	inline const std::string & GetRenderShaderName() const
	{
		return renderShader_;
	}

	inline DataContainerForShaders* GetDataContainerForShaders() const
	{
		return this->pModelToShaderMediator_->GetDataContainerForShaders();
	}

protected:
	// a pointer to the mediator which will be used to call a rendering 
	// function of some chosen shader
	ModelToShaderMediatorInterface* pModelToShaderMediator_ = nullptr;

	// name of the shader which will be used for rendering this model
	std::string renderShader_{ "no_shader_name" }; 
};