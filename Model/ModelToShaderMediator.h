////////////////////////////////////////////////////////////////////
// Filename:    ModelToShaderMediator.h
// Description: this class is a mediator between the ModelClass and
//              the shaders
//
// Created:     06.02.23
////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

#include "../Engine/log.h"

#include "../Model/ModelMediator.h"
#include "../Model/ModelClass.h"
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/DataContainerForShadersClass.h"
#include "../ShaderClass/ShadersContainer.h"


class ModelToShaderMediator : public ModelMediator
{
public:
	ModelToShaderMediator(ModelClass* pModel, 
		ShaderClass* pShader,
		DataContainerForShadersClass* pDataForShader);

	~ModelToShaderMediator();

	// renders a model using a shader
	virtual void Render(ID3D11DeviceContext* pDeviceContext) override;

	// set a shader class which will be used for rendering the model
	virtual void SetRenderingShaderByName(const std::string & shaderName) override;

private:
	ModelClass* pModel_ = nullptr;    // some model object
	ShaderClass* pShader_ = nullptr;  // some shader which will be used for rendering the model
	DataContainerForShadersClass* pDataForShader_ = nullptr;  // each shader need some particular data for rendering the model (for instance: light sources)
};
