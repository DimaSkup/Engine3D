////////////////////////////////////////////////////////////////////
// Filename:    ModelToShaderMediator.cpp
// Description: this class is a mediator between the ModelClass and
//              the shaders
//
// Created:     06.02.23
////////////////////////////////////////////////////////////////////

#include "ModelToShaderMediator.h"

// this constructor takes a pointer to the model,
// a SHADER NAME, and a pointer to the shaders data
ModelToShaderMediator::ModelToShaderMediator(Model* pModel,
	const char* shaderName,
	DataContainerForShadersClass* pDataForShader)
	:
	pModel_(pModel),
	pDataForShader_(pDataForShader)
{
	// find a shader by its name and set it as related
	this->SetRenderingShaderByName(shaderName);
}


// this constructor takes a pointer to the model, 
// a POINTER TO THE SHADER, and a pointer to the shaders data
ModelToShaderMediator::ModelToShaderMediator(Model* pModel,
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

ModelToShaderMediator::~ModelToShaderMediator()
{
	this->pModel_ = nullptr;
	this->pShader_ = nullptr;
	this->pDataForShader_ = nullptr;
}


// renders a model using a shader
void ModelToShaderMediator::Render(ID3D11DeviceContext* pDeviceContext)
{
	assert(pDeviceContext != nullptr);

	bool result = false;

	// execute rendering of the model
	result = pShader_->Render(pDeviceContext,
		pModel_->GetIndexCount(),
		pModel_->GetWorldMatrix(),
		pModel_->GetTextureResourcesArray(),
		pDataForShader_);

	COM_ERROR_IF_FALSE(result, "can't render a model using the shader");

	return;
}


// set a shader class which will be used for rendering the model
void ModelToShaderMediator::SetRenderingShaderByName(const std::string & shaderName)
{
	assert(shaderName.empty() != true);

	this->pShader_ = ShadersContainer::Get()->GetShaderByName(shaderName);

	return;
}