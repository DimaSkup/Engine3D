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
#include "../Model/Model.h"
#include "../ShaderClass/shaderclass.h"
#include "../ShaderClass/DataContainerForShadersClass.h"
#include "../ShaderClass/ShadersContainer.h"


class ModelToShaderMediator : public ModelMediator
{
public:
	ModelToShaderMediator(Model* pModel,           // this constructor takes a pointer to the model, a SHADER NAME, and a pointer to the shaders data
		const char* shaderName,
		DataContainerForShadersClass* pDataForShader);

	ModelToShaderMediator(Model* pModel,           // this constructor takes a pointer to the model, a POINTER TO THE SHADER, and a pointer to the shaders data
		ShaderClass* pShader,
		DataContainerForShadersClass* pDataForShader);

	~ModelToShaderMediator();

	// renders a model using a shader
	virtual void Render(ID3D11DeviceContext* pDeviceContext) override;

	// set a shader class which will be used for rendering the model
	virtual void SetRenderingShaderByName(const std::string & shaderName) override;

private:
	Model* pModel_ = nullptr;         // a pointer to some model object
	ShaderClass* pShader_ = nullptr;  // a pointer to some shader which will be used for rendering the model
	DataContainerForShadersClass* pDataForShader_ = nullptr;  // each shader need some particular data for rendering the model (for instance: light sources)
};
