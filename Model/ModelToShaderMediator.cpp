////////////////////////////////////////////////////////////////////
// Filename:    ModelToShaderMediator.cpp
// Description: this class is a mediator between the ModelClass and
//              the shaders
//
// Created:     06.02.23
////////////////////////////////////////////////////////////////////

#include "ModelToShaderMediator.h"


ModelToShaderMediator::ModelToShaderMediator(ModelClass* pModel, 
	ShaderClass* pShader,
	DataContainerForShadersClass* pDataForShader)
	:
	pModel_(pModel),
	pShader_(pShader)
{
	this->pModel_->SetMediator(this);
	this->pShader_->SetMediator(this);
	this->pDataForShader_ = pDataForShader;
}


// by particular shader name we define which shader we will use for rendering of the model
void ModelToShaderMediator::Render(ID3D11DeviceContext* pDeviceContext,
	std::string shaderName,
	GraphicsComponent* pModel)
{
	bool result = false;


	result = pShader_->Render(pDeviceContext,
		pModel_->GetIndexCount(),
		pModel_->GetWorldMatrix(),
		pModel_->GetTextureArray(),
		pDataForShader_);

	COM_ERROR_IF_FALSE(result, "can't render a model using the shader");

	return;
}