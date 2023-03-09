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


class ModelToShaderMediator : public ModelMediator
{
public:
	ModelToShaderMediator(ModelClass* pModel, 
		ShaderClass* pShader,
		DataContainerForShadersClass* pDataForShader);

	~ModelToShaderMediator();

	virtual void Render(ID3D11DeviceContext* pDeviceContext, GraphicsComponent* pModel) override;

private:
	ModelClass* pModel_ = nullptr;
	ShaderClass* pShader_ = nullptr;
	DataContainerForShadersClass* pDataForShader_ = nullptr;
};
